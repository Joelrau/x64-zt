#include <std_include.hpp>
#include "soundcurve.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::h2
{
	namespace
	{
		constexpr auto sound_curve_fields = glz::tuple
		{
			&SndCurve::isDefault,
			&SndCurve::filename,
		};
	}

	SndCurve* sound_curve::parse(const std::string& name, const std::string& type, zone_memory* mem)
	{
		if (name.empty() || type.empty())
		{
			return nullptr;
		}

		const auto path = type + "\\"s + name + ".json"s;
		auto file = filesystem::file(path);
		if (file.exists())
		{
			ZONETOOL_INFO("Parsing %s \"%s\"...", type.data(), name.data());

			// parse json file
			file.open("rb");
			auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();
			json data = json::parse(bytes);

			auto asset = mem->allocate<SndCurve>();

			json_reflect::read<sound_curve_fields>(data, *asset, mem);

			const auto& knots = data["knots"];
			if (knots.is_array())
			{
				json_reflect::read(knots, asset->knots, mem);
				asset->knotCount = static_cast<unsigned short>(std::min(knots.size(), std::size(asset->knots)));
			}
			return asset;
		}

		return nullptr;
	}

	SndCurve* sound_curve::parse(const std::string& name, zone_memory* mem)
	{
		return parse(name, "sndcurve", mem);
	}

	void sound_curve::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).sndCurve;
		}
	}

	void sound_curve::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void sound_curve::load_depending(zone_base* zone)
	{
	}

	std::string sound_curve::name()
	{
		return this->name_;
	}

	std::int32_t sound_curve::type()
	{
		return ASSET_TYPE_SOUND_CURVE;
	}

	void sound_curve::read(zone_reader& reader, SndCurve* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->filename);
		reader.pop_stream();
	}

	void sound_curve::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->filename = buf->write_str(this->name());

		buf->pop_stream();
	}

	void sound_curve::dump(SndCurve* asset, const std::string& type)
	{
		const auto path = type + "\\"s + asset->name + ".json";
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json j;

		json_reflect::write<sound_curve_fields>(j, *asset);
		json_reflect::write_array(j["knots"], asset->knots, asset->knotCount);

		auto str = j.dump(4);
		j.clear();

		file.write(str);
		file.close();
	}

	void sound_curve::dump(SndCurve* asset)
	{
		dump(asset, "sndcurve");
	}
}
