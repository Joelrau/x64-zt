#include <std_include.hpp>
#include "soundsubmix.hpp"

#include "sound.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::s1
{
	const std::unordered_map<short, const char*> volmod_map =
	{
		{-2, "all"}
	};

	const char* get_vol_mod_name(short index)
	{
		if (volmod_map.contains(index))
		{
			return volmod_map.at(index);
		}

		return sound::get_vol_mod_name(index);
	}

	short get_vol_mod_index_from_name(const char* name)
	{
		for (auto& volmod : volmod_map)
		{
			if (!_stricmp(volmod.second, name))
			{
				return volmod.first;
			}
		}

		return sound::get_vol_mod_index_from_name(name);
	}

	namespace
	{
		constexpr auto submix_fields = glz::tuple
		{
			&SndSubmix::name,
			json_reflect::converted(&SndSubmix::volModIndex, get_vol_mod_name, get_vol_mod_index_from_name),
			json_reflect::renamed("volume", &SndSubmix::value),
			&SndSubmix::fadeInTime,
			&SndSubmix::fadeOutTime,
		};
	}

	SndSubmixList* sound_submix::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "sndsubmix\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		file.open("rb");
		if (!file.get_fp())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing sndsubmix \"%s\"...", name.data());

		const auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();

		auto data = json::parse(bytes);

		auto asset = mem->allocate<SndSubmixList>();
		asset->name = mem->duplicate_string(data["name"].get<std::string>());
		json_reflect::read_array<submix_fields>(data["list"], asset->head, asset->count, mem);

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
		return ASSET_TYPE_SOUND_SUBMIX;
	}

	void sound_submix::read(zone_reader& reader, SndSubmixList* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto submixes = reader.read_array(asset->head, 3, asset->count))
		{
			for (auto i = 0; i < asset->count; i++)
			{
				reader.read_string(submixes[i].name);
			}
		}
		reader.pop_stream();
	}

	void sound_submix::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->head)
		{
			buf->align(3);

			auto* dest_submixes = buf->write(data->head, data->count);
			auto* data_submixes = data->head;
			for (auto i = 0; i < data->count; i++)
			{
				if (data_submixes[i].name)
				{
					dest_submixes[i].name = buf->write_str(data_submixes[i].name);
				}
			}

			buf->clear_pointer(&dest->head);
		}

		buf->pop_stream();
	}

	void sound_submix::dump(SndSubmixList* asset)
	{
		const auto path = "sndsubmix\\"s + asset->name + ".json"s;

		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		data["name"] = asset->name;
		if (asset->head)
		{
			json_reflect::write_array<submix_fields>(data["list"], asset->head, asset->count);
		}

		auto str = data.dump(4);
		file.write(str);

		file.close();
	}
}
