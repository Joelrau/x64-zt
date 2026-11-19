#pragma once

#include <h1/zone/loader.hpp>

namespace zonetool::s2::converter::h1::techset
{
	struct h1_techset_source
	{
		std::filesystem::path zone;
		bool global;
	};

	struct techset_name
	{
		std::string name;
		std::string prefix;
		std::string kind;
		std::unordered_set<std::string> features;
	};

	struct catalog
	{
		std::unordered_map<std::string, h1_techset_source> sources;
		std::vector<techset_name> names;
		std::unordered_set<std::string> global_images;
		std::unordered_map<std::string, std::filesystem::path> shader_sources;
	};

	struct shader_key
	{
		zonetool::h1::XAssetType type;
		std::string name;
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
