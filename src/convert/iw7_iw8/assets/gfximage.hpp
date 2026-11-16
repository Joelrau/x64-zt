#pragma once

namespace zonetool::iw7::converter::iw8::gfximage
{
	zonetool::iw8::GfxImage* dump_color(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_color_specular(const GfxImage* asset);
	zonetool::iw8::GfxImage* dump_normal_gloss(const GfxImage* asset);
}
