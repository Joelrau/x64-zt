#include <std_include.hpp>
#include "reticle.hpp"

namespace zonetool::iw8
{
	void reticle_def::read(zone_reader& reader, ReticleDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->widgetName);
		reader.read_string(asset->secondaryWidgetName);
		reader.pop_stream();
	}
}
