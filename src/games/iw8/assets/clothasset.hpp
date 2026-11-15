#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class cloth_asset
	{
	public:
		static void read(zone_reader& reader, ClothAsset* asset);
	};
}
