#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

namespace zonetool::convert::s1_s2
{
	namespace
	{
		namespace source = zonetool::s1;
		namespace target = zonetool::h1::converter::s2;

		utils::memory::allocator deferred_allocator;

		template <typename S>
		S* header_as(void* header)
		{
			return static_cast<S*>(header);
		}

		// S1 and H1 materials differ only in the size of the per-technique arrays, the S2 converter reads none of them
		zonetool::h1::Material* material_view(const source::Material* asset)
		{
			static_assert(sizeof(source::MaterialInfo) == sizeof(zonetool::h1::MaterialInfo));
			static_assert(sizeof(source::MaterialTextureDef) == sizeof(zonetool::h1::MaterialTextureDef));
			static_assert(sizeof(source::MaterialConstantDef) == sizeof(zonetool::h1::MaterialConstantDef));

			const auto view = deferred_allocator.allocate<zonetool::h1::Material>();
			std::memcpy(&view->info, &asset->info, sizeof(view->info));
			view->textureCount = asset->textureCount;
			view->constantCount = asset->constantCount;
			view->stateFlags = asset->stateFlags;
			view->layerCount = asset->layerCount;
			view->techniqueSet = reinterpret_cast<zonetool::h1::MaterialTechniqueSet*>(asset->techniqueSet);
			view->textureTable = reinterpret_cast<zonetool::h1::MaterialTextureDef*>(asset->textureTable);
			view->constantTable = reinterpret_cast<zonetool::h1::MaterialConstantDef*>(asset->constantTable);
			return view;
		}

		bool is_unnamed_key_line(const std::string& line)
		{
			return !line.empty() && std::isdigit(static_cast<unsigned char>(line.front())) && !line.starts_with("0 ");
		}

		// the S1 to H1 key conversion keeps numeric ids that S1 has no name for, the S2 converter would read them as H1 ids
		void remove_unnamed_keys(zonetool::h1::MapEnts* asset, utils::memory::allocator& allocator)
		{
			std::string buffer;
			for (const auto& line : utils::string::split(std::string{asset->entityString, static_cast<std::size_t>(asset->numEntityChars)}, '\n'))
			{
				if (is_unnamed_key_line(line))
				{
					ZONETOOL_WARNING("map_ents: S1 key id \"%s\" has no name, dropping it", line.data());
					continue;
				}

				buffer.append(line).append("\n");
			}

			asset->entityString = allocator.duplicate_string(buffer);
			asset->numEntityChars = static_cast<int>(buffer.size());
		}

		void dump_mapents(source::MapEnts* asset)
		{
			utils::memory::allocator allocator;
			const auto h1_asset = zonetool::s1::converter::h1::mapents::convert(asset, allocator);
			remove_unnamed_keys(h1_asset, allocator);
			target::mapents::dump(h1_asset);
		}

		void dump_gfxworld(source::GfxWorld* asset)
		{
			target::gfxworld::dump(zonetool::s1::converter::h1::gfxworld::convert(asset, deferred_allocator));
		}

		void dump_asset(const std::int32_t type, void* header)
		{
			switch (type)
			{
			case source::ASSET_TYPE_IMAGE:
				target::gfximage::dump(header_as<zonetool::h1::GfxImage>(header));
				break;
			case source::ASSET_TYPE_MATERIAL:
				target::material::dump(material_view(header_as<source::Material>(header)));
				break;
			case source::ASSET_TYPE_XMODEL:
				target::xmodel::dump(header_as<zonetool::h1::XModel>(header));
				break;
			case source::ASSET_TYPE_XMODEL_SURFS:
				target::xsurface::dump(header_as<zonetool::h1::XModelSurfs>(header));
				break;
			case source::ASSET_TYPE_CLIPMAP:
				target::clipmap::dump(header_as<zonetool::h1::clipMap_t>(header));
				break;
			case source::ASSET_TYPE_COMWORLD:
				target::comworld::dump(header_as<zonetool::h1::ComWorld>(header));
				break;
			case source::ASSET_TYPE_FXWORLD:
				target::fxworld::dump(header_as<zonetool::h1::FxWorld>(header));
				break;
			case source::ASSET_TYPE_GFXWORLD:
				dump_gfxworld(header_as<source::GfxWorld>(header));
				break;
			case source::ASSET_TYPE_GLASSWORLD:
				target::glassworld::dump(header_as<zonetool::h1::GlassWorld>(header));
				break;
			case source::ASSET_TYPE_MAP_ENTS:
				dump_mapents(header_as<source::MapEnts>(header));
				break;
			case source::ASSET_TYPE_PHYSWORLDMAP:
				target::physworld::dump(header_as<zonetool::h1::PhysWorld>(header));
				break;
			default:
				break;
			}
		}

		void setup(const std::filesystem::path& target_game_path)
		{
			target::set_s2_folder(target_game_path);
		}

		void finish(const std::string& zone_name)
		{
			target::finish(zone_name);
			deferred_allocator.clear();
		}
	}

	const converter& definition()
	{
		static const converter s1_s2
		{
			.source = "s1",
			.target = "s2",
			.dump_asset = dump_asset,
			.setup = setup,
			.finish = finish,
		};

		return s1_s2;
	}
}
