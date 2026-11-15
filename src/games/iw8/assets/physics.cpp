#include <std_include.hpp>
#include "physics.hpp"

namespace zonetool::iw8
{
	namespace
	{
		template <typename Rules>
		void read_rules(zone_reader& reader, Rules& rules);

		template <typename Rule>
		void read_rule_entries(zone_reader& reader, Rule& rule)
		{
			if (const auto entries = reader.read_array(rule.entries, 7, rule.numentries))
			{
				for (auto i = 0; i < rule.numentries; i++)
				{
					read_rules(reader, entries[i].rules);
				}
			}
		}

		void read_rule(zone_reader& reader, PhysicsSFXEventAssetRule& rule)
		{
			switch (rule.type)
			{
			case Types_PhysicsSFXEventSoundRule:
				reader.read_string(rule.u.soundRule.hitSoundAlias);
				reader.read_string(rule.u.soundRule.scrapeSoundAlias);
				break;
			case Types_PhysicsSFXEventMaterialRule:
				read_rule_entries(reader, rule.u.materialRule);
				break;
			case Types_PhysicsSFXEventAngleRule:
				read_rule_entries(reader, rule.u.angleRule);
				break;
			case Types_PhysicsSFXEventSpeedRule:
				read_rule_entries(reader, rule.u.speedRule);
				break;
			case Types_PhysicsSFXEventMassRule:
				read_rule_entries(reader, rule.u.massRule);
				break;
			case Types_PhysicsSFXEventGravityRule:
				read_rule_entries(reader, rule.u.gravityRule);
				break;
			case Types_PhysicsSFXEventAtmosphereRule:
				read_rule_entries(reader, rule.u.atmosphereRule);
				break;
			case Types_PhysicsSFXEventCameraDistanceRule:
				read_rule_entries(reader, rule.u.cameraDistanceRule);
				break;
			case Types_PhysicsSFXEventFrequencyRule:
				read_rule_entries(reader, rule.u.frequencyRule);
				break;
			default:
				break;
			}
		}

		void read_rule(zone_reader& reader, PhysicsVFXEventAssetRule& rule)
		{
			switch (rule.type)
			{
			case Types_PhysicsVFXEventParticleEffectRule:
				reader.read_string(rule.u.particleEffectRule.hitParticleEffectAlias);
				reader.read_string(rule.u.particleEffectRule.scrapeParticleEffectAlias);
				break;
			case Types_PhysicsVFXEventMaterialRule:
				read_rule_entries(reader, rule.u.materialRule);
				break;
			case Types_PhysicsVFXEventAngleRule:
				read_rule_entries(reader, rule.u.angleRule);
				break;
			case Types_PhysicsVFXEventSpeedRule:
				read_rule_entries(reader, rule.u.speedRule);
				break;
			case Types_PhysicsVFXEventMassRule:
				read_rule_entries(reader, rule.u.massRule);
				break;
			case Types_PhysicsVFXEventGravityRule:
				read_rule_entries(reader, rule.u.gravityRule);
				break;
			case Types_PhysicsVFXEventAtmosphereRule:
				read_rule_entries(reader, rule.u.atmosphereRule);
				break;
			case Types_PhysicsVFXEventCameraDistanceRule:
				read_rule_entries(reader, rule.u.cameraDistanceRule);
				break;
			case Types_PhysicsVFXEventFrequencyRule:
				read_rule_entries(reader, rule.u.frequencyRule);
				break;
			default:
				break;
			}
		}

		template <typename Rules>
		void read_rules(zone_reader& reader, Rules& rules)
		{
			if (const auto entries = reader.read_array(rules.rules, 7, rules.numrules))
			{
				for (auto i = 0; i < rules.numrules; i++)
				{
					read_rule(reader, entries[i]);
				}
			}
		}
	}

	void physics_sfx_event::read(zone_reader& reader, PhysicsSFXEventAsset* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_rules(reader, asset->rules);
		reader.pop_stream();
	}

	void physics_vfx_event::read(zone_reader& reader, PhysicsVFXEventAsset* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_rules(reader, asset->rules);
		reader.pop_stream();
	}

	void physics_asset::read(zone_reader& reader, PhysicsAsset* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->havokData, 15, asset->havokDataSize);
		reader.read_array(asset->simulationCategories, 3, asset->numRigidBodies);
		reader.read_array(asset->bodyContents, 3, asset->numRigidBodies);
		reader.read_asset_array(ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET, asset->sfxEventAssets, asset->numSFXEventAssets);
		reader.read_asset_array(ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET, asset->vfxEventAssets, asset->numVFXEventAssets);
		reader.pop_stream();
	}

	void physics_fx_shape::read(zone_reader& reader, PhysicsFXShape* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->havokData, 15, asset->havokDataSize);
		reader.read_string(asset->soundAlias);
		reader.pop_stream();
	}

	void physics_fx_pipeline::read(zone_reader& reader, PhysicsFXPipeline* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}
}
