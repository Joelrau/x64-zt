#include <std_include.hpp>
#include "netconststrings.hpp"

namespace zonetool::s1
{
	NetConstStrings* INetConstStrings::parse(std::string name, ZoneMemory* mem)
	{
		const auto path = "netconststrings\\"s + name + ".json"s;
		filesystem::file file(path);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing netconststrings \"%s\"...", name.data());

			auto asset = mem->Alloc<NetConstStrings>();
			
			const auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();

			auto data = json::parse(bytes);

			asset->name = mem->StrDup(data["name"].get<std::string>());
			asset->stringType = data["stringType"].get<NetConstStringType>();
			asset->sourceType = data["sourceType"].get<NetConstStringSource>();
			asset->entryCount = static_cast<unsigned int>(data["stringList"].size());
			if (asset->entryCount)
			{
				asset->stringList = mem->Alloc<const char*>(asset->entryCount);
				for (unsigned int i = 0; i < asset->entryCount; i++)
				{
					asset->stringList[i] = mem->StrDup(data["stringList"][i].get<std::string>());
				}
			}
			
			return asset;
		}

		return nullptr;
	}

	void INetConstStrings::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).netConstStrings;
		}
	}

	void INetConstStrings::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void INetConstStrings::load_depending(IZone* zone)
	{
	}

	std::string INetConstStrings::name()
	{
		return this->name_;
	}

	std::int32_t INetConstStrings::type()
	{
		return ASSET_TYPE_NET_CONST_STRINGS;
	}

	void INetConstStrings::write(IZone* zone, ZoneBuffer* buf)
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
			ZoneBuffer::clear_pointer(&dest->stringList);
		}

		buf->pop_stream();
	}

	void INetConstStrings::dump(NetConstStrings* asset)
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