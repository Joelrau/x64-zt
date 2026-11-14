#include <std_include.hpp>

#include "../../include.hpp"
#include "stock_zone.hpp"

#include <utils/compression.hpp>

namespace zonetool::h1::converter::s2::stock_zone
{
	s2_zone::s2_zone(const std::filesystem::path& path)
		: database_(db_create())
	{
		db_scope _(*this->database_);
		this->zone_ = zonetool::s2::load_zone(path);

		for (const auto& asset : this->zone_->assets)
		{
			if (!asset.referenced)
			{
				this->assets_[asset.type].emplace_back(asset.header);
			}
		}
	}

	const std::vector<void*>& s2_zone::find_assets(const zonetool::s2::XAssetType type) const
	{
		static const std::vector<void*> none;
		const auto entry = this->assets_.find(type);
		return entry != this->assets_.end() ? entry->second : none;
	}

	database& s2_zone::db() const
	{
		return *this->database_;
	}

	const std::unordered_map<std::string, std::string>& common_rawfiles()
	{
		static const auto rawfiles = []
		{
			std::unordered_map<std::string, std::string> result;
			for (const auto* asset : common_zone().find_assets(zonetool::s2::ASSET_TYPE_RAWFILE))
			{
				const auto* rawfile = static_cast<const zonetool::s2::RawFile*>(asset);
				if (!rawfile->name || !rawfile->buffer)
				{
					continue;
				}

				result.try_emplace(rawfile->name, rawfile->compressedLen > 0
					? utils::compression::zlib::decompress({rawfile->buffer, static_cast<std::size_t>(rawfile->compressedLen)})
					: std::string(rawfile->buffer, rawfile->len));
			}

			return result;
		}();

		return rawfiles;
	}

	const s2_zone& common_zone()
	{
		static const s2_zone zone(s2_folder() / "zone" / "common_mp.ff");
		return zone;
	}

	bool is_stock_asset(const zonetool::s2::XAssetType type, const char* name)
	{
		db_scope _(common_zone().db());
		return zonetool::s2::db_find_x_asset_header_safe(type, name).data != nullptr;
	}
}
