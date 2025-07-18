#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/iw7/game.hpp"

#include <utils/hook.hpp>

#include "zonetool/iw7/structs.hpp"

namespace iw7
{
	namespace imagefiles
	{
		namespace
		{
			void DB_CreateGfxImageStream_Internal(unsigned int imageIndex, zonetool::iw7::GfxImage* image)
			{
				if (image->streamed)
				{
					for (auto i = 0; i < 4; i++)
					{
						utils::hook::invoke<void>(0x1409E7450); // DB_GetNextImageStreamFile
					}
				}
			}
		}

		class component final : public component_interface
		{
		public:
			void post_unpack() override
			{
				utils::hook::jump(0x140A7DB10, DB_CreateGfxImageStream_Internal);
			}
		};
	}
}

REGISTER_COMPONENT_IW7(iw7::imagefiles::component)