#pragma once

#include <s2/zone/loader.hpp>

namespace zonetool::h1::converter::s2::techset
{
	constexpr std::string_view donor_zone_name = "mp_aachen_v2";

	class s2_zone
	{
	public:
		explicit s2_zone(const std::filesystem::path& path);

		zonetool::s2::Material* find_donor(const std::string& techset) const;
		zonetool::s2::MaterialTechniqueSet* find_techset(const std::string& techset) const;
		bool is_global_techset(const std::string& techset) const;
		bool is_global_image(const std::string& image) const;
		std::vector<std::string> donor_techsets() const;
		zonetool::s2::XModelBase* model_base_donor() const;
		void* find_asset(zonetool::s2::XAssetType type) const;
		const std::vector<void*>& find_assets(zonetool::s2::XAssetType type) const;
		database& db() const;

	private:
		std::shared_ptr<database> database_;
		std::unique_ptr<zonetool::s2::loaded_zone> zone_;
		std::unordered_map<std::string, zonetool::s2::Material*> donors_;
		std::unordered_map<std::string, zonetool::s2::MaterialTechniqueSet*> techsets_;
		std::unordered_set<std::string> global_techsets_;
		std::unordered_set<std::string> global_images_;
		zonetool::s2::XModelBase* model_base_donor_{};
		std::unordered_map<std::int32_t, std::vector<void*>> assets_;
	};

	const s2_zone& donor_zone();
	std::string select(const std::string& h1_techset);
	void dump(zonetool::s2::MaterialTechniqueSet* asset);
}
