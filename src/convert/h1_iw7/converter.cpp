#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

namespace zonetool::convert::h1_iw7
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
			namespace source = zonetool::h1;
			namespace target = zonetool::h1::converter::iw7;

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
			case source::ASSET_TYPE_PATHDATA:
				target::path_data::dump(header_as<source::PathData>(header));
				break;
			case source::ASSET_TYPE_CLIPMAP:
				target::clip_map::dump(header_as<source::clipMap_t>(header));
				break;
			case source::ASSET_TYPE_COMWORLD:
				target::com_world::dump(header_as<source::ComWorld>(header));
				break;
			case source::ASSET_TYPE_FXWORLD:
				target::fx_world::dump(header_as<source::FxWorld>(header));
				break;
			case source::ASSET_TYPE_GFXWORLD:
				target::gfx_world::dump(header_as<source::GfxWorld>(header));
				break;
			case source::ASSET_TYPE_GLASSWORLD:
				target::glass_world::dump(header_as<source::GlassWorld>(header));
				break;
			case source::ASSET_TYPE_LOCALIZE_ENTRY:
				source::localize::dump(header_as<source::LocalizeEntry>(header));
				break;
			case source::ASSET_TYPE_RAWFILE:
				source::rawfile::dump(header_as<source::RawFile>(header));
				break;
			case source::ASSET_TYPE_STRINGTABLE:
				source::string_table::dump(header_as<source::StringTable>(header));
				break;
			case source::ASSET_TYPE_STRUCTURED_DATA_DEF:
				source::structured_data_def_set::dump(header_as<source::StructuredDataDefSet>(header));
				break;
			case source::ASSET_TYPE_TTF:
				source::ttf_def::dump(header_as<source::TTFDef>(header));
				break;
			default:
				break;
			}
		}
	}

	const converter& definition()
	{
		static const converter h1_iw7
		{
			.source = "h1",
			.target = "iw7",
			.dump_asset = dump_asset,
		};

		return h1_iw7;
	}
}
