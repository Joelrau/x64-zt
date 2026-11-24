#pragma once

namespace zonetool::iw7::converter::iw8::xmodel
{
	void dump(XModel* asset);
	std::string dump_static(XModel* asset);
	std::size_t dynamic_body_count(const XModel* asset);
}
