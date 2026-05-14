#include <std_include.hpp>
#include "fontdef.hpp"

#include "stb_truetype.h"

namespace zonetool::iw6
{
	bool GenerateFontAtlas(
		const uint8_t* ttfData,
		size_t ttfSize,
		float pixelHeight,
		std::vector<Glyph>& outGlyphs,
		std::vector<unsigned char>& outPixels,
		int& atlasWidth,
		int& atlasHeight)
	{
		stbtt_fontinfo font{};
		int fontOffset = stbtt_GetFontOffsetForIndex(ttfData, 0);
		if (fontOffset < 0)
			return false;

		if (!stbtt_InitFont(&font, ttfData, fontOffset))
			return false;

		//--------------------------------------------
		// Find supported codepoints
		//--------------------------------------------

		std::vector<int> codepoints;
		for (int c = 32; c < 128; c++)
		{
			if (stbtt_FindGlyphIndex(&font, c) != 0)
				codepoints.push_back(c);
		}

		if (codepoints.empty())
			return false;

		//--------------------------------------------
		// Dynamic atlas sizing (TEMP alpha buffer)
		//--------------------------------------------

		const int maxAtlasSize = 8192;

		atlasWidth = 256;
		atlasHeight = 256;

		std::vector<unsigned char> alphaPixels;
		std::vector<stbtt_packedchar> packedChars(codepoints.size());

		bool packedSuccessfully = false;

		while (!packedSuccessfully)
		{
			alphaPixels.clear();
			alphaPixels.resize(atlasWidth * atlasHeight, 0);

			stbtt_pack_context packContext{};

			if (!stbtt_PackBegin(
				&packContext,
				alphaPixels.data(),
				atlasWidth,
				atlasHeight,
				0,
				1,
				nullptr))
			{
				return false;
			}

			stbtt_PackSetOversampling(&packContext, 1, 1);

			packedSuccessfully = true;

			for (size_t i = 0; i < codepoints.size(); i++)
			{
				if (!stbtt_PackFontRange(
					&packContext,
					ttfData,
					fontOffset,
					pixelHeight,
					codepoints[i],
					1,
					&packedChars[i]))
				{
					packedSuccessfully = false;
					break;
				}
			}

			stbtt_PackEnd(&packContext);

			if (!packedSuccessfully)
			{
				if (atlasWidth <= atlasHeight)
					atlasWidth *= 2;
				else
					atlasHeight *= 2;

				if (atlasWidth > maxAtlasSize || atlasHeight > maxAtlasSize)
					return false;
			}
		}

		//--------------------------------------------
		// CONVERT alpha → RGBA
		//--------------------------------------------

		outPixels.resize(atlasWidth * atlasHeight * 4);

		for (int i = 0; i < atlasWidth * atlasHeight; i++)
		{
			unsigned char a = alphaPixels[i];

			outPixels[i * 4 + 0] = 255; // R
			outPixels[i * 4 + 1] = 255; // G
			outPixels[i * 4 + 2] = 255; // B
			outPixels[i * 4 + 3] = a;   // A
		}

		//--------------------------------------------
		// Convert glyphs
		//--------------------------------------------

		outGlyphs.clear();
		outGlyphs.reserve(codepoints.size());

		for (size_t i = 0; i < codepoints.size(); i++)
		{
			const auto& p = packedChars[i];

			Glyph g{};

			g.letter = static_cast<unsigned short>(codepoints[i]);
			g.x0 = static_cast<char>(std::round(p.xoff));
			g.y0 = static_cast<char>(std::round(p.yoff));
			g.dx = static_cast<char>(std::round(p.xadvance));

			g.pixelWidth = static_cast<char>(p.x1 - p.x0);
			g.pixelHeight = static_cast<char>(p.y1 - p.y0);

			g.s0 = p.x0 / float(atlasWidth);
			g.t0 = p.y0 / float(atlasHeight);
			g.s1 = p.x1 / float(atlasWidth);
			g.t1 = p.y1 / float(atlasHeight);

			outGlyphs.push_back(g);
		}

		return true;
	}

