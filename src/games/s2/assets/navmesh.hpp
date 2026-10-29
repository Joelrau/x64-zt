#pragma once

namespace zonetool::s2
{
	class nav_mesh : public asset_base<NavMeshData, ASSET_TYPE_NAVMESH>
	{
	public:
		static void read(zone_reader& reader, NavMeshData* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);

			if (const auto resources = reader.read_array(asset->navResources, 7, asset->numNavResources))
			{
				for (auto i = 0; i < asset->numNavResources; i++)
				{
					reader.read_script_string(resources[i].targetName);
					reader.read_array(resources[i].pGraphBuffer, 3, resources[i].graphSize);
					reader.read_array(resources[i].pWorkingGraph, 3, working_graph_size(&resources[i]));
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

			if (const auto glass_bounds = reader.read_array(asset->glassBounds, 7, asset->numGlassBounds))
			{
				for (auto i = 0; i < asset->numGlassBounds; i++)
				{
					reader.read_array(glass_bounds[i].m_Boundaries, 3, glass_bounds[i].m_NumBoundaries);
				}
			}

			if (const auto modifiers = reader.read_array(asset->modifiers, 7, asset->numModifiers))
			{
				for (auto i = 0; i < asset->numModifiers; i++)
				{
					reader.read_array(modifiers[i].m_Boundaries, 3, modifiers[i].m_NumBoundaries);
					reader.read_script_string(modifiers[i].m_TargetName);
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

			if (const auto custom_volumes = reader.read_array(asset->rawCustomVolumes, 7, asset->numRawCustomVolumes))
			{
				for (auto i = 0; i < asset->numRawCustomVolumes; i++)
				{
					reader.read_array(custom_volumes[i].m_Tris, 3, custom_volumes[i].m_NumTris);
				}
			}

			reader.pop_stream();
		}

		NavMeshData* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_NAVMESH, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing navmesh \"%s\"...", name.data());

			auto* asset = read.read_single<NavMeshData>();
			asset->name = read.read_string();

			if (const auto resources = parse_array(read, mem, asset->navResources, asset->numNavResources))
			{
				for (auto i = 0; i < asset->numNavResources; i++)
				{
					resources[i].targetName = parse_script_string(read);
					parse_array(read, mem, resources[i].pGraphBuffer, resources[i].graphSize);
					parse_array(read, mem, resources[i].pWorkingGraph, working_graph_size(&resources[i]));
				}
			}

			if (const auto bounds = parse_array(read, mem, asset->obstacleBounds, asset->numObstacleBounds))
			{
				for (auto i = 0; i < asset->numObstacleBounds; i++)
				{
					if (const auto hulls = parse_array(read, mem, bounds[i].m_Hulls, bounds[i].m_NumHulls))
					{
						for (auto j = 0; j < bounds[i].m_NumHulls; j++)
						{
							parse_array(read, mem, hulls[j].m_Boundaries, hulls[j].m_NumBoundaries);
						}
					}
				}
			}

			if (const auto glass_bounds = parse_array(read, mem, asset->glassBounds, asset->numGlassBounds))
			{
				for (auto i = 0; i < asset->numGlassBounds; i++)
				{
					parse_array(read, mem, glass_bounds[i].m_Boundaries, glass_bounds[i].m_NumBoundaries);
				}
			}

			if (const auto modifiers = parse_array(read, mem, asset->modifiers, asset->numModifiers))
			{
				for (auto i = 0; i < asset->numModifiers; i++)
				{
					parse_array(read, mem, modifiers[i].m_Boundaries, modifiers[i].m_NumBoundaries);
					modifiers[i].m_TargetName = parse_script_string(read);
				}
			}

			if (const auto links = parse_array(read, mem, asset->linkCreationData, asset->numLinkCreationData))
			{
				for (auto i = 0; i < asset->numLinkCreationData; i++)
				{
					links[i].m_Animscript = parse_script_string(read);
					links[i].m_Target = parse_script_string(read);
					links[i].m_Parent = parse_script_string(read);
				}
			}

			parse_array(read, mem, asset->volumeSeeds, asset->numVolumeSeeds);
			parse_array(read, mem, asset->rawVolumes, asset->numRawVolumes);

			if (const auto custom_volumes = parse_array(read, mem, asset->rawCustomVolumes, asset->numRawCustomVolumes))
			{
				for (auto i = 0; i < asset->numRawCustomVolumes; i++)
				{
					parse_array(read, mem, custom_volumes[i].m_Tris, custom_volumes[i].m_NumTris);
				}
			}
			read.close();

			return asset;
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
			auto* data = this->asset_;
			script_string_preparer prepare(buf);

			for (auto i = 0; data->navResources && i < data->numNavResources; i++)
			{
				prepare(data->navResources[i].targetName);
			}

			for (auto i = 0; data->modifiers && i < data->numModifiers; i++)
			{
				prepare(data->modifiers[i].m_TargetName);
			}

			for (auto i = 0; data->linkCreationData && i < data->numLinkCreationData; i++)
			{
				prepare(data->linkCreationData[i].m_Animscript);
				prepare(data->linkCreationData[i].m_Target);
				prepare(data->linkCreationData[i].m_Parent);
			}
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);

			if (const auto dest_resources = write_array(buf, 7, data->navResources, data->numNavResources, dest->navResources))
			{
				for (auto i = 0; i < data->numNavResources; i++)
				{
					auto* resource = &data->navResources[i];
					write_array(buf, 3, resource->pGraphBuffer, resource->graphSize, dest_resources[i].pGraphBuffer);
					write_array(buf, 3, resource->pWorkingGraph, working_graph_size(resource), dest_resources[i].pWorkingGraph);
				}
			}

			if (const auto dest_bounds = write_array(buf, 7, data->obstacleBounds, data->numObstacleBounds, dest->obstacleBounds))
			{
				for (auto i = 0; i < data->numObstacleBounds; i++)
				{
					auto* bounds = &data->obstacleBounds[i];
					if (const auto dest_hulls = write_array(buf, 7, bounds->m_Hulls, bounds->m_NumHulls, dest_bounds[i].m_Hulls))
					{
						for (auto j = 0; j < bounds->m_NumHulls; j++)
						{
							write_array(buf, 3, bounds->m_Hulls[j].m_Boundaries, bounds->m_Hulls[j].m_NumBoundaries, dest_hulls[j].m_Boundaries);
						}
					}
				}
			}

			if (const auto dest_glass_bounds = write_array(buf, 7, data->glassBounds, data->numGlassBounds, dest->glassBounds))
			{
				for (auto i = 0; i < data->numGlassBounds; i++)
				{
					write_array(buf, 3, data->glassBounds[i].m_Boundaries, data->glassBounds[i].m_NumBoundaries, dest_glass_bounds[i].m_Boundaries);
				}
			}

			if (const auto dest_modifiers = write_array(buf, 7, data->modifiers, data->numModifiers, dest->modifiers))
			{
				for (auto i = 0; i < data->numModifiers; i++)
				{
					write_array(buf, 3, data->modifiers[i].m_Boundaries, data->modifiers[i].m_NumBoundaries, dest_modifiers[i].m_Boundaries);
				}
			}

			write_array(buf, 3, data->linkCreationData, data->numLinkCreationData, dest->linkCreationData);
			write_array(buf, 3, data->volumeSeeds, data->numVolumeSeeds, dest->volumeSeeds);
			write_array(buf, 3, data->rawVolumes, data->numRawVolumes, dest->rawVolumes);

			if (const auto dest_custom_volumes = write_array(buf, 7, data->rawCustomVolumes, data->numRawCustomVolumes, dest->rawCustomVolumes))
			{
				for (auto i = 0; i < data->numRawCustomVolumes; i++)
				{
					write_array(buf, 3, data->rawCustomVolumes[i].m_Tris, data->rawCustomVolumes[i].m_NumTris, dest_custom_volumes[i].m_Tris);
				}
			}

			buf->pop_stream();
		}

