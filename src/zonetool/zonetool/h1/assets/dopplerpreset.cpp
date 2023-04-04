#include <std_include.hpp>
#include "dopplerpreset.hpp"

namespace zonetool::h1
{
	DopplerPreset* doppler_preset::parse(const std::string& name, zone_memory* mem)
	{
		if (name.empty())
		{
			return nullptr;
		}

		const auto path = "dopplerpreset\\"s + name + "json"s;
		auto file = filesystem::file(path);
		if (file.exists())
		{
			ZONETOOL_INFO("Parsing dopplerpreset \"%s\"...", name.data());

			// parse json file
			file.open("rb");
			auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();
			json data = json::parse(bytes);

			auto asset = mem->allocate<DopplerPreset>();

			auto p_name = data["name"];
			if (!p_name.is_null() && !p_name.empty())
			{
				asset->name = mem->duplicate_string(p_name.get<std::string>().data());
			}

			auto speedOfSound = data["speedOfSound"];
			if (!speedOfSound.is_null())
			{
				asset->speedOfSound = speedOfSound.get<float>();
			}

			auto playerVelocityScale = data["playerVelocityScale"];
			if (!playerVelocityScale.is_null())
			{
				asset->playerVelocityScale = playerVelocityScale.get<float>();
			}

			auto minPitch = data["minPitch"];
			if (!minPitch.is_null())
			{
				asset->minPitch = minPitch.get<float>();
			}

			auto maxPitch = data["maxPitch"];
			if (!maxPitch.is_null())
			{
				asset->maxPitch = maxPitch.get<float>();
			}

			auto smoothing = data["smoothing"];
			if (!smoothing.is_null())
			{
				asset->smoothing = smoothing.get<float>();
			}

			return asset;
		}

		return nullptr;
	}

	void doppler_preset::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).doppler;
		}
	}

	void doppler_preset::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void doppler_preset::load_depending(zone_base* zone)
	{
	}

	std::string doppler_preset::name()
	{
		return this->name_;
	}

	std::int32_t doppler_preset::type()
	{
		return ASSET_TYPE_DOPPLER_PRESET;
	}

	void doppler_preset::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void doppler_preset::dump(DopplerPreset* asset)
	{
		const auto path = "dopplerpreset\\"s + asset->name + "json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json j;

		j["name"] = asset->name;
		j["speedOfSound"] = asset->speedOfSound;
		j["playerVelocityScale"] = asset->playerVelocityScale;
		j["minPitch"] = asset->minPitch;
		j["maxPitch"] = asset->maxPitch;
		j["smoothing"] = asset->smoothing;

		auto str = j.dump(4);
		j.clear();

		file.write(str);
		file.close();
	}
}