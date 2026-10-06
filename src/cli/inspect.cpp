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
			std::uint64_t size;
			std::uint64_t external_size;
			std::vector<std::uint64_t> block_sizes;
			XAssetList asset_list;
		};

		zone_summary summarize(const game& game, const std::filesystem::path& path)
		{
			zone_summary summary{};
			summary.file = xfile::read(path, game.format);

			const auto& payload = summary.file.payload;
			const auto memory_size = sizeof(std::uint64_t) * (2 + game.block_count);
			if (payload.size() < memory_size + sizeof(XAssetList))
			{
				throw std::runtime_error("zone is smaller than its headers");
			}

			std::memcpy(&summary.size, payload.data(), sizeof(summary.size));
			std::memcpy(&summary.external_size, payload.data() + 8, sizeof(summary.external_size));

			summary.block_sizes.resize(game.block_count);
			std::memcpy(summary.block_sizes.data(), payload.data() + 16, game.block_count * sizeof(std::uint64_t));
			std::memcpy(&summary.asset_list, payload.data() + memory_size, sizeof(XAssetList));

			if (summary.size != payload.size() - memory_size)
			{
				throw std::runtime_error(std::format("zone size 0x{:X} does not match data size 0x{:X}",
					summary.size, payload.size() - memory_size));
			}

			return summary;
		}

		void print_summary(const zone_summary& summary)
		{
			const auto& header = summary.file.header;
			std::cout << std::format("magic:          {}\n", std::string_view(header.header, sizeof(header.header)));
			std::cout << std::format("version:        {}\n", header.version);
			std::cout << std::format("signed:         {}\n", summary.file.is_signed);
			std::cout << std::format("compress type:  {}\n", header.compressType);
			std::cout << std::format("stream files:   {}\n", summary.file.stream_files.size());
			std::cout << std::format("data size:      0x{:X}\n", summary.size);
			std::cout << std::format("external size:  0x{:X}\n", summary.external_size);

			for (std::size_t block = 0; block < summary.block_sizes.size(); block++)
			{
				std::cout << std::format("block {}:        0x{:X}\n", block, summary.block_sizes[block]);
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
