#pragma once

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace scriptfile
		{
			zonetool::h1::ScriptFile* convert(zonetool::h2::ScriptFile* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h2::ScriptFile* asset);
		}
	}
}