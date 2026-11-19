#pragma once

#include <string>
#include <string_view>

namespace utils::shader_compiler
{
	std::string compile(std::string_view source, const char* name, const char* entry, const char* profile);
}
