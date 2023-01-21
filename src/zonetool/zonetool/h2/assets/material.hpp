#pragma once
#include "../zonetool.hpp"

namespace zonetool::h2
{
	class IMaterial : public IAsset
	{
	private:
		std::string name_;
		Material* asset_ = nullptr;

		std::vector<std::array<std::uint64_t, 10>> depth_stenchil_state_bits;
		std::vector<std::array<std::uint32_t, 3>> blend_state_bits;

		MaterialTextureDef* prase_texture_table(json& matdata, ZoneMemory* mem);

	public:
		Material* parse(std::string name, ZoneMemory* mem);

		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(Material* asset);
	};
}