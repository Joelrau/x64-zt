#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"
#include "gfximage.hpp"
#include "techset.hpp"
#include "techset/native.hpp"
#include "techset/slots.hpp"

#include <utils/string.hpp>

namespace zonetool::s2::converter::h1::material
{
	namespace
	{
		constexpr std::array<default_texture, 3> default_textures
		{{
			{"colorMap", zonetool::h1::TS_COLOR_MAP, "$white"},
			{"normalMap", zonetool::h1::TS_NORMAL_MAP, "$identitynormalmap"},
			{"specularMap", zonetool::h1::TS_SPECULAR_MAP, "$black"},
		}};

		std::vector<Material*> pending_materials;
		std::unordered_map<std::string, std::uint8_t> camera_regions;

		std::uint32_t texture_name_hash(const std::string_view name)
		{
			std::uint32_t hash = 0;
			for (const auto c : name)
			{
				hash = (static_cast<std::uint8_t>(c) | 0x20) ^ (hash * 33);
			}

			return hash;
		}

		const Material* find_table_owner(const Material* asset)
		{
			while (!asset->textureTable && !asset->constantTable && asset->parentMaterial)
			{
				asset = asset->parentMaterial;
			}

			return asset;
		}

		const Material* find_state_owner(const Material* asset)
		{
			while (!asset->stateBitsTable && asset->parentMaterial)
			{
				asset = asset->parentMaterial;
			}

			return asset;
		}

		const MaterialTextureDef* find_texture(const Material* asset, const std::uint32_t hash)
		{
			const auto owner = find_table_owner(asset);
			for (auto i = 0; i < owner->textureCount && owner->textureTable; i++)
			{
				if (owner->textureTable[i].nameHash == hash)
				{
					return &owner->textureTable[i];
				}
			}

			return nullptr;
		}

		std::vector<std::uint32_t> srgb_textures(const Material* asset)
		{
			std::vector<std::uint32_t> result;
			const auto owner = find_table_owner(asset);
			for (auto i = 0; i < owner->textureCount && owner->textureTable; i++)
			{
				const auto& texture = owner->textureTable[i];
				if (texture.u.image && texture.u.image->name && gfximage::is_srgb_image(texture.u.image->name))
				{
					result.emplace_back(texture.nameHash);
				}
			}

			std::ranges::sort(result);
			return result;
		}

		const MaterialConstantDef* find_constant(const Material* asset, const std::uint32_t hash)
		{
			const auto owner = find_table_owner(asset);
			for (auto i = 0; i < owner->constantCount && owner->constantTable; i++)
			{
				if (owner->constantTable[i].nameHash == hash)
				{
					return &owner->constantTable[i];
				}
			}

			return nullptr;
		}

		std::string get_techset_name(const Material* asset)
		{
			return asset->techniqueSet && asset->techniqueSet->name ? asset->techniqueSet->name : "";
		}

		std::uint8_t convert_sort_key(const std::uint8_t sort_key)
		{
			switch (sort_key)
			{
			case MTL_SORTKEY_OPAQUE:
				return zonetool::h1::MTL_SORTKEY_OPAQUE;
			case MTL_SORTKEY_SKY:
				return zonetool::h1::MTL_SORTKEY_SKY;
			default:
				break;
			}

			if (sort_key < MTL_SORTKEY_SKY)
			{
				return zonetool::h1::MTL_SORTKEY_OPAQUE;
			}

			if (sort_key <= zonetool::h1::MTL_SORTKEY_DECAL_TOP_3 + 3)
			{
				return static_cast<std::uint8_t>(sort_key - 3);
			}

			if (sort_key < MTL_SORTKEY_TRANS_BEGIN)
			{
				return zonetool::h1::MTL_SORTKEY_HAIR;
			}

			if (sort_key <= zonetool::h1::MTL_SORTKEY_VIEWMODEL_EFFECT)
			{
				return sort_key;
			}

			return static_cast<std::uint8_t>(sort_key >= zonetool::h1::MTL_SORTKEY_2D + 2 ? sort_key - 2 : sort_key - 1);
		}

		std::uint8_t convert_camera_region(const std::uint8_t camera_region)
		{
			switch (camera_region)
			{
			case CAMERA_REGION_LIT_OPAQUE:
				return zonetool::h1::CAMERA_REGION_LIT_OPAQUE;
			case CAMERA_REGION_LIT_OPAQUE_SSS:
				return zonetool::h1::CAMERA_REGION_LIT_OPAQUE_SSS;
			case CAMERA_REGION_LIT_DECAL:
				return zonetool::h1::CAMERA_REGION_LIT_DECAL;
			case CAMERA_REGION_LIT_TRANS:
				return zonetool::h1::CAMERA_REGION_LIT_TRANS;
			case CAMERA_REGION_EMISSIVE:
				return zonetool::h1::CAMERA_REGION_EMISSIVE;
			case 3:
				return zonetool::h1::CAMERA_REGION_LIT_TRANS;
			default:
				return zonetool::h1::CAMERA_REGION_NONE;
			}
		}

