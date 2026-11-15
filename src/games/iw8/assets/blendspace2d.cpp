#include <std_include.hpp>
#include "blendspace2d.hpp"

namespace zonetool::iw8
{
	void blend_space_2d::read(zone_reader& reader, BlendSpace2DDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto entries = reader.read_array(asset->animEntries, 7, asset->numAnimEntries))
		{
			for (auto i = 0u; i < asset->numAnimEntries; i++)
			{
				reader.read_asset(ASSET_TYPE_XANIMPARTS, entries[i].anim);
			}
		}

		reader.read_array(asset->triangleCoords, 3, asset->numTriangleCoords);
		reader.pop_stream();
	}

	void blend_space_2d::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (const auto entries = write_array(buf, dest->animEntries, data->numAnimEntries, 7))
		{
			for (auto i = 0u; i < data->numAnimEntries; i++)
			{
				write_asset(zone, ASSET_TYPE_XANIMPARTS, entries[i].anim);
			}
		}

		write_array(buf, dest->triangleCoords, data->numTriangleCoords, 3);
		buf->pop_stream();
	}

	void blend_space_2d::dump(BlendSpace2DDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("blendspace2d\\"s + asset->name + ".blendspace2db"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_array(asset->animEntries, asset->numAnimEntries);
		if (asset->animEntries)
		{
			for (auto i = 0u; i < asset->numAnimEntries; i++)
			{
				dump.dump_asset(asset->animEntries[i].anim);
			}
		}

		dump.dump_array(asset->triangleCoords, asset->numTriangleCoords);
		dump.close();
	}

	BlendSpace2DDef* blend_space_2d::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("blendspace2d\\"s + name + ".blendspace2db"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing blendspace2d \"%s\"...", name.data());

		auto* asset = read.read_single<BlendSpace2DDef>();
		asset->name = read.read_string();
		if ((asset->animEntries = read.read_array<BlendSpace2DAnimEntry>()))
		{
			for (auto i = 0u; i < asset->numAnimEntries; i++)
			{
				parse_asset(read, asset->animEntries[i].anim);
			}
		}

		asset->triangleCoords = read.read_array<unsigned int>();
		read.close();
		return asset;
	}

	void blend_space_2d::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<BlendSpace2DDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void blend_space_2d::load_depending(zone_base* zone)
	{
		if (this->referenced() || !this->asset_->animEntries)
		{
			return;
		}

		for (auto i = 0u; i < this->asset_->numAnimEntries; i++)
		{
			add_asset(zone, ASSET_TYPE_XANIMPARTS, this->asset_->animEntries[i].anim);
		}
	}

	std::string blend_space_2d::name()
	{
		return this->name_;
	}

	std::int32_t blend_space_2d::type()
	{
		return ASSET_TYPE_BLEND_SPACE_2D;
	}
}
