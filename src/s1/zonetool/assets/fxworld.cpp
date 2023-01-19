#include <std_include.hpp>
#include "fxworld.hpp"

namespace zonetool::s1
{
	FxWorld* IFxWorld::parse(const std::string& name, ZoneMemory* mem)
	{
		const auto path = name + ".fxmap";

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

			asset->glassSys.defs[i].physPreset = read.read_asset<PhysPreset>();

			asset->glassSys.defs[i].pieceBreakEffect = read.read_asset<FxEffectDef>();
			asset->glassSys.defs[i].shatterEffect = read.read_asset<FxEffectDef>();
			asset->glassSys.defs[i].shatterSmallEffect = read.read_asset<FxEffectDef>();
			asset->glassSys.defs[i].crackDecalEffect = read.read_asset<FxEffectDef>();

			asset->glassSys.defs[i].damagedSound = read.read_asset<snd_alias_list_t>();
			asset->glassSys.defs[i].destroyedSound = read.read_asset<snd_alias_list_t>();
			asset->glassSys.defs[i].destroyedQuietSound = read.read_asset<snd_alias_list_t>();
		}

		asset->glassSys.piecePlaces = read.read_array<FxGlassPiecePlace>();
		asset->glassSys.pieceStates = read.read_array<FxGlassPieceState>();
		asset->glassSys.pieceDynamics = read.read_array<FxGlassPieceDynamics>();
		asset->glassSys.geoData = read.read_array<FxGlassGeometryData>();
		asset->glassSys.isInUse = read.read_array<unsigned int>();
		asset->glassSys.cellBits = read.read_array<unsigned int>();
		asset->glassSys.visData = read.read_array<unsigned char>();
		asset->glassSys.linkOrg = read.read_array<float[3]>();
		asset->glassSys.halfThickness = read.read_array<float>();
		asset->glassSys.lightingHandles = read.read_array<unsigned short>();
		asset->glassSys.initGeoData = read.read_array<FxGlassGeometryData>();
		asset->glassSys.initPieceStates = read.read_array<FxGlassInitPieceState>();
		read.close();