		std::uint8_t convert_state_flags(const std::uint8_t s2_flags, const std::uint32_t load_bits0)
		{
			constexpr std::array<std::pair<std::uint8_t, std::uint8_t>, 4> flag_map
			{{
				{STATE_FLAG_DECAL, zonetool::h1::STATE_FLAG_DECAL},
				{STATE_FLAG_WRITES_DEPTH, zonetool::h1::STATE_FLAG_WRITES_DEPTH},
				{STATE_FLAG_USES_DEPTH_BUFFER, zonetool::h1::STATE_FLAG_USES_DEPTH_BUFFER},
				{STATE_FLAG_USES_STENCIL_BUFFER, zonetool::h1::STATE_FLAG_USES_STENCIL_BUFFER},
			}};

			std::uint8_t result = 0;
			for (const auto& [s2_flag, h1_flag] : flag_map)
			{
				if (s2_flags & s2_flag)
				{
					result |= h1_flag;
				}
			}

			switch (load_bits0 & zonetool::h1::GFXS0_CULL_MASK)
			{
			case zonetool::h1::GFXS0_CULL_BACK:
				return result | zonetool::h1::STATE_FLAG_CULL_BACK;
			case zonetool::h1::GFXS0_CULL_FRONT:
				return result | zonetool::h1::STATE_FLAG_CULL_FRONT;
			default:
				return result;
			}
		}

		std::uint8_t convert_material_type(const std::string& h1_techset)
		{
			static const std::unordered_map<std::string_view, zonetool::h1::MaterialType> prefix_types
			{
				{"m", zonetool::h1::MTL_TYPE_MODEL},
				{"me", zonetool::h1::MTL_TYPE_MODEL_EFFECT},
				{"mc", zonetool::h1::MTL_TYPE_MODEL_VERTCOL},
				{"mce", zonetool::h1::MTL_TYPE_MODEL_VERTCOL_EFFECT},
				{"mv", zonetool::h1::MTL_TYPE_MODEL_VERTLIT},
				{"mvc", zonetool::h1::MTL_TYPE_MODEL_VERTLIT_VERTCOL},
				{"ml", zonetool::h1::MTL_TYPE_MODEL_LMAP},
				{"mlc", zonetool::h1::MTL_TYPE_MODEL_LMAP_VERTCOL},
				{"ms", zonetool::h1::MTL_TYPE_MODEL_SUBDIV},
				{"w", zonetool::h1::MTL_TYPE_WORLD},
				{"wc", zonetool::h1::MTL_TYPE_WORLD_VERTCOL},
			};

			const std::string_view name(h1_techset);
			if (const auto type = prefix_types.find(name.substr(0, name.find('_'))); type != prefix_types.end())
			{
				return static_cast<std::uint8_t>(type->second);
			}

			return name.starts_with("l_") ? zonetool::h1::MTL_TYPE_WORLD : zonetool::h1::MTL_TYPE_DEFAULT;
		}

		std::uint32_t unpack_blend_word(const std::uint32_t s2_word)
		{
			return (s2_word & 0x7FF) | ((s2_word & 0x3FF800) << 1) | ((s2_word & 0x3C00000) << 2);
		}

		load_bits convert_load_bits(const GfxStateBits& state)
		{
			const auto polygon_offset = (state.loadBits[2] >> 27) & 3;

			load_bits result{};
			result[0] = state.loadBits[0];
			result[1] = (state.loadBits[1] & ~zonetool::h1::GFXS1_POLYGON_OFFSET_MASK) | (polygon_offset << zonetool::h1::GFXS1_POLYGON_OFFSET_SHIFT);
			if ((result[1] & zonetool::h1::GFXS1_DEPTHTEST_MASK) == zonetool::h1::GFXS1_DEPTHTEST_EQUAL)
			{
				result[1] = (result[1] & ~zonetool::h1::GFXS1_DEPTHTEST_MASK) | zonetool::h1::GFXS1_DEPTHTEST_LESSEQUAL;
			}
			result[2] = state.loadBits[2] & 0xFFFFFF;
			result[3] = unpack_blend_word(state.loadBits[3] & ~GFXS3_BLEND_CLASS_MASK);
			result[4] = unpack_blend_word(state.loadBits[4]);
			result[5] = unpack_blend_word(state.loadBits[5]);
			return result;
		}

		std::uint32_t make_blend(const std::uint32_t src, const std::uint32_t dst, const std::uint32_t op)
		{
			return (src << zonetool::h1::GFXS3_SRCBLEND_RGB_SHIFT) | (dst << zonetool::h1::GFXS3_DSTBLEND_RGB_SHIFT) | (op << zonetool::h1::GFXS3_BLENDOP_RGB_SHIFT);
		}

		void use_stock_blend(std::vector<load_bits>& states, const std::string_view h1_techset)
		{
			if (h1_techset.find("premul") != std::string_view::npos)
			{
				return;
			}

			const auto tokens = utils::string::split(std::string(h1_techset), '_');
			const auto additive = std::ranges::find(tokens, "add") != tokens.end();
			for (auto& state : states)
			{
				const auto src = state[3] & zonetool::h1::GFXS3_SRCBLEND_RGB_MASK;
				const auto dst = (state[3] & zonetool::h1::GFXS3_DSTBLEND_RGB_MASK) >> zonetool::h1::GFXS3_DSTBLEND_RGB_SHIFT;
				if (src != zonetool::h1::GFXS_BLEND_ONE || dst != zonetool::h1::GFXS_BLEND_INVSRCALPHA)
				{
					continue;
				}

				state[3] = (state[3] & ~zonetool::h1::GFXS3_BLEND_RGB_MASK) | (additive
					? make_blend(zonetool::h1::GFXS_BLEND_ONE, zonetool::h1::GFXS_BLEND_ONE, zonetool::h1::GFXS_BLENDOP_ADD)
					: make_blend(zonetool::h1::GFXS_BLEND_SRCALPHA, zonetool::h1::GFXS_BLEND_INVSRCALPHA, zonetool::h1::GFXS_BLENDOP_ADD));
			}
		}

