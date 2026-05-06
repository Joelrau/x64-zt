#include "std_include.hpp"
#include "xmodel_detail_collision.hpp"

namespace zonetool::iw8
{
	void xmodel_detail_collision::add_script_string(scr_string_t* ptr, const char* str)
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

	const char* xmodel_detail_collision::get_script_string(scr_string_t* ptr)
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

	XModelDetailCollision* xmodel_detail_collision::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "xmodel\\" + name + ".xdc";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing XModelDetailCollision \"%s\"...", name.data());

		auto* asset = read.read_single<XModelDetailCollision>();
		asset->name = read.read_string();

		asset->physicsLODData = read.read_raw<char>();
		asset->physicsLODDataNames = read.read_array<scr_string_t>();
		for (unsigned int i = 0; i < asset->physicsLODDataNameCount; i++)
		{
			this->add_script_string(&asset->physicsLODDataNames[i], read.read_string());
		}

		read.close();

		return asset;
	}

	void xmodel_detail_collision::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).modelDetailCollision;
		}
	}

	void xmodel_detail_collision::prepare(zone_buffer* buf, zone_memory* mem)
	{
		// fixup scriptstrings
		auto* asset = this->asset_;

		if (asset->physicsLODDataNames)
		{
			for (auto i = 0u; i < asset->physicsLODDataNameCount; i++)
			{
				asset->physicsLODDataNames[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&asset->physicsLODDataNames[i])));
			}
		}
	}

	void xmodel_detail_collision::load_depending(zone_base* zone)
	{
	}

	std::string xmodel_detail_collision::name()
	{
		return this->name_;
	}

	std::int32_t xmodel_detail_collision::type()
	{
		return ASSET_TYPE_XMODEL_DETAIL_COLLISION;
	}

	void xmodel_detail_collision::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL); // 5

		dest->name = buf->write_str(this->name());

		if (data->physicsLODData)
		{
			buf->align(15);
			buf->write(data->physicsLODData, data->physicsLODDataSize);
			buf->clear_pointer(&dest->physicsLODData);
		}

		buf->pop_stream();
	}

	void xmodel_detail_collision::dump(XModelDetailCollision* asset)
	{
		const auto path = "xmodel\\"s + asset->name + ".xdc";

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_raw(asset->physicsLODData, asset->physicsLODDataSize);
		dump.dump_array(asset->physicsLODDataNames, asset->physicsLODDataNameCount);
		for (unsigned int i = 0; i < asset->physicsLODDataNameCount; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->physicsLODDataNames[i]));
		}

		dump.close();
	}
}
