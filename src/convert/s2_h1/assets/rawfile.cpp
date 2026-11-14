#include <std_include.hpp>

#include "../include.hpp"
#include "rawfile.hpp"

#include <utils/compression.hpp>
#include <utils/string.hpp>

namespace zonetool::s2::converter::h1::rawfile
{
	namespace
	{
		const std::unordered_map<std::string, std::string> h1_vision_dvars =
		{
			{"4389", "r_filmEnable"},
			{"1485", "r_filmContrast"},
			{"5216", "r_filmBrightness"},
			{"1558", "r_filmDesaturation"},
			{"191", "r_filmDesaturationDark"},
			{"2437", "r_filmInvert"},
			{"283", "r_filmLightTint"},
			{"4713", "r_filmMediumTint"},
			{"1168", "r_filmDarkTint"},
			{"2534", "r_glow"},
			{"3804", "r_glowRadius0"},
			{"470", "r_glowBloomPinch"},
			{"2579", "r_glowBloomCutoff"},
			{"4522", "r_glowBloomDesaturation"},
			{"5303", "r_glowBloomIntensity0"},
			{"5235", "r_glowUseAltCutoff"},
			{"2696", "r_volumeLightScatter"},
			{"2564", "r_volumeLightScatterLinearAtten"},
			{"3758", "r_volumeLightScatterQuadraticAtten"},
			{"750", "r_volumeLightScatterAngularAtten"},
			{"3070", "r_volumeLightScatterDepthAttenNear"},
			{"5444", "r_volumeLightScatterDepthAttenFar"},
			{"4506", "r_volumeLightScatterBackgroundDistance"},
			{"3200", "r_volumeLightScatterColor"},
			{"4638", "r_volumeLightScatterEv"},
			{"5332", "r_rimLightBias"},
			{"915", "r_rimLightFalloffMinDistance"},
			{"5574", "r_rimLightFalloffMaxDistance"},
			{"284", "r_unlitSurfaceHDRScalar"},
			{"5622", "r_chromaticAberrationMode"},
			{"2844", "r_chromaticSeparation"},
			{"700", "r_chromaticAberrationAlpha"},
		};

		std::string read_rawfile(const RawFile& asset)
		{
			if (!asset.compressedLen)
			{
				return {asset.buffer, static_cast<std::size_t>(asset.len)};
			}

			return utils::compression::zlib::decompress({asset.buffer, static_cast<std::size_t>(asset.compressedLen)});
		}

		std::string convert_vision(const RawFile& asset)
		{
			std::string text;
			for (auto line : utils::string::split(read_rawfile(asset), '\n'))
			{
				if (line.ends_with('\r'))
				{
					line.pop_back();
				}

				const auto separator = line.find(' ');
				const auto dvar = h1_vision_dvars.find(line.substr(0, separator));
				if (dvar != h1_vision_dvars.end() && separator != std::string::npos)
				{
					text += dvar->second + line.substr(separator) + "\n";
				}
			}

			return text;
		}

		std::string remove_dev_blocks(std::string source)
		{
			for (auto start = source.find("/$"); start != std::string::npos; start = source.find("/$", start))
			{
				const auto end = source.find("$/", start + 2);
				source.erase(start, end == std::string::npos ? std::string::npos : end + 2 - start);
			}

			return source;
		}
	}

	void dump(RawFile* asset)
	{
		if (!asset->buffer)
		{
			return;
		}

		const std::string name = asset->name;
		if (name.starts_with("vision/") && name.ends_with(".vision"))
		{
			write_file(name, convert_vision(*asset));
		}
		else if (name.starts_with("maps/createart/") && name.ends_with("_fog_hdr.gsc"))
		{
			write_file(name, remove_dev_blocks(read_rawfile(*asset)));
		}
		else
		{
			return;
		}

		add_csv_row("rawfile", name);
	}
}
