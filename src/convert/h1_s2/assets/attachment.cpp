#include <std_include.hpp>

#include "../include.hpp"
#include "attachment.hpp"
#include "sound.hpp"
#include "xmodel.hpp"

namespace zonetool::h1::converter::s2::attachment
{
	namespace
	{
		constexpr std::size_t h1_world_model_count = 2;
		constexpr std::size_t h1_view_model_count = 2;
		constexpr std::size_t h1_reticle_view_model_count = 64;
		constexpr std::size_t s2_world_model_count = 2;
		constexpr std::size_t s2_view_model_count = 12;
		constexpr std::size_t s2_reticle_view_model_count = 255;

		const std::unordered_map<std::string_view, std::vector<std::string_view>> field_renames =
		{
			{"adsSceneBlur", {}}, // s2 has no equivalent
			{"adsSceneBlurStrength", {"adsSceneBlur"}},
			{"inspectAnimTime", {"stateTimers.inspectTime"}},
			{"actionSlotShowAmmo", {"dpadIconShowsAmmo"}},
			{"hasDetonatorEmptyThrow", {"detonatesOnEmptyThrow"}},
			{"hasDetonatorDoubleTap", {"detonatesOnDoubleTap"}},
			{"midDamage", {"mid1Damage"}},
			{"midPlayerDamage", {"mid1PlayerDamage"}},
			{"signatureMidDamage", {"signatureMid1Damage"}},
			{"minDamage", {"minDamage", "mid2Damage"}}, // h1 has one step between mid and min, s2 has two
			{"minPlayerDamage", {"minPlayerDamage", "mid2PlayerDamage"}},
			{"minDamageRange", {"minDamageRange", "mid1DamageRange"}},
		};

		std::unordered_set<std::string> dumped_attachments;

		std::vector<std::uint16_t> convert_field_offsets(const std::string& h1_name)
		{
			std::vector<std::string_view> s2_names{h1_name};
			if (const auto rename = field_renames.find(h1_name); rename != field_renames.end())
			{
				s2_names = rename->second;
			}

			std::vector<std::uint16_t> offsets;
			for (const auto& s2_name : s2_names)
			{
				if (const auto offset = zonetool::s2::weapon_field_offset(std::string(s2_name)))
				{
					offsets.emplace_back(*offset);
				}
			}

			return offsets;
		}

		bool is_string_field(const WAField& field)
		{
			return field.type < WAFIELD_TYPE_INT || field.type > WAFIELD_TYPE_FLOAT32;
		}

		zonetool::s2::AttachmentFieldValue convert_set_value(const WAField& field)
		{
			zonetool::s2::AttachmentFieldValue value{};
			switch (field.type)
			{
			case WAFIELD_TYPE_INT:
				value.intValue = static_cast<int>(field.parm.p_float); // h1 stores int fields as a float
				break;
			case WAFIELD_TYPE_BOOL:
				value.intValue = field.parm.p_bool;
				break;
			case WAFIELD_TYPE_FLOAT32:
				value.intValue = static_cast<int>(std::lround(field.parm.p_float * 1000.0f)); // seconds -> milliseconds
				break;
			default:
				value.floatValue = field.parm.p_float;
				break;
			}

			return value;
		}

		bool convert_field_asset(const WAField& field, const char* attachment_name)
		{
			const auto* name = field.parm.string;
			if (!name || !*name)
			{
				return true;
			}

			switch (field.type)
			{
			case WAFIELD_TYPE_STRING:
				return true;
			case WAFIELD_TYPE_SOUND:
				return sound::dump_used(name);
			case WAFIELD_TYPE_MODEL:
				xmodel::dump_used(name);
				return true;
			default:
				ZONETOOL_WARNING("attachment \"%s\": asset \"%s\" of field type %d is not converted", attachment_name, name, field.type);
				return false;
			}
		}

		std::optional<zonetool::s2::AttachmentField> convert_field(const WAField& field, const char* attachment_name)
		{
			zonetool::s2::AttachmentField result{};
			result.index = field.index;
			result.type = field.type;

			if (is_string_field(field))
			{
				if (field.code != FIELD_OP_STRING_SET || !convert_field_asset(field, attachment_name))
				{
					return std::nullopt;
				}

				result.code = zonetool::s2::WAFIELD_CODE_NONE;
				result.value.string = field.parm.string;
				return result;
			}

			switch (field.code)
			{
			case FIELD_OP_NUMBER_SET:
				result.code = zonetool::s2::WAFIELD_CODE_SET;
				result.value = convert_set_value(field);
				return result;
			case FIELD_OP_NUMBER_MULTIPLY:
				result.code = zonetool::s2::WAFIELD_CODE_MULTIPLY;
				result.value.floatValue = field.parm.p_float;
				return result;
			default:
				return std::nullopt;
			}
		}

