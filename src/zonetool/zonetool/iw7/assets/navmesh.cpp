#include <std_include.hpp>
#include "navmesh.hpp"

namespace zonetool::iw7
{
	void nav_mesh::add_script_string(scr_string_t* ptr, const char* str)
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

	const char* nav_mesh::get_script_string(scr_string_t* ptr)
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

	NavMeshData* nav_mesh::parse(std::string name, zone_memory* mem)
	{
		assetmanager::reader read(mem);

		const auto path = name + ".navmesh"s;
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing navmesh \"%s\"...", name.data());

		const auto asset = read.read_single<NavMeshData>();
		asset->name = read.read_string();

		asset->navResources = read.read_array<nav_resource_s>();
		for (int i = 0; i < asset->numNavResources; i++)
		{
			this->add_script_string(&asset->navResources[i].targetName, read.read_string());
			asset->navResources[i].pGraphBuffer = read.read_array<char>();
			if (asset->navResources[i].bDockable)
			{
				asset->navResources[i].pWorkingGraph = read.read_array<char>();
			}
		}

		asset->obstacleBounds = read.read_array<nav_obstacle_bounds_s>();
		for (int i = 0; i < asset->numObstacleBounds; i++)
		{
			if (asset->obstacleBounds[i].m_Hulls)
			{
				asset->obstacleBounds[i].m_Hulls = read.read_array<nav_obstacle_hull_s>();
				for (int j = 0; j < asset->obstacleBounds[i].m_NumHulls; j++)
				{
					asset->obstacleBounds[i].m_Hulls[j].m_Boundaries = read.read_array<nav_boundary_plane_s>();
				}
			}
		}

		asset->glassBounds = read.read_array<nav_glass_bounds_s>();
		for (int i = 0; i < asset->numGlassBounds; i++)
		{
			if (asset->glassBounds[i].m_Boundaries)
			{
				asset->glassBounds[i].m_Boundaries = read.read_array<nav_boundary_plane_s>();
			}
		}

		asset->modifiers = read.read_array<nav_modifier_s>();
		for (int i = 0; i < asset->numModifiers; i++)
		{
			asset->modifiers[i].m_Boundaries = read.read_array<nav_boundary_plane_s>();
			this->add_script_string(&asset->modifiers[i].m_TargetName, read.read_string());
		}

		asset->linkCreationData = read.read_array<nav_link_creation_data_s>();
		for (int i = 0; i < asset->numLinkCreationData; i++)
		{
			this->add_script_string(&asset->linkCreationData[i].m_Animscript, read.read_string());
			this->add_script_string(&asset->linkCreationData[i].m_Target, read.read_string());
			this->add_script_string(&asset->linkCreationData[i].m_Parent, read.read_string());
		}

		asset->volumeSeeds = read.read_array<vec3_t>();

		asset->rawVolumes = read.read_array<nav_raw_volume_s>();

		asset->rawCustomVolumes = read.read_array<nav_raw_custom_volume_s>();
		for (int i = 0; i < asset->numRawCustomVolumes; i++)
		{
			asset->rawCustomVolumes[i].m_Tris = read.read_array<nav_raw_custom_volume_tri_s>();
		}

