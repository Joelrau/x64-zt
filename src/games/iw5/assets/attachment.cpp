#include <std_include.hpp>
#include "attachment.hpp"

namespace zonetool::iw5
{
	void attachment::read(zone_reader32& reader, WeaponAttachment* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->szInternalName);
		reader.read_string(asset->szDisplayName);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->worldModels, 16);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->viewModels, 16);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->reticleViewModels, 8);

		if (const auto ammo = reader.read_single(asset->ammoGeneral, 3))
		{
			reader.read_asset(ASSET_TYPE_TRACER, ammo->tracerType);
		}

		reader.read_single(asset->sight, 3);
		reader.read_single(asset->reload, 0);
		reader.read_single(asset->addOns, 0);

		if (const auto general = reader.read_single(asset->general, 3))
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, general->reticleCenter);
			reader.read_asset(ASSET_TYPE_MATERIAL, general->reticleSide);
		}

		reader.read_single(asset->aimAssist, 3);
		reader.read_single(asset->ammunition, 3);
		reader.read_single(asset->damage, 3);
		reader.read_single(asset->locationDamage, 3);
		reader.read_single(asset->idleSettings, 3);
		reader.read_single(asset->adsSettings, 3);
		reader.read_single(asset->adsSettingsMain, 3);
		reader.read_single(asset->hipSpread, 3);
		reader.read_single(asset->gunKick, 3);
		reader.read_single(asset->viewKick, 3);

		if (const auto overlay = reader.read_single(asset->adsOverlay, 3))
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, overlay->overlay.shader);
			reader.read_asset(ASSET_TYPE_MATERIAL, overlay->overlay.shaderLowRes);
			reader.read_asset(ASSET_TYPE_MATERIAL, overlay->overlay.shaderEMP);
			reader.read_asset(ASSET_TYPE_MATERIAL, overlay->overlay.shaderEMPLowRes);
		}

		if (const auto ui = reader.read_single(asset->ui, 3))
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, ui->dpadIcon);
			reader.read_asset(ASSET_TYPE_MATERIAL, ui->ammoCounterIcon);
		}

		if (const auto rumbles = reader.read_single(asset->rumbles, 3))
		{
			reader.read_string(rumbles->fireRumble);
			reader.read_string(rumbles->meleeImpactRumble);
		}

		if (const auto projectile = reader.read_single(asset->projectile, 3))
		{
			reader.read_asset(ASSET_TYPE_XMODEL, projectile->projectileModel);
			reader.read_asset(ASSET_TYPE_FX, projectile->projExplosionEffect);
			sound::read_custom(reader, projectile->projExplosionSound);
			reader.read_asset(ASSET_TYPE_FX, projectile->projDudEffect);
			sound::read_custom(reader, projectile->projDudSound);
			reader.read_asset(ASSET_TYPE_FX, projectile->projTrailEffect);
			reader.read_asset(ASSET_TYPE_FX, projectile->projIgnitionEffect);
			sound::read_custom(reader, projectile->projIgnitionSound);
		}

		reader.pop_stream();
	}
}
