#include <std_include.hpp>
#include "commands.hpp"

#include <utils/cryptography.hpp>
#include <utils/flags.hpp>

namespace zonetool::cli
{
	namespace
	{
		struct zone_summary
		{
			xfile::fastfile file;
			XAssetList asset_list;
			xfile::container container;
		};

		zone_summary summarize(const game& game, const std::filesystem::path& path)
		{
			zone_summary summary{};
			summary.file = xfile::read(path, game.format);
			summary.container = game.format.container;

			const auto& payload = summary.file.payload;
			if (payload.size() < sizeof(XAssetList))
			{
				throw std::runtime_error("zone is smaller than its asset list");
			}

			if (game.inspect_zone)
			{
				return summary;
			}

			if (summary.container == xfile::container::iw3 || summary.container == xfile::container::iw4)
			{
				struct x86_asset_list
				{
					std::uint32_t string_count;
					std::uint32_t strings;
					std::uint32_t asset_count;
					std::uint32_t assets;
				};

				x86_asset_list list{};
				std::memcpy(&list, payload.data(), sizeof(list));
				summary.asset_list = {};
				summary.asset_list.stringCount = static_cast<decltype(summary.asset_list.stringCount)>(list.string_count);
				summary.asset_list.assetCount = static_cast<decltype(summary.asset_list.assetCount)>(list.asset_count);
				return summary;
			}

			std::memcpy(&summary.asset_list, payload.data(), sizeof(XAssetList));
			return summary;
		}

		void print_summary(const zone_summary& summary)
		{
			const auto& file = summary.file;
			std::cout << std::format("magic:          {}\n", file.magic);
			std::cout << std::format("version:        {}\n", file.version);
			std::cout << std::format("signed:         {}\n", file.is_signed);
			std::cout << std::format("stream files:   {}\n", file.stream_files.size());
			std::cout << std::format("shared streams: {}\n", file.shared_stream_files.size());
			std::cout << std::format("data size:      0x{:X}\n", file.payload.size());

			for (std::size_t block = 0; block < file.block_sizes.size(); block++)
			{
				std::cout << std::format("block {}:        0x{:X}\n", block, file.block_sizes[block]);
			}

			if (summary.container == xfile::container::t9)
			{
				std::cout << std::format("data sha1:      {}\n", utils::cryptography::sha1::compute(file.payload, true));
				return;
			}

			std::cout << std::format("script strings: {}\n", summary.asset_list.stringCount);
			std::cout << std::format("assets:         {}\n", summary.asset_list.assetCount);
			std::cout << std::format("globals:        {}\n", summary.asset_list.globals != 0);

			if (summary.container == xfile::container::iw8)
			{
				const auto& header = file.iw8_header;
				std::cout << std::format("transient type: {}\n", header.transient_file_type);
				std::cout << std::format("resident size:  0x{:X}\n", header.resident_part_size);
				std::cout << std::format("resident hash:  0x{:X}\n", header.resident_hash);
				std::cout << std::format("always loaded:  0x{:X}\n", header.always_loaded_part_size);
				std::cout << std::format("preload walk:   0x{:X}\n", header.preload_walk_size);
			}
		}

		void inspect_all(const settings& settings)
		{
			const auto zones = game_files::list_zones();

			std::atomic_size_t next_zone{};
			std::atomic_size_t failed{};
			std::mutex output_mutex;

			const auto worker = [&]
			{
				for (auto index = next_zone++; index < zones.size(); index = next_zone++)
				{
					try
					{
						if (settings.game->inspect_zone)
						{
							settings.game->inspect_zone(settings.zone_folder(), zones[index]);
						}
						else
						{
							summarize(*settings.game, zones[index]);
						}
					}
					catch (const std::exception& e)
					{
						failed++;
						std::lock_guard _(output_mutex);
						std::cout << std::format("{}: {}\n", zones[index].string(), e.what());
					}
				}
			};

			std::vector<std::jthread> threads(std::clamp(utils::flags::get_thread_count(), 1u, 8u));
			for (auto& thread : threads)
			{
				thread = std::jthread(worker);
			}
			threads.clear();

			std::cout << std::format("{} of {} zones passed\n", zones.size() - failed, zones.size());
		}
	}

	void inspect(const settings& settings, const std::span<const std::string> args)
	{
		if (args.empty())
		{
			std::cout << "usage: inspect <zone> | inspect --all\n";
			return;
		}

		if (args.front() == "--all")
		{
			inspect_all(settings);
			return;
		}

		const auto path = game_files::find_zone(args.front());
		if (!path)
		{
			std::cout << std::format("zone \"{}\" not found in {}\n", args.front(), game_files::get().description());
			return;
		}

		std::cout << std::format("{}\n", path->string());
		print_summary(summarize(*settings.game, *path));
		if (settings.game->inspect_zone)
		{
			std::cout << settings.game->inspect_zone(settings.zone_folder(), *path);
		}
	}
}
