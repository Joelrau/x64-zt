#pragma once

namespace zonetool::iw7::converter::iw8::gradingclut
{
	struct lmh_band
	{
		std::array<float, 3> scale{1.0f, 1.0f, 1.0f};
		std::array<float, 3> balance{};
		float saturation{};
	};

	struct filmic_curve
	{
		float black_point{};
		float white_point{1024.0f};
		float crossover{2.0f};
		float shoulder{0.95f};
		float toe{0.2f};
	};

	struct grading // TODO: defaults for a missing vision key are not proven
	{
		filmic_curve filmic{};
		float curve_low_min{};
		float curve_low_max{1.0f};
		float curve_high_min{1.0f};
		float curve_high_max{};
		float curve_mid_midpoint{0.5f};
		float curve_mid_range{};
		float curve_mid_falloff{0.5f};
		std::array<lmh_band, 3> bands{}; // low, mid, high
		float preserve_luminance{};
		float levels_in_min{};
		float levels_in_gamma{1.0f};
		float levels_in_max{255.0f};
		float levels_out_min{};
		float levels_out_max{255.0f};
	};

	std::optional<std::string> dump(const GfxImage* clut, const grading& values);
}
