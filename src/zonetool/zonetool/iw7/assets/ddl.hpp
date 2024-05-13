#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw7
{
	class ddl : public asset_interface
	{
	private:
		std::string name_;
		DDLFile* asset_ = nullptr;

	public:
		static void dump(DDLFile* asset);
	};
}