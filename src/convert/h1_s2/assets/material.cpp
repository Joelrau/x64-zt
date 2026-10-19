#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"
#include "gfximage.hpp"
#include "techset.hpp"

namespace zonetool::h1::converter::s2::material
{
	namespace
	{
		struct converted_material
		{
			zonetool::s2::Material material;
			std::vector<zonetool::s2::MaterialTextureDef> textures;
			std::vector<zonetool::s2::GfxImage> images;
			std::vector<std::string> image_names;
			std::vector<zonetool::s2::MaterialConstantDef> constants;
			std::vector<zonetool::s2::GfxStateBits> state_bits;
			std::vector<zonetool::s2::material_state> states;
			std::vector<zonetool::s2::MaterialConstantBufferDef> constant_buffers;
		};

		std::vector<Material*> pending_materials;
		std::unordered_set<std::string> dumped_donor_images;

		const MaterialTextureDef* find_texture(const Material* asset, const std::uint32_t hash)
		{
			for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
			{
				if (asset->textureTable[i].nameHash == hash && asset->textureTable[i].semantic != TS_WATER_MAP)
				{
					return &asset->textureTable[i];
				}
			}

			return nullptr;
		}

		std::string fallback_image(const std::uint8_t semantic)
		{
			switch (semantic)
			{
			case zonetool::s2::TS_COLOR_MAP:
				return gfximage::dump_solid("h1_s2_white", zonetool::s2::TS_COLOR_MAP, {255, 255, 255, 255});
			case zonetool::s2::TS_NORMAL_MAP:
				return "$identitynormalmap";
			case zonetool::s2::TS_SPECULAR_MAP:
				return gfximage::dump_solid("h1_s2_specular", zonetool::s2::TS_SPECULAR_MAP, {49, 255, 128, 255});
			default:
				return "$black";
			}
		}

		std::string select_image(const Material* asset, const zonetool::s2::MaterialTextureDef& donor_texture)
		{
			const auto& zone = techset::donor_zone();
			const auto source_texture = donor_texture.semantic != zonetool::s2::TS_ENV_BRDF_LUT
				? find_texture(asset, donor_texture.nameHash)
				: nullptr;
			const auto source_image = source_texture && source_texture->u.image ? source_texture->u.image->name : nullptr;

			if (source_image && donor_texture.semantic == zonetool::s2::TS_COLOR_MAP && source_texture->u.image->mapType == MAPTYPE_CUBE)
			{
				if (const auto sky = gfximage::dump_sky(source_texture->u.image))
				{
					return *sky;
				}
			}

			if (const auto image = source_image ? gfximage::dump_used(source_image) : std::nullopt)
			{
				return *image;
			}

			if (source_image && source_image[0] != '$')
			{
				ZONETOOL_WARNING("Image \"%s\" of material \"%s\" is not in a loaded zone", source_image, asset->name);
			}

			const auto donor_image = donor_texture.u.image;
			if (donor_image && donor_image->name)
			{
				const auto is_core_semantic = donor_texture.semantic == zonetool::s2::TS_COLOR_MAP
					|| donor_texture.semantic == zonetool::s2::TS_NORMAL_MAP
					|| donor_texture.semantic == zonetool::s2::TS_SPECULAR_MAP;
				if (zone.is_global_image(donor_image->name))
				{
					return donor_image->name;
				}

				if (!is_core_semantic)
				{
					if (dumped_donor_images.insert(donor_image->name).second)
					{
						db_scope _(zone.db());
						zonetool::s2::gfx_image::dump(donor_image);
						add_csv_row("image", donor_image->name);
					}

					return donor_image->name;
				}
			}

			return fallback_image(donor_texture.semantic);
		}

