#include <std_include.hpp>
#include "gfxlightmap.hpp"

namespace zonetool::iw7
{
	namespace
	{
		std::string clean_name(const std::string& name)
		{
			auto new_name = name;

			for (auto i = 0u; i < name.size(); i++)
			{
				switch (new_name[i])
				{
				case '*':
					new_name[i] = '_';
					break;
				}
			}

			return new_name;
		}
	}

	GfxLightMap* gfx_light_map::parse(const std::string& name, zone_memory* mem)
	{
		auto c_name = clean_name(name);

		const auto path = "lightmaps\\"s + c_name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing lightmap \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<GfxLightMap>();

		asset->name = mem->duplicate_string(name);
		for (auto i = 0; i < 3; i++)
		{
			if (!data["textures"][i].get<std::string>().empty())
			{
				asset->textures[i] = mem->allocate<GfxImage>();
				asset->textures[i]->name = mem->duplicate_string(data["textures"][i].get<std::string>());
			}
		}

		return asset;
	}

	void gfx_light_map::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).lightMap;
		}
	}

	void gfx_light_map::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void gfx_light_map::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		for (auto i = 0; i < 3; i++)
		{
			if (data->textures[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->textures[i]->name);
			}
		}
	}

	std::string gfx_light_map::name()
	{
		return this->name_;
	}

	std::int32_t gfx_light_map::type()
	{
		return ASSET_TYPE_GFXLIGHTMAP;
	}

	void gfx_light_map::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		for (auto i = 0; i < 3; i++)
		{
			if (data->textures[i])
			{
				dest->textures[i] = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(ASSET_TYPE_IMAGE, data->textures[i]->name));
			}
		}

		buf->pop_stream();
	}

	void gfx_light_map::dump(GfxLightMap* asset)
	{
		auto c_name = clean_name(asset->name);

		const auto path = "lightmaps\\"s + c_name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		for (auto i = 0; i < 3; i++)
		{
			data["textures"][i] = asset->textures[i] ? asset->textures[i]->name : "";
		}

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}