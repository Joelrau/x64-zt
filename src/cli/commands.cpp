#include <std_include.hpp>
#include "commands.hpp"

namespace zonetool::cli
{
	namespace
	{
		bool is_flag(const std::string& arg)
		{
			return arg.size() > 1 && arg[0] == '-' && arg[1] != '-';
		}

		std::vector<std::string> without_flags(const std::span<const std::string> args)
		{
			std::vector<std::string> result;
			std::ranges::copy_if(args, std::back_inserter(result), [](const std::string& arg)
			{
				return !is_flag(arg);
			});
			return result;
		}

		const std::array dump_options
		{
			option{"-dds", "write images as DDS too"},
			option{"-dump_streamed_image", "read & write imagefile*.pak (streamed images)"},
			option{"-castdump", "writes .cast next to each xmodel/xanim"},
			option{"-lambert", "uses stock lambert techsets (iw8 -> iw7)"},
			option{"-max_image_size <n>", "limits images to pixels on the longest side (iw7 -> iw8)"},
			option{"-no_gpu", "compresses BC6H and BC7 images on CPU (iw7 -> iw8)"},
		};

		const std::array build_options
		{
			option{"-use_usermaps", "writes zone to usermaps/<zone>/ instead of zone/"},
			option{"-copy_to <folder>", "copies .ff and .pak to folder"},
			option{"-more_memory", "doubles zone memory limit"},
			option{"-keep_mp_traverse_nodes", "keeps invalid mp traverse nodes in aipaths (h2)"},
			option{"-uncompressed", "writes zone without compression (iw8)"},
		};
	}

	std::span<const option> global_options()
	{
		static const std::array list
		{
			option{"-game <name>", "source game"},
			option{"-path <folder>", "source game folder"},
			option{"-target_path <folder>", "target game folder"},
			option{"-no_cache", "ignores cache and writes all files"},
			option{"-threads <n>", "limit worker threads"},
		};

		return list;
	}

	std::span<const command> commands()
	{
		static const std::array list
		{
			command{"inspect", "inspect <zone> | inspect --all", "Shows the fastfile header and container data", inspect},
			command{"dumpzone", "dumpzone <zone> [--target <game>]", "Writes assets of a zone to dump/<zone>/", dump_zone, dump_options},
			command{"dumpmap", "dumpmap <map> [--target <game>] [--skip-common]", "Writes map zones to dump/<map>/ (h1, h2, iw6, s1)",
				dump_map, dump_options},
			command{"dumpasset", "dumpasset <type> <name|*> <zone> [--target <game>]", "Writes one asset (* = all of the type) to dump/assets/",
				dump_asset, dump_options},
			command{"verifyzone", "verifyzone <zone> | verifyzone --all", "Reads every asset and checks the zone", verify_zone},
			command{"listzone", "listzone <zone> | listzone --all", "Writes the asset list to asset_listing/<game>/", list_zone},
			command{"listzones", "listzones", "Prints name of every zone the game has, one per line", list_zones},
			command{"buildzone", "buildzone <zone>", "Builds zone/<zone>.ff from zone_source/<zone>.csv and zonetool/<zone>/",
				build_zone, build_options},
			command{"generatecsv", "generatecsv <map> [sp] [--path <dir>]...", "Writes zone_source/<map>.csv from zonetool/<map>/",
				generate_csv},
		};

		return list;
	}

	bool execute(const settings& settings, const std::span<const std::string> all_args)
	{
		const auto args = without_flags(all_args);
		if (args.empty())
		{
			return true;
		}

		for (const auto& command : commands())
		{
			if (command.name == args.front())
			{
				command.handler(settings, std::span(args).subspan(1));
				return true;
			}
		}

		std::cout << std::format("unknown command \"{}\"\n", args.front());
		return false;
	}
}
