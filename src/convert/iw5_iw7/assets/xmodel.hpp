#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace xmodel
	{
		zonetool::iw7::XModel* convert(zonetool::iw5::XModel* asset, utils::memory::allocator& allocator);
		int convert_surface_flags(int flags);
		void request_dynamic_box(const std::string& model, float mass);
		bool wants_dynamic_box(const std::string& model, float* mass);
		void dump(zonetool::iw5::XModel* asset);
	}
}
