#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	void read_phys_constraint(auto& reader, PhysConstraint* constraint);

	class physpreset
	{
	public:
		static void read(auto& reader, PhysPreset* asset);
	};

	class physconstraints
	{
	public:
		static void read(auto& reader, PhysConstraints* asset);
	};

	class ragdoll
	{
	public:
		static void read(auto& reader, Ragdoll* asset);
	};

	class xanim_curve
	{
	public:
		static void read(auto& reader, XAnimCurve* asset);
	};
}
