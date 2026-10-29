#include <std_include.hpp>

#include "../include.hpp"
#include "attachment.hpp"
#include "fxeffectdef.hpp"
#include "material.hpp"
#include "sound.hpp"
#include "tracer.hpp"
#include "weapon.hpp"
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
			{"thermalScope", {}}, // s2 has no thermal scopes
			{"knifeModel", {}}, // s2 weapons have no knife models, tacticalKnife marks the attachment instead
			{"worldKnifeModel", {}},
			{"knifeAttachTagLeft", {}}, // s2 never reads its knife attach tags
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
			{"meleeDamage", {"meleeDamage", "meleeChargeDamage"}}, // the s2 alternate melee damage too, h1 has one damage for every swing
		};

		const std::unordered_set<std::string_view> attachment_level_fields =
		{
			"adsSceneBlur", // s2 reads ads blur only from the attachment's own fields
			"adsSceneBlurStrength",
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
				value.intValue = field.parm.p_float != 0.0f; // h1 stores bool fields as a float too, s2 as an int
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

		std::optional<const char*> convert_field_asset(const WAField& field, const char* attachment_name)
		{
			const auto* name = field.parm.string;
			if (!name || !*name)
			{
				return name;
			}

			switch (field.type)
			{
			case WAFIELD_TYPE_STRING:
				return name;
			case WAFIELD_TYPE_FX:
				return fxeffectdef::dump_used(name) ? std::optional(name) : std::nullopt;
			case WAFIELD_TYPE_MODEL:
				xmodel::dump_used(name);
				return xmodel::converted_name(zonetool::s2::ASSET_TYPE_XMODEL, name);
			case WAFIELD_TYPE_MATERIAL:
				return material::dump_used(name) ? std::optional(name) : std::nullopt;
			case WAFIELD_TYPE_SOUND:
				return sound::dump_used(name) ? std::optional(name) : std::nullopt;
			case WAFIELD_TYPE_TRACER:
			{
				const auto* converted = tracer::dump_used(name);
				return converted ? std::optional(converted) : std::nullopt;
			}
			default:
				ZONETOOL_WARNING("attachment \"%s\": asset \"%s\" of field type %d is not converted", attachment_name, name, field.type);
				return std::nullopt;
			}
		}

		std::optional<zonetool::s2::AttachmentField> convert_field(const WAField& field, const char* attachment_name)
		{
			zonetool::s2::AttachmentField result{};
			result.index = field.index;
			result.type = field.type;

			if (is_string_field(field))
			{
				const auto value = field.code == FIELD_OP_STRING_SET ? convert_field_asset(field, attachment_name) : std::nullopt;
				if (!value)
				{
					return std::nullopt;
				}

				result.code = zonetool::s2::WAFIELD_CODE_NONE;
				result.value.string = *value;
				return result;
			}

			switch (field.code)
			{
			case FIELD_OP_NUMBER_SET:
				result.code = zonetool::s2::WAFIELD_CODE_SET;
				result.value = convert_set_value(field);
				return result;
			case FIELD_OP_NUMBER_ADD_MULTIPLY:
				result.code = zonetool::s2::WAFIELD_CODE_MULTIPLY; // h1 adds value times the base value, a multiply by 1 + value on its own
				result.value.floatValue = 1.0f + field.parm.p_float;
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
				if (name && attachment_level_fields.contains(*name))
				{
					continue;
				}

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

				if (*name == "overlay.shader") // s2 only draws an overlay shader with overlay.enabled set
				{
					zonetool::s2::AttachmentField enabled{};
					enabled.type = zonetool::s2::WAFIELD_TYPE_BOOL;
					enabled.code = zonetool::s2::WAFIELD_CODE_SET;
					enabled.value.intValue = 1;
					fields.emplace_back(*zonetool::s2::weapon_field_offset("overlay.enabled"), enabled);
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

		const WAField* find_field(const WeaponAttachment* asset, const std::string_view field_name)
		{
			for (auto i = 0u; asset->fields && asset->fieldOffsets && i < asset->numFields; i++)
			{
				if (const auto name = weapon_field_name(asset->fieldOffsets[i]); name && *name == field_name)
				{
					return &asset->fields[i];
				}
			}

			return nullptr;
		}

		bool sets_bool_field(const WeaponAttachment* asset, const std::string_view field_name)
		{
			const auto* field = find_field(asset, field_name);
			return field && field->parm.p_float != 0.0f;
		}

		void convert_attachment_level_fields(const WeaponAttachment* asset, zonetool::s2::WeaponAttachment* attachment)
		{
			attachment->adsSceneBlur = sets_bool_field(asset, "adsSceneBlur");
			if (const auto* strength = find_field(asset, "adsSceneBlurStrength"))
			{
				attachment->adsSceneBlurStrength = strength->parm.p_float;
			}

			attachment->tacticalKnife = asset->knifeAlwaysAttached || sets_bool_field(asset, "knifeAlwaysAttached"); // s2 reads the tactical knife from the attachment, h1 from the weapon
		}

		zonetool::s2::AttachmentType convert_attachment_type(const WeaponAttachment* asset)
		{
			switch (asset->type)
			{
			case ATTACHMENT_SCOPE:
				return zonetool::s2::ATTACHMENT_SCOPE;
			case ATTACHMENT_UNDERBARREL:
				return zonetool::s2::ATTACHMENT_UNDERBARREL;
			case ATTACHMENT_OTHER:
				if (sets_bool_field(asset, "silenced"))
				{
					return zonetool::s2::ATTACHMENT_MUZZLE; // s2 files suppressors under muzzle
				}

				return find_field(asset, "clipSize") ? zonetool::s2::ATTACHMENT_MAGAZINE : zonetool::s2::ATTACHMENT_MODELFREE;
			default:
				return zonetool::s2::ATTACHMENT_MODELFREE; // s2 has no side rail
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
		attachment->type = convert_attachment_type(asset);
		attachment->weaponType = weapon::convert_weap_type(asset->weaponType);
		attachment->weapClass = weapon::convert_weap_class(asset->weapClass).value_or(zonetool::s2::WEAPCLASS_RIFLE);
		attachment->greebleType = static_cast<zonetool::s2::weapGreebleType_t>(asset->greebleType);
		attachment->worldModels = convert_models(asset->worldModels, h1_world_model_count, s2_world_model_count, allocator);
		attachment->viewModels = convert_models(asset->viewModels, h1_view_model_count, s2_view_model_count, allocator);
		attachment->reticleViewModels = convert_models(asset->reticleViewModels, h1_reticle_view_model_count,
			s2_reticle_view_model_count, allocator);
		attachment->hybridSettings = retype<zonetool::s2::AttHybridSettings>(asset->hybridSettings);
		attachment->hideIronSightsWithThisAttachment = asset->hideIronSightsWithThisAttachment;
		attachment->showMasterRail = asset->showMasterRail;
		attachment->showSideRail = asset->showSideRail;
		attachment->shareAmmoWithAlt = asset->shareAmmoWithAlt;
		attachment->isAlternateAmmo = asset->isAlternateAmmo;
		attachment->riotShield = asset->riotShield;
		attachment->automaticAttachment = asset->automaticAttachment;
		convert_attachment_level_fields(asset, attachment);
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
