#include <std_include.hpp>
#include "commands.hpp"

namespace zonetool::cli
{
	namespace
	{
		struct XAssetList
		{
			std::int32_t stringCount;
			std::uint64_t strings;
			std::int32_t assetCount;
			std::uint64_t assets;
			std::uint64_t globals;
		};

		struct zone_summary
		{
			xfile::fastfile file;
			XAssetList asset_list;
		};

		zone_summary summarize(const game& game, const std::filesystem::path& path)
		{
			zone_summary summary{};
			summary.file = xfile::read(path, game.format);

			const auto& payload = summary.file.payload;
			if (payload.size() < sizeof(XAssetList))
			{
				throw std::runtime_error("zone is smaller than its asset list");
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

			std::cout << std::format("script strings: {}\n", summary.asset_list.stringCount);
			std::cout << std::format("assets:         {}\n", summary.asset_list.assetCount);
			std::cout << std::format("globals:        {}\n", summary.asset_list.globals != 0);
		}

		void inspect_all(const settings& settings)
		{
			std::vector<std::filesystem::path> zones;
			for (const auto& entry : std::filesystem::recursive_directory_iterator(settings.zone_folder()))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".ff")
				{
					zones.emplace_back(entry.path());
				}
			}

			std::atomic_size_t next_zone{};
			std::atomic_size_t failed{};
			std::mutex output_mutex;

			const auto worker = [&]
			{
				for (auto index = next_zone++; index < zones.size(); index = next_zone++)
				{
					try
					{
						summarize(*settings.game, zones[index]);
					}
					catch (const std::exception& e)
					{
						failed++;
						std::lock_guard _(output_mutex);
						std::cout << std::format("{}: {}\n", zones[index].string(), e.what());
					}
				}
			};

			std::vector<std::jthread> threads(std::clamp(std::thread::hardware_concurrency(), 1u, 8u));
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

		const auto path = settings.find_zone(args.front());
		if (!path)
		{
			std::cout << std::format("zone \"{}\" not found in {}\n", args.front(), settings.zone_folder().string());
			return;
		}

		std::cout << std::format("{}\n", path->string());
		print_summary(summarize(*settings.game, *path));
	}
}
