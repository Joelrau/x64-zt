#include <std_include.hpp>
#include "physics.hpp"

namespace zonetool::iw8
{
	namespace
	{
		template <typename Walk, typename Rules>
		void walk_rules(Walk walk, Rules& rules);

		template <typename Walk, typename Rule>
		void walk_rule_entries(Walk walk, Rule& rule)
		{
			if (const auto entries = walk.array(rule.entries, rule.numentries, 7))
			{
				for (auto i = 0; i < rule.numentries; i++)
				{
					walk_rules(walk, entries[i].rules);
				}
			}
		}

		template <typename Walk>
		void walk_rule(Walk walk, PhysicsSFXEventAssetRule& rule)
		{
			switch (rule.type)
			{
			case Types_PhysicsSFXEventSoundRule:
				walk.string(rule.u.soundRule.hitSoundAlias);
				walk.string(rule.u.soundRule.scrapeSoundAlias);
				break;
			case Types_PhysicsSFXEventMaterialRule:
				walk_rule_entries(walk, rule.u.materialRule);
				break;
			case Types_PhysicsSFXEventAngleRule:
				walk_rule_entries(walk, rule.u.angleRule);
				break;
			case Types_PhysicsSFXEventSpeedRule:
				walk_rule_entries(walk, rule.u.speedRule);
				break;
			case Types_PhysicsSFXEventMassRule:
				walk_rule_entries(walk, rule.u.massRule);
				break;
			case Types_PhysicsSFXEventGravityRule:
				walk_rule_entries(walk, rule.u.gravityRule);
				break;
			case Types_PhysicsSFXEventAtmosphereRule:
				walk_rule_entries(walk, rule.u.atmosphereRule);
				break;
			case Types_PhysicsSFXEventCameraDistanceRule:
				walk_rule_entries(walk, rule.u.cameraDistanceRule);
				break;
			case Types_PhysicsSFXEventFrequencyRule:
				walk_rule_entries(walk, rule.u.frequencyRule);
				break;
			default:
				break;
			}
		}

		template <typename Walk>
		void walk_rule(Walk walk, PhysicsVFXEventAssetRule& rule)
		{
			switch (rule.type)
			{
			case Types_PhysicsVFXEventParticleEffectRule:
				walk.string(rule.u.particleEffectRule.hitParticleEffectAlias);
				walk.string(rule.u.particleEffectRule.scrapeParticleEffectAlias);
				break;
			case Types_PhysicsVFXEventMaterialRule:
				walk_rule_entries(walk, rule.u.materialRule);
				break;
			case Types_PhysicsVFXEventAngleRule:
				walk_rule_entries(walk, rule.u.angleRule);
				break;
			case Types_PhysicsVFXEventSpeedRule:
				walk_rule_entries(walk, rule.u.speedRule);
				break;
			case Types_PhysicsVFXEventMassRule:
				walk_rule_entries(walk, rule.u.massRule);
				break;
			case Types_PhysicsVFXEventGravityRule:
				walk_rule_entries(walk, rule.u.gravityRule);
				break;
			case Types_PhysicsVFXEventAtmosphereRule:
				walk_rule_entries(walk, rule.u.atmosphereRule);
				break;
			case Types_PhysicsVFXEventCameraDistanceRule:
				walk_rule_entries(walk, rule.u.cameraDistanceRule);
				break;
			case Types_PhysicsVFXEventFrequencyRule:
				walk_rule_entries(walk, rule.u.frequencyRule);
				break;
			default:
				break;
			}
		}

		template <typename Walk, typename Rules>
		void walk_rules(Walk walk, Rules& rules)
		{
			if (const auto entries = walk.array(rules.rules, rules.numrules, 7))
			{
				for (auto i = 0; i < rules.numrules; i++)
				{
					walk_rule(walk, entries[i]);
				}
			}
		}
	}

	void physics_sfx_event::read(zone_reader& reader, PhysicsSFXEventAsset* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_rules(read_walk{reader}, asset->rules);
		reader.pop_stream();
	}

