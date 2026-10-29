#pragma once

#include <s2/zone/loader.hpp>

namespace zonetool::h1::converter::s2::techset
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

	struct native_techset
	{
		zonetool::s2::MaterialTechniqueSet* techset;
		std::array<std::int32_t, zonetool::s2::technique_count> sources;
		std::array<std::int32_t, zonetool::s2::technique_count> light_sources;
	};

	const s2_zone& common_zone();
	bool is_stock_asset(zonetool::s2::XAssetType type, const char* name);
	const std::unordered_map<std::string, std::string>& common_rawfiles();
	const native_techset* convert_native(const std::string& h1_techset, bool sprite = false);

	void dump(zonetool::s2::MaterialTechniqueSet* asset);
	void clear();
}
