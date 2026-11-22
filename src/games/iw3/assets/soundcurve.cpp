#include <std_include.hpp>
#include "soundcurve.hpp"

namespace zonetool::iw3
{
	void sound_curve::read(zone_reader32& reader, SndCurve* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->filename);
		reader.pop_stream();
	}
}