		return asset;
	}

	void nav_mesh::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		this->asset_ = this->parse(name, mem);

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse navmesh \"%s\"", name.data());
		}
	}

	void nav_mesh::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;

		for (int i = 0; i < data->numNavResources; i++)
		{
			data->navResources[i].targetName = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->navResources[i].targetName)));
		}

		for (int i = 0; i < data->numModifiers; i++)
		{
			data->modifiers[i].m_TargetName = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->modifiers[i].m_TargetName)));
		}

		for (int i = 0; i < data->numLinkCreationData; i++)
		{
			data->linkCreationData[i].m_Animscript = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->linkCreationData[i].m_Animscript)));
			data->linkCreationData[i].m_Target = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->linkCreationData[i].m_Target)));
			data->linkCreationData[i].m_Parent = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->linkCreationData[i].m_Parent)));
		}
	}

	void nav_mesh::load_depending(zone_base* zone)
	{
	}

	std::string nav_mesh::name()
	{
		return this->name_;
	}

	std::int32_t nav_mesh::type()
	{
		return ASSET_TYPE_NAVMESH;
	}

	void nav_mesh::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->navResources)
		{
			buf->align(7);
			auto* dest_navResources = buf->write(data->navResources, data->numNavResources);
			for (int i = 0; i < data->numNavResources; i++)
			{
				if (data->navResources[i].pGraphBuffer)
				{
					buf->align(3);
					buf->write(data->navResources[i].pGraphBuffer, data->navResources[i].graphSize);
					buf->clear_pointer(&dest_navResources[i].pGraphBuffer);
				}
				
				if (data->navResources[i].pWorkingGraph && data->navResources[i].bDockable)
				{
					buf->align(3);
					buf->write(data->navResources[i].pWorkingGraph, data->navResources[i].graphSize);
					buf->clear_pointer(&dest_navResources[i].pWorkingGraph);
				}
				else
				{
					dest_navResources[i].pWorkingGraph = nullptr;
				}
			}
			buf->clear_pointer(&dest->navResources);
		}

		if (data->obstacleBounds)
		{
			buf->align(7);
			auto* dest_obstacleBounds = buf->write(data->obstacleBounds, data->numObstacleBounds);
			for (int i = 0; i < data->numObstacleBounds; i++)
			{
				if (data->obstacleBounds[i].m_Hulls)
				{
					buf->align(7);
					auto* dest_hulls = buf->write(data->obstacleBounds[i].m_Hulls, data->obstacleBounds[i].m_NumHulls);
					for (int j = 0; j < data->obstacleBounds[i].m_NumHulls; j++)
					{
						if (data->obstacleBounds[i].m_Hulls[j].m_Boundaries)
						{
							buf->align(3);
							buf->write(data->obstacleBounds[i].m_Hulls[j].m_Boundaries, data->obstacleBounds[i].m_Hulls[j].m_NumBoundaries);
							buf->clear_pointer(&dest_hulls[j].m_Boundaries);
						}
					}
					buf->clear_pointer(&dest_obstacleBounds[i].m_Hulls);
				}
			}
			buf->clear_pointer(&dest->obstacleBounds);
		}

		if (data->glassBounds)
		{
			buf->align(7);
			auto* dest_glassBounds = buf->write(data->glassBounds, data->numGlassBounds);
			for (int i = 0; i < data->numGlassBounds; i++)
			{
				if (data->glassBounds[i].m_Boundaries)
				{
					buf->align(3);
					buf->write(data->glassBounds[i].m_Boundaries, data->glassBounds[i].m_NumBoundaries);
					buf->clear_pointer(&dest_glassBounds[i].m_Boundaries);
				}
			}
			buf->clear_pointer(&dest->glassBounds);
		}

		if (data->modifiers)
		{
			buf->align(7);
			auto* dest_modifiers = buf->write(data->modifiers, data->numModifiers);
			for (int i = 0; i < data->numModifiers; i++)
			{
				if (data->modifiers[i].m_Boundaries)
				{
					buf->align(3);
					buf->write(data->modifiers[i].m_Boundaries, data->modifiers[i].m_NumBoundaries);
					buf->clear_pointer(&dest_modifiers[i].m_Boundaries);
				}
			}
			buf->clear_pointer(&dest->modifiers);
		}

		if (data->linkCreationData)
		{
			buf->align(3);
			buf->write(data->linkCreationData, data->numLinkCreationData);
			buf->clear_pointer(&dest->linkCreationData);
		}

		if (data->volumeSeeds)
		{
			buf->align(3);
			buf->write(data->volumeSeeds, data->numVolumeSeeds);
			buf->clear_pointer(&dest->volumeSeeds);
		}

		if (data->rawVolumes)
		{
			buf->align(3);
			buf->write(data->rawVolumes, data->numRawVolumes);
			buf->clear_pointer(&dest->rawVolumes);
		}

		if (data->rawCustomVolumes)
		{
			buf->align(7);
			auto* dest_rawCustomVolumes = buf->write(data->rawCustomVolumes, data->numRawCustomVolumes);
			for (int i = 0; i < data->numRawCustomVolumes; i++)
			{
				if (data->rawCustomVolumes[i].m_Tris)
				{
					buf->align(3);
					buf->write(data->rawCustomVolumes[i].m_Tris, data->rawCustomVolumes[i].m_NumTris);
					buf->clear_pointer(&dest_rawCustomVolumes[i].m_Tris);
				}
			}
			buf->clear_pointer(&dest->rawCustomVolumes);
		}

		buf->pop_stream();
	}

	void dump_nav_resource(assetmanager::dumper& dumper, nav_resource_s* data)
	{
		dumper.dump_string(SL_ConvertToString(data->targetName));
		dumper.dump_array(data->pGraphBuffer, data->graphSize);
		if (data->bDockable)
		{
			dumper.dump_array(data->pWorkingGraph, data->graphSize);
		}
	}

	void dump_nav_obstacle_bounds(assetmanager::dumper& dumper, nav_obstacle_bounds_s* data)
	{
		dumper.dump_array(data->m_Hulls, data->m_NumHulls);
		for (int i = 0; i < data->m_NumHulls; i++)
		{
			dumper.dump_array(data->m_Hulls[i].m_Boundaries, data->m_Hulls[i].m_NumBoundaries);
		}
	}

	void dump_nav_glass_bounds(assetmanager::dumper& dumper, nav_glass_bounds_s* data)
	{
		dumper.dump_array(data->m_Boundaries, data->m_NumBoundaries);
	}

	void dump_nav_modifiers(assetmanager::dumper& dumper, nav_modifier_s* data)
	{
		dumper.dump_array(data->m_Boundaries, data->m_NumBoundaries);
		dumper.dump_string(SL_ConvertToString(data->m_TargetName));
	}


	void dump_nav_link_creation_data(assetmanager::dumper& dumper, nav_link_creation_data_s* data)
	{
		dumper.dump_string(SL_ConvertToString(data->m_Animscript));
		dumper.dump_string(SL_ConvertToString(data->m_Target));
		dumper.dump_string(SL_ConvertToString(data->m_Parent));
	}

	void dump_nav_raw_custom_volume(assetmanager::dumper& dumper, nav_raw_custom_volume_s* data)
	{
		dumper.dump_array(data->m_Tris, data->m_NumTris);
	}

	void nav_mesh::dump(NavMeshData* asset)
	{
		const auto path = asset->name + ".navmesh"s;

		assetmanager::dumper dumper;
		if (!dumper.open(path))
		{
			return;
		}

		dumper.dump_single(asset);
		dumper.dump_string(asset->name);

		dumper.dump_array(asset->navResources, asset->numNavResources);
		for (int i = 0; i < asset->numNavResources; i++)
		{
			dump_nav_resource(dumper, &asset->navResources[i]);
		}

		dumper.dump_array(asset->obstacleBounds, asset->numObstacleBounds);
		for (int i = 0; i < asset->numObstacleBounds; i++)
		{
			dump_nav_obstacle_bounds(dumper, &asset->obstacleBounds[i]);
		}

		dumper.dump_array(asset->glassBounds, asset->numGlassBounds);
		for (int i = 0; i < asset->numGlassBounds; i++)
		{
			dump_nav_glass_bounds(dumper, &asset->glassBounds[i]);
		}

		dumper.dump_array(asset->modifiers, asset->numModifiers);
		for (int i = 0; i < asset->numModifiers; i++)
		{
			dump_nav_modifiers(dumper, &asset->modifiers[i]);
		}

		dumper.dump_array(asset->linkCreationData, asset->numLinkCreationData);
		for (int i = 0; i < asset->numLinkCreationData; i++)
		{
			dump_nav_link_creation_data(dumper, &asset->linkCreationData[i]);
		}

		dumper.dump_array(asset->volumeSeeds, asset->numVolumeSeeds);

		dumper.dump_array(asset->rawVolumes, asset->numRawVolumes);

		dumper.dump_array(asset->rawCustomVolumes, asset->numRawCustomVolumes);
		for (int i = 0; i < asset->numRawCustomVolumes; i++)
		{
			dump_nav_raw_custom_volume(dumper, &asset->rawCustomVolumes[i]);
		}
	}
}