#include <std_include.hpp>
#include "gesture.hpp"

namespace zonetool::s4
{
	void gesture::read(zone_reader& reader, Gesture* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset_array(ASSET_TYPE_XANIMPARTS, asset->anims, 22);
		reader.read_script_string(asset->ikTargetEntityBoneName);
		reader.pop_stream();
	}

	void gesture::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		write_asset_array(zone, buf, ASSET_TYPE_XANIMPARTS, dest->anims, 22);
		buf->pop_stream();
	}

	void gesture::dump(Gesture* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("gesture\\"s + asset->name + ".gestureb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump_asset_array(dump, asset->anims, 22);
		dump_script_string(dump, asset->ikTargetEntityBoneName);
		dump.close();
	}

	Gesture* gesture::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("gesture\\"s + name + ".gestureb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing gesture \"%s\"...", name.data());

		auto* asset = read.read_single<Gesture>();
		asset->name = read.read_string();
		parse_asset_array(read, asset->anims, 22);
		this->script_strings_.parse(read, asset->ikTargetEntityBoneName);
		read.close();
		return asset;
	}

	void gesture::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<Gesture>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void gesture::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			this->script_strings_.prepare(buf, this->asset_->ikTargetEntityBoneName);
		}
	}

	void gesture::load_depending(zone_base* zone)
	{
		if (!this->referenced())
		{
			add_asset_array(zone, ASSET_TYPE_XANIMPARTS, this->asset_->anims, 22);
		}
	}

	std::string gesture::name()
	{
		return this->name_;
	}

	std::int32_t gesture::type()
	{
		return ASSET_TYPE_GESTURE;
	}
}
