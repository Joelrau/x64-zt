#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	template <typename Walk, typename T>
	void walk_temp_array(Walk walk, T*& field, const std::size_t count, const std::uint32_t alignment)
	{
		walk.push_stream(XFILE_BLOCK_TEMP);
		walk.array(field, count, alignment);
		walk.keep_temp(field, count);
		walk.pop_stream();
	}

	template <typename Walk>
	void walk_lightset_probe_data(Walk walk, GfxDynamicLightsetProbeData& data)
	{
		walk.array(data.chunkG, data.numGChunk, 3);
		walk.array(data.chunkGBF, data.numGChunkBF, 1);
		walk_temp_array(walk, data.origProbes, data.numOrigProbes, 31);
		walk_temp_array(walk, data.bfProbes, data.numBFProbes, 31);
		walk_temp_array(walk, data.skipRecords, data.numSkipRecords, 1);
		walk_temp_array(walk, data.palletteRecords, data.numSkipRecords, 3);
		walk_temp_array(walk, data.tempRemap, data.numOrigProbes, 3);
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