		load_bits make_lit_state(load_bits state)
		{
			const auto opaque = (state[3] & zonetool::h1::GFXS3_BLEND_RGB_MASK) == make_blend(zonetool::h1::GFXS_BLEND_ONE, zonetool::h1::GFXS_BLEND_ZERO, zonetool::h1::GFXS_BLENDOP_DISABLED);
			if (opaque && !(state[1] & zonetool::h1::GFXS1_DEPTHWRITE))
			{
				state[3] |= zonetool::h1::GFXS3_DEPTHWRITE_OPAQUE;
			}

			return state;
		}

		load_bits make_light_state(load_bits lit)
		{
			const auto dst = (lit[3] & zonetool::h1::GFXS3_DSTBLEND_RGB_MASK) >> zonetool::h1::GFXS3_DSTBLEND_RGB_SHIFT;
			const auto src = dst == zonetool::h1::GFXS_BLEND_INVSRCALPHA ? zonetool::h1::GFXS_BLEND_SRCALPHA : zonetool::h1::GFXS_BLEND_ONE;

			lit[1] = (lit[1] & ~(zonetool::h1::GFXS1_DEPTHWRITE | zonetool::h1::GFXS1_POLYGON_OFFSET_MASK)) | zonetool::h1::GFXS1_POLYGON_OFFSET_2;
			lit[3] = (lit[3] & (zonetool::h1::GFXS3_BLEND_ALPHA_MASK | zonetool::h1::GFXS3_BLEND_COLORWRITE_MASK))
				| make_blend(src, zonetool::h1::GFXS_BLEND_ONE, zonetool::h1::GFXS_BLENDOP_ADD);
			return lit;
		}

		std::array<std::uint64_t, zonetool::h1::GFX_DEPTH_STENCIL_MODE_COUNT> depth_stencil_states(const load_bits& state)
		{
			const auto base = state[1] & ~zonetool::h1::GFXS1_POLYGON_OFFSET_MASK;
			const auto depth = base & 0xBF;
			const auto stencil_masks = state[2] != 0xFFFF ? state[2] & 0xFFFF : 0x0909;
			auto cached_spot = base | zonetool::h1::GFXS1_STENCIL_FRONT_ENABLE | zonetool::h1::GFXS1_DEPTHWRITE | zonetool::h1::GFXS1_STENCILFUNC_FRONTBACK_MASK;
			cached_spot = (cached_spot & ~((7u << zonetool::h1::GFXS1_STENCIL_FRONT_PASS_SHIFT) | (7u << zonetool::h1::GFXS1_STENCIL_BACK_PASS_SHIFT)))
				| (3u << zonetool::h1::GFXS1_STENCIL_FRONT_PASS_SHIFT) | (3u << zonetool::h1::GFXS1_STENCIL_BACK_PASS_SHIFT);

			const std::array<std::pair<std::uint32_t, std::uint32_t>, zonetool::h1::GFX_DEPTH_STENCIL_MODE_COUNT> states
			{{
				{stencil_masks, base},
				{0x1010, depth | 0xE02E0240},
				{0x0404, base},
				{0x0202, depth | 0xE02E0241},
				{0x1212, depth | 0xE02E0241},
				{0x0909, base | zonetool::h1::GFXS1_DEPTHWRITE},
				{0x1010, depth | 0xE02E0241},
				{0xFFFF, cached_spot},
				{0xFFFF, cached_spot & ~zonetool::h1::GFXS1_DEPTHTEST_MASK},
				{0xFFFF, base},
			}};

			std::array<std::uint64_t, zonetool::h1::GFX_DEPTH_STENCIL_MODE_COUNT> result{};
			for (auto i = 0u; i < result.size(); i++)
			{
				result[i] = (static_cast<std::uint64_t>(states[i].first) << 32) | states[i].second;
			}

			return result;
		}

		std::uint8_t rasterizer_state(const load_bits& state)
		{
			return static_cast<std::uint8_t>(((state[0] >> 3) & 3) | ((state[1] & zonetool::h1::GFXS1_POLYGON_OFFSET_MASK) >> 2)
				| ((state[0] & zonetool::h1::GFXS0_POLYMODE_LINE) >> 1) | ((state[0] & zonetool::h1::GFXS0_ATEST_MASK) << 4)
				| ((state[0] & zonetool::h1::GFXS0_GAMMAWRITE) << 1));
		}

		load_bits with_stencil_ops(load_bits state, const std::uint32_t stencil_ops)
		{
			if (!(state[1] & ~0xFFu))
			{
				state[1] |= stencil_ops;
			}

			return state;
		}

		load_bits with_color_write(load_bits state, const std::uint32_t color_write)
		{
			state[3] = (state[3] & ~zonetool::h1::GFXS3_BLEND_COLORWRITE_MASK) | color_write;
			return state;
		}

