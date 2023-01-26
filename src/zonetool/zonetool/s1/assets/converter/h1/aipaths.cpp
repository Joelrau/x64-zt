#include <std_include.hpp>
#include "aipaths.hpp"

#include "zonetool/s1/zonetool.hpp"
#include "zonetool/h1/assets/aipaths.hpp"

namespace zonetool::s1
{
	namespace converter::h1::aipaths
	{
		void dump(PathData* s1_asset, ZoneMemory* mem)
		{
			auto* asset = converter::h1::convert_path_data(s1_asset, mem);
			zonetool::h1::IAIPaths::dump(asset, reinterpret_cast<decltype(zonetool::h1::SL_ConvertToString.get())>(zonetool::s1::SL_ConvertToString.get()));
		}
	}
}