		static void dump(NavMeshData* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_NAVMESH, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);

			write.dump_array(asset->navResources, asset->numNavResources);
			for (auto i = 0; asset->navResources && i < asset->numNavResources; i++)
			{
				auto* resource = &asset->navResources[i];
				dump_script_string(write, resource->targetName);
				write.dump_array(resource->pGraphBuffer, resource->graphSize);
				write.dump_array(resource->pWorkingGraph, working_graph_size(resource));
			}

			write.dump_array(asset->obstacleBounds, asset->numObstacleBounds);
			for (auto i = 0; asset->obstacleBounds && i < asset->numObstacleBounds; i++)
			{
				auto* bounds = &asset->obstacleBounds[i];
				write.dump_array(bounds->m_Hulls, bounds->m_NumHulls);
				for (auto j = 0; bounds->m_Hulls && j < bounds->m_NumHulls; j++)
				{
					write.dump_array(bounds->m_Hulls[j].m_Boundaries, bounds->m_Hulls[j].m_NumBoundaries);
				}
			}

			write.dump_array(asset->glassBounds, asset->numGlassBounds);
			for (auto i = 0; asset->glassBounds && i < asset->numGlassBounds; i++)
			{
				write.dump_array(asset->glassBounds[i].m_Boundaries, asset->glassBounds[i].m_NumBoundaries);
			}

			write.dump_array(asset->modifiers, asset->numModifiers);
			for (auto i = 0; asset->modifiers && i < asset->numModifiers; i++)
			{
				write.dump_array(asset->modifiers[i].m_Boundaries, asset->modifiers[i].m_NumBoundaries);
				dump_script_string(write, asset->modifiers[i].m_TargetName);
			}

			write.dump_array(asset->linkCreationData, asset->numLinkCreationData);
			for (auto i = 0; asset->linkCreationData && i < asset->numLinkCreationData; i++)
			{
				dump_script_string(write, asset->linkCreationData[i].m_Animscript);
				dump_script_string(write, asset->linkCreationData[i].m_Target);
				dump_script_string(write, asset->linkCreationData[i].m_Parent);
			}

			write.dump_array(asset->volumeSeeds, asset->numVolumeSeeds);
			write.dump_array(asset->rawVolumes, asset->numRawVolumes);

			write.dump_array(asset->rawCustomVolumes, asset->numRawCustomVolumes);
			for (auto i = 0; asset->rawCustomVolumes && i < asset->numRawCustomVolumes; i++)
			{
				write.dump_array(asset->rawCustomVolumes[i].m_Tris, asset->rawCustomVolumes[i].m_NumTris);
			}
			write.close();
		}

	private:
		static int working_graph_size(const nav_resource_s* resource)
		{
			return resource->bDockable ? resource->graphSize : 0;
		}
	};
}
