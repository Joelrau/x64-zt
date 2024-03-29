#include <std_include.hpp>
#include "addonmapents.hpp"

namespace zonetool::h2
{
	void addon_map_ents::parse_triggers(zone_memory* mem, std::string name, MapTriggers* trigger)
	{
		const auto path = name + ".triggers"s;
		const auto file_path = filesystem::get_file_path(path);

		assetmanager::reader reader(mem);
		if (reader.open(path))
		{
			trigger->count = reader.read_int();
			trigger->models = reader.read_array<TriggerModel>();

			trigger->hullCount = reader.read_int();
			trigger->hulls = reader.read_array<TriggerHull>();

			trigger->slabCount = reader.read_int();
			trigger->slabs = reader.read_array<TriggerSlab>();

			reader.close();
		}
	}

	void addon_map_ents::parse_entity_strings(zone_memory* mem, std::string name, char** entityStrings, int* numEntityChars)
	{
		const auto path = name;
		auto file = filesystem::file(path);
		file.open("rb");
		if (!file.get_fp())
		{
			return;
		}

		*numEntityChars = static_cast<int>(file.size());

		*entityStrings = mem->allocate<char>(static_cast<size_t>(*numEntityChars + 1));
		memset(*entityStrings, 0, *numEntityChars);

		fread(*entityStrings, *numEntityChars, 1, file.get_fp());
		(*entityStrings)[*numEntityChars] = '\0';

		file.close();
	}

	AddonMapEnts* addon_map_ents::parse(std::string name, zone_memory* mem)
	{
		const auto path = name;

		if (!filesystem::file(path).exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing addonmapents \"%s\"...", name.data());

		AddonMapEnts* ents = mem->allocate<AddonMapEnts>();
		ents->name = mem->duplicate_string(name);

		parse_entity_strings(mem, name, &ents->entityString, &ents->numEntityChars);
		map_ents::convert_ents(reinterpret_cast<MapEnts*>(ents), mem);

		parse_triggers(mem, name, &ents->trigger);

		return ents;
	}

	void addon_map_ents::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") + filesystem::get_fastfile() + ".mapents"; // name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not find addonmapents: \"%s\"", this->name_.data());
		}
	}

	void addon_map_ents::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void addon_map_ents::load_depending(zone_base* zone)
	{
	}

	std::string addon_map_ents::name()
	{
		return this->name_;
	}

	std::int32_t addon_map_ents::type()
	{
		return ASSET_TYPE_ADDON_MAP_ENTS;
	}

	void addon_map_ents::write_triggers(zone_buffer* buf, MapTriggers* dest)
	{
		if (dest->models)
		{
			dest->models = buf->write_s(3, dest->models, dest->count);
		}

		if (dest->hulls)
		{
			dest->hulls = buf->write_s(3, dest->hulls, dest->hullCount);
		}

		if (dest->slabs)
		{
			dest->slabs = buf->write_s(3, dest->slabs, dest->slabCount);
		}
	}

	void addon_map_ents::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->entityString)
		{
			buf->align(0);
			buf->write(data->entityString, data->numEntityChars);
			buf->clear_pointer(&dest->entityString);
		}

		write_triggers(buf, &dest->trigger);

		buf->pop_stream();
	}

	void addon_map_ents::dump_triggers(const std::string& name, MapTriggers* trigger)
	{
		assetmanager::dumper dumper;
		const auto path = name + ".triggers"s;
		if (dumper.open(path))
		{
			dumper.dump_int(trigger->count);
			dumper.dump_array<TriggerModel>(trigger->models, trigger->count);

			dumper.dump_int(trigger->hullCount);
			dumper.dump_array<TriggerHull>(trigger->hulls, trigger->hullCount);

			dumper.dump_int(trigger->slabCount);
			dumper.dump_array<TriggerSlab>(trigger->slabs, trigger->slabCount);

			dumper.close();
		}
	}

	void addon_map_ents::dump_entity_strings(const std::string& name, char* entityString, int numEntityChars)
	{
		const auto& path = name;
		auto file = filesystem::file(path);
		file.open("wb");
		if (file.get_fp())
		{
			file.write(entityString, numEntityChars, 1);
			file.close();
		}
	}

	void addon_map_ents::dump(AddonMapEnts* asset)
	{
		dump_entity_strings(asset->name, asset->entityString, asset->numEntityChars);
		dump_triggers(asset->name, &asset->trigger);
	}
}