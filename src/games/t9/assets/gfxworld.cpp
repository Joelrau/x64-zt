#include <std_include.hpp>
#include "gfxworld.hpp"
#include "clipmap.hpp"
#include "fx.hpp"
#include "../xpak.hpp"

namespace zonetool::t9
{
	namespace
	{
		template <typename T>
		T* read_list(auto& reader, GfxList<T>& list, const std::size_t alignment)
		{
			return reader.read_array(list.values, alignment, static_cast<std::size_t>(list.count));
		}

		template <typename T>
		T* read_list(auto& reader, GfxArray<T>& list, const std::size_t alignment)
		{
			return reader.read_array(list.values, alignment, list.count);
		}

		template <typename T>
		void read_runtime_list(auto& reader, T& list, const std::size_t alignment)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME_VIRTUAL);
			read_list(reader, list, alignment);
			reader.pop_stream();
		}

		void read_buffer(auto& reader, GfxBuffer*& field)
		{
			if (const auto buffer = reader.read_array(field, 15))
			{
				read_gfx_buffer(reader, buffer);
			}
		}

		void read_vertex_data(auto& reader, GfxWorldVertexData& data)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, data.image);
			reader.read_array(data.positions, 15, data.vertexCount);
			reader.read_array(data.vertices, 15, data.vertexCount);
			reader.read_array(data.indices, 1, data.indexCount);
		}

		void read_data_table(auto& reader, GfxWorldDataTable& table)
		{
			read_list(reader, table.data, 0);
			read_list(reader, table.values1, 3);
			read_list(reader, table.values2, 3);
		}

		void read_model_group(auto& reader, GfxWorldModelGroup& group)
		{
			read_list(reader, group.values1, 3);
			read_list(reader, group.values2, 3);
			read_clip_map_models(reader, group.models.values, static_cast<std::size_t>(group.models.count));
			reader.read_asset_array(ASSET_TYPE_XMODEL, group.xmodels.values, static_cast<std::size_t>(group.xmodels.count));
		}

		void read_model_instance(auto& reader, GfxWorldModelInstance& instance)
		{
			reader.read_asset(ASSET_TYPE_XMODEL, instance.model1);
			reader.read_asset(ASSET_TYPE_XMODEL, instance.model2);

			if (const auto lists = read_list(reader, instance.materialLists, 7))
			{
				for (auto i = 0ull; i < instance.materialLists.count; i++)
				{
					auto& list = lists[i];
					reader.read_asset_array(ASSET_TYPE_MATERIAL, list.materials1.values, static_cast<std::size_t>(list.materials1.count));
					reader.read_asset_array(ASSET_TYPE_MATERIAL, list.materials2.values, static_cast<std::size_t>(list.materials2.count));
				}
			}

			read_list(reader, instance.values, 3);
			read_runtime_list(reader, instance.runtimeData1, 63);
			read_runtime_list(reader, instance.runtimeData2, 63);
		}

		void read_model_data(auto& reader, GfxWorldModelData* data)
		{
			if (const auto instances = read_list(reader, data->instances, 7))
			{
				for (auto i = 0ull; i < data->instances.count; i++)
				{
					read_model_instance(reader, instances[i]);
				}
			}

			if (const auto images = read_list(reader, data->images, 7))
			{
				for (auto i = 0ull; i < data->images.count; i++)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, images[i].image);
					read_list(reader, images[i].values, 3);
				}
			}

			if (const auto effects = read_list(reader, data->effects, 7))
			{
				for (auto i = 0ull; i < data->effects.count; i++)
				{
					read_list(reader, effects[i].values1, 3);
					reader.read_asset(ASSET_TYPE_FX, effects[i].effect);
					read_list(reader, effects[i].values2, 3);
				}
			}

			for (auto& buffer : data->buffers)
			{
				read_buffer(reader, buffer);
			}

			read_list(reader, data->indices, 1);
		}

		void read_draw_data(auto& reader, GfxWorldDrawData& draw, const GfxWorld* world)
		{
			for (auto& buffers : draw.runtimeBuffers)
			{
				for (auto& buffer : buffers)
				{
					read_runtime_list(reader, buffer, 15);
				}
			}

			for (auto& table : draw.valueTables)
			{
				read_list(reader, table.values, 3);
			}

			for (auto& table : draw.dataTables)
			{
				read_data_table(reader, table);
			}

			reader.read_array(draw.cellValues, 1, draw.cellCount);
			reader.read_array(draw.values1, 3, draw.values1Count);
			reader.read_array(draw.values2, 3, draw.values2Count);

			const auto surface_count = static_cast<std::size_t>(world->surfaceCount);
			if (const auto surfaces = reader.read_array(draw.surfaces, 15, surface_count))
			{
				for (auto i = 0u; i < surface_count; i++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, surfaces[i].material);
				}
			}

			read_model_group(reader, draw.modelGroup);
			read_list(reader, draw.values4, 3);

			reader.push_stream(XFILE_BLOCK_RUNTIME_VIRTUAL);
			reader.read_array(draw.runtimeCells, 7, draw.cellCount);
			reader.pop_stream();

			reader.read_array(draw.values3, 3, draw.values3Count);
			read_list(reader, draw.data1, 0);
			read_list(reader, draw.data2, 0);

			for (auto& buffer : draw.buffers)
			{
				read_buffer(reader, buffer);
			}

			if (const auto data = reader.read_array(draw.modelData, 7))
			{
				read_model_data(reader, data);
			}

			if (const auto groups = read_list(reader, draw.lodGroups, 7))
			{
				for (auto i = 0ull; i < draw.lodGroups.count; i++)
				{
					reader.read_asset(ASSET_TYPE_GROUPLODMODEL, groups[i].model);
					read_list(reader, groups[i].data, 0);
				}
			}

			reader.read_asset_array(ASSET_TYPE_GROUPLODMODEL, draw.lodModels.values, static_cast<std::size_t>(draw.lodModels.count));
			read_list(reader, draw.data3, 0);

			if (const auto named = read_list(reader, draw.namedValues, 7))
			{
				for (auto i = 0ull; i < draw.namedValues.count; i++)
				{
					read_list(reader, named[i].values, 3);
				}
			}
		}

		void read_runtime_buffers(auto& reader, GfxWorldRuntimeBuffers& buffers)
		{
			for (auto& buffer : buffers.buffers)
			{
				read_runtime_list(reader, buffer, 15);
			}

			read_runtime_list(reader, buffers.buffer, 15);
		}

		void read_volume(auto& reader, GfxWorldVolume& volume)
		{
			reader.read_array(volume.values1, 3, volume.values1Count);

			if (const auto lists = reader.read_array(volume.probeLists, 7, volume.probeListCount))
			{
				for (auto i = 0u; i < volume.probeListCount; i++)
				{
					reader.read_array(lists[i].values, 3, lists[i].count);
				}
			}

			if (const auto lists = reader.read_array(volume.probeIndexLists, 7, volume.probeIndexListCount))
			{
				for (auto i = 0u; i < volume.probeIndexListCount; i++)
				{
					reader.read_array(lists[i].values, 3, lists[i].count);
				}
			}

			reader.read_asset(ASSET_TYPE_MATERIAL, volume.material);
		}

		void read_volumes(auto& reader, GfxWorldVolumeList& volumes)
		{
			if (const auto entries = reader.read_array(volumes.entries, 7, volumes.entryCount))
			{
				for (auto i = 0u; i < volumes.entryCount; i++)
				{
					read_volume(reader, entries[i].volume);
					reader.read_array(entries[i].values, 7, entries[i].valueCount);
				}
			}

			read_runtime_list(reader, volumes.runtimeData, 15);
		}

		void read_runtime_grid(auto& reader, GfxWorldRuntimeGrid& grid)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME_PHYSICAL);
			reader.read_array(grid.values, 15, grid.valueCount);
			reader.pop_stream();

			reader.push_stream(XFILE_BLOCK_RUNTIME_PHYSICAL);
			reader.read_array(grid.cells, 255, grid.cellCount * 8ull + 8);
			reader.pop_stream();
		}

		void read_cell_data(auto& reader, GfxWorldCellData& data)
		{
			reader.read_array(data.values1, 1, data.values1Count);
			reader.read_array(data.values2, 3, data.values2Count);
			reader.read_array(data.values3, 3, data.values3Count);
			reader.read_array(data.values4, 3, data.values4Count);
			reader.read_array(data.values5, 3, data.values5Count);

			for (auto i = 0u; i < 4; i++)
			{
				reader.read_array(data.blocks[i], 0, data.blockCounts[i]);
			}
		}

		void read_model_ref(auto& reader, GfxWorldModelRef& ref)
		{
			if (ref.data1)
			{
				reader.resolve_pointer(ref.data1);
			}

			reader.read_asset(ASSET_TYPE_XMODEL, ref.model1);

			if (ref.data2)
			{
				reader.resolve_pointer(ref.data2);
			}

			reader.read_asset(ASSET_TYPE_XMODEL, ref.model2);
		}
	}

	void gfx_world::read(auto& reader, GfxWorld* asset)
	{
		reader.read_string(asset->baseName);
		read_list(reader, asset->values1, 3);
		read_list(reader, asset->values2, 3);
		read_vertex_data(reader, asset->vertexData);
		read_list(reader, asset->values3, 15);

		reader.push_stream(XFILE_BLOCK_RUNTIME_VIRTUAL);
		reader.read_array(asset->runtimeValues, 3, asset->runtimeBuffers.count);
		reader.pop_stream();

		read_draw_data(reader, asset->draw, asset);
		read_runtime_buffers(reader, asset->runtimeBuffers);

		for (auto& material : asset->materials)
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, material);
		}

		for (auto& image : asset->images1)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, image);
		}

		if (const auto animations = read_list(reader, asset->animations, 7))
		{
			for (auto i = 0ull; i < asset->animations.count; i++)
			{
				if (const auto animation = reader.read_array(animations[i], 7))
				{
					reader.read_asset(ASSET_TYPE_SANIM, animation->anim);
				}
			}
		}

		read_list(reader, asset->values4, 3);

		if (const auto lights = read_list(reader, asset->lights, 7))
		{
			for (auto i = 0u; i < asset->lights.count; i++)
			{
				reader.read_asset(ASSET_TYPE_MATERIAL, lights[i].material1);
				reader.read_asset(ASSET_TYPE_MATERIAL, lights[i].material2);
			}
		}

		reader.push_stream(XFILE_BLOCK_RUNTIME_VIRTUAL);
		reader.read_array(asset->runtimeLights, 7, asset->lights.count);
		reader.pop_stream();

		if (const auto decals = read_list(reader, asset->decals, 7))
		{
			for (auto i = 0u; i < asset->decals.count; i++)
			{
				reader.read_asset(ASSET_TYPE_MATERIAL, decals[i].material);
			}
		}

		if (const auto refs = read_list(reader, asset->materialRefs, 7))
		{
			for (auto i = 0u; i < asset->materialRefs.count; i++)
			{
				reader.read_asset(ASSET_TYPE_MATERIAL, refs[i].material);
			}
		}

		reader.push_stream(XFILE_BLOCK_MEMMAPPED);
		reader.read_array(asset->data1, 0, asset->data1Size);
		reader.pop_stream();

		reader.push_stream(XFILE_BLOCK_MEMMAPPED);
		reader.read_array(asset->data2, 0, asset->data2Size);
		reader.pop_stream();

		reader.read_asset(ASSET_TYPE_IMAGE, asset->image2);
		read_volumes(reader, asset->volumes);
		read_runtime_grid(reader, asset->runtimeGrid);
		read_cell_data(reader, asset->cellData);

		read_list(reader, asset->values5, 3);
		read_list(reader, asset->values6, 3);
		read_list(reader, asset->values7, 3);

		reader.read_asset(ASSET_TYPE_LIGHTING, asset->lighting);
		reader.read_asset(ASSET_TYPE_STREAMERWORLD, asset->streamerWorld);

		if (const auto refs = read_list(reader, asset->modelRefs, 7))
		{
			for (auto i = 0ull; i < asset->modelRefs.count; i++)
			{
				read_model_ref(reader, refs[i]);
			}
		}

		read_list(reader, asset->values8, 1);
		read_list(reader, asset->values9, 3);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image3);
		read_list(reader, asset->values10, 3);
		read_list(reader, asset->values11, 3);
		read_list(reader, asset->values12, 3);
		read_list(reader, asset->values13, 3);
		read_list(reader, asset->values14, 3);
		read_list(reader, asset->values15, 3);
		read_list(reader, asset->values16, 3);
		read_list(reader, asset->values17, 3);

		for (auto& block : asset->blocks)
		{
			read_list(reader, block, 0);
		}

		for (auto& image : asset->images2)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, image);
		}
	}

	void gfx_world::dump(GfxWorld* asset)
	{
		const auto& data = asset->vertexData;
		if (!data.positions || !data.vertices || !data.indices || !asset->draw.surfaces)
		{
			return;
		}

		std::string text;
		for (auto i = 0u; i < data.vertexCount; i++)
		{
			text += std::format("v {} {} {}\n", data.positions[i][0], data.positions[i][1], data.positions[i][2]);
		}

		for (auto i = 0u; i < data.vertexCount; i++)
		{
			const auto& vertex = data.vertices[i];
			text += std::format("vt {} {}\n", vertex.texCoord[0], 1.0f - vertex.texCoord[1]);
		}

		for (auto i = 0u; i < data.vertexCount; i++)
		{
			float normal[3]{};
			for (auto j = 0; j < 3; j++)
			{
				normal[j] = static_cast<float>((data.vertices[i].normal >> (10 * j)) & 0x3FF) / 1023.0f * 2.0f - 1.0f;
			}

			text += std::format("vn {} {} {}\n", normal[0], normal[1], normal[2]);
		}

		for (auto i = 0; i < asset->surfaceCount; i++)
		{
			const auto& surface = asset->draw.surfaces[i];
			const auto index_count = surface.triCount * 3u;
			if (surface.baseIndex + index_count > data.indexCount)
			{
				ZONETOOL_WARNING("gfxworld surface %d is outside the index data", i);
				continue;
			}

			text += std::format("g surface_{}\nusemtl {}\n", i, surface.material ? xhash_name(surface.material->name) : "");
			const auto* indices = data.indices + surface.baseIndex;
			for (auto j = 0u; j < index_count; j += 3)
			{
				text += 'f';
				for (auto k = 0u; k < 3; k++)
				{
					const auto vertex = surface.baseVertex + indices[j + k] + 1;
					text += std::format(" {}/{}/{}", vertex, vertex, vertex);
				}

				text += '\n';
			}
		}

		filesystem::file file("maps/" + xhash_name(asset->name) + ".gfxworld.obj");
		file.open("wb");
		file.write(text);
		file.close();
	}

	namespace
	{
		std::optional<std::string> read_stream_key_data(const StreamKey* key)
		{
			if (key->flags & STREAM_KEY_FLAG_RESIDENT)
			{
				return key->data ? std::optional(std::string(reinterpret_cast<const char*>(key->data), key->size)) : std::nullopt;
			}

			return xpak::read(key->xpakKey, key->size);
		}

		void append_static_models(const StreamKey* key, std::vector<static_model_instance>& models)
		{
			const auto data = read_stream_key_data(key);
			if (!data || data->size() < sizeof(StaticModelStreamHeader))
			{
				ZONETOOL_WARNING("Static model stream \"%s\" has no data", xhash_name(key->name).data());
				return;
			}

			const auto header = reinterpret_cast<const StaticModelStreamHeader*>(data->data());
			const auto count = static_cast<std::size_t>(header->instanceCount);
			if (data->size() < sizeof(StaticModelStreamHeader) + count * (sizeof(StaticModelInstance) + sizeof(StaticModelPlacement)))
			{
				throw std::runtime_error(std::format("static model stream \"{}\" is too small for {} instances", xhash_name(key->name), count));
			}

			const auto instances = reinterpret_cast<const StaticModelInstance*>(header + 1);
			const auto placements = reinterpret_cast<const StaticModelPlacement*>(instances + count);
			for (auto i = 0u; i < count; i++)
			{
				models.push_back({instances[i].model, placements[i], instances[i].deformationIndex});
			}
		}
	}

	std::vector<static_model_instance> read_static_models(const Districts* asset)
	{
		std::vector<static_model_instance> models;
		for (auto i = 0ull; i < asset->districtCount; i++)
		{
			if (const auto key = asset->districts[i].streamKeys[DISTRICT_STREAM_KEY_STATIC_MODELS])
			{
				append_static_models(key, models);
			}
		}

		return models;
	}

	void group_lod_model::read(auto& reader, GroupLodModel* asset)
	{
		reader.read_asset(ASSET_TYPE_XMODEL, asset->model);
		reader.read_asset_array(ASSET_TYPE_XMODELMESH, asset->meshes.values, static_cast<std::size_t>(asset->meshes.count));
	}

	void static_level_fx_list::read(auto& reader, StaticLevelFxList* asset)
	{
		if (const auto effects = reader.read_array(asset->effects, 7, asset->effectCount))
		{
			for (auto i = 0u; i < asset->effectCount; i++)
			{
				auto& effect = effects[i];
				reader.read_asset(ASSET_TYPE_FX, effect.effect);
				reader.read_string(effect.name);

				if (const auto targets = reader.read_array(effect.targets, 7, effect.targetCount))
				{
					for (auto j = 0u; j < effect.targetCount; j++)
					{
						reader.read_string(targets[j].name);
					}
				}
			}
		}

		if (const auto strings = reader.read_array(asset->strings, 7, asset->stringCount))
		{
			for (auto i = 0u; i < asset->stringCount; i++)
			{
				reader.read_string(strings[i]);
			}
		}
	}

	void districts::read(auto& reader, Districts* asset)
	{
		reader.read_asset(ASSET_TYPE_GFX_MAP, asset->world);

		auto& data = asset->data;
		reader.read_array(data.values1, 7, data.count1);
		reader.read_array(data.values2, 3, data.count1);
		reader.read_array(data.values4, 3, data.count1);
		reader.read_array(data.values3, 3, data.count2);

		if (const auto entries = reader.read_array(asset->districts, 7, static_cast<std::size_t>(asset->districtCount)))
		{
			for (auto i = 0u; i < asset->districtCount; i++)
			{
				for (auto& stream_key : entries[i].streamKeys)
				{
					reader.read_asset(ASSET_TYPE_STREAMKEY, stream_key);
				}
			}
		}

		if (const auto refs = reader.read_array(asset->refs, 7, static_cast<std::size_t>(asset->refCount)))
		{
			for (auto i = 0u; i < asset->refCount; i++)
			{
				if (refs[i])
				{
					reader.resolve_pointer(refs[i]);
				}
			}
		}
	}

	void districts::dump(Districts* asset)
	{
		std::string text = "model,origin_x,origin_y,origin_z,quat_x,quat_y,quat_z,quat_w,scale,mins_x,mins_y,mins_z,maxs_x,maxs_y,maxs_z,deformation_index\n";
		for (const auto& [model, placement, deformation_index] : read_static_models(asset))
		{
			text += std::format("{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}\n", xhash_name(model), placement.origin[0], placement.origin[1], placement.origin[2],
				placement.quat[0], placement.quat[1], placement.quat[2], placement.quat[3], placement.scale,
				placement.mins[0], placement.mins[1], placement.mins[2], placement.maxs[0], placement.maxs[1], placement.maxs[2],
				static_cast<std::int32_t>(deformation_index));
		}

		filesystem::file file("maps/" + xhash_name(asset->name) + ".smodels.csv");
		file.open("wb");
		file.write(text);
		file.close();
	}
}

namespace zonetool::t9
{
	template void gfx_world::read(zone_reader&, GfxWorld*);
	template void group_lod_model::read(zone_reader&, GroupLodModel*);
	template void static_level_fx_list::read(zone_reader&, StaticLevelFxList*);
	template void districts::read(zone_reader&, Districts*);
	template void gfx_world::read(zone_writer&, GfxWorld*);
	template void group_lod_model::read(zone_writer&, GroupLodModel*);
	template void static_level_fx_list::read(zone_writer&, StaticLevelFxList*);
	template void districts::read(zone_writer&, Districts*);
}
