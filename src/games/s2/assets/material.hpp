#pragma once

namespace zonetool::s2
{
	struct material_state
	{
		std::uint64_t depth_stencil[12];
		std::uint32_t blend[3];
	};

	class material : public asset_base<Material, ASSET_TYPE_MATERIAL>
	{
	public:
		static void read(zone_reader& reader, Material* asset);

		Material* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(Material* asset);

	private:
		material_state* states_{};
	};
}
