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
			asset->count = static_cast<int>(submix_count);
			asset->head = mem->allocate<SndSubmix>(submix_count);

			for (auto i = 0; i < asset->count; i++)
			{
				asset->head[i].name = mem->duplicate_string(data["list"][i]["name"].get<std::string>());
				asset->head[i].volModIndex = get_vol_mod_index_from_name(mem->duplicate_string(data["list"][i]["volModIndex"].get<std::string>().data()));
				asset->head[i].value = data["list"][i]["volume"].get<float>();
				asset->head[i].fadeInTime = data["list"][i]["fadeInTime"].get<int>();
				asset->head[i].fadeOutTime = data["list"][i]["fadeOutTime"].get<int>();
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
		return ASSET_TYPE_SOUND_SUBMIX;
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
			data["list"] = {};

			for (auto i = 0; i < asset->count; i++)
			{
				data["list"][i]["name"] = asset->head[i].name ? asset->head[i].name : "";
				data["list"][i]["volModIndex"] = get_vol_mod_name(asset->head[i].volModIndex);
				data["list"][i]["volume"] = asset->head[i].value;
				data["list"][i]["fadeInTime"] = asset->head[i].fadeInTime;
				data["list"][i]["fadeOutTime"] = asset->head[i].fadeOutTime;
			}
		}

		auto str = data.dump(4);
		file.write(str);

		file.close();
	}
}