		load_bits finish_h1_state(load_bits state, const std::int32_t h1_base, const zonetool::h1::MaterialTechniqueSet* techset)
		{
			const auto has_lit = techset->techniques[zonetool::h1::TECHNIQUE_LIT] != nullptr;
			if (techset::uses_lit_state(h1_base) || techset::is_light_pass(h1_base) || (h1_base == zonetool::h1::TECHNIQUE_UNLIT && has_lit))
			{
				state[0] |= zonetool::h1::GFXS0_GAMMAWRITE;
			}

			switch (h1_base)
			{
			case zonetool::h1::TECHNIQUE_ZPREPASS:
				return with_color_write(with_stencil_ops(state, 0xE49E4900), 0);
			case zonetool::h1::TECHNIQUE_ZPREPASS_HIDIR:
				return with_color_write(with_stencil_ops(state, 0xE49E4900), zonetool::h1::GFXS3_BLEND_COLORWRITE_RGB);
			case zonetool::h1::TECHNIQUE_WIREFRAME_SOLID:
			case zonetool::h1::TECHNIQUE_WIREFRAME_SHADED:
				return with_stencil_ops(state, 0xE49E4900);
			case zonetool::h1::TECHNIQUE_VELOCITY_RIGID:
			case zonetool::h1::TECHNIQUE_VELOCITY_SKINNED:
				return with_stencil_ops(state, 0xE00E1200);
			case zonetool::h1::TECHNIQUE_BUILD_SHADOWMAP_DEPTH:
				return techset->techniques[zonetool::h1::TECHNIQUE_BUILD_SHADOWMAP_COLOR] ? state : with_color_write(state, 0);
			default:
				return state;
			}
		}

		class state_converter
		{
		public:
			explicit state_converter(const Material* asset)
				: asset_(find_state_owner(asset))
			{
			}

			std::optional<load_bits> source_state(const std::int32_t s2_group_begin, const std::int32_t s2_base, const std::uint32_t pass) const
			{
				for (const auto group : {s2_group_begin, static_cast<std::int32_t>(TECHNIQUE_STANDARD_BEGIN)})
				{
					const auto entry = this->asset_->stateBitsEntry[group + s2_base];
					if (entry == 0xFF || !this->asset_->stateBitsTable)
					{
						continue;
					}

					const auto technique = this->asset_->techniqueSet ? this->asset_->techniqueSet->techniques[group + s2_base] : nullptr;
					const auto pass_count = technique ? std::max<std::uint32_t>(technique->hdr.passCount, 1) : 1;
					const auto index = entry + std::min(pass, pass_count - 1);
					if (index < this->asset_->stateBitsCount)
					{
						return convert_load_bits(this->asset_->stateBitsTable[index]);
					}
				}

				return {};
			}

			std::optional<load_bits> lit_state(const std::int32_t s2_group_begin, const std::uint32_t pass) const
			{
				for (auto base = static_cast<std::int32_t>(TECHNIQUE_LIT); base < TECHNIQUE_LIT_END; base++)
				{
					if (const auto state = this->source_state(s2_group_begin, base, pass))
					{
						return make_lit_state(*state);
					}
				}

				for (const auto base : {static_cast<std::int32_t>(TECHNIQUE_EMISSIVE), static_cast<std::int32_t>(TECHNIQUE_UNLIT)})
				{
					if (const auto state = this->source_state(s2_group_begin, base, pass))
					{
						return *state;
					}
				}

				return {};
			}

			std::optional<load_bits> state_for(const std::int32_t h1_slot, const std::uint32_t pass) const
			{
				const auto h1_group = h1_slot / zonetool::h1::TECHNIQUE_INSTANCED_BEGIN * zonetool::h1::TECHNIQUE_INSTANCED_BEGIN;
				const auto h1_base = h1_slot - h1_group;
				const auto group = techset::s2_group(h1_group);

				if (techset::is_light_pass(h1_base))
				{
					const auto lit = this->lit_state(group, pass);
					return lit ? std::optional(make_light_state(*lit)) : std::nullopt;
				}

				if (techset::uses_lit_state(h1_base))
				{
					return this->lit_state(group, pass);
				}

				if (const auto s2_base = techset::s2_base_slot(h1_base); s2_base >= 0)
				{
					if (const auto state = this->source_state(group, s2_base, pass))
					{
						return *state;
					}
				}

				return this->lit_state(group, pass);
			}

		private:
			const Material* asset_;
		};

		std::vector<std::uint32_t> collect_arg_hashes(const zonetool::h1::MaterialTechniqueSet* techset, const std::initializer_list<std::uint8_t> types)
		{
			std::set<std::uint32_t> hashes;
			for (const auto technique : techset->techniques)
			{
				for (auto pass = 0; technique && pass < technique->hdr.passCount; pass++)
				{
					const auto& data = technique->passArray[pass];
					for (auto i = 0; i < data.perPrimArgCount + data.perObjArgCount + data.stableArgCount; i++)
					{
						if (std::ranges::find(types, data.args[i].type) != types.end())
						{
							hashes.insert(data.args[i].u.nameHash);
						}
					}
				}
			}

			return {hashes.begin(), hashes.end()};
		}

		buffer_layout make_buffer_layout(const zonetool::h1::MaterialTechnique* technique, const std::vector<std::uint32_t>& constants)
		{
			buffer_layout layout{};
			for (auto* stage : {&layout.vs, &layout.ps})
			{
				stage->registers.assign(constants.size(), 0xFFFF);
			}

			for (auto pass = 0; pass < technique->hdr.passCount; pass++)
			{
				const auto& data = technique->passArray[pass];
				for (auto i = 0; i < data.perPrimArgCount + data.perObjArgCount + data.stableArgCount; i++)
				{
					const auto& arg = data.args[i];
					if (arg.type != zonetool::h1::MTL_ARG_MATERIAL_CONST)
					{
						continue;
					}

					const auto index = std::ranges::lower_bound(constants, arg.u.nameHash) - constants.begin();
					for (const auto& [bit, stage] : {std::pair{2, &layout.vs}, std::pair{16, &layout.ps}})
					{
						if (arg.shader & bit)
						{
							stage->registers[index] = arg.dest;
							stage->size = std::max<std::uint16_t>(stage->size, static_cast<std::uint16_t>((arg.dest + 1) * 16));
						}
					}
				}
			}

			return layout;
		}

