#pragma once

namespace zonetool::iw8::converter::iw7::shader_compiler
{
	std::string compile(std::string_view source, const char* name, const char* entry, const char* profile);
}
