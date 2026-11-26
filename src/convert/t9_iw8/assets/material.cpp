#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"
#include "gfximage.hpp"

#include <iw8/zone/loader.hpp>

#include <formats/dxil.hpp>
#include <io/game_files.hpp>
#include <xfile/oodle.hpp>

namespace zonetool::t9::converter::iw8::material
{
	namespace
	{
		namespace target = zonetool::iw8;

		enum class donor_kind
		{
			opaque,
			emissive,
			alpha_test,
			alpha_test_two_sided,
			decal,
			translucent,
			glass,
		};

		enum class material_usage
		{
			world,
			static_model,
			model,
		};

		const char* get_world_donor_techset(const donor_kind kind) // TODO: stock IW8 techsets, convert natively
		{
			switch (kind)
			{
			case donor_kind::emissive:
				return "tw/lit_3_lit_rpl_ta1_804800_1002000000000010_0_1_1_0_0_13810017b_0_0_1_0_0";
			case donor_kind::alpha_test:
			case donor_kind::alpha_test_two_sided:
				return "tw/lit_3_lit_alt_ta1_80c000_1002000000000010_0_1_1_0_0_138f0015b_0_0_1_0_0";
			case donor_kind::decal:
				return "tw/lit_3_lit_bad_tca_814002_1002000000000010_0_1_1_0_0_13810015b_0_0_3_0_0";
			case donor_kind::translucent:
				return "tw/lit_3_lit_bad_tca_280002_1002000000000010_0_1_1_0_0_13810015b_0_0_3_0_0";
			case donor_kind::glass:
				return "tw/lit_3_lit_rpl_ta1_801004_1002000001080011_0_1_1_0_0_13810035b_0_0_1_0_0";
			default:
				return "tw/lit_3_lit_rpl_ta1_800800_1002000000000010_0_1_1_0_0_13810015b_0_0_1_0_0";
			}
		}

		const char* get_static_model_donor_techset(const donor_kind kind) // TODO: stock IW8 techsets, convert natively
		{
			switch (kind)
			{
			case donor_kind::emissive:
				return "tmo/lit_3_lit_rpl_ta1_800800_1002000000000010_0_1_1_0_0_13810017b_0_0_1_0_0";
			case donor_kind::alpha_test:
				return "tmo/lit_3_lit_alt_ta1_800800_1002000000000010_0_1_1_0_0_138f0015b_0_0_1_0_0";
			case donor_kind::alpha_test_two_sided:
				return "tmo/lit_3_lit_alt_ta1_808800_1002000000000010_0_1_1_0_0_13830015b_0_0_1_0_0";
			case donor_kind::decal:
				return "tmo/lit_3_lit_bad_tca_a90802_1002000000000010_0_1_1_0_0_13810015b_0_0_3_0_0";
			case donor_kind::translucent:
				return "tmo/lit_3_lit_bad_tca_284002_1002000000000010_0_1_1_0_0_13810015b_0_0_3_0_0";
			case donor_kind::glass:
				return "tmo/lit_3_lit_bad_ta1_c02_1002000000000011_0_1_1_0_0_13810035b_0_0_1_0_0";
			default:
				return "tmo/lit_3_lit_rpl_ta1_800800_1002000000000010_0_1_1_0_0_13810015b_0_0_1_0_0";
			}
		}

		const char* get_model_donor_techset(const donor_kind kind) // TODO: stock IW8 techsets, convert natively
		{
			switch (kind)
			{
			case donor_kind::alpha_test:
			case donor_kind::alpha_test_two_sided:
				return "mo/lit_3_lit_alt_ta1_808800_1002000000000010_0_1_1_0_0_13830015b_0_0_1_0_0";
			case donor_kind::decal:
				return "mo/lit_3_lit_bad_tca_a90802_1002000000000010_0_1_1_0_0_13810015b_0_0_3_0_0";
			case donor_kind::translucent:
				return "mo/lit_3_lit_bad_tca_802_1002000000000010_0_1_1_0_0_13810015b_0_0_3_0_0";
			case donor_kind::glass:
				return "mo/lit_3_lit_bad_ta1_c02_1002000000000011_0_1_1_0_0_13810035b_0_0_1_0_0";
			default:
				return "mo/lit_3_lit_rpl_ta1_800800_1002000000000010_0_1_1_0_0_13810017b_0_0_1_0_0";
			}
		}

		const char* get_donor_techset(const donor_kind kind, const material_usage usage)
		{
			switch (usage)
			{
			case material_usage::world:
				return get_world_donor_techset(kind);
			case material_usage::model:
				return get_model_donor_techset(kind);
			default:
				return get_static_model_donor_techset(kind);
			}
		}

