#include <std_include.hpp>
#include "vehicle.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_physics(Walk walk, VehiclePhysicsDef& physics)
		{
			walk.inline_script_strings(physics.physics_wheelBones, std::size(physics.physics_wheelBones));
			walk.inline_script_strings(physics.unknownStrings0, std::size(physics.unknownStrings0));
			walk.inline_script_strings(physics.unknownStrings1, std::size(physics.unknownStrings1));
			walk.array(physics.unknownPoints, physics.numUnknownPoints, 3);
			walk.inline_script_strings(physics.unknownStrings2, std::size(physics.unknownStrings2));
			walk.asset(ASSET_TYPE_SKELETON_CONSTRAINTS, physics.skeletonConstraints);

			for (auto& constraints : physics.unknownSkeletonConstraints)
			{
				walk.asset(ASSET_TYPE_SKELETON_CONSTRAINTS, constraints);
			}

			for (auto& group : physics.vfxGroups)
			{
				walk.asset(ASSET_TYPE_FOOTSTEP_VFX, group.surfaceEffects);
				walk.asset(ASSET_TYPE_FOOTSTEP_VFX, group.unknownSurfaceEffects);
			}
		}

		template <typename Walk>
		void walk_vehicle(Walk walk, VehicleDef& asset)
		{
			walk.string(asset.unknownString0);
			walk.inline_script_strings(asset.unknownStrings0, std::size(asset.unknownStrings0));
			walk.string(asset.unknownString1);
			walk.asset(ASSET_TYPE_CAMERA, asset.cameraDef);
			walk.asset(ASSET_TYPE_CAMERA, asset.killCamDef);
			walk.string(asset.unknownString2);
			walk_physics(walk, asset.vehiclePhysicsDef);

			for (auto& fx : asset.unknownFx0)
			{
				walk.asset(ASSET_TYPE_VFX, fx.particleSystemDef);
			}

			walk.string(asset.turretWeaponName);
			walk.asset(ASSET_TYPE_WEAPON, asset.turretWeapon);
			walk.inline_script_strings(asset.unknownStrings1, std::size(asset.unknownStrings1));

			for (auto& fx : asset.unknownFx1)
			{
				walk.asset(ASSET_TYPE_VFX, fx.particleSystemDef);
			}

			for (auto& material : asset.unknownMaterials)
			{
				walk.asset(ASSET_TYPE_MATERIAL, material);
			}

			for (auto& bank : asset.soundBanks)
			{
				walk.asset(ASSET_TYPE_SOUND_BANK_TRANSIENT, bank);
			}

			walk.script_string(asset.unknownString3);
			walk.array(asset.unknownPoints.points, asset.unknownPoints.numPoints, 3);

			for (auto& string : asset.unknownStrings2)
			{
				walk.string(string.string);
			}

			for (auto& string : asset.unknownStrings3)
			{
				walk.string(string);
			}
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