		std::unique_ptr<converted_material> convert(const Material* asset, const zonetool::s2::Material* donor)
		{
			auto result = std::make_unique<converted_material>();
			auto& material = result->material;
			material = *donor;

			material.info.name = asset->name;
			material.info.surfaceTypeBits = convert_surface_type_bits(static_cast<std::uint64_t>(asset->info.surfaceTypeBits));
			material.parentMaterial = nullptr;

			result->images.resize(donor->textureCount);
			result->image_names.resize(donor->textureCount);
			result->textures.assign(donor->textureTable, donor->textureTable + donor->textureCount);
			for (auto i = 0u; i < result->textures.size(); i++)
			{
				auto& texture = result->textures[i];
				result->image_names[i] = select_image(asset, texture);
				result->images[i].name = result->image_names[i].data();
				texture.u.image = &result->images[i];
			}

			const auto is_sky = std::ranges::any_of(result->image_names, [](const std::string& name)
			{
				return name.ends_with("_s2sky");
			});
			for (auto i = 0u; is_sky && i < result->textures.size(); i++)
			{
				const auto donor_image = donor->textureTable[i].u.image;
				if (donor_image && donor_image->name && std::string_view{donor_image->name}.ends_with("_flow"))
				{
					result->image_names[i] = gfximage::dump_solid("h1_s2_sky_still_flow", result->textures[i].semantic, {128, 128, 0, 255});
					result->images[i].name = result->image_names[i].data();
				}
			}

			material.textureTable = result->textures.data();
			return result;
		}

		constexpr std::uint8_t no_entry = 0xFF;
		constexpr std::uint32_t h1_polygon_offset_shift = 4;
		constexpr std::uint32_t h1_polygon_offset_mask = 3u << h1_polygon_offset_shift;
		constexpr std::uint32_t s2_polygon_offset_shift = 27;
		constexpr std::uint32_t blend_class_shift = 26;
		constexpr std::uint32_t blend_word_count = 3;

		std::uint32_t convert_blend_word(const std::uint32_t h1_word)
		{
			return (h1_word & 0x7FF) | ((h1_word & 0x7FF000) >> 1) | ((h1_word & 0xF000000) >> 2);
		}

		std::uint32_t blend_class(const std::int32_t slot)
		{
			const auto type = slot % zonetool::s2::TECHNIQUE_GROUP_SIZE;
			const auto is_lit = type >= zonetool::s2::TECHNIQUE_EMISSIVE && type < zonetool::s2::TECHNIQUE_LIT_END;
			return type == zonetool::s2::TECHNIQUE_UNLIT || is_lit || type == zonetool::s2::TECHNIQUE_WIREFRAME_SOLID ? 1 : 0;
		}

		std::uint8_t rasterizer_index(const std::uint32_t load_bits0, const std::uint32_t load_bits2)
		{
			return static_cast<std::uint8_t>(((load_bits0 >> 3) & 3) | ((load_bits2 >> 25) & 0x1C) | (load_bits0 & 0x20)); // sub_1407FB3E0
		}

		zonetool::s2::GfxStateBits convert_state_bits(const GfxStateBits& source, const std::uint32_t blend_class)
		{
			const auto polygon_offset = (source.loadBits[1] & h1_polygon_offset_mask) >> h1_polygon_offset_shift;

			zonetool::s2::GfxStateBits result{};
			result.loadBits[0] = source.loadBits[0];
			result.loadBits[1] = source.loadBits[1] & ~h1_polygon_offset_mask;
			result.loadBits[2] = source.loadBits[2] | (polygon_offset << s2_polygon_offset_shift);
			for (auto i = 0u; i < blend_word_count; i++)
			{
				result.loadBits[3 + i] = convert_blend_word(source.loadBits[3 + i]) | (blend_class << blend_class_shift);
			}
			result.rasterizerState = rasterizer_index(result.loadBits[0], result.loadBits[2]);
			return result;
		}

		constexpr auto depth_stencil_mode_count = std::extent_v<decltype(zonetool::s2::material_state::depth_stencil)>;