		return asset;
	}

	void IFxWorld::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = "maps/"s + (filesystem::get_fastfile().substr(0, 3) == "mp_" ? "mp/" : "") + filesystem::get_fastfile() + ".d3dbsp"; // name;
		this->asset_ = this->parse(name, mem);

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse fxmap \"%s\"", name.data());
		}
	}

	void IFxWorld::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IFxWorld::load_depending(IZone* zone)
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

				if (data->glassSys.defs[i].physPreset)
				{
					zone->add_asset_of_type(ASSET_TYPE_PHYSPRESET, data->glassSys.defs[i].physPreset->name);
				}

				if (data->glassSys.defs[i].pieceBreakEffect)
				{
					zone->add_asset_of_type(ASSET_TYPE_FX, data->glassSys.defs[i].pieceBreakEffect->name);
				}
				if (data->glassSys.defs[i].shatterEffect)
				{
					zone->add_asset_of_type(ASSET_TYPE_FX, data->glassSys.defs[i].shatterEffect->name);
				}
				if (data->glassSys.defs[i].shatterSmallEffect)
				{
					zone->add_asset_of_type(ASSET_TYPE_FX, data->glassSys.defs[i].shatterSmallEffect->name);
				}
				if (data->glassSys.defs[i].crackDecalEffect)
				{
					zone->add_asset_of_type(ASSET_TYPE_FX, data->glassSys.defs[i].crackDecalEffect->name);
				}

				if (data->glassSys.defs[i].damagedSound)
				{
					zone->add_asset_of_type(ASSET_TYPE_SOUND, data->glassSys.defs[i].damagedSound->name);
				}
				if (data->glassSys.defs[i].destroyedSound)
				{
					zone->add_asset_of_type(ASSET_TYPE_SOUND, data->glassSys.defs[i].destroyedSound->name);
				}
				if (data->glassSys.defs[i].destroyedQuietSound)
				{
					zone->add_asset_of_type(ASSET_TYPE_SOUND, data->glassSys.defs[i].destroyedQuietSound->name);
				}
			}
		}
	}

	std::string IFxWorld::name()
	{
		return this->name_;
	}

	std::int32_t IFxWorld::type()
	{
		return ASSET_TYPE_FXWORLD;
	}

	void IFxWorld::write(IZone* zone, ZoneBuffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->glassSys.defs)
		{
			buf->align(3);
			auto* glass_def = buf->write(data->glassSys.defs, data->glassSys.defCount);

			for (std::uint32_t i = 0; i < data->glassSys.defCount; i++)
			{
				if (data->glassSys.defs[i].physPreset)
				{
					glass_def[i].physPreset = reinterpret_cast<PhysPreset*>(zone->get_asset_pointer(
						ASSET_TYPE_PHYSPRESET, data->glassSys.defs[i].physPreset->name));
				}

				if (data->glassSys.defs[i].material)
				{
					glass_def[i].material = reinterpret_cast<Material*>(zone->get_asset_pointer(
						ASSET_TYPE_MATERIAL, data->glassSys.defs[i].material->name));
				}
				if (data->glassSys.defs[i].materialShattered)
				{
					glass_def[i].materialShattered = reinterpret_cast<Material*>(zone->get_asset_pointer(
						ASSET_TYPE_MATERIAL, data->glassSys.defs[i].materialShattered->name));
				}

				if (data->glassSys.defs[i].pieceBreakEffect)
				{
					glass_def[i].pieceBreakEffect = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
						ASSET_TYPE_FX, data->glassSys.defs[i].pieceBreakEffect->name));
				}
				if (data->glassSys.defs[i].shatterEffect)
				{
					glass_def[i].shatterEffect = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
						ASSET_TYPE_FX, data->glassSys.defs[i].shatterEffect->name));
				}
				if (data->glassSys.defs[i].shatterSmallEffect)
				{
					glass_def[i].shatterSmallEffect = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
						ASSET_TYPE_FX, data->glassSys.defs[i].shatterSmallEffect->name));
				}
				if (data->glassSys.defs[i].crackDecalEffect)
				{
					glass_def[i].crackDecalEffect = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(
						ASSET_TYPE_FX, data->glassSys.defs[i].crackDecalEffect->name));
				}

				if (data->glassSys.defs[i].damagedSound)
				{
					auto ptr = 0xFDFDFDFFFFFFFFFF;
					buf->write(&ptr);
					buf->align(7);
					buf->write_str(data->glassSys.defs[i].damagedSound->name);
					ZoneBuffer::clear_pointer(&glass_def[i].damagedSound);
				}
				if (data->glassSys.defs[i].destroyedSound)
				{
					auto ptr = 0xFDFDFDFFFFFFFFFF;
					buf->write(&ptr);
					buf->align(7);
					buf->write_str(data->glassSys.defs[i].destroyedSound->name);
					ZoneBuffer::clear_pointer(&glass_def[i].destroyedSound);
				}
				if (data->glassSys.defs[i].destroyedQuietSound)
				{
					auto ptr = 0xFDFDFDFFFFFFFFFF;
					buf->write(&ptr);
					buf->align(7);
					buf->write_str(data->glassSys.defs[i].destroyedQuietSound->name);
					ZoneBuffer::clear_pointer(&glass_def[i].destroyedQuietSound);
				}
			}

			ZoneBuffer::clear_pointer(&dest->glassSys.defs);
		}

		buf->push_stream(2);
		if (data->glassSys.piecePlaces)
		{
			buf->align(3);
			buf->write(data->glassSys.piecePlaces, data->glassSys.pieceLimit);
			ZoneBuffer::clear_pointer(&dest->glassSys.piecePlaces);
		}

		if (data->glassSys.pieceStates)
		{
			buf->align(3);
			buf->write(data->glassSys.pieceStates, data->glassSys.pieceLimit);
			ZoneBuffer::clear_pointer(&dest->glassSys.pieceStates);
		}

		if (data->glassSys.pieceDynamics)
		{
			buf->align(3);
			buf->write(data->glassSys.pieceDynamics, data->glassSys.pieceLimit);
			ZoneBuffer::clear_pointer(&dest->glassSys.pieceDynamics);
		}

		if (data->glassSys.geoData)
		{
			buf->align(3);
			buf->write(data->glassSys.geoData, data->glassSys.geoDataLimit);
			ZoneBuffer::clear_pointer(&dest->glassSys.geoData);
		}

		if (data->glassSys.isInUse)
		{
			buf->align(3);
			buf->write(data->glassSys.isInUse, data->glassSys.pieceWordCount);
			ZoneBuffer::clear_pointer(&dest->glassSys.isInUse);
		}

		if (data->glassSys.cellBits)
		{
			buf->align(3);
			buf->write(data->glassSys.cellBits, data->glassSys.pieceWordCount * data->glassSys.cellCount);
			ZoneBuffer::clear_pointer(&dest->glassSys.cellBits);
		}

		if (data->glassSys.visData)
		{
			buf->align(15);
			buf->write(data->glassSys.visData, (data->glassSys.pieceLimit + 15) & 0xFFFFFFF0);
			ZoneBuffer::clear_pointer(&dest->glassSys.visData);
		}

		if (data->glassSys.linkOrg)
		{
			buf->align(3);
			buf->write(data->glassSys.linkOrg, data->glassSys.pieceLimit);
			ZoneBuffer::clear_pointer(&dest->glassSys.linkOrg);
		}

		if (data->glassSys.halfThickness)
		{
			buf->align(15);
			buf->write(data->glassSys.halfThickness, (data->glassSys.pieceLimit + 3) & 0xFFFFFFFC);
			ZoneBuffer::clear_pointer(&dest->glassSys.halfThickness);
		}
		buf->pop_stream();

		if (data->glassSys.lightingHandles)
		{
			buf->align(1);
			buf->write(data->glassSys.lightingHandles, data->glassSys.initPieceCount);
			ZoneBuffer::clear_pointer(&dest->glassSys.lightingHandles);
		}

		if (data->glassSys.initGeoData)
		{
			buf->align(3);
			buf->write(data->glassSys.initGeoData, data->glassSys.initGeoDataCount);
			ZoneBuffer::clear_pointer(&dest->glassSys.initGeoData);
		}

		if (data->glassSys.initPieceStates)
		{
			buf->align(3);
			buf->write(data->glassSys.initPieceStates, data->glassSys.initPieceCount);
			ZoneBuffer::clear_pointer(&dest->glassSys.initPieceStates);
		}

		buf->pop_stream();
	}

	void IFxWorld::dump(FxWorld* asset)
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

			write.dump_asset(asset->glassSys.defs[i].physPreset);

			write.dump_asset(asset->glassSys.defs[i].pieceBreakEffect);
			write.dump_asset(asset->glassSys.defs[i].shatterEffect);
			write.dump_asset(asset->glassSys.defs[i].shatterSmallEffect);
			write.dump_asset(asset->glassSys.defs[i].crackDecalEffect);

			write.dump_asset(asset->glassSys.defs[i].damagedSound);
			write.dump_asset(asset->glassSys.defs[i].destroyedSound);
			write.dump_asset(asset->glassSys.defs[i].destroyedQuietSound);
		}

		write.dump_array(asset->glassSys.piecePlaces, asset->glassSys.pieceLimit);

		write.dump_array(asset->glassSys.pieceStates, asset->glassSys.pieceLimit);

		write.dump_array(asset->glassSys.pieceDynamics, asset->glassSys.pieceLimit);

		write.dump_array(asset->glassSys.geoData, asset->glassSys.geoDataLimit);

		write.dump_array(asset->glassSys.isInUse, asset->glassSys.pieceWordCount);

		write.dump_array(asset->glassSys.cellBits, asset->glassSys.pieceWordCount * asset->glassSys.cellCount);

		write.dump_array(asset->glassSys.visData, (asset->glassSys.pieceLimit + 15) & 0xFFFFFFF0);

		write.dump_array(asset->glassSys.linkOrg, asset->glassSys.pieceLimit);

		write.dump_array(asset->glassSys.halfThickness, (asset->glassSys.pieceLimit + 3) & 0xFFFFFFFC);

		write.dump_array(asset->glassSys.lightingHandles, asset->glassSys.initPieceCount);

		write.dump_array(asset->glassSys.initGeoData, asset->glassSys.initGeoDataCount);

		write.dump_array(asset->glassSys.initPieceStates, asset->glassSys.initPieceCount);
		
		write.close();
	}
}