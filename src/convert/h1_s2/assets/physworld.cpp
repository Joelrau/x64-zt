#include <std_include.hpp>

#include "../include.hpp"
#include "physworld.hpp"

namespace zonetool::h1::converter::s2::physworld
{
	namespace target = zonetool::s2;

	// stock S2 models are {int polytope_start, u16 polytope_count, u16 mesh}. A model without a shape has
	// polytope_start -1, polytope_count 0 and mesh 0xffff. The engine reads all polytopes of model 0 (0x631F89).
	constexpr std::uint64_t empty_model = 0xFFFF0000FFFFFFFF;

	// TODO: polytopes and meshes are not converted, brush models have no physics shape
	void dump(PhysWorld* asset)
	{
		if (asset->polytopeCount || asset->meshDataCount || asset->waterVolumeDefCount)
		{
			ZONETOOL_WARNING("PhysWorld \"%s\": %u polytopes, %u meshes and %u water volumes are not converted",
				asset->name, asset->polytopeCount, asset->meshDataCount, asset->waterVolumeDefCount);
		}

		std::vector<std::uint64_t> models(asset->brushModelCount, empty_model);

		target::PhysWorld new_asset{};
		new_asset.name = asset->name;
		new_asset.modelCount = asset->brushModelCount;
		new_asset.models = models.empty() ? nullptr : models.data();
		target::phys_worldmap::dump(&new_asset);
		add_csv_row("phys_worldmap", new_asset.name);
	}
}
