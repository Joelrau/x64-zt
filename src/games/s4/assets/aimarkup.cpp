#include <std_include.hpp>
#include "aimarkup.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_ai_markup(Walk walk, AIMarkup& asset)
		{
			walk.array(asset.records, asset.recordCount, 3);
		}
	}

	void ai_markup::read(zone_reader& reader, AIMarkup* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_ai_markup(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void ai_markup::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_ai_markup(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void ai_markup::dump(AIMarkup* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("aimarkup\\"s + asset->name + ".aimarkupb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_ai_markup(dump_walk{dump}, *asset);
		dump.close();
	}

	AIMarkup* ai_markup::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("aimarkup\\"s + name + ".aimarkupb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing aimarkup \"%s\"...", name.data());

		auto* asset = read.read_single<AIMarkup>();
		asset->name = read.read_string();
		walk_ai_markup(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void ai_markup::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<AIMarkup>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string ai_markup::name()
	{
		return this->name_;
	}

	std::int32_t ai_markup::type()
	{
		return ASSET_TYPE_AI_MARKUP;
	}
}
