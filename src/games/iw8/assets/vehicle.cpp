#include <std_include.hpp>
#include "vehicle.hpp"

namespace zonetool::iw8
{
	namespace
	{
		template <typename Walk>
		void walk_physics(Walk walk, VehiclePhysicsDef& physics)
		{
			walk.inline_script_strings(physics.physics_wheelBones, std::size(physics.physics_wheelBones));
			walk.string(physics.physics_rcpBoostSound.name);
			walk.inline_script_strings(physics.treads_firstBones, std::size(physics.treads_firstBones));

			for (auto group : {&physics.sndImpactHard, &physics.sndImpactLight, &physics.sndDrivingFast, &physics.sndDrivingSlow,
				&physics.sndSuspension, &physics.sndBrakingHard, &physics.sndBrakingLight, &physics.sndDrifting, &physics.sndSkidding,
				&physics.sndDlc1, &physics.sndDlc2, &physics.sndDlc3, &physics.sndDlc4})
			{
				walk.string(group->alias.name);
			}

			for (auto group : {&physics.vfxImpactHard, &physics.vfxImpactLight, &physics.vfxDrivingFast, &physics.vfxDrivingSlow,
				&physics.vfxBrakingHard, &physics.vfxBrakingLight, &physics.vfxDrifting, &physics.vfxSkidding,
				&physics.vfxDrivingFlatFast, &physics.vfxDrivingFlatSlow, &physics.vfxDlc3, &physics.vfxDlc4})
			{
				walk.asset(ASSET_TYPE_FOOTSTEP_VFX, group->surfaceEffects);
			}
		}

