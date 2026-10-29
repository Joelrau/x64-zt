#include "std_include.hpp"
#include "vehicledef.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::h1
{

	namespace
	{
		constexpr const char* vehicle_fire_type_names[VEH_TURRET_FIRE_TYPE_COUNT] =
		{
			"singleFire",
			"dualFire",
			"altFire",
		};

		constexpr const char* vehicle_axle_type_names[VEH_AXLE_COUNT] =
		{
			"front",
			"rear",
			"all",
		};

		constexpr const char* vehicle_type_names[VEH_TYPE_COUNT] =
		{
			"wheels4",
			"tank",
			"plane",
			"boat",
			"artillery",
			"helicopter",
			"snowmobile",
			"submarine",
			"ugv",
			"walker",
			"ball_wheels",
			"mobilecover",
			"jetbike",
			"recon_ugv",
			"hovertank",
			"divehoat",
		};

		constexpr auto phys_mass_fields = glz::tuple
		{
			&PhysMass::centerOfMass,
			&PhysMass::momentsOfInertia,
			&PhysMass::productsOfInertia,
		};

		constexpr auto veh_phys_def_fields = glz::tuple
		{
			&VehiclePhysDef::physicsEnabled,
			&VehiclePhysDef::physPresetName,
			&VehiclePhysDef::physPreset,
			&VehiclePhysDef::physMassName,
			json_reflect::nested("PhysMass", &VehiclePhysDef::physMass, phys_mass_fields),
			&VehiclePhysDef::accelGraphName,
			&VehiclePhysDef::steeringAxle,
			&VehiclePhysDef::powerAxle,
			&VehiclePhysDef::brakingAxle,
			&VehiclePhysDef::topSpeed,
			&VehiclePhysDef::topSpeedTurbo,
			&VehiclePhysDef::reverseSpeed,
			&VehiclePhysDef::maxVelocity,
			&VehiclePhysDef::maxPitch,
			&VehiclePhysDef::maxRoll,
			&VehiclePhysDef::wheelRadius,
			&VehiclePhysDef::suspensionTravelFront,
			&VehiclePhysDef::suspensionTravelRear,
			&VehiclePhysDef::suspensionStrengthFront,
			&VehiclePhysDef::suspensionDampingFront,
			&VehiclePhysDef::suspensionStrengthRear,
			&VehiclePhysDef::suspensionDampingRear,
			&VehiclePhysDef::frictionBraking,
			&VehiclePhysDef::frictionCoasting,
			&VehiclePhysDef::frictionTopSpeed,
			&VehiclePhysDef::frictionSide,
			&VehiclePhysDef::frictionSideRear,
			&VehiclePhysDef::handBrakeLongitudinalSteerableFrictionScale,
			&VehiclePhysDef::handBrakeLateralSteerableFrictionScale,
			&VehiclePhysDef::handBrakeLongitudinalNonsteerableFrictionScale,
			&VehiclePhysDef::handBrakeLateralNonsteerableFrictionScale,
			&VehiclePhysDef::handBrakingStrength,
			&VehiclePhysDef::handBrakeExtraYawTorque,
			&VehiclePhysDef::speedAtMaxHandBrakeExtraYawTorque,
			&VehiclePhysDef::driveForceFalloffFraction,
			&VehiclePhysDef::velocityDependentSlip,
			&VehiclePhysDef::rollStability,
			&VehiclePhysDef::rollResistance,
			&VehiclePhysDef::pitchResistance,
			&VehiclePhysDef::yawResistance,
			&VehiclePhysDef::uprightStrengthPitch,
			&VehiclePhysDef::uprightStrengthRoll,
			&VehiclePhysDef::targetAirPitch,
			&VehiclePhysDef::airYawTorque,
			&VehiclePhysDef::airPitchTorque,
			&VehiclePhysDef::minimumMomentumForCollision,
			&VehiclePhysDef::collisionLaunchForceScale,
			&VehiclePhysDef::wreckedMassScale,
			&VehiclePhysDef::wreckedBodyFriction,
			&VehiclePhysDef::minimumJoltForNotify,
			&VehiclePhysDef::slipThresholdFront,
			&VehiclePhysDef::slipThresholdRear,
			&VehiclePhysDef::slipFricScaleFront,
			&VehiclePhysDef::slipFricScaleRear,
			&VehiclePhysDef::slipFricRateFront,
			&VehiclePhysDef::slipFricRateRear,
			&VehiclePhysDef::slipYawTorque,
			&VehiclePhysDef::cruiseControlProportionalGain,
			&VehiclePhysDef::cruiseControlIntegralGain,
		};

		constexpr auto vehicle_fields = glz::tuple
		{
			&VehicleDef::name,
			&VehicleDef::type,
			&VehicleDef::useHintString,
			&VehicleDef::health,
			&VehicleDef::quadBarrel,
			&VehicleDef::hitClientScriptables,
			&VehicleDef::multipleLinkedGroundEntities,
			&VehicleDef::hideVehicleForDriver,
			&VehicleDef::texScrollScale,
			&VehicleDef::topSpeed,
			&VehicleDef::accel,
			&VehicleDef::rotRate,
			&VehicleDef::rotAccel,
			&VehicleDef::maxBodyPitch,
			&VehicleDef::maxBodyRoll,
			&VehicleDef::legIK,
			&VehicleDef::fakeBodyAccelPitch,
			&VehicleDef::fakeBodyAccelRoll,
			&VehicleDef::fakeBodyVelPitch,
			&VehicleDef::fakeBodyVelRoll,
			&VehicleDef::fakeBodySideVelPitch,
			&VehicleDef::fakeBodyPitchStrength,
			&VehicleDef::fakeBodyRollStrength,
			&VehicleDef::fakeBodyPitchDampening,
			&VehicleDef::fakeBodyRollDampening,
			&VehicleDef::fakeBodyBoatRockingAmplitude,
			&VehicleDef::fakeBodyBoatRockingPeriod,
			&VehicleDef::fakeBodyBoatRockingRotationPeriod,
			&VehicleDef::fakeBodyBoatRockingFadeoutSpeed,
			&VehicleDef::boatBouncingMinForce,
			&VehicleDef::boatBouncingMaxForce,
			&VehicleDef::boatBouncingRate,
			&VehicleDef::boatBouncingFadeinSpeed,
			&VehicleDef::boatBouncingFadeoutSteeringAngle,
			&VehicleDef::collisionDamage,
			&VehicleDef::collisionSpeed,
			&VehicleDef::killcamOffset,
			&VehicleDef::playerProtected,
			&VehicleDef::bulletDamage,
			&VehicleDef::armorPiercingDamage,
			&VehicleDef::grenadeDamage,
			&VehicleDef::projectileDamage,
			&VehicleDef::projectileSplashDamage,
			&VehicleDef::heavyExplosiveDamage,
			json_reflect::nested(&VehicleDef::vehPhysDef, veh_phys_def_fields),
			&VehicleDef::boostDuration,
			&VehicleDef::boostRechargeTime,
			&VehicleDef::boostAcceleration,
			&VehicleDef::boostTopSpeed,
			&VehicleDef::suspensionTravel,
			&VehicleDef::maxSteeringAngle,
			&VehicleDef::steeringLerp,
			&VehicleDef::steeringLerpCentering,
			&VehicleDef::minSteeringScale,
			&VehicleDef::minSteeringSpeed,
			&VehicleDef::disableWheelsTurning,
			&VehicleDef::treadDefaultFx,
			&VehicleDef::handBrakeDefaultFx,
			&VehicleDef::handBrakeLeftFx,
			&VehicleDef::handBrakeRightFx,
			&VehicleDef::boostFx,
			&VehicleDef::treadFxSlowestRepeatRate,
			&VehicleDef::treadFxFastestRepeatRate,
			&VehicleDef::treadFxMinSpeed,
			&VehicleDef::treadFxMaxSpeed,
			&VehicleDef::vehHelicopterIsASplinePlane,
			&VehicleDef::vehHelicopterOrbitsAroundPoint,
			&VehicleDef::vehHelicopterLockAltitude,
			&VehicleDef::vehHelicopterOffsetFromMesh,
			&VehicleDef::vehHelicopterAltitudeOffset,
			&VehicleDef::vehHelicopterPitchOffset,
			&VehicleDef::vehHelicopterBoundsRadius,
			&VehicleDef::vehHelicopterBoundsOffsetZ,
			&VehicleDef::vehHelicopterMaxSpeed,
			&VehicleDef::vehHelicopterMaxSpeedVertical,
			&VehicleDef::vehHelicopterMaxAccel,
			&VehicleDef::vehHelicopterMaxAccelVertical,
			&VehicleDef::vehHelicopterDecelerationFwd,
			&VehicleDef::vehHelicopterDecelerationSide,
			&VehicleDef::vehHelicopterDecelerationUp,
			&VehicleDef::vehHelicopterMaxYawRate,
			&VehicleDef::vehHelicopterMaxYawAccel,
			&VehicleDef::vehHelicopterTiltFromVelocity,
			&VehicleDef::vehHelicopterTiltFromControllerAxes,
			&VehicleDef::vehHelicopterTiltFromAcceleration,
			&VehicleDef::vehHelicopterTiltFromDeceleration,
			&VehicleDef::vehHelicopterTiltFromFwdAndYaw_VelAtMaxTilt,
			&VehicleDef::vehHelicopterTiltFromFwdAndYaw,
			&VehicleDef::vehHelicopterTiltMomentum,
			&VehicleDef::vehHelicopterTiltSpeed,
			&VehicleDef::vehHelicopterMaxPitch,
			&VehicleDef::vehHelicopterMaxRoll,
			&VehicleDef::vehHelicopterHoverSpeedThreshold,
			&VehicleDef::vehHelicopterJitterJerkyness,
			&VehicleDef::vehHelicopterUseHoverWobble,
			&VehicleDef::vehHelicopterHoverWobblePhase,
			&VehicleDef::vehHelicopterHoverWobbleAmplitude,
			&VehicleDef::vehHelicopterUseBob,
			&VehicleDef::vehHelicopterBobPhase,
			&VehicleDef::vehHelicopterBobAmplitude,
			&VehicleDef::vehHelicopterLookaheadTime,
			&VehicleDef::vehHelicopterSoftCollisions,
			&VehicleDef::vehHelicopterUseGroundFX,
			&VehicleDef::vehHelicopterGroundFx,
			&VehicleDef::vehHelicopterGroundWaterFx,
			&VehicleDef::vehHelicopterGroundFxDefaultRepeatRate,
			&VehicleDef::vehHelicopterGroundFxSlowestRepeatRate,
			&VehicleDef::vehHelicopterGroundFxFastestRepeatRate,
			&VehicleDef::vehHelicopterGroundFxMinGroundDist,
			&VehicleDef::vehHelicopterGroundFxMaxGroundDist,
			&VehicleDef::vehSplinePlaneCorridorMaxXVel,
			&VehicleDef::vehSplinePlaneCorridorMaxZVel,
			&VehicleDef::vehSplinePlaneCorridorMaxXAccel,
			&VehicleDef::vehSplinePlaneCorridorMaxZAccel,
			&VehicleDef::vehSplinePlaneTangentLookAtRate,
			&VehicleDef::vehSplinePlaneMaxPitchSpeed,
			&VehicleDef::vehSplinePlaneMaxYawSpeed,
			&VehicleDef::vehSplinePlaneMaxRollSpeed,
			&VehicleDef::vehSplinePlanePitchSpeedRate,
			&VehicleDef::vehSplinePlaneYawSpeedRate,
			&VehicleDef::vehSplinePlaneRollSpeedRateUp,
			&VehicleDef::vehSplinePlaneRollSpeedRateDown,
			&VehicleDef::vehSplinePlaneMaxPitchAccel,
			&VehicleDef::vehSplinePlaneMaxYawAccel,
			&VehicleDef::vehSplinePlaneMaxRollAccel,
			&VehicleDef::vehSplinePlaneYawToRollFactor,
			&VehicleDef::vehSplinePlaneRollToYawFactor,
			&VehicleDef::vehSplinePlaneRollToYawFactorBlend,
			&VehicleDef::vehSplinePlaneMaxPitch,
			&VehicleDef::vehSplinePlaneMaxRoll,
			&VehicleDef::vehSplinePlaneMaxPitchFromRoll,
			&VehicleDef::vehSplinePlaneMaxRollYawOffset,
			&VehicleDef::vehSplinePlaneRollYawOffsetThreshold,
			&VehicleDef::vehSplinePlaneMaxTiltRoll,
			&VehicleDef::vehSplinePlaneMaxTiltPitch,
			&VehicleDef::vehSplinePlaneTiltRollRate,
			&VehicleDef::vehSplinePlaneTiltPitchRate,
			&VehicleDef::vehJetbikeThrottleForce,
			&VehicleDef::vehJetbikeStrafeForce,
			&VehicleDef::vehJetbikeYawTorque,
			&VehicleDef::vehJetbikePitchTorque,
			&VehicleDef::vehJetbikeYawDamping,
			&VehicleDef::vehJetbikePitchDamping,
			&VehicleDef::vehJetbikeRollDamping,
			&VehicleDef::vehJetbikeRepulsorMaxForceFraction,
			&VehicleDef::vehJetbikeRepulsorMinForceFraction,
			&VehicleDef::vehJetbikeRepulsorCompressionDampingConstant,
			&VehicleDef::vehJetbikeRepulsorReboundDampingConstant,
			&VehicleDef::vehJetbikeRepulsorTorqueScale,
			&VehicleDef::vehJetbikeRepulsorCrossCoupling,
			&VehicleDef::vehJetbikeAntislipConstant,
			&VehicleDef::vehJetbikeAntislipMaxForce,
			&VehicleDef::vehJetbikeControlForceLocalOffsetX,
			&VehicleDef::vehJetbikeControlForceLocalOffsetZ,
			&VehicleDef::vehJetbikeControlTorqueLocalOffsetX,
			&VehicleDef::vehJetbikeControlTorqueLocalOffsetZ,
			&VehicleDef::vehJetbikeMaxControlForce,
			&VehicleDef::vehJetbikeMinContactForFullControl,
			&VehicleDef::vehJetbikeThrustScaleWithNoContact,
			&VehicleDef::vehJetbikeTorqueScaleWithNoContact,
			&VehicleDef::vehJetbikeUprightingTorque,
			&VehicleDef::vehJetbikeUprightingTorqueWithNoContact,
			&VehicleDef::vehJetbikeWeathervaneTorque,
			&VehicleDef::vehJetbikeWeathervaneTorqueWithNoContact,
			&VehicleDef::vehJetbikeAiSteeringConstant,
			&VehicleDef::vehJetbikeAiStationarySteeringScale,
			&VehicleDef::vehJetbikeAiThrottleConstant,
			&VehicleDef::vehHovertankAutoYawForce,
			&VehicleDef::vehHovertankAutoBrakeForce,
			&VehicleDef::vehHovertankRandomHoverForceMagMin,
			&VehicleDef::vehHovertankRandomHoverForceMagMax,
			&VehicleDef::vehHovertankRandomHoverForceStartTimerMin,
			&VehicleDef::vehHovertankRandomHoverForceStartTimerMax,
			&VehicleDef::vehHovertankRandomHoverForceDurationMin,
			&VehicleDef::vehHovertankRandomHoverForceDurationMax,
			&VehicleDef::vehDiveboatInitialDiveForceFactor,
			&VehicleDef::vehDiveboatContinuingDiveForceFactor,
			&VehicleDef::vehDiveboatMaxDiveTime,
			&VehicleDef::vehDiveboatDiveResetTime,
			&VehicleDef::vehDiveboatSubmergedDragFactor,
			&VehicleDef::vehDiveboatRollFactor,
			&VehicleDef::vehDiveboatBuoyancyOffset,
			&VehicleDef::vehDiveboatSteeringGraphName,
			&VehicleDef::vehOrbiterMinYaw,
			&VehicleDef::vehOrbiterMaxYaw,
			&VehicleDef::vehOrbiterMinZ,
			&VehicleDef::vehOrbiterMaxZ,
			&VehicleDef::vehOrbiterAngularAcceleration,
			&VehicleDef::vehOrbiterAngularMaxVelocity,
			&VehicleDef::vehOrbiterAngularDeceleration,
			&VehicleDef::vehOrbiterAngularADSDeceleration,
			&VehicleDef::vehOrbiterAngularBraking,
			&VehicleDef::vehOrbiterAngularADSBraking,
			&VehicleDef::vehOrbiterAngularLookAheadTime,
			&VehicleDef::vehOrbiterVerticalAcceleration,
			&VehicleDef::vehOrbiterVerticalMaxVelocity,
			&VehicleDef::vehOrbiterVerticalDeceleration,
			&VehicleDef::vehOrbiterVerticalADSDeceleration,
			&VehicleDef::vehOrbiterVerticalBraking,
			&VehicleDef::vehOrbiterVerticalADSBraking,
			&VehicleDef::vehOrbiterVerticalLookAheadTime,
			&VehicleDef::vehOrbiterADSVelocityMult,
			&VehicleDef::vehOrbiterTiltRollMax,
			&VehicleDef::vehOrbiterTiltRollRate,
			&VehicleDef::vehOrbiterTiltPitchMax,
			&VehicleDef::vehOrbiterTiltPitchRate,
			&VehicleDef::camLookEnabled,
			&VehicleDef::camRelativeControl,
			&VehicleDef::camRemoteDrive,
			&VehicleDef::camLerp,
			&VehicleDef::camHeight,
			&VehicleDef::camRadius,
			&VehicleDef::camPitchInfluence,
			&VehicleDef::camYawInfluence,
			&VehicleDef::camRollInfluence,
			&VehicleDef::camFovIncrease,
			&VehicleDef::camFovOffset,
			&VehicleDef::camFovSpeed,
			&VehicleDef::camReturnSpeed,
			&VehicleDef::camReturnLerp,
			&VehicleDef::camVehicleAnglePitchRate,
			&VehicleDef::camVehicleAngleYawRate,
			&VehicleDef::camVehicleAngleRollRate,
			&VehicleDef::camShakeMinSpeed,
			&VehicleDef::camShakeMaxSpeed,
			&VehicleDef::camShakeMinFreq,
			&VehicleDef::camShakeMaxFreq,
			&VehicleDef::camShakeMaxAmplitudePitch,
			&VehicleDef::camShakeMaxAmplitudeYaw,
			&VehicleDef::camShakeMaxAmplitudeRoll,
			&VehicleDef::camShakeMaxAmplitudeX,
			&VehicleDef::camShakeMaxAmplitudeY,
			&VehicleDef::camShakeMaxAmplitudeZ,
			&VehicleDef::camShakeMinAmplitudeScale,
			&VehicleDef::camShakeTurretInherit,
			&VehicleDef::vehCam_UseGDT,
			&VehicleDef::vehCam_anglesPitch,
			&VehicleDef::vehCam_anglesYaw,
			&VehicleDef::vehCam_anglesRoll,
			&VehicleDef::vehCam_offsetX,
			&VehicleDef::vehCam_offsetY,
			&VehicleDef::vehCam_offsetZ,
			&VehicleDef::vehCam_radius,
			&VehicleDef::vehCam_speedInfluence,
			&VehicleDef::vehCam_pitchTurnRate,
			&VehicleDef::vehCam_pitchClamp,
			&VehicleDef::vehCam_yawTurnRate,
			&VehicleDef::vehCam_yawClamp,
			&VehicleDef::vehCam_zOffsetMode,
			&VehicleDef::vehCam_anglesPitch3P,
			&VehicleDef::vehCam_anglesYaw3P,
			&VehicleDef::vehCam_anglesRoll3P,
			&VehicleDef::vehCam_offsetX3P,
			&VehicleDef::vehCam_offsetY3P,
			&VehicleDef::vehCam_offsetZ3P,
			&VehicleDef::vehCam_radius3P,
			&VehicleDef::vehCam_speedInfluence3P,
			&VehicleDef::vehCam_pitchTurnRate3P,
			&VehicleDef::vehCam_pitchClamp3P,
			&VehicleDef::vehCam_yawTurnRate3P,
			&VehicleDef::vehCam_yawTurnRate3PHandbrakeInc,
			&VehicleDef::vehCam_yawClamp3P,
			&VehicleDef::vehCam_zOffsetMode3P,
			&VehicleDef::turretWeaponName,
			&VehicleDef::turretWeapon,
			&VehicleDef::turretHorizSpanLeft,
			&VehicleDef::turretHorizSpanRight,
			&VehicleDef::turretVertSpanUp,
			&VehicleDef::turretVertSpanDown,
			&VehicleDef::turretHorizResistLeft,
			&VehicleDef::turretHorizResistRight,
			&VehicleDef::turretVertResistUp,
			&VehicleDef::turretVertResistDown,
			&VehicleDef::turretRotRate,
			&VehicleDef::turretFireType,
			&VehicleDef::turretSpinSnd,
			&VehicleDef::turretStopSnd,
			&VehicleDef::trophyEnabled,
			&VehicleDef::trophyRadius,
			&VehicleDef::trophyInactiveRadius,
			&VehicleDef::trophyAmmoCount,
			&VehicleDef::trophyReloadTime,
			json_reflect::manual("trophyTags"),
			&VehicleDef::trophyExplodeFx,
			&VehicleDef::trophyFlashFx,
			&VehicleDef::compassFriendlyIcon,
			&VehicleDef::compassEnemyIcon,
			&VehicleDef::compassFriendlyAltIcon,
			&VehicleDef::compassEnemyAltIcon,
			&VehicleDef::compassIconWidth,
			&VehicleDef::compassIconHeight,
			&VehicleDef::idleLowSnd,
			&VehicleDef::idleHighSnd,
			&VehicleDef::engineLowSnd,
			&VehicleDef::engineHighSnd,
			&VehicleDef::boostSnd,
			&VehicleDef::engineSndSpeed,
			json_reflect::manual("audioOriginTag"),
			&VehicleDef::idleLowSndAlt,
			&VehicleDef::idleHighSndAlt,
			&VehicleDef::engineLowSndAlt,
			&VehicleDef::engineHighSndAlt,
			&VehicleDef::engineSndSpeedAlt,
			json_reflect::manual("audioOriginTagAlt"),
			&VehicleDef::turretSpinSndAlt,
			&VehicleDef::turretStopSndAlt,
			&VehicleDef::engineStartUpSnd,
			&VehicleDef::engineStartUpLength,
			&VehicleDef::engineShutdownSnd,
			&VehicleDef::engineIdleSnd,
			&VehicleDef::engineSustainSnd,
			&VehicleDef::engineRampUpSnd,
			&VehicleDef::engineRampUpLength,
			&VehicleDef::engineRampDownSnd,
			&VehicleDef::engineRampDownLength,
			&VehicleDef::suspensionSoftSnd,
			&VehicleDef::suspensionSoftCompression,
			&VehicleDef::suspensionHardSnd,
			&VehicleDef::suspensionHardCompression,
			&VehicleDef::collisionSnd,
			&VehicleDef::collisionBlendSpeed,
			&VehicleDef::speedSnd,
			&VehicleDef::speedSndBlendSpeed,
			&VehicleDef::surfaceSndPrefix,
			&VehicleDef::surfaceSnds,
			&VehicleDef::surfaceSndBlendSpeed,
			&VehicleDef::slideVolume,
			&VehicleDef::slideBlendSpeed,
			&VehicleDef::inAirPitch,
			&VehicleDef::soundTriggerOverrideZone,
			&VehicleDef::soundTriggerOverrideReverb,
			&VehicleDef::soundTriggerOverrideMix,
			&VehicleDef::soundTriggerOverrideFilter,
			&VehicleDef::soundTriggerOverrideOcclusion,
			&VehicleDef::soundTriggerOverrideAmbient,
			&VehicleDef::soundTriggerOverrideAmbientEvents,
		};

		constexpr json_reflect::options json_options{.strings = json_reflect::null_string::null};
	}

	void vehicle_def::add_script_string(scr_string_t* ptr, const char* str)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return;
			}
		}
		this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
	}

	const char* vehicle_def::get_script_string(scr_string_t* ptr)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return this->script_strings[i].second;
			}
		}
		return nullptr;
	}

	VehicleDef* vehicle_def::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "vehicles\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing vehicle \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();
		ordered_json data = json::parse(bytes);

		auto* vehicle = mem->allocate<VehicleDef>();
		json_reflect::read<vehicle_fields>(data, *vehicle, mem, json_options);

		for (auto i = 0; i < 4; i++)
		{
			if (!data["trophyTags"][i].empty())
			{
				this->add_script_string(&vehicle->trophyTags[i], mem->duplicate_string(data["trophyTags"][i].get<std::string>()));
			}
		}

		if (!data["audioOriginTag"].empty())
		{
			this->add_script_string(&vehicle->audioOriginTag, mem->duplicate_string(data["audioOriginTag"].get<std::string>()));
		}

		if (!data["audioOriginTagAlt"].empty())
		{
			this->add_script_string(&vehicle->audioOriginTagAlt, mem->duplicate_string(data["audioOriginTagAlt"].get<std::string>()));
		}

		return vehicle;
	}

	void vehicle_def::init(const std::string& name, zone_memory* mem)
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
			ZONETOOL_FATAL("Vehicle file \"%s\" not found.", name.data());
		}
	}

	void vehicle_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* vehicle = this->asset_;

		for (auto i = 0; i < 4; i++)
		{
			vehicle->trophyTags[i] = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&vehicle->trophyTags[i])));
		}

		vehicle->audioOriginTag = static_cast<scr_string_t>(buf->write_scriptstring(
			this->get_script_string(&vehicle->audioOriginTag)));

		vehicle->audioOriginTagAlt = static_cast<scr_string_t>(buf->write_scriptstring(
			this->get_script_string(&vehicle->audioOriginTagAlt)));
	}

	void vehicle_def::load_depending(zone_base* zone)
	{
		auto vehicle = this->asset_;
#define VEHICLE_SUBASSET_DEPENDING(__field__,__type__/*,__struct__*/) \
		if (vehicle->__field__) \
		{ \
			zone->add_asset_of_type(__type__, vehicle->__field__->name); \
		}

		VEHICLE_SUBASSET_DEPENDING(vehPhysDef.physPreset, ASSET_TYPE_PHYSPRESET);

		VEHICLE_SUBASSET_DEPENDING(treadDefaultFx, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(handBrakeDefaultFx, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(handBrakeLeftFx, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(handBrakeRightFx, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(boostFx, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(vehHelicopterGroundFx, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(vehHelicopterGroundWaterFx, ASSET_TYPE_FX);

		VEHICLE_SUBASSET_DEPENDING(turretWeapon, ASSET_TYPE_WEAPON);

		VEHICLE_SUBASSET_DEPENDING(turretSpinSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(turretStopSnd, ASSET_TYPE_SOUND);

		VEHICLE_SUBASSET_DEPENDING(trophyExplodeFx, ASSET_TYPE_FX);
		VEHICLE_SUBASSET_DEPENDING(trophyFlashFx, ASSET_TYPE_FX);

		VEHICLE_SUBASSET_DEPENDING(compassFriendlyIcon, ASSET_TYPE_MATERIAL);
		VEHICLE_SUBASSET_DEPENDING(compassEnemyIcon, ASSET_TYPE_MATERIAL);
		VEHICLE_SUBASSET_DEPENDING(compassFriendlyAltIcon, ASSET_TYPE_MATERIAL);
		VEHICLE_SUBASSET_DEPENDING(compassEnemyAltIcon, ASSET_TYPE_MATERIAL);

		VEHICLE_SUBASSET_DEPENDING(idleLowSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(idleHighSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineLowSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineHighSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(boostSnd, ASSET_TYPE_SOUND);

		VEHICLE_SUBASSET_DEPENDING(idleLowSndAlt, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(idleHighSndAlt, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineLowSndAlt, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineHighSndAlt, ASSET_TYPE_SOUND);

		VEHICLE_SUBASSET_DEPENDING(turretSpinSndAlt, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(turretStopSndAlt, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineStartUpSnd, ASSET_TYPE_SOUND);

		VEHICLE_SUBASSET_DEPENDING(engineShutdownSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineIdleSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineSustainSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineRampUpSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(engineRampDownSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(suspensionSoftSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(suspensionHardSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(collisionSnd, ASSET_TYPE_SOUND);
		VEHICLE_SUBASSET_DEPENDING(speedSnd, ASSET_TYPE_SOUND);

		for (auto i = 0; i < 53; i++)
		{
			VEHICLE_SUBASSET_DEPENDING(surfaceSnds[i], ASSET_TYPE_SOUND);
		}
	}

	std::string vehicle_def::name()
	{
		return this->name_;
	}

	std::int32_t vehicle_def::type()
	{
		return ASSET_TYPE_VEHICLE;
	}

	namespace
	{
		void read_physics(zone_reader& reader, VehiclePhysDef* physics)
		{
			reader.read_string(physics->physPresetName);
			reader.read_asset(ASSET_TYPE_PHYSPRESET, physics->physPreset);
			reader.read_string(physics->physMassName);
			reader.read_single(physics->physMass, 3);
			reader.read_string(physics->accelGraphName);
		}

		void read_engine_sounds(zone_reader& reader, VehicleDef* asset)
		{
			reader.read_name_reference(asset->idleLowSnd);
			reader.read_name_reference(asset->idleHighSnd);
			reader.read_name_reference(asset->engineLowSnd);
			reader.read_name_reference(asset->engineHighSnd);
			reader.read_name_reference(asset->boostSnd);
			reader.read_name_reference(asset->idleLowSndAlt);
			reader.read_name_reference(asset->idleHighSndAlt);
			reader.read_name_reference(asset->engineLowSndAlt);
			reader.read_name_reference(asset->engineHighSndAlt);
			reader.read_name_reference(asset->turretSpinSndAlt);
			reader.read_name_reference(asset->turretStopSndAlt);
			reader.read_name_reference(asset->engineStartUpSnd);
			reader.read_name_reference(asset->engineShutdownSnd);
			reader.read_name_reference(asset->engineIdleSnd);
			reader.read_name_reference(asset->engineSustainSnd);
			reader.read_name_reference(asset->engineRampUpSnd);
			reader.read_name_reference(asset->engineRampDownSnd);
			reader.read_name_reference(asset->suspensionSoftSnd);
			reader.read_name_reference(asset->suspensionHardSnd);
			reader.read_name_reference(asset->collisionSnd);
			reader.read_name_reference(asset->speedSnd);
		}
	}

	void vehicle_def::read(zone_reader& reader, VehicleDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->useHintString);
		reader.read_script_strings(asset->trophyTags, std::size(asset->trophyTags));
		reader.read_script_string(asset->audioOriginTag);
		reader.read_script_string(asset->audioOriginTagAlt);
		read_physics(reader, &asset->vehPhysDef);
		reader.read_asset(ASSET_TYPE_FX, asset->treadDefaultFx);
		reader.read_asset(ASSET_TYPE_FX, asset->handBrakeDefaultFx);
		reader.read_asset(ASSET_TYPE_FX, asset->handBrakeLeftFx);
		reader.read_asset(ASSET_TYPE_FX, asset->handBrakeRightFx);
		reader.read_asset(ASSET_TYPE_FX, asset->boostFx);
		reader.read_asset(ASSET_TYPE_FX, asset->vehHelicopterGroundFx);
		reader.read_asset(ASSET_TYPE_FX, asset->vehHelicopterGroundWaterFx);
		reader.read_string(asset->vehDiveboatSteeringGraphName);
		reader.read_string(asset->turretWeaponName);
		reader.read_asset(ASSET_TYPE_WEAPON, asset->turretWeapon);
		reader.read_name_reference(asset->turretSpinSnd);
		reader.read_name_reference(asset->turretStopSnd);
		reader.read_asset(ASSET_TYPE_FX, asset->trophyExplodeFx);
		reader.read_asset(ASSET_TYPE_FX, asset->trophyFlashFx);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassFriendlyIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassEnemyIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassFriendlyAltIcon);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->compassEnemyAltIcon);
		read_engine_sounds(reader, asset);
		reader.read_string(asset->surfaceSndPrefix);

		for (auto& sound : asset->surfaceSnds)
		{
			reader.read_name_reference(sound);
		}

		reader.read_string(asset->soundTriggerOverrideZone);
		reader.pop_stream();
	}

	void vehicle_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

#define VEHICLE_STRING(__field__) \
		if (data->__field__) \
		{ \
			dest->__field__ = buf->write_str(data->__field__); \
		}

#define VEHICLE_SUBASSET(__field__,__type__,__struct__) \
		if (data->__field__) \
		{ \
			dest->__field__ = reinterpret_cast<__struct__*>(zone->get_asset_pointer(__type__, data->__field__->name)); \
		}

#define VEHICLE_SOUND_CUSTOM(__field__) \
		if (data->__field__) \
		{ \
			buf->align(7); \
			buf->write(&buf->data_following); \
			buf->write_str_raw(data->__field__->name); \
			buf->clear_pointer(&dest->__field__); \
		}

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		VEHICLE_STRING(useHintString);

		VEHICLE_STRING(vehPhysDef.physPresetName);
		VEHICLE_SUBASSET(vehPhysDef.physPreset, ASSET_TYPE_PHYSPRESET, PhysPreset);

		VEHICLE_STRING(vehPhysDef.physMassName);
		if (data->vehPhysDef.physMass)
		{
			buf->align(3);
			buf->write(data->vehPhysDef.physMass);
			buf->clear_pointer(&dest->vehPhysDef.physMass);
		}

		VEHICLE_STRING(vehPhysDef.accelGraphName);

		VEHICLE_SUBASSET(treadDefaultFx, ASSET_TYPE_FX, FxEffectDef);
		VEHICLE_SUBASSET(handBrakeDefaultFx, ASSET_TYPE_FX, FxEffectDef);
		VEHICLE_SUBASSET(handBrakeLeftFx, ASSET_TYPE_FX, FxEffectDef);
		VEHICLE_SUBASSET(handBrakeRightFx, ASSET_TYPE_FX, FxEffectDef);
		VEHICLE_SUBASSET(boostFx, ASSET_TYPE_FX, FxEffectDef);

		VEHICLE_SUBASSET(vehHelicopterGroundFx, ASSET_TYPE_FX, FxEffectDef);
		VEHICLE_SUBASSET(vehHelicopterGroundWaterFx, ASSET_TYPE_FX, FxEffectDef);

		VEHICLE_STRING(vehDiveboatSteeringGraphName);
		VEHICLE_STRING(turretWeaponName);

		VEHICLE_SUBASSET(turretWeapon, ASSET_TYPE_WEAPON, WeaponDef);

		VEHICLE_SOUND_CUSTOM(turretSpinSnd);
		VEHICLE_SOUND_CUSTOM(turretStopSnd);

		VEHICLE_SUBASSET(trophyExplodeFx, ASSET_TYPE_FX, FxEffectDef);
		VEHICLE_SUBASSET(trophyFlashFx, ASSET_TYPE_FX, FxEffectDef);

		VEHICLE_SUBASSET(compassFriendlyIcon, ASSET_TYPE_MATERIAL, Material);
		VEHICLE_SUBASSET(compassEnemyIcon, ASSET_TYPE_MATERIAL, Material);
		VEHICLE_SUBASSET(compassFriendlyAltIcon, ASSET_TYPE_MATERIAL, Material);
		VEHICLE_SUBASSET(compassEnemyAltIcon, ASSET_TYPE_MATERIAL, Material);

		VEHICLE_SOUND_CUSTOM(idleLowSnd);
		VEHICLE_SOUND_CUSTOM(idleHighSnd);
		VEHICLE_SOUND_CUSTOM(engineLowSnd);
		VEHICLE_SOUND_CUSTOM(engineHighSnd);
		VEHICLE_SOUND_CUSTOM(boostSnd);
		VEHICLE_SOUND_CUSTOM(idleLowSndAlt);
		VEHICLE_SOUND_CUSTOM(idleHighSndAlt);
		VEHICLE_SOUND_CUSTOM(engineLowSndAlt);
		VEHICLE_SOUND_CUSTOM(engineHighSndAlt);
		VEHICLE_SOUND_CUSTOM(turretSpinSndAlt);
		VEHICLE_SOUND_CUSTOM(turretStopSndAlt);
		VEHICLE_SOUND_CUSTOM(engineStartUpSnd);
		VEHICLE_SOUND_CUSTOM(engineShutdownSnd);
		VEHICLE_SOUND_CUSTOM(engineIdleSnd);
		VEHICLE_SOUND_CUSTOM(engineSustainSnd);
		VEHICLE_SOUND_CUSTOM(engineRampUpSnd);
		VEHICLE_SOUND_CUSTOM(engineRampDownSnd);
		VEHICLE_SOUND_CUSTOM(suspensionSoftSnd);
		VEHICLE_SOUND_CUSTOM(suspensionHardSnd);
		VEHICLE_SOUND_CUSTOM(collisionSnd);
		VEHICLE_SOUND_CUSTOM(speedSnd);

		VEHICLE_STRING(surfaceSndPrefix);

		for (auto i = 0; i < 53; i++)
		{
			VEHICLE_SOUND_CUSTOM(surfaceSnds[i]);
		}

		VEHICLE_STRING(soundTriggerOverrideZone);

		buf->pop_stream();
	}

	void vehicle_def::dump(VehicleDef* asset)
	{
		const auto path = "vehicles\\"s + asset->name + ".json"s;

		ordered_json data;
		json_reflect::write<vehicle_fields>(data, *asset, json_options);

		for (auto i = 0; i < 4; i++)
		{
			if (asset->trophyTags[i])
			{
				data["trophyTags"][i] = SL_ConvertToString(asset->trophyTags[i]);
			}
			else
			{
				data["trophyTags"][i] = "";
			}
		}

		if (asset->audioOriginTag)
		{
			data["audioOriginTag"] = SL_ConvertToString(asset->audioOriginTag);
		}
		else
		{
			data["audioOriginTag"] = "";
		}

		if (asset->audioOriginTagAlt)
		{
			data["audioOriginTagAlt"] = SL_ConvertToString(asset->audioOriginTagAlt);
		}
		else
		{
			data["audioOriginTagAlt"] = "";
		}

		std::string json = data.dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}