		std::array<std::uint64_t, depth_stencil_mode_count> depth_stencil_states(const std::uint32_t load_bits1, const std::uint32_t load_bits2)
		{
			constexpr std::uint32_t depth_write = 1;
			constexpr std::uint32_t front_stencil_func = 0x000E0000;
			constexpr std::uint32_t back_stencil_func = 0xE0000000;
			constexpr std::uint32_t default_stencil_masks = 0xFFFF;
			constexpr std::array<std::uint32_t, depth_stencil_mode_count> default_masks
			{
				0x0909, 0x1010, 0x0404, 0x0808, 0x0902, 0x0912, 0x0303, 0x0707, 0x1313, 0x1717, 0x0909, 0x1010,
			};

			const auto front_func = load_bits1 & GFXS1_STENCIL_FRONT_ENABLE ? load_bits1 & front_stencil_func : front_stencil_func;
			const auto back_func = load_bits1 & GFXS1_STENCIL_BACK_ENABLE
				? load_bits1 & back_stencil_func
				: front_func << (GFXS1_STENCIL_BACK_FUNC_SHIFT - GFXS1_STENCIL_FRONT_FUNC_SHIFT); // sub_140827CB0 case 2
			const auto stencil_states = load_bits1 & 0xFFFFFF00 ? load_bits1 | depth_write : load_bits1 | 0xE02E0241;
			const std::array<std::uint32_t, depth_stencil_mode_count> states
			{
				load_bits1,
				(load_bits1 & 0xBF) | 0xE02E0240,
				(load_bits1 & 0xBF) | front_func | back_func | 0x200240,
				load_bits1,
				(load_bits1 & ~depth_write) | 0x400400C0,
				(load_bits1 & ~depth_write) | 0x400400C0,
				stencil_states,
				stencil_states,
				stencil_states,
				stencil_states,
				load_bits1 | depth_write,
				(load_bits1 & 0xBE) | 0xE02E0241,
			};

			const auto keeps_stencil_masks = load_bits2 != default_stencil_masks;
			std::array<std::uint64_t, depth_stencil_mode_count> result{};
			for (auto mode = 0u; mode < depth_stencil_mode_count; mode++)
			{
				const auto uses_stencil_masks = mode == 0 || (mode >= 6 && mode <= 9);
				const auto masks = keeps_stencil_masks && uses_stencil_masks ? load_bits2 & default_stencil_masks : default_masks[mode];
				result[mode] = (static_cast<std::uint64_t>(masks) << 32) | states[mode];
			}

			return result;
		}

		zonetool::s2::material_state convert_state(const zonetool::s2::GfxStateBits& state_bits)
		{
			zonetool::s2::material_state result{};
			const auto depth_stencil = depth_stencil_states(state_bits.loadBits[1], state_bits.loadBits[2]);
			std::ranges::copy(depth_stencil, result.depth_stencil);
			for (auto i = 0u; i < blend_word_count; i++)
			{
				result.blend[i] = state_bits.loadBits[3 + i] & ~(7u << blend_class_shift);
			}

			return result;
		}

		std::string select_native_image(const MaterialTextureDef& texture)
		{
			if (texture.u.image && texture.u.image->name)
			{
				auto* image = zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_IMAGE, texture.u.image->name).image;
				if (const auto name = image ? gfximage::dump_native(image) : std::nullopt)
				{
					return *name;
				}
			}

			if (texture.semantic == TS_SPECULAR_MAP)
			{
				return gfximage::dump_solid("h1_s2_native_specular", zonetool::s2::TS_SPECULAR_MAP, {32, 32, 32, 128});
			}

			return fallback_image(texture.semantic);
		}

		zonetool::s2::MaterialConstantBufferDef convert_constant_buffer(const MaterialConstantBufferDef& source)
		{
			zonetool::s2::MaterialConstantBufferDef result{};
			result.vsDataSize = source.vsDataSize;
			result.hsDataSize = source.hsDataSize;
			result.dsDataSize = source.dsDataSize;
			result.psDataSize = source.psDataSize;
			result.vsOffsetDataSize = source.vsOffsetDataSize;
			result.hsOffsetDataSize = source.hsOffsetDataSize;
			result.dsOffsetDataSize = source.dsOffsetDataSize;
			result.psOffsetDataSize = source.psOffsetDataSize;
			result.vsData = source.vsData;
			result.hsData = source.hsData;
			result.dsData = source.dsData;
			result.psData = source.psData;
			result.vsOffsetData = source.vsOffsetData;
			result.hsOffsetData = source.hsOffsetData;
			result.dsOffsetData = source.dsOffsetData;
			result.psOffsetData = source.psOffsetData;
			return result;
		}

