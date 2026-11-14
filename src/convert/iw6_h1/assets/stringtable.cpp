#include <std_include.hpp>
#include "../include.hpp"
#include "stringtable.hpp"
#include "stringtable/soundtables.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace stringtable
		{
			void dump(StringTable* asset)
			{
				if (std::string_view(asset->name).starts_with("soundtables/"))
				{
					soundtables::dump(asset);
					return;
				}

				zonetool::h1::string_table::dump(reinterpret_cast<zonetool::h1::StringTable*>(asset));
			}
		}
	}
}
