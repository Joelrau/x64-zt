#include <std_include.hpp>
#include "../include.hpp"
#include "impactfx.hpp"
#include "common/surface_type.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace impactfx
		{
			namespace
			{
				// iw6 row of each h1 row: h1 adds shotgun energy (8) and energy beam (16), stock fills 17 like 0 and 18 like 12
				constexpr std::array<int, 19> source_rows{0, 1, 2, 3, 4, 5, 6, 7, 5, 8, 9, 10, 11, 12, 13, 14, 0, 0, 11};
			}

			zonetool::h1::FxImpactTable* convert(FxImpactTable* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::FxImpactTable>();
				new_asset->name = asset->name;

				if (!asset->table)
				{
					return new_asset;
				}

				new_asset->table = allocator.allocate_array<zonetool::h1::FxImpactEntry>(source_rows.size());
				for (auto row = 0u; row < source_rows.size(); row++)
				{
					const auto& entry = asset->table[source_rows[row]];
					auto& new_entry = new_asset->table[row];
					for (auto type = 0; type < zonetool::h1::SURF_TYPE_COUNT; type++)
					{
						const auto source = surface_type::get_source(static_cast<zonetool::h1::materialSurfType_t>(type));
						new_entry.nonflesh[type] = reinterpret_cast<zonetool::h1::FxEffectDef*>(entry.nonflesh[source]);
					}

					for (auto i = 0u; i < std::size(entry.flesh); i++)
					{
						new_entry.flesh[i] = reinterpret_cast<zonetool::h1::FxEffectDef*>(entry.flesh[i]);
					}
				}

				return new_asset;
			}

			void dump(FxImpactTable* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::impact_fx::dump(converted_asset);
			}
		}
	}
}
