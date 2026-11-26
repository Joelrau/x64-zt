#include <std_include.hpp>
#include "camera.hpp"

namespace zonetool::s4
{
	void camera_def::read(zone_reader& reader, CameraDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}

	void camera_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		buf->pop_stream();
	}

	void camera_def::dump(CameraDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("camera\\"s + asset->name + ".camerab"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.close();
	}

	CameraDef* camera_def::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("camera\\"s + name + ".camerab"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing camera \"%s\"...", name.data());

		auto* asset = read.read_single<CameraDef>();
		asset->name = read.read_string();
		read.close();
		return asset;
	}

	void camera_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<CameraDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string camera_def::name()
	{
		return this->name_;
	}

	std::int32_t camera_def::type()
	{
		return ASSET_TYPE_CAMERA;
	}
}
