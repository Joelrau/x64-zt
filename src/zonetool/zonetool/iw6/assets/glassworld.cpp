#include <std_include.hpp>
#include "glassworld.hpp"

namespace zonetool::iw6
{
	GlassWorld* glass_world::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = name + ".glassmap";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing glassmap \"%s\" ...", name.data());

		GlassWorld* asset = read.read_single<GlassWorld>();
		asset->name = read.read_string();
		asset->g_glassData = read.read_single<G_GlassData>();

		if (asset->g_glassData)
		{
			asset->g_glassData->glassPieces = read.read_array<G_GlassPiece>();
			asset->g_glassData->glassNames = read.read_array<G_GlassName>();

			for (unsigned int i = 0; i < asset->g_glassData->glassNameCount; i++)
			{
				asset->g_glassData->glassNames[i].nameStr = read.read_string();
				asset->g_glassData->glassNames[i].pieceIndices = read.read_array<unsigned short>();
			}
		}

		read.close();

		return asset;
	}

	void glass_world::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") + filesystem::get_fastfile() + ".d3dbsp"; // name;
		this->asset_ = this->parse(name, mem);

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse glassmap \"%s\"", name.data());
		}
	}

	void glass_world::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void glass_world::load_depending(zone_base* zone)
	{
	}

	std::string glass_world::name()
	{
		return this->name_;
	}

	std::int32_t glass_world::type()
	{
		return ASSET_TYPE_GLASSWORLD;
	}

	void glass_world::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->g_glassData)
		{
			buf->align(3);

			auto glassdata = data->g_glassData;
			auto destdata = buf->write(glassdata);

			if (glassdata->glassPieces)
			{
				buf->align(3);
				buf->write(glassdata->glassPieces, glassdata->pieceCount);
				zone_buffer::clear_pointer(&destdata->glassPieces);
			}
			if (glassdata->glassNames)
			{
				buf->align(3);
				auto namedest = buf->write(glassdata->glassNames, glassdata->glassNameCount);

				for (unsigned int i = 0; i < glassdata->glassNameCount; i++)
				{
					namedest[i].nameStr = buf->write_str(glassdata->glassNames[i].nameStr);

					if (glassdata->glassNames[i].pieceIndices)
					{
						buf->align(1);
						buf->write(glassdata->glassNames[i].pieceIndices, glassdata->glassNames[i].pieceCount);
						zone_buffer::clear_pointer(&namedest[i].pieceIndices);
					}
				}

				zone_buffer::clear_pointer(&destdata->glassNames);
			}

			zone_buffer::clear_pointer(&dest->g_glassData);
		}
		buf->pop_stream();
	}

	void glass_world::dump(GlassWorld* asset)
	{
		const auto path = asset->name + ".glassmap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_single(asset->g_glassData);

		if (asset->g_glassData)
		{
			write.dump_array(asset->g_glassData->glassPieces, asset->g_glassData->pieceCount);
			write.dump_array(asset->g_glassData->glassNames, asset->g_glassData->glassNameCount);

			for (unsigned int i = 0; i < asset->g_glassData->glassNameCount; i++)
			{
				write.dump_string(asset->g_glassData->glassNames[i].nameStr);
				write.dump_array(asset->g_glassData->glassNames[i].pieceIndices,
					asset->g_glassData->glassNames[i].pieceCount);
			}
		}

		write.close();
	}
}