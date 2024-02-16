#include "std_include.hpp"
#include "physics_asset.hpp"

namespace zonetool::iw7
{
	PhysicsAsset* physics_asset::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "physicsasset\\"s + name;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physicsasset \"%s\"...", name.data());

		auto* asset = read.read_single<PhysicsAsset>();
		asset->name = read.read_string();

		asset->havokData = read.read_array<char>();

		asset->sfxEventAssets = mem->allocate<PhysicsSFXEventAsset*>(asset->numSFXEventAssets);
		for (auto i = 0; i < asset->numSFXEventAssets; i++)
		{
			asset->sfxEventAssets[i] = read.read_asset<PhysicsSFXEventAsset>();
		}

		asset->vfxEventAssets = mem->allocate<PhysicsVFXEventAsset*>(asset->numVFXEventAssets);
		for (auto i = 0; i < asset->numVFXEventAssets; i++)
		{
			asset->vfxEventAssets[i] = read.read_asset<PhysicsVFXEventAsset>();
		}

		read.close();

		return asset;
	}

	void physics_asset::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physicsAsset;
		}
	}

	void physics_asset::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void physics_asset::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;

		for (auto i = 0; i < asset->numSFXEventAssets; i++)
		{
			zone->add_asset_of_type(ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET, asset->sfxEventAssets[i]->name);
		}

		for (auto i = 0; i < asset->numVFXEventAssets; i++)
		{
			zone->add_asset_of_type(ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET, asset->vfxEventAssets[i]->name);
		}
	}

	std::string physics_asset::name()
	{
		return this->name_;
	}

	std::int32_t physics_asset::type()
	{
		return ASSET_TYPE_PHYSICSASSET;
	}

	void physics_asset::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->havokData)
		{
			buf->align(15);
			buf->write(data->havokData, data->havokDataSize);
			buf->clear_pointer(&dest->havokData);
		}

		if (data->sfxEventAssets)
		{
			buf->align(7);
			auto* dest_events = buf->write(data->sfxEventAssets, data->numSFXEventAssets);
			for (auto i = 0; i < data->numSFXEventAssets; i++)
			{
				dest_events[i] = reinterpret_cast<PhysicsSFXEventAsset*>(
					zone->get_asset_pointer(ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET, data->sfxEventAssets[i]->name));
			}
			buf->clear_pointer(&dest->sfxEventAssets);
		}

		if (data->vfxEventAssets)
		{
			buf->align(7);
			auto* dest_events = buf->write(data->vfxEventAssets, data->numVFXEventAssets);
			for (auto i = 0; i < data->numVFXEventAssets; i++)
			{
				dest_events[i] = reinterpret_cast<PhysicsVFXEventAsset*>(
					zone->get_asset_pointer(ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET, data->vfxEventAssets[i]->name));
			}
			buf->clear_pointer(&dest->vfxEventAssets);
		}

		buf->pop_stream();
	}

	void physics_asset::dump(PhysicsAsset* asset)
	{
		const auto path = "physicsasset\\"s + asset->name;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->havokData, asset->havokDataSize);

		for (auto i = 0; i < asset->numSFXEventAssets; i++)
		{
			write.dump_asset(asset->sfxEventAssets[i]);
		}

		for (auto i = 0; i < asset->numVFXEventAssets; i++)
		{
			write.dump_asset(asset->vfxEventAssets[i]);
		}

		write.close();
	}
}