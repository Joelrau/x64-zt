#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_collision_tree(zone_reader& reader, XSurfaceCollisionLeaf* tree)
		{
			reader.read_array(tree->nodes, 15, tree->nodeCount);
			reader.read_array(tree->leafs, 1, tree->leafCount);
		}

		void read_surface(zone_reader& reader, XSurface* surface)
		{
			reader.read_asset(ASSET_TYPE_XSURFACE_SHARED, surface->shared);
			reader.read_stream(surface->triIndices, 15, 6, surface->triCount);

			if (const auto lists = reader.read_array(surface->rigidVertLists, 7, surface->rigidVertListCount))
			{
				for (auto i = 0; i < surface->rigidVertListCount; i++)
				{
					if (const auto tree = reader.read_single(lists[i].collisionTree, 7))
					{
						read_collision_tree(reader, tree);
					}
				}
			}

			reader.read_array(surface->verts, 15, surface->vertCount);
		}
	}

	void xmodel_surfs::read_surfaces(zone_reader& reader, XSurface* surfaces, const std::size_t count)
	{
		for (std::size_t i = 0; i < count; i++)
		{
			read_surface(reader, &surfaces[i]);
		}
	}

	void xmodel_surfs::read(zone_reader& reader, XModelSurfs* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto surfaces = reader.read_array(asset->surfs, 7, asset->numsurfs))
		{
			read_surfaces(reader, surfaces, asset->numsurfs);
		}

		reader.pop_stream();
	}

	void xmodel_surfs::write_surfaces(zone_base* zone, zone_buffer* buf, XSurface* surfaces, XSurface* dest, const std::size_t count)
	{
		for (std::size_t i = 0; i < count; i++)
		{
			auto* surface = &surfaces[i];
			auto* dest_surface = &dest[i];

			dest_surface->shared = surface->shared
				? static_cast<XSurfaceShared*>(zone->get_asset_pointer(ASSET_TYPE_XSURFACE_SHARED, surface_shared_key(surface->shared)))
				: nullptr;

			if (surface->triIndices)
			{
				buf->align(15);
				buf->write(surface->triIndices, 3 * surface->triCount);
				buf->clear_pointer(&dest_surface->triIndices);
			}

			if (surface->rigidVertLists)
			{
				buf->align(7);
				auto* dest_lists = buf->write(surface->rigidVertLists, surface->rigidVertListCount);
				for (auto j = 0; j < surface->rigidVertListCount; j++)
				{
					auto* tree = surface->rigidVertLists[j].collisionTree;
					if (!tree)
					{
						continue;
					}

					buf->align(7);
					auto* dest_tree = buf->write(tree);
					if (tree->nodes)
					{
						buf->align(15);
						buf->write(tree->nodes, tree->nodeCount);
						buf->clear_pointer(&dest_tree->nodes);
					}
					if (tree->leafs)
					{
						buf->align(1);
						buf->write(tree->leafs, tree->leafCount);
						buf->clear_pointer(&dest_tree->leafs);
					}
					buf->clear_pointer(&dest_lists[j].collisionTree);
				}
				buf->clear_pointer(&dest_surface->rigidVertLists);
			}

			if (surface->verts)
			{
				buf->align(15);
				buf->write(surface->verts, surface->vertCount);
				buf->clear_pointer(&dest_surface->verts);
			}
		}
	}

	void xmodel_surfs::add_shared_dependencies(zone_base* zone, XSurface* surfaces, const std::size_t count)
	{
		for (std::size_t i = 0; i < count; i++)
		{
			zone->add_asset_of_type_by_pointer(ASSET_TYPE_XSURFACE_SHARED, surfaces[i].shared);
		}
	}

	void xmodel_surfs::dump_surfaces(assetmanager::dumper& write, XSurface* surfaces, const std::size_t count)
	{
		std::unordered_set<std::string> dumped_shared;
		std::deque<std::string> keys;

		write.dump_array(surfaces, static_cast<std::uint32_t>(count));
		for (std::size_t i = 0; i < count; i++)
		{
			const auto* surface = &surfaces[i];
			const auto* shared = surface->shared;

			const auto& key = keys.emplace_back(shared ? surface_shared_key(shared) : std::string{});
			write.dump_string(key.data());
			if (shared && dumped_shared.insert(key).second)
			{
				write.dump_single(shared);
				write.dump_array(shared->flags & XSURFACE_SHARED_STREAMED ? nullptr : shared->data, shared->dataSize);
				write.dump_array(shared->subdivData, static_cast<std::uint32_t>(shared->subdivCount * surface_subdiv_size));
			}

			write.dump_array(surface->triIndices, 3 * surface->triCount);
			write.dump_array(surface->rigidVertLists, surface->rigidVertListCount);
			for (auto j = 0; j < surface->rigidVertListCount; j++)
			{
				const auto* tree = surface->rigidVertLists[j].collisionTree;
				write.dump_single(tree);
				if (tree)
				{
					write.dump_array(tree->nodes, tree->nodeCount);
					write.dump_array(tree->leafs, tree->leafCount);
				}
			}
			write.dump_array(surface->verts, surface->vertCount);
		}
	}

	void xmodel_surfs::parse_surfaces(assetmanager::reader& read, XSurface* surfaces, const std::size_t count)
	{
		std::unordered_map<std::string, XSurfaceShared*> parsed_shared;

		for (std::size_t i = 0; i < count; i++)
		{
			auto* surface = &surfaces[i];

			const std::string key = read.read_string();
			surface->shared = nullptr;
			if (!key.empty())
			{
				if (const auto found = parsed_shared.find(key); found != parsed_shared.end())
				{
					surface->shared = found->second;
				}
				else
				{
					auto* shared = read.read_single<XSurfaceShared>();
					shared->data = read.read_array<char>();
					shared->subdivData = read.read_array<char>();
					parsed_shared.emplace(key, shared);
					surface->shared = shared;
				}
			}

			surface->triIndices = read.read_array<unsigned short>();
			surface->rigidVertLists = read.read_array<XRigidVertList>();
			for (auto j = 0; j < surface->rigidVertListCount; j++)
			{
				auto* tree = read.read_single<XSurfaceCollisionLeaf>();
				if (tree)
				{
					tree->nodes = read.read_array<XSurfaceCollisionNode>();
					tree->leafs = read.read_array<unsigned short>();
				}
				surface->rigidVertLists[j].collisionTree = tree;
			}
			surface->verts = read.read_array<vec4_t>();
		}
	}

	XModelSurfs* xmodel_surfs::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_XMODEL_SURFS, name.data())))
		{
			return cast_xmodel::find_surfs(name, mem);
		}

		ZONETOOL_INFO("Parsing xmodelsurfs \"%s\"...", name.data());

		auto* asset = read.read_single<XModelSurfs>();
		asset->name = read.read_string();
		asset->surfs = read.read_array<XSurface>();
		parse_surfaces(read, asset->surfs, asset->surfs ? asset->numsurfs : 0);
		read.close();

		return asset;
	}

	void xmodel_surfs::load_depending(zone_base* zone)
	{
		add_shared_dependencies(zone, this->asset_->surfs, this->asset_->surfs ? this->asset_->numsurfs : 0);
	}

	void xmodel_surfs::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);

		if (data->surfs)
		{
			buf->align(7);
			auto* dest_surfaces = buf->write(data->surfs, data->numsurfs);
			write_surfaces(zone, buf, data->surfs, dest_surfaces, data->numsurfs);
			buf->clear_pointer(&dest->surfs);
		}

		buf->pop_stream();
	}

	void xmodel_surfs::dump(XModelSurfs* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_XMODEL_SURFS, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		dump_surfaces(write, asset->surfs, asset->surfs ? asset->numsurfs : 0);
		write.close();
	}
}
