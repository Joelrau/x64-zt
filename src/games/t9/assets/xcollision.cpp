#include <std_include.hpp>
#include "xcollision.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_hull(auto& reader, XCollisionHull* hull)
		{
			reader.read_array(hull->planes, 3, hull->planeCount);
			reader.read_array(hull->vertices, 3, hull->vertexCount);
			reader.read_array(hull->edgeValues1, 3, hull->edgeCount);
			reader.read_array(hull->edgeValues2, 3, hull->edgeCount);
			reader.read_array(hull->edgeValues3, 3, hull->edgeCount);
			reader.read_array(hull->packedEdges, 7, (hull->planeCount + 6 * hull->edgeCount + 5) / 6);
			reader.read_array(hull->edges, 3, hull->edgeCount);
			reader.read_array(hull->extraData, 0, hull->extraDataSize);
			reader.read_array(hull->faces, 3, hull->faceCount);
			reader.read_array(hull->faceIndices, 0, hull->faceIndexCount);
			reader.read_array(hull->polys, 7, hull->polyCount);
			reader.read_array(hull->polyIndices, 0, hull->polyIndexCount);
			reader.read_array(hull->records, 3, hull->recordCount);
		}

		void read_bvh(auto& reader, XCollisionBvh* bvh)
		{
			reader.read_array(bvh->nodes, 3, bvh->nodeCount);
			reader.read_array(bvh->leaves, 3, bvh->nodeCount);
			reader.read_array(bvh->data, 0, bvh->dataSize);
		}

		void read_data(auto& reader, XCollisionData* data)
		{
			reader.read_array(data->bytes, 0, data->byteCount);

			if (const auto meshes = reader.read_array(data->meshes, 7, data->meshCount))
			{
				for (auto i = 0; i < data->meshCount; i++)
				{
					reader.read_array(meshes[i].triangles, 0, meshes[i].triangleCount);
					reader.read_array(meshes[i].indices, 1, meshes[i].indexCount);
					reader.read_array(meshes[i].values, 3, meshes[i].valueCount);
					reader.read_array(meshes[i].data, 0, meshes[i].dataSize);
				}
			}

			if (const auto shapes = reader.read_array(data->shapes, 7, data->shapeCount))
			{
				for (auto i = 0; i < data->shapeCount; i++)
				{
					if (const auto hull = reader.read_array(shapes[i].hull, 7))
					{
						read_hull(reader, hull);
					}

					if (const auto bvh = reader.read_array(shapes[i].bvh, 7))
					{
						read_bvh(reader, bvh);
					}
				}
			}
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

		void write_data(filesystem::file& file, const XCollisionData& data)
		{
			write_raw(file, data.bytes, data.byteCount);
			for (auto i = 0; data.meshes && i < data.meshCount; i++)
			{
				const auto& mesh = data.meshes[i];
				write_raw(file, &mesh);
				write_raw(file, mesh.triangles, mesh.triangleCount);
				write_raw(file, mesh.indices, mesh.indexCount);
				write_raw(file, mesh.values, mesh.valueCount);
				write_raw(file, mesh.data, mesh.dataSize);
			}

			for (auto i = 0; data.shapes && i < data.shapeCount; i++)
			{
				write_raw(file, &data.shapes[i]);
				write_hull_raw(file, data.shapes[i].hull);

				const auto* bvh = data.shapes[i].bvh;
				write_raw(file, bvh);
				if (bvh)
				{
					write_raw(file, bvh->nodes, bvh->nodeCount);
					write_raw(file, bvh->leaves, bvh->nodeCount);
					write_raw(file, bvh->data, bvh->dataSize);
				}
			}
		}
	}

	void write_hull_raw(filesystem::file& file, const XCollisionHull* hull)
	{
		write_raw(file, hull);
		if (!hull)
		{
			return;
		}

		write_raw(file, hull->planes, hull->planeCount);
		write_raw(file, hull->vertices, hull->vertexCount);
		write_raw(file, hull->edgeValues1, hull->edgeCount);
		write_raw(file, hull->edgeValues2, hull->edgeCount);
		write_raw(file, hull->edgeValues3, hull->edgeCount);
		write_raw(file, hull->packedEdges, (hull->planeCount + 6 * hull->edgeCount + 5) / 6);
		write_raw(file, hull->edges, hull->edgeCount);
		write_raw(file, hull->extraData, hull->extraDataSize);
		write_raw(file, hull->faces, hull->faceCount);
		write_raw(file, hull->faceIndices, hull->faceIndexCount);
		write_raw(file, hull->polys, hull->polyCount);
		write_raw(file, hull->polyIndices, hull->polyIndexCount);
		write_raw(file, hull->records, hull->recordCount);
	}

	void xcollision::read(auto& reader, XCollision* asset)
	{
		reader.read_asset(ASSET_TYPE_XSKELETON, asset->skeleton);
		reader.read_asset(ASSET_TYPE_STREAMKEY, asset->streamKey);

		if (const auto data = reader.read_array(asset->data, 7))
		{
			read_data(reader, data);
		}

		reader.read_asset(ASSET_TYPE_PHYSPRESET, asset->physPreset);
		reader.read_asset(ASSET_TYPE_TRIGGERACTIONS, asset->triggerActions);
		reader.read_asset(ASSET_TYPE_PHYSCONSTRAINTS, asset->physConstraints);
	}

	void xcollision::dump(XCollision* asset)
	{
		filesystem::file file(std::format("collision/{}.xcollision.raw", xhash_name(asset->name)));
		file.open("wb");
		write_raw(file, asset);
		if (asset->data)
		{
			write_raw(file, asset->data);
			write_data(file, *asset->data);
		}

		file.close();
	}
}

namespace zonetool::t9
{
	template void xcollision::read(zone_reader&, XCollision*);
	template void xcollision::read(zone_writer&, XCollision*);
}
