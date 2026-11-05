#pragma once

namespace zonetool::s2::converter::h1::material
{
	struct converted_material
	{
		zonetool::h1::Material material;
		std::vector<zonetool::h1::MaterialTextureDef> textures;
		std::vector<zonetool::h1::MaterialConstantDef> constants;
		std::vector<zonetool::h1::GfxImage> images;
		std::vector<std::string> image_names;
	};

	struct conversion_stats
	{
		std::size_t source_images;
		std::size_t global_images;
		std::size_t fallback_images;
		std::size_t lost_images;
		std::unordered_set<std::string> missing_images;
	};

	void dump(zonetool::s2::Material* asset);
	void finish();
	std::optional<std::uint8_t> find_camera_region(const std::string& name);
}
