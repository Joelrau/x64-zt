#pragma once

namespace zonetool::s4::converter::iw8::physics
{
	unsigned int convert_contents(unsigned int contents);
	zonetool::iw8::PhysicsAsset* convert(const PhysicsAsset* asset, utils::memory::allocator& allocator);
	void dump(PhysicsAsset* asset);
	void dump(XModelDetailCollision* asset);
	void dump(PhysicsFXShape* asset);
}
