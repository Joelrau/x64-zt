#include <std_include.hpp>
#include "dynmodel.hpp"

namespace zonetool::t9
{
	void dyn_model::read(auto& reader, DynModel* asset)
	{
		for (auto& state : asset->stateEffects)
		{
			reader.read_asset(ASSET_TYPE_FX, state.effect);
		}

		for (auto& effect : asset->effects1)
		{
			reader.read_asset(ASSET_TYPE_FX, effect);
		}

		for (auto& effect : asset->effects2)
		{
			reader.read_asset(ASSET_TYPE_FX, effect);
		}

		read_script_bundle_object(reader, asset->bundle);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->model1);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->model2);
		reader.read_asset(ASSET_TYPE_PHYSPRESET, asset->physPreset);
	}
}

namespace zonetool::t9
{
	template void dyn_model::read(zone_reader&, DynModel*);
	template void dyn_model::read(zone_writer&, DynModel*);
}
