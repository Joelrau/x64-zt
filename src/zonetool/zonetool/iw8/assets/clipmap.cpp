#include <std_include.hpp>
#include "clipmap.hpp"

#include "../common/havok.hpp"

namespace zonetool::iw8
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

		asset->staticModelCollisionModelLists = read.read_array<StaticModelCollisionModelList>();
		for (unsigned int i = 0; i < asset->numStaticModelCollisionModelLists; i++)
		{
			if (asset->staticModelCollisionModelLists[i].models)
			{
				asset->staticModelCollisionModelLists[i].models = read.read_array<StaticModelCollisionModel>();
				for (unsigned int model_i = 0; model_i < asset->numStaticModelCollisionModelLists; model_i++)
				{
					auto* model = &asset->staticModelCollisionModelLists[i].models[model_i];
					this->add_script_string(&model->name, read.read_string());
					if (model->physicsAsset)
						model->physicsAsset = read.read_asset<PhysicsAsset>();
					if (model->physicsAsset)
						model->detailCollision = read.read_asset<XModelDetailCollision>();
					if (model->instances)
						model->instances = read.read_array<StaticModelCollisionInstance>();
				}
			}
		}

		asset->mapEnts = read.read_asset<MapEnts>();

		asset->stages = read.read_array<Stage>();
		for (int i = 0; i < asset->stageCount; i++)
		{
			asset->stages[i].name = read.read_string();
		}

		if (asset->stageTrigger.models)
		{
			asset->stageTrigger.models = read.read_array<TriggerModel>();
			for (unsigned int i = 0; i < asset->stageTrigger.count; i++)
			{
				if (asset->stageTrigger.models[i].physicsAsset)
				{
					asset->stageTrigger.models[i].physicsAsset = read.read_asset<PhysicsAsset>();
				}
			}
		}

		if (asset->stageTrigger.hulls)
		{
			asset->stageTrigger.hulls = read.read_array<TriggerHull>();
		}

		if (asset->stageTrigger.slabs)
		{
			asset->stageTrigger.slabs = read.read_array<TriggerSlab>();
		}

		if (asset->stageTrigger.windings)
		{
			asset->stageTrigger.windings = read.read_array<TriggerWinding>();
		}

		if (asset->stageTrigger.windingPoints)
		{
			asset->stageTrigger.windingPoints = read.read_array<TriggerWindingPoint>();
		}

		const auto havok_data_path = path;
		asset->havokWorldShapeData = havok::binary::parse_havok_data(havok_data_path, &asset->havokWorldShapeDataSize, mem);

		asset->collisionHeatmap = read.read_array<CollisionHeatmapEntry>();

		asset->topDownMapData = read.read_single<TopDownMapData>();

		read.close();

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
		auto* data = this->asset_;

		if (data->numStaticModelCollisionModelLists)
		{
			for (unsigned int i = 0; i < data->numStaticModelCollisionModelLists; i++)
			{
				if (data->staticModelCollisionModelLists[i].models)
				{
					for (unsigned int j = 0; j < data->staticModelCollisionModelLists[i].numModels; j++)
					{
						auto* model = &data->staticModelCollisionModelLists[i].models[j];
						model->name = static_cast<scr_string_t>(buf->write_scriptstring(
							this->get_script_string(&model->name)));
					}
				}
			}
		}
	}

	void clip_map::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		if (data->staticModelCollisionModelLists->models)
		{
			for (unsigned int i = 0; i < data->staticModelCollisionModelLists->numModels; i++)
			{
				if (data->staticModelCollisionModelLists->models[i].physicsAsset)
				{
					zone->add_asset_of_type(ASSET_TYPE_PHYSICSASSET, data->staticModelCollisionModelLists->models[i].physicsAsset->name);
				}

				if (data->staticModelCollisionModelLists->models[i].detailCollision)
				{
					zone->add_asset_of_type(ASSET_TYPE_XMODEL_DETAIL_COLLISION, data->staticModelCollisionModelLists->models[i].detailCollision->name);
				}
			}
		}

		if (data->mapEnts)
		{
			zone->add_asset_of_type(ASSET_TYPE_MAP_ENTS, this->asset_->mapEnts->name);
		}

		if (data->stageTrigger.models)
		{
			for (unsigned int i = 0; i < data->stageTrigger.count; i++)
			{
				if (data->stageTrigger.models[i].physicsAsset)
				{
					zone->add_asset_of_type(ASSET_TYPE_PHYSICSASSET, data->stageTrigger.models[i].physicsAsset->name);
				}
			}
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

	void clip_map::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name_);

		if (data->staticModelCollisionModelLists)
		{
			buf->align(7);
			dest->staticModelCollisionModelLists = buf->write(data->staticModelCollisionModelLists, data->numStaticModelCollisionModelLists);
			for (unsigned int i = 0; i < data->numStaticModelCollisionModelLists; i++)
			{
				// Load_StaticModelCollisionModelList
				if (data->staticModelCollisionModelLists[i].models)
				{
					buf->align(7);
					dest->staticModelCollisionModelLists[i].models = buf->write(data->staticModelCollisionModelLists[i].models, data->staticModelCollisionModelLists[i].numModels);
					for (unsigned int j = 0; j < data->staticModelCollisionModelLists[i].numModels; j++)
					{
						// Load_StaticModelCollisionModel
						auto* model = &data->staticModelCollisionModelLists[i].models[j];

						//if (model->name)
						//	dest->staticModelCollisionModelLists[i].models[j].name = static_cast<scr_string_t>(buf->write_scriptstring(
						//		this->get_script_string(&model->name)));

						if (model->physicsAsset)
						{
							model->physicsAsset = reinterpret_cast<PhysicsAsset*>(
								zone->get_asset_pointer(ASSET_TYPE_PHYSICSASSET, model->physicsAsset->name));
						}

						if (model->detailCollision)
						{
							model->detailCollision = reinterpret_cast<XModelDetailCollision*>(
								zone->get_asset_pointer(ASSET_TYPE_XMODEL_DETAIL_COLLISION, model->detailCollision->name));
						}

						if (model->instances)
						{
							buf->align(3);
							buf->write(model->instances, model->numInstances);
							buf->clear_pointer(&dest->staticModelCollisionModelLists[i].models[j].instances);
						}
					}

					buf->clear_pointer(&dest->staticModelCollisionModelLists[i].models);
				}
			}

			buf->clear_pointer(&dest->staticModelCollisionModelLists);
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

		// stageTrigger is now in clipMap_t instead of MapEnts
		if (dest->stageTrigger.models)
		{
			TriggerModel* dest_models;
			dest->stageTrigger.models = buf->write_s(7, dest->stageTrigger.models, dest->stageTrigger.count, sizeof(TriggerModel), &dest_models);
			if (reinterpret_cast<std::uint64_t>(dest->stageTrigger.models) == buf->data_following)
			{
				for (unsigned int i = 0; i < dest->stageTrigger.count; i++)
				{
					if (dest_models[i].physicsAsset)
					{
						dest_models[i].physicsAsset = reinterpret_cast<PhysicsAsset*>(
							zone->get_asset_pointer(ASSET_TYPE_PHYSICSASSET, dest_models[i].physicsAsset->name));
					}
				}

			}
		}

		if (dest->stageTrigger.hulls)
		{
			dest->stageTrigger.hulls = buf->write_s(3, dest->stageTrigger.hulls, dest->stageTrigger.hullCount);
		}

		if (dest->stageTrigger.slabs)
		{
			dest->stageTrigger.slabs = buf->write_s(3, dest->stageTrigger.slabs, dest->stageTrigger.slabCount);
		}

		if (dest->stageTrigger.windings)
		{
			dest->stageTrigger.windings = buf->write_s(3, dest->stageTrigger.windings, dest->stageTrigger.windingCount);
		}

		if (dest->stageTrigger.windingPoints)
		{
			dest->stageTrigger.windingPoints = buf->write_s(3, dest->stageTrigger.windingPoints, dest->stageTrigger.windingPointCount);
		}

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

		write.dump_array(asset->staticModelCollisionModelLists, asset->numStaticModelCollisionModelLists);
		for (unsigned int i = 0; i < asset->numStaticModelCollisionModelLists; i++)
		{
			// Load_StaticModelCollisionModelPtr
			if (asset->staticModelCollisionModelLists[i].models)
			{
				write.dump_array(asset->staticModelCollisionModelLists[i].models, asset->staticModelCollisionModelLists[i].numModels);
				for (unsigned int model_i = 0; model_i < asset->numStaticModelCollisionModelLists; model_i++)
				{
					// Load_StaticModelCollisionModel
					auto* model = &asset->staticModelCollisionModelLists[i].models[model_i];
					write.dump_string(SL_ConvertToString(model->name));
					if (model->physicsAsset)
						write.dump_asset(model->physicsAsset);
					if (model->physicsAsset)
						write.dump_asset(model->detailCollision);
					if (model->instances)
						write.dump_array(model->instances, model->numInstances);
				}
			}
		}
		
		write.dump_asset(asset->mapEnts);

		write.dump_array(asset->stages, asset->stageCount);
		for (unsigned char i = 0; i < asset->stageCount; i++)
		{
			write.dump_string(asset->stages[i].name);
		}

		// Load_MapTriggers
		if (asset->stageTrigger.models)
		{
			write.dump_array(asset->stageTrigger.models, asset->stageTrigger.count);
			for (unsigned int i = 0; i < asset->stageTrigger.count; i++)
			{
				auto* model = &asset->stageTrigger.models[i];
				write.dump_asset(model->physicsAsset);
			}
		}

		if (asset->stageTrigger.hulls)
			write.dump_array(asset->stageTrigger.hulls, asset->stageTrigger.hullCount);

		if (asset->stageTrigger.slabs)
			write.dump_array(asset->stageTrigger.slabs, asset->stageTrigger.slabCount);

		if (asset->stageTrigger.windings)
			write.dump_array(asset->stageTrigger.windings, asset->stageTrigger.windingCount);

		if (asset->stageTrigger.windingPoints)
			write.dump_array(asset->stageTrigger.windingPoints, asset->stageTrigger.windingPointCount);

		const auto havok_data_path = path;
		havok::binary::dump_havok_data(havok_data_path, asset->havokWorldShapeData, asset->havokWorldShapeDataSize);

		write.dump_array(asset->collisionHeatmap, asset->numCollisionHeatmapEntries);

		write.dump_single(asset->topDownMapData);

		write.close();
	}
}
