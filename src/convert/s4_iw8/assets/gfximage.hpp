#pragma once

namespace zonetool::s4::converter::iw8::gfximage
{
	std::optional<zonetool::iw8::GfxPixelFormat> convert_format(GfxPixelFormat format);
	zonetool::iw8::GfxImage* convert(const GfxImage* asset, utils::memory::allocator& allocator);
	zonetool::iw8::GfxImage* image_reference(const GfxImage* asset, utils::memory::allocator& allocator);
	zonetool::iw8::GfxImage* dump_alpha_reveal(const GfxImage* alpha, const GfxImage* reveal, utils::memory::allocator& allocator);
	bool dump(GfxImage* asset);
	bool dump_used(const GfxImage* asset);
}
