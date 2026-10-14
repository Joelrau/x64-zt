#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_geom(zone_reader& reader, PhysGeomInfo* geom)
		{
			switch (geom->type)
			{
			case 1:
				if (const auto polytope = reader.read_single(geom->polytope, 7))
				{
					phys_collmap::read_polytope(reader, polytope);
				}
				break;
			case 2:
				reader.read_single(geom->box, 3);
				break;
			case 3:
				reader.read_single(geom->capsule, 3);
				break;
			}
		}
	}

	void phys_collmap::read_polytope(zone_reader& reader, dmPolytopeData* data)
	{
		reader.read_array(data->vec4_array0, 15, data->vec4_array0_count);
		reader.read_array(data->vec4_array1, 15, data->vec4_array1_count);
		reader.read_array(data->uint_array, 0, data->uint_array_count);
		reader.read_array(data->byte_array, 0, data->vec4_array1_count);
	}

	void phys_collmap::read(zone_reader& reader, PhysCollmap* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto geoms = reader.read_array(asset->geoms, 7, asset->geomCount))
		{
			for (auto i = 0; i < asset->geomCount; i++)
			{
				read_geom(reader, &geoms[i]);
			}
		}

		if (const auto parts = reader.read_array(asset->parts, 3, asset->partCount))
		{
			for (auto i = 0; i < asset->partCount; i++)
			{
				reader.read_script_string(parts[i].name);
			}
		}

		reader.pop_stream();
	}

	void phys_worldmap::read(zone_reader& reader, PhysWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->models, 3, asset->modelCount);

		if (const auto polytopes = reader.read_array(asset->polytopes, 7, asset->polytopeCount))
		{
			for (auto i = 0u; i < asset->polytopeCount; i++)
			{
				phys_collmap::read_polytope(reader, &polytopes[i]);
			}
		}

		if (const auto meshes = reader.read_array(asset->meshes, 7, asset->meshCount))
		{
			for (auto i = 0u; i < asset->meshCount; i++)
			{
				reader.read_array(meshes[i].vertices, 15, meshes[i].vertexCount);
				reader.read_array(meshes[i].normals, 15, meshes[i].normalCount);
				reader.read_array(meshes[i].triangles, 3, meshes[i].triangleCount);
			}
		}

		if (const auto waters = reader.read_array(asset->waters, 7, asset->waterCount))
		{
			for (auto i = 0u; i < asset->waterCount; i++)
			{
				reader.read_asset(ASSET_TYPE_PHYSWATERPRESET, waters[i].preset);
				reader.read_script_string(waters[i].name);
			}
		}

		reader.pop_stream();
	}
}
