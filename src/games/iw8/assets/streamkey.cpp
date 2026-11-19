#include <std_include.hpp>
#include "streamkey.hpp"

namespace zonetool::iw8
{
	namespace
	{
		bool is_resident(const StreamKey* asset)
		{
			return asset->flags & STREAM_KEY_RESIDENT;
		}
	}

	StreamKey* stream_key::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("streamkey\\"s + name + ".streamkey"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing streamkey \"%s\"...", name.data());

		auto* asset = read.read_single<StreamKey>();
		asset->name = read.read_string();
		if (is_resident(asset))
		{
			asset->data.residentData = read.read_array<char>();
		}

		read.close();
		return asset;
	}

	void stream_key::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<StreamKey>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void stream_key::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void stream_key::load_depending(zone_base* zone)
	{
	}

	std::string stream_key::name()
	{
		return this->name_;
	}

	std::int32_t stream_key::type()
	{
		return ASSET_TYPE_STREAM_KEY;
	}

	void stream_key::read(zone_reader& reader, StreamKey* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (is_resident(asset))
		{
			reader.read_array(asset->data.residentData, 15, asset->dataSize);
		}
		reader.pop_stream();
	}

	void stream_key::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (is_resident(data) && data->data.residentData)
		{
			buf->align(15);
			buf->write(data->data.residentData, data->dataSize);
			buf->clear_pointer(&dest->data.residentData);
		}
		buf->pop_stream();
	}

	void stream_key::dump(StreamKey* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("streamkey\\"s + asset->name + ".streamkey"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		if (is_resident(asset))
		{
			dump.dump_array(asset->data.residentData, asset->dataSize);
		}
		dump.close();
	}
}
