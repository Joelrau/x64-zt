#include <std_include.hpp>
#include "clipmap.hpp"

#include "../common/havok.hpp"

namespace zonetool::iw7
{
	void clip_map::add_script_string(scr_string_t* ptr, const char* str)
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

	const char* clip_map::get_script_string(scr_string_t* ptr)
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

	void clip_map::parse_info(ClipInfo* info, assetmanager::reader& read, zone_memory* mem)
	{
		info->planes = read.read_array<cplane_s>();
	}

	clipMap_t* clip_map::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = name + ".colmap";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing colmap \"%s\"...", name.data());

		clipMap_t* asset = read.read_single<clipMap_t>();
		asset->name = read.read_string();

		parse_info(&asset->info, read, mem);

		asset->staticModelList = read.read_array<cStaticModel_s>();
		for (unsigned int i = 0; i < asset->numStaticModels; i++)
		{
			if (asset->staticModelList[i].xmodel)
			{
				asset->staticModelList[i].xmodel = read.read_asset<XModel>();
			}
		}

		asset->unk01.unk = read.read_array<int>();
		asset->unk02 = read.read_array<unk_1453E2338>();
		for (unsigned int i = 0; i < asset->numUnk02; i++)
		{
			asset->unk02[i].unk = read.read_array<int>();
		}

		asset->mapEnts = read.read_asset<MapEnts>();

		asset->stages = read.read_array<Stage>();
		for (int i = 0; i < asset->stageCount; i++)
		{
			asset->stages[i].name = read.read_string();
		}

		const auto havok_data_path = path;
		asset->havokWorldShapeData = havok::binary::parse_havok_data(havok_data_path, &asset->havokWorldShapeDataSize, mem);

		asset->collisionHeatmap = read.read_array<CollisionHeatmapEntry>();

		asset->topDownMapData = read.read_single<TopDownMapData>();

		read.close();

		// copy info into pInfo
		//asset->pInfo = &asset->info;
		asset->pInfo = mem->allocate<ClipInfo>();
		memcpy(asset->pInfo, &asset->info, sizeof(ClipInfo));

		return asset;
	}

	void clip_map::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;
		this->asset_ = this->parse(name, mem);

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse clipmap \"%s\"", name.data());
		}
	}

	void clip_map::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void clip_map::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		if (data->staticModelList)
		{
			for (unsigned int i = 0; i < data->numStaticModels; i++)
			{
				if (data->staticModelList[i].xmodel)
				{
					zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->staticModelList[i].xmodel->name);
				}
			}
		}

		if (data->mapEnts)
		{
			zone->add_asset_of_type(ASSET_TYPE_MAP_ENTS, this->asset_->mapEnts->name);
		}
	}

	std::string clip_map::name()
	{
		return this->name_;
	}

	std::int32_t clip_map::type()
	{
		return ASSET_TYPE_CLIPMAP;
	}

	void clip_map::write_info(zone_base* zone, zone_buffer* buf, ClipInfo* data, ClipInfo* dest)
	{
		if (data->planes)
		{
			dest->planes = buf->write_s(3, data->planes, data->planeCount);
		}
	}

	void clip_map::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name_);

		clip_map::write_info(zone, buf, &data->info, &dest->info);

		buf->push_stream(XFILE_BLOCK_TEMP);
		if (data->pInfo)
		{
			buf->align(7);
			auto destInfo = buf->write(data->pInfo);
			clip_map::write_info(zone, buf, data->pInfo, destInfo);
			buf->clear_pointer(&dest->pInfo);
		}
		buf->pop_stream();

		if (data->staticModelList)
		{
			buf->align(7);
			auto static_model = buf->write(data->staticModelList, data->numStaticModels);
			for (unsigned int i = 0; i < data->numStaticModels; i++)
			{
				if (data->staticModelList[i].xmodel)
				{
					static_model[i].xmodel = reinterpret_cast<XModel*>(zone->get_asset_pointer(
						ASSET_TYPE_XMODEL, data->staticModelList[i].xmodel->name));
				}
			}
			buf->clear_pointer(&dest->staticModelList);
		}

		if (data->unk01.unk)
		{
			buf->align(3);
			buf->write(data->unk01.unk, data->unk01.num);
			buf->clear_pointer(&dest->unk01.unk);
		}

		if (data->unk02)
		{
			buf->align(7);
			dest->unk02 = buf->write(data->unk02, data->numUnk02);
			for (unsigned int i = 0; i < data->numUnk02; i++)
			{
				if (data->unk02[i].unk)
				{
					buf->align(3);
					buf->write(data->unk02[i].unk, data->unk02[i].num);
					buf->clear_pointer(&dest->unk02[i].unk);
				}
			}
			buf->clear_pointer(&dest->unk02);
		}

		if (data->mapEnts)
		{
			dest->mapEnts = reinterpret_cast<MapEnts*>(zone->get_asset_pointer(ASSET_TYPE_MAP_ENTS, this->name()));
		}

		if (data->stages)
		{
			buf->align(7);
			auto destStages = buf->write(data->stages, data->stageCount);

			for (unsigned char i = 0; i < data->stageCount; i++)
			{
				if (data->stages[i].name)
				{
					destStages[i].name = buf->write_str(data->stages[i].name);
				}
			}

			buf->clear_pointer(&dest->stages);
		}

		// copy trigger data from mapents
		auto* mapEntsAsset = zone->find_asset(ASSET_TYPE_MAP_ENTS, this->name());
		auto* mapEnts = mapEntsAsset ? reinterpret_cast<MapEnts*>(mapEntsAsset->pointer()) : nullptr;
		if (mapEnts)
		{
			memcpy(&dest->stageTrigger, &mapEnts->trigger, sizeof(MapTriggers));
		}
		else
		{
			memset(&dest->stageTrigger, 0, sizeof(MapTriggers));
		}

		map_ents::write_triggers(zone, buf, &dest->stageTrigger);

		if (data->havokWorldShapeData)
		{
			buf->align(15);
			buf->write(data->havokWorldShapeData, data->havokWorldShapeDataSize);
			buf->clear_pointer(&dest->havokWorldShapeData);
		}

		if (data->collisionHeatmap)
		{
			buf->align(3);
			buf->write(data->collisionHeatmap, data->numCollisionHeatmapEntries);
			buf->clear_pointer(&dest->collisionHeatmap);
		}

		if (data->topDownMapData)
		{
			dest->topDownMapData = buf->write_s(3, data->topDownMapData);
		}

		buf->pop_stream();
	}

	void clip_map::dump_info(ClipInfo* info, assetmanager::dumper& write)
	{
		write.dump_array(info->planes, info->planeCount);
	}

	void clip_map::dump(clipMap_t* asset)
	{
		const auto path = asset->name + ".colmap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		dump_info(&asset->info, write);

		write.dump_array(asset->staticModelList, asset->numStaticModels);
		for (unsigned int i = 0; i < asset->numStaticModels; i++)
		{
			write.dump_asset(asset->staticModelList[i].xmodel);
		}

		write.dump_array(asset->unk01.unk, asset->unk01.num);
		write.dump_array(asset->unk02, asset->numUnk02);
		for (unsigned int i = 0; i < asset->numUnk02; i++)
		{
			write.dump_array(asset->unk02[i].unk, asset->unk02[i].num);
		}
		
		write.dump_asset(asset->mapEnts);

		write.dump_array(asset->stages, asset->stageCount);
		for (unsigned char i = 0; i < asset->stageCount; i++)
		{
			write.dump_string(asset->stages[i].name);
		}

		const auto havok_data_path = path;
		havok::binary::dump_havok_data(havok_data_path, asset->havokWorldShapeData, asset->havokWorldShapeDataSize);

		write.dump_array(asset->collisionHeatmap, asset->numCollisionHeatmapEntries);

		write.dump_single(asset->topDownMapData);

		write.close();
	}
}