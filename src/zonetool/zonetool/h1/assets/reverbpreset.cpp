#include <std_include.hpp>
#include "reverbpreset.hpp"

namespace zonetool::h1
{
	ReverbPreset* reverb_preset::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file("reverbpreset\\"s + name + ".json"s);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing reverbpreset \"%s\"...", name.data());

			const auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();

			auto json_data = json::parse(bytes);

			auto asset = mem->allocate<ReverbPreset>();

			asset->name = mem->duplicate_string(json_data["name"].get<std::string>());
			
			asset->earlyTime = json_data["earlyTime"].get<float>();
			asset->earlyGain = json_data["earlyGain"].get<float>();
			asset->lateGain = json_data["lateGain"].get<float>();
			asset->lateGainProx = json_data["lateGainProx"].get<float>();
			asset->returnGain = json_data["returnGain"].get<float>();
			asset->earlyLpf = json_data["earlyLpf"].get<float>();
			asset->lateLpf = json_data["lateLpf"].get<float>();
			asset->inputLpf = json_data["inputLpf"].get<float>();
			asset->dampLpf = json_data["dampLpf"].get<float>();
			asset->wallReflect = json_data["wallReflect"].get<float>();
			asset->dryGain = json_data["dryGain"].get<float>();
			asset->lateSize = json_data["lateSize"].get<float>();
			asset->diffusion = json_data["diffusion"].get<float>();
			asset->rearLevel = json_data["rearLevel"].get<float>();

			return asset;
		}

		return nullptr;
	}

	void reverb_preset::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).reverbPreset;
		}
	}

	void reverb_preset::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void reverb_preset::load_depending(zone_base* zone)
	{
	}

	std::string reverb_preset::name()
	{
		return this->name_;
	}

	std::int32_t reverb_preset::type()
	{
		return ASSET_TYPE_REVERB_PRESET;
	}

	void reverb_preset::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void reverb_preset::dump(ReverbPreset* asset)
	{
		const auto path = asset->name;
		auto file = filesystem::file("reverbpreset\\"s + path + ".json"s);
		file.open("wb");

		ordered_json json_data;
		json_data["name"] = asset->name;

		json_data["earlyTime"] = asset->earlyTime;
		json_data["lateTime"] = asset->lateTime;
		json_data["earlyGain"] = asset->earlyGain;
		json_data["lateGain"] = asset->lateGain;
		json_data["lateGainProx"] = asset->lateGainProx;
		json_data["returnGain"] = asset->returnGain;
		json_data["earlyLpf"] = asset->earlyLpf;
		json_data["lateLpf"] = asset->lateLpf;
		json_data["inputLpf"] = asset->inputLpf;
		json_data["dampLpf"] = asset->dampLpf;
		json_data["wallReflect"] = asset->wallReflect;
		json_data["dryGain"] = asset->dryGain;
		json_data["earlySize"] = asset->earlySize;
		json_data["lateSize"] = asset->lateSize;
		json_data["diffusion"] = asset->diffusion;
		json_data["rearLevel"] = asset->rearLevel;

		auto str = json_data.dump(4);
		file.write(str);

		file.close();
	}
}