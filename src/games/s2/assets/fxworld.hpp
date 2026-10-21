#pragma once

namespace zonetool::s2
{
	class fx_world : public asset_base<FxWorld, ASSET_TYPE_FXWORLD>
	{
	public:
		static void read(zone_reader& reader, FxWorld* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);

			auto* glass = &asset->glassSys;
			if (const auto defs = reader.read_array(glass->defs, 7, glass->defCount))
			{
				for (auto i = 0u; i < glass->defCount; i++)
				{
					reader.read_asset(ASSET_TYPE_PHYSPRESET, defs[i].physPreset);
					reader.read_asset(ASSET_TYPE_MATERIAL, defs[i].material);
					reader.read_asset(ASSET_TYPE_MATERIAL, defs[i].materialShattered);
					reader.read_asset(ASSET_TYPE_FX, defs[i].pieceBreakEffect);
					reader.read_asset(ASSET_TYPE_FX, defs[i].shatterEffect);
					reader.read_asset(ASSET_TYPE_FX, defs[i].shatterSmallEffect);
					reader.read_asset(ASSET_TYPE_FX, defs[i].crackDecalEffect);
					reader.read_name_reference(defs[i].damagedSound);
					reader.read_name_reference(defs[i].destroyedSound);
					reader.read_name_reference(defs[i].destroyedQuietSound);
				}
			}

			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, glass->piecePlaces, 3, glass->pieceLimit);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, glass->pieceStates, 3, glass->pieceLimit);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, glass->pieceDynamics, 7, glass->pieceLimit);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, glass->geoData, 1, glass->geoDataLimit);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, glass->isInUse, 3, glass->pieceWordCount);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, glass->cellBits, 3, cell_bits_count(glass));
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, glass->visData, 15, vis_data_count(glass));
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, glass->linkOrg, 3, glass->pieceLimit);
			read_runtime_array(reader, XFILE_BLOCK_RUNTIME, glass->halfThickness, 15, half_thickness_count(glass));

			reader.read_array(glass->lightingHandles, 1, glass->initPieceCount);
			reader.read_array(glass->initGeoData, 1, glass->initGeoDataCount);
			reader.read_array(glass->initPieceStates, 3, glass->initPieceCount);

			reader.pop_stream();
		}

		FxWorld* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_FXWORLD, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing fxworld \"%s\"...", name.data());

			auto* asset = read.read_single<FxWorld>();
			asset->name = read.read_string();

			auto* glass = &asset->glassSys;
			if (const auto defs = parse_array(read, mem, glass->defs, glass->defCount))
			{
				for (auto i = 0u; i < glass->defCount; i++)
				{
					defs[i].physPreset = read.read_asset<PhysPreset>();
					defs[i].material = read.read_asset<Material>();
					defs[i].materialShattered = read.read_asset<Material>();
					defs[i].pieceBreakEffect = read.read_asset<FxEffectDef>();
					defs[i].shatterEffect = read.read_asset<FxEffectDef>();
					defs[i].shatterSmallEffect = read.read_asset<FxEffectDef>();
					defs[i].crackDecalEffect = read.read_asset<FxEffectDef>();
					defs[i].damagedSound = read.read_asset<snd_alias_list_t>();
					defs[i].destroyedSound = read.read_asset<snd_alias_list_t>();
					defs[i].destroyedQuietSound = read.read_asset<snd_alias_list_t>();
				}
			}

			parse_runtime_array(mem, glass->piecePlaces, glass->pieceLimit);
			parse_runtime_array(mem, glass->pieceStates, glass->pieceLimit);
			parse_runtime_array(mem, glass->pieceDynamics, glass->pieceLimit);
			parse_runtime_array(mem, glass->geoData, glass->geoDataLimit);
			parse_runtime_array(mem, glass->isInUse, glass->pieceWordCount);
			parse_runtime_array(mem, glass->cellBits, cell_bits_count(glass));
			parse_runtime_array(mem, glass->visData, vis_data_count(glass));
			parse_runtime_array(mem, glass->linkOrg, glass->pieceLimit);
			parse_runtime_array(mem, glass->halfThickness, half_thickness_count(glass));

			parse_array(read, mem, glass->lightingHandles, glass->initPieceCount);
			parse_array(read, mem, glass->initGeoData, glass->initGeoDataCount);
			parse_array(read, mem, glass->initPieceStates, glass->initPieceCount);
			read.close();

			return asset;
		}

		void load_depending(zone_base* zone) override
		{
			const auto* glass = &this->asset_->glassSys;
			for (auto i = 0u; glass->defs && i < glass->defCount; i++)
			{
				const auto* def = &glass->defs[i];
				add_dependency(zone, ASSET_TYPE_PHYSPRESET, def->physPreset);
				add_dependency(zone, ASSET_TYPE_MATERIAL, def->material);
				add_dependency(zone, ASSET_TYPE_MATERIAL, def->materialShattered);
				add_dependency(zone, ASSET_TYPE_FX, def->pieceBreakEffect);
				add_dependency(zone, ASSET_TYPE_FX, def->shatterEffect);
				add_dependency(zone, ASSET_TYPE_FX, def->shatterSmallEffect);
				add_dependency(zone, ASSET_TYPE_FX, def->crackDecalEffect);
				add_dependency(zone, ASSET_TYPE_SOUND, def->damagedSound);
				add_dependency(zone, ASSET_TYPE_SOUND, def->destroyedSound);
				add_dependency(zone, ASSET_TYPE_SOUND, def->destroyedQuietSound);
			}
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);

			auto* glass = &data->glassSys;
			auto* dest_glass = &dest->glassSys;
			if (const auto dest_defs = write_array(buf, 7, glass->defs, glass->defCount, dest_glass->defs))
			{
				for (auto i = 0u; i < glass->defCount; i++)
				{
					auto* def = &glass->defs[i];
					auto* dest_def = &dest_defs[i];
					dest_def->physPreset = asset_reference(zone, ASSET_TYPE_PHYSPRESET, def->physPreset);
					dest_def->material = asset_reference(zone, ASSET_TYPE_MATERIAL, def->material);
					dest_def->materialShattered = asset_reference(zone, ASSET_TYPE_MATERIAL, def->materialShattered);
					dest_def->pieceBreakEffect = asset_reference(zone, ASSET_TYPE_FX, def->pieceBreakEffect);
					dest_def->shatterEffect = asset_reference(zone, ASSET_TYPE_FX, def->shatterEffect);
					dest_def->shatterSmallEffect = asset_reference(zone, ASSET_TYPE_FX, def->shatterSmallEffect);
					dest_def->crackDecalEffect = asset_reference(zone, ASSET_TYPE_FX, def->crackDecalEffect);
					write_sound(buf, def->damagedSound, dest_def->damagedSound);
					write_sound(buf, def->destroyedSound, dest_def->destroyedSound);
					write_sound(buf, def->destroyedQuietSound, dest_def->destroyedQuietSound);
				}
			}

			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, glass->piecePlaces, glass->pieceLimit, dest_glass->piecePlaces);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, glass->pieceStates, glass->pieceLimit, dest_glass->pieceStates);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 7, glass->pieceDynamics, glass->pieceLimit, dest_glass->pieceDynamics);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 1, glass->geoData, glass->geoDataLimit, dest_glass->geoData);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, glass->isInUse, glass->pieceWordCount, dest_glass->isInUse);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, glass->cellBits, cell_bits_count(glass), dest_glass->cellBits);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 15, glass->visData, vis_data_count(glass), dest_glass->visData);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 3, glass->linkOrg, glass->pieceLimit, dest_glass->linkOrg);
			write_runtime_array(buf, XFILE_BLOCK_RUNTIME, 15, glass->halfThickness, half_thickness_count(glass), dest_glass->halfThickness);

			write_array(buf, 1, glass->lightingHandles, glass->initPieceCount, dest_glass->lightingHandles);
			write_array(buf, 1, glass->initGeoData, glass->initGeoDataCount, dest_glass->initGeoData);
			write_array(buf, 3, glass->initPieceStates, glass->initPieceCount, dest_glass->initPieceStates);

			buf->pop_stream();
		}

		static void dump(FxWorld* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_FXWORLD, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);

			auto* glass = &asset->glassSys;
			write.dump_array(glass->defs, glass->defCount);
			for (auto i = 0u; glass->defs && i < glass->defCount; i++)
			{
				auto* def = &glass->defs[i];
				write.dump_asset(def->physPreset);
				write.dump_asset(def->material);
				write.dump_asset(def->materialShattered);
				write.dump_asset(def->pieceBreakEffect);
				write.dump_asset(def->shatterEffect);
				write.dump_asset(def->shatterSmallEffect);
				write.dump_asset(def->crackDecalEffect);
				write.dump_asset(def->damagedSound);
				write.dump_asset(def->destroyedSound);
				write.dump_asset(def->destroyedQuietSound);
			}

			write.dump_array(glass->lightingHandles, glass->initPieceCount);
			write.dump_array(glass->initGeoData, glass->initGeoDataCount);
			write.dump_array(glass->initPieceStates, glass->initPieceCount);
			write.close();
		}

	private:
		static unsigned int cell_bits_count(const FxGlassSystem* glass)
		{
			return glass->pieceWordCount * glass->cellCount;
		}

		static unsigned int vis_data_count(const FxGlassSystem* glass)
		{
			return (glass->pieceLimit + 15) & ~15u;
		}

		static unsigned int half_thickness_count(const FxGlassSystem* glass)
		{
			return (glass->pieceLimit + 3) & ~3u;
		}
	};
}
