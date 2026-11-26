#include <std_include.hpp>
#include "weaponparts.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_beam(Walk walk, Beam& asset)
		{
			for (auto& effect : asset.effects)
			{
				walk.asset(ASSET_TYPE_VFX, effect.particleSystemDef);
			}

			for (auto& model : asset.models)
			{
				walk.asset(ASSET_TYPE_XMODEL, model);
			}
		}

		template <typename Walk>
		void walk_curve(Walk walk, Curve& asset)
		{
			if (const auto segments = walk.array(asset.segments, asset.numSegments, 7))
			{
				for (auto i = 0; i < asset.numSegments; i++)
				{
					walk.array(segments[i].points, segments[i].numPoints, 3);
					walk.array(segments[i].data, segments[i].dataSize, 0);
				}
			}
		}

		template <typename Walk>
		void walk_weapon_trigger(Walk walk, WeaponTrigger& asset)
		{
			for (auto& rumble : asset.rumbles)
			{
				walk.asset(ASSET_TYPE_RUMBLE, rumble);
			}
		}

		template <typename Walk>
		void walk_weapon_accuracy(Walk walk, WeaponAccuracy& asset)
		{
			walk.array(asset.knots, asset.knotCount, 3);
		}
	}

	void beam::read(zone_reader& reader, Beam* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_beam(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void beam::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_beam(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void beam::dump(Beam* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("beam\\"s + asset->name + ".beamb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_beam(dump_walk{dump}, *asset);
		dump.close();
	}

	Beam* beam::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("beam\\"s + name + ".beamb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing beam \"%s\"...", name.data());

		auto* asset = read.read_single<Beam>();
		asset->name = read.read_string();
		walk_beam(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void beam::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<Beam>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void beam::load_depending(zone_base* zone)
	{
		if (!this->referenced())
		{
			walk_beam(depend_walk{zone}, *this->asset_);
		}
	}

	std::string beam::name()
	{
		return this->name_;
	}

	std::int32_t beam::type()
	{
		return ASSET_TYPE_BEAM;
	}

	void curve::read(zone_reader& reader, Curve* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_curve(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void curve::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_curve(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void curve::dump(Curve* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("curve\\"s + asset->name + ".curveb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_curve(dump_walk{dump}, *asset);
		dump.close();
	}

	Curve* curve::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("curve\\"s + name + ".curveb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing curve \"%s\"...", name.data());

		auto* asset = read.read_single<Curve>();
		asset->name = read.read_string();
		walk_curve(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void curve::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<Curve>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void curve::load_depending(zone_base* zone)
	{
		if (!this->referenced())
		{
			walk_curve(depend_walk{zone}, *this->asset_);
		}
	}

	std::string curve::name()
	{
		return this->name_;
	}

	std::int32_t curve::type()
	{
		return ASSET_TYPE_CURVE;
	}

	void trigger_effect::read(zone_reader& reader, TriggerEffect* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}

	void trigger_effect::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		buf->pop_stream();
	}

	void trigger_effect::dump(TriggerEffect* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("triggereffect\\"s + asset->name + ".triggereffectb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.close();
	}

	TriggerEffect* trigger_effect::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("triggereffect\\"s + name + ".triggereffectb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing triggereffect \"%s\"...", name.data());

		auto* asset = read.read_single<TriggerEffect>();
		asset->name = read.read_string();
		read.close();
		return asset;
	}

	void trigger_effect::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<TriggerEffect>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string trigger_effect::name()
	{
		return this->name_;
	}

	std::int32_t trigger_effect::type()
	{
		return ASSET_TYPE_TRIGGER_EFFECT;
	}

	void weapon_trigger::read(zone_reader& reader, WeaponTrigger* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_weapon_trigger(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void weapon_trigger::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_weapon_trigger(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void weapon_trigger::dump(WeaponTrigger* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("weapontrigger\\"s + asset->name + ".weapontriggerb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_weapon_trigger(dump_walk{dump}, *asset);
		dump.close();
	}

	WeaponTrigger* weapon_trigger::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("weapontrigger\\"s + name + ".weapontriggerb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing weapontrigger \"%s\"...", name.data());

		auto* asset = read.read_single<WeaponTrigger>();
		asset->name = read.read_string();
		walk_weapon_trigger(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void weapon_trigger::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<WeaponTrigger>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void weapon_trigger::load_depending(zone_base* zone)
	{
		if (!this->referenced())
		{
			walk_weapon_trigger(depend_walk{zone}, *this->asset_);
		}
	}

	std::string weapon_trigger::name()
	{
		return this->name_;
	}

	std::int32_t weapon_trigger::type()
	{
		return ASSET_TYPE_WEAPON_TRIGGER;
	}

	void reactive_stage_set::read(zone_reader& reader, ReactiveStageSet* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}

	void reactive_stage_set::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		buf->pop_stream();
	}

	void reactive_stage_set::dump(ReactiveStageSet* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("reactivestageset\\"s + asset->name + ".reactivestagesetb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.close();
	}

	ReactiveStageSet* reactive_stage_set::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("reactivestageset\\"s + name + ".reactivestagesetb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing reactivestageset \"%s\"...", name.data());

		auto* asset = read.read_single<ReactiveStageSet>();
		asset->name = read.read_string();
		read.close();
		return asset;
	}

	void reactive_stage_set::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<ReactiveStageSet>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string reactive_stage_set::name()
	{
		return this->name_;
	}

	std::int32_t reactive_stage_set::type()
	{
		return ASSET_TYPE_REACTIVE_STAGE_SET;
	}

	void weapon_accuracy::read(zone_reader& reader, WeaponAccuracy* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_weapon_accuracy(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void weapon_accuracy::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_weapon_accuracy(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void weapon_accuracy::dump(WeaponAccuracy* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("weaponaccuracy\\"s + asset->name + ".weaponaccuracyb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_weapon_accuracy(dump_walk{dump}, *asset);
		dump.close();
	}

	WeaponAccuracy* weapon_accuracy::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("weaponaccuracy\\"s + name + ".weaponaccuracyb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing weaponaccuracy \"%s\"...", name.data());

		auto* asset = read.read_single<WeaponAccuracy>();
		asset->name = read.read_string();
		walk_weapon_accuracy(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void weapon_accuracy::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<WeaponAccuracy>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void weapon_accuracy::load_depending(zone_base* zone)
	{
		if (!this->referenced())
		{
			walk_weapon_accuracy(depend_walk{zone}, *this->asset_);
		}
	}

	std::string weapon_accuracy::name()
	{
		return this->name_;
	}

	std::int32_t weapon_accuracy::type()
	{
		return ASSET_TYPE_WEAPON_ACCURACY;
	}
}
