#pragma once
#include "../zonetool.hpp"

namespace zonetool::h1
{
	class IStructuredDataDefSet
	{
	private:
		std::string name_;
		StructuredDataDefSet* asset_ = nullptr;

	public:

		static void dump(StructuredDataDefSet* asset, const std::function<const char* (scr_string_t)>& convertToString = SL_ConvertToString.get());
	};
}