		template <typename Walk>
		void walk_vehicle(Walk walk, VehicleDef& asset)
		{
			walk.string(asset.useHintString);
			walk.inline_script_strings(asset.doorBones, std::size(asset.doorBones));
			walk.asset(ASSET_TYPE_CAMERA, asset.cameraDef);
			walk.asset(ASSET_TYPE_CAMERA, asset.killCamDef);
			walk_physics(walk, asset.vehiclePhysicsDef);
			walk.asset(ASSET_TYPE_VFX, asset.vehHelicopterGroundFx.particleSystemDef);
			walk.asset(ASSET_TYPE_VFX, asset.vehHelicopterGroundWaterFx.particleSystemDef);
			walk.script_string(asset.ssAnimTree);

			for (auto& parts : asset.ssAnimParts)
			{
				for (auto& anim : parts.anim)
				{
					walk.asset(ASSET_TYPE_XANIMPARTS, anim);
				}
			}

			walk.asset(ASSET_TYPE_VFX, asset.ssThrustFxLoop.particleSystemDef);
			walk.asset(ASSET_TYPE_VFX, asset.ssJukeFx.particleSystemDef);
			walk.asset(ASSET_TYPE_RUMBLE, asset.ssIdleRumble);
			walk.asset(ASSET_TYPE_RUMBLE, asset.ssSmallRumble);
			walk.asset(ASSET_TYPE_RUMBLE, asset.ssMedRumble);
			walk.asset(ASSET_TYPE_RUMBLE, asset.ssLargeRumble);
			walk.string(asset.rattleLoop.name);
			walk.string(asset.airLoop.name);
			walk.string(asset.engineLoop.name);
			walk.string(asset.hoverLoop.name);
			walk.string(asset.boostLoop.name);
			walk.string(asset.ssThrustLoop.name);
			walk.string(asset.boostStart.name);
			walk.string(asset.boostStop.name);
			walk.string(asset.boostDepleted.name);
			walk.string(asset.boostUnavailable.name);
			walk.string(asset.jukeLeft.name);
			walk.string(asset.jukeRight.name);
			walk.string(asset.jukeUpDown.name);
			walk.string(asset.jukeBack.name);
			walk.string(asset.jukeFront.name);
			walk.string(asset.flightOn.name);
			walk.string(asset.flightOff.name);
			walk.string(asset.hardCollision.name);
			walk.string(asset.softCollision.name);
			walk.string(asset.turretWeaponName);
			walk.asset(ASSET_TYPE_WEAPON, asset.turretWeapon);
			walk.string(asset.turretSpinSnd.name);
			walk.string(asset.turretStopSnd.name);
			walk.inline_script_strings(asset.trophyTags, std::size(asset.trophyTags));
			walk.asset(ASSET_TYPE_VFX, asset.trophyExplodeFx.particleSystemDef);
			walk.asset(ASSET_TYPE_VFX, asset.trophyFlashFx.particleSystemDef);
			walk.asset(ASSET_TYPE_MATERIAL, asset.compassFriendlyIcon);
			walk.asset(ASSET_TYPE_MATERIAL, asset.compassEnemyIcon);
			walk.asset(ASSET_TYPE_MATERIAL, asset.compassFriendlyAltIcon);
			walk.asset(ASSET_TYPE_MATERIAL, asset.compassEnemyAltIcon);
			walk.asset(ASSET_TYPE_SOUND_BANK_TRANSIENT, asset.detailSoundBankNPC);
			walk.asset(ASSET_TYPE_SOUND_BANK_TRANSIENT, asset.detailSoundBankPlayer);
			walk.string(asset.idleLowSnd.name);
			walk.string(asset.idleHighSnd.name);
			walk.string(asset.engineLowSnd.name);
			walk.string(asset.engineHighSnd.name);
			walk.string(asset.fallbackIdleLowSnd.name);
			walk.string(asset.fallbackIdleHighSnd.name);
			walk.string(asset.fallbackEngineLowSnd.name);
			walk.string(asset.fallbackEngineHighSnd.name);
			walk.script_string(asset.audioOriginTag);
			walk.string(asset.mainRotorLowRpmSnd.name);
			walk.string(asset.mainRotorOperatingRpmSnd.name);
			walk.string(asset.mainRotorOperatingEffortSnd.name);
			walk.string(asset.tailRotorLowRpmSnd.name);
			walk.string(asset.tailRotorOperatingRpmSnd.name);
			walk.string(asset.tailRotorOperatingEffortSndLeft.name);
			walk.string(asset.tailRotorOperatingEffortSndRight.name);
			walk.string(asset.physicsHeliStartup.name);
			walk.string(asset.physicsHeliShutdown.name);
			walk.string(asset.turbineLowRpmSnd.name);
			walk.string(asset.turbineOperatingRpmSnd.name);
			walk.string(asset.revLowFidelityMod.name);
			walk.string(asset.revHighFidelityMod.name);
			walk.string(asset.engineStartUpSnd.name);
			walk.string(asset.engineShutdownSnd.name);
			walk.string(asset.engineIdleSnd.name);
			walk.string(asset.engineSustainSnd.name);
			walk.string(asset.engineRampUpSnd.name);
			walk.string(asset.engineRampDownSnd.name);
			walk.string(asset.suspensionSoftSnd.name);
			walk.string(asset.suspensionHardSnd.name);
			walk.string(asset.collisionSnd.name);
			walk.string(asset.speedSnd.name);
			walk.string(asset.surfaceSndName);
			walk.string(asset.soundTriggerOverrideZone);
			walk.string(asset.globalVisionSettings);
			walk.string(asset.mapVisionSettings);
			walk.string(asset.luiCrosshairWidget);
		}
	}

	void vehicle::read(zone_reader& reader, VehicleDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_vehicle(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void vehicle::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_vehicle(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void vehicle::dump(VehicleDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("vehicles\\"s + asset->name + ".vehicleb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_vehicle(dump_walk{dump}, *asset);
		dump.close();
	}

	VehicleDef* vehicle::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("vehicles\\"s + name + ".vehicleb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing vehicle \"%s\"...", name.data());

		auto* asset = read.read_single<VehicleDef>();
		asset->name = read.read_string();
		walk_vehicle(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void vehicle::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<VehicleDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void vehicle::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_vehicle(depend_walk{zone}, *this->asset_);
	}

	void vehicle::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_vehicle(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string vehicle::name()
	{
		return this->name_;
	}

	std::int32_t vehicle::type()
	{
		return ASSET_TYPE_VEHICLE;
	}
}
