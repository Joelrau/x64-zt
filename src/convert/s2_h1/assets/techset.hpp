#pragma once

#include <h1/zone/loader.hpp>

namespace zonetool::s2::converter::h1::techset
{
	struct h1_techset_source
	{
		std::filesystem::path zone;
		bool global;
	};

	class h1_zone
	{
	public:
		explicit h1_zone(const std::filesystem::path& path);

		zonetool::h1::Material* find_donor(const std::string& techset) const;
		zonetool::h1::MaterialTechniqueSet* find_techset(const std::string& techset) const;
		database& db() const;

	private:
		std::shared_ptr<database> database_;
		std::unique_ptr<zonetool::h1::loaded_zone> zone_;
		std::unordered_map<std::string, zonetool::h1::Material*> donors_;
		std::unordered_map<std::string, zonetool::h1::MaterialTechniqueSet*> techsets_;
	};

	void set_h1_folder(const std::filesystem::path& folder);

	std::string select(const std::string& s2_techset);
	const h1_techset_source* find_source(const std::string& h1_techset);
	bool is_global_image(const std::string& name);

	void dump(zonetool::h1::MaterialTechniqueSet* asset, const std::filesystem::path& zone_path);
	void dump_missing_shaders();
}
