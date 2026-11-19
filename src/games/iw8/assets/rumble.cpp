#include <std_include.hpp>
#include "rumble.hpp"

namespace zonetool::iw8
{
	namespace
	{
		constexpr std::array rumble_graphs
		{
			&RumbleInfo::highRumbleGraph, &RumbleInfo::lowRumbleGraph, &RumbleInfo::leftTriggerRumbleGraph,
			&RumbleInfo::rightTriggerRumbleGraph,
		};
	}

	void rumble::read(zone_reader& reader, RumbleInfo* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		for (const auto graph : rumble_graphs)
		{
			reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->*graph);
		}
		reader.pop_stream();
	}

	void rumble::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		for (const auto graph : rumble_graphs)
		{
			write_asset(zone, ASSET_TYPE_RUMBLE_GRAPH, dest->*graph);
		}
		buf->pop_stream();
	}

	void rumble::dump(RumbleInfo* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("rumble\\"s + asset->name + ".rumbleb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		for (const auto graph : rumble_graphs)
		{
			dump.dump_asset(asset->*graph);
		}
		dump.close();
	}

	RumbleInfo* rumble::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("rumble\\"s + name + ".rumbleb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing rumble \"%s\"...", name.data());

		auto* asset = read.read_single<RumbleInfo>();
		asset->name = read.read_string();
		for (const auto graph : rumble_graphs)
		{
			parse_asset(read, asset->*graph);
		}
		read.close();
		return asset;
	}

	void rumble::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<RumbleInfo>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void rumble::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		for (const auto graph : rumble_graphs)
		{
			add_asset(zone, ASSET_TYPE_RUMBLE_GRAPH, this->asset_->*graph);
		}
	}

	std::string rumble::name()
	{
		return this->name_;
	}

	std::int32_t rumble::type()
	{
		return ASSET_TYPE_RUMBLE;
	}

	void rumble_graph::read(zone_reader& reader, RumbleGraph* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}

	void rumble_graph::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		buf->pop_stream();
	}

	void rumble_graph::dump(RumbleGraph* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("rumble_graph\\"s + asset->name + ".rumblegraphb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.close();
	}

	RumbleGraph* rumble_graph::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("rumble_graph\\"s + name + ".rumblegraphb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing rumblegraph \"%s\"...", name.data());

		auto* asset = read.read_single<RumbleGraph>();
		asset->name = read.read_string();
		read.close();
		return asset;
	}

	void rumble_graph::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<RumbleGraph>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string rumble_graph::name()
	{
		return this->name_;
	}

	std::int32_t rumble_graph::type()
	{
		return ASSET_TYPE_RUMBLE_GRAPH;
	}
}
