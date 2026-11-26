#pragma once

namespace zonetool::s4::converter::iw8::xsurface
{
	std::uint32_t convert_vertex_color(std::uint8_t alpha);
	zonetool::iw8::XModelSurfs* convert(const XModelSurfs* asset, const std::vector<bool>& layer_masks, std::string& data, utils::memory::allocator& allocator);
	void set_layer_masks(const XModelSurfs* asset, std::vector<bool> layer_masks);
	void queue(const XModelSurfs* asset);
	void dump_queued();
}
