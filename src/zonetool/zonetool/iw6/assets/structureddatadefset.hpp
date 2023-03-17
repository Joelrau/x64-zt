#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw6
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