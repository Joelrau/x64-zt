#include <std_include.hpp>
#include "soundsubmix.hpp"

namespace zonetool::h1
{
	SndSubmixList* sound_submix::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "sndsubmix\\"s + name;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing sndsubmix \"%s\"...", name.data());

		auto* asset = read.read_single<SndSubmixList>();
		asset->name = read.read_string();

		asset->submixes = read.read_array<SndSubmix>();
		for (auto i = 0; i < asset->submixCount; i++)
		{
			asset->submixes[i].name = read.read_string();
		}

		read.close();

		return asset;
	}

	void sound_submix::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).sndSubmix;
		}
	}

	void sound_submix::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void sound_submix::load_depending(zone_base* zone)
	{
	}

	std::string sound_submix::name()
	{
		return this->name_;
	}

	std::int32_t sound_submix::type()
	{
		return ASSET_TYPE_SOUNDSUBMIX;
	}

	void sound_submix::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->submixes)
		{
			buf->align(3);

			auto* dest_submixes = buf->write(data->submixes, data->submixCount);
			auto* data_submixes = data->submixes;
			for (auto i = 0; i < data->submixCount; i++)
			{
				if (data_submixes[i].name)
				{
					dest_submixes[i].name = buf->write_str(data_submixes[i].name);
				}
			}

			zone_buffer::clear_pointer(&dest->submixes);
		}

		buf->pop_stream();
	}

	void sound_submix::dump(SndSubmixList* asset)
	{
		const auto path = "sndsubmix\\"s + asset->name;

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_array(asset->submixes, asset->submixCount);
		for (auto i = 0; i < asset->submixCount; i++)
		{
			dump.dump_string(asset->submixes[i].name);
		}

		dump.close();
	}
}