#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace scriptfile
		{
			zonetool::h1::ScriptFile* convert(ScriptFile* asset, utils::memory::allocator& allocator);
			void dump(ScriptFile* asset);
		}
	}
}
