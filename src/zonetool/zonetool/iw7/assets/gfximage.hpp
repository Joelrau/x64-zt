#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw7
{
	class gfx_image : public asset_interface
	{
	private:
		std::string name_;
		GfxImage* asset_ = nullptr;

	public:
		std::array<XStreamFile*, 4> image_stream_files;
		std::array<std::optional<std::string>, 4> image_stream_blocks_paths;
		std::array<std::optional<std::string>, 4> image_stream_blocks;
		bool custom_streamed_image = false;	

		bool is_iwi = false;

		GfxImage* parse_custom(const std::string& name, zone_memory* mem);

		GfxImage* parse_streamed_image(const std::string& name, zone_memory* mem);
		GfxImage* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void init(void* asset, zone_memory* mem) override;

		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(GfxImage* asset);
	};
}