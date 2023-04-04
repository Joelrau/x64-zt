#include <std_include.hpp>
#include "soundcurve.hpp"

namespace zonetool::iw6
{
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

			auto isDefault = data["isDefault"];
			if (!isDefault.is_null())
			{
				asset->isDefault = isDefault.get<bool>();
			}

			auto filename = data["filename"];
			if (!filename.is_null() && !filename.empty())
			{
				asset->filename = mem->duplicate_string(filename.get<std::string>().data());
			}

			auto knots = data["knots"];
			if (!knots.is_null())
			{
				asset->knotCount = static_cast<unsigned short>(knots.size());

				for (unsigned short i = 0; i < asset->knotCount; i++)
				{
					for (unsigned char e = 0; e < 2; e++)
					{
						auto knot = knots[i][e];
						if (!knot.is_null())
						{
							asset->knots[i][e] = knot.get<float>();
						}
					}
				}
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

	void sound_curve::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->filename = buf->write_str(this->name());

		buf->pop_stream();
	}

	void sound_curve::dump(SndCurve* asset, const std::string& type)
	{
		const auto path = type + "\\"s + asset->name + ".json";
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json j;

		j["isDefault"] = asset->isDefault;
		j["filename"] = asset->filename;
		j["knots"] = {};
		for (unsigned short i = 0; i < asset->knotCount; i++)
		{
			for (unsigned char e = 0; e < 2; e++)
			{
				j["knots"][i][e] = asset->knots[i][e];
			}
		}

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