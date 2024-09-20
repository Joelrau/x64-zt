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

		asset->brushModels = read.read_array<PhysBrushModel>();
		asset->polytopeDatas = read.read_array<dmPolytopeData>();
		for (unsigned int i = 0; i < asset->polytopeCount; i++)
		{
			asset->polytopeDatas[i].m_aVertices = read.read_array<dmFloat4>();
			asset->polytopeDatas[i].m_aPlanes = read.read_array<dmPlane>();
			asset->polytopeDatas[i].m_surfaceTypes = read.read_array<dm_uint16>();
			asset->polytopeDatas[i].m_vertexMaterials = read.read_array<dm_uint16>();
			asset->polytopeDatas[i].m_aSubEdges = read.read_array<dmSubEdge>();
			asset->polytopeDatas[i].m_aFaceSubEdges = read.read_array<dm_uint8>();
		}
		asset->meshDatas = read.read_array<dmMeshData>();
		for (unsigned int i = 0; i < asset->meshDataCount; i++)
		{
			asset->meshDatas[i].m_pRoot = read.read_array<dmMeshNode>();
			asset->meshDatas[i].m_aVertices = read.read_array<dmFloat4>();
			asset->meshDatas[i].m_aTriangles = read.read_array<dmMeshTriangle>();
		}
		asset->waterVolumeDefs = read.read_array<PhysWaterVolumeDef>();
		for (unsigned int i = 0; i < asset->waterVolumeDefCount; i++)
		{
			asset->waterVolumeDefs[i].physWaterPreset = read.read_asset<PhysWaterPreset>();
			this->add_script_string(&asset->waterVolumeDefs[i].string, read.read_string());
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

		if (data->waterVolumeDefs)
		{
			for (unsigned int i = 0; i < data->waterVolumeDefCount; i++)
			{
				data->waterVolumeDefs[i].string = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->waterVolumeDefs[i].string)));
			}
		}
	}

	void phys_world::load_depending(zone_base* zone)
	{
		for (unsigned int i = 0; i < this->asset_->waterVolumeDefCount; i++)
		{
			zone->add_asset_of_type(ASSET_TYPE_PHYSWATERPRESET, this->asset_->waterVolumeDefs[i].physWaterPreset->name);
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

		if (data->brushModels)
		{
			buf->align(3);
			buf->write(data->brushModels, data->brushModelCount);
			buf->clear_pointer(&dest->brushModels);
		}

		if (data->polytopeDatas)
		{
			buf->align(3);
			auto* dest_poly_datas = buf->write(data->polytopeDatas, data->polytopeCount);
			for (unsigned int i = 0; i < data->polytopeCount; i++)
			{
				auto* data_poly_data = &data->polytopeDatas[i];
				auto* dest_poly_data = &dest_poly_datas[i];

				if (data_poly_data->m_aVertices)
				{
					buf->align(15);
					buf->write(data_poly_data->m_aVertices, data_poly_data->m_vertexCount);
					buf->clear_pointer(&dest_poly_data->m_aVertices);
				}

				if (data_poly_data->m_aPlanes)
				{
					buf->align(15);
					buf->write(data_poly_data->m_aPlanes, data_poly_data->m_faceCount);
					buf->clear_pointer(&dest_poly_data->m_aPlanes);
				}

				if (data_poly_data->m_aSubEdges)
				{
					buf->align(3);
					buf->write(data_poly_data->m_aSubEdges, data_poly_data->m_subEdgeCount);
					buf->clear_pointer(&dest_poly_data->m_aSubEdges);
				}

				if (data_poly_data->m_aFaceSubEdges)
				{
					buf->align(0);
					buf->write(data_poly_data->m_aFaceSubEdges, data_poly_data->m_faceCount);
					buf->clear_pointer(&dest_poly_data->m_aFaceSubEdges);
				}

				if (data_poly_data->m_surfaceTypes)
				{
					buf->align(1);
					buf->write(data_poly_data->m_surfaceTypes, data_poly_data->m_faceCount);
					buf->clear_pointer(&dest_poly_data->m_surfaceTypes);
				}

				if (data_poly_data->m_vertexMaterials)
				{
					buf->align(1);
					buf->write(data_poly_data->m_vertexMaterials, data_poly_data->m_vertexCount);
					buf->clear_pointer(&dest_poly_data->m_vertexMaterials);
				}
			}
			buf->clear_pointer(&dest->polytopeDatas);
		}

		if (data->meshDatas)
		{
			buf->align(3);
			auto* dest_mesh_datas = buf->write(data->meshDatas, data->meshDataCount);
			for (unsigned int i = 0; i < data->meshDataCount; i++)
			{
				auto* data_mesh_data = &data->meshDatas[i];
				auto* dest_mesh_data = &dest_mesh_datas[i];

				if (data_mesh_data->m_pRoot)
				{
					buf->align(15);
					buf->write(data_mesh_data->m_pRoot, data_mesh_data->m_nodeCount);
					buf->clear_pointer(&dest_mesh_data->m_pRoot);
				}

				if (data_mesh_data->m_aVertices)
				{
					buf->align(15);
					buf->write(data_mesh_data->m_aVertices, data_mesh_data->m_vertexCount);
					buf->clear_pointer(&dest_mesh_data->m_aVertices);
				}

				if (data_mesh_data->m_aTriangles)
				{
					buf->align(3);
					buf->write(data_mesh_data->m_aTriangles, data_mesh_data->m_triangleCount);
					buf->clear_pointer(&dest_mesh_data->m_aTriangles);
				}
			}
			buf->clear_pointer(&dest->meshDatas);
		}

		if (data->waterVolumeDefs)
		{
			buf->align(3);
			auto* dest_waterVolumes = buf->write(data->waterVolumeDefs, data->waterVolumeDefCount);
			for (unsigned int i = 0; i < data->waterVolumeDefCount; i++)
			{
				auto* data_waterVolume = &data->waterVolumeDefs[i];
				auto* dest_waterVolume = &dest_waterVolumes[i];

				if (data_waterVolume->physWaterPreset)
				{
					dest_waterVolume->physWaterPreset = reinterpret_cast<PhysWaterPreset*>(
						zone->get_asset_pointer(ASSET_TYPE_PHYSWATERPRESET, data->waterVolumeDefs->physWaterPreset->name));
				}
			}
			buf->clear_pointer(&dest->waterVolumeDefs);
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

		write.dump_array(asset->brushModels, asset->brushModelCount);
		write.dump_array(asset->polytopeDatas, asset->polytopeCount);
		for (unsigned int i = 0; i < asset->polytopeCount; i++)
		{
			write.dump_array(asset->polytopeDatas[i].m_aVertices, asset->polytopeDatas[i].m_vertexCount);
			write.dump_array(asset->polytopeDatas[i].m_aPlanes, asset->polytopeDatas[i].m_faceCount);
			write.dump_array(asset->polytopeDatas[i].m_surfaceTypes, asset->polytopeDatas[i].m_faceCount);
			write.dump_array(asset->polytopeDatas[i].m_vertexMaterials, asset->polytopeDatas[i].m_vertexCount);
			write.dump_array(asset->polytopeDatas[i].m_aSubEdges, asset->polytopeDatas[i].m_subEdgeCount);
			write.dump_array(asset->polytopeDatas[i].m_aFaceSubEdges, asset->polytopeDatas[i].m_faceCount);
		}
		write.dump_array(asset->meshDatas, asset->meshDataCount);
		for (unsigned int i = 0; i < asset->meshDataCount; i++)
		{
			write.dump_array(asset->meshDatas[i].m_pRoot, asset->meshDatas[i].m_nodeCount);
			write.dump_array(asset->meshDatas[i].m_aVertices, asset->meshDatas[i].m_vertexCount);
			write.dump_array(asset->meshDatas[i].m_aTriangles, asset->meshDatas[i].m_triangleCount);
		}
		write.dump_array(asset->waterVolumeDefs, asset->waterVolumeDefCount);
		for (unsigned int i = 0; i < asset->waterVolumeDefCount; i++)
		{
			write.dump_asset(asset->waterVolumeDefs[i].physWaterPreset);
			write.dump_string(SL_ConvertToString(asset->waterVolumeDefs[i].string));
		}

		write.close();
	}
}