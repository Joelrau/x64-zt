#include <std_include.hpp>
#include "streaming_info.hpp"

namespace zonetool::iw7
{
	namespace
	{
		unsigned int DB_StreamingInfo_CalcUInts(const unsigned int count)
		{
			return (4 * count + 31) >> 5;
		}
	}

	void parse_alwaysloaded_flags(assetmanager::reader& read, AlwaysloadedFlags& flags)
	{
		flags.images = read.read_array<GfxImage*>();
		for (unsigned int i = 0; i < flags.imageCount; i++)
		{
			flags.images[i] = read.read_asset<GfxImage>();
		}
		flags.imageFlags = read.read_array<unsigned int>();

		flags.strings = read.read_array<const char*>();
		for (unsigned int i = 0; i < flags.stringCount; i++)
		{
			flags.strings[i] = read.read_string();
		}
		flags.stringFlags = read.read_array<unsigned int>();

		flags.materials = read.read_array<Material*>();
		for (unsigned int i = 0; i < flags.materialCount; i++)
		{
			flags.materials[i] = read.read_asset<Material>();
		}
		flags.materialFlags = read.read_array<unsigned int>();
	}

	StreamingInfo* streaming_info::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);

		const auto path = "streaming_info\\"s + name;
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing streaminginfo \"%s\"...", name.data());

		auto* asset = read.read_single<StreamingInfo>();
		asset->name = read.read_string();

		parse_alwaysloaded_flags(read, asset->flags);

		asset->perModelFlags = read.read_array<AlwaysloadedFlagsPerModel>();
		for (unsigned int i = 0; i < asset->modelCount; i++)
		{
			asset->perModelFlags[i].model = read.read_asset<XModel>();
			parse_alwaysloaded_flags(read, asset->perModelFlags[i].flags);
		}

		asset->forcedStubAssets = read.read_array<ForcedStubAssetReference>();
		for (unsigned int i = 0; i < asset->forcedStubAssetCount; i++)
		{
			asset->forcedStubAssets[i].name = read.read_string();
		}

		asset->transientFiles = read.read_array<TransientFileReference>();
		for (unsigned int i = 0; i < asset->transientFileCount; i++)
		{
			asset->transientFiles[i].name = read.read_string();
		}

		asset->transientAssets = read.read_array<TransientAssetReference>();
		for (unsigned int i = 0; i < asset->transientAssetCount; i++)
		{
			asset->transientAssets[i].name = read.read_string();
		}

		for (unsigned int i = 0; i < asset->stringCount; i++)
		{
			asset->strings[i] = read.read_string();
		}

		asset->transientSets.names = read.read_array<const char*>();
		for (unsigned int i = 0; i < asset->transientSets.count; i++)
		{
			asset->transientSets.names[i] = read.read_string();
		}
		asset->transientSets.masks = read.read_array<unsigned int>();

		read.close();

		return asset;
	}

	void streaming_info::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).streamingInfo;
		}
	}

	void streaming_info::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void load_deps_alwaysloaded_flags(zone_base* zone, AlwaysloadedFlags* data)
	{
		for (unsigned int i = 0; i < data->imageCount; i++)
		{
			zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->images[i]->name);
		}

		for (unsigned int i = 0; i < data->materialCount; i++)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->materials[i]->name);
		}
	}

	void streaming_info::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		load_deps_alwaysloaded_flags(zone, &data->flags);

		for (unsigned int i = 0; i < data->modelCount; i++)
		{
			if (data->perModelFlags[i].model)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->perModelFlags[i].model->name);
			}
		}
	}

	std::string streaming_info::name()
	{
		return this->name_;
	}

	std::int32_t streaming_info::type()
	{
		return ASSET_TYPE_STREAMING_INFO;
	}

	void write_alwaysloaded_flags(zone_base* zone, zone_buffer* buf, AlwaysloadedFlags* data, AlwaysloadedFlags* dest)
	{
		if (data->images)
		{
			buf->align(7);
			dest->images = buf->write(data->images, data->imageCount);
			for (unsigned int i = 0; i < data->imageCount; i++)
			{
				if (data->images[i])
				{
					dest->images[i] = reinterpret_cast<GfxImage*>(
						zone->get_asset_pointer(ASSET_TYPE_IMAGE, data->images[i]->name));
				}
			}
			buf->clear_pointer(&dest->images);
		}

		if (data->imageFlags)
		{
			buf->align(3);
			buf->write(data->imageFlags, DB_StreamingInfo_CalcUInts(data->imageCount));
			buf->clear_pointer(&dest->imageFlags);
		}

		if (data->strings)
		{
			buf->align(7);
			dest->strings = buf->write(data->strings, data->stringCount);
			for (unsigned int i = 0; i < data->stringCount; i++)
			{
				if (data->strings[i])
				{
					dest->strings[i] = buf->write_str(data->strings[i]);
				}
			}
			buf->clear_pointer(&dest->strings);
		}

		if (data->stringFlags)
		{
			buf->align(3);
			buf->write(data->stringFlags, 4 * data->imageCount * data->stringCount);
			buf->clear_pointer(&dest->stringFlags);
		}

		if (data->materials)
		{
			buf->align(7);
			dest->materials = buf->write(data->materials, data->materialCount);
			for (unsigned int i = 0; i < data->materialCount; i++)
			{
				if (data->materials[i])
				{
					dest->materials[i] = reinterpret_cast<Material*>(
						zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->materials[i]->name));
				}
			}
			buf->clear_pointer(&dest->materials);
		}

		if (data->materialFlags)
		{
			buf->align(3);
			buf->write(data->materialFlags, DB_StreamingInfo_CalcUInts(data->materialCount));
			buf->clear_pointer(&dest->materialFlags);
		}
	}

	void streaming_info::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		write_alwaysloaded_flags(zone, buf, &data->flags, &dest->flags);

		if (data->perModelFlags)
		{
			buf->align(7);
			dest->perModelFlags = buf->write(data->perModelFlags, data->modelCount);
			for (unsigned int i = 0; i < data->modelCount; i++)
			{
				if (data->perModelFlags[i].model)
				{
					dest->perModelFlags[i].model = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->perModelFlags[i].model->name));
				}

				write_alwaysloaded_flags(zone, buf, &data->perModelFlags[i].flags, &dest->perModelFlags[i].flags);
			}
			buf->clear_pointer(&dest->perModelFlags);
		}

		if (data->forcedStubAssets)
		{
			buf->align(7);
			dest->forcedStubAssets = buf->write(data->forcedStubAssets, data->forcedStubAssetCount);
			for (unsigned int i = 0; i < data->forcedStubAssetCount; i++)
			{
				if (data->forcedStubAssets[i].name)
				{
					dest->forcedStubAssets[i].name = buf->write_str(data->forcedStubAssets[i].name);
				}
			}
			buf->clear_pointer(&dest->forcedStubAssets);
		}

		if (data->transientFiles)
		{
			buf->align(7);
			dest->transientFiles = buf->write(data->transientFiles, data->transientFileCount);
			for (unsigned int i = 0; i < data->transientFileCount; i++)
			{
				if (data->transientFiles[i].name)
				{
					dest->transientFiles[i].name = buf->write_str(data->transientFiles[i].name);
				}
			}
			buf->clear_pointer(&dest->transientFiles);
		}

		if (data->transientAssets)
		{
			buf->align(7);
			dest->transientAssets = buf->write(data->transientAssets, data->transientAssetCount);
			for (unsigned int i = 0; i < data->transientAssetCount; i++)
			{
				if (data->transientAssets[i].name)
				{
					dest->transientAssets[i].name = buf->write_str(data->transientAssets[i].name);
				}
			}
			buf->clear_pointer(&dest->transientAssets);
		}

		if (data->strings)
		{
			buf->align(7);
			dest->strings = buf->write(data->strings, data->stringCount);
			for (unsigned int i = 0; i < data->stringCount; i++)
			{
				if (data->strings[i])
				{
					dest->strings[i] = buf->write_str(data->strings[i]);
				}
			}
			buf->clear_pointer(&dest->strings);
		}

		if (data->transientSets.names)
		{
			buf->align(7);
			dest->transientSets.names = buf->write(data->transientSets.names, data->transientSets.count);
			for (unsigned int i = 0; i < data->transientSets.count; i++)
			{
				if (data->transientSets.names[i])
				{
					dest->transientSets.names[i] = buf->write_str(data->transientSets.names[i]);
				}
			}
			buf->clear_pointer(&dest->transientSets.names);
		}

		if (data->transientSets.masks)
		{
			buf->align(3);
			buf->write(data->transientSets.masks, data->transientSets.count);
			buf->clear_pointer(&dest->transientSets.masks);
		}

		buf->pop_stream();
	}

	void dump_alwaysloaded_flags(assetmanager::dumper& dump, AlwaysloadedFlags& flags)
	{
		dump.dump_array(flags.images, flags.imageCount);
		for (unsigned int i = 0; i < flags.imageCount; i++)
		{
			dump.dump_asset(flags.images[i]);
		}
		dump.dump_array(flags.imageFlags, DB_StreamingInfo_CalcUInts(flags.imageCount));

		dump.dump_array(flags.strings, flags.stringCount);
		for (unsigned int i = 0; i < flags.stringCount; i++)
		{
			dump.dump_string(flags.strings[i]);
		}
		dump.dump_array(flags.stringFlags, 4 * flags.imageCount * flags.stringCount);

		dump.dump_array(flags.materials, flags.materialCount);
		for (unsigned int i = 0; i < flags.materialCount; i++)
		{
			dump.dump_asset(flags.materials[i]);
		}
		dump.dump_array(flags.materialFlags, DB_StreamingInfo_CalcUInts(flags.materialCount));
	}

	void streaming_info::dump(StreamingInfo* asset)
	{
		assetmanager::dumper dump;

		const auto path = "streaming_info\\"s + asset->name;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump_alwaysloaded_flags(dump, asset->flags);

		dump.dump_array(asset->perModelFlags, asset->modelCount);
		for (unsigned int i = 0; i < asset->modelCount; i++)
		{
			dump.dump_asset(asset->perModelFlags[i].model);
			dump_alwaysloaded_flags(dump, asset->perModelFlags[i].flags);
		}

		dump.dump_array(asset->forcedStubAssets, asset->forcedStubAssetCount);
		for (unsigned int i = 0; i < asset->forcedStubAssetCount; i++)
		{
			dump.dump_string(asset->forcedStubAssets[i].name);
		}

		dump.dump_array(asset->transientFiles, asset->transientFileCount);
		for (unsigned int i = 0; i < asset->transientFileCount; i++)
		{
			dump.dump_string(asset->transientFiles[i].name);
		}

		dump.dump_array(asset->transientAssets, asset->transientAssetCount);
		for (unsigned int i = 0; i < asset->transientAssetCount; i++)
		{
			dump.dump_string(asset->transientAssets[i].name);
		}

		for (unsigned int i = 0; i < asset->stringCount; i++)
		{
			dump.dump_string(asset->strings[i]);
		}

		dump.dump_array(asset->transientSets.names, asset->transientSets.count);
		for (unsigned int i = 0; i < asset->transientSets.count; i++)
		{
			dump.dump_string(asset->transientSets.names[i]);
		}
		dump.dump_array(asset->transientSets.masks, asset->transientSets.count);

		dump.close();
	}
}