#include <std_include.hpp>
#include "../../include.hpp"
#include "surface_type.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace surface_type
		{
			namespace
			{
				constexpr std::pair<zonetool::h1::materialSurfType_t, materialSurfType_t> sources[]
				{
					{zonetool::h1::SURF_TYPE_DEFAULT, SURF_TYPE_DEFAULT},
					{zonetool::h1::SURF_TYPE_BARK, SURF_TYPE_BARK},
					{zonetool::h1::SURF_TYPE_BRICK, SURF_TYPE_BRICK},
					{zonetool::h1::SURF_TYPE_CARPET, SURF_TYPE_CARPET},
					{zonetool::h1::SURF_TYPE_CLOTH, SURF_TYPE_CLOTH},
					{zonetool::h1::SURF_TYPE_CONCRETE, SURF_TYPE_CONCRETE},
					{zonetool::h1::SURF_TYPE_DIRT, SURF_TYPE_DIRT},
					{zonetool::h1::SURF_TYPE_FLESH, SURF_TYPE_FLESH},
					{zonetool::h1::SURF_TYPE_FOLIAGE_DEBRIS, SURF_TYPE_FOLIAGE},
					{zonetool::h1::SURF_TYPE_GLASS, SURF_TYPE_GLASS},
					{zonetool::h1::SURF_TYPE_GRASS, SURF_TYPE_GRASS},
					{zonetool::h1::SURF_TYPE_GRAVEL, SURF_TYPE_GRAVEL},
					{zonetool::h1::SURF_TYPE_ICE, SURF_TYPE_ICE},
					{zonetool::h1::SURF_TYPE_METAL_SOLID, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_METAL_GRATE, SURF_TYPE_METAL_GRATE},
					{zonetool::h1::SURF_TYPE_MUD, SURF_TYPE_MUD},
					{zonetool::h1::SURF_TYPE_PAPER, SURF_TYPE_PAPER},
					{zonetool::h1::SURF_TYPE_PLASTER, SURF_TYPE_PLASTER},
					{zonetool::h1::SURF_TYPE_ROCK, SURF_TYPE_ROCK},
					{zonetool::h1::SURF_TYPE_SAND, SURF_TYPE_SAND},
					{zonetool::h1::SURF_TYPE_SNOW, SURF_TYPE_SNOW},
					{zonetool::h1::SURF_TYPE_WATER_WAIST, SURF_TYPE_WATER},
					{zonetool::h1::SURF_TYPE_WOOD_SOLID, SURF_TYPE_WOOD},
					{zonetool::h1::SURF_TYPE_ASPHALT, SURF_TYPE_ASPHALT},
					{zonetool::h1::SURF_TYPE_CERAMIC, SURF_TYPE_CERAMIC},
					{zonetool::h1::SURF_TYPE_PLASTIC_SOLID, SURF_TYPE_PLASTIC},
					{zonetool::h1::SURF_TYPE_RUBBER, SURF_TYPE_RUBBER},
					{zonetool::h1::SURF_TYPE_FRUIT, SURF_TYPE_FRUIT},
					{zonetool::h1::SURF_TYPE_PAINTEDMETAL, SURF_TYPE_PAINTEDMETAL},
					{zonetool::h1::SURF_TYPE_RIOTSHIELD, SURF_TYPE_RIOTSHIELD},
					{zonetool::h1::SURF_TYPE_SLUSH, SURF_TYPE_SLUSH},
					{zonetool::h1::SURF_TYPE_ASPHALT_WET, SURF_TYPE_ASPHALT},
					{zonetool::h1::SURF_TYPE_ASPHALT_DEBRIS, SURF_TYPE_ASPHALT},
					{zonetool::h1::SURF_TYPE_CONCRETE_WET, SURF_TYPE_CONCRETE},
					{zonetool::h1::SURF_TYPE_CONCRETE_DEBRIS, SURF_TYPE_CONCRETE},
					{zonetool::h1::SURF_TYPE_FOLIAGE_VEGETATION, SURF_TYPE_FOLIAGE},
					{zonetool::h1::SURF_TYPE_FOLIAGE_LEAVES, SURF_TYPE_FOLIAGE},
					{zonetool::h1::SURF_TYPE_GRASS_TALL, SURF_TYPE_GRASS},
					{zonetool::h1::SURF_TYPE_METAL_HOLLOW, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_METAL_VEHICLE, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_METAL_THIN, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_METAL_WET, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_METAL_DEBRIS, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_PLASTIC_HOLLOW, SURF_TYPE_PLASTIC},
					{zonetool::h1::SURF_TYPE_PLASTIC_TARP, SURF_TYPE_PLASTIC},
					{zonetool::h1::SURF_TYPE_ROCK_WET, SURF_TYPE_ROCK},
					{zonetool::h1::SURF_TYPE_ROCK_DEBRIS, SURF_TYPE_ROCK},
					{zonetool::h1::SURF_TYPE_WATER_ANKLE, SURF_TYPE_WATER},
					{zonetool::h1::SURF_TYPE_WATER_KNEE, SURF_TYPE_WATER},
					{zonetool::h1::SURF_TYPE_WOOD_HOLLOW, SURF_TYPE_WOOD},
					{zonetool::h1::SURF_TYPE_WOOD_WET, SURF_TYPE_WOOD},
					{zonetool::h1::SURF_TYPE_WOOD_DEBRIS, SURF_TYPE_WOOD},
					{zonetool::h1::SURF_TYPE_CUSHION, SURF_TYPE_CLOTH},
				};
			}

			materialSurfType_t get_source(const zonetool::h1::materialSurfType_t type)
			{
				const auto source = std::ranges::find(sources, type, &std::pair<zonetool::h1::materialSurfType_t, materialSurfType_t>::first);
				return source != std::end(sources) ? source->second : SURF_TYPE_DEFAULT;
			}
		}
	}
}
