#include <std_include.hpp>
#include "soundcurve.hpp"

namespace zonetool::h1
{
	SndCurve* ISoundCurve::parse(const std::string& name, const std::string& type, ZoneMemory* mem)
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

			auto asset = mem->Alloc<SndCurve>();

			auto isDefault = data["isDefault"];
			if (!isDefault.is_null())
			{
				asset->isDefault = isDefault.get<bool>();
			}

			auto filename = data["filename"];
			if (!filename.is_null() && !filename.empty())
			{
				asset->filename = mem->StrDup(filename.get<std::string>().data());
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

	SndCurve* ISoundCurve::parse(const std::string& name, ZoneMemory* mem)
	{
		return parse(name, "sndcurve", mem);
	}

	void ISoundCurve::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).sndCurve;
		}
	}

	void ISoundCurve::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void ISoundCurve::load_depending(IZone* zone)
	{
	}

	std::string ISoundCurve::name()
	{
		return this->name_;
	}

	std::int32_t ISoundCurve::type()
	{
		return ASSET_TYPE_SOUND_CURVE;
	}

	void ISoundCurve::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->filename = buf->write_str(this->name());

		buf->pop_stream();
	}

	void ISoundCurve::dump(SndCurve* asset, const std::string& type)
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

	void ISoundCurve::dump(SndCurve* asset)
	{
		dump(asset, "sndcurve");
	}
}