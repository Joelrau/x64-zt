#include <std_include.hpp>
#include "detailcollision.hpp"

namespace zonetool::iw8
{
	void xmodel_detail_collision::read(zone_reader& reader, XModelDetailCollision* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->physicsLODData, 15, asset->physicsLODDataSize);
		reader.read_script_string_array(asset->physicsLODDataNames, asset->physicsLODDataNameCount);
		reader.pop_stream();
	}

	void xmodel_detail_collision::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		write_array(buf, dest->physicsLODData, data->physicsLODDataSize, 15);
		write_array(buf, dest->physicsLODDataNames, data->physicsLODDataNameCount, 3);
		buf->pop_stream();
	}

	void xmodel_detail_collision::dump(XModelDetailCollision* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("xmodeldetailcollision\\"s + asset->name + ".xmodeldetailcollisionb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_array(asset->physicsLODData, asset->physicsLODDataSize);
		dump_script_strings(dump, asset->physicsLODDataNames, asset->physicsLODDataNameCount);
		dump.close();
	}

	XModelDetailCollision* xmodel_detail_collision::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("xmodeldetailcollision\\"s + name + ".xmodeldetailcollisionb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing xmodeldetailcollision \"%s\"...", name.data());

		auto* asset = read.read_single<XModelDetailCollision>();
		asset->name = read.read_string();
		asset->physicsLODData = read.read_array<char>();
		this->script_strings_.parse(read, asset->physicsLODDataNames, asset->physicsLODDataNameCount);
		read.close();
		return asset;
	}

	void xmodel_detail_collision::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<XModelDetailCollision>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void xmodel_detail_collision::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			this->script_strings_.prepare(buf, this->asset_->physicsLODDataNames, this->asset_->physicsLODDataNameCount);
		}
	}

	std::string xmodel_detail_collision::name()
	{
		return this->name_;
	}

	std::int32_t xmodel_detail_collision::type()
	{
		return ASSET_TYPE_XMODEL_DETAIL_COLLISION;
	}
}
