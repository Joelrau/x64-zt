#pragma once

namespace zonetool::iw8::converter::iw7::techset
{
	enum generated_technique : std::uint8_t
	{
		GENERATED_DEPTH_PREPASS,
		GENERATED_SHADOWMAP,
		GENERATED_LIT,
		GENERATED_UNLIT,
		GENERATED_SKY,
		GENERATED_DECAL,
		GENERATED_TECHNIQUE_COUNT,
	};

	enum generated_techset : std::uint8_t
	{
		GENERATED_TECHSET_WORLD,
		GENERATED_TECHSET_MODEL,
		GENERATED_TECHSET_SKY,
		GENERATED_TECHSET_DECAL,
		GENERATED_TECHSET_COUNT,
	};

	inline constexpr std::array<const char*, 4> material_constant_names{"uvTransform", "uvRotation", "colorTint", "surfRanges"};
	inline constexpr std::array<const char*, 5> decal_constant_names{"uvTransform", "uvRotation", "colorTint", "surfRanges", "decalParams"};
	inline constexpr std::array<const char*, 2> material_texture_names{"colorMap", "normalMap"};
	inline constexpr std::array<const char*, 3> decal_texture_names{"colorMap", "normalMap", "alphaRevealMap"};
	inline constexpr std::array<const char*, 1> sky_constant_names{"skyParams"};

	std::span<const char* const> get_constant_names(generated_techset techset);
	std::uint32_t hash_material_name(std::string_view name);
	const char* get_techset_name(generated_techset techset);
	std::optional<generated_technique> get_generated_technique(generated_techset techset, std::uint32_t technique);
	void dump_techset(generated_techset techset);
}
