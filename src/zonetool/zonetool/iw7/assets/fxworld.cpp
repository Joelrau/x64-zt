#include <std_include.hpp>
#include "fxworld.hpp"

namespace zonetool::iw7
{
#define READ_VFX(__NAME__) \
	if (__NAME__.type == FX_COMBINED_VFX) \
	{ \
		__NAME__.u.vfx = read.read_asset<ParticleSystemDef>(); \
	} \
	else \
	{ \
		__NAME__.u.fx = read.read_asset<FxEffectDef>(); \
	}

	FxWorld* fx_world::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = name + ".fxmap"s;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing fxmap \"%s\" ...", name.data());

		FxWorld* asset = read.read_single<FxWorld>();
		asset->name = read.read_string();

		asset->glassSys.defs = read.read_array<FxGlassDef>();
		for (unsigned int i = 0; i < asset->glassSys.defCount; i++)
		{
			asset->glassSys.defs[i].material = read.read_asset<Material>();
			asset->glassSys.defs[i].materialShattered = read.read_asset<Material>();

			asset->glassSys.defs[i].physicsAsset = read.read_asset<PhysicsAsset>();

			READ_VFX(asset->glassSys.defs[i].pieceBreakEffect);
			READ_VFX(asset->glassSys.defs[i].shatterEffect);
			READ_VFX(asset->glassSys.defs[i].shatterSmallEffect);
			READ_VFX(asset->glassSys.defs[i].crackDecalEffect);

			asset->glassSys.defs[i].damagedSound = read.read_string();
			asset->glassSys.defs[i].destroyedSound = read.read_string();
			asset->glassSys.defs[i].destroyedQuietSound = read.read_string();
		}

		asset->glassSys.piecePlaces = mem->allocate<FxGlassPiecePlace>(asset->glassSys.pieceLimit);
		asset->glassSys.pieceStates = mem->allocate<FxGlassPieceState>(asset->glassSys.pieceLimit);
		asset->glassSys.pieceDynamics = mem->allocate<FxGlassPieceDynamics>(asset->glassSys.pieceLimit);
		asset->glassSys.geoData = mem->allocate<FxGlassGeometryData>(asset->glassSys.geoDataLimit);
		asset->glassSys.isInUse = mem->allocate<unsigned int>(asset->glassSys.pieceWordCount);
		asset->glassSys.cellBits = mem->allocate<unsigned int>(asset->glassSys.pieceWordCount * asset->glassSys.cellCount);
		asset->glassSys.visData = mem->allocate<unsigned char>((asset->glassSys.pieceLimit + 15) & 0xFFFFFFF0);
		asset->glassSys.linkOrg = mem->allocate<float[3]>(asset->glassSys.pieceLimit);
		asset->glassSys.halfThickness = mem->allocate<float>((asset->glassSys.pieceLimit + 3) & 0xFFFFFFFC);

		asset->glassSys.lightingHandles = read.read_array<unsigned short>();
		asset->glassSys.initGeoData = read.read_array<FxGlassGeometryData>();
		asset->glassSys.initPieceStates = read.read_array<FxGlassInitPieceState>();
		read.close();

		return asset;
	}

#undef READ_VFX

	void fx_world::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;
		this->asset_ = this->parse(name, mem);

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse fxmap \"%s\"", name.data());
		}
	}

	void fx_world::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

#define DEPENDING_VFX(__NAME__) \
	if (__NAME__.u.data) \
	{ \
		if (__NAME__.type == FX_COMBINED_VFX) \
		{ \
			zone->add_asset_of_type(ASSET_TYPE_VFX, __NAME__.u.vfx->name); \
		} \
		else \
		{ \
			zone->add_asset_of_type(ASSET_TYPE_FX, __NAME__.u.fx->name); \
		} \
	}
	

	void fx_world::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		if (data->glassSys.defs)
		{
			for (unsigned int i = 0; i < data->glassSys.defCount; i++)
			{
				if (data->glassSys.defs[i].material)
				{
					zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->glassSys.defs[i].material->name);
				}
				if (data->glassSys.defs[i].materialShattered)
				{
					zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->glassSys.defs[i].materialShattered->name);
				}

				if (data->glassSys.defs[i].physicsAsset)
				{
					zone->add_asset_of_type(ASSET_TYPE_PHYSICSASSET, data->glassSys.defs[i].physicsAsset->name);
				}

				DEPENDING_VFX(data->glassSys.defs[i].pieceBreakEffect);
				DEPENDING_VFX(data->glassSys.defs[i].shatterEffect);
				DEPENDING_VFX(data->glassSys.defs[i].shatterSmallEffect);
				DEPENDING_VFX(data->glassSys.defs[i].crackDecalEffect);

				// data->glassSys.defs[i].damagedSound
				// data->glassSys.defs[i].destroyedSound
				// data->glassSys.defs[i].destroyedQuietSound
			}
		}
	}