		void fill_stage(const stage_layout& layout, const zonetool::h1::MaterialConstantDef* constants, unsigned int& data_size, unsigned char*& data,
			unsigned int& offset_size, unsigned short*& offsets, utils::memory::allocator& allocator)
		{
			if (!layout.size)
			{
				return;
			}

			data_size = layout.size;
			data = allocator.allocate_array<unsigned char>(layout.size);
			offset_size = static_cast<unsigned int>(layout.registers.size());
			offsets = allocator.allocate_array<unsigned short>(layout.registers.size());
			for (auto i = 0u; i < layout.registers.size(); i++)
			{
				const auto offset = layout.registers[i] == 0xFFFF ? 0xFFFF : layout.registers[i] * 16;
				offsets[i] = static_cast<unsigned short>(offset);
				if (offset != 0xFFFF)
				{
					std::memcpy(data + offset, constants[i].literal, sizeof(constants[i].literal));
				}
			}
		}

		void convert_constant_buffers(const zonetool::h1::MaterialTechniqueSet* techset, const std::vector<std::uint32_t>& constants,
			zonetool::h1::Material* material, utils::memory::allocator& allocator)
		{
			std::ranges::fill(material->constantBufferIndex, 0xFF);
			if (constants.empty())
			{
				return;
			}

			std::vector<buffer_layout> layouts;
			for (auto slot = 0; slot < zonetool::h1::TECHNIQUE_COUNT; slot++)
			{
				const auto technique = techset->techniques[slot];
				if (!technique)
				{
					continue;
				}

				const auto layout = make_buffer_layout(technique, constants);
				if (!layout.vs.size && !layout.ps.size)
				{
					continue;
				}

				auto entry = std::ranges::find(layouts, layout);
				if (entry == layouts.end())
				{
					layouts.emplace_back(layout);
					entry = layouts.end() - 1;
				}

				material->constantBufferIndex[slot] = static_cast<unsigned char>(entry - layouts.begin());
			}

			material->constantBufferCount = static_cast<unsigned char>(layouts.size());
			material->constantBufferTable = allocator.allocate_array<zonetool::h1::MaterialConstantBufferDef>(layouts.size());
			for (auto i = 0u; i < layouts.size(); i++)
			{
				auto& buffer = material->constantBufferTable[i];
				fill_stage(layouts[i].vs, material->constantTable, buffer.vsDataSize, buffer.vsData, buffer.vsOffsetDataSize, buffer.vsOffsetData, allocator);
				fill_stage(layouts[i].ps, material->constantTable, buffer.psDataSize, buffer.psData, buffer.psOffsetDataSize, buffer.psOffsetData, allocator);
			}
		}

		std::vector<load_bits> convert_states(const Material* asset, const zonetool::h1::MaterialTechniqueSet* techset, zonetool::h1::Material* material,
			conversion_stats& stats)
		{
			const state_converter converter(asset);
			std::vector<load_bits> states;
			std::map<std::vector<load_bits>, std::uint8_t> runs;

			std::ranges::fill(material->stateBitsEntry, 0xFF);
			for (auto slot = 0; slot < zonetool::h1::TECHNIQUE_COUNT; slot++)
			{
				const auto technique = techset->techniques[slot];
				if (!technique)
				{
					continue;
				}

				const auto pass_count = std::max<std::uint32_t>(technique->hdr.passCount, 1);
				stats.multi_pass_techniques += pass_count > 1;

				std::vector<load_bits> run;
				for (auto pass = 0u; pass < pass_count; pass++)
				{
					const auto state = converter.state_for(slot, pass);
					if (!state)
					{
						break;
					}

					run.emplace_back(finish_h1_state(*state, slot % zonetool::h1::TECHNIQUE_INSTANCED_BEGIN, techset));
				}

				if (run.size() != pass_count)
				{
					stats.missing_states++;
					ZONETOOL_WARNING("Material \"%s\" has no S2 state for technique \"%s\" of \"%s\"", asset->name, technique->hdr.name, techset->name);
					continue;
				}

				const auto entry = runs.find(run);
				if (entry != runs.end())
				{
					material->stateBitsEntry[slot] = entry->second;
					continue;
				}

				const auto index = static_cast<std::uint8_t>(states.size());
				states.insert(states.end(), run.begin(), run.end());
				runs.emplace(run, index);
				material->stateBitsEntry[slot] = index;
			}

			return states;
		}

		std::string fallback_image(const std::uint8_t semantic)
		{
			switch (semantic)
			{
			case zonetool::h1::TS_NORMAL_MAP:
				return "$identitynormalmap";
			case zonetool::h1::TS_SPECULAR_MAP:
				return "$black";
			default:
				return "$white";
			}
		}

		bool is_sky(const Material* asset)
		{
			return get_techset_name(asset).find("_sky") != std::string::npos;
		}

		bool is_ui3d_screen(const Material* asset)
		{
			return get_techset_name(asset).find("_ui3d") != std::string::npos;
		}