		target::MaterialGeometryType get_material_type(const material_usage usage)
		{
			switch (usage)
			{
			case material_usage::world:
				return target::MTL_TYPE_WORLD;
			case material_usage::model:
				return target::MTL_TYPE_MODEL_SELFVIS;
			default:
				return target::MTL_TYPE_STATIC_MODEL_SELFVIS;
			}
		}

		std::unordered_set<std::string> donor_techsets;

		const Material* resolve(const Material* asset)
		{
			if (asset->techniqueSet || asset->textureCount)
			{
				return asset;
			}

			const auto loaded = static_cast<const Material*>(db_find_asset(ASSET_TYPE_MATERIAL, xhash_name(asset->name)));
			return loaded ? loaded : asset;
		}

		const GfxImage* find_texture(const Material* asset, const MaterialTextureSlot slot)
		{
			for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
			{
				if (asset->textureTable[i].slot == slot)
				{
					return asset->textureTable[i].image;
				}
			}

			return nullptr;
		}

		target::GfxImage* make_image_reference(const char* name, utils::memory::allocator& allocator)
		{
			const auto image = allocator.allocate<target::GfxImage>();
			image->name = name;
			return image;
		}

		target::GfxImage* image_or_reference(target::GfxImage* image, const char* fallback, utils::memory::allocator& allocator)
		{
			return image ? image : make_image_reference(fallback, allocator);
		}

		bool is_glass(const Material* asset)
		{
			const auto color_map = find_texture(asset, TEXTURE_SLOT_COLOR_MAP);
			return !color_map || gfximage::is_code_image(color_map) || find_texture(asset, TEXTURE_SLOT_THICKNESS_MAP);
		}

		const MaterialTechniqueSet* resolve_techset(const Material* asset)
		{
			if (!asset->techniqueSet)
			{
				return nullptr;
			}

			const auto loaded = static_cast<const MaterialTechniqueSet*>(db_find_asset(ASSET_TYPE_TECHSET, xhash_name(asset->techniqueSet->name)));
			return loaded ? loaded : asset->techniqueSet;
		}

		std::uint32_t get_techset_flags(const Material* asset)
		{
			const auto techset = resolve_techset(asset);
			return techset ? techset->flags : 0;
		}

		const MaterialShaderProgram* get_pixel_program(const MaterialTechniqueSet* techset, const std::size_t technique_index)
		{
			const auto technique = techset && technique_index < std::size(techset->techniques) ? techset->techniques[technique_index] : nullptr;
			const auto shader_set = technique ? technique->passes[0].shaderSet : nullptr;
			return shader_set ? shader_set->programs[1] : nullptr;
		}

		std::span<const std::uint8_t> get_pixel_constant_data(const Material* asset, const std::size_t technique_index)
		{
			const auto& physical = asset->physicalData;
			const auto entries = technique_index < std::size(physical.entrySets) ? physical.entrySets[technique_index] : nullptr;
			const auto entry = entries ? entries->entries[2] : nullptr;
			if (!entry || !physical.data || entry->offset + entry->size > physical.dataSize)
			{
				return {};
			}

			return {physical.data + entry->offset, entry->size};
		}

		template <typename T>
		std::optional<T> read_constant(const std::span<const std::uint8_t> data, const dxil::cbuffer* globals, const std::string_view name)
		{
			const auto field = globals ? globals->find(name) : nullptr;
			if (!field || field->size < sizeof(T) || field->offset + sizeof(T) > data.size())
			{
				return {};
			}

			T value{};
			std::memcpy(&value, data.data() + field->offset, sizeof(T));
			return value;
		}

		std::optional<material_instance> read_material_instance(const Material* asset)
		{
			const auto* images = asset->imageSets[0];
			if (!images || !images->instanceData)
			{
				return {};
			}

			const auto techset = resolve_techset(asset);
			for (const auto technique_index : std::array<std::size_t, 2>{13, 14})
			{
				const auto program = get_pixel_program(techset, technique_index);
				const auto buffers = program && program->bytecode ? dxil::read_structured_buffers(program->bytecode, program->bytecodeSize) : std::vector<dxil::cbuffer>{};
				const auto* layout = dxil::find_cbuffer(buffers, "g_MaterialInstanceData");
				if (layout && layout->size == images->instanceDataCount * sizeof(std::uint32_t))
				{
					return material_instance{{reinterpret_cast<const std::uint8_t*>(images->instanceData), layout->size}, *layout, images};
				}
			}

			return {};
		}

		const GfxImage* find_instance_image(const material_instance& instance, const std::string_view field_name)
		{
			const auto* field = instance.layout.find(field_name);
			for (auto i = 0u; field && instance.images->entries && i < instance.images->entryCount; i++)
			{
				if (instance.images->entries[i].instanceDataIndex * sizeof(std::uint32_t) == field->offset)
				{
					return instance.images->entries[i].image;
				}
			}

			return nullptr;
		}

