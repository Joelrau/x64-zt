#pragma once

namespace zonetool::iw7::converter::iw8::xmodel
{
	void dump(XModel* asset);
	bool dump_with_attachment(XModel* asset, XModel* attachment);
	std::string dump_static(XModel* asset);
	std::size_t dynamic_body_count(const XModel* asset);
	zonetool::iw8::PhysicsAsset* dump_physics_asset(const PhysicsAsset* asset, utils::memory::allocator& allocator);
}
