#pragma once

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace scriptfile
		{
			zonetool::s1::ScriptFile* convert(ScriptFile* asset, utils::memory::allocator& allocator);
			void dump(ScriptFile* asset);
		}
	}
}
