#pragma once

namespace zonetool::s2
{
	class vehicle_track : public asset_base<VehicleTrack, ASSET_TYPE_VEHICLE_TRACK>
	{
	public:
		static void read(zone_reader& reader, VehicleTrack* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);

			if (const auto segments = reader.read_array(asset->segments, 7, asset->segmentCount))
			{
				for (auto i = 0u; i < asset->segmentCount; i++)
				{
					read_segment(reader, &segments[i]);
				}
			}

			reader.pop_stream();
		}

		VehicleTrack* parse(const std::string&, zone_memory*) override
		{
			return nullptr;
		}

	private:
		static void read_segment(zone_reader& reader, VehicleTrackSegment* segment)
		{
			reader.read_string(segment->targetName);

			if (const auto sectors = reader.read_array(segment->sectors, 7, segment->sectorCount))
			{
				for (auto i = 0u; i < segment->sectorCount; i++)
				{
					reader.read_array(sectors[i].obstacles, 3, sectors[i].obstacleCount);
				}
			}

			read_branches(reader, segment->nextBranches, segment->nextBranchesCount);
			read_branches(reader, segment->prevBranches, segment->prevBranchesCount);
		}

		static void read_branches(zone_reader& reader, VehicleTrackSegment**& branches, const unsigned int count)
		{
			if (const auto entries = reader.read_array(branches, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					if (const auto segment = reader.read_single(entries[i], 7))
					{
						read_segment(reader, segment);
					}
				}
			}
		}
	};

	class vehicle_def : public asset_base<VehicleDef, ASSET_TYPE_VEHICLE>
	{
		using effect_field = FxEffectDef* VehicleDef::*;
		using sound_field = snd_alias_list_t* VehicleDef::*;

		// each list is contiguous in zone and dump order
		static constexpr effect_field vehicle_effects[] =
		{
			&VehicleDef::treadDefaultFx,
			&VehicleDef::handBrakeDefaultFx,
			&VehicleDef::handBrakeLeftFx,
			&VehicleDef::handBrakeRightFx,
			&VehicleDef::boostFx,
			&VehicleDef::vehHelicopterGroundFx,
			&VehicleDef::vehHelicopterGroundWaterFx,
		};

		static constexpr sound_field turret_sounds[] =
		{
			&VehicleDef::turretSpinSnd,
			&VehicleDef::turretStopSnd,
		};

		static constexpr effect_field trophy_effects[] =
		{
			&VehicleDef::trophyExplodeFx,
			&VehicleDef::trophyFlashFx,
		};

		static constexpr sound_field engine_sounds[] =
		{
			&VehicleDef::idleLowSnd,
			&VehicleDef::idleHighSnd,
			&VehicleDef::engineLowSnd,
			&VehicleDef::engineHighSnd,
			&VehicleDef::boostSnd,
		};

		static constexpr sound_field engine_alt_sounds[] =
		{
			&VehicleDef::idleLowSndAlt,
			&VehicleDef::idleHighSndAlt,
			&VehicleDef::engineLowSndAlt,
			&VehicleDef::engineHighSndAlt,
		};

		static constexpr sound_field event_sounds[] =
		{
			&VehicleDef::turretSpinSndAlt,
			&VehicleDef::turretStopSndAlt,
			&VehicleDef::engineStartUpSnd,
			&VehicleDef::engineShutdownSnd,
			&VehicleDef::engineIdleSnd,
			&VehicleDef::engineSustainSnd,
			&VehicleDef::engineRampUpSnd,
			&VehicleDef::engineRampDownSnd,
			&VehicleDef::suspensionSoftSnd,
			&VehicleDef::suspensionHardSnd,
			&VehicleDef::collisionSnd,
			&VehicleDef::speedSnd,
		};

	public:
		static void read(zone_reader& reader, VehicleDef* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);

			reader.read_string(asset->name);
			reader.read_string(asset->useHintString);
			reader.read_string(asset->buttonsConfig);
			for (auto& id : asset->wheelTags)
			{
				reader.read_script_string(id);
			}

			auto& phys = asset->vehPhysDef;
			reader.read_asset(ASSET_TYPE_PHYSPRESET, phys.physPreset);
			reader.read_string(phys.physMassName);
			reader.read_array(phys.physMass, 3, 1);
			reader.read_string(phys.accelGraphName);

			read_assets(reader, ASSET_TYPE_FX, asset, vehicle_effects);
			reader.read_string(asset->vehDiveboatSteeringGraphName);
			reader.read_string(asset->turretWeaponName);
			reader.read_asset(ASSET_TYPE_WEAPON, asset->turretWeapon);
			read_sounds(reader, asset, turret_sounds);
			for (auto& id : asset->trophyTags)
			{
				reader.read_script_string(id);
			}
			read_assets(reader, ASSET_TYPE_FX, asset, trophy_effects);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassFriendlyIcon);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassEnemyIcon);
			read_assets(reader, ASSET_TYPE_XANIM, asset->driveAnims);
			read_sounds(reader, asset, engine_sounds);
			reader.read_script_string(asset->audioOriginTag);
			read_sounds(reader, asset, engine_alt_sounds);
			reader.read_script_string(asset->audioOriginTagAlt);
			read_sounds(reader, asset, event_sounds);
			reader.read_string(asset->surfaceSndPrefix);
			read_sounds(reader, asset->surfaceSnds);
			reader.read_string(asset->soundTriggerOverrideZone);

			reader.pop_stream();
		}

		VehicleDef* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_VEHICLE, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing vehicle \"%s\"...", name.data());

			auto* asset = read.read_single<VehicleDef>();
			asset->name = read.read_string();
			asset->useHintString = read.read_string();
			asset->buttonsConfig = read.read_string();
			for (auto& id : asset->wheelTags)
			{
				id = parse_script_string(read);
			}

			auto& phys = asset->vehPhysDef;
			phys.physPreset = read.read_asset<PhysPreset>();
			phys.physMassName = read.read_string();
			parse_array(read, mem, phys.physMass, 1);
			phys.accelGraphName = read.read_string();

			parse_assets(read, asset, vehicle_effects);
			asset->vehDiveboatSteeringGraphName = read.read_string();
			asset->turretWeaponName = read.read_string();
			asset->turretWeapon = nullptr;
			if (const auto* weapon_name = read.read_string())
			{
				asset->turretWeapon = mem->allocate<WeaponCompleteDef>();
				asset->turretWeapon->szInternalName = weapon_name;
			}
			parse_assets(read, asset, turret_sounds);
			for (auto& id : asset->trophyTags)
			{
				id = parse_script_string(read);
			}
			parse_assets(read, asset, trophy_effects);
			asset->compassFriendlyIcon = read.read_asset<Material>();
			asset->compassEnemyIcon = read.read_asset<Material>();
			parse_assets(read, asset->driveAnims);
			parse_assets(read, asset, engine_sounds);
			asset->audioOriginTag = parse_script_string(read);
			parse_assets(read, asset, engine_alt_sounds);
			asset->audioOriginTagAlt = parse_script_string(read);
			parse_assets(read, asset, event_sounds);
			asset->surfaceSndPrefix = read.read_string();
			parse_assets(read, asset->surfaceSnds);
			asset->soundTriggerOverrideZone = read.read_string();

			read.close();

			return asset;
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
			auto* data = this->asset_;
			script_string_preparer prepare(buf);
			prepare(data->wheelTags, std::size(data->wheelTags));
			prepare(data->trophyTags, std::size(data->trophyTags));
			prepare(data->audioOriginTag);
			prepare(data->audioOriginTagAlt);
		}

		void load_depending(zone_base* zone) override
		{
			auto* data = this->asset_;
			add_dependency(zone, ASSET_TYPE_PHYSPRESET, data->vehPhysDef.physPreset);
			add_field_dependencies(zone, ASSET_TYPE_FX, data, vehicle_effects);
			add_dependency(zone, ASSET_TYPE_WEAPON, data->turretWeapon);
			add_field_dependencies(zone, ASSET_TYPE_SOUND, data, turret_sounds);
			add_field_dependencies(zone, ASSET_TYPE_FX, data, trophy_effects);
			add_dependency(zone, ASSET_TYPE_MATERIAL, data->compassFriendlyIcon);
			add_dependency(zone, ASSET_TYPE_MATERIAL, data->compassEnemyIcon);
			add_dependencies(zone, ASSET_TYPE_XANIM, data->driveAnims, std::size(data->driveAnims));
			add_field_dependencies(zone, ASSET_TYPE_SOUND, data, engine_sounds);
			add_field_dependencies(zone, ASSET_TYPE_SOUND, data, engine_alt_sounds);
			add_field_dependencies(zone, ASSET_TYPE_SOUND, data, event_sounds);
			add_dependencies(zone, ASSET_TYPE_SOUND, data->surfaceSnds, std::size(data->surfaceSnds));
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			write_string(buf, data->useHintString, dest->useHintString);
			write_string(buf, data->buttonsConfig, dest->buttonsConfig);

			const auto& phys = data->vehPhysDef;
			auto& dest_phys = dest->vehPhysDef;
			dest_phys.physPreset = asset_reference(zone, ASSET_TYPE_PHYSPRESET, phys.physPreset);
			write_string(buf, phys.physMassName, dest_phys.physMassName);
			write_array(buf, 3, phys.physMass, 1, dest_phys.physMass);
			write_string(buf, phys.accelGraphName, dest_phys.accelGraphName);

			write_assets(zone, ASSET_TYPE_FX, data, dest, vehicle_effects);
			write_string(buf, data->vehDiveboatSteeringGraphName, dest->vehDiveboatSteeringGraphName);
			write_string(buf, data->turretWeaponName, dest->turretWeaponName);
			dest->turretWeapon = asset_reference(zone, ASSET_TYPE_WEAPON, data->turretWeapon);
			write_sounds(buf, data, dest, turret_sounds);
			write_assets(zone, ASSET_TYPE_FX, data, dest, trophy_effects);
			dest->compassFriendlyIcon = asset_reference(zone, ASSET_TYPE_MATERIAL, data->compassFriendlyIcon);
			dest->compassEnemyIcon = asset_reference(zone, ASSET_TYPE_MATERIAL, data->compassEnemyIcon);
			write_assets(zone, ASSET_TYPE_XANIM, data->driveAnims, dest->driveAnims);
			write_sounds(buf, data, dest, engine_sounds);
			write_sounds(buf, data, dest, engine_alt_sounds);
			write_sounds(buf, data, dest, event_sounds);
			write_string(buf, data->surfaceSndPrefix, dest->surfaceSndPrefix);
			write_sounds(buf, data->surfaceSnds, dest->surfaceSnds);
			write_string(buf, data->soundTriggerOverrideZone, dest->soundTriggerOverrideZone);

			buf->pop_stream();
		}

		static void dump(VehicleDef* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_VEHICLE, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_string(asset->useHintString);
			write.dump_string(asset->buttonsConfig);
			dump_script_strings(write, asset->wheelTags, std::size(asset->wheelTags));

			const auto& phys = asset->vehPhysDef;
			write.dump_asset(phys.physPreset);
			write.dump_string(phys.physMassName);
			write.dump_array(phys.physMass, 1);
			write.dump_string(phys.accelGraphName);

			dump_assets(write, asset, vehicle_effects);
			write.dump_string(asset->vehDiveboatSteeringGraphName);
			write.dump_string(asset->turretWeaponName);
			write.dump_string(asset->turretWeapon ? asset->turretWeapon->szInternalName : nullptr);
			dump_assets(write, asset, turret_sounds);
			dump_script_strings(write, asset->trophyTags, std::size(asset->trophyTags));
			dump_assets(write, asset, trophy_effects);
			write.dump_asset(asset->compassFriendlyIcon);
			write.dump_asset(asset->compassEnemyIcon);
			dump_assets(write, asset->driveAnims);
			dump_assets(write, asset, engine_sounds);
			dump_script_string(write, asset->audioOriginTag);
			dump_assets(write, asset, engine_alt_sounds);
			dump_script_string(write, asset->audioOriginTagAlt);
			dump_assets(write, asset, event_sounds);
			write.dump_string(asset->surfaceSndPrefix);
			dump_assets(write, asset->surfaceSnds);
			write.dump_string(asset->soundTriggerOverrideZone);

			write.close();
		}

	private:
		template <typename T, std::size_t N>
		static void read_assets(zone_reader& reader, const XAssetType type, T* (&assets)[N])
		{
			for (auto& asset : assets)
			{
				reader.read_asset(type, asset);
			}
		}

		template <typename T, std::size_t N>
		static void read_assets(zone_reader& reader, const XAssetType type, VehicleDef* asset, T* VehicleDef::* const (&fields)[N])
		{
			for (const auto field : fields)
			{
				reader.read_asset(type, asset->*field);
			}
		}

		template <std::size_t N>
		static void read_sounds(zone_reader& reader, snd_alias_list_t* (&sounds)[N])
		{
			for (auto& sound : sounds)
			{
				reader.read_name_reference(sound);
			}
		}

		template <std::size_t N>
		static void read_sounds(zone_reader& reader, VehicleDef* asset, const sound_field (&fields)[N])
		{
			for (const auto field : fields)
			{
				reader.read_name_reference(asset->*field);
			}
		}

		template <typename T, std::size_t N>
		static void parse_assets(assetmanager::reader& read, T* (&assets)[N])
		{
			for (auto& asset : assets)
			{
				asset = read.read_asset<T>();
			}
		}

		template <typename T, std::size_t N>
		static void parse_assets(assetmanager::reader& read, VehicleDef* asset, T* VehicleDef::* const (&fields)[N])
		{
			for (const auto field : fields)
			{
				asset->*field = read.read_asset<T>();
			}
		}

		template <typename T, std::size_t N>
		static void add_field_dependencies(zone_base* zone, const XAssetType type, const VehicleDef* asset, T* VehicleDef::* const (&fields)[N])
		{
			for (const auto field : fields)
			{
				add_dependency(zone, type, asset->*field);
			}
		}

		template <typename T, std::size_t N>
		static void write_assets(zone_base* zone, const XAssetType type, T* const (&assets)[N], T* (&dest)[N])
		{
			for (auto i = 0u; i < N; i++)
			{
				dest[i] = asset_reference(zone, type, assets[i]);
			}
		}

		template <typename T, std::size_t N>
		static void write_assets(zone_base* zone, const XAssetType type, const VehicleDef* data, VehicleDef* dest, T* VehicleDef::* const (&fields)[N])
		{
			for (const auto field : fields)
			{
				dest->*field = asset_reference(zone, type, data->*field);
			}
		}

		template <std::size_t N>
		static void write_sounds(zone_buffer* buf, snd_alias_list_t* const (&sounds)[N], snd_alias_list_t* (&dest)[N])
		{
			for (auto i = 0u; i < N; i++)
			{
				write_sound(buf, sounds[i], dest[i]);
			}
		}

		template <std::size_t N>
		static void write_sounds(zone_buffer* buf, const VehicleDef* data, VehicleDef* dest, const sound_field (&fields)[N])
		{
			for (const auto field : fields)
			{
				write_sound(buf, data->*field, dest->*field);
			}
		}

		template <typename T, std::size_t N>
		static void dump_assets(assetmanager::dumper& write, T* const (&assets)[N])
		{
			for (auto* asset : assets)
			{
				write.dump_asset(asset);
			}
		}

		template <typename T, std::size_t N>
		static void dump_assets(assetmanager::dumper& write, const VehicleDef* asset, T* VehicleDef::* const (&fields)[N])
		{
			for (const auto field : fields)
			{
				write.dump_asset(asset->*field);
			}
		}
	};
}
