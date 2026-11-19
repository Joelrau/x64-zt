#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	template <typename Walk>
	void walk_lightset_probe_data(Walk walk, GfxDynamicLightsetProbeData& data)
	{
		walk.array(data.chunkG, data.numGChunk, 3);
		walk.array(data.chunkGBF, data.numGChunkBF, 1);
		walk.array(data.origProbes, data.numOrigProbes, 31);
		walk.array(data.bfProbes, data.numBFProbes, 31);
		walk.array(data.skipRecords, data.numSkipRecords, 1);
		walk.array(data.palletteRecords, data.numSkipRecords, 3);
		walk.array(data.tempRemap, data.numOrigProbes, 3);
	}

	class gfx_world_transient_zone : public asset_interface
	{
	private:
		std::string name_;
		GfxWorldTransientZone* asset_ = nullptr;

	public:
		GfxWorldTransientZone* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, GfxWorldTransientZone* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(GfxWorldTransientZone* asset);
	};
}