		void convert_fields(const WeaponAttachment* asset, zonetool::s2::WeaponAttachment* attachment, utils::memory::allocator& allocator)
		{
			std::vector<std::pair<std::uint16_t, zonetool::s2::AttachmentField>> fields;
			for (auto i = 0u; asset->fields && asset->fieldOffsets && i < asset->numFields; i++)
			{
				const auto& field = asset->fields[i];
				if (field.type == WAFIELD_TYPE_ANIM)
				{
					continue;
				}

				const auto name = weapon_field_name(asset->fieldOffsets[i]);
				const auto offsets = name ? convert_field_offsets(*name) : std::vector<std::uint16_t>{};
				const auto converted = offsets.empty() ? std::nullopt : convert_field(field, asset->name);
				if (!converted)
				{
					ZONETOOL_WARNING("attachment \"%s\": field \"%s\" (code %d) is not converted",
						asset->name, name ? name->data() : "?", field.code);
					continue;
				}

				for (const auto offset : offsets)
				{
					fields.emplace_back(offset, *converted);
				}
			}

			attachment->fieldCount = static_cast<int>(fields.size());
			attachment->fieldOffsets = allocator.allocate_array<unsigned short>(std::max<std::size_t>(fields.size(), 1));
			attachment->fields = allocator.allocate_array<zonetool::s2::AttachmentField>(std::max<std::size_t>(fields.size(), 1));
			for (auto i = 0u; i < fields.size(); i++)
			{
				attachment->fieldOffsets[i] = fields[i].first;
				attachment->fields[i] = fields[i].second;
			}
		}

		zonetool::s2::XModel** convert_models(XModel* const* models, const std::size_t h1_count, const std::size_t s2_count,
			utils::memory::allocator& allocator)
		{
			if (!models)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<zonetool::s2::XModel*>(s2_count);
			for (auto i = 0u; i < h1_count; i++)
			{
				result[i] = xmodel::reference(models[i], allocator);
			}

			return result;
		}
	}

	void dump(WeaponAttachment* asset)
	{
		if (!dumped_attachments.insert(asset->name).second)
		{
			return;
		}

		utils::memory::allocator allocator;
		auto* attachment = allocator.allocate<zonetool::s2::WeaponAttachment>();
		attachment->szInternalName = asset->szInternalName;
		attachment->szDisplayName = asset->szDisplayName;
		attachment->type = asset->type;
		attachment->weaponType = asset->weaponType;
		attachment->weapClass = asset->weapClass;
		attachment->greebleType = asset->greebleType;
		attachment->worldModels = convert_models(asset->worldModels, h1_world_model_count, s2_world_model_count, allocator);
		attachment->viewModels = convert_models(asset->viewModels, h1_view_model_count, s2_view_model_count, allocator);
		attachment->reticleViewModels = convert_models(asset->reticleViewModels, h1_reticle_view_model_count,
			s2_reticle_view_model_count, allocator);
		attachment->hybridSettings = retype<zonetool::s2::AttHybridSettings>(asset->hybridSettings);
		convert_fields(asset, attachment, allocator);

		zonetool::s2::weapon_attachment::dump(attachment);
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_ATTACHMENT), asset->name);
	}

	zonetool::s2::WeaponAttachment* reference(const WeaponAttachment* asset, utils::memory::allocator& allocator)
	{
		if (!asset || !asset->name)
		{
			return nullptr;
		}

		auto* loaded = db_find_x_asset_header_safe(ASSET_TYPE_ATTACHMENT, asset->name).attachment;
		if (!loaded)
		{
			ZONETOOL_WARNING("attachment \"%s\" not found in the loaded h1 zones", asset->name);
			return nullptr;
		}

		dump(loaded);

		auto* stub = allocator.allocate<zonetool::s2::WeaponAttachment>();
		stub->szInternalName = asset->name;
		return stub;
	}

	void clear()
	{
		dumped_attachments.clear();
	}
}
