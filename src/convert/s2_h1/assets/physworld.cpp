#include <std_include.hpp>

#include "../include.hpp"
#include "physworld.hpp"

#include <h1/assets/physworld.hpp>

namespace zonetool::s2::converter::h1::physworld
{
	void dump(PhysWorld* asset)
	{
		std::vector<zonetool::h1::PhysBrushModel> brush_models(asset->modelCount);
		for (auto& model : brush_models)
		{
			model.fields.polytopeIndex = -1;
			model.fields.unk = -1;
			model.fields.worldIndex = 0;
			model.fields.meshIndex = -1;
		}

		zonetool::h1::PhysWorld new_asset{};
		new_asset.name = asset->name;
		new_asset.brushModelCount = asset->modelCount;
		new_asset.brushModels = brush_models.data();
		zonetool::h1::phys_world::dump(&new_asset);
		add_csv_row("phys_worldmap", new_asset.name);
	}
}