	GfxImage* generate_image(const std::string& name, const std::vector<unsigned char>& pixels, int width, int height, zone_memory* mem)
	{
		GfxImage* image = mem->allocate<GfxImage>();
		image->name = mem->duplicate_string(name);
		image->imageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		image->mapType = MAPTYPE_2D;
		image->semantic = TS_2D;
		image->category = IMG_CATEGORY_AUTO_GENERATED;
		image->flags = 0;
		image->dataLen1 = static_cast<unsigned int>(pixels.size());
		image->dataLen2 = static_cast<unsigned int>(pixels.size());
		image->width = static_cast<unsigned short>(width);
		image->height = static_cast<unsigned short>(height);
		image->depth = 1;
		image->numElements = 1;
		image->levelCount = 1;
		image->streamed = 0;
		unsigned char* pixelData = mem->allocate<unsigned char>(pixels.size());
		std::memcpy(pixelData, pixels.data(), pixels.size());
		image->pixelData = pixelData;
		return image;
	}

	Material* generate_material(const std::string& name, GfxImage* image,
		const std::string& techniqueset, const uint8_t sort_key, const uint8_t camera_region, zone_memory* mem)
	{
		const auto gamefonts_name = "fonts/gamefonts_pc"s;
		auto* gamefonts = db_find_x_asset_header_safe(ASSET_TYPE_MATERIAL, gamefonts_name).material;
		if (!gamefonts)
		{
			ZONETOOL_FATAL("Cannot generate material for \"%s\". Dependent asset \"%s\" not found.", name.data(), gamefonts_name.data());
			return nullptr;
		}

		Material* mat = mem->allocate<Material>();
		mat->name = mem->duplicate_string(name);
		mat->techniqueSet = mem->allocate<MaterialTechniqueSet>();
		mat->techniqueSet->name = mem->duplicate_string(techniqueset);
		mat->info.sortKey = sort_key;
		mat->cameraRegion = camera_region;
		mat->materialType = MTL_TYPE_DEFAULT;
		mat->assetFlags = MTL_ASSETFLAG_NONE;
		mat->stateFlags = 0;
		mat->stateFlags |= STATE_FLAG_CULL_BACK;
		mat->stateFlags |= STATE_FLAG_USES_DEPTH_BUFFER;
		mat->info.textureAtlasRowCount = 1;
		mat->info.textureAtlasColumnCount = 1;
		mat->textureCount = 1;
		mat->textureTable = mem->allocate<MaterialTextureDef>(1);
		mat->textureTable[0].u.image = image;
		mat->textureTable[0].semantic = 0;
		mat->textureTable[0].nameHash = 2695565377;
		mat->textureTable[0].nameStart = 99;
		mat->textureTable[0].nameEnd = 112;
		mat->textureTable[0].samplerState = 226;

		std::memset(mat->stateBitsEntry, 0xFF, sizeof(mat->stateBitsEntry));
		std::memset(mat->constantBufferIndex, 0xFF, sizeof(mat->constantBufferIndex));

		//mat->stateBitsEntry[MaterialTechniqueType::TECHNIQUE_UNLIT] = 0;
		// generate statebits ....

		std::memcpy(mat->stateBitsEntry, gamefonts->stateBitsEntry, sizeof(mat->stateBitsEntry));
		mat->stateBitsCount = gamefonts->stateBitsCount;
		mat->stateBitsTable = gamefonts->stateBitsTable;

		return mat;
	}

