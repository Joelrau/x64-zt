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

	template <typename V>
	void phys_world_layout::visit(const V& w)
	{
		w.string(offsetof(PhysWorld, name));
		w.array(S2_ARRAY_FIELD(PhysWorld, models), 3, w.template at<unsigned int>(offsetof(PhysWorld, modelCount)));
		w.entries(offsetof(PhysWorld, polytopes), sizeof(dmPolytopeData), w.template at<unsigned int>(offsetof(PhysWorld, polytopeCount)), [](const auto& polytope, std::size_t)
		{
			const auto count = [&](const std::size_t offset)
			{
				return polytope.template at<int>(offset);
			};

			polytope.array(S2_ARRAY_FIELD(dmPolytopeData, vec4_array0), 15, count(offsetof(dmPolytopeData, vec4_array0_count)));
			polytope.array(S2_ARRAY_FIELD(dmPolytopeData, vec4_array1), 15, count(offsetof(dmPolytopeData, vec4_array1_count)));
			polytope.array(S2_ARRAY_FIELD(dmPolytopeData, uint_array), 0, count(offsetof(dmPolytopeData, uint_array_count)));
			polytope.array(S2_ARRAY_FIELD(dmPolytopeData, byte_array), 0, count(offsetof(dmPolytopeData, vec4_array1_count)));
		});
		w.entries(offsetof(PhysWorld, meshes), sizeof(PhysMeshData), w.template at<unsigned int>(offsetof(PhysWorld, meshCount)), [](const auto& mesh, std::size_t)
		{
			mesh.array(S2_ARRAY_FIELD(PhysMeshData, nodes), 15, mesh.template at<int>(offsetof(PhysMeshData, nodeCount)));
			mesh.array(S2_ARRAY_FIELD(PhysMeshData, vertices), 15, mesh.template at<int>(offsetof(PhysMeshData, vertexCount)));
			mesh.array(S2_ARRAY_FIELD(PhysMeshData, triangles), 3, mesh.template at<int>(offsetof(PhysMeshData, triangleCount)));
		});
		w.entries(offsetof(PhysWorld, waters), sizeof(PhysWaterVolumeDef), w.template at<unsigned int>(offsetof(PhysWorld, waterCount)), [](const auto& water, std::size_t)
		{
			water.asset(ASSET_TYPE_PHYSWATERPRESET, offsetof(PhysWaterVolumeDef, preset));
			water.script_string(offsetof(PhysWaterVolumeDef, name));
		});
	}

	template void phys_world_layout::visit(const field_reader&);
	template void phys_world_layout::visit(const field_writer&);
	template void phys_world_layout::visit(const field_dumper&);
	template void phys_world_layout::visit(const field_parser&);
	template void phys_world_layout::visit(const field_dependencies&);
	template void phys_world_layout::visit(const field_preparer&);
}
