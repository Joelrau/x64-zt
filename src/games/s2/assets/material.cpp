#include <std_include.hpp>
#include "s2.hpp"
#include "json_fields.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_constant_buffer(zone_reader& reader, MaterialConstantBufferDef* buffer)
		{
			reader.read_array(buffer->vsData, 15, buffer->vsDataSize);
			reader.read_array(buffer->hsData, 15, buffer->hsDataSize);
			reader.read_array(buffer->dsData, 15, buffer->dsDataSize);
			reader.read_array(buffer->psData, 15, buffer->psDataSize);
			reader.read_array(buffer->vsOffsetData, 1, buffer->vsOffsetDataSize);
			reader.read_array(buffer->hsOffsetData, 1, buffer->hsOffsetDataSize);
			reader.read_array(buffer->dsOffsetData, 1, buffer->dsOffsetDataSize);
			reader.read_array(buffer->psOffsetData, 1, buffer->psOffsetDataSize);
		}
	}

	void material::read(zone_reader& reader, Material* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_TECHNIQUE_SET, asset->techniqueSet);

		if (const auto textures = reader.read_array(asset->textureTable, 7, asset->textureCount))
		{
			for (auto i = 0; i < asset->textureCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, textures[i].u.image);
			}
		}

		reader.read_array(asset->constantTable, 15, asset->constantCount);
		if (const auto state_bits = reader.read_array(asset->stateBitsTable, 3, asset->stateBitsCount))
		{
			for (auto i = 0; i < asset->stateBitsCount; i++)
			{
				state_bits[i].zone = reader.zone_index();
			}
		}
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->parentMaterial);

		if (const auto buffers = reader.read_array(asset->constantBufferTable, 7, asset->constantBufferCount))
		{
			for (auto i = 0; i < asset->constantBufferCount; i++)
			{
				read_constant_buffer(reader, &buffers[i]);
			}
		}

		if (const auto sub_materials = reader.read_array(asset->subMaterials, 7, asset->layerCount))
		{
			for (auto i = 0; i < asset->layerCount; i++)
			{
				reader.read_string(sub_materials[i]);
			}
		}

		reader.pop_stream();
	}

	namespace
	{
		template <typename T>
		void write_inline_array(zone_buffer* buf, T*& dest_field, T* source, const std::size_t alignment, const std::size_t count)
		{
			if (!source)
			{
				return;
			}

			buf->align(alignment);
			buf->write(source, count);
			buf->clear_pointer(&dest_field);
		}

		void write_constant_buffer(zone_buffer* buf, const MaterialConstantBufferDef& source, MaterialConstantBufferDef& dest)
		{
			write_inline_array(buf, dest.vsData, source.vsData, 15, source.vsDataSize);
			write_inline_array(buf, dest.hsData, source.hsData, 15, source.hsDataSize);
			write_inline_array(buf, dest.dsData, source.dsData, 15, source.dsDataSize);
			write_inline_array(buf, dest.psData, source.psData, 15, source.psDataSize);
			write_inline_array(buf, dest.vsOffsetData, source.vsOffsetData, 1, source.vsOffsetDataSize);
			write_inline_array(buf, dest.hsOffsetData, source.hsOffsetData, 1, source.hsOffsetDataSize);
			write_inline_array(buf, dest.dsOffsetData, source.dsOffsetData, 1, source.dsOffsetDataSize);
			write_inline_array(buf, dest.psOffsetData, source.psOffsetData, 1, source.psOffsetDataSize);
		}

		void parse_constant_buffer(assetmanager::reader& read, MaterialConstantBufferDef& buffer)
		{
			buffer.vsData = read.read_array<unsigned char>();
			buffer.hsData = read.read_array<unsigned char>();
			buffer.dsData = read.read_array<unsigned char>();
			buffer.psData = read.read_array<unsigned char>();
			buffer.vsOffsetData = read.read_array<unsigned short>();
			buffer.hsOffsetData = read.read_array<unsigned short>();
			buffer.dsOffsetData = read.read_array<unsigned short>();
			buffer.psOffsetData = read.read_array<unsigned short>();
		}

		std::vector<material_state> get_states(const Material* asset)
		{
			std::vector<material_state> states(asset->stateBitsTable ? asset->stateBitsCount : 0);
			for (auto i = 0u; i < states.size(); i++)
			{
				const auto& state_bits = asset->stateBitsTable[i];
				const auto* globals = get_x_gfx_globals_for_zone<XGfxGlobals>(state_bits.zone);
				if (!globals)
				{
					ZONETOOL_WARNING("material \"%s\" has no gfx globals for its state bits", asset->name);
					continue;
				}

				for (auto j = 0u; j < std::size(state_bits.depthStencilState); j++)
				{
					const auto index = state_bits.depthStencilState[j];
					states[i].depth_stencil[j] = index < globals->depthStencilStateCount ? globals->depthStencilStateBits[index] : 0;
				}

				if (state_bits.blendState < globals->blendStateCount)
				{
					std::copy_n(globals->blendStateBits[state_bits.blendState], 3, states[i].blend);
				}
			}

			return states;
		}

		std::string json_path(const std::string& name)
		{
			return dump_path(ASSET_TYPE_MATERIAL, name.data(), ".json");
		}

#define MATERIAL_FIELD(field) #field, value.field
		template <typename J>
		void visit_texture_json(const J& j, MaterialTextureDef& value)
		{
			j.asset("image", value.u.image);
			j.value(MATERIAL_FIELD(nameHash));
			j.value(MATERIAL_FIELD(nameStart));
			j.value(MATERIAL_FIELD(nameEnd));
			j.value(MATERIAL_FIELD(samplerState));
			j.value(MATERIAL_FIELD(semantic));
		}

		template <typename J>
		void visit_constant_json(const J& j, MaterialConstantDef& value)
		{
			j.fixed_string(MATERIAL_FIELD(name));
			j.value(MATERIAL_FIELD(nameHash));
			j.value(MATERIAL_FIELD(literal));
		}

		template <typename J>
		void visit_state_bits_json(const J& j, state_bits_entry& value)
		{
			j.value("loadBits", value.bits.loadBits);
			j.value("rasterizerState", value.bits.rasterizerState);
			j.value("depthStencilState", value.state.depth_stencil);
			j.value("blendState", value.state.blend);
		}

		template <typename J>
		void visit_constant_buffer_json(const J& j, MaterialConstantBufferDef& value)
		{
			j.array_list(MATERIAL_FIELD(vsData), value.vsDataSize);
			j.array_list(MATERIAL_FIELD(hsData), value.hsDataSize);
			j.array_list(MATERIAL_FIELD(dsData), value.dsDataSize);
			j.array_list(MATERIAL_FIELD(psData), value.psDataSize);
			j.array_list(MATERIAL_FIELD(vsOffsetData), value.vsOffsetDataSize);
			j.array_list(MATERIAL_FIELD(hsOffsetData), value.hsOffsetDataSize);
			j.array_list(MATERIAL_FIELD(dsOffsetData), value.dsOffsetDataSize);
			j.array_list(MATERIAL_FIELD(psOffsetData), value.psOffsetDataSize);
		}

		template <typename J>
		void visit_material_json(const J& j, Material& value)
		{
			j.string(MATERIAL_FIELD(name));
			j.asset(MATERIAL_FIELD(techniqueSet));
			j.asset(MATERIAL_FIELD(parentMaterial));

			j.value("gameFlags", value.info.gameFlags);
			j.value("unkFlags", value.info.unkFlags);
			j.value("sortKey", value.info.sortKey);
			j.value("textureAtlasRowCount", value.info.textureAtlasRowCount);
			j.value("textureAtlasColumnCount", value.info.textureAtlasColumnCount);
			j.value("textureAtlasFrameBlend", value.info.textureAtlasFrameBlend);
			j.value("textureAtlasAsArray", value.info.textureAtlasAsArray);
			j.value("surfaceTypeBits", value.info.surfaceTypeBits);

			j.value(MATERIAL_FIELD(stateFlags));
			j.value(MATERIAL_FIELD(cameraRegion));
			j.value(MATERIAL_FIELD(materialType));
			j.value(MATERIAL_FIELD(assetFlags));
			j.value(MATERIAL_FIELD(stateBitsEntry));
			j.value(MATERIAL_FIELD(constantBufferIndex));

			j.entries(MATERIAL_FIELD(textureTable), value.textureCount, visit_texture_json<J>);
			j.entries(MATERIAL_FIELD(constantTable), value.constantCount, visit_constant_json<J>);
			j.entries(MATERIAL_FIELD(constantBufferTable), value.constantBufferCount, visit_constant_buffer_json<J>);

			j.value(MATERIAL_FIELD(layerCount));
			j.string_array(MATERIAL_FIELD(subMaterials), value.layerCount);
		}
#undef MATERIAL_FIELD
	}

	Material* material::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file(json_path(name));
		if (file.exists())
		{
			ZONETOOL_INFO("Parsing material \"%s\"...", name.data());

			file.open("rb");
			const auto data = json::parse(file.read_bytes(file.size()));
			file.close();

			auto* asset = mem->allocate<Material>();
			const json_field_parser j(data, mem);
			visit_material_json(j, *asset);

			state_bits_entry* entries{};
			j.entries("stateBitsTable", entries, asset->stateBitsCount, visit_state_bits_json<json_field_parser>);
			if (entries)
			{
				const auto count = std::max<std::size_t>(asset->stateBitsCount, 1);
				asset->stateBitsTable = mem->allocate<GfxStateBits>(count);
				this->states_ = mem->allocate<material_state>(count);
				for (auto i = 0u; i < asset->stateBitsCount; i++)
				{
					asset->stateBitsTable[i] = entries[i].bits;
					this->states_[i] = entries[i].state;
				}
			}

			return asset;
		}

		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_MATERIAL, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing material \"%s\"...", name.data());

		auto* asset = read.read_single<Material>();
		asset->name = read.read_string();
		asset->techniqueSet = read.read_asset<MaterialTechniqueSet>();

		asset->textureTable = read.read_array<MaterialTextureDef>();
		for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
		{
			asset->textureTable[i].u.image = read.read_asset<GfxImage>();
		}

		asset->constantTable = read.read_array<MaterialConstantDef>();
		asset->stateBitsTable = read.read_array<GfxStateBits>();
		this->states_ = asset->stateBitsTable ? read.read_array<material_state>() : nullptr;
		asset->parentMaterial = read.read_asset<Material>();

		asset->constantBufferTable = read.read_array<MaterialConstantBufferDef>();
		for (auto i = 0; asset->constantBufferTable && i < asset->constantBufferCount; i++)
		{
			parse_constant_buffer(read, asset->constantBufferTable[i]);
		}

		if (asset->subMaterials)
		{
			asset->subMaterials = mem->allocate<const char*>(asset->layerCount);
			for (auto i = 0; i < asset->layerCount; i++)
			{
				asset->subMaterials[i] = read.read_string();
			}
		}
		read.close();

		return asset;
	}

	void material::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		if (!data->stateBitsTable)
		{
			return;
		}

		const auto runtime_states = this->states_ ? std::vector<material_state>{} : get_states(data);
		const auto* states = this->states_ ? this->states_ : runtime_states.data();
		for (auto i = 0; i < data->stateBitsCount; i++)
		{
			auto& state_bits = data->stateBitsTable[i];
			for (auto j = 0u; j < std::size(state_bits.depthStencilState); j++)
			{
				state_bits.depthStencilState[j] = buf->write_depthstencilstatebit(states[i].depth_stencil[j]);
			}

			state_bits.blendState = buf->write_blendstatebits({states[i].blend[0], states[i].blend[1], states[i].blend[2], 0});
		}
	}

	void material::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		add_dependency(zone, ASSET_TYPE_TECHNIQUE_SET, data->techniqueSet);
		for (auto i = 0; data->textureTable && i < data->textureCount; i++)
		{
			add_dependency(zone, ASSET_TYPE_IMAGE, data->textureTable[i].u.image);
		}
		add_dependency(zone, ASSET_TYPE_MATERIAL, data->parentMaterial);
	}

	void material::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);
		dest->techniqueSet = asset_reference(zone, ASSET_TYPE_TECHNIQUE_SET, data->techniqueSet);

		if (data->textureTable)
		{
			buf->align(7);
			auto* textures = buf->write(data->textureTable, data->textureCount);
			buf->clear_pointer(&dest->textureTable);
			for (auto i = 0; i < data->textureCount; i++)
			{
				textures[i].u.image = asset_reference(zone, ASSET_TYPE_IMAGE, data->textureTable[i].u.image);
			}
		}

		write_inline_array(buf, dest->constantTable, data->constantTable, 15, data->constantCount);
		write_inline_array(buf, dest->stateBitsTable, data->stateBitsTable, 3, data->stateBitsCount);
		dest->parentMaterial = asset_reference(zone, ASSET_TYPE_MATERIAL, data->parentMaterial);

		if (data->constantBufferTable)
		{
			buf->align(7);
			auto* buffers = buf->write(data->constantBufferTable, data->constantBufferCount);
			buf->clear_pointer(&dest->constantBufferTable);
			for (auto i = 0; i < data->constantBufferCount; i++)
			{
				write_constant_buffer(buf, data->constantBufferTable[i], buffers[i]);
			}
		}

		if (data->subMaterials)
		{
			buf->align(7);
			auto* sub_materials = buf->write(data->subMaterials, data->layerCount);
			buf->clear_pointer(&dest->subMaterials);
			for (auto i = 0; i < data->layerCount; i++)
			{
				sub_materials[i] = data->subMaterials[i] ? buf->write_str(data->subMaterials[i]) : nullptr;
			}
		}

		buf->pop_stream();
	}

	void material::dump(Material* asset, const material_state* states)
	{
		ordered_json data;
		const json_field_dumper j(data);
		visit_material_json(j, *asset);

		if (asset->stateBitsTable)
		{
			const auto resolved_states = states
				? std::vector<material_state>(states, states + asset->stateBitsCount)
				: get_states(asset);

			std::vector<state_bits_entry> entries(std::max<std::size_t>(asset->stateBitsCount, 1)); // keeps an empty table non-null
			for (auto i = 0u; i < asset->stateBitsCount; i++)
			{
				entries[i].bits = asset->stateBitsTable[i];
				entries[i].state = resolved_states[i];
			}

			j.entries("stateBitsTable", entries.data(), asset->stateBitsCount, visit_state_bits_json<json_field_dumper>);
		}
		else
		{
			data["stateBitsTable"] = nullptr;
		}

		const auto str = data.dump(4);

		auto file = filesystem::file(json_path(asset->name));
		file.open("wb");
		file.write(str.data(), str.size(), 1);
		file.close();
	}
}