	void parse_config(const std::string& name, int& pixelHeight)
	{
		const auto path = name + ".config.json"s;
		auto file = filesystem::file(path);
		if (!file.exists())
		{
			ZONETOOL_WARNING("No %s found for %s.ttf. Using default settings.", path.data(), name.data());
			return;
		}
		file.open("rb");

		auto size = file.size();
		auto bytes = file.read_bytes(size);
		nlohmann::json config = nlohmann::json::parse(bytes);

#define READ_CONFIG_VAL(__name__, __type__) \
		if(config.contains(#__name__)) \
		{ \
			pixelHeight = config[#__name__].get<__type__>(); \
		} \
		else \
		{ \
			ZONETOOL_WARNING("%s", std::format("value {} of type {} not found in {}. using default value: {}", #__name__, #__type__, path, __name__).data()); \
		}

		READ_CONFIG_VAL(pixelHeight, int);
	}

	Font_s* font_def::parse_ttf(const std::string& name, zone_memory* mem)
	{
		const auto path = name + ".ttf"s;
		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}
		file.open("rb");

		ZONETOOL_INFO("Parsing ttf font \"%s\"...", name.data());

		int pixelHeight = 32;

		parse_config(name, pixelHeight);

		std::vector<unsigned char> ttfData = file.read_bytes(file.size());
		std::vector<Glyph> glyphs;
		std::vector<unsigned char> atlasPixels;
		int atlasWidth, atlasHeight;
		if (!GenerateFontAtlas(ttfData.data(), ttfData.size(), static_cast<float>(pixelHeight), glyphs, atlasPixels, atlasWidth, atlasHeight))
		{
			ZONETOOL_ERROR("Failed to generate font atlas for \"%s\".", name.data());
			return nullptr;
		}

		std::string base_name = name.data();
		if (name.starts_with("fonts/"))
		{
			base_name = name.substr(6);
		}
		const auto image_name = utils::string::va("%s_atlas", base_name.data());
		auto image = generate_image(image_name, atlasPixels, atlasWidth, atlasHeight, mem);

		const auto techniqueset = "2d";
		const auto sortkey = 54;
		const auto camera_region = CAMERA_REGION_NONE;

		auto font = mem->allocate<Font_s>();
		font->name = mem->duplicate_string(name);
		font->glyphCount = static_cast<int>(glyphs.size());
		font->pixelHeight = pixelHeight;
		font->material = generate_material(name, image, techniqueset, sortkey, camera_region, mem);
		font->glowMaterial = generate_material(name + "_glow", image, techniqueset, sortkey, camera_region, mem);
		font->glyphs = mem->allocate<Glyph>(glyphs.size());
		std::memcpy(font->glyphs, glyphs.data(), sizeof(Glyph) * glyphs.size());

		this->ttfFont = true;

		return font;
	}

	Font_s* font_def::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = name + ".json"s;
		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return parse_ttf(name, mem);
		}
		file.open("rb");

		ZONETOOL_INFO("Parsing font \"%s\"...", name.data());

		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();

		nlohmann::json fontdata = nlohmann::json::parse(bytes);

		auto font = mem->allocate<Font_s>();
		font->name = mem->duplicate_string(fontdata["fontName"].get<std::string>().data());
		font->glyphCount = static_cast<int>(fontdata["glyphs"].size());
		font->pixelHeight = fontdata["pixelHeight"].get<int>();

		font->material = mem->allocate<Material>();
		font->material->name = mem->duplicate_string(fontdata["material"].get<std::string>().data());
		font->glowMaterial = mem->allocate<Material>();
		font->glowMaterial->name = mem->duplicate_string(fontdata["glowMaterial"].get<std::string>().data());

		font->glyphs = mem->allocate<Glyph>(font->glyphCount);

		for (int i = 0; i < font->glyphCount; i++)
		{
			font->glyphs[i].letter = fontdata["glyphs"][i]["letter"].get<unsigned short>();
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

	void font_def::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).font;
		}
	}

	void font_def::prepare(zone_buffer* buf, zone_memory* mem)
	{}

	void font_def::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		if (this->ttfFont)
		{
			zone->add_asset_of_type_by_pointer(ASSET_TYPE_IMAGE, data->material->textureTable[0].u.image);
			zone->add_asset_of_type_by_pointer(ASSET_TYPE_MATERIAL, data->material);
			zone->add_asset_of_type_by_pointer(ASSET_TYPE_MATERIAL, data->glowMaterial);
			return;
		}

		if (data->material)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->material->name);
		}

		if (data->glowMaterial)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->glowMaterial->name);
		}
	}

	std::string font_def::name()
	{
		return this->name_;
	}

	std::int32_t font_def::type()
	{
		return ASSET_TYPE_FONT;
	}

	void font_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write<Font_s>(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

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
			buf->clear_pointer(&dest->glyphs);
		}

		buf->pop_stream();
	}

	void font_def::dump(Font_s* asset)
	{
		const auto path = asset->fontName + ".json"s;
		auto f = filesystem::file(path);
		f.open("wb");

		ordered_json fontdata;
		ordered_json glyph;

		fontdata["fontName"] = asset->fontName;
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