		std::string select_image(const Material* asset, const MaterialTextureDef& texture, const std::uint8_t semantic, const bool native, conversion_stats& stats)
		{
			const auto image = texture.u.image ? texture.u.image->name : nullptr;
			if (is_sky(asset) && semantic == zonetool::h1::TS_COLOR_MAP && image)
			{
				const auto rotation = find_constant(asset, 0xBC8EB53C);
				const auto rotation_degrees = rotation ? static_cast<int>(std::lround(rotation->literal[2])) : 0;
				const auto cube = gfximage::dump_sky_cube(texture.u.image, rotation ? rotation->literal[0] : 1.0f, rotation ? rotation->literal[1] : 0.0f, rotation_degrees);
				if (!native)
				{
					return cube;
				}
			}

			if (is_ui3d_screen(asset) && semantic == zonetool::h1::TS_COLOR_MAP)
			{
				return "$black";
			}

			if (image && native)
			{
				if (const auto native_image = gfximage::dump_native_used(image))
				{
					stats.source_images++;
					return *native_image;
				}
			}

			if (image && (gfximage::is_dumped(image) || techset::is_global_image(image) || gfximage::dump_used(image)))
			{
				stats.source_images++;
				return image;
			}

			if (image)
			{
				stats.missing_images.insert(image);
			}

			stats.fallback_images++;
			return fallback_image(semantic);
		}

		zonetool::h1::MaterialTextureDef make_missing_texture(const std::uint32_t hash, const unsigned char sampler_state)
		{
			zonetool::h1::MaterialTextureDef texture{};
			texture.nameHash = hash;
			texture.samplerState = sampler_state;
			texture.semantic = zonetool::h1::TS_2D;
			for (const auto& known : default_textures)
			{
				if (texture_name_hash(known.name) == hash)
				{
					texture.nameStart = known.name.front();
					texture.nameEnd = known.name.back();
					texture.semantic = known.semantic;
				}
			}

			return texture;
		}

		std::string missing_texture_image(const zonetool::h1::MaterialTextureDef& texture)
		{
			for (const auto& known : default_textures)
			{
				if (texture_name_hash(known.name) == texture.nameHash)
				{
					return std::string(known.image);
				}
			}

			return "$white";
		}

		void convert_textures(const Material* asset, const std::vector<std::uint32_t>& hashes, const bool native, zonetool::h1::Material* material,
			std::vector<std::string>& image_names, conversion_stats& stats, utils::memory::allocator& allocator)
		{
			const auto color_map = find_texture(asset, texture_name_hash("colorMap"));
			const auto default_sampler = color_map ? color_map->samplerState : static_cast<unsigned char>(0xE2);

			material->textureCount = static_cast<unsigned char>(hashes.size());
			material->textureTable = allocator.allocate_array<zonetool::h1::MaterialTextureDef>(hashes.size());
			image_names.resize(hashes.size());
			for (auto i = 0u; i < hashes.size(); i++)
			{
				auto& texture = material->textureTable[i];
				if (const auto source = find_texture(asset, hashes[i]))
				{
					texture.nameHash = source->nameHash;
					texture.nameStart = source->nameStart;
					texture.nameEnd = source->nameEnd;
					texture.samplerState = source->samplerState;
					texture.semantic = source->semantic == zonetool::h1::TS_WATER_MAP ? zonetool::h1::TS_2D : source->semantic;
					image_names[i] = select_image(asset, *source, texture.semantic, native, stats);
				}
				else
				{
					texture = make_missing_texture(hashes[i], default_sampler);
					image_names[i] = missing_texture_image(texture);
					stats.missing_textures++;
				}

				texture.u.image = allocator.allocate<zonetool::h1::GfxImage>();
				texture.u.image->name = allocator.duplicate_string(image_names[i]);
			}
		}

		struct stock_constant
		{
			std::uint32_t hash;
			const char* name;
			std::array<float, 4> literal;
		};

		const std::vector<stock_constant> stock_ocean_constants // stock h1 wc/cli_lagoon_water_01
		{
			{0x3D9994DC, "envMapParms", {0.1f, 1.0f, 5.0f, 1.0f}},
			{0x3ECE14FE, "oceanSpecPar", {0.462077f, 0.462077f, 0.462077f, 0.953125f}},
			{0x470F6C9A, "oceanUVAnimP", {0.00006f, 0.00006f, 0.00005f, 0.0f}},
			{0x470F6C9B, "oceanUVAnimP", {0.00004f, 0.00004f, 0.0f, 0.00007f}},
			{0x6373ABA0, "oceanUVAnimP", {0.004f, 0.004f, -0.002868f, 0.004096f}},
			{0x6373ABA1, "oceanUVAnimP", {0.002f, 0.002f, 0.004226f, 0.009063f}},
			{0x9D5408FF, "oceanAmplitu", {1.0f, 1.0f, 5.0f, 512.0f}},
			{0xA3D2CA9B, "oceanSeconda", {0.0f, 0.0f, 0.0f, 0.0f}},
			{0xAA2E7C4F, "oceanMiscPar", {0.1f, 0.0f, 0.1f, 0.08f}},
			{0xB574A566, "oceanImageSi", {0.00390625f, 0.00390625f, 1.0f, 1.0f}},
			{0xC75417B1, "reflectionRa", {4096.0f, 1.0f, 0.0f, 0.0f}},
			{0xC7789F32, "oceanVtxDisp", {1.0f, 1.0f, 1.0f, 0.0f}},
			{0xC798E057, "oceanImageSi", {0.001953125f, 0.001953125f, 1.0f, 1.0f}},
			{0xC97690DE, "hdrColorParm", {0.25f, 0.0f, 0.0f, 0.0f}},
			{0xE8A62BF0, "depthBlendPa", {25.0f, 1.0f, 0.0f, 40.0f}},
		};