#undef DEPENDING_VFX

	std::string fx_world::name()
	{
		return this->name_;
	}

	std::int32_t fx_world::type()
	{
		return ASSET_TYPE_FXWORLD;
	}

#define WRITE_VFX(__NAME__) \
	if (data->__NAME__.u.data) \
	{ \
		if (data->__NAME__.type == FX_COMBINED_VFX) \
		{ \
			dest->__NAME__.u.vfx = reinterpret_cast<ParticleSystemDef*>(zone->get_asset_pointer(ASSET_TYPE_VFX, data->__NAME__.u.vfx->name)); \
		} \
		else \
		{ \
			dest->__NAME__.u.fx = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->__NAME__.u.fx->name)); \
		} \
	}

	void fx_world::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->glassSys.defs)
		{
			buf->align(7);
			dest->glassSys.defs = buf->write(data->glassSys.defs, data->glassSys.defCount);

			for (unsigned int i = 0; i < data->glassSys.defCount; i++)
			{
				if (data->glassSys.defs[i].physicsAsset)
				{
					dest->glassSys.defs[i].physicsAsset = reinterpret_cast<PhysicsAsset*>(zone->get_asset_pointer(
						ASSET_TYPE_PHYSICSASSET, data->glassSys.defs[i].physicsAsset->name));
				}

				if (data->glassSys.defs[i].material)
				{
					dest->glassSys.defs[i].material = reinterpret_cast<Material*>(zone->get_asset_pointer(
						ASSET_TYPE_MATERIAL, data->glassSys.defs[i].material->name));
				}
				if (data->glassSys.defs[i].materialShattered)
				{
					dest->glassSys.defs[i].materialShattered = reinterpret_cast<Material*>(zone->get_asset_pointer(
						ASSET_TYPE_MATERIAL, data->glassSys.defs[i].materialShattered->name));
				}

				WRITE_VFX(glassSys.defs[i].pieceBreakEffect);
				WRITE_VFX(glassSys.defs[i].shatterEffect);
				WRITE_VFX(glassSys.defs[i].shatterSmallEffect);
				WRITE_VFX(glassSys.defs[i].crackDecalEffect);

				if (data->glassSys.defs[i].damagedSound)
				{
					dest->glassSys.defs[i].damagedSound = buf->write_str(data->glassSys.defs[i].damagedSound);
				}
				if (data->glassSys.defs[i].destroyedSound)
				{
					dest->glassSys.defs[i].destroyedSound = buf->write_str(data->glassSys.defs[i].destroyedSound);
				}
				if (data->glassSys.defs[i].destroyedQuietSound)
				{
					dest->glassSys.defs[i].destroyedQuietSound = buf->write_str(data->glassSys.defs[i].destroyedQuietSound);
				}
			}

			buf->clear_pointer(&dest->glassSys.defs);
		}

		buf->push_stream(XFILE_BLOCK_RUNTIME);
		if (data->glassSys.piecePlaces)
		{
			buf->align(3);
			buf->write(data->glassSys.piecePlaces, data->glassSys.pieceLimit);
			buf->clear_pointer(&dest->glassSys.piecePlaces);
		}

		if (data->glassSys.pieceStates)
		{
			buf->align(3);
			buf->write(data->glassSys.pieceStates, data->glassSys.pieceLimit);
			buf->clear_pointer(&dest->glassSys.pieceStates);
		}

		if (data->glassSys.pieceDynamics)
		{
			buf->align(3);
			buf->write(data->glassSys.pieceDynamics, data->glassSys.pieceLimit);
			buf->clear_pointer(&dest->glassSys.pieceDynamics);
		}

		if (data->glassSys.geoData)
		{
			buf->align(3);
			buf->write(data->glassSys.geoData, data->glassSys.geoDataLimit);
			buf->clear_pointer(&dest->glassSys.geoData);
		}

		if (data->glassSys.isInUse)
		{
			buf->align(3);
			buf->write(data->glassSys.isInUse, data->glassSys.pieceWordCount);
			buf->clear_pointer(&dest->glassSys.isInUse);
		}

		if (data->glassSys.cellBits)
		{
			buf->align(3);
			buf->write(data->glassSys.cellBits, data->glassSys.pieceWordCount * data->glassSys.cellCount);
			buf->clear_pointer(&dest->glassSys.cellBits);
		}

		if (data->glassSys.visData)
		{
			buf->align(15);
			buf->write(data->glassSys.visData, (data->glassSys.pieceLimit + 15) & 0xFFFFFFF0);
			buf->clear_pointer(&dest->glassSys.visData);
		}

		if (data->glassSys.linkOrg)
		{
			buf->align(3);
			buf->write(data->glassSys.linkOrg, data->glassSys.pieceLimit);
			buf->clear_pointer(&dest->glassSys.linkOrg);
		}

		if (data->glassSys.halfThickness)
		{
			buf->align(15);
			buf->write(data->glassSys.halfThickness, (data->glassSys.pieceLimit + 3) & 0xFFFFFFFC);
			buf->clear_pointer(&dest->glassSys.halfThickness);
		}
		buf->pop_stream();

		if (data->glassSys.lightingHandles)
		{
			buf->align(1);
			buf->write(data->glassSys.lightingHandles, data->glassSys.initPieceCount);
			buf->clear_pointer(&dest->glassSys.lightingHandles);
		}

		if (data->glassSys.initPieceStates)
		{
			buf->align(3);
			buf->write(data->glassSys.initPieceStates, data->glassSys.initPieceCount);
			buf->clear_pointer(&dest->glassSys.initPieceStates);
		}

		if (data->glassSys.initGeoData)
		{
			buf->align(3);
			buf->write(data->glassSys.initGeoData, data->glassSys.initGeoDataCount);
			buf->clear_pointer(&dest->glassSys.initGeoData);
		}

		buf->pop_stream();
	}

