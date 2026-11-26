#include <std_include.hpp>
#include "physics.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_constraint_material(auto& reader, PhysConstraintMaterial* data)
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, data->material);
			for (auto& record : data->records)
			{
				reader.read_array(record.values, 3, record.valueCount);
			}
		}

		void read_constraint_curves(auto& reader, PhysConstraintCurves* data)
		{
			reader.read_array(data->points, 3, data->pointCount);
			for (auto& ref : data->curves1)
			{
				reader.read_asset(ASSET_TYPE_XANIMCURVE, ref.curve);
			}

			for (auto& ref : data->curves2)
			{
				reader.read_asset(ASSET_TYPE_XANIMCURVE, ref.curve);
			}
		}

	}

	void read_phys_constraint(auto& reader, PhysConstraint* constraint)
	{
		switch (constraint->type)
		{
		case PHYS_CONSTRAINT_CURVES:
			read_constraint_curves(reader, &constraint->curves);
			break;
		case PHYS_CONSTRAINT_NAMES:
			break;
		case PHYS_CONSTRAINT_POINTS:
			reader.read_array(constraint->points.points, 3, constraint->points.pointCount);
			break;
		default:
			read_constraint_material(reader, &constraint->material);
			break;
		}
	}

	void physpreset::read(auto& reader, PhysPreset* asset)
	{
		reader.read_asset(ASSET_TYPE_FX, asset->fx);
		reader.read_asset(ASSET_TYPE_IMPACTSFXTABLE, asset->impactFxTable);
		reader.read_asset(ASSET_TYPE_IMPACTSOUNDSTABLE, asset->impactSoundsTable);
	}

	void physconstraints::read(auto& reader, PhysConstraints* asset)
	{
		reader.read_asset(ASSET_TYPE_RAGDOLL, asset->ragdoll);
		reader.read_array(asset->bodyInfos, 15, 4);

		if (const auto constraints = reader.read_array(asset->constraints, 7, asset->constraintCount))
		{
			for (auto i = 0u; i < asset->constraintCount; i++)
			{
				read_phys_constraint(reader, &constraints[i]);
			}
		}

		reader.read_array(asset->bodyNames, 3, asset->bodyNameCount);
	}

	void ragdoll::read(auto& reader, Ragdoll* asset)
	{
		reader.read_asset(ASSET_TYPE_IMPACTSOUNDSTABLE, asset->impactSoundsTable);
	}

	void xanim_curve::read(auto& reader, XAnimCurve* asset)
	{
		reader.read_array(asset->keys, 3, asset->keyCount);
	}
}

namespace zonetool::t9
{
	template void read_phys_constraint(zone_reader&, PhysConstraint*);
	template void physpreset::read(zone_reader&, PhysPreset*);
	template void physconstraints::read(zone_reader&, PhysConstraints*);
	template void ragdoll::read(zone_reader&, Ragdoll*);
	template void xanim_curve::read(zone_reader&, XAnimCurve*);
	template void read_phys_constraint(zone_writer&, PhysConstraint*);
	template void physpreset::read(zone_writer&, PhysPreset*);
	template void physconstraints::read(zone_writer&, PhysConstraints*);
	template void ragdoll::read(zone_writer&, Ragdoll*);
	template void xanim_curve::read(zone_writer&, XAnimCurve*);
}
