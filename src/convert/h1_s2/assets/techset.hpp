#pragma once

#include <s2/zone/loader.hpp>

namespace zonetool::h1::converter::s2::techset
{
	// a stock map, for the assets of the common zones it references, its reverb presets and minimap materials
	constexpr std::string_view stock_zone_name = "mp_aachen_v2";

	class s2_zone
	{
	public:
		explicit s2_zone(const std::filesystem::path& path);

		bool is_global_techset(const std::string& techset) const;
		bool is_global_image(const std::string& image) const;
		bool is_global_asset(const void* header) const;
		const std::vector<void*>& find_assets(zonetool::s2::XAssetType type) const;
		database& db() const;

	private:
		std::shared_ptr<database> database_;
		std::unique_ptr<zonetool::s2::loaded_zone> zone_;
		std::unordered_set<std::string> global_techsets_;
		std::unordered_set<std::string> global_images_;
		std::unordered_set<const void*> global_assets_;
		std::unordered_map<std::int32_t, std::vector<void*>> assets_;
	};

	struct native_techset
	{
		zonetool::s2::MaterialTechniqueSet* techset;
		std::array<std::int32_t, zonetool::s2::technique_count> sources;
		std::array<std::int32_t, zonetool::s2::technique_count> light_sources; // h1 light technique of the appended primary light pass
	};

	const s2_zone& stock_zone();
	const std::unordered_map<std::string, std::string>& common_rawfiles(); // s2 common_mp rawfiles by name, decompressed
	const native_techset* convert_native(const std::string& h1_techset, bool sprite = false);

	void dump(zonetool::s2::MaterialTechniqueSet* asset);
	void clear();
}
