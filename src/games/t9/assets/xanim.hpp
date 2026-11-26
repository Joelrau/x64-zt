#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class xanim
	{
	public:
		static void read(auto& reader, XAnim* asset);
	};

	class sanim
	{
	public:
		static void read(auto& reader, SAnim* asset);
	};

	class xcam
	{
	public:
		static void read(auto& reader, XCam* asset);
	};
}
