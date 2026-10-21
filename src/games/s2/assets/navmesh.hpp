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

			if (const auto regions = reader.read_array(asset->regions, 7, asset->regionCount))
			{
				for (auto i = 0; i < asset->regionCount; i++)
				{
					reader.read_script_string(regions[i].name);
					reader.read_array(regions[i].data, 3, regions[i].dataSize);
					reader.read_array(regions[i].extraData, 3, extra_data_size(&regions[i]));
				}
			}

			if (const auto edge_lists = reader.read_array(asset->edgeLists, 7, asset->edgeListCount))
			{
				for (auto i = 0; i < asset->edgeListCount; i++)
				{
					if (const auto edges = reader.read_array(edge_lists[i].edges, 7, edge_lists[i].edgeCount))
					{
						for (auto j = 0; j < edge_lists[i].edgeCount; j++)
						{
							reader.read_array(edges[j].points, 3, edges[j].pointCount);
						}
					}
				}
			}

			if (const auto polygons = reader.read_array(asset->polygons, 7, asset->polygonCount))
			{
				for (auto i = 0; i < asset->polygonCount; i++)
				{
					reader.read_array(polygons[i].points, 3, polygons[i].pointCount);
				}
			}

			if (const auto volumes = reader.read_array(asset->volumes, 7, asset->volumeCount))
			{
				for (auto i = 0; i < asset->volumeCount; i++)
				{
					reader.read_array(volumes[i].points, 3, volumes[i].pointCount);
					reader.read_script_string(volumes[i].name);
				}
			}

			if (const auto links = reader.read_array(asset->links, 3, asset->linkCount))
			{
				for (auto i = 0; i < asset->linkCount; i++)
				{
					reader.read_script_strings(links[i].names, std::size(links[i].names));
				}
			}

			reader.read_array(asset->nodes, 3, asset->nodeCount);
			reader.read_array(asset->faces, 3, asset->faceCount);

			if (const auto clusters = reader.read_array(asset->clusters, 7, asset->clusterCount))
			{
				for (auto i = 0; i < asset->clusterCount; i++)
				{
					reader.read_array(clusters[i].data, 3, clusters[i].count);
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

			if (const auto regions = parse_array(read, mem, asset->regions, asset->regionCount))
			{
				for (auto i = 0; i < asset->regionCount; i++)
				{
					regions[i].name = parse_script_string(read);
					parse_array(read, mem, regions[i].data, regions[i].dataSize);
					parse_array(read, mem, regions[i].extraData, extra_data_size(&regions[i]));
				}
			}

			if (const auto edge_lists = parse_array(read, mem, asset->edgeLists, asset->edgeListCount))
			{
				for (auto i = 0; i < asset->edgeListCount; i++)
				{
					if (const auto edges = parse_array(read, mem, edge_lists[i].edges, edge_lists[i].edgeCount))
					{
						for (auto j = 0; j < edge_lists[i].edgeCount; j++)
						{
							parse_array(read, mem, edges[j].points, edges[j].pointCount);
						}
					}
				}
			}

			if (const auto polygons = parse_array(read, mem, asset->polygons, asset->polygonCount))
			{
				for (auto i = 0; i < asset->polygonCount; i++)
				{
					parse_array(read, mem, polygons[i].points, polygons[i].pointCount);
				}
			}

			if (const auto volumes = parse_array(read, mem, asset->volumes, asset->volumeCount))
			{
				for (auto i = 0; i < asset->volumeCount; i++)
				{
					parse_array(read, mem, volumes[i].points, volumes[i].pointCount);
					volumes[i].name = parse_script_string(read);
				}
			}

			if (const auto links = parse_array(read, mem, asset->links, asset->linkCount))
			{
				for (auto i = 0; i < asset->linkCount; i++)
				{
					for (auto& link_name : links[i].names)
					{
						link_name = parse_script_string(read);
					}
				}
			}

			parse_array(read, mem, asset->nodes, asset->nodeCount);
			parse_array(read, mem, asset->faces, asset->faceCount);

			if (const auto clusters = parse_array(read, mem, asset->clusters, asset->clusterCount))
			{
				for (auto i = 0; i < asset->clusterCount; i++)
				{
					parse_array(read, mem, clusters[i].data, clusters[i].count);
				}
			}
			read.close();

			return asset;
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
			auto* data = this->asset_;
			script_string_preparer prepare(buf);

			for (auto i = 0; data->regions && i < data->regionCount; i++)
			{
				prepare(data->regions[i].name);
			}

			for (auto i = 0; data->volumes && i < data->volumeCount; i++)
			{
				prepare(data->volumes[i].name);
			}

			for (auto i = 0; data->links && i < data->linkCount; i++)
			{
				prepare(data->links[i].names, std::size(data->links[i].names));
			}
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);

			if (const auto dest_regions = write_array(buf, 7, data->regions, data->regionCount, dest->regions))
			{
				for (auto i = 0; i < data->regionCount; i++)
				{
					auto* region = &data->regions[i];
					write_array(buf, 3, region->data, region->dataSize, dest_regions[i].data);
					write_array(buf, 3, region->extraData, extra_data_size(region), dest_regions[i].extraData);
				}
			}

			if (const auto dest_edge_lists = write_array(buf, 7, data->edgeLists, data->edgeListCount, dest->edgeLists))
			{
				for (auto i = 0; i < data->edgeListCount; i++)
				{
					auto* edge_list = &data->edgeLists[i];
					if (const auto dest_edges = write_array(buf, 7, edge_list->edges, edge_list->edgeCount, dest_edge_lists[i].edges))
					{
						for (auto j = 0; j < edge_list->edgeCount; j++)
						{
							write_array(buf, 3, edge_list->edges[j].points, edge_list->edges[j].pointCount, dest_edges[j].points);
						}
					}
				}
			}

			if (const auto dest_polygons = write_array(buf, 7, data->polygons, data->polygonCount, dest->polygons))
			{
				for (auto i = 0; i < data->polygonCount; i++)
				{
					write_array(buf, 3, data->polygons[i].points, data->polygons[i].pointCount, dest_polygons[i].points);
				}
			}

			if (const auto dest_volumes = write_array(buf, 7, data->volumes, data->volumeCount, dest->volumes))
			{
				for (auto i = 0; i < data->volumeCount; i++)
				{
					write_array(buf, 3, data->volumes[i].points, data->volumes[i].pointCount, dest_volumes[i].points);
				}
			}

			write_array(buf, 3, data->links, data->linkCount, dest->links);
			write_array(buf, 3, data->nodes, data->nodeCount, dest->nodes);
			write_array(buf, 3, data->faces, data->faceCount, dest->faces);

			if (const auto dest_clusters = write_array(buf, 7, data->clusters, data->clusterCount, dest->clusters))
			{
				for (auto i = 0; i < data->clusterCount; i++)
				{
					write_array(buf, 3, data->clusters[i].data, data->clusters[i].count, dest_clusters[i].data);
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

			write.dump_array(asset->regions, asset->regionCount);
			for (auto i = 0; asset->regions && i < asset->regionCount; i++)
			{
				auto* region = &asset->regions[i];
				dump_script_string(write, region->name);
				write.dump_array(region->data, region->dataSize);
				write.dump_array(region->extraData, extra_data_size(region));
			}

			write.dump_array(asset->edgeLists, asset->edgeListCount);
			for (auto i = 0; asset->edgeLists && i < asset->edgeListCount; i++)
			{
				auto* edge_list = &asset->edgeLists[i];
				write.dump_array(edge_list->edges, edge_list->edgeCount);
				for (auto j = 0; edge_list->edges && j < edge_list->edgeCount; j++)
				{
					write.dump_array(edge_list->edges[j].points, edge_list->edges[j].pointCount);
				}
			}

			write.dump_array(asset->polygons, asset->polygonCount);
			for (auto i = 0; asset->polygons && i < asset->polygonCount; i++)
			{
				write.dump_array(asset->polygons[i].points, asset->polygons[i].pointCount);
			}

			write.dump_array(asset->volumes, asset->volumeCount);
			for (auto i = 0; asset->volumes && i < asset->volumeCount; i++)
			{
				write.dump_array(asset->volumes[i].points, asset->volumes[i].pointCount);
				dump_script_string(write, asset->volumes[i].name);
			}

			write.dump_array(asset->links, asset->linkCount);
			for (auto i = 0; asset->links && i < asset->linkCount; i++)
			{
				dump_script_strings(write, asset->links[i].names, std::size(asset->links[i].names));
			}

			write.dump_array(asset->nodes, asset->nodeCount);
			write.dump_array(asset->faces, asset->faceCount);

			write.dump_array(asset->clusters, asset->clusterCount);
			for (auto i = 0; asset->clusters && i < asset->clusterCount; i++)
			{
				write.dump_array(asset->clusters[i].data, asset->clusters[i].count);
			}
			write.close();
		}

	private:
		static int extra_data_size(const NavMeshRegion* region)
		{
			return region->hasExtraData ? region->dataSize : 0;
		}
	};
}
