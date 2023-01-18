#pragma once

#include "game.hpp"
#include "structs.hpp"
#include <string>

namespace dvars
{
	std::string dvar_get_vector_domain(const int components, const game::dvar_limits& domain);
	std::string dvar_get_domain(const game::dvar_type type, const game::dvar_limits& domain);

	game::dvar_t* register_int(const std::string& name, int value, int min, int max,
		unsigned int flags, const std::string& description);
	game::dvar_t* register_bool(const std::string& name, bool value,
		unsigned int flags, const std::string& description);
	game::dvar_t* register_string(const std::string& name, const char* value, 
		unsigned int flags, const std::string& description);
	game::dvar_t* register_float(const std::string& name, float value, float min, float max,
		unsigned int flags, const std::string& description);
	game::dvar_t* register_vec4(const std::string& name, float x, float y, float z, float w, float min, 
		float max, unsigned int flags, const std::string& description);
}
