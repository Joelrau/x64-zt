#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw6
{
	class IGfxImage : public IAsset
	{
	private:
		std::string name_;
		GfxImage* asset_ = nullptr;

	public:
		std::array<XStreamFile*, 4> image_stream_files;
		std::array<std::optional<std::string>, 4> image_stream_blocks;
		bool custom_streamed_image = false;	

		static GfxImage* parse_custom(const std::string& name, ZoneMemory* mem);

		GfxImage* parse_streamed_image(const std::string& name, ZoneMemory* mem);
		GfxImage* parse(const std::string& name, ZoneMemory* mem);

		void init(const std::string& name, ZoneMemory* mem) override;
		void init(void* asset, ZoneMemory* mem) override;

		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(GfxImage* asset);
	};
}