#pragma once
#include "../zonetool.hpp"

namespace zonetool::h1
{
	class structured_data_def_set
	{
	private:
		std::string name_;
		StructuredDataDefSet* asset_ = nullptr;

	public:

		static void dump(StructuredDataDefSet* asset);
	};
}