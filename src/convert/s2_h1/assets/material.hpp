#pragma once

namespace zonetool::s2::converter::h1::material
{
	using load_bits = std::array<std::uint32_t, 6>;

	struct conversion_stats
	{
		std::size_t source_images;
		std::size_t fallback_images;
		std::size_t missing_textures;
		std::size_t missing_constants;
		std::size_t missing_states;
		std::size_t multi_pass_techniques;
		std::unordered_set<std::string> missing_images;
	};

	struct stage_layout
	{
		std::vector<std::uint16_t> registers;
		std::uint16_t size;

		auto operator<=>(const stage_layout&) const = default;
	};

	struct default_texture
	{
		std::string_view name;
		zonetool::h1::IMG_TS semantic;
		std::string_view image;
	};

	struct buffer_layout
	{
		stage_layout vs;
		stage_layout ps;

		auto operator<=>(const buffer_layout&) const = default;
	};

	void dump(zonetool::s2::Material* asset);
	void finish();
	std::optional<std::uint8_t> find_camera_region(const std::string& name);
}
