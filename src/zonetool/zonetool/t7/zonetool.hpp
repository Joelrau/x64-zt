#pragma once

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include "game/t7/game.hpp"
#include "component/t7/command.hpp"
#include "component/t7/scheduler.hpp"

#include "structs.hpp"
#include "functions.hpp"
#include "variables.hpp"

#include "zonetool/utils/utils.hpp"
#include "zonetool/shared/shared.hpp"

//#include "assets/xmodel.hpp"

//#include "zone.hpp"

namespace zonetool::t7
{
	extern std::unordered_set<std::pair<std::uint32_t, std::string>, pair_hash<std::uint32_t, std::string>> ignore_assets;

	void initialize();
	void start();
}