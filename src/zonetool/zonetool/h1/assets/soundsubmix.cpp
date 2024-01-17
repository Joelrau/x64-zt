#include <std_include.hpp>
#include "soundsubmix.hpp"

#include "sound.hpp"

namespace zonetool::h1
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

		if (data["list"].is_object())
		{
			const auto submix_count = data["list"].size();
			asset->submixCount = static_cast<int>(submix_count);
			asset->submixes = mem->allocate<SndSubmix>(submix_count);

			for (auto i = 0; i < asset->submixCount; i++)
			{
				asset->submixes[i].name = mem->duplicate_string(data["list"][i]["name"].get<std::string>());
				asset->submixes[i].volModIndex = get_vol_mod_index_from_name(mem->duplicate_string(data["list"][i]["volModIndex"].get<std::string>().data()));
				asset->submixes[i].volume = data["list"][i]["volume"].get<float>();
				asset->submixes[i].unk[0] = data["list"][i]["unk"][0].get<int>();
				asset->submixes[i].unk[1] = data["list"][i]["unk"][1].get<int>();
			}
		}

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
		const auto path = "sndsubmix\\"s + asset->name + ".json"s;

		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		data["name"] = asset->name;
		if (asset->submixes)
		{
			data["list"] = {};

			for (auto i = 0; i < asset->submixCount; i++)
			{
				data["list"][i]["name"] = asset->submixes[i].name ? asset->submixes[i].name : "";
				data["list"][i]["volModIndex"] = get_vol_mod_name(asset->submixes[i].volModIndex);
				data["list"][i]["volume"] = asset->submixes[i].volume;
				data["list"][i]["unk"][0] = asset->submixes[i].unk[0];
				data["list"][i]["unk"][1] = asset->submixes[i].unk[1];
			}
		}

		auto str = data.dump(4);
		file.write(str);

		file.close();
	}
}