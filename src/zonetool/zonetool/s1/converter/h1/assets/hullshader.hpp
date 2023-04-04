#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace hullshader
		{
			zonetool::h1::MaterialHullShader* convert(MaterialHullShader* asset, zone_memory* mem);
			void dump(MaterialHullShader* asset, zone_memory* mem);
		}
	}
}