		void convert_native_textures(const Material* asset, converted_material& result)
		{
			const auto textures = std::span{asset->textureTable, asset->textureTable ? asset->textureCount : 0u};
			if (std::ranges::any_of(textures, [](const MaterialTextureDef& texture) { return texture.semantic == TS_WATER_MAP; }))
			{
				throw std::runtime_error("water textures are not converted");
			}

			result.textures.resize(textures.size());
			result.images.resize(textures.size());
			result.image_names.resize(textures.size());
			for (auto i = 0u; i < textures.size(); i++)
			{
				const auto& source = textures[i];
				auto& texture = result.textures[i];
				texture.nameHash = source.nameHash;
				texture.nameStart = source.nameStart;
				texture.nameEnd = source.nameEnd;
				texture.samplerState = source.samplerState;
				texture.semantic = source.semantic;

				result.image_names[i] = select_native_image(source);
				result.images[i].name = result.image_names[i].data();
				texture.u.image = &result.images[i];
			}

			result.material.textureCount = static_cast<unsigned char>(textures.size());
			result.material.textureTable = result.textures.empty() ? nullptr : result.textures.data();
		}

		void convert_native_constants(const Material* asset, converted_material& result)
		{
			result.constants.resize(asset->constantTable ? asset->constantCount : 0);
			for (auto i = 0u; i < result.constants.size(); i++)
			{
				const auto& source = asset->constantTable[i];
				auto& constant = result.constants[i];
				constant.nameHash = source.nameHash;
				std::ranges::copy(source.name, constant.name);
				std::ranges::copy(source.literal, constant.literal);
			}

			result.material.constantCount = static_cast<unsigned char>(result.constants.size());
			result.material.constantTable = result.constants.empty() ? nullptr : result.constants.data();

			result.constant_buffers.resize(asset->constantBufferTable ? asset->constantBufferCount : 0);
			for (auto i = 0u; i < result.constant_buffers.size(); i++)
			{
				result.constant_buffers[i] = convert_constant_buffer(asset->constantBufferTable[i]);
			}

			result.material.constantBufferCount = static_cast<unsigned char>(result.constant_buffers.size());
			result.material.constantBufferTable = result.constant_buffers.empty() ? nullptr : result.constant_buffers.data();
		}

		void convert_native_states(const Material* asset, const techset::native_techset& native, converted_material& result)
		{
			auto& material = result.material;
			std::ranges::fill(material.stateBitsEntry, no_entry);
			std::ranges::fill(material.constantBufferIndex, no_entry);

			std::map<std::pair<std::uint8_t, std::uint32_t>, std::uint8_t> entries;
			for (auto slot = 0; slot < zonetool::s2::technique_count; slot++)
			{
				const auto source = native.sources[slot];
				if (source < 0)
				{
					continue;
				}

				material.constantBufferIndex[slot] = asset->constantBufferIndex[source];

				const auto h1_entry = asset->stateBitsEntry[source];
				if (h1_entry == no_entry)
				{
					continue;
				}

				if (!asset->stateBitsTable || h1_entry >= asset->stateBitsCount)
				{
					throw std::runtime_error(std::format("technique {} has no state bits", source));
				}

				const auto key = std::make_pair(h1_entry, blend_class(slot));
				const auto [entry, inserted] = entries.try_emplace(key, static_cast<std::uint8_t>(result.state_bits.size()));
				if (inserted)
				{
					result.state_bits.emplace_back(convert_state_bits(asset->stateBitsTable[h1_entry], key.second));
					result.states.emplace_back(convert_state(result.state_bits.back()));
				}

				material.stateBitsEntry[slot] = entry->second;
			}

			if (result.state_bits.size() >= no_entry)
			{
				throw std::runtime_error("too many state bits");
			}

			material.stateBitsCount = static_cast<unsigned char>(result.state_bits.size());
			material.stateBitsTable = result.state_bits.empty() ? nullptr : result.state_bits.data();
		}