		template <typename T>
		std::optional<T> read_instance_value(const material_instance& instance, const std::string_view field_name)
		{
			return read_constant<T>(instance.data, &instance.layout, field_name);
		}

		std::optional<gfximage::color_tint> read_instance_tint(const material_instance& instance)
		{
			const auto tint = read_instance_value<std::array<float, 3>>(instance, "diffuseConsts.colorTint");
			if (!tint || *tint == std::array{1.0f, 1.0f, 1.0f})
			{
				return {};
			}

			return gfximage::color_tint{*tint, find_instance_image(instance, "diffuseConsts.tintMask")};
		}

		std::optional<gfximage::layered_color> read_instance_layers(const material_instance& instance)
		{
			const auto* second_color = find_instance_image(instance, "mixConsts.colorMapDetail2");
			const auto* softlight = find_instance_image(instance, "diffuseConsts.detailColorMapSoftlightBase");
			if (!second_color && !softlight)
			{
				return {};
			}

			const std::array white{1.0f, 1.0f, 1.0f};
			const std::array unit_scale{1.0f, 1.0f};
			const std::array identity_levels{0.0f, 1.0f, 0.0f, 1.0f};
			const auto make_layer = [&](const GfxImage* color, const char* tint, const char* mask, const char* scale)
			{
				return gfximage::color_layer{color, {read_instance_value<std::array<float, 3>>(instance, tint).value_or(white), find_instance_image(instance, mask)},
					read_instance_value<std::array<float, 2>>(instance, scale).value_or(unit_scale)};
			};

			gfximage::layered_color result{};
			result.layers.push_back(make_layer(find_instance_image(instance, "diffuseConsts.colorMap"), "diffuseConsts.colorTint", "diffuseConsts.tintMask",
				"mixConsts.detailScale1"));
			if (second_color)
			{
				result.layers.push_back(make_layer(second_color, "mixConsts.colorTint1", "mixConsts.tintMask1", "mixConsts.detailScale2"));
				result.mixes.push_back({find_instance_image(instance, "mixConsts.mixMap"),
					read_instance_value<std::array<float, 2>>(instance, "mixConsts.mixScale").value_or(unit_scale),
					read_instance_value<std::array<float, 4>>(instance, "mixConsts.mixLevelsControls").value_or(identity_levels)});
			}

			if (const auto* third_color = find_instance_image(instance, "mixConsts.colorMapDetail3"); second_color && third_color)
			{
				result.layers.push_back(make_layer(third_color, "mixConsts.colorTint2", "mixConsts.tintMask2", "mixConsts.detailScale3"));
				result.mixes.push_back({find_instance_image(instance, "mixConsts.mixMap1"),
					read_instance_value<std::array<float, 2>>(instance, "mixConsts.mixScale1").value_or(unit_scale), identity_levels});
			}

			if (softlight)
			{
				result.softlight = {softlight, read_instance_value<std::array<float, 2>>(instance, "diffuseConsts.detailColorMapScale").value_or(unit_scale)};
			}

			return result;
		}

		std::optional<gfximage::color_tint> read_color_tint(const Material* asset)
		{
			const auto techset = resolve_techset(asset);
			for (const auto technique_index : std::array<std::size_t, 5>{5, 6, 7, 3, 4})
			{
				const auto program = get_pixel_program(techset, technique_index);
				const auto cbuffers = program && program->bytecode ? dxil::read_cbuffers(program->bytecode, program->bytecodeSize) : std::vector<dxil::cbuffer>{};
				const auto tint = read_constant<std::array<float, 3>>(get_pixel_constant_data(asset, technique_index), dxil::find_cbuffer(cbuffers, "$Globals"), "colorTint");
				if (!tint)
				{
					continue;
				}

				if (*tint == std::array{1.0f, 1.0f, 1.0f})
				{
					return {};
				}

				return gfximage::color_tint{*tint, find_texture(asset, TEXTURE_SLOT_TINT_MASK)};
			}

			return {};
		}

