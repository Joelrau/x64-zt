#include <std_include.hpp>
#include "comworld.hpp"

namespace zonetool::h2
{
	ComWorld* IComWorld::parse(const std::string& name, ZoneMemory* mem)
	{
		const auto path = name + ".commap";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing commap \"%s\"...", name.data());

		ComWorld* asset = read.read_single<ComWorld>();
		asset->name = read.read_string();

		asset->primaryLights = read.read_array<ComPrimaryLight>();
		for (unsigned int i = 0; i < asset->primaryLightCount; i++)
		{
			asset->primaryLights[i].defName = read.read_string();
		}
		asset->primaryLightEnvs = read.read_array<ComPrimaryLightEnv>();

		read.close();

		return asset;
	}

	void IComWorld::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") + filesystem::get_fastfile() + ".d3dbsp"; // name;
		this->asset_ = this->parse(name, mem);

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse commap \"%s\"", name.data());
		}
	}

	void IComWorld::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IComWorld::load_depending(IZone* zone)
	{
		auto asset = this->asset_;

		for (unsigned int i = 0; i < asset->primaryLightCount; i++)
		{
			if (asset->primaryLights[i].defName)
			{
				zone->add_asset_of_type(ASSET_TYPE_LIGHT_DEF, asset->primaryLights[i].defName);
			}
		}
	}

	std::string IComWorld::name()
	{
		return this->name_;
	}

	std::int32_t IComWorld::type()
	{
		return ASSET_TYPE_COM_MAP;
	}

	void IComWorld::write(IZone* zone, ZoneBuffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->primaryLights)
		{
			buf->align(3);
			auto* primary_light = buf->write(data->primaryLights, data->primaryLightCount);

			for (unsigned int i = 0; i < data->primaryLightCount; i++)
			{
				if (data->primaryLights[i].defName)
				{
					primary_light[i].defName = buf->write_str(data->primaryLights[i].defName);
				}
			}

			ZoneBuffer::clear_pointer(&dest->primaryLights);
		}

		if (data->primaryLightEnvs)
		{
			buf->align(3);
			buf->write(data->primaryLightEnvs, data->primaryLightEnvCount);
			ZoneBuffer::clear_pointer(&dest->primaryLightEnvs);
		}

		buf->pop_stream();
	}

	void IComWorld::dump(ComWorld* asset)
	{
		const auto path = asset->name + ".commap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->primaryLights, asset->primaryLightCount);
		for (unsigned int i = 0; i < asset->primaryLightCount; i++)
		{
			write.dump_string(asset->primaryLights[i].defName);
		}
		write.dump_array(asset->primaryLightEnvs, asset->primaryLightEnvCount);

		write.close();
	}
}