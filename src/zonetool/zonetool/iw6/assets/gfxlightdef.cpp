#include <std_include.hpp>
#include "gfxlightdef.hpp"

namespace zonetool::iw6
{
	void parseLightImage(GfxLightImage* image, json& data, zone_memory* mem)
	{
		if (data["image"].is_string())
		{
			const auto image_name = data["image"].get<std::string>();

			if (image_name.size())
			{
				image->image = mem->allocate<GfxImage>();
				image->image->name = mem->duplicate_string(image_name);
			}
		}

		image->samplerState = data["samplerState"].get<char>();
	}

	GfxLightDef* gfx_light_def::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "lights\\"s + name + ".json"s;
		filesystem::file file(path);
		if (file.exists())
		{
			ZONETOOL_INFO("Parsing lightdef \"%s\"...", name.data());

			auto asset = mem->allocate<GfxLightDef>();

			file.open("rb");
			const auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();

			auto data = json::parse(bytes);

			asset->name = mem->duplicate_string(data["name"].get<std::string>());

			// parse light images
			parseLightImage(&asset->attenuation, data["attenuation"], mem);
			parseLightImage(&asset->cucoloris, data["cucoloris"], mem);

			asset->lmapLookupStart = data["lmapLookupStart"].get<int>();

			return asset;
		}

		return nullptr;
	}

	void gfx_light_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).lightDef;
		}
	}

	void gfx_light_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void gfx_light_def::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;

		if (asset->attenuation.image)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, asset->attenuation.image->name);
		}
		if (asset->cucoloris.image)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, asset->cucoloris.image->name);
		}
	}

	std::string gfx_light_def::name()
	{
		return this->name_;
	}

	std::int32_t gfx_light_def::type()
	{
		return ASSET_TYPE_LIGHT_DEF;
	}

	void gfx_light_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->attenuation.image)
		{
			dest->attenuation.image = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
				ASSET_TYPE_IMAGE, data->attenuation.image->name));
		}
		if (data->cucoloris.image)
		{
			dest->cucoloris.image = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
				ASSET_TYPE_IMAGE, data->cucoloris.image->name));
		}

		buf->pop_stream();
	}

	void gfx_light_def::dump(GfxLightDef* asset)
	{
		const auto path = "lights\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		data["name"] = asset->name;

		data["attenuation"] = {};
		data["attenuation"]["image"] = asset->attenuation.image ? asset->attenuation.image->name : "";
		data["attenuation"]["samplerState"] = asset->attenuation.samplerState;

		data["cucoloris"] = {};
		data["cucoloris"]["image"] = asset->cucoloris.image ? asset->cucoloris.image->name : "";
		data["cucoloris"]["samplerState"] = asset->cucoloris.samplerState;

		data["lmapLookupStart"] = asset->lmapLookupStart;

		const auto json = data.dump(4);

		file.write(json.data(), json.size(), 1);

		file.close();
	}
}