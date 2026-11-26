#include <std_include.hpp>
#include "streaminginfo.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_flag_set(Walk walk, AlwaysloadedFlagSet& set)
		{
			walk_asset_array(walk, ASSET_TYPE_IMAGE, set.images, set.imageCount + set.unknownImageCount);
			walk.array(set.imageFlags, (8 * set.imageCount + 31) >> 5, 3);
			walk_asset_array(walk, ASSET_TYPE_XMODEL_SURFS, set.modelSurfs, set.modelSurfCount);
		}

		template <typename Walk>
		void walk_transient_sets(Walk walk, TransientSets& sets)
		{
			if (const auto names = walk.array(sets.names, sets.count, 7))
			{
				for (auto i = 0u; i < sets.count; i++)
				{
					walk.string(names[i]);
				}
			}

			walk.array(sets.masks, sets.count, 3);
		}

		template <typename Walk>
		void walk_transient_costs(Walk walk, TransientCosts& costs)
		{
			const auto count = costs.transientCostCount;
			walk.array(costs.streamingCosts, costs.streamingItemCount, 3);

			auto& set = costs.streamContents;
			walk.array(set.contentCounts, count, 1);

			if (const auto contents = walk.array(set.contents, count, 7))
			{
				for (auto i = 0u; i < count; i++)
				{
					walk.array(contents[i].contents, set.contentCounts[i], 1);
				}
			}

			walk.array(costs.residentPageSizes, count, 1);
			walk.array(costs.tempPageSizes, count, 1);
		}

		template <typename Walk, typename T>
		void walk_named_references(Walk walk, T*& field, const unsigned int count)
		{
			if (const auto references = walk.array(field, count, 7))
			{
				for (auto i = 0u; i < count; i++)
				{
					walk.string(references[i].name);
				}
			}
		}

		template <typename T>
		void rename_references(T* references, const unsigned int count, zone_memory* mem)
		{
			for (auto i = 0u; references && i < count; i++)
			{
				if (!references[i].name)
				{
					continue;
				}

				const auto renamed = renamed_asset_name(references[i].name);
				if (renamed != references[i].name)
				{
					references[i].name = mem->duplicate_string(renamed);
				}
			}
		}

		void rename_transient_references(TransientInfo& info, zone_memory* mem)
		{
			rename_references(info.forcedStubAssets, info.forcedStubAssetCount, mem);
			rename_references(info.transientFiles, info.transientFileCount, mem);
			rename_references(info.transientAssets, info.transientAssetCount, mem);
		}

		template <typename Walk>
		void walk_transient_info(Walk walk, TransientInfo& info)
		{
			walk_transient_sets(walk, info.transientSets);
			walk_transient_costs(walk, info.transientCosts);
			walk_named_references(walk, info.forcedStubAssets, info.forcedStubAssetCount);
			walk_named_references(walk, info.transientFiles, info.transientFileCount);
			walk_named_references(walk, info.transientAssets, info.transientAssetCount);

			if (const auto maps = walk.array(info.transientWorldGridTileMaps, info.transientWorldGridTileMapCount, 7))
			{
				for (auto i = 0u; i < info.transientWorldGridTileMapCount; i++)
				{
					walk.array(maps[i].spaceToTransientIndex, maps[i].indexCount, 1);
				}
			}

			walk_spatial_tree(walk, info.transientWorldGridSpatial);
			walk.array(info.transientWorldGridBounds, info.transientWorldGridBoundsCount, 3);
			walk.array(info.transientWorldGridHighLodLookup, info.transientWorldGridHighLodLookupCount, 1);
			walk.array(info.transientSkipLoading.array, info.transientSkipLoading.wordCount, 3);
		}

		template <typename Walk>
		void walk_streaming_info(Walk walk, StreamingInfo& asset)
		{
			walk_flag_set(walk, asset.alwaysloadedFlagSet);

			if (const auto info = walk.array(asset.transientInfo, 1, 7))
			{
				walk_transient_info(walk, *info);
			}
		}
	}

	void streaming_info::read(zone_reader& reader, StreamingInfo* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_streaming_info(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void streaming_info::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_streaming_info(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void streaming_info::dump(StreamingInfo* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("streaminginfo\\"s + asset->name + ".streaminginfob"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_streaming_info(dump_walk{dump}, *asset);
		dump.close();
	}

	StreamingInfo* streaming_info::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("streaminginfo\\"s + name + ".streaminginfob"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing streaminginfo \"%s\"...", name.data());

		auto* asset = read.read_single<StreamingInfo>();
		asset->name = read.read_string();
		walk_streaming_info(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void streaming_info::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<StreamingInfo>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (this->asset_ && this->asset_->transientInfo)
		{
			rename_transient_references(*this->asset_->transientInfo, mem);
		}
	}

	void streaming_info::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_streaming_info(depend_walk{zone}, *this->asset_);
	}

	std::string streaming_info::name()
	{
		return this->name_;
	}

	std::int32_t streaming_info::type()
	{
		return ASSET_TYPE_STREAMING_INFO;
	}
}