		std::optional<gfximage::layered_color> read_layered_color(const Material* asset)
		{
			const auto techset = resolve_techset(asset);
			for (const auto technique_index : std::array<std::size_t, 5>{5, 6, 7, 3, 4})
			{
				const auto program = get_pixel_program(techset, technique_index);
				const auto cbuffers = program && program->bytecode ? dxil::read_cbuffers(program->bytecode, program->bytecodeSize) : std::vector<dxil::cbuffer>{};
				const auto* globals = dxil::find_cbuffer(cbuffers, "$Globals");
				const auto data = get_pixel_constant_data(asset, technique_index);
				const auto layer_tint = read_constant<std::array<float, 3>>(data, globals, "colorTint1");
				if (!layer_tint)
				{
					continue;
				}

				const auto* layer_color = find_texture(asset, TEXTURE_SLOT_COLOR_MAP_DETAIL2);
				const auto* mix = find_texture(asset, TEXTURE_SLOT_MIX_MAP);
				if (!layer_color || !mix)
				{
					return {};
				}

				const std::array white{1.0f, 1.0f, 1.0f};
				const std::array unit_scale{1.0f, 1.0f};
				gfximage::layered_color result{};
				result.layers.push_back({find_texture(asset, TEXTURE_SLOT_COLOR_MAP),
					{read_constant<std::array<float, 3>>(data, globals, "colorTint").value_or(white), find_texture(asset, TEXTURE_SLOT_TINT_MASK)},
					read_constant<std::array<float, 2>>(data, globals, "detailScale1").value_or(unit_scale)});
				result.layers.push_back({layer_color, {*layer_tint, find_texture(asset, TEXTURE_SLOT_TINT_MASK1)},
					read_constant<std::array<float, 2>>(data, globals, "detailScale2").value_or(unit_scale)});
				result.mixes.push_back({mix, read_constant<std::array<float, 2>>(data, globals, "mixScale").value_or(unit_scale),
					read_constant<std::array<float, 4>>(data, globals, "mixLevelsControls").value_or(std::array{0.0f, 1.0f, 0.0f, 1.0f})});
				return result;
			}

			return {};
		}

		target::GfxImage* convert_color(const Material* asset)
		{
			const auto instance = read_material_instance(asset);
			if (const auto layered = instance ? read_instance_layers(*instance) : read_layered_color(asset))
			{
				if (auto* image = gfximage::dump_layered_color(*layered))
				{
					return image;
				}
			}

			const auto color = find_texture(asset, TEXTURE_SLOT_COLOR_MAP);
			const auto specular = find_texture(asset, TEXTURE_SLOT_SPEC_COLOR_MAP);
			const auto tint = instance ? read_instance_tint(*instance) : read_color_tint(asset);
			const auto tint_pointer = tint ? &*tint : nullptr;
			return specular ? gfximage::dump_color_specular(color, specular, tint_pointer) : gfximage::dump_color(color, tint_pointer);
		}

		std::array<std::uint32_t, 4> convert_emissive_tint(const Material* asset, const std::string& name)
		{
			const auto program = get_pixel_program(resolve_techset(asset), 6);
			const auto cbuffers = program && program->bytecode ? dxil::read_cbuffers(program->bytecode, program->bytecodeSize) : std::vector<dxil::cbuffer>{};
			const auto globals = dxil::find_cbuffer(cbuffers, "$Globals");
			const auto data = get_pixel_constant_data(asset, 6);

			const auto hdr_scale = read_constant<float>(data, globals, "hdrScale");
			const auto tint = read_constant<std::array<float, 3>>(data, globals, "emissiveTint1");
			const auto relative = read_constant<std::uint32_t>(data, globals, "relativeHDR");
			if (!hdr_scale || !tint)
			{
				ZONETOOL_WARNING("Material \"%s\": no emissive constants, intensity 1", name.data());
				return {0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000};
			}

			const auto scale = relative.value_or(0) ? *hdr_scale : *hdr_scale / 128.0f; // TODO: Q23 absolute HDR unit
			return {std::bit_cast<std::uint32_t>((*tint)[0] * scale), std::bit_cast<std::uint32_t>((*tint)[1] * scale),
				std::bit_cast<std::uint32_t>((*tint)[2] * scale), 0x3F800000};
		}

		donor_kind get_opaque_kind(const Material* asset, const bool emissive)
		{
			if (emissive)
			{
				return donor_kind::emissive;
			}

			const auto flags = get_techset_flags(asset);
			if (!(flags & TECHSET_FLAG_ALPHA_TEST) || (flags & TECHSET_FLAG_OPAQUE_DEPTH) || !find_texture(asset, TEXTURE_SLOT_COLOR_MAP))
			{
				return donor_kind::opaque;
			}

			return flags & TECHSET_FLAG_TWO_SIDED ? donor_kind::alpha_test_two_sided : donor_kind::alpha_test;
		}

