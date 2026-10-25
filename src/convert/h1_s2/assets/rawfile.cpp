#include <std_include.hpp>

#include "../include.hpp"
#include "rawfile.hpp"
#include "scriptfile.hpp"
#include "techset.hpp"

#include <utils/compression.hpp>

namespace zonetool::h1::converter::s2::rawfile
{
	namespace
	{
		const std::unordered_map<std::string, std::string> s2_sun_dvars = // s2 sun files name each dvar by its id
		{
			{"r_sunsprite_shader", "4843"},
			{"r_sunsprite_size", "4337"},
			{"r_sunflare_shader", "5654"},
			{"r_sunflare_min_size", "2938"},
			{"r_sunflare_min_angle", "1834"},
			{"r_sunflare_max_size", "89"},
			{"r_sunflare_max_angle", "3627"},
			{"r_sunflare_max_alpha", "2957"},
			{"r_sunflare_fadein", "371"},
			{"r_sunflare_fadeout", "1615"},
			{"r_sunblind_min_angle", "5397"},
			{"r_sunblind_max_angle", "3858"},
			{"r_sunblind_max_darken", "1590"},
			{"r_sunblind_fadein", "4939"},
			{"r_sunblind_fadeout", "407"},
			{"r_sunglare_min_angle", "3682"},
			{"r_sunglare_max_angle", "970"},
			{"r_sunglare_max_lighten", "4772"},
			{"r_sunglare_fadein", "3538"},
			{"r_sunglare_fadeout", "438"},
			{"r_sun_fx_position", "3196"},
		};

		const std::unordered_map<std::string, std::string> s2_vision_dvars = // s2 vision files name each dvar by its id
		{
			{"r_filmEnable", "4389"},
			{"r_filmContrast", "1485"},
			{"r_filmBrightness", "5216"},
			{"r_filmDesaturation", "1558"},
			{"r_filmDesaturationDark", "191"},
			{"r_filmInvert", "2437"},
			{"r_filmLightTint", "283"},
			{"r_filmMediumTint", "4713"},
			{"r_filmDarkTint", "1168"},
			{"r_glow", "2534"},
			{"r_glowRadius0", "3804"},
			{"r_glowBloomPinch", "470"},
			{"r_glowBloomCutoff", "2579"},
			{"r_glowBloomDesaturation", "4522"},
			{"r_glowBloomIntensity0", "5303"},
			{"r_glowUseAltCutoff", "5235"},
			{"r_volumeLightScatter", "2696"},
			{"r_volumeLightScatterLinearAtten", "2564"},
			{"r_volumeLightScatterQuadraticAtten", "3758"},
			{"r_volumeLightScatterAngularAtten", "750"},
			{"r_volumeLightScatterDepthAttenNear", "3070"},
			{"r_volumeLightScatterDepthAttenFar", "5444"},
			{"r_volumeLightScatterBackgroundDistance", "4506"},
			{"r_volumeLightScatterColor", "3200"},
			{"r_volumeLightScatterEv", "4638"},
			{"r_unlitSurfaceHDRScalar", "284"},
			{"r_chromaticAberrationMode", "5622"},
			{"r_chromaticSeparation", "2844"},
			{"r_chromaticAberrationAlpha", "700"},
		};

		std::string read_rawfile(const RawFile& asset)
		{
			if (!asset.compressedLen)
			{
				return {asset.buffer, static_cast<std::size_t>(asset.len)};
			}

			return utils::compression::zlib::decompress({asset.buffer, static_cast<std::size_t>(asset.compressedLen)});
		}

		std::string convert_dvar_file(const RawFile& asset, const std::unordered_map<std::string, std::string>& dvar_ids)
		{
			std::string text;
			for (auto line : utils::string::split(read_rawfile(asset), '\n'))
			{
				if (line.ends_with('\r'))
				{
					line.pop_back();
				}

				const auto separator = line.find(' ');
				const auto dvar = line.substr(0, separator);
				if (dvar.empty())
				{
					continue;
				}

				if (std::ranges::all_of(dvar, [](const unsigned char c) { return std::isdigit(c); })) // already an id
				{
					text += line + "\n";
					continue;
				}

				const auto id = dvar_ids.find(dvar);
				if (id == dvar_ids.end() || separator == std::string::npos)
				{
					ZONETOOL_WARNING("rawfile \"%s\": S2 does not know dvar \"%s\", dropping it", asset.name, dvar.data());
					continue;
				}

				text += id->second + line.substr(separator) + "\n";
			}

			return text;
		}
	}

	void dump(RawFile* asset)
	{
		if (!asset->buffer)
		{
			return;
		}

		const std::string name = asset->name;
		const auto is_sun = name.starts_with("sun/") && name.ends_with(".sun");
		const auto is_vision = name.starts_with("vision/") && name.ends_with(".vision");
		const auto is_gsc = name.ends_with(".gsc");

		// s2 common_mp ships visions with the same names, a map zone can't replace them so s2x reads override/<name> first
		const auto out_name = is_vision && techset::common_rawfiles().contains(name) ? "override/" + name : name;
		if (is_sun || is_vision || is_gsc)
		{
			const auto text = is_gsc ? scriptfile::patch(name, read_rawfile(*asset)) : convert_dvar_file(*asset, is_sun ? s2_sun_dvars : s2_vision_dvars);
			zonetool::s2::RawFile new_asset{out_name.data(), 0, static_cast<int>(text.size()), text.data()};
			zonetool::s2::rawfile::dump(&new_asset);
		}
		else
		{
			zonetool::s2::RawFile new_asset{asset->name, asset->compressedLen, asset->len, asset->buffer};
			zonetool::s2::rawfile::dump(&new_asset);
		}

		add_csv_row("rawfile", out_name);
	}
}
