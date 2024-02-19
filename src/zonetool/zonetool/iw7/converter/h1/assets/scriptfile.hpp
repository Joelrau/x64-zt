#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace scriptfile
		{
			zonetool::h2::ScriptFile* convert(ScriptFile* asset, utils::memory::allocator& allocator);
			void dump(ScriptFile* asset);
		}
	}
}