		const std::vector<stock_constant> fallback_constants // stock h1 mtl_usmc_ghillie_price_grass_mesh, zero sssTransmitt gives NaN
		{
			{0x5525080A, "transmittanc", {0.188f, 0.188f, 0.188f, 1.0f}},
			{0x886FAA58, "sssTransmitt", {1.0f, 0.5f, 0.8f, 0.3f}},
		};

		const stock_constant* find_stock_constant(const zonetool::h1::MaterialTechniqueSet* techset, const std::uint32_t hash)
		{
			if (!techset->name || !std::string_view(techset->name).starts_with("wc_ocean_") || std::string_view(techset->name).ends_with("_s2"))
			{
				return nullptr;
			}

			const auto entry = std::ranges::find(stock_ocean_constants, hash, &stock_constant::hash);
			return entry != stock_ocean_constants.end() ? &*entry : nullptr;
		}

		void convert_constants(const Material* asset, const std::vector<std::uint32_t>& hashes, zonetool::h1::Material* material,
			conversion_stats& stats, utils::memory::allocator& allocator)
		{
			material->constantCount = static_cast<unsigned char>(hashes.size());
			material->constantTable = allocator.allocate_array<zonetool::h1::MaterialConstantDef>(hashes.size());
			for (auto i = 0u; i < hashes.size(); i++)
			{
				auto& constant = material->constantTable[i];
				constant.nameHash = hashes[i];
				if (const auto stock = find_stock_constant(material->techniqueSet, hashes[i]))
				{
					std::strncpy(constant.name, stock->name, sizeof(constant.name));
					std::ranges::copy(stock->literal, constant.literal);
					continue;
				}

				if (const auto source = find_constant(asset, hashes[i]))
				{
					std::memcpy(constant.name, source->name, sizeof(constant.name));
					std::memcpy(constant.literal, source->literal, sizeof(constant.literal));
					continue;
				}

				if (const auto fallback = std::ranges::find(fallback_constants, hashes[i], &stock_constant::hash); fallback != fallback_constants.end())
				{
					std::strncpy(constant.name, fallback->name, sizeof(constant.name));
					std::ranges::copy(fallback->literal, constant.literal);
					continue;
				}

				stats.missing_constants++;
				ZONETOOL_WARNING("Material \"%s\": H1 constant 0x%08X has no S2 value", asset->name, hashes[i]);
			}
		}

		void dump_statebits_map(const std::string& techset, const std::string& material, const std::vector<load_bits>& states)
		{
			ordered_json data = ordered_json::array();
			for (const auto& state : states)
			{
				ordered_json entry;
				for (auto i = 0u; i < state.size(); i++)
				{
					entry["loadBits"][i] = state[i];
				}

				const auto depth_stencil = depth_stencil_states(state);
				for (auto i = 0u; i < depth_stencil.size(); i++)
				{
					entry["depthStencilStateBits"][i] = depth_stencil[i];
				}

				for (auto i = 0u; i < 3; i++)
				{
					entry["blendStateBits"][i] = state[3 + i];
				}

				entry["rasterizerState"] = rasterizer_state(state);
				data.push_back(entry);
			}

			write_file(std::format("techsets\\state\\{}\\{}.statebitsmap", techset, material), data.dump(4));
		}

		void convert_material(Material* asset, const zonetool::h1::MaterialTechniqueSet* techset, const bool native, conversion_stats& stats)
		{
			utils::memory::allocator allocator;
			const auto material = allocator.allocate<zonetool::h1::Material>();
			const auto state_owner = find_state_owner(asset);

			material->info.name = asset->name;
			material->info.gameFlags = asset->info.gameFlags;
			material->info.sortKey = convert_sort_key(asset->info.sortKey);
			material->info.textureAtlasRowCount = asset->info.textureAtlasRowCount;
			material->info.textureAtlasColumnCount = asset->info.textureAtlasColumnCount;
			material->info.textureAtlasFrameBlend = asset->info.textureAtlasFrameBlend;
			material->info.textureAtlasAsArray = asset->info.textureAtlasAsArray;
			material->info.renderFlags = static_cast<unsigned char>(asset->info.renderFlags & (MTL_RENDER_FLAG_BLUR_DISTORTION | MTL_RENDER_FLAG_DEPTH_HACK_DOF));
			material->info.surfaceTypeBits = static_cast<zonetool::h1::SurfaceTypeBits>(convert_surface_type_bits(asset->info.surfaceTypeBits));
			material->cameraRegion = convert_camera_region(asset->cameraRegion);
			material->materialType = convert_material_type(techset::h1_style_name(techset->name));
			material->techniqueSet = allocator.allocate<zonetool::h1::MaterialTechniqueSet>();
			material->techniqueSet->name = techset->name;
			material->layerCount = asset->layerCount;
			material->subMaterials = asset->subMaterials;

			std::vector<std::string> image_names;
			convert_textures(asset, collect_arg_hashes(techset, {zonetool::h1::MTL_ARG_MATERIAL_TEXTURE, zonetool::h1::MTL_ARG_MATERIAL_SAMPLER}), native,
				material, image_names, stats, allocator);

			const auto constants = collect_arg_hashes(techset, {zonetool::h1::MTL_ARG_MATERIAL_CONST});
			convert_constants(asset, constants, material, stats, allocator);
			convert_constant_buffers(techset, constants, material, allocator);

			auto states = convert_states(asset, techset, material, stats);
			if (!native)
			{
				use_stock_blend(states, techset->name);
			}

			const auto lit_entry = material->stateBitsEntry[zonetool::h1::TECHNIQUE_LIT];
			const auto first_entry = std::ranges::find_if(material->stateBitsEntry, [](const std::uint8_t entry) { return entry != 0xFF; });
			const auto main_entry = lit_entry != 0xFF ? lit_entry : first_entry != std::end(material->stateBitsEntry) ? *first_entry : 0xFF;
			material->stateFlags = convert_state_flags(state_owner->stateFlags, main_entry != 0xFF ? states[main_entry][0] : 0u);

			zonetool::h1::material::dump(material);

			std::string name = asset->name;
			std::ranges::replace(name, '*', '_');
			dump_statebits_map(techset->name, name, states);

			camera_regions[asset->name] = material->cameraRegion;
			add_csv_row("material", asset->name);
			for (const auto& image : image_names)
			{
				if (!gfximage::is_dumped(image))
				{
					add_csv_row("image", image, true);
				}
			}
		}

		std::unordered_map<std::string, std::string> select_techsets(const std::vector<Material*>& materials)
		{
			std::unordered_map<std::string, std::string> result;
			for (const auto asset : materials)
			{
				const auto s2_techset = get_techset_name(asset);
				if (!s2_techset.empty() && !result.contains(s2_techset))
				{
					result[s2_techset] = techset::select(s2_techset);
					ZONETOOL_INFO("techset %s -> %s", s2_techset.data(), result[s2_techset].data());
				}
			}

			return result;
		}

		std::vector<Material*> convert_native_materials(conversion_stats& stats)
		{
			std::vector<Material*> remaining;
			std::unordered_set<zonetool::h1::MaterialTechniqueSet*> dumped_techsets;
			for (const auto asset : pending_materials)
			{
				const auto native = techset::convert_native(asset->techniqueSet, srgb_textures(asset));
				if (!native)
				{
					remaining.emplace_back(asset);
					continue;
				}

				if (dumped_techsets.insert(native).second)
				{
					techset::dump_native(native);
					add_csv_row("techset", native->name);
				}

				convert_material(asset, native, true, stats);
			}

			return remaining;
		}

		void convert_zone_materials(const std::filesystem::path& zone_path, const std::vector<std::pair<Material*, std::string>>& materials,
			conversion_stats& stats)
		{
			ZONETOOL_INFO("Converting %zu materials with \"%s\"...", materials.size(), zone_path.stem().string().data());

			const techset::h1_zone zone(zone_path);
			std::unordered_set<std::string> dumped_techsets;

			for (const auto& [asset, h1_techset] : materials)
			{
				const auto techniques = zone.find_techset(h1_techset);
				if (!techniques)
				{
					ZONETOOL_WARNING("H1 techset \"%s\" is not in \"%s\"", h1_techset.data(), zone_path.stem().string().data());
					continue;
				}

				const auto source = techset::find_source(h1_techset);
				if (!source->global && dumped_techsets.insert(techniques->name).second)
				{
					techset::dump(techniques, zone_path);
				}

				add_csv_row("techset", techniques->name, source->global);
				convert_material(asset, techniques, false, stats);
			}
		}
	}

	void dump(Material* asset)
	{
		if (std::ranges::find(pending_materials, asset) == pending_materials.end())
		{
			pending_materials.emplace_back(asset);
		}
	}

	void finish()
	{
		if (pending_materials.empty())
		{
			return;
		}

		conversion_stats stats{};
		const auto stock_materials = convert_native_materials(stats);
		const auto selected = select_techsets(stock_materials);

		std::map<std::filesystem::path, std::vector<std::pair<Material*, std::string>>> materials_by_zone;
		for (const auto asset : stock_materials)
		{
			const auto s2_techset = get_techset_name(asset);
			if (s2_techset.empty())
			{
				ZONETOOL_WARNING("Material \"%s\" has no techset", asset->name);
				continue;
			}

			const auto& h1_techset = selected.at(s2_techset);
			const auto source = techset::find_source(h1_techset);
			if (!source)
			{
				ZONETOOL_WARNING("H1 techset \"%s\" for material \"%s\" is not found", h1_techset.data(), asset->name);
				continue;
			}

			materials_by_zone[source->zone].emplace_back(asset, h1_techset);
		}

		for (const auto& [zone, materials] : materials_by_zone)
		{
			convert_zone_materials(zone, materials, stats);
		}

		techset::dump_missing_shaders();

		ZONETOOL_INFO("Material images: %zu from source, %zu fallback, %zu H1 textures without an S2 texture", stats.source_images, stats.fallback_images,
			stats.missing_textures);
		ZONETOOL_INFO("Material constants: %zu H1 constants without an S2 value; states: %zu techniques without an S2 state, %zu multi pass techniques",
			stats.missing_constants, stats.missing_states, stats.multi_pass_techniques);

		for (const auto& image : stats.missing_images)
		{
			ZONETOOL_WARNING("Image \"%s\" is not in the source zone", image.data());
		}

		techset::finish_native();
		pending_materials.clear();
	}

	std::optional<std::uint8_t> find_camera_region(const std::string& name)
	{
		const auto region = camera_regions.find(name);
		if (region == camera_regions.end())
		{
			return {};
		}

		return region->second;
	}
}
