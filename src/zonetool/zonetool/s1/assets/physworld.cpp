#include "std_include.hpp"
#include "physworld.hpp"

namespace zonetool::s1
{
	void phys_world::add_script_string(scr_string_t* ptr, const char* str)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return;
			}
		}
		this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
	}

	const char* phys_world::get_script_string(scr_string_t* ptr)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return this->script_strings[i].second;
			}
		}
		return nullptr;
	}

	PhysWorld* phys_world::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = name + ".physmap"s;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physmap \"%s\"...", name.data());

		PhysWorld* asset = read.read_single<PhysWorld>();
		asset->name = read.read_string();

		asset->models = read.read_array<PhysBrushModel>();
		asset->polytopeDatas = read.read_array<dmPolytopeData>();
		for (unsigned int i = 0; i < asset->polytopeDatasCount; i++)
		{
			asset->polytopeDatas[i].vec4_array0 = read.read_array<vec4_t>();
			asset->polytopeDatas[i].vec4_array1 = read.read_array<vec4_t>();
			asset->polytopeDatas[i].uint16_array0 = read.read_array<unsigned short>();
			asset->polytopeDatas[i].uint16_array1 = read.read_array<unsigned short>();
			asset->polytopeDatas[i].edges = read.read_array<dmSubEdge>();
			asset->polytopeDatas[i].uint8_array0 = read.read_array<unsigned char>();
		}
		asset->meshDatas = read.read_array<dmMeshData>();
		for (unsigned int i = 0; i < asset->meshDatasCount; i++)
		{
			asset->meshDatas[i].meshNodes = read.read_array<dmMeshNode_array_t>();
			asset->meshDatas[i].vec4_array0 = read.read_array<vec4_t>();
			asset->meshDatas[i].meshTriangles = read.read_array<dmMeshTriangle>();
		}
		asset->waterVolumes = read.read_array<PhysWaterVolumeDef>();
		for (unsigned int i = 0; i < asset->waterVolumesCount; i++)
		{
			asset->waterVolumes[i].physWaterPreset = read.read_asset<PhysWaterPreset>();
			this->add_script_string(&asset->waterVolumes[i].string, read.read_string());
		}

		read.close();

		return asset;
	}

	void phys_world::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") + filesystem::get_fastfile() + ".d3dbsp"; // name;
		this->asset_ = parse(name, mem);

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse physmap \"%s\"", name.data());
		}
	}

	void phys_world::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;

		if (data->waterVolumes)
		{
			for (unsigned int i = 0; i < data->waterVolumesCount; i++)
			{
				data->waterVolumes[i].string = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->waterVolumes[i].string)));
			}
		}
	}

	void phys_world::load_depending(zone_base* zone)
	{
		for (unsigned int i = 0; i < this->asset_->waterVolumesCount; i++)
		{
			zone->add_asset_of_type(ASSET_TYPE_PHYSWATERPRESET, this->asset_->waterVolumes[i].physWaterPreset->name);
		}
	}

	std::string phys_world::name()
	{
		return this->name_;
	}

	std::int32_t phys_world::type()
	{
		return ASSET_TYPE_PHYSWORLDMAP;
	}

	void phys_world::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->models)
		{
			buf->align(3);
			buf->write(data->models, data->modelsCount);
			zone_buffer::clear_pointer(&dest->models);
		}

		if (data->polytopeDatas)
		{
			buf->align(3);
			auto* dest_poly_datas = buf->write(data->polytopeDatas, data->polytopeDatasCount);
			for (unsigned int i = 0; i < data->polytopeDatasCount; i++)
			{
				auto* data_poly_data = &data->polytopeDatas[i];
				auto* dest_poly_data = &dest_poly_datas[i];

				if (data_poly_data->vec4_array0)
				{
					buf->align(15);
					buf->write(data_poly_data->vec4_array0, data_poly_data->count0);
					zone_buffer::clear_pointer(&dest_poly_data->vec4_array0);
				}

				if (data_poly_data->vec4_array1)
				{
					buf->align(15);
					buf->write(data_poly_data->vec4_array1, data_poly_data->count1);
					zone_buffer::clear_pointer(&dest_poly_data->vec4_array1);
				}

				if (data_poly_data->edges)
				{
					buf->align(3);
					buf->write(data_poly_data->edges, data_poly_data->count2);
					zone_buffer::clear_pointer(&dest_poly_data->edges);
				}

				if (data_poly_data->uint8_array0)
				{
					buf->align(0);
					buf->write(data_poly_data->uint8_array0, data_poly_data->count1);
					zone_buffer::clear_pointer(&dest_poly_data->uint8_array0);
				}

				if (data_poly_data->uint16_array0)
				{
					buf->align(1);
					buf->write(data_poly_data->uint16_array0, data_poly_data->count1);
					zone_buffer::clear_pointer(&dest_poly_data->uint16_array0);
				}

				if (data_poly_data->uint16_array1)
				{
					buf->align(1);
					buf->write(data_poly_data->uint16_array1, data_poly_data->count0);
					zone_buffer::clear_pointer(&dest_poly_data->uint16_array1);
				}
			}
			zone_buffer::clear_pointer(&dest->polytopeDatas);
		}

		if (data->meshDatas)
		{
			buf->align(3);
			auto* dest_mesh_datas = buf->write(data->meshDatas, data->meshDatasCount);
			for (unsigned int i = 0; i < data->meshDatasCount; i++)
			{
				auto* data_mesh_data = &data->meshDatas[i];
				auto* dest_mesh_data = &dest_mesh_datas[i];

				if (data_mesh_data->meshNodes)
				{
					buf->align(15);
					buf->write(data_mesh_data->meshNodes, data_mesh_data->meshNodeCount);
					zone_buffer::clear_pointer(&dest_mesh_data->meshNodes);
				}

				if (data_mesh_data->vec4_array0)
				{
					buf->align(15);
					buf->write(data_mesh_data->vec4_array0, data_mesh_data->vec4_array0_count);
					zone_buffer::clear_pointer(&dest_mesh_data->vec4_array0);
				}

				if (data_mesh_data->meshTriangles)
				{
					buf->align(3);
					buf->write(data_mesh_data->meshTriangles, data_mesh_data->meshTriangleCount);
					zone_buffer::clear_pointer(&dest_mesh_data->meshTriangles);
				}
			}
			zone_buffer::clear_pointer(&dest->meshDatas);
		}

		if (data->waterVolumes)
		{
			buf->align(3);
			auto* dest_waterVolumes = buf->write(data->waterVolumes, data->waterVolumesCount);
			for (unsigned int i = 0; i < data->waterVolumesCount; i++)
			{
				auto* data_waterVolume = &data->waterVolumes[i];
				auto* dest_waterVolume = &dest_waterVolumes[i];

				if (data_waterVolume->physWaterPreset)
				{
					dest_waterVolume->physWaterPreset = reinterpret_cast<PhysWaterPreset*>(
						zone->get_asset_pointer(ASSET_TYPE_PHYSWATERPRESET, data->waterVolumes->physWaterPreset->name));
				}
			}
			zone_buffer::clear_pointer(&dest->waterVolumes);
		}
		buf->pop_stream();
	}

	void phys_world::dump(PhysWorld* asset)
	{
		const auto path = asset->name + ".physmap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->models, asset->modelsCount);
		write.dump_array(asset->polytopeDatas, asset->polytopeDatasCount);
		for (unsigned int i = 0; i < asset->polytopeDatasCount; i++)
		{
			write.dump_array(asset->polytopeDatas[i].vec4_array0, asset->polytopeDatas[i].count0);
			write.dump_array(asset->polytopeDatas[i].vec4_array1, asset->polytopeDatas[i].count1);
			write.dump_array(asset->polytopeDatas[i].uint16_array0, asset->polytopeDatas[i].count1);
			write.dump_array(asset->polytopeDatas[i].uint16_array1, asset->polytopeDatas[i].count0);
			write.dump_array(asset->polytopeDatas[i].edges, asset->polytopeDatas[i].count2);
			write.dump_array(asset->polytopeDatas[i].uint8_array0, asset->polytopeDatas[i].count1);
		}
		write.dump_array(asset->meshDatas, asset->meshDatasCount);
		for (unsigned int i = 0; i < asset->meshDatasCount; i++)
		{
			write.dump_array(asset->meshDatas[i].meshNodes, asset->meshDatas[i].meshNodeCount);
			write.dump_array(asset->meshDatas[i].vec4_array0, asset->meshDatas[i].vec4_array0_count);
			write.dump_array(asset->meshDatas[i].meshTriangles, asset->meshDatas[i].meshTriangleCount);
		}
		write.dump_array(asset->waterVolumes, asset->waterVolumesCount);
		for (unsigned int i = 0; i < asset->waterVolumesCount; i++)
		{
			write.dump_asset(asset->waterVolumes[i].physWaterPreset);
			write.dump_string(SL_ConvertToString(asset->waterVolumes[i].string));
		}

		write.close();
	}
}