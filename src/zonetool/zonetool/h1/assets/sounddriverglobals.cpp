#include <std_include.hpp>
#include "sounddriverglobals.hpp"

namespace zonetool::h1
{
	SndDriverGlobals* sound_driver_globals::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "snddriverglobals\\"s + name;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing snddriverglobals \"%s\"...", name.data());

		auto* asset = read.read_single<SndDriverGlobals>();
		asset->name = read.read_string();

		if (asset->reverbSettings)
		{
			asset->reverbSettings = read.read_array<XaReverbSettings>();
		}

		read.close();

		return asset;
	}

	void sound_driver_globals::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).sndDriverGlobals;
		}
	}

	void sound_driver_globals::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void sound_driver_globals::load_depending(zone_base* zone)
	{
	}

	std::string sound_driver_globals::name()
	{
		return this->name_;
	}

	std::int32_t sound_driver_globals::type()
	{
		return ASSET_TYPE_SNDDRIVER_GLOBALS;
	}

	void sound_driver_globals::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->reverbSettings)
		{
			buf->align(3);
			buf->write(data->reverbSettings, 26);
			buf->clear_pointer(&dest->reverbSettings);
		}

		buf->pop_stream();
	}

	void sound_driver_globals::dump(SndDriverGlobals* asset)
	{
		const auto path = "snddriverglobals\\"s + asset->name;

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		if (asset->reverbSettings)
		{
			dump.dump_array(asset->reverbSettings, 26);
		}

		dump.close();
	}
}