#include <std_include.hpp>
#include "reverbpreset.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::h1
{
	namespace
	{
		constexpr auto reverb_preset_fields = glz::tuple
		{
			&ReverbPreset::name,
			&ReverbPreset::earlyTime,
			&ReverbPreset::lateTime,
			&ReverbPreset::earlyGain,
			&ReverbPreset::lateGain,
			&ReverbPreset::lateGainProx,
			&ReverbPreset::returnGain,
			&ReverbPreset::earlyLpf,
			&ReverbPreset::lateLpf,
			&ReverbPreset::inputLpf,
			&ReverbPreset::dampLpf,
			&ReverbPreset::wallReflect,
			&ReverbPreset::dryGain,
			&ReverbPreset::earlySize,
			&ReverbPreset::lateSize,
			&ReverbPreset::diffusion,
			&ReverbPreset::rearLevel,
		};
	}

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

			json_reflect::read<reverb_preset_fields>(json_data, *asset, mem);

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

	void reverb_preset::read(zone_reader& reader, ReverbPreset* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
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
		json_reflect::write<reverb_preset_fields>(json_data, *asset);

		auto str = json_data.dump(4);
		file.write(str);

		file.close();
	}
}
