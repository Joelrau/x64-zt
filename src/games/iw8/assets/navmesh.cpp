#include <std_include.hpp>
#include "navmesh.hpp"

namespace zonetool::iw8
{
	void nav_mesh::read(zone_reader& reader, NavMeshData* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto resources = reader.read_array(asset->navResources, 7, asset->numNavResources))
		{
			for (auto i = 0; i < asset->numNavResources; i++)
			{
				auto& resource = resources[i];
				reader.read_script_string(resource.targetName);
				reader.read_array(resource.pGraphBuffer, 3, resource.graphSize);
				reader.read_array(resource.pWorkingGraph, 3, resource.bDockable ? resource.graphSize : 0);
			}
		}

		if (const auto bounds = reader.read_array(asset->obstacleBounds, 7, asset->numObstacleBounds))
		{
			for (auto i = 0; i < asset->numObstacleBounds; i++)
			{
				if (const auto hulls = reader.read_array(bounds[i].m_Hulls, 7, bounds[i].m_NumHulls))
				{
					for (auto j = 0; j < bounds[i].m_NumHulls; j++)
					{
						reader.read_array(hulls[j].m_Boundaries, 3, hulls[j].m_NumBoundaries);
					}
				}
			}
		}

		if (const auto glass = reader.read_array(asset->glassBounds, 7, asset->numGlassBounds))
		{
			for (auto i = 0; i < asset->numGlassBounds; i++)
			{
				reader.read_array(glass[i].m_Boundaries, 3, glass[i].m_NumBoundaries);
			}
		}

		if (const auto modifiers = reader.read_array(asset->modifiers, 7, asset->numModifiers))
		{
			for (auto i = 0; i < asset->numModifiers; i++)
			{
				auto& modifier = modifiers[i];
				reader.read_array(modifier.m_Boundaries, 3, modifier.m_NumBoundaries);
				reader.read_script_string(modifier.m_TargetName);
				reader.read_script_string(modifier.m_Target);
				reader.read_script_string(modifier.m_ScriptNoteworthy);
				reader.read_script_string(modifier.m_ScriptLinkName);
			}
		}

		if (const auto links = reader.read_array(asset->linkCreationData, 3, asset->numLinkCreationData))
		{
			for (auto i = 0; i < asset->numLinkCreationData; i++)
			{
				reader.read_script_string(links[i].m_Animscript);
				reader.read_script_string(links[i].m_Target);
				reader.read_script_string(links[i].m_Parent);
			}
		}

		reader.read_array(asset->volumeSeeds, 3, asset->numVolumeSeeds);
		reader.read_array(asset->rawVolumes, 3, asset->numRawVolumes);

		if (const auto volumes = reader.read_array(asset->rawCustomVolumes, 7, asset->numRawCustomVolumes))
		{
			for (auto i = 0; i < asset->numRawCustomVolumes; i++)
			{
				reader.read_array(volumes[i].m_Tris, 3, volumes[i].m_NumTris);
			}
		}
		reader.pop_stream();
	}
}
