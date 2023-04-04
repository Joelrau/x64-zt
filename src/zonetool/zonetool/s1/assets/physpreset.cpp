#include "std_include.hpp"
#include "physpreset.hpp"

namespace zonetool::s1
{
	PhysPreset* phys_preset::parse(const std::string& name, zone_memory* mem)
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

	void phys_preset::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physPreset;
		}
	}

	void phys_preset::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void phys_preset::load_depending(zone_base* zone)
	{
	}

	std::string phys_preset::name()
	{
		return this->name_;
	}

	std::int32_t phys_preset::type()
	{
		return ASSET_TYPE_PHYSPRESET;
	}

	void phys_preset::write(zone_base* zone, zone_buffer* buf)
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

	void phys_preset::dump(PhysPreset* asset)
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