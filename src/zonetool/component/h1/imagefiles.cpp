#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "imagefiles.hpp"

#include "game/h1/game.hpp"

#include <utils/hook.hpp>

#include "zonetool/h1/structs.hpp"

namespace h1
{
	namespace imagefiles
	{
		namespace
		{
			void DB_CreateGfxImageStream_Internal(unsigned int imageIndex, zonetool::h1::GfxImage* image)
			{
				if (image->streamed)
				{
					for (auto i = 0; i < 4; i++)
					{
						utils::hook::invoke<void>(0x14028D720); // DB_GetNextImageStreamFile
					}
				}
			}
		}

		class component final : public component_interface
		{
		public:
			void post_load() override
			{
				utils::hook::jump(0x1402C5930, DB_CreateGfxImageStream_Internal);
			}
		};
	}
}

REGISTER_COMPONENT_H1(h1::imagefiles::component)