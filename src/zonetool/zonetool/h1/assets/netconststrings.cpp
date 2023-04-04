#include <std_include.hpp>
#include "netconststrings.hpp"

namespace zonetool::h1
{
	NetConstStrings* net_const_strings::parse(std::string name, zone_memory* mem)
	{
		const auto path = "netconststrings\\"s + name + ".json"s;
		filesystem::file file(path);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing netconststrings \"%s\"...", name.data());

			auto asset = mem->allocate<NetConstStrings>();
			
			const auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();

			auto data = json::parse(bytes);

			asset->name = mem->duplicate_string(data["name"].get<std::string>());
			asset->stringType = data["stringType"].get<NetConstStringType>();
			asset->sourceType = data["sourceType"].get<NetConstStringSource>();
			asset->entryCount = static_cast<unsigned int>(data["stringList"].size());
			if (asset->entryCount)
			{
				asset->stringList = mem->allocate<const char*>(asset->entryCount);
				for (unsigned int i = 0; i < asset->entryCount; i++)
				{
					asset->stringList[i] = mem->duplicate_string(data["stringList"][i].get<std::string>());
				}
			}
			
			return asset;
		}

		return nullptr;
	}

	void net_const_strings::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).netConstStrings;
		}
	}

	void net_const_strings::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void net_const_strings::load_depending(zone_base* zone)
	{
	}

	std::string net_const_strings::name()
	{
		return this->name_;
	}

	std::int32_t net_const_strings::type()
	{
		return ASSET_TYPE_NET_CONST_STRINGS;
	}

	void net_const_strings::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->stringList)
		{
			buf->align(7);
			auto* strings = buf->write(data->stringList, data->entryCount);
			for (unsigned int i = 0; i < data->entryCount; i++)
			{
				if (data->stringList[i])
				{
					strings[i] = buf->write_str(data->stringList[i]);
				}
			}
			zone_buffer::clear_pointer(&dest->stringList);
		}

		buf->pop_stream();
	}

	void net_const_strings::dump(NetConstStrings* asset)
	{
		const auto path = "netconststrings\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json j;

		j["name"] = asset->name;
		j["stringType"] = asset->stringType;
		j["sourceType"] = asset->sourceType;
		j["stringList"] = {};
		for (unsigned int i = 0; i < asset->entryCount; i++)
		{
			j["stringList"][i] = asset->stringList[i];
		}

		auto str = j.dump(4);

		j.clear();

		file.write(str);
		file.close();
	}
}