#undef WRITE_VFX

#define DUMP_VFX(__NAME__) \
	if (__NAME__.type == FX_COMBINED_VFX) \
	{ \
		write.dump_asset(__NAME__.u.vfx); \
	} \
	else \
	{ \
		write.dump_asset(__NAME__.u.fx); \
	}

	void fx_world::dump(FxWorld* asset)
	{
		const auto path = asset->name + ".fxmap"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}
		
		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->glassSys.defs, asset->glassSys.defCount);
		for (unsigned int i = 0; i < asset->glassSys.defCount; i++)
		{
			write.dump_asset(asset->glassSys.defs[i].material);
			write.dump_asset(asset->glassSys.defs[i].materialShattered);

			write.dump_asset(asset->glassSys.defs[i].physicsAsset);

			DUMP_VFX(asset->glassSys.defs[i].pieceBreakEffect);
			DUMP_VFX(asset->glassSys.defs[i].shatterEffect);
			DUMP_VFX(asset->glassSys.defs[i].shatterSmallEffect);
			DUMP_VFX(asset->glassSys.defs[i].crackDecalEffect);
			
			write.dump_string(asset->glassSys.defs[i].damagedSound);
			write.dump_string(asset->glassSys.defs[i].destroyedSound);
			write.dump_string(asset->glassSys.defs[i].destroyedQuietSound);
		}

		write.dump_array(asset->glassSys.lightingHandles, asset->glassSys.initPieceCount);
		write.dump_array(asset->glassSys.initGeoData, asset->glassSys.initGeoDataCount);
		write.dump_array(asset->glassSys.initPieceStates, asset->glassSys.initPieceCount);
		
		write.close();
	}

#undef DUMP_VFX
}