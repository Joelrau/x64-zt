#pragma once

namespace zonetool::s2::cast_xmodel
{
	XModel* parse(const std::string& name, zone_memory* mem);
	XModelBase* find_base(const std::string& name, zone_memory* mem);
	XModelSurfs* find_surfs(const std::string& name, zone_memory* mem);
	void reset();

	void dump(XModel* model);
}