		std::uint8_t convert_state_flags(const std::uint8_t h1_flags)
		{
			constexpr std::array<std::pair<std::uint8_t, std::uint8_t>, 5> flag_map
			{{
				{STATE_FLAG_CULL_BACK, zonetool::s2::STATE_FLAG_CULL_BACK},
				{STATE_FLAG_DECAL, zonetool::s2::STATE_FLAG_DECAL},
				{STATE_FLAG_WRITES_DEPTH, zonetool::s2::STATE_FLAG_WRITES_DEPTH},
				{STATE_FLAG_USES_DEPTH_BUFFER, zonetool::s2::STATE_FLAG_USES_DEPTH_BUFFER},
				{STATE_FLAG_USES_STENCIL_BUFFER, zonetool::s2::STATE_FLAG_USES_STENCIL_BUFFER},
			}};

			std::uint8_t result = 0;
			for (const auto& [h1_flag, s2_flag] : flag_map)
			{
				if (h1_flags & h1_flag)
				{
					result |= s2_flag;
				}
			}

			return result;
		}

		std::uint8_t convert_sort_key(const std::uint8_t sort_key)
		{
			constexpr auto h1_opaque = 2;
			constexpr auto h1_sky = 3;
			constexpr auto h1_decal_end = 17;
			constexpr auto h1_trans_end = 36;
			constexpr auto h1_2d = 60;
			constexpr auto s2_opaque = 4;
			constexpr auto s2_sky = 6;
			constexpr auto s2_trans_begin = 22;
			constexpr auto max_sort_key = 63;

			switch (sort_key)
			{
			case h1_opaque:
				return s2_opaque;
			case h1_sky:
				return s2_sky;
			default:
				break;
			}

			if (sort_key <= h1_decal_end)
			{
				return static_cast<std::uint8_t>(sort_key + 3); // s2 decals are 10 to 20
			}

			if (sort_key <= h1_trans_end)
			{
				return static_cast<std::uint8_t>(std::max(+sort_key, s2_trans_begin)); // keys below 22 are s2 decals
			}

			if (sort_key >= h1_2d)
			{
				return static_cast<std::uint8_t>(std::min(sort_key + 2, max_sort_key));
			}

			return static_cast<std::uint8_t>(sort_key + 1); // shadow caster 38 to 39, effect auto 53 to 54
		}

		std::uint8_t convert_camera_region(const std::uint8_t camera_region)
		{
			switch (camera_region)
			{
			case CAMERA_REGION_LIT_OPAQUE:
			case CAMERA_REGION_DEPTH_HACK: // s2 stock zones use no depth hack regions
			case CAMERA_REGION_PRE_DEPTH_HACK:
			case CAMERA_REGION_PRE_OPAQUE:
				return zonetool::s2::CAMERA_REGION_LIT_OPAQUE;
			case CAMERA_REGION_LIT_OPAQUE_SSS:
			case CAMERA_REGION_DEPTH_HACK_SSS:
			case CAMERA_REGION_PRE_DEPTH_HACK_SSS:
			case CAMERA_REGION_PRE_SSS:
				return zonetool::s2::CAMERA_REGION_LIT_OPAQUE_SSS;
			case CAMERA_REGION_LIT_DECAL:
				return zonetool::s2::CAMERA_REGION_LIT_DECAL;
			case CAMERA_REGION_LIT_TRANS:
			case CAMERA_REGION_DEPTH_HACK_TRANS:
				return zonetool::s2::CAMERA_REGION_LIT_TRANS;
			case CAMERA_REGION_EMISSIVE:
				return zonetool::s2::CAMERA_REGION_EMISSIVE;
			default:
				return zonetool::s2::CAMERA_REGION_NONE;
			}
		}

		std::unique_ptr<converted_material> convert_native(const Material* asset, const zonetool::s2::Material* donor, const techset::native_techset& native)
		{
			auto result = std::make_unique<converted_material>();
			auto& material = result->material;
			material = *donor;

			material.info.name = asset->name;
			material.info.gameFlags = asset->info.gameFlags;
			material.info.sortKey = convert_sort_key(asset->info.sortKey);
			material.info.drawSurf.fields.primarySortKey = material.info.sortKey;
			material.cameraRegion = convert_camera_region(asset->cameraRegion);
			material.info.textureAtlasRowCount = asset->info.textureAtlasRowCount;
			material.info.textureAtlasColumnCount = asset->info.textureAtlasColumnCount;
			material.info.textureAtlasFrameBlend = asset->info.textureAtlasFrameBlend;
			material.info.textureAtlasAsArray = asset->info.textureAtlasAsArray;
			material.info.surfaceTypeBits = convert_surface_type_bits(static_cast<std::uint64_t>(asset->info.surfaceTypeBits));
			material.stateFlags = convert_state_flags(asset->stateFlags);
			material.techniqueSet = native.techset;
			material.parentMaterial = nullptr;
			material.layerCount = asset->subMaterials ? asset->layerCount : 0;
			material.subMaterials = asset->subMaterials;

			convert_native_states(asset, native, *result);
			convert_native_constants(asset, *result);
			convert_native_textures(asset, *result);
			return result;
		}

		std::unique_ptr<converted_material> try_convert_native(const Material* asset, const zonetool::s2::Material* donor, const std::string& h1_techset)
		{
			const auto native = techset::convert_native(h1_techset);
			if (!native)
			{
				return {};
			}

			try
			{
				return convert_native(asset, donor, *native);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_WARNING("Material \"%s\" needs a donor: %s", asset->name, e.what());
				return {};
			}
		}

		std::string get_techset_name(const Material* asset)
		{
			return asset->techniqueSet && asset->techniqueSet->name ? asset->techniqueSet->name : "";
		}

		void add_image_rows(const converted_material& converted)
		{
			for (const auto& image : converted.images)
			{
				if (image.name && !gfximage::is_dumped(image.name) && !dumped_donor_images.contains(image.name))
				{
					add_csv_row("image", image.name, true);
				}
			}
		}

		void convert_material(Material* asset, std::unordered_map<std::string, std::string>& selected)
		{
			const auto h1_techset = get_techset_name(asset);
			if (h1_techset.empty())
			{
				ZONETOOL_WARNING("Material \"%s\" has no techset", asset->name);
				return;
			}

			auto [entry, inserted] = selected.try_emplace(h1_techset);
			if (inserted)
			{
				entry->second = techset::select(h1_techset);
				ZONETOOL_INFO("techset %s -> %s", h1_techset.data(), entry->second.data());
			}

			const auto& s2_techset = entry->second;
			const auto& zone = techset::donor_zone();
			const auto donor = zone.find_donor(s2_techset);
			if (!donor)
			{
				ZONETOOL_WARNING("No S2 material uses techset \"%s\"", s2_techset.data());
				return;
			}

			if (const auto converted = try_convert_native(asset, donor, h1_techset))
			{
				const auto native = converted->material.techniqueSet;
				techset::dump(native);
				add_csv_row("techset", native->name);
				zonetool::s2::material::dump(&converted->material, converted->states.data());
				add_csv_row("material", asset->name);
				add_image_rows(*converted);
				return;
			}

			const auto global = zone.is_global_techset(s2_techset);
			if (!global)
			{
				if (const auto techniques = zone.find_techset(s2_techset))
				{
					db_scope _(zone.db());
					techset::dump(techniques);
				}
			}
			add_csv_row("techset", s2_techset, global);

			const auto converted = convert(asset, donor);
			{
				db_scope _(zone.db());
				zonetool::s2::material::dump(&converted->material);
			}
			add_csv_row("material", asset->name);
			add_image_rows(*converted);
		}
	}

	void dump(Material* asset)
	{
		pending_materials.emplace_back(asset);
	}

	void finish()
	{
		std::unordered_map<std::string, std::string> selected;
		for (const auto asset : pending_materials)
		{
			try
			{
				convert_material(asset, selected);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_ERROR("Failed to convert material \"%s\": %s", asset->name, e.what());
			}
		}

		pending_materials.clear();
	}
}