		target::SurfaceType convert_surface_type(const std::uint32_t surface_type_bits)
		{
			switch (surface_type_bits >> 20)
			{
			case SURFACE_TYPE_BARK:
				return target::SURFACE_TYPE_BARK;
			case SURFACE_TYPE_BRICK:
				return target::SURFACE_TYPE_BRICK;
			case SURFACE_TYPE_CARPET:
				return target::SURFACE_TYPE_CARPET_SOLID;
			case SURFACE_TYPE_CLOTH:
				return target::SURFACE_TYPE_CLOTH;
			case SURFACE_TYPE_CONCRETE:
				return target::SURFACE_TYPE_CONCRETE_DRY;
			case SURFACE_TYPE_DIRT:
				return target::SURFACE_TYPE_DIRT;
			case SURFACE_TYPE_FLESH:
			case SURFACE_TYPE_PLAYER:
				return target::SURFACE_TYPE_FLESH;
			case SURFACE_TYPE_FOLIAGE:
				return target::SURFACE_TYPE_FOLIAGE_DRY;
			case SURFACE_TYPE_GLASS:
				return target::SURFACE_TYPE_GLASS_PANE;
			case SURFACE_TYPE_GRASS:
				return target::SURFACE_TYPE_GRASS_SHORT;
			case SURFACE_TYPE_GRAVEL:
				return target::SURFACE_TYPE_GRAVEL;
			case SURFACE_TYPE_ICE:
				return target::SURFACE_TYPE_ICE_SOLID;
			case SURFACE_TYPE_METAL:
			case SURFACE_TYPE_PAINTED_METAL:
				return target::SURFACE_TYPE_METAL_THICK;
			case SURFACE_TYPE_MUD:
				return target::SURFACE_TYPE_MUD;
			case SURFACE_TYPE_PAPER:
				return target::SURFACE_TYPE_PAPER;
			case SURFACE_TYPE_PLASTER:
				return target::SURFACE_TYPE_PLASTER;
			case SURFACE_TYPE_ROCK:
				return target::SURFACE_TYPE_ROCK;
			case SURFACE_TYPE_SAND:
				return target::SURFACE_TYPE_SAND;
			case SURFACE_TYPE_SNOW:
				return target::SURFACE_TYPE_SNOW;
			case SURFACE_TYPE_WATER:
				return target::SURFACE_TYPE_WATER;
			case SURFACE_TYPE_WOOD:
				return target::SURFACE_TYPE_WOOD_SOLID;
			case SURFACE_TYPE_ASPHALT:
				return target::SURFACE_TYPE_ASPHALT_DRY;
			case SURFACE_TYPE_CERAMIC:
				return target::SURFACE_TYPE_CERAMIC;
			case SURFACE_TYPE_PLASTIC:
				return target::SURFACE_TYPE_PLASTIC;
			case SURFACE_TYPE_RUBBER:
				return target::SURFACE_TYPE_RUBBER;
			case SURFACE_TYPE_CUSHION:
				return target::SURFACE_TYPE_CUSHION;
			case SURFACE_TYPE_FRUIT:
				return target::SURFACE_TYPE_FRUIT;
			case SURFACE_TYPE_TALL_GRASS:
				return target::SURFACE_TYPE_GRASS_TALL;
			case SURFACE_TYPE_RIOT_SHIELD:
				return target::SURFACE_TYPE_RIOTSHIELD;
			case SURFACE_TYPE_METAL_THIN:
			case SURFACE_TYPE_METAL_HOLLOW:
				return target::SURFACE_TYPE_METAL_THIN;
			case SURFACE_TYPE_METAL_CATWALK:
				return target::SURFACE_TYPE_METAL_GRATE;
			case SURFACE_TYPE_METAL_CAR:
				return target::SURFACE_TYPE_METAL_CAR;
			case SURFACE_TYPE_GLASS_CAR:
				return target::SURFACE_TYPE_GLASS_VEHICLE;
			case SURFACE_TYPE_GLASS_BULLETPROOF:
				return target::SURFACE_TYPE_GLASS_SOLID;
			case SURFACE_TYPE_WATER_SHALLOW:
				return target::SURFACE_TYPE_WATER_KNEE;
			case SURFACE_TYPE_BODY_ARMOR:
				return target::SURFACE_TYPE_BODY_ARMOR;
			default:
				return target::SURFACE_TYPE_NONE;
			}
		}

		donor_kind get_donor_kind(const Material* asset, const bool emissive, const bool world)
		{
			switch (asset->drawList)
			{
			case DRAW_LIST_GBUFFER_DEPTH_HACK_DECAL:
			case DRAW_LIST_GBUFFER_DECAL:
			case DRAW_LIST_GBUFFER_NO_VOLUME_DECAL:
				return donor_kind::decal;
			case DRAW_LIST_LIT_TRANS:
			case DRAW_LIST_LIT_TRANS_POST_RESOLVE:
			case DRAW_LIST_DEPTH_HACK_TRANS:
			case DRAW_LIST_OIT:
				if (is_glass(asset))
				{
					return donor_kind::glass;
				}

				return donor_kind::translucent;
			default:
				return get_opaque_kind(asset, emissive);
			}
		}

