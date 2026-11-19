#include <std_include.hpp>
#include "dopplerpreset.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::iw6
{
	namespace
	{
		constexpr auto doppler_preset_fields = glz::tuple
		{
			&DopplerPreset::name,
			&DopplerPreset::speedOfSound,
			&DopplerPreset::playerVelocityScale,
			&DopplerPreset::minPitch,
			&DopplerPreset::maxPitch,
			&DopplerPreset::smoothing,
		};
	}

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

			json_reflect::read<doppler_preset_fields>(data, *asset, mem);

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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).dopplerPreset;
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

	void doppler_preset::read(zone_reader& reader, DopplerPreset* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}

	void doppler_preset::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void doppler_preset::dump(DopplerPreset* asset)
	{
		const auto path = "dopplerpreset\\"s + asset->name + "json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json j;

		json_reflect::write<doppler_preset_fields>(j, *asset);

		auto str = j.dump(4);
		j.clear();

		file.write(str);
		file.close();
	}
}
