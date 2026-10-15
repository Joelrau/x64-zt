#pragma once

namespace zonetool::s2
{
	struct vehicle_layout
	{
		template <typename V>
		static void visit(const V& v)
		{
			v.strings({0, 16, 32});
			for (auto i = 0u; i < 18; i++)
			{
				v.script_string(340 + i * 4);
			}

			const auto physics = v.sub(416);
			physics.asset(ASSET_TYPE_PHYSPRESET, 8);
			physics.string(16);
			physics.bytes(24, 3, 0x24);
			physics.string(32);

			v.assets(ASSET_TYPE_FX, 840, 5);
			v.assets(ASSET_TYPE_FX, 1048, 2);
			v.strings({1376, 2032});
			v.asset(ASSET_TYPE_WEAPON, 2040);
			v.sounds(2152, 2);
			for (auto i = 0u; i < 4; i++)
			{
				v.script_string(2188 + i * 4);
			}
			v.assets(ASSET_TYPE_FX, 2208, 2);
			v.assets(ASSET_TYPE_MATERIAL, 2224, 2);
			v.assets(ASSET_TYPE_XANIM, 2248, 2);
			v.sounds(2272, 5);
			v.script_string(2316);
			v.sounds(2320, 4);
			v.script_string(2356);
			v.sounds(2360, 3);
			v.sounds(2392, 4);
			for (const auto offset : {2432u, 2448u, 2464u, 2480u, 2496u})
			{
				v.sound(offset);
			}
			v.string(2512);
			v.sounds(2520, 62);
			v.string(3032);
		}
	};

	class vehicle_def : public field_layout_asset<VehicleDef, ASSET_TYPE_VEHICLE, vehicle_layout>
	{
	};
}
