#pragma once

#include <utils/string.hpp>

namespace zonetool
{
	const char* strip_template(const std::string& function_name);
}

#define ZONETOOL_INFO(__FMT__, ...) \
	printf("[ INFO ][ %s ]: " __FMT__ "\n", zonetool::strip_template(__FUNCTION__), __VA_ARGS__)

#define ZONETOOL_ERROR(__FMT__, ...) \
	printf("[ ERROR ][ %s ]: " __FMT__ "\n", zonetool::strip_template(__FUNCTION__), __VA_ARGS__)

#define ZONETOOL_FATAL(__FMT__, ...) \
	throw std::runtime_error(utils::string::va(__FMT__, __VA_ARGS__))

#define ZONETOOL_WARNING(__FMT__, ...) \
	printf("[ WARNING ][ %s ]: " __FMT__ "\n", zonetool::strip_template(__FUNCTION__), __VA_ARGS__)
