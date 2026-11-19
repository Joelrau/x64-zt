#include <std_include.hpp>
#include "fxworld.hpp"

namespace zonetool::iw8
{
	namespace
	{
		template <typename Walk>
		void walk_glass_globals(Walk walk, FxGlassGlobals& globals)
		{
			if (const auto defs = walk.array(globals.defs, globals.defCount, 7))
			{
				for (auto i = 0u; i < globals.defCount; i++)
				{
					auto& def = defs[i];
					walk.asset(ASSET_TYPE_PHYSICSASSET, def.physicsAsset);
					walk.asset(ASSET_TYPE_MATERIAL, def.material);
					walk.asset(ASSET_TYPE_MATERIAL, def.materialShattered);
					walk.asset(ASSET_TYPE_VFX, def.pieceBreakEffect.particleSystemDef);
					walk.asset(ASSET_TYPE_VFX, def.shatterEffect.particleSystemDef);
					walk.asset(ASSET_TYPE_VFX, def.shatterSmallEffect.particleSystemDef);
					walk.asset(ASSET_TYPE_VFX, def.crackDecalEffect.particleSystemDef);
					walk.string(def.damagedSound.name);
					walk.string(def.destroyedSound.name);
					walk.string(def.destroyedQuietSound.name);
				}
			}

			walk.array(globals.glassTransientLookup, globals.glassTransientLookupCount, 1);

			walk.push_stream(XFILE_BLOCK_RUNTIME);
			walk.array(globals.distances, globals.defCount, 3);
			walk.pop_stream();
		}

		template <typename Walk>
		void walk_glass_system(Walk walk, FxGlassSystem& system)
		{
			walk.push_stream(XFILE_BLOCK_RUNTIME);
			walk.array(system.piecePlaces, system.pieceLimit, 3);
			walk.array(system.pieceStates, system.pieceLimit, 3);
			walk.array(system.pieceDynamics, system.pieceLimit, 3);
			walk.array(system.geoData, system.geoDataLimit, 1);
			walk.array(system.isInUse, system.pieceWordCount, 3);
			walk.array(system.cellBits, system.pieceWordCount * system.cellCount, 3);
			walk.array(system.visData, (system.pieceLimit + 15) & ~15u, 15);
			walk.array(system.linkOrg, system.pieceLimit, 3);
			walk.array(system.halfThickness, (system.pieceLimit + 3) & ~3u, 15);
			walk.pop_stream();

			walk.array(system.initGeoData, system.initGeoDataCount, 1);
			walk.array(system.initPieceStates, system.initPieceCount, 3);
		}

		template <typename Walk>
		void walk_fx_world(Walk walk, FxWorld& asset)
		{
			walk_glass_globals(walk, asset.glassGlob);
			walk_glass_system(walk, asset.glassSys);

			for (auto& zone : asset.transientZones)
			{
				walk.asset(ASSET_TYPE_FXWORLD_TRANSIENT_ZONE, zone);
			}
		}

		template <typename Walk>
		void walk_fx_world_transient_zone(Walk walk, FxWorldTransientZone& asset)
		{
			walk_glass_system(walk, asset.glassSys);
		}
	}

	void fx_world::read(zone_reader& reader, FxWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_fx_world(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void fx_world::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_fx_world(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void fx_world::dump(FxWorld* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(asset->name + ".fxmap"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_fx_world(dump_walk{dump}, *asset);
		dump.close();
	}

	FxWorld* fx_world::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(name + ".fxmap"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing fxworld \"%s\"...", name.data());

		auto* asset = read.read_single<FxWorld>();
		asset->name = read.read_string();
		walk_fx_world(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void fx_world::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<FxWorld>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void fx_world::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_fx_world(depend_walk{zone}, *this->asset_);
	}

	std::string fx_world::name()
	{
		return this->name_;
	}

	std::int32_t fx_world::type()
	{
		return ASSET_TYPE_FXWORLD;
	}

	void fx_world_transient_zone::read(zone_reader& reader, FxWorldTransientZone* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_fx_world_transient_zone(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void fx_world_transient_zone::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_fx_world_transient_zone(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void fx_world_transient_zone::dump(FxWorldTransientZone* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("transient_zones\\"s + asset->name + ".fxmap_tr"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_fx_world_transient_zone(dump_walk{dump}, *asset);
		dump.close();
	}

	FxWorldTransientZone* fx_world_transient_zone::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("transient_zones\\"s + name + ".fxmap_tr"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing fxworld_tr \"%s\"...", name.data());

		auto* asset = read.read_single<FxWorldTransientZone>();
		asset->name = read.read_string();
		walk_fx_world_transient_zone(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void fx_world_transient_zone::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<FxWorldTransientZone>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string fx_world_transient_zone::name()
	{
		return this->name_;
	}

	std::int32_t fx_world_transient_zone::type()
	{
		return ASSET_TYPE_FXWORLD_TRANSIENT_ZONE;
	}
}
