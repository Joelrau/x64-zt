#include "tome.hpp"
#include "../win_include.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace utils::umbra
{
	namespace
	{
		// Shared prefix of tome versions 0x12 and 0x14. Every pointer is a byte offset from the tome start, 0 is null.
		struct tome_header
		{
			std::uint32_t m_versionMagic;
			std::uint32_t m_crc32;
			std::uint32_t m_size;
			float m_lodBaseDistance;
			std::uint32_t m_flags;
			float m_treeMin[3];
			float m_treeMax[3];
			std::uint32_t m_tileTree_nodeCount_mapWidth;
			std::uint32_t m_tileTree_treeData;
			std::uint32_t m_tileTree_map;
			std::uint32_t m_tileTree_numSplitValues;
			std::uint32_t m_tileTree_splitValues;
			std::int32_t m_numObjects;
			std::uint32_t m_objBounds;
			std::uint32_t m_objDistances;
			std::uint32_t m_userIDStarts;
			std::uint32_t m_userIDs;
			std::uint32_t m_listWidths;
			std::uint32_t m_objectLists;
			std::int32_t m_objectListSize;
			std::uint32_t m_clusterLists;
			std::int32_t m_clusterListSize;
			std::int32_t m_numGates;
			std::uint32_t m_gateIndexMap;
			std::uint32_t m_gateVertices;
			std::int32_t m_numGateVertices;
			std::uint32_t m_gateIndices;
			std::int32_t m_numClusters;
			std::uint32_t m_clusters;
			std::uint32_t m_clusterPortals;
			std::uint32_t m_cellStarts;
			std::int32_t m_numLeafTiles;
			std::int32_t m_numTiles;
			std::int32_t m_bitsPerSlotPath;
			std::uint32_t m_slotPaths;
			std::uint32_t m_tileLodLevels;
			std::uint32_t m_tiles;
			std::uint32_t m_tileMatchingData;
			std::uint32_t m_matchingTrees;
			std::int32_t m_numMatchingTrees;
			std::int32_t m_numTomes;
			std::uint32_t m_tomeClusterStarts;
			std::uint32_t m_tomeClusterPortalStarts;
			char m_computationString[128];
			std::uint32_t m_objectDepthmaps;
			std::uint32_t m_depthmapFaces;
			std::uint32_t m_depthmapPalettes;
			std::int32_t m_numFaces;
		};
		static_assert(sizeof(tome_header) == 332);

		// Shared prefix of the 0x12 and 0x14 tiles.
		struct tile_header
		{
			float m_treeMin[3];
			float m_treeMax[3];
			std::uint32_t m_viewTree_nodeCount_mapWidth;
			std::uint32_t m_viewTree_treeData;
			std::uint32_t m_viewTree_map;
			std::uint32_t m_viewTree_numSplitValues;
			std::uint32_t m_viewTree_splitValues;
			std::int32_t m_sizeAndFlags;
			float m_portalExpand;
			std::int32_t m_numCellsAndClusters;
			std::uint32_t m_cells;
			std::uint32_t m_portals;
		};
		static_assert(sizeof(tile_header) == 64);

		constexpr std::uint32_t CELL_NODE_SIZE = 36;
		constexpr std::uint32_t TOME_MAGIC = 0xD6000000;
		constexpr std::uint32_t TOME_VERSION_MIN = 0x12;
		constexpr std::uint32_t TOME_VERSION_MAX = 0x14;

		std::string env_string(const char* name)
		{
			char* value = nullptr;
			std::size_t length = 0;
			if (_dupenv_s(&value, &length, name) != 0 || !value)
			{
				return {};
			}

			std::string result(value);
			std::free(value);
			return result;
		}

		float env_float(const char* name, const float fallback)
		{
			const auto value = env_string(name);
			return !value.empty() ? static_cast<float>(std::atof(value.data())) : fallback;
		}

		std::string module_directory()
		{
			HMODULE module = nullptr;
			if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
				reinterpret_cast<LPCSTR>(&module_directory), &module))
			{
				return {};
			}

			char path[MAX_PATH]{};
			if (!GetModuleFileNameA(module, path, sizeof(path)))
			{
				return {};
			}

			std::string result = path;
			const auto slash = result.find_last_of("\\/");
			return slash == std::string::npos ? std::string(".") : result.substr(0, slash);
		}

		bool file_exists(const std::string& path)
		{
			const auto attributes = GetFileAttributesA(path.c_str());
			return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
		}

		bool write_scene(const std::string& path, const tome_input& input, std::string& error)
		{
			std::ofstream file(path, std::ios::binary);
			if (!file)
			{
				error = "cannot write scene \"" + path + "\"";
				return false;
			}

			const auto put = [&file](const void* data, const std::size_t size)
			{
				file.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
			};

			scene_header header{};
			std::memcpy(header.magic, SCENE_MAGIC, sizeof(SCENE_MAGIC));
			header.version = SCENE_VERSION;
			header.header_size = sizeof(scene_header);
			header.params = input.params;
			header.model_count = static_cast<std::uint32_t>(input.models.size());
			header.object_count = static_cast<std::uint32_t>(input.objects.size());
			header.view_volume_count = static_cast<std::uint32_t>(input.view_volumes.size());
			header.seed_point_count = static_cast<std::uint32_t>(input.seed_points.size());
			put(&header, sizeof(header));

			for (const auto& model : input.models)
			{
				scene_model entry{};
				entry.vertex_count = static_cast<std::uint32_t>(model.vertices.size() / 3);
				entry.triangle_count = static_cast<std::uint32_t>(model.indices.size() / 3);
				put(&entry, sizeof(entry));
				put(model.vertices.data(), entry.vertex_count * 3 * sizeof(float));
				put(model.indices.data(), entry.triangle_count * 3 * sizeof(std::uint32_t));
			}

			for (const auto& object : input.objects)
			{
				scene_object entry{ object.model, object.user_id, object.flags };
				put(&entry, sizeof(entry));
			}

			for (const auto& volume : input.view_volumes)
			{
				put(&volume, sizeof(volume));
			}

			for (const auto& seed : input.seed_points)
			{
				scene_seed_point entry{ { seed[0], seed[1], seed[2] } };
				put(&entry, sizeof(entry));
			}

			if (!file)
			{
				error = "short write to scene \"" + path + "\"";
				return false;
			}
			return true;
		}

		bool read_file(const std::string& path, std::vector<unsigned char>& out)
		{
			std::ifstream file(path, std::ios::binary | std::ios::ate);
			if (!file)
			{
				return false;
			}
			const auto size = file.tellg();
			file.seekg(0);
			out.resize(static_cast<std::size_t>(size));
			file.read(reinterpret_cast<char*>(out.data()), size);
			return static_cast<bool>(file);
		}

		void forward_lines(const std::string& path, const log_callback& log)
		{
			std::ifstream file(path);
			std::string line;
			while (std::getline(file, line))
			{
				while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
				{
					line.pop_back();
				}
				if (!line.empty())
				{
					log(line);
				}
			}
		}

		int run_generator(const std::string& command_line, const std::string& output_path, std::string& error)
		{
			SECURITY_ATTRIBUTES inheritable{};
			inheritable.nLength = sizeof(inheritable);
			inheritable.bInheritHandle = TRUE;

			HANDLE output = CreateFileA(output_path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, &inheritable,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (output == INVALID_HANDLE_VALUE)
			{
				error = "cannot create \"" + output_path + "\"";
				return -1;
			}

			STARTUPINFOA startup{};
			startup.cb = sizeof(startup);
			startup.dwFlags = STARTF_USESTDHANDLES;
			startup.hStdOutput = output;
			startup.hStdError = output;
			startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

			PROCESS_INFORMATION process{};
			std::vector<char> mutable_command(command_line.begin(), command_line.end());
			mutable_command.push_back(0);

			const auto started = CreateProcessA(nullptr, mutable_command.data(), nullptr, nullptr, TRUE,
				CREATE_NO_WINDOW, nullptr, nullptr, &startup, &process);
			CloseHandle(output);
			if (!started)
			{
				error = "CreateProcess failed with " + std::to_string(GetLastError()) + " for: " + command_line;
				return -1;
			}

			WaitForSingleObject(process.hProcess, INFINITE);
			DWORD exit_code = static_cast<DWORD>(-1);
			GetExitCodeProcess(process.hProcess, &exit_code);
			CloseHandle(process.hThread);
			CloseHandle(process.hProcess);
			return static_cast<int>(exit_code);
		}
	}

	scene_params tome_input::default_params()
	{
		scene_params params{};
		params.smallest_occluder = env_float("ZT_UMBRA_SMALLEST_OCCLUDER", 128.0f);
		params.smallest_hole = env_float("ZT_UMBRA_SMALLEST_HOLE", 4.0f);
		params.backface_limit = env_float("ZT_UMBRA_BACKFACE_LIMIT", 100.0f);
		params.cluster_size = 0.0f;
		params.object_group_cost = 0.0f;
		params.minimum_accurate_distance = 0.0f;
		params.output_flags = 0;
		params.thread_count = 0;
		return params;
	}

	std::uint32_t tome_input::add_box_model(const float* mins, const float* maxs)
	{
		tome_model model;
		model.vertices.reserve(8 * 3);
		for (unsigned int corner = 0; corner < 8; corner++)
		{
			model.vertices.push_back((corner & 1) ? maxs[0] : mins[0]);
			model.vertices.push_back((corner & 2) ? maxs[1] : mins[1]);
			model.vertices.push_back((corner & 4) ? maxs[2] : mins[2]);
		}

		static const std::uint32_t faces[6][4] = {
			{ 0, 4, 6, 2 },
			{ 1, 3, 7, 5 },
			{ 0, 1, 5, 4 },
			{ 2, 6, 7, 3 },
			{ 0, 2, 3, 1 },
			{ 4, 5, 7, 6 },
		};
		for (const auto& face : faces)
		{
			model.indices.insert(model.indices.end(), { face[0], face[1], face[2], face[0], face[2], face[3] });
		}

		this->models.emplace_back(std::move(model));
		return static_cast<std::uint32_t>(this->models.size() - 1);
	}

	std::string find_generator()
	{
		if (const auto override = env_string("ZT_UMBRA_TOMEGEN"); !override.empty())
		{
			return file_exists(override) ? override : std::string();
		}

		const auto candidate = module_directory() + "\\umbra-tomegen.exe";
		return file_exists(candidate) ? candidate : std::string();
	}

	bool generate_tome(const tome_input& input, tome_result& result, const log_callback& log)
	{
		result = {};

		const auto generator = find_generator();
		if (generator.empty())
		{
			result.error = "umbra-tomegen.exe not found: set ZT_UMBRA_TOMEGEN or put it next to zonetool.exe";
			return false;
		}

		for (const auto& object : input.objects)
		{
			if (object.model >= input.models.size())
			{
				result.error = "object references model " + std::to_string(object.model) + " of "
					+ std::to_string(input.models.size());
				return false;
			}
		}

		auto work_directory = input.work_directory;
		if (work_directory.empty())
		{
			char temp[MAX_PATH]{};
			GetTempPathA(sizeof(temp), temp);
			work_directory = std::string(temp) + "zonetool-umbra";
		}
		CreateDirectoryA(work_directory.c_str(), nullptr);

		const auto base = work_directory + "\\" + (input.name.empty() ? "tome" : input.name);
		const auto scene_path = base + ".umbrascene";
		const auto tome_path = base + ".tome";
		const auto log_path = base + ".umbra.log";
		const auto output_path = base + ".tomegen.log";

		if (!write_scene(scene_path, input, result.error))
		{
			return false;
		}

		std::string command = "\"" + generator + "\" generate \"" + scene_path + "\" \"" + tome_path + "\" --log \""
			+ log_path + "\"";
		if (input.params.thread_count)
		{
			command += " --threads " + std::to_string(input.params.thread_count);
		}
		if (input.verify)
		{
			command += " --verify";
		}

		log("running " + command);
		DeleteFileA(tome_path.c_str());

		const auto started = std::chrono::steady_clock::now();
		const auto exit_code = run_generator(command, output_path, result.error);
		result.generation_seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();

		forward_lines(output_path, log);

		if (exit_code < 0)
		{
			return false;
		}

		if (exit_code != 0)
		{
			result.error = "umbra-tomegen exited with " + std::to_string(exit_code) + " (see " + output_path + ")";
			return false;
		}

		if (!read_file(tome_path, result.data) || result.data.empty())
		{
			result.error = "umbra-tomegen reported success but wrote no tome to " + tome_path;
			return false;
		}

		if (!inspect_tome(result.data.data(), result.data.size(), result.stats))
		{
			result.error = "umbra-tomegen output is not a tome";
			result.data.clear();
			return false;
		}

		std::string reason;
		if (!is_tome_accepted_by_iw7(result.data.data(), result.data.size(), reason))
		{
			result.error = "generated tome would be rejected by IW7: " + reason;
			result.data.clear();
			return false;
		}

		if (!input.keep_files)
		{
			DeleteFileA(scene_path.c_str());
			DeleteFileA(tome_path.c_str());
			DeleteFileA(log_path.c_str());
			DeleteFileA(output_path.c_str());
		}

		return true;
	}

	bool inspect_tome(const unsigned char* data, const std::size_t size, tome_stats& stats)
	{
		stats = {};
		if (!data || size < sizeof(tome_header))
		{
			return false;
		}

		tome_header header{};
		std::memcpy(&header, data, sizeof(header));
		if ((header.m_versionMagic & 0xFFFF0000) != TOME_MAGIC)
		{
			return false;
		}

		stats.version = header.m_versionMagic & 0xFFFF;
		stats.size = header.m_size;
		stats.crc32 = header.m_crc32;
		stats.lod_base_distance = header.m_lodBaseDistance;
		std::memcpy(stats.tree_min, header.m_treeMin, sizeof(stats.tree_min));
		std::memcpy(stats.tree_max, header.m_treeMax, sizeof(stats.tree_max));
		stats.object_count = header.m_numObjects;
		stats.tile_count = header.m_numTiles;
		stats.leaf_tile_count = header.m_numLeafTiles;
		stats.cluster_count = header.m_numClusters;
		stats.gate_count = header.m_numGates;
		stats.object_list_entries = static_cast<std::uint32_t>(std::max(header.m_objectListSize, 0));

		const auto usable = std::min<std::size_t>(size, header.m_size);

		if (header.m_numObjects > 0)
		{
			if (header.m_userIDStarts && header.m_userIDStarts + (header.m_numObjects + 1u) * 4u <= usable)
			{
				std::memcpy(&stats.user_id_count, data + header.m_userIDStarts + header.m_numObjects * 4u, 4);
			}
			else
			{
				stats.user_id_count = static_cast<std::uint32_t>(header.m_numObjects);
			}
		}

		const auto node_count = header.m_tileTree_nodeCount_mapWidth >> 5;
		if (header.m_tiles && header.m_tiles + node_count * 4u <= usable)
		{
			for (std::uint32_t node = 0; node < node_count; node++)
			{
				std::uint32_t tile_offset;
				std::memcpy(&tile_offset, data + header.m_tiles + node * 4u, 4);
				if (!tile_offset || tile_offset + sizeof(tile_header) > usable)
				{
					continue;
				}

				tile_header tile{};
				std::memcpy(&tile, data + tile_offset, sizeof(tile));
				if (!(static_cast<std::uint32_t>(tile.m_sizeAndFlags) & 1))
				{
					continue;
				}

				const auto cell_count = static_cast<std::uint32_t>(tile.m_numCellsAndClusters) & 0xFFFF;
				stats.cell_count += cell_count;
				if (!tile.m_cells || tile_offset + tile.m_cells + cell_count * CELL_NODE_SIZE > usable)
				{
					continue;
				}

				for (std::uint32_t cell = 0; cell < cell_count; cell++)
				{
					std::uint32_t portal_count;
					std::memcpy(&portal_count, data + tile_offset + tile.m_cells + cell * CELL_NODE_SIZE + 4, 4);
					stats.portal_count += portal_count;
					stats.cells_without_portals += portal_count == 0 ? 1 : 0;
				}
			}
		}

		return true;
	}

	std::vector<std::uint32_t> read_user_ids(const unsigned char* data, const std::size_t size)
	{
		std::vector<std::uint32_t> ids;
		if (!data || size < sizeof(tome_header))
		{
			return ids;
		}

		tome_header header{};
		std::memcpy(&header, data, sizeof(header));
		if ((header.m_versionMagic & 0xFFFF0000) != TOME_MAGIC || header.m_numObjects <= 0 || !header.m_userIDs)
		{
			return ids;
		}

		const auto usable = std::min<std::size_t>(size, header.m_size);
		std::uint32_t count = static_cast<std::uint32_t>(header.m_numObjects);
		if (header.m_userIDStarts)
		{
			if (header.m_userIDStarts + (header.m_numObjects + 1u) * 4u > usable)
			{
				return ids;
			}
			std::memcpy(&count, data + header.m_userIDStarts + header.m_numObjects * 4u, 4);
		}

		if (header.m_userIDs + static_cast<std::size_t>(count) * 4u > usable)
		{
			return ids;
		}

		ids.resize(count);
		std::memcpy(ids.data(), data + header.m_userIDs, static_cast<std::size_t>(count) * 4u);
		return ids;
	}

	bool is_tome_accepted_by_iw7(const unsigned char* data, const std::size_t size, std::string& reason)
	{
		if (!data || size < 12)
		{
			reason = "buffer smaller than a tome header";
			return false;
		}

		std::uint32_t magic, tome_size;
		std::memcpy(&magic, data, 4);
		std::memcpy(&tome_size, data + 8, 4);

		if ((magic & 0xFFFF0000) != TOME_MAGIC)
		{
			reason = "magic is not 0xD600";
			return false;
		}
		const auto version = magic & 0xFFFF;
		if (version < TOME_VERSION_MIN || version > TOME_VERSION_MAX)
		{
			reason = "version 0x" + std::to_string(version) + " outside 0x12..0x14";
			return false;
		}
		if (tome_size > size)
		{
			reason = "m_size " + std::to_string(tome_size) + " exceeds the buffer (" + std::to_string(size) + ")";
			return false;
		}
		return true;
	}
}
