#pragma once

#include <s2/zone/loader.hpp>

namespace zonetool::h1::converter::s2::stock_zone
{
	class s2_zone
	{
	public:
		explicit s2_zone(const std::filesystem::path& path);

		const std::vector<void*>& find_assets(zonetool::s2::XAssetType type) const;
		database& db() const;

	private:
		std::shared_ptr<database> database_;
		std::unique_ptr<zonetool::s2::loaded_zone> zone_;
		std::unordered_map<std::int32_t, std::vector<void*>> assets_;
	};

	const s2_zone& common_zone();
	bool is_stock_asset(zonetool::s2::XAssetType type, const char* name);
	const std::unordered_map<std::string, std::string>& common_rawfiles();
}
