#include <std_include.hpp>
#include "fontdef.hpp"

namespace zonetool::iw6
{
	Font_s* IFontDef::parse(const std::string& name, ZoneMemory* mem)
	{
		auto path = name;
		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}
		file.open("rb");

		ZONETOOL_INFO("Parsing font \"%s\"...", name.data());

		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();

		nlohmann::json fontdata = nlohmann::json::parse(bytes);

		auto font = mem->Alloc<Font_s>();
		font->name = _strdup(fontdata["fontName"].get<std::string>().data());
		font->glyphCount = fontdata["glyphCount"].get<int>();
		font->pixelHeight = fontdata["pixelHeight"].get<int>();

		font->material = mem->Alloc<Material>();
		font->material->name = _strdup(fontdata["material"].get<std::string>().data());
		font->glowMaterial = mem->Alloc<Material>();
		font->glowMaterial->name = _strdup(fontdata["glowMaterial"].get<std::string>().data());

		font->glyphs = mem->Alloc<Glyph>(font->glyphCount);

		for (int i = 0; i < font->glyphCount; i++)
		{
			font->glyphs[i].letter = fontdata["glyphs"][i]["letter"].get<unsigned __int16>();
			font->glyphs[i].x0 = fontdata["glyphs"][i]["x0"].get<char>();
			font->glyphs[i].y0 = fontdata["glyphs"][i]["y0"].get<char>();
			font->glyphs[i].dx = fontdata["glyphs"][i]["dx"].get<char>();
			font->glyphs[i].pixelWidth = fontdata["glyphs"][i]["pixelWidth"].get<char>();
			font->glyphs[i].pixelHeight = fontdata["glyphs"][i]["pixelHeight"].get<char>();
			font->glyphs[i].s0 = fontdata["glyphs"][i]["s0"].get<float>();
			font->glyphs[i].t0 = fontdata["glyphs"][i]["t0"].get<float>();
			font->glyphs[i].s1 = fontdata["glyphs"][i]["s1"].get<float>();
			font->glyphs[i].t1 = fontdata["glyphs"][i]["t1"].get<float>();
		}

		return font;
	}

	void IFontDef::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;
		this->asset_ = this->parse(name, mem);

		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name_.data()).font;
		}
	}

	void IFontDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IFontDef::load_depending(IZone* zone)
	{
		auto* data = this->asset_;

		if (data->material)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->material->name);
		}

		if (data->glowMaterial)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->glowMaterial->name);
		}
	}

	std::string IFontDef::name()
	{
		return this->name_;
	}

	std::int32_t IFontDef::type()
	{
		return ASSET_TYPE_FONT;
	}

	void IFontDef::write(IZone* zone, ZoneBuffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->material)
		{
			dest->material = reinterpret_cast<Material*>(
				zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->material->name)
				);
		}

		if (data->glowMaterial)
		{
			dest->glowMaterial = reinterpret_cast<Material*>(
				zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->glowMaterial->name)
				);
		}

		if (data->glyphs)
		{
			buf->align(3);
			buf->write(data->glyphs, data->glyphCount);
			ZoneBuffer::clear_pointer(&dest->glyphs);
		}

		buf->pop_stream();
	}

	void IFontDef::dump(Font_s* asset)
	{
		auto f = filesystem::file(asset->fontName);
		f.open("wb");

		ordered_json fontdata;
		ordered_json glyph;

		fontdata["fontName"] = asset->fontName;
		fontdata["glyphCount"] = asset->glyphCount;
		fontdata["material"] = asset->material->name;
		fontdata["glowMaterial"] = asset->glowMaterial->name;
		fontdata["pixelHeight"] = asset->pixelHeight;
		fontdata["glyphs"] = {};
		for (int i = 0; i < asset->glyphCount; i++)
		{
			glyph.clear();
			glyph["letter"] = asset->glyphs[i].letter;
			glyph["x0"] = asset->glyphs[i].x0;
			glyph["y0"] = asset->glyphs[i].y0;
			glyph["dx"] = asset->glyphs[i].dx;
			glyph["pixelWidth"] = asset->glyphs[i].pixelWidth;
			glyph["pixelHeight"] = asset->glyphs[i].pixelHeight;
			glyph["s0"] = asset->glyphs[i].s0;
			glyph["t0"] = asset->glyphs[i].t0;
			glyph["s1"] = asset->glyphs[i].s1;
			glyph["t1"] = asset->glyphs[i].t1;
			fontdata["glyphs"].push_back(glyph);
		}

		auto s = fontdata.dump(4);

		fontdata.clear();
		glyph.clear();

		f.write(s);

		f.close();
	}
}