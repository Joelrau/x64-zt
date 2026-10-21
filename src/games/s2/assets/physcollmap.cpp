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

	namespace
	{
		enum phys_geom_type
		{
			phys_geom_polytope = 1,
			phys_geom_box = 2,
			phys_geom_capsule = 3,
		};
	}

	void phys_collmap::write_polytope(zone_buffer* buf, dmPolytopeData* data, dmPolytopeData* dest)
	{
		write_inline(buf, 15, data->vec4_array0, data->vec4_array0_count, dest->vec4_array0);
		write_inline(buf, 15, data->vec4_array1, data->vec4_array1_count, dest->vec4_array1);
		write_inline(buf, 0, data->uint_array, data->uint_array_count, dest->uint_array);
		write_inline(buf, 0, data->byte_array, data->vec4_array1_count, dest->byte_array);
	}

	void phys_collmap::dump_polytope(assetmanager::dumper& write, dmPolytopeData* data)
	{
		write.dump_array(data->vec4_array0, data->vec4_array0_count);
		write.dump_array(data->vec4_array1, data->vec4_array1_count);
		write.dump_array(data->uint_array, data->uint_array_count);
		write.dump_array(data->byte_array, data->vec4_array1_count);
	}

	void phys_collmap::parse_polytope(assetmanager::reader& read, dmPolytopeData* data)
	{
		data->vec4_array0 = read.read_array<vec4_t>();
		data->vec4_array1 = read.read_array<vec4_t>();
		data->uint_array = read.read_array<unsigned int>();
		data->byte_array = read.read_array<unsigned char>();
	}

	PhysCollmap* phys_collmap::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_PHYSCOLLMAP, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing phys_collmap \"%s\"...", name.data());

		auto* asset = read.read_single<PhysCollmap>();
		asset->name = read.read_string();

		asset->geoms = read.read_array<PhysGeomInfo>();
		for (auto i = 0; asset->geoms && i < asset->geomCount; i++)
		{
			auto& geom = asset->geoms[i];
			switch (geom.type)
			{
			case phys_geom_polytope:
				geom.polytope = read.read_single<dmPolytopeData>();
				if (geom.polytope)
				{
					parse_polytope(read, geom.polytope);
				}
				break;
			case phys_geom_box:
				geom.box = read.read_single<PhysGeomBox>();
				break;
			case phys_geom_capsule:
				geom.capsule = read.read_single<PhysGeomCapsule>();
				break;
			default:
				geom.data = nullptr;
				break;
			}
		}

		asset->parts = read.read_array<PhysCollmapPart>();
		for (auto i = 0; asset->parts && i < asset->partCount; i++)
		{
			asset->parts[i].name = parse_script_string(read);
		}

		read.close();
		return asset;
	}

	void phys_collmap::prepare(zone_buffer* buf, zone_memory* mem)
	{
		script_string_preparer prepare(buf);
		for (auto i = 0; this->asset_->parts && i < this->asset_->partCount; i++)
		{
			prepare(this->asset_->parts[i].name);
		}
	}

	void phys_collmap::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);

		if (data->geoms)
		{
			buf->align(7);
			auto* geoms = buf->write(data->geoms, data->geomCount);
			for (auto i = 0; i < data->geomCount; i++)
			{
				const auto& geom = data->geoms[i];
				if (!geom.data)
				{
					continue;
				}

				switch (geom.type)
				{
				case phys_geom_polytope:
				{
					buf->align(7);
					auto* polytope = buf->write(geom.polytope);
					write_polytope(buf, geom.polytope, polytope);
					buf->clear_pointer(&geoms[i].polytope);
					break;
				}
				case phys_geom_box:
					write_inline(buf, 3, geom.box, 1, geoms[i].box);
					break;
				case phys_geom_capsule:
					write_inline(buf, 3, geom.capsule, 1, geoms[i].capsule);
					break;
				default:
					geoms[i].data = nullptr;
					break;
				}
			}
			buf->clear_pointer(&dest->geoms);
		}

		write_inline(buf, 3, data->parts, data->partCount, dest->parts);

		buf->pop_stream();
	}

	void phys_collmap::dump(PhysCollmap* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_PHYSCOLLMAP, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->geoms, asset->geomCount);
		for (auto i = 0; asset->geoms && i < asset->geomCount; i++)
		{
			const auto& geom = asset->geoms[i];
			switch (geom.type)
			{
			case phys_geom_polytope:
				write.dump_single(geom.polytope);
				if (geom.polytope)
				{
					dump_polytope(write, geom.polytope);
				}
				break;
			case phys_geom_box:
				write.dump_single(geom.box);
				break;
			case phys_geom_capsule:
				write.dump_single(geom.capsule);
				break;
			}
		}

		write.dump_array(asset->parts, asset->partCount);
		for (auto i = 0; asset->parts && i < asset->partCount; i++)
		{
			dump_script_string(write, asset->parts[i].name);
		}

		write.close();
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
				reader.read_array(meshes[i].nodes, 15, meshes[i].nodeCount);
				reader.read_array(meshes[i].vertices, 15, meshes[i].vertexCount);
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

	PhysWorld* phys_worldmap::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_PHYSWORLDMAP, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing phys_worldmap \"%s\"...", name.data());

		auto* asset = read.read_single<PhysWorld>();
		asset->name = read.read_string();
		parse_array(read, mem, asset->models, asset->modelCount);
		if (const auto polytopes = parse_array(read, mem, asset->polytopes, asset->polytopeCount))
		{
			for (auto i = 0u; i < asset->polytopeCount; i++)
			{
				auto* polytope = &polytopes[i];
				parse_array(read, mem, polytope->vec4_array0, polytope->vec4_array0_count);
				parse_array(read, mem, polytope->vec4_array1, polytope->vec4_array1_count);
				parse_array(read, mem, polytope->uint_array, polytope->uint_array_count);
				parse_array(read, mem, polytope->byte_array, polytope->vec4_array1_count);
			}
		}
		if (const auto meshes = parse_array(read, mem, asset->meshes, asset->meshCount))
		{
			for (auto i = 0u; i < asset->meshCount; i++)
			{
				parse_array(read, mem, meshes[i].nodes, meshes[i].nodeCount);
				parse_array(read, mem, meshes[i].vertices, meshes[i].vertexCount);
				parse_array(read, mem, meshes[i].triangles, meshes[i].triangleCount);
			}
		}
		if (const auto waters = parse_array(read, mem, asset->waters, asset->waterCount))
		{
			for (auto i = 0u; i < asset->waterCount; i++)
			{
				waters[i].preset = read.read_asset<PhysWaterPreset>();
				waters[i].name = parse_script_string(read);
			}
		}
		read.close();

		return asset;
	}

	void phys_worldmap::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		script_string_preparer prepare(buf);
		for (auto i = 0u; data->waters && i < data->waterCount; i++)
		{
			prepare(data->waters[i].name);
		}
	}

	void phys_worldmap::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		for (auto i = 0u; data->waters && i < data->waterCount; i++)
		{
			add_dependency(zone, ASSET_TYPE_PHYSWATERPRESET, data->waters[i].preset);
		}
	}

	void phys_worldmap::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = this->write_name(buf);
		write_array(buf, 3, data->models, data->modelCount, dest->models);
		if (const auto dest_polytopes = write_array(buf, 7, data->polytopes, data->polytopeCount, dest->polytopes))
		{
			for (auto i = 0u; i < data->polytopeCount; i++)
			{
				auto* polytope = &data->polytopes[i];
				auto* dest_polytope = &dest_polytopes[i];
				write_array(buf, 15, polytope->vec4_array0, polytope->vec4_array0_count, dest_polytope->vec4_array0);
				write_array(buf, 15, polytope->vec4_array1, polytope->vec4_array1_count, dest_polytope->vec4_array1);
				write_array(buf, 0, polytope->uint_array, polytope->uint_array_count, dest_polytope->uint_array);
				write_array(buf, 0, polytope->byte_array, polytope->vec4_array1_count, dest_polytope->byte_array);
			}
		}
		if (const auto dest_meshes = write_array(buf, 7, data->meshes, data->meshCount, dest->meshes))
		{
			for (auto i = 0u; i < data->meshCount; i++)
			{
				auto* mesh = &data->meshes[i];
				write_array(buf, 15, mesh->nodes, mesh->nodeCount, dest_meshes[i].nodes);
				write_array(buf, 15, mesh->vertices, mesh->vertexCount, dest_meshes[i].vertices);
				write_array(buf, 3, mesh->triangles, mesh->triangleCount, dest_meshes[i].triangles);
			}
		}
		if (const auto dest_waters = write_array(buf, 7, data->waters, data->waterCount, dest->waters))
		{
			for (auto i = 0u; i < data->waterCount; i++)
			{
				dest_waters[i].preset = asset_reference(zone, ASSET_TYPE_PHYSWATERPRESET, data->waters[i].preset);
			}
		}
		buf->pop_stream();
	}

	void phys_worldmap::dump(PhysWorld* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_PHYSWORLDMAP, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->models, asset->modelCount);
		write.dump_array(asset->polytopes, asset->polytopeCount);
		for (auto i = 0u; asset->polytopes && i < asset->polytopeCount; i++)
		{
			phys_collmap::dump_polytope(write, &asset->polytopes[i]);
		}
		write.dump_array(asset->meshes, asset->meshCount);
		for (auto i = 0u; asset->meshes && i < asset->meshCount; i++)
		{
			write.dump_array(asset->meshes[i].nodes, asset->meshes[i].nodeCount);
			write.dump_array(asset->meshes[i].vertices, asset->meshes[i].vertexCount);
			write.dump_array(asset->meshes[i].triangles, asset->meshes[i].triangleCount);
		}
		write.dump_array(asset->waters, asset->waterCount);
		for (auto i = 0u; asset->waters && i < asset->waterCount; i++)
		{
			write.dump_asset(asset->waters[i].preset);
			dump_script_string(write, asset->waters[i].name);
		}
		write.close();
	}
}
