#include "std_include.hpp"
#include "physpreset.hpp"

namespace zonetool::s1
{
	PhysPreset* IPhysPreset::parse(const std::string& name, ZoneMemory* mem)
	{
		const auto path = "physpreset\\"s + name + ".pp";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physpreset \"%s\"...", name.data());

		auto* asset = read.read_single<PhysPreset>();
		asset->name = read.read_string();
		asset->sndAliasPrefix = read.read_string();
		read.close();

		return asset;
	}

	void IPhysPreset::init(const std::string& name, ZoneMemory* mem)
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
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).physPreset;
		}
	}

	void IPhysPreset::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IPhysPreset::load_depending(IZone* zone)
	{
	}

	std::string IPhysPreset::name()
	{
		return this->name_;
	}

	std::int32_t IPhysPreset::type()
	{
		return ASSET_TYPE_PHYSPRESET;
	}

	void IPhysPreset::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->sndAliasPrefix)
		{
			dest->sndAliasPrefix = buf->write_str(data->sndAliasPrefix);
		}

		buf->pop_stream();
	}

	void IPhysPreset::dump(PhysPreset* asset)
	{
		const auto path = "physpreset\\"s + asset->name + ".pp";

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_string(asset->sndAliasPrefix);
		write.close();
	}
}