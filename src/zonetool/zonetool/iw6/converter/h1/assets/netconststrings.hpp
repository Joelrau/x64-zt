#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace net_const_strings
		{
			zonetool::h1::ScriptFile* convert(NetConstStrings* asset, utils::memory::allocator& allocator);
			void dump(NetConstStrings* asset);
		}
	}
}