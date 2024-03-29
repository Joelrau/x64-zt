#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw6
{
	class material : public asset_interface
	{
	private:
		std::string name_;
		Material* asset_ = nullptr;

		std::vector<std::array<std::uint64_t, 11>> depth_stenchil_state_bits;
		std::vector<std::array<std::uint32_t, 1>> blend_state_bits;

		MaterialTextureDef* parse_texture_table(json& matdata, zone_memory* mem);

	public:
		static std::unordered_map<GfxImage*, std::string> fixed_nml_images_map;

		Material* parse(std::string name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(Material* asset);
	};
}