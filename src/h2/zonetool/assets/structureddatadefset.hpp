#pragma once
#include "../zonetool.hpp"

namespace zonetool::h2
{
	class IStructuredDataDefSet
	{
	private:
		std::string name_;
		StructuredDataDefSet* asset_ = nullptr;

	public:

		static void dump(StructuredDataDefSet* asset);
	};
}