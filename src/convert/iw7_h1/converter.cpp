#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

namespace zonetool::convert::iw7_h1
{
	namespace
	{
		template <typename S>
		S* header_as(void* header)
		{
			return static_cast<S*>(header);
		}

		void dump_asset(const std::int32_t type, void* header)
		{
			namespace source = zonetool::iw7;
			namespace target = zonetool::iw7::converter::h1;

			switch (type)
			{
			case source::ASSET_TYPE_IMAGE:
				target::gfximage::dump(header_as<source::GfxImage>(header));
				break;
			case source::ASSET_TYPE_MATERIAL:
				target::material::dump(header_as<source::Material>(header));
				break;
			case source::ASSET_TYPE_XMODEL:
				target::xmodel::dump(header_as<source::XModel>(header));
				break;
			case source::ASSET_TYPE_XMODEL_SURFS:
				target::xsurface::dump(header_as<source::XModelSurfs>(header));
				break;
			default:
				break;
			}
		}
	}

	const converter& definition()
	{
		static const converter iw7_h1
		{
			.source = "iw7",
			.target = "h1",
			.dump_asset = dump_asset,
		};

		return iw7_h1;
	}
}
