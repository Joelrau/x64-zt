#include <std_include.hpp>

#include "../include.hpp"
#include "gfximage.hpp"
#include "gradingclut.hpp"

#include <iw8/assets/gradingclut.hpp>

namespace zonetool::iw7::converter::iw8::gradingclut
{
	namespace
	{
		using color = std::array<double, 3>;
		using matrix = std::array<color, 3>;

		color multiply(const matrix& m, const color& v)
		{
			color result{};
			for (auto i = 0u; i < 3; i++)
			{
				result[i] = m[i][0] * v[0] + m[i][1] * v[1] + m[i][2] * v[2];
			}

			return result;
		}

		color clamp_min(color v, const double minimum)
		{
			for (auto& channel : v)
			{
				channel = std::max(channel, minimum);
			}

			return v;
		}

		double luminance(const color& v)
		{
			return 0.2627 * v[0] + 0.678 * v[1] + 0.0593 * v[2];
		}

		double pq_decode(const double value)
		{
			const auto p = std::pow(std::min(value, 1.0), 1.0 / 78.84375);
			return std::pow(std::max(p - 0.8359375, 0.0) / (18.8515625 - 18.6875 * p), 1.0 / 0.1593017578125);
		}

		double pq_encode(const double value)
		{
			const auto p = std::pow(std::max(value, 0.0), 0.1593017578125);
			return std::pow((0.8359375 + 18.8515625 * p) / (1.0 + 18.6875 * p), 78.84375);
		}

		double evaluate_spline(const std::array<double, 6>& knots, const double position)
		{
			const auto index = std::clamp(static_cast<int>(position), 0, 3);
			const auto t = position - index;
			const auto c0 = knots[index];
			const auto c1 = knots[index + 1];
			const auto c2 = knots[index + 2];
			return t * t * (0.5 * c0 - c1 + 0.5 * c2) + t * (c1 - c0) + 0.5 * (c0 + c1);
		}

		double segmented_spline_c5(const double value)
		{
			const std::array<double, 6> low{-4.0, -4.0, -3.157377, -0.485250, 1.847732, 1.847732};
			const std::array<double, 6> high{-0.718548, 2.081031, 3.668124, 4.0, 4.0, 4.0};

			const auto log_x = std::log2(std::max(value, 1e-30));
			const auto log_y = log_x <= -17.473930 ? -4.0
				: log_x < -2.473931 ? evaluate_spline(low, (log_x * 0.301030 + 5.260177) * 0.664386)
				: log_x < 15.526069 ? evaluate_spline(high, (log_x * 0.301030 + 0.744727) * 0.553655)
				: 4.0;
			return std::exp2(log_y * 3.321928);
		}

		color apply_aces_rrt(const color& input)
		{
			const matrix to_aces{{{0.679086, 0.157701, 0.163213}, {0.046002, 0.859054, 0.094943}, {-0.000574, 0.028468, 0.972106}}};
			const matrix to_ap1{{{1.451439, -0.236511, -0.214929}, {-0.076554, 1.176230, -0.099676}, {0.008316, -0.006032, 0.997716}}};
			const matrix desaturate{{{0.970889, 0.026963, 0.002148}, {0.010889, 0.986963, 0.002148}, {0.010889, 0.026963, 0.962148}}};
			const matrix to_xyz{{{0.695452, 0.140679, 0.163869}, {0.044795, 0.859671, 0.095534}, {-0.005526, 0.004025, 1.001501}}};
			const matrix to_output{{{1.490409, -0.266171, -0.224239}, {-0.080168, 1.182167, -0.102000}, {0.003228, -0.034776, 1.031549}}};

			auto aces = multiply(to_aces, input);
			const auto& [r, g, b] = aces;
			const auto maximum = std::max({r, g, b});
			const auto minimum = std::min({r, g, b});
			const auto saturation = (std::max(maximum, 0.0) - std::max(minimum, 0.0)) / std::max(maximum, 0.01);
			const auto chroma = std::sqrt(std::max(b * (b - g) + g * (g - r) + r * (r - b), 0.0));
			const auto sum = r + g + b + 1.75 * chroma;
			const auto s = 2.5 * (saturation - 0.4);
			const auto t = std::max(1.0 - std::abs(s), 0.0);
			const auto gain = 0.025 * (1.0 + (s > 0.0 ? 1.0 : s < 0.0 ? -1.0 : 0.0) * (1.0 - t * t));
			const auto glow = sum <= 0.16 ? gain : sum >= 0.48 ? 0.0 : gain * (0.08 / (sum / 3.0) - 0.5);
			for (auto& channel : aces)
			{
				channel = std::max(channel * (1.0 + glow), 0.0); // TODO: red modifier omitted, CB0[1].w assumed 0
			}

			auto curve = multiply(desaturate, clamp_min(multiply(to_ap1, aces), 0.0));
			for (auto& channel : curve)
			{
				channel = segmented_spline_c5(std::max(channel, 0.0));
			}

			return multiply(to_output, multiply(to_xyz, curve));
		}

		color apply_display(color value)
		{
			const matrix to_bt709{{{1.6605, -0.5877, -0.0728}, {-0.1246, 1.1330, -0.0084}, {-0.0182, -0.1006, 1.1187}}};

			value = clamp_min(value, 0.0); // TODO: CB0[4].w matrix assumed off
			for (auto& channel : value)
			{
				channel /= 333.0; // TODO: CB0[3].x 333 unproven
			}

			color shoulder{};
			for (auto i = 0u; i < 3; i++)
			{
				shoulder[i] = value[i] < 0.15 ? value[i] : 0.15 + 0.85 * (1.0 - std::exp2(-1.697288 * (value[i] - 0.15)));
			}

			const auto input_luminance = luminance(value);
			if (input_luminance > 0.0)
			{
				const auto scale = luminance(shoulder) / input_luminance;
				for (auto& channel : value)
				{
					channel *= scale;
				}
			}

			const auto scaled_luminance = luminance(value);
			auto desaturation = input_luminance < 2.5 ? 0.2 * input_luminance
				: 0.5 + 0.5 * (1.0 - std::exp2((0.2 * input_luminance - 0.5) * -2.88539));
			desaturation = (desaturation * 1.1 - 0.1) * 0.8;

			auto excess = 0.5;
			if (scaled_luminance <= 0.0)
			{
				excess = 0.0;
			}
			else if (scaled_luminance < 1.0)
			{
				excess = 0.0;
				for (const auto channel : multiply(to_bt709, value))
				{
					const auto distance = channel - scaled_luminance;
					excess = std::max(excess, distance > 0.0 ? (std::max(channel, 1.0) - 1.0) / distance : 0.0);
				}

				excess *= 0.5;
			}

			desaturation = std::max(desaturation, excess);
			for (auto& channel : value)
			{
				channel += desaturation * (scaled_luminance - channel);
			}

			auto output = multiply(to_bt709, value);
			for (auto& channel : output)
			{
				channel = std::pow(std::clamp(channel, 0.0, 1.0), 1.0 / 2.2); // TODO: display mode 0 unproven
			}

			return output;
		}

		color apply_tonemap(const color& value)
		{
			return apply_display(apply_aces_rrt(value));
		}

		color lookup_iw7_clut(const std::vector<std::uint8_t>& texels, const std::size_t size, const color& value) // TODO: display encoded input assumed
		{
			std::array<std::size_t, 3> lower{};
			std::array<std::size_t, 3> upper{};
			color fraction{};
			for (auto i = 0u; i < 3; i++)
			{
				const auto position = std::clamp(value[i], 0.0, 1.0) * (size - 1);
				lower[i] = std::min(static_cast<std::size_t>(position), size - 1);
				upper[i] = std::min(lower[i] + 1, size - 1);
				fraction[i] = position - lower[i];
			}

			color result{};
			for (auto corner = 0u; corner < 8; corner++)
			{
				const auto r = corner & 1 ? upper[0] : lower[0];
				const auto g = corner & 2 ? upper[1] : lower[1];
				const auto b = corner & 4 ? upper[2] : lower[2];
				const auto weight = (corner & 1 ? fraction[0] : 1.0 - fraction[0]) * (corner & 2 ? fraction[1] : 1.0 - fraction[1])
					* (corner & 4 ? fraction[2] : 1.0 - fraction[2]);
				const auto* texel = &texels[((b * size + g) * size + r) * 4];
				for (auto i = 0u; i < 3; i++)
				{
					result[i] += weight * texel[i] / 255.0;
				}
			}

			return result;
		}

		double get_error(const color& log_value, const color& start, const color& target)
		{
			const auto value = apply_tonemap({std::exp(log_value[0]), std::exp(log_value[1]), std::exp(log_value[2])});
			auto error = 0.0;
			for (auto i = 0u; i < 3; i++)
			{
				error += (value[i] - target[i]) * (value[i] - target[i]) + 1e-6 * (log_value[i] - start[i]) * (log_value[i] - start[i]);
			}

			return error;
		}

		std::optional<color> solve(matrix m, color v)
		{
			const auto determinant = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
				+ m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
			if (std::abs(determinant) < 1e-30)
			{
				return {};
			}

			color result{};
			for (auto column = 0u; column < 3; column++)
			{
				auto replaced = m;
				for (auto row = 0u; row < 3; row++)
				{
					replaced[row][column] = v[row];
				}

				result[column] = (replaced[0][0] * (replaced[1][1] * replaced[2][2] - replaced[1][2] * replaced[2][1])
					- replaced[0][1] * (replaced[1][0] * replaced[2][2] - replaced[1][2] * replaced[2][0])
					+ replaced[0][2] * (replaced[1][0] * replaced[2][1] - replaced[1][1] * replaced[2][0])) / determinant;
			}

			return result;
		}

		std::pair<color, double> invert_tonemap(const color& start_log, color log_value, const color& target)
		{
			const auto minimum = std::log(1e-8);
			const auto maximum = std::log(1024.0);

			auto error = get_error(log_value, start_log, target);
			auto damping = 1e-3;
			for (auto iteration = 0u; iteration < 64 && error > 1e-12 && damping < 1e8; iteration++)
			{
				const auto value = apply_tonemap({std::exp(log_value[0]), std::exp(log_value[1]), std::exp(log_value[2])});
				matrix jacobian{};
				for (auto i = 0u; i < 3; i++)
				{
					auto offset = log_value;
					offset[i] += 1e-4;
					const auto moved = apply_tonemap({std::exp(offset[0]), std::exp(offset[1]), std::exp(offset[2])});
					for (auto j = 0u; j < 3; j++)
					{
						jacobian[j][i] = (moved[j] - value[j]) / 1e-4;
					}
				}

				matrix normal{};
				color gradient{};
				for (auto i = 0u; i < 3; i++)
				{
					for (auto j = 0u; j < 3; j++)
					{
						for (auto k = 0u; k < 3; k++)
						{
							normal[i][j] += jacobian[k][i] * jacobian[k][j];
						}
					}

					for (auto k = 0u; k < 3; k++)
					{
						gradient[i] -= jacobian[k][i] * (value[k] - target[k]);
					}

					normal[i][i] += 1e-6;
					gradient[i] -= 1e-6 * (log_value[i] - start_log[i]);
				}

				for (auto i = 0u; i < 3; i++)
				{
					normal[i][i] += damping * normal[i][i];
				}

				const auto step = solve(normal, gradient);
				if (!step)
				{
					damping *= 4.0;
					continue;
				}

				color candidate{};
				for (auto i = 0u; i < 3; i++)
				{
					candidate[i] = std::clamp(log_value[i] + (*step)[i], minimum, maximum);
				}

				const auto candidate_error = get_error(candidate, start_log, target);
				if (candidate_error < error)
				{
					log_value = candidate;
					error = candidate_error;
					damping = std::max(damping / 3.0, 1e-9);
				}
				else
				{
					damping *= 4.0;
				}
			}

			return {log_value, error};
		}

		std::vector<std::uint16_t> convert_texels(const std::vector<std::uint8_t>& texels, const std::size_t source_size, const std::size_t size)
		{
			std::vector<std::uint16_t> converted(size * size * size * 4);
			std::vector<color> solved(size * size * size);
			for (auto b = 0u; b < size; b++)
			{
				for (auto g = 0u; g < size; g++)
				{
					for (auto r = 0u; r < size; r++)
					{
						const color encoded{static_cast<double>(r) / (size - 1), static_cast<double>(g) / (size - 1), static_cast<double>(b) / (size - 1)};
						const color linear{pq_decode(encoded[0]) * 1024.0, pq_decode(encoded[1]) * 1024.0, pq_decode(encoded[2]) * 1024.0};
						const auto graded = lookup_iw7_clut(texels, source_size, apply_tonemap(linear));
						const auto index = (b * size + g) * size + r;

						color start_log{};
						for (auto i = 0u; i < 3; i++)
						{
							start_log[i] = std::clamp(std::log(std::max(linear[i], 1e-8)), std::log(1e-8), std::log(1024.0));
						}

						auto best = invert_tonemap(start_log, start_log, graded);
						for (const auto neighbor : {r ? index - 1 : index, g ? index - size : index, b ? index - size * size : index})
						{
							if (best.second <= 1e-10 || neighbor == index)
							{
								continue;
							}

							const auto candidate = invert_tonemap(start_log, solved[neighbor], graded);
							best = candidate.second < best.second ? candidate : best;
						}

						solved[index] = best.first;
						auto* texel = &converted[index * 4];
						for (auto i = 0u; i < 3; i++)
						{
							texel[i] = static_cast<std::uint16_t>(std::lround(std::clamp(pq_encode(std::exp(best.first[i]) / 1024.0), 0.0, 1.0) * 65535.0));
						}
					}
				}
			}

			return converted;
		}
	}

	std::optional<std::string> dump(const GfxImage* clut)
	{
		static std::unordered_map<std::string, std::optional<std::string>> converted;
		if (const auto entry = converted.find(clut->name); entry != converted.end())
		{
			return entry->second;
		}

		const auto& map = filesystem::get_fastfile();
		auto name = converted.empty() ? std::format("{}_clut", map) : std::format("{}_{}", map, clut->name);
		auto& result = converted[clut->name];

		const auto size = 32u; // TODO: clutSize from stock mp_rust_clut only
		const auto image_name = "gradingclut" + name;
		auto* image = gfximage::dump_grading_clut(clut, image_name, size, [&](const std::vector<std::uint8_t>& texels, const std::size_t source_size)
		{
			return convert_texels(texels, source_size, size);
		});
		if (!image)
		{
			return result;
		}

		zonetool::iw8::GfxGradingClut grading_clut{};
		grading_clut.name = name.data();
		grading_clut.inputColorimetry = zonetool::iw8::HDR_GRADING_CLUT_INPUT_COLORIMETRY_BT2020_PQ;
		grading_clut.outputColorimetry = zonetool::iw8::HDR_GRADING_CLUT_OUTPUT_COLORIMETRY_BT2020_PQ;
		grading_clut.clutSize = size;
		grading_clut.hdrClutImage = image;
		zonetool::iw8::grading_clut::dump(&grading_clut);
		add_csv_row("gradingclut", name);

		result = std::move(name);
		return result;
	}
}