	void physics_sfx_event::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_rules(write_walk{zone, buf}, dest->rules);
		buf->pop_stream();
	}

	void physics_sfx_event::dump(PhysicsSFXEventAsset* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("physicssfxeventasset\\"s + asset->name + ".physicssfxeventassetb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_rules(dump_walk{dump}, asset->rules);
		dump.close();
	}

	PhysicsSFXEventAsset* physics_sfx_event::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("physicssfxeventasset\\"s + name + ".physicssfxeventassetb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physicssfxeventasset \"%s\"...", name.data());

		auto* asset = read.read_single<PhysicsSFXEventAsset>();
		asset->name = read.read_string();
		walk_rules(parse_walk{read}, asset->rules);
		read.close();
		return asset;
	}

	void physics_sfx_event::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<PhysicsSFXEventAsset>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string physics_sfx_event::name()
	{
		return this->name_;
	}

	std::int32_t physics_sfx_event::type()
	{
		return ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET;
	}

	void physics_vfx_event::read(zone_reader& reader, PhysicsVFXEventAsset* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_rules(read_walk{reader}, asset->rules);
		reader.pop_stream();
	}

	void physics_vfx_event::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_rules(write_walk{zone, buf}, dest->rules);
		buf->pop_stream();
	}

	void physics_vfx_event::dump(PhysicsVFXEventAsset* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("physicsvfxeventasset\\"s + asset->name + ".physicsvfxeventassetb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_rules(dump_walk{dump}, asset->rules);
		dump.close();
	}

	PhysicsVFXEventAsset* physics_vfx_event::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("physicsvfxeventasset\\"s + name + ".physicsvfxeventassetb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physicsvfxeventasset \"%s\"...", name.data());

		auto* asset = read.read_single<PhysicsVFXEventAsset>();
		asset->name = read.read_string();
		walk_rules(parse_walk{read}, asset->rules);
		read.close();
		return asset;
	}

	void physics_vfx_event::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<PhysicsVFXEventAsset>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string physics_vfx_event::name()
	{
		return this->name_;
	}

	std::int32_t physics_vfx_event::type()
	{
		return ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET;
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

	void physics_asset::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		write_array(buf, dest->havokData, data->havokDataSize, 15);
		write_array(buf, dest->simulationCategories, data->numRigidBodies, 3);
		write_array(buf, dest->bodyContents, data->numRigidBodies, 3);
		write_asset_array(zone, buf, ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET, dest->sfxEventAssets, data->numSFXEventAssets);
		write_asset_array(zone, buf, ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET, dest->vfxEventAssets, data->numVFXEventAssets);
		buf->pop_stream();
	}

	void physics_asset::dump(PhysicsAsset* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("physicsasset\\"s + asset->name + ".physicsassetb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_array(asset->havokData, asset->havokDataSize);
		dump.dump_array(asset->simulationCategories, asset->numRigidBodies);
		dump.dump_array(asset->bodyContents, asset->numRigidBodies);
		dump_asset_array(dump, asset->sfxEventAssets, asset->numSFXEventAssets);
		dump_asset_array(dump, asset->vfxEventAssets, asset->numVFXEventAssets);
		dump.close();
	}

	PhysicsAsset* physics_asset::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("physicsasset\\"s + name + ".physicsassetb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physicsasset \"%s\"...", name.data());

		auto* asset = read.read_single<PhysicsAsset>();
		asset->name = read.read_string();
		asset->havokData = read.read_array<char>();
		asset->simulationCategories = read.read_array<PhysicsAssetSimulationCategory>();
		asset->bodyContents = read.read_array<unsigned int>();
		parse_asset_array(read, asset->sfxEventAssets, asset->numSFXEventAssets);
		parse_asset_array(read, asset->vfxEventAssets, asset->numVFXEventAssets);
		read.close();
		return asset;
	}

	void physics_asset::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<PhysicsAsset>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void physics_asset::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		add_asset_array(zone, ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET, this->asset_->sfxEventAssets, this->asset_->numSFXEventAssets);
		add_asset_array(zone, ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET, this->asset_->vfxEventAssets, this->asset_->numVFXEventAssets);
	}

	std::string physics_asset::name()
	{
		return this->name_;
	}

	std::int32_t physics_asset::type()
	{
		return ASSET_TYPE_PHYSICSASSET;
	}

	void physics_fx_shape::read(zone_reader& reader, PhysicsFXShape* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->havokData, 15, asset->havokDataSize);
		reader.read_string(asset->soundAlias);
		reader.pop_stream();
	}

	void physics_fx_shape::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		write_array(buf, dest->havokData, data->havokDataSize, 15);
		write_string(buf, dest->soundAlias);
		buf->pop_stream();
	}

	void physics_fx_shape::dump(PhysicsFXShape* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("physicsfxshape\\"s + asset->name + ".physicsfxshapeb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_array(asset->havokData, asset->havokDataSize);
		dump.dump_string(asset->soundAlias);
		dump.close();
	}

	PhysicsFXShape* physics_fx_shape::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("physicsfxshape\\"s + name + ".physicsfxshapeb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physicsfxshape \"%s\"...", name.data());

		auto* asset = read.read_single<PhysicsFXShape>();
		asset->name = read.read_string();
		asset->havokData = read.read_array<char>();
		asset->soundAlias = read.read_string();
		read.close();
		return asset;
	}

	void physics_fx_shape::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<PhysicsFXShape>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string physics_fx_shape::name()
	{
		return this->name_;
	}

	std::int32_t physics_fx_shape::type()
	{
		return ASSET_TYPE_PHYSICS_FX_SHAPE;
	}

	void physics_fx_pipeline::read(zone_reader& reader, PhysicsFXPipeline* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}

	void physics_fx_pipeline::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		buf->pop_stream();
	}

	void physics_fx_pipeline::dump(PhysicsFXPipeline* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("physicsfxpipeline\\"s + asset->name + ".physicsfxpipelineb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.close();
	}

	PhysicsFXPipeline* physics_fx_pipeline::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("physicsfxpipeline\\"s + name + ".physicsfxpipelineb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physicsfxpipeline \"%s\"...", name.data());

		auto* asset = read.read_single<PhysicsFXPipeline>();
		asset->name = read.read_string();
		read.close();
		return asset;
	}

	void physics_fx_pipeline::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<PhysicsFXPipeline>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string physics_fx_pipeline::name()
	{
		return this->name_;
	}

	std::int32_t physics_fx_pipeline::type()
	{
		return ASSET_TYPE_PHYSICS_FX_PIPELINE;
	}
}
