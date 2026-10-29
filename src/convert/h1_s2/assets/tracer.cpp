#include <std_include.hpp>

#include "../include.hpp"
#include "fxeffectdef.hpp"
#include "material.hpp"
#include "tracer.hpp"
#include "xmodel.hpp"

namespace zonetool::h1::converter::s2::tracer
{
	namespace
	{
		std::unordered_set<std::string> dumped_tracers;
	}

	const char* dump_used(const char* name)
	{
		const auto* loaded = db_find_x_asset_header_safe(ASSET_TYPE_TRACER, name).tracerDef;
		if (!loaded)
		{
			ZONETOOL_WARNING("tracer \"%s\" not found in the loaded h1 zones", name);
			return nullptr;
		}

		const auto* converted = xmodel::converted_name(zonetool::s2::ASSET_TYPE_TRACER, name); // most h1 tracer names are stock s2 tracers too
		if (!dumped_tracers.insert(converted).second)
		{
			return converted;
		}

		const auto* material = loaded->material;
		const auto* effect = loaded->effectDef;

		utils::memory::allocator allocator;
		auto* tracer = allocator.allocate<zonetool::s2::TracerDef>();
		tracer->name = converted;
		tracer->material = material && material->name && material::dump_used(material->name)
			? make_stub<zonetool::s2::Material>(material->name, allocator) : nullptr;
		tracer->effectDef = effect && effect->name && fxeffectdef::dump_used(effect->name)
			? make_stub<zonetool::s2::FxEffectDef>(effect->name, allocator) : nullptr;
		tracer->drawInterval = loaded->drawInterval;
		tracer->speed = loaded->speed;
		tracer->beamLength = loaded->beamLength;
		tracer->beamWidth = loaded->beamWidth;
		tracer->screwRadius = loaded->screwRadius;
		tracer->screwDist = loaded->screwDist;
		std::memcpy(tracer->colors, loaded->colors, sizeof(tracer->colors));
		zonetool::s2::tracer_def::dump(tracer);
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_TRACER), converted);
		return converted;
	}

	void clear()
	{
		dumped_tracers.clear();
	}
}
