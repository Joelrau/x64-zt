#pragma once

#include "game.hpp"
#include "structs.hpp"

namespace iw7
{
	namespace dvars
	{
		std::string dvar_get_vector_domain(const int components, const game::DvarLimits& domain);
		std::string dvar_get_domain(const game::DvarType type, const game::DvarLimits& domain);
		std::string dvar_get_name(const game::dvar_t* dvar);
		std::string dvar_get_description(const game::dvar_t* dvar);
		void dvar_set_name(const game::dvar_t* dvar, const std::string& name);
		void dvar_set_description(const game::dvar_t* dvar, const std::string& description);
	}

}