		void set_draw_order(const donor_kind kind, const bool world, target::Material* result)
		{
			switch (kind)
			{
			case donor_kind::decal:
				result->cameraRegion = target::CAMERA_REGION_LIT_DECAL;
				result->sortKey = 10;
				break;
			case donor_kind::translucent:
				result->cameraRegion = target::CAMERA_REGION_LIT_TRANS;
				result->sortKey = 30;
				break;
			case donor_kind::glass:
				if (world)
				{
					result->cameraRegion = target::CAMERA_REGION_LIT_OPAQUE;
					result->sortKey = 2;
					break;
				}

				result->cameraRegion = target::CAMERA_REGION_LIT_TRANS;
				result->sortKey = 25;
				break;
			default:
				result->cameraRegion = target::CAMERA_REGION_LIT_OPAQUE;
				result->sortKey = 2;
				break;
			}
		}

		target::GfxImage* convert_opacity(const Material* asset, utils::memory::allocator& allocator)
		{
			if (const auto reveal = find_texture(asset, TEXTURE_SLOT_REVEAL_MAP))
			{
				if (const auto image = gfximage::dump_reveal_opacity(reveal))
				{
					return image;
				}
			}

			const auto image = gfximage::dump_opacity(find_texture(asset, TEXTURE_SLOT_COLOR_MAP));
			return image_or_reference(image, "$white", allocator);
		}

		target::GfxImage* convert_transmission(const Material* asset, utils::memory::allocator& allocator) // TODO: T9 thickness map
		{
			const auto image = gfximage::dump_alpha_transmission(find_texture(asset, TEXTURE_SLOT_COLOR_MAP));
			return image_or_reference(image, "$white", allocator);
		}

		target::Material* convert(const Material* asset, const std::string& name, const material_usage usage, utils::memory::allocator& allocator)
		{
			const auto world = usage == material_usage::world;
			const auto emissive_map = find_texture(asset, TEXTURE_SLOT_EMISSIVE_MAP);
			const auto emissive = emissive_map ? gfximage::dump_color(emissive_map) : nullptr;
			const auto kind = get_donor_kind(asset, emissive != nullptr, world);
			const auto blend = kind == donor_kind::decal || kind == donor_kind::translucent;
			const auto alpha_test = kind == donor_kind::alpha_test || kind == donor_kind::alpha_test_two_sided;
			const auto glass = kind == donor_kind::glass;
			const auto emissive_donor = kind == donor_kind::emissive || (usage == material_usage::model && kind == donor_kind::opaque);
			const auto donor = get_donor_techset(kind, usage);
			donor_techsets.emplace(donor);

			const auto result = allocator.allocate<target::Material>();
			result->name = allocator.duplicate_string(name);
			result->contents = asset->contents;
			result->surfaceFlags = convert_surface_type(asset->surfaceTypeBits) << 19;
			result->materialType = get_material_type(usage);
			result->flags = 0x10;
			set_draw_order(kind, world, result);
			result->textureAtlasRowCount = 1;
			result->textureAtlasColumnCount = 1;
			result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
			result->techniqueSet->name = donor;

			const auto color_map = find_texture(asset, TEXTURE_SLOT_COLOR_MAP);
			const auto code_color = color_map && gfximage::is_code_image(color_map);
			const auto color = glass && code_color ? nullptr : convert_color(asset);
			if (color_map && !color && !code_color)
			{
				ZONETOOL_WARNING("Material \"%s\": color map \"%s\" is not converted", name.data(), xhash_name(color_map->name).data());
			}

			const auto normal = gfximage::dump_normal_gloss(find_texture(asset, TEXTURE_SLOT_NORMAL_MAP), find_texture(asset, TEXTURE_SLOT_ROUGHNESS),
				find_texture(asset, TEXTURE_SLOT_AO_MAP));

			std::vector<target::MaterialTextureDef> textures
			{
				{0, image_or_reference(color, glass ? "$black" : gfximage::get_color_reference(color_map), allocator)},
				{9, image_or_reference(normal, "$identitynormalmap", allocator)},
			};

			if (emissive_donor)
			{
				textures.push_back({18, image_or_reference(emissive, "$black", allocator)});
			}

			if (blend)
			{
				textures.push_back({27, convert_opacity(asset, allocator)});
			}
			else if (alpha_test)
			{
				textures.push_back({27, image_or_reference(gfximage::dump_opacity(color_map), "$white", allocator)});
			}
			else if (glass && !world)
			{
				textures.push_back({27, make_image_reference("$white", allocator)});
			}

			textures.push_back({59, make_image_reference("$black", allocator)});
			if (glass)
			{
				textures.push_back({68, convert_transmission(asset, allocator)});
			}

			result->textureCount = static_cast<unsigned char>(textures.size());
			result->textureTable = allocator.allocate_array<target::MaterialTextureDef>(textures.size());
			std::ranges::copy(textures, result->textureTable);

			std::vector<std::pair<unsigned char, std::array<std::uint32_t, 4>>> literals
			{
				{0, {blend || alpha_test ? 0xFu : 0x7u, 0x80003C00, 0x3C003C00, 0}},
				{9, {0xFFFFFFFF, 0x00FF2FAD, 0x0000BC00, 0}},
				{54, {color ? color->semanticSpecific.albedoMapScaleBias : 0x9, normal ? normal->semanticSpecific.normalMapScaleBias : 0x00FF00FF, 0x4F800000, 0}},
				{72, {0x3F800000, 0x1, 0x3FD9364E, 0x3FD9364E}},
			};

			if (kind == donor_kind::emissive)
			{
				literals.insert(literals.begin() + 2, {target::MATERIAL_CONSTANT_EFFECT_COLOR_TINT, convert_emissive_tint(asset, name)});
			}
			else if (emissive_donor)
			{
				literals.insert(literals.begin() + 2, {target::MATERIAL_CONSTANT_EFFECT_COLOR_TINT, {0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000}});
			}
			else if (glass)
			{
				literals.insert(literals.begin() + 2, {target::MATERIAL_CONSTANT_TRANSMISSION_TINT, {0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000}}); // TODO: T9 glass tint
			}

			if (kind == donor_kind::translucent || (kind == donor_kind::decal && !world))
			{
				literals.push_back({87, {0x49742400, 0x36039605, 0x4BFA459C, 0xB4000000}}); // stock tmo decal value
				literals.push_back({89, {0, 0, 0, 0}});
			}

			result->constantCount = static_cast<unsigned char>(literals.size());
			result->constantTable = allocator.allocate_array<target::MaterialConstantDef>(literals.size());
			for (auto i = 0u; i < literals.size(); i++)
			{
				result->constantTable[i].index = literals[i].first;
				std::memcpy(result->constantTable[i].literal, literals[i].second.data(), sizeof(result->constantTable[i].literal));
			}

			return result;
		}

		void add_image_reference_rows(const target::Material* material)
		{
			for (auto i = 0; i < material->textureCount; i++)
			{
				if (const auto image = material->textureTable[i].image->name; image[0] == '$')
				{
					add_csv_row("image", ","s + image);
				}
			}
		}

		void dump_zone_donor_techsets(const char* zone_name)
		{
			const auto path = game_files::find_zone(zone_name);
			if (!path)
			{
				ZONETOOL_WARNING("IW8 zone \"%s\" is missing, its donor techsets are not dumped", zone_name);
				return;
			}

			const auto database = db_create();
			db_scope scope(*database);
			const auto zone = target::load_zone(*path);

			std::unordered_set<std::string> shaders;
			for (const auto& asset : zone->assets)
			{
				auto* techset = static_cast<target::MaterialTechniqueSet*>(asset.header);
				if (asset.referenced || asset.type != target::ASSET_TYPE_TECHNIQUE_SET || !donor_techsets.erase(techset->name))
				{
					continue;
				}

				target::technique_set::dump(techset);
				for (auto i = 0u; techset->maskedTechniques && i < target::get_technique_count(techset); i++)
				{
					const auto* technique = techset->maskedTechniques[i];
					if (!technique)
					{
						continue;
					}

					for (const auto* shader : {technique->vertexShader ? technique->vertexShader->name : nullptr,
						technique->hullShader ? technique->hullShader->name : nullptr,
						technique->domainShader ? technique->domainShader->name : nullptr,
						technique->pixelShader ? technique->pixelShader->name : nullptr})
					{
						if (shader)
						{
							shaders.emplace(shader);
						}
					}
				}
			}

			for (const auto& asset : zone->assets)
			{
				if (asset.type != target::ASSET_TYPE_VERTEXSHADER && asset.type != target::ASSET_TYPE_HULLSHADER
					&& asset.type != target::ASSET_TYPE_DOMAINSHADER && asset.type != target::ASSET_TYPE_PIXELSHADER)
				{
					continue;
				}

				const auto* shader = *static_cast<const char* const*>(asset.header);
				if (!shaders.contains(shader))
				{
					continue;
				}

				if (asset.referenced)
				{
					add_csv_row(target::type_to_string(asset.type), ","s + shader);
					continue;
				}

				switch (asset.type)
				{
				case target::ASSET_TYPE_VERTEXSHADER:
					target::vertex_shader::dump(static_cast<target::MaterialVertexShader*>(asset.header));
					break;
				case target::ASSET_TYPE_HULLSHADER:
					target::hull_shader::dump(static_cast<target::MaterialHullShader*>(asset.header));
					break;
				case target::ASSET_TYPE_DOMAINSHADER:
					target::domain_shader::dump(static_cast<target::MaterialDomainShader*>(asset.header));
					break;
				case target::ASSET_TYPE_PIXELSHADER:
					target::pixel_shader::dump(static_cast<target::MaterialPixelShader*>(asset.header));
					break;
				default:
					break;
				}
			}
		}

		void dump_material(const Material* asset, const std::string& name, const material_usage usage)
		{
			if (!mark_dumped(target::ASSET_TYPE_MATERIAL, name))
			{
				return;
			}

			utils::memory::allocator allocator;
			const auto converted = convert(resolve(asset), name, usage, allocator);
			zonetool::iw8::material::dump(converted);
			add_image_reference_rows(converted);
			add_csv_row("material", name);
		}
	}

	void dump(const Material* asset)
	{
		dump_material(asset, xhash_name(asset->name), material_usage::static_model);
	}

	std::string dump_model(const Material* asset)
	{
		const auto name = "mo/" + xhash_name(asset->name);
		dump_material(asset, name, material_usage::model);
		return name;
	}

	std::string dump_world(const Material* asset)
	{
		const auto name = "tw/" + xhash_name(asset->name);
		dump_material(asset, name, material_usage::world);
		return name;
	}

	void dump_2d(const GfxImage* image, const std::string& name)
	{
		if (!mark_dumped(target::ASSET_TYPE_MATERIAL, name))
		{
			return;
		}

		const auto* const techset = "cust_2_2d_rpl_ta0_0_0_0_0_0"; // TODO: stock compass_map_mp_rust techset, convert natively

		utils::memory::allocator allocator;
		const auto result = allocator.allocate<target::Material>();
		result->name = allocator.duplicate_string(name);
		result->contents = 1;
		result->surfaceFlags = 0x10000;
		result->materialType = target::MTL_TYPE_DEFAULT;
		result->cameraRegion = target::CAMERA_REGION_LIT_TRANS;
		result->sortKey = 30;
		result->textureAtlasRowCount = 1;
		result->textureAtlasColumnCount = 1;
		result->techniqueSet = allocator.allocate<target::MaterialTechniqueSet>();
		result->techniqueSet->name = techset;

		result->textureCount = 1;
		result->textureTable = allocator.allocate<target::MaterialTextureDef>();
		result->textureTable->index = 158;
		result->textureTable->image = image_or_reference(gfximage::dump_2d(image), "$white", allocator);

		const std::array<std::uint32_t, 4> tint{0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000};
		result->constantCount = 1;
		result->constantTable = allocator.allocate<target::MaterialConstantDef>();
		result->constantTable->index = target::MATERIAL_CONSTANT_2D_COLOR_TINT;
		std::memcpy(result->constantTable->literal, tint.data(), sizeof(tint));

		result->constantBufferIndex = allocator.allocate_array<unsigned char>(target::TECHNIQUE_COUNT);
		std::memset(result->constantBufferIndex, 0xFF, target::TECHNIQUE_COUNT);
		result->constantBufferIndex[target::TECHNIQUE_EMISSIVE] = 0;
		result->constantBufferCount = 1;
		result->constantBufferTable = allocator.allocate<target::MaterialConstantBufferDef>();
		result->constantBufferTable->psDataSize = sizeof(tint);
		result->constantBufferTable->psData = allocator.allocate_array<char>(sizeof(tint));
		std::memcpy(result->constantBufferTable->psData, tint.data(), sizeof(tint));

		zonetool::iw8::material::dump(result);
		add_image_reference_rows(result);
		add_csv_row("techset", ","s + techset);
		add_csv_row("material", name);
	}

	world_surface_list get_world_surface_list(const Material* asset)
	{
		switch (get_donor_kind(resolve(asset), false, true))
		{
		case donor_kind::decal:
			return world_surface_list::decal;
		case donor_kind::translucent:
			return world_surface_list::trans;
		default:
			return world_surface_list::opaque;
		}
	}

	void dump_donor_techsets()
	{
		if (donor_techsets.empty())
		{
			return;
		}

		xfile::oodle::load(iw8_folder());
		auto previous_files = game_files::exchange(game_files::open_casc(iw8_folder()));
		const auto _ = gsl::finally([&previous_files] { game_files::set(std::move(previous_files)); });

		for (const auto* zone : {"techsets_mp_aniyah_tac", "techsets_mp_rust", "techsets_mp_donetsk", "techsets_common_base_mp"})
		{
			if (!donor_techsets.empty())
			{
				dump_zone_donor_techsets(zone);
			}
		}

		for (const auto& name : donor_techsets)
		{
			ZONETOOL_WARNING("IW8 donor techset \"%s\" is missing", name.data());
		}
	}
}
