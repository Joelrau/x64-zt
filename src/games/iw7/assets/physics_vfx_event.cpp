#include <std_include.hpp>
#include "physics_vfx_event.hpp"

namespace zonetool::iw7
{
	PhysicsVFXEventAsset* physics_vfx_event::parse(const std::string& name, zone_memory* mem)
	{
		// TODO:

		/*const auto path = "physicsvfxeventasset\\"s + name;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physicsvfxeventasset \"%s\"...", name.data());

		auto* asset = read.read_single<PhysicsVFXEventAsset>();
		asset->name = read.read_string();
		
		//

		read.close();

		return asset;*/

		return nullptr;
	}

	void physics_vfx_event::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physicsVFXEventAsset;
		}
	}

	void physics_vfx_event::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void physics_vfx_event::load_depending(zone_base* zone)
	{
	}

	std::string physics_vfx_event::name()
	{
		return this->name_;
	}

	std::int32_t physics_vfx_event::type()
	{
		return ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET;
	}

	void write_rules(PhysicsVFXEventAssetRules* data, PhysicsVFXEventAssetRules* dest, zone_base* zone, zone_buffer* buf);

	void write_rule(PhysicsVFXEventAssetRule* data, PhysicsVFXEventAssetRule* dest, zone_base* zone, zone_buffer* buf)
	{
		switch (data->type)
		{
		case Types_PhysicsVFXEventParticleEffectRule:
			dest->u.particleEffectRule.hitParticleEffectAlias = buf->write_str(data->u.particleEffectRule.hitParticleEffectAlias);
			dest->u.particleEffectRule.scrapeParticleEffectAlias = buf->write_str(data->u.particleEffectRule.scrapeParticleEffectAlias);
			break;
		case Types_PhysicsSFXEventMaterialRule:
			if (data->u.materialRule.entries)
			{
				buf->align(7);
				auto* dest_entries = buf->write(data->u.materialRule.entries, data->u.materialRule.numentries);
				auto* data_entries = data->u.materialRule.entries;
				for (auto entry_index = 0; entry_index < data->u.materialRule.numentries; entry_index++)
				{
					write_rules(&data_entries[entry_index].rules, &dest_entries[entry_index].rules, zone, buf);
				}
				buf->clear_pointer(&dest->u.materialRule.entries);
			}
			break;
		case Types_PhysicsSFXEventAngleRule:
			if (data->u.angleRule.entries)
			{
				buf->align(7);
				auto* dest_entries = buf->write(data->u.angleRule.entries, data->u.angleRule.numentries);
				auto* data_entries = data->u.angleRule.entries;
				for (auto entry_index = 0; entry_index < data->u.angleRule.numentries; entry_index++)
				{
					write_rules(&data_entries[entry_index].rules, &dest_entries[entry_index].rules, zone, buf);
				}
				buf->clear_pointer(&dest->u.angleRule.entries);
			}
			break;
		case Types_PhysicsSFXEventSpeedRule:
			if (data->u.speedRule.entries)
			{
				buf->align(7);
				auto* dest_entries = buf->write(data->u.speedRule.entries, data->u.speedRule.numentries);
				auto* data_entries = data->u.speedRule.entries;
				for (auto entry_index = 0; entry_index < data->u.speedRule.numentries; entry_index++)
				{
					write_rules(&data_entries[entry_index].rules, &dest_entries[entry_index].rules, zone, buf);
				}
				buf->clear_pointer(&dest->u.speedRule.entries);
			}
			break;
		case Types_PhysicsSFXEventMassRule:
			if (data->u.massRule.entries)
			{
				buf->align(7);
				auto* dest_entries = buf->write(data->u.massRule.entries, data->u.massRule.numentries);
				auto* data_entries = data->u.massRule.entries;
				for (auto entry_index = 0; entry_index < data->u.massRule.numentries; entry_index++)
				{
					write_rules(&data_entries[entry_index].rules, &dest_entries[entry_index].rules, zone, buf);
				}
				buf->clear_pointer(&dest->u.massRule.entries);
			}
			break;
		case Types_PhysicsSFXEventGravityRule:
			if (data->u.gravityRule.entries)
			{
				buf->align(7);
				auto* dest_entries = buf->write(data->u.gravityRule.entries, data->u.gravityRule.numentries);
				auto* data_entries = data->u.gravityRule.entries;
				for (auto entry_index = 0; entry_index < data->u.gravityRule.numentries; entry_index++)
				{
					write_rules(&data_entries[entry_index].rules, &dest_entries[entry_index].rules, zone, buf);
				}
				buf->clear_pointer(&dest->u.gravityRule.entries);
			}
			break;
		case Types_PhysicsSFXEventAtmosphereRule:
			if (data->u.atmosphereRule.entries)
			{
				buf->align(7);
				auto* dest_entries = buf->write(data->u.atmosphereRule.entries, data->u.atmosphereRule.numentries);
				auto* data_entries = data->u.atmosphereRule.entries;
				for (auto entry_index = 0; entry_index < data->u.atmosphereRule.numentries; entry_index++)
				{
					write_rules(&data_entries[entry_index].rules, &dest_entries[entry_index].rules, zone, buf);
				}
				buf->clear_pointer(&dest->u.atmosphereRule.entries);
			}
			break;
		case Types_PhysicsSFXEventCameraDistanceRule:
			if (data->u.cameraDistanceRule.entries)
			{
				buf->align(7);
				auto* dest_entries = buf->write(data->u.cameraDistanceRule.entries, data->u.cameraDistanceRule.numentries);
				auto* data_entries = data->u.cameraDistanceRule.entries;
				for (auto entry_index = 0; entry_index < data->u.cameraDistanceRule.numentries; entry_index++)
				{
					write_rules(&data_entries[entry_index].rules, &dest_entries[entry_index].rules, zone, buf);
				}
				buf->clear_pointer(&dest->u.cameraDistanceRule.entries);
			}
			break;
		case Types_PhysicsSFXEventFrequencyRule:
			if (data->u.frequencyRule.entries)
			{
				buf->align(7);
				auto* dest_entries = buf->write(data->u.frequencyRule.entries, data->u.frequencyRule.numentries);
				auto* data_entries = data->u.frequencyRule.entries;
				for (auto entry_index = 0; entry_index < data->u.frequencyRule.numentries; entry_index++)
				{
					write_rules(&data_entries[entry_index].rules, &dest_entries[entry_index].rules, zone, buf);
				}
				buf->clear_pointer(&dest->u.frequencyRule.entries);
			}
			break;
		}
	}

	void write_rules(PhysicsVFXEventAssetRules* data, PhysicsVFXEventAssetRules* dest, zone_base* zone, zone_buffer* buf)
	{
		if (data->rules)
		{
			buf->align(7);

			auto* dest_rules = buf->write(data->rules, data->numrules);
			auto* data_rules = data->rules;
			for (auto rule_index = 0; rule_index < data->numrules; rule_index++)
			{
				write_rule(&data_rules[rule_index], &dest_rules[rule_index], zone, buf);
			}

			buf->clear_pointer(&dest->rules);
		}
	}

	namespace
	{
		void read_rules(zone_reader& reader, PhysicsVFXEventAssetRules& rules);

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
			}
		}

		void read_rules(zone_reader& reader, PhysicsVFXEventAssetRules& rules)
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

	void physics_vfx_event::read(zone_reader& reader, PhysicsVFXEventAsset* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_rules(reader, asset->rules);
		reader.pop_stream();
	}

	void physics_vfx_event::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		write_rules(&data->rules, &dest->rules, zone, buf);

		buf->pop_stream();
	}

	void physics_vfx_event::dump(PhysicsVFXEventAsset* asset)
	{
		// TODO:

		/*const auto path = "physicsvfxeventasset\\"s + asset->name;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		
		//

		write.close();*/
	}
}
