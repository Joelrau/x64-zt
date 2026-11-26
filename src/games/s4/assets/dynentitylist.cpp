#include <std_include.hpp>
#include "dynentitylist.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_dyn_entity_defs(Walk walk, DynEntityDef*& field, const unsigned int count)
		{
			if (const auto defs = walk.array(field, count, 7))
			{
				for (auto i = 0u; i < count; i++)
				{
					walk.asset(ASSET_TYPE_XMODEL, defs[i].baseModel);
					walk.array(defs[i].linkTo, 1, 3);
				}
			}
		}

		template <typename Walk>
		void walk_transient_population(Walk walk, SpatialPartition_TransientPopulation*& field)
		{
			if (const auto population = walk.array(field, 1, 7))
			{
				walk.array(population->buckets, population->partitionCount, 3);
				walk.array(population->bucketTails, population->partitionCount, 3);
				walk.array(population->partitionIds, population->partitionCount, 3);
			}
		}

		template <typename Walk>
		void walk_dyn_entity_list(Walk walk, DynEntityList& asset)
		{
			walk_dyn_entity_defs(walk, asset.dynEntDefList[0], asset.dynEntCount[0]);
			walk_dyn_entity_defs(walk, asset.dynEntDefList[1], asset.dynEntCount[1]);

			for (auto& population : asset.dynEntSpatialPopulation)
			{
				walk_transient_population(walk, population);
			}
		}
	}

	void dyn_entity_list::read(zone_reader& reader, DynEntityList* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_dyn_entity_list(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void dyn_entity_list::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_dyn_entity_list(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void dyn_entity_list::dump(DynEntityList* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(asset->name + ".dynentitylist"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_dyn_entity_list(dump_walk{dump}, *asset);
		dump.close();
	}

	DynEntityList* dyn_entity_list::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(name + ".dynentitylist"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing dynentitylist \"%s\"...", name.data());

		auto* asset = read.read_single<DynEntityList>();
		asset->name = read.read_string();
		walk_dyn_entity_list(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void dyn_entity_list::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<DynEntityList>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void dyn_entity_list::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_dyn_entity_list(depend_walk{zone}, *this->asset_);
	}

	std::string dyn_entity_list::name()
	{
		return this->name_;
	}

	std::int32_t dyn_entity_list::type()
	{
		return ASSET_TYPE_DYN_ENTITY_LIST;
	}
}
