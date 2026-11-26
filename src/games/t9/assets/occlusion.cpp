#include <std_include.hpp>
#include "occlusion.hpp"

namespace zonetool::t9
{
	void read_occlusion_buffer(auto& reader, CpuOcclusionBuffer& buffer)
	{
		reader.read_array(buffer.data, 0, buffer.dataSize);
		reader.read_array(buffer.data1, 3, buffer.count1);
		reader.read_array(buffer.data2, 3, buffer.count2);
	}

	void cpu_occlusion_data::read(auto& reader, CpuOcclusionData* asset)
	{
		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0ull; i < asset->entryCount; i++)
			{
				reader.read_array(entries[i].data1, 3, entries[i].count1);
				reader.read_array(entries[i].data2, 1, entries[i].count2);
				reader.read_array(entries[i].data3, 1, entries[i].count3);
			}
		}

		reader.read_array(asset->records1, 7, asset->records1Count);
		reader.read_array(asset->records2, 7, asset->records2Count);
		reader.read_array(asset->names, 3, asset->nameCount);
		read_occlusion_buffer(reader, asset->buffer1);
		read_occlusion_buffer(reader, asset->buffer2);

		reader.push_stream(XFILE_BLOCK_RUNTIME_VIRTUAL);
		reader.read_array(asset->runtimeData1, 63, static_cast<std::uint32_t>(asset->entryCount));
		reader.pop_stream();

		reader.push_stream(XFILE_BLOCK_RUNTIME_VIRTUAL);
		reader.read_array(asset->runtimeData2, 63, static_cast<std::uint32_t>(asset->entryCount));
		reader.pop_stream();
	}
}

namespace zonetool::t9
{
	template void read_occlusion_buffer(zone_reader&, CpuOcclusionBuffer&);
	template void cpu_occlusion_data::read(zone_reader&, CpuOcclusionData*);
	template void read_occlusion_buffer(zone_writer&, CpuOcclusionBuffer&);
	template void cpu_occlusion_data::read(zone_writer&, CpuOcclusionData*);
}
