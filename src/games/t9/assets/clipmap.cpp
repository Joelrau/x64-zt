#include <std_include.hpp>
#include "clipmap.hpp"

namespace zonetool::t9
{
	namespace
	{
		template <typename T>
		void read_memmapped_array(auto& reader, T*& field, const std::size_t alignment, const std::size_t count)
		{
			reader.push_stream(XFILE_BLOCK_MEMMAPPED);
			reader.read_array(field, alignment, count);
			reader.pop_stream();
		}

		template <typename T>
		void read_runtime_array(auto& reader, T*& field, const std::size_t alignment, const std::size_t count)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME_VIRTUAL);
			reader.read_array(field, alignment, count);
			reader.pop_stream();
		}

		void read_model(auto& reader, ClipMapModel* model)
		{
			reader.read_array(model->names, 7);

			if (const auto list = reader.read_array(model->entryList, 7))
			{
				reader.read_array(list->entries, 7, list->entryCount);
			}

			reader.read_asset_array(ASSET_TYPE_XMODEL, model->models, model->modelCount);
			reader.read_array(model->modelValues, 1, model->modelCount);
			reader.read_array(model->values, 3, model->valueCount);

			reader.read_array(model->data.values1, 15, static_cast<std::size_t>(model->dataCount1));
			reader.read_array(model->data.values2, 3, static_cast<std::size_t>(model->dataCount1 + model->dataCount2));
			reader.read_array(model->data.values3, 3, model->dataCount3);

			reader.read_array(model->indices, 1, model->indexCount ? model->indexCount + 1u : 0u);
		}

		template <typename T>
		void write_raw(filesystem::file& file, const T* values, const std::size_t count = 1)
		{
			const std::uint8_t present = values != nullptr;
			file.write(&present);
			if (values && count)
			{
				file.write(values, sizeof(T), count);
			}
		}

		void write_name(filesystem::file& file, const XCollision* collision)
		{
			const auto hash = collision ? collision->name.value : 0ull;
			file.write(&hash);
		}

		void write_mesh_raw(filesystem::file& file, const ClipMapMesh& mesh)
		{
			write_raw(file, &mesh);
			write_raw(file, mesh.planes, mesh.planeCount);
			write_raw(file, mesh.brushVertices, mesh.brushVertexCount);
			write_raw(file, mesh.brushFirstPlane, mesh.brushCount);
			write_raw(file, mesh.brushVertexRanges, mesh.brushCount);
			write_raw(file, mesh.values5, mesh.brushCount);
			write_raw(file, mesh.bits, (mesh.planeCount + 6ull * mesh.brushCount + 5) / 6);
			write_raw(file, mesh.brushes, mesh.brushCount);
			write_raw(file, mesh.data1, mesh.data1Size);
			write_raw(file, mesh.meshVertices, mesh.meshVertexCount);
			write_raw(file, mesh.meshIndices, mesh.meshIndexCount);
			write_raw(file, mesh.meshLeaves, mesh.meshLeafCount);
			write_raw(file, mesh.data3, mesh.data3Size);
			write_raw(file, mesh.values9, mesh.count6);
		}

		void write_models_raw(filesystem::file& file, ClipMapModel* const* models, const std::size_t count)
		{
			for (auto i = 0u; models && i < count; i++)
			{
				const auto* model = models[i];
				write_raw(file, model);
				if (!model)
				{
					continue;
				}

				write_raw(file, model->names);
				write_raw(file, model->entryList);
				if (model->entryList)
				{
					write_raw(file, model->entryList->entries, model->entryList->entryCount);
				}

				for (auto j = 0u; model->models && j < model->modelCount; j++)
				{
					const auto hash = model->models[j] ? model->models[j]->name.value : 0ull;
					file.write(&hash);
				}

				write_raw(file, model->modelValues, model->modelCount);
				write_raw(file, model->values, model->valueCount);
				write_raw(file, model->data.values1, static_cast<std::size_t>(model->dataCount1));
				write_raw(file, model->data.values2, static_cast<std::size_t>(model->dataCount1 + model->dataCount2));
				write_raw(file, model->data.values3, model->dataCount3);
				write_raw(file, model->indices, model->indexCount ? model->indexCount + 1u : 0u);
			}
		}
	}

	void read_clip_map_mesh(auto& reader, ClipMapMesh& mesh)
	{
		read_memmapped_array(reader, mesh.planes, 3, mesh.planeCount);
		read_memmapped_array(reader, mesh.brushVertices, 3, mesh.brushVertexCount);
		read_memmapped_array(reader, mesh.brushFirstPlane, 3, mesh.brushCount);
		read_memmapped_array(reader, mesh.brushVertexRanges, 3, mesh.brushCount);
		read_memmapped_array(reader, mesh.values5, 3, mesh.brushCount);
		reader.read_array(mesh.bits, 7, (mesh.planeCount + 6ull * mesh.brushCount + 5) / 6);
		read_memmapped_array(reader, mesh.brushes, 3, mesh.brushCount);
		read_memmapped_array(reader, mesh.data1, 0, mesh.data1Size);
		read_memmapped_array(reader, mesh.meshVertices, 3, mesh.meshVertexCount);
		read_memmapped_array(reader, mesh.meshIndices, 0, mesh.meshIndexCount);
		reader.read_array(mesh.meshLeaves, 7, mesh.meshLeafCount);
		read_memmapped_array(reader, mesh.data3, 0, mesh.data3Size);
		reader.read_array(mesh.values9, 3, mesh.count6);
	}

	void read_clip_map_table(auto& reader, ClipMapTable& table)
	{
		reader.read_array(table.values1, 3, table.valueCount);
		reader.read_array(table.values2, 3, table.valueCount);
		reader.read_array(table.data, 0, table.dataSize);
	}

	void read_clip_map_models(auto& reader, ClipMapModel**& models, const std::size_t count)
	{
		if (const auto pointers = reader.read_array(models, 7, count))
		{
			for (auto i = 0u; i < count; i++)
			{
				if (const auto model = reader.read_array(pointers[i], 7))
				{
					read_model(reader, model);
				}
			}
		}
	}

	void clip_map::read(auto& reader, ClipMap* asset)
	{
		if (const auto partitions = reader.read_array(asset->partitions, 7, asset->partitionCount))
		{
			for (auto i = 0u; i < asset->partitionCount; i++)
			{
				auto& partition = partitions[i];
				read_memmapped_array(reader, partition.data, 0, partition.dataSize);

				if (const auto entries = reader.read_array(partition.entries, 7, partition.entryCount))
				{
					for (auto j = 0u; j < partition.entryCount; j++)
					{
						reader.read_asset(ASSET_TYPE_XCOLLISION, entries[j].collision);
					}
				}
			}
		}

		reader.read_array(asset->partitionValues, 3, asset->partitionCount);
		read_clip_map_models(reader, asset->models1, asset->models1Count);
		read_clip_map_models(reader, asset->models2, asset->models2Count);

		if (const auto list = reader.read_array(asset->modelList, 7))
		{
			read_clip_map_models(reader, list->models, list->modelCount);
			reader.read_array(list->values, 3, list->modelCount);
		}

		if (const auto meshes = reader.read_array(asset->meshes, 7, asset->meshCount))
		{
			for (auto i = 0u; i < asset->meshCount; i++)
			{
				read_clip_map_mesh(reader, meshes[i]);
			}
		}

		if (const auto table = reader.read_array(asset->table, 7))
		{
			read_clip_map_table(reader, *table);
		}

		reader.read_asset_array(ASSET_TYPE_XCOLLISION, asset->collisions, asset->collisionCount);
		reader.read_array(asset->values1, 3, asset->values1Count);
		reader.read_array(asset->values2, 1, asset->values2Count);

		if (const auto dyn_models = reader.read_array(asset->dynModels, 7, asset->dynModelCount))
		{
			for (auto i = 0u; i < asset->dynModelCount; i++)
			{
				reader.read_asset(ASSET_TYPE_DYNMODEL, dyn_models[i].dynModel);
			}
		}

		read_runtime_array(reader, asset->runtime1, 7, asset->dynModelCount);
		read_runtime_array(reader, asset->runtime2, 7, asset->dynModelCount);
		read_runtime_array(reader, asset->runtime3, 7, asset->runtimeCount);
		read_runtime_array(reader, asset->runtime4, 3, asset->dynModelCount);
		read_runtime_array(reader, asset->runtime5, 3, asset->runtimeCount);

		if (const auto constraints = reader.read_array(asset->constraints, 7, asset->constraintCount))
		{
			for (auto i = 0u; i < asset->constraintCount; i++)
			{
				read_phys_constraint(reader, &constraints[i]);
			}
		}

		read_runtime_array(reader, asset->runtime6, 7, asset->runtime6Count);

		if (const auto list = reader.read_array(asset->regionList, 7))
		{
			read_terrain_regions(reader, list);
		}
	}

	void clip_map::dump(ClipMap* asset)
	{
		filesystem::file file(std::format("collision/{}.clipmap.raw", xhash_name(asset->name)));
		file.open("wb");
		write_raw(file, asset);

		for (auto i = 0u; asset->partitions && i < asset->partitionCount; i++)
		{
			const auto& partition = asset->partitions[i];
			write_raw(file, &partition);
			write_raw(file, partition.data, partition.dataSize);
			for (auto j = 0u; partition.entries && j < partition.entryCount; j++)
			{
				write_name(file, partition.entries[j].collision);
				file.write(partition.entries[j].__pad0, sizeof(partition.entries[j].__pad0), 1);
			}
		}

		write_raw(file, asset->partitionValues, asset->partitionCount);
		write_models_raw(file, asset->models1, asset->models1Count);
		write_models_raw(file, asset->models2, asset->models2Count);

		for (auto i = 0u; asset->meshes && i < asset->meshCount; i++)
		{
			write_mesh_raw(file, asset->meshes[i]);
		}

		for (auto i = 0u; asset->collisions && i < asset->collisionCount; i++)
		{
			write_name(file, asset->collisions[i]);
		}

		write_raw(file, asset->values1, asset->values1Count);
		write_raw(file, asset->values2, asset->values2Count);
		write_raw(file, asset->dynModels, asset->dynModelCount);
		file.close();
	}
}

namespace zonetool::t9
{
	template void read_clip_map_mesh(zone_reader&, ClipMapMesh&);
	template void read_clip_map_table(zone_reader&, ClipMapTable&);
	template void read_clip_map_models(zone_reader&, ClipMapModel**&, std::size_t);
	template void clip_map::read(zone_reader&, ClipMap*);
	template void read_clip_map_mesh(zone_writer&, ClipMapMesh&);
	template void read_clip_map_table(zone_writer&, ClipMapTable&);
	template void read_clip_map_models(zone_writer&, ClipMapModel**&, std::size_t);
	template void clip_map::read(zone_writer&, ClipMap*);
}
