#include <std_include.hpp>
#include "navmesh.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_nav_mesh(Walk walk, NavMeshData& asset)
		{
			if (const auto resources = walk.array(asset.navResources, asset.numNavResources, 7))
			{
				for (auto i = 0; i < asset.numNavResources; i++)
				{
					auto& resource = resources[i];
					walk.script_string(resource.targetName);
					walk.array(resource.pGraphBuffer, resource.graphSize, 3);
					walk.array(resource.pWorkingGraph, resource.bDockable ? resource.graphSize : 0, 3);
				}
			}

			if (const auto bounds = walk.array(asset.obstacleBounds, asset.numObstacleBounds, 7))
			{
				for (auto i = 0; i < asset.numObstacleBounds; i++)
				{
					if (const auto hulls = walk.array(bounds[i].m_Hulls, bounds[i].m_NumHulls, 7))
					{
						for (auto j = 0; j < bounds[i].m_NumHulls; j++)
						{
							walk.array(hulls[j].m_Boundaries, hulls[j].m_NumBoundaries, 3);
						}
					}
				}
			}

			if (const auto glass = walk.array(asset.glassBounds, asset.numGlassBounds, 7))
			{
				for (auto i = 0; i < asset.numGlassBounds; i++)
				{
					walk.array(glass[i].m_Boundaries, glass[i].m_NumBoundaries, 3);
				}
			}

			if (const auto modifiers = walk.array(asset.modifiers, asset.numModifiers, 7))
			{
				for (auto i = 0; i < asset.numModifiers; i++)
				{
					auto& modifier = modifiers[i];
					walk.array(modifier.m_Boundaries, modifier.m_NumBoundaries, 3);
					walk.script_string(modifier.m_TargetName);
					walk.script_string(modifier.m_Target);
					walk.script_string(modifier.m_ScriptNoteworthy);
					walk.script_string(modifier.m_ScriptLinkName);
				}
			}

			if (const auto links = walk.array(asset.linkCreationData, asset.numLinkCreationData, 3))
			{
				for (auto i = 0; i < asset.numLinkCreationData; i++)
				{
					walk.script_string(links[i].m_Animscript);
					walk.script_string(links[i].m_Target);
					walk.script_string(links[i].m_Parent);
				}
			}

			walk.array(asset.volumeSeeds, asset.numVolumeSeeds, 3);
			walk.array(asset.rawVolumes, asset.numRawVolumes, 3);

			if (const auto volumes = walk.array(asset.rawCustomVolumes, asset.numRawCustomVolumes, 7))
			{
				for (auto i = 0; i < asset.numRawCustomVolumes; i++)
				{
					walk.array(volumes[i].m_Tris, volumes[i].m_NumTris, 3);
				}
			}
		}
	}

	void nav_mesh::read(zone_reader& reader, NavMeshData* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_nav_mesh(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void nav_mesh::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_nav_mesh(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void nav_mesh::dump(NavMeshData* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(asset->name + ".navmesh"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_nav_mesh(dump_walk{dump}, *asset);
		dump.close();
	}

	NavMeshData* nav_mesh::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(name + ".navmesh"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing navmesh \"%s\"...", name.data());

		auto* asset = read.read_single<NavMeshData>();
		asset->name = read.read_string();
		walk_nav_mesh(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void nav_mesh::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<NavMeshData>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void nav_mesh::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_nav_mesh(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string nav_mesh::name()
	{
		return this->name_;
	}

	std::int32_t nav_mesh::type()
	{
		return ASSET_TYPE_NAVMESH;
	}
}
