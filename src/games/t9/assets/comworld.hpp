#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class com_world
	{
	public:
		static void read_primary_lights(auto& reader, ComPrimaryLight*& field, std::uint32_t count);

		template <typename T>
		static void read_record_groups(auto& reader, ComRecordGroup<T>*& field, const std::uint32_t count)
		{
			const auto groups = reader.read_array(field, 7, count);
			if (!groups)
			{
				return;
			}

			for (auto i = 0u; i < count; i++)
			{
				reader.read_array(groups[i].records, 3, groups[i].recordCount);
			}
		}

		static void read(auto& reader, ComWorld* asset);
		static void dump(ComWorld* asset);
	};
}
