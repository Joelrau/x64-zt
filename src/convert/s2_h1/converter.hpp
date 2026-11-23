#pragma once

#include "include.hpp"

#include "assets/gfximage.hpp"
#include "assets/gfxworld.hpp"
#include "assets/clipmap.hpp"
#include "assets/comworld.hpp"
#include "assets/fxeffectdef.hpp"
#include "assets/fxworld.hpp"
#include "assets/glassworld.hpp"
#include "assets/physworld.hpp"
#include "assets/mapents.hpp"
#include "assets/lightsets.hpp"
#include "assets/material.hpp"
#include "assets/rawfile.hpp"
#include "assets/scriptfile.hpp"
#include "assets/sound.hpp"
#include "assets/soundtables.hpp"
#include "assets/techset.hpp"
#include "assets/xsurface.hpp"
#include "assets/xmodel.hpp"
#include "assets/xanim.hpp"
#include "assets/scriptable.hpp"

namespace zonetool::s2::converter::h1
{
	struct csv_row
	{
		std::string type;
		std::string name;
		bool referenced;
	};
}
