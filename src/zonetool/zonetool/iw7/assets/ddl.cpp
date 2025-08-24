#include <std_include.hpp>
#include "ddl.hpp"

//#define DUMP_DDL_JSON

namespace zonetool::iw7
{
	namespace
	{
		int compare_hash(const void* a, const void* b)
		{
			const auto hash_a = reinterpret_cast<DDLHash*>(
				reinterpret_cast<size_t>(a))->hash;
			const auto hash_b = reinterpret_cast<DDLHash*>(
				reinterpret_cast<size_t>(b))->hash;

			if (hash_a < hash_b)
			{
				return -1;
			}
			else if (hash_a > hash_b)
			{
				return 1;
			}

			return 0;
		}
	}

	void generateHashTables(DDLDef* def, zone_memory* mem)
	{
		for (auto enum_index = 0; enum_index < def->enumCount; enum_index++)
		{
			def->enumList[enum_index].hashTable.max = 1024;
			def->enumList[enum_index].hashTable.count = def->enumList[enum_index].memberCount;
			def->enumList[enum_index].hashTable.list = mem->allocate<DDLHash>(def->enumList[enum_index].hashTable.count);
			for (auto list_index = 0; list_index < def->enumList[enum_index].hashTable.count; list_index++)
			{
				def->enumList[enum_index].hashTable.list[list_index].index = list_index;
				def->enumList[enum_index].hashTable.list[list_index].hash = DDL_HashString(def->enumList[enum_index].members[list_index]);
			}
			std::qsort(def->enumList[enum_index].hashTable.list,
				def->enumList[enum_index].hashTable.count, sizeof(DDLHash), compare_hash);
		}

		for (auto struct_index = 0; struct_index < def->structCount; struct_index++)
		{
			auto& struct_def = def->structList[struct_index];

			std::vector<DDLHash> upper_hashes;
			std::vector<DDLHash> lower_hashes;

			std::unordered_map<uint32_t, std::vector<int>> upperMap;

			std::vector<DDLHash> pad_hashes;

			for (auto member_index = 0; member_index < struct_def.memberCount; member_index++)
			{
				const auto& member = struct_def.members[member_index];
				uint32_t hash = DDL_HashString(member.name);

				DDLHash entry{};
				entry.hash = hash;
				entry.index = member_index;

				if (member.type == DDLType::DDL_PAD_TYPE)
				{
					pad_hashes.push_back(entry);
					continue;
				}

				if ((member.type == DDLType::DDL_UINT_TYPE && (member.bitSize == 1 || member.limitSize == 1)))
				{
					lower_hashes.push_back(entry);
				}
				else
				{
					upper_hashes.push_back(entry);
				}
			}

			std::qsort(upper_hashes.data(),
				upper_hashes.size(), sizeof(DDLHash), compare_hash);

			std::qsort(lower_hashes.data(),
				lower_hashes.size(), sizeof(DDLHash), compare_hash);

			struct_def.hashTableUpper.max = static_cast<int>(upper_hashes.size());
			struct_def.hashTableUpper.count = static_cast<int>(upper_hashes.size());
			struct_def.hashTableUpper.list = mem->allocate<DDLHash>(struct_def.hashTableUpper.count);
			for (auto list_index = 0; list_index < upper_hashes.size(); list_index++)
			{
				struct_def.hashTableUpper.list[list_index].index = upper_hashes[list_index].index;
				struct_def.hashTableUpper.list[list_index].hash = upper_hashes[list_index].hash;
			}

			struct_def.hashTableLower.max = static_cast<int>(lower_hashes.size() + pad_hashes.size());
			struct_def.hashTableLower.count = static_cast<int>(lower_hashes.size() + pad_hashes.size());
			struct_def.hashTableLower.list = mem->allocate<DDLHash>(struct_def.hashTableLower.count);
			for (auto list_index = 0; list_index < lower_hashes.size(); list_index++)
			{
				struct_def.hashTableLower.list[list_index].index = lower_hashes[list_index].index;
				struct_def.hashTableLower.list[list_index].hash = lower_hashes[list_index].hash;
			}

			// add pads to the end of the lower hash table
			for (auto i = 0; i < pad_hashes.size(); i++)
			{
				int list_index = static_cast<int>(lower_hashes.size()) + i;
				struct_def.hashTableLower.list[list_index].index = pad_hashes[i].index;
				struct_def.hashTableLower.list[list_index].hash = pad_hashes[i].hash;
			}
		}

		// calculate offsets
		for (auto struct_index = 0; struct_index < def->structCount; struct_index++)
		{
			auto& struct_def = def->structList[struct_index];
			int offset = 0;

			for (auto list_index = 0; list_index < struct_def.hashTableUpper.count; list_index++)
			{
				auto member_index = struct_def.hashTableUpper.list[list_index].index;
				auto& member = struct_def.members[member_index];
				member.offset = offset;
				offset += member.bitSize;
			}

			for (auto list_index = 0; list_index < struct_def.hashTableLower.count; list_index++)
			{
				auto member_index = struct_def.hashTableLower.list[list_index].index;
				auto& member = struct_def.members[member_index];
				member.offset = offset;
				offset += member.bitSize;
			}
		}
	}

	namespace ddl_json
	{
		DDLFile* parse_from_json(const std::string& name, zone_memory* mem)
		{
			auto file = filesystem::file("ddl\\"s + name + ".json"s);
			file.open("rb");

			if (file.get_fp())
			{
				ZONETOOL_INFO("Parsing ddl json \"%s\"...", name.data());

				const auto size = file.size();
				auto bytes = file.read_bytes(size);
				file.close();

				auto data = json::parse(bytes);

				auto asset = mem->allocate<DDLFile>();
				asset->name = mem->duplicate_string(data["name"].get<std::string>());

				asset->ddlDef = mem->allocate<DDLDef>();
				auto j_defs = data["defs"];

				assert(j_defs.size());

				auto def = asset->ddlDef;
				auto def_index = 0;
				while (def)
				{
					auto j_def = j_defs[def_index];

					def->name = mem->duplicate_string(j_def["name"].get<std::string>());
					def->version = j_def["version"].get<unsigned short>();
					def->checksum = j_def["checksum"].get<unsigned int>();
					def->flags = j_def["flags"].get<unsigned char>();
					def->bitSize = j_def["bitSize"].get<int>();
					def->byteSize = j_def["byteSize"].get<int>();

					if (j_def["structs"].size())
					{
						def->structCount = static_cast<int>(j_def["structs"].size());
						def->structList = mem->allocate<DDLStruct>(def->structCount);
						for (auto struct_index = 0; struct_index < def->structCount; struct_index++)
						{
							def->structList[struct_index].name = mem->duplicate_string(j_def["structs"][struct_index]["name"].get<std::string>());
							def->structList[struct_index].bitSize = j_def["structs"][struct_index]["bitSize"].get<int>();
							if (j_def["structs"][struct_index]["members"].size())
							{
								def->structList[struct_index].memberCount = static_cast<int>(j_def["structs"][struct_index]["members"].size());
								def->structList[struct_index].members = mem->allocate<DDLMember>(def->structList[struct_index].memberCount);
								for (auto member_index = 0; member_index < def->structList[struct_index].memberCount; member_index++)
								{
									def->structList[struct_index].members[member_index].name = mem->duplicate_string(
										j_def["structs"][struct_index]["members"][member_index]["name"].get<std::string>());

									def->structList[struct_index].members[member_index].index =
										j_def["structs"][struct_index]["members"][member_index]["index"].get<int>();

									def->structList[struct_index].members[member_index].parent = nullptr;

									def->structList[struct_index].members[member_index].bitSize =
										j_def["structs"][struct_index]["members"][member_index]["bitSize"].get<int>();

									def->structList[struct_index].members[member_index].limitSize =
										j_def["structs"][struct_index]["members"][member_index]["limitSize"].get<int>();

									def->structList[struct_index].members[member_index].offset =
										j_def["structs"][struct_index]["members"][member_index]["offset"].get<int>();

									def->structList[struct_index].members[member_index].type =
										j_def["structs"][struct_index]["members"][member_index]["type"].get<int>();

									def->structList[struct_index].members[member_index].externalIndex =
										j_def["structs"][struct_index]["members"][member_index]["externalIndex"].get<int>();

									def->structList[struct_index].members[member_index].rangeLimit =
										j_def["structs"][struct_index]["members"][member_index]["rangeLimit"].get<unsigned int>();

									def->structList[struct_index].members[member_index].serverDelta =
										j_def["structs"][struct_index]["members"][member_index]["serverDelta"].get<unsigned int>();

									def->structList[struct_index].members[member_index].clientDelta =
										j_def["structs"][struct_index]["members"][member_index]["clientDelta"].get<unsigned int>();

									def->structList[struct_index].members[member_index].arraySize =
										j_def["structs"][struct_index]["members"][member_index]["arraySize"].get<int>();

									def->structList[struct_index].members[member_index].enumIndex =
										j_def["structs"][struct_index]["members"][member_index]["enumIndex"].get<int>();

									def->structList[struct_index].members[member_index].permission =
										j_def["structs"][struct_index]["members"][member_index]["permission"].get<int>();
								}
							}
						}
					}

					if (j_def["enums"].size())
					{
						def->enumCount = static_cast<int>(j_def["enums"].size());
						def->enumList = mem->allocate<DDLEnum>(def->enumCount);
						for (auto enum_index = 0; enum_index < def->enumCount; enum_index++)
						{
							def->enumList[enum_index].name = mem->duplicate_string(j_def["enums"][enum_index]["name"].get<std::string>());
							if (j_def["enums"][enum_index]["members"].size())
							{
								def->enumList[enum_index].memberCount = static_cast<int>(j_def["enums"][enum_index]["members"].size());
								def->enumList[enum_index].members = mem->allocate<const char*>(def->enumList[enum_index].memberCount);
								for (auto member_index = 0; member_index < def->enumList[enum_index].memberCount; member_index++)
								{
									def->enumList[enum_index].members[member_index] = mem->duplicate_string(
										j_def["enums"][enum_index]["members"][member_index].get<std::string>());
								}
							}
						}
					}

					def->headerBitSize = j_def["headerBitSize"].get<int>();
					def->headerByteSize = j_def["headerByteSize"].get<int>();
					def->reserveSize = j_def["reserveSize"].get<int>();
					def->userFlagsSize = j_def["userFlagsSize"].get<int>();
					def->paddingUsed = j_def["paddingUsed"].get<bool>();

					// Generate hash tables
					generateHashTables(def, mem);

					if (def_index == static_cast<int>(j_defs.size()) - 1)
					{
						def->next = nullptr;
						def = nullptr;
					}
					else
					{
						def->next = mem->allocate<DDLDef>();
						def = def->next;

						def_index++;
					}
				}

				return asset;
			}

			return nullptr;
		}

		void dump_as_json(DDLFile* asset)
		{
			const auto path = asset->name;
			auto file = filesystem::file("ddl\\"s + path + ".json"s);
			file.open("wb");

			ordered_json ddl;
			ddl["name"] = asset->name;

			assert(asset->ddlDef != nullptr);

			auto def = asset->ddlDef;
			int ddl_index = 0;

			ddl["defs"] = {};
			while (def)
			{
				ordered_json j_def;
				j_def["name"] = def->name;
				j_def["version"] = def->version;
				j_def["checksum"] = def->checksum; // checksum seems to be unused in the game
				j_def["flags"] = def->flags;
				j_def["bitSize"] = def->bitSize;
				j_def["byteSize"] = def->byteSize;

				j_def["structs"] = {};
				for (auto struct_index = 0; struct_index < def->structCount; struct_index++)
				{
					ordered_json j_struct;
					j_struct["name"] = def->structList[struct_index].name;
					j_struct["bitSize"] = def->structList[struct_index].bitSize;

					j_struct["members"] = {};
					for (auto member_index = 0; member_index < def->structList[struct_index].memberCount; member_index++)
					{
						ordered_json j_member;
						j_member["name"] = def->structList[struct_index].members[member_index].name;
						j_member["index"] = def->structList[struct_index].members[member_index].index;
						//j_member["parent"] = nullptr;
						//if (def->structList[struct_index].members[member_index].parent)
						//{
						//	j_member["parent"] = reinterpret_cast<DDLMember*>(def->structList[struct_index].members[member_index].parent)->name; // ??
						//}
						j_member["bitSize"] = def->structList[struct_index].members[member_index].bitSize;
						j_member["limitSize"] = def->structList[struct_index].members[member_index].limitSize;
						j_member["offset"] = def->structList[struct_index].members[member_index].offset;
						j_member["type"] = def->structList[struct_index].members[member_index].type;
						j_member["externalIndex"] = def->structList[struct_index].members[member_index].externalIndex;
						j_member["rangeLimit"] = def->structList[struct_index].members[member_index].rangeLimit;
						j_member["serverDelta"] = def->structList[struct_index].members[member_index].serverDelta;
						j_member["clientDelta"] = def->structList[struct_index].members[member_index].clientDelta;
						j_member["arraySize"] = def->structList[struct_index].members[member_index].arraySize;
						j_member["enumIndex"] = def->structList[struct_index].members[member_index].enumIndex;
						j_member["permission"] = def->structList[struct_index].members[member_index].permission;

						j_struct["members"][member_index] = j_member;
					}

					for (auto index = 0; index < def->structList[struct_index].hashTableUpper.count; index++)
					{
						j_struct["hashTableUpper"][index] = {
							{"hash", def->structList[struct_index].hashTableUpper.list[index].hash},
							{"index", def->structList[struct_index].hashTableUpper.list[index].index}
						};
					}

					//for (auto index = 0; index < def->structList[struct_index].hashTableLower.count; index++)
					//{
					//	j_struct["hashTableLower"][index] = {
					//		{"hash", def->structList[struct_index].hashTableLower.list[index].hash},
					//		{"index", def->structList[struct_index].hashTableLower.list[index].index}
					//	};
					//}

					j_def["structs"][struct_index] = j_struct;
				}

				j_def["enums"] = {};
				for (auto enum_index = 0; enum_index < def->enumCount; enum_index++)
				{
					ordered_json j_enum;
					j_enum["name"] = def->enumList[enum_index].name;

					j_enum["members"] = {};
					for (auto member_index = 0; member_index < def->enumList[enum_index].memberCount; member_index++)
					{
						j_enum["members"][member_index] = def->enumList[enum_index].members[member_index];
					}

					//for (auto index = 0; index < def->enumList[enum_index].hashTable.count; index++)
					//{
					//	j_enum["hashTable"][index] = {
					//		{"hash", def->enumList[enum_index].hashTable.list[index].hash},
					//		{"index", def->enumList[enum_index].hashTable.list[index].index}
					//	};
					//}

					j_def["enums"][enum_index] = j_enum;
				}

				j_def["headerBitSize"] = def->headerBitSize;
				j_def["headerByteSize"] = def->headerByteSize;
				j_def["reserveSize"] = def->reserveSize;
				j_def["userFlagsSize"] = def->userFlagsSize;
				j_def["paddingUsed"] = def->paddingUsed;

				ddl["defs"][ddl_index] = j_def;
				ddl_index++;

				def = def->next;
			}

			auto str = ddl.dump(4);
			file.write(str);

			file.close();
		}
	}
	using namespace ddl_json;

	class ddl_dumper
	{
	private:
		std::string buffer;
		std::string tab = "\t";
		unsigned int tabs = 0;
		unsigned int col = 0;

		void write_internal(const std::string& str)
		{
			if (!col) // beginning of line, indent
			{
				buffer.append(tabs * tab.size(), '\t');
				col += tabs * static_cast<unsigned int>(tab.size());
			}

			buffer.append(str);
			col += static_cast<unsigned int>(str.size());
		}

	public:
		ddl_dumper() = default;

		void write(const std::string& str) { write_internal(str); }

		void writeln(const std::string& str)
		{
			write_internal(str);
			buffer.push_back('\n');
			col = 0;
		}

		void writeln() { writeln(""); }

		void obracket() { writeln("{"); ++tabs; }
		void cbracket() { --tabs; writeln("}"); }

		void write_decoration(const std::string& str) { writeln("[" + str + "]"); }
		void write_decoration(const std::string& str, std::uint32_t value)
		{
			write_decoration(str + " " + std::to_string(value));
		}

		std::string& get_buffer() { return buffer; }
	};

	enum DDLTypeFixup : std::uint32_t
	{
		DDL_BOOL_TYPE_FIXUP = 11,
		DDL_INT8_TYPE_FIXUP = 12,
		DDL_UINT8_TYPE_FIXUP = 13,
		DDL_INT16_TYPE_FIXUP = 14,
		DDL_UINT16_TYPE_FIXUP = 15,
	};

	std::unordered_map<std::uint32_t, std::string> type_names =
	{
		{DDL_BYTE_TYPE, "byte"},
		{DDL_SHORT_TYPE, "short"},
		{DDL_UINT_TYPE, "uint"},
		{DDL_INT_TYPE, "int"},
		{DDL_UINT64_TYPE, "uint64"},
		{DDL_FLOAT_TYPE, "float"},
		{DDL_FIXEDPOINT_TYPE, "fixed"}, // unused
		{DDL_STRING_TYPE, "string"},

		{DDL_BOOL_TYPE_FIXUP, "bool"},
		{DDL_INT8_TYPE_FIXUP, "int8"},
		{DDL_UINT8_TYPE_FIXUP, "uint8"},
		{DDL_INT16_TYPE_FIXUP, "int16"},
		{DDL_UINT16_TYPE_FIXUP, "uint16"},
	};

	std::unordered_map<std::uint32_t, std::int32_t> type_sizes =
	{
		{DDL_BYTE_TYPE, 8},
		{DDL_SHORT_TYPE, 16},
		{DDL_UINT_TYPE, 32},
		{DDL_INT_TYPE, 32},
		{DDL_UINT64_TYPE, 64},
		{DDL_FLOAT_TYPE, 32},
		{DDL_FIXEDPOINT_TYPE, 0}, // unused
		{DDL_STRING_TYPE, 8}, // custom

		{DDL_BOOL_TYPE_FIXUP, 1},
		{DDL_INT8_TYPE_FIXUP, 8},
		{DDL_UINT8_TYPE_FIXUP, 8},
		{DDL_INT16_TYPE_FIXUP, 16},
		{DDL_UINT16_TYPE_FIXUP, 16},
	};

	std::unordered_map<std::uint32_t, std::uint64_t> type_limits =
	{
		{DDL_BYTE_TYPE, std::numeric_limits<byte>().max()},
		{DDL_SHORT_TYPE, std::numeric_limits<unsigned short>().max()},
		{DDL_UINT_TYPE, std::numeric_limits<unsigned int>().max()},
		{DDL_INT_TYPE, std::numeric_limits<int>().max()},
		{DDL_UINT64_TYPE, 0},
		{DDL_FLOAT_TYPE, std::numeric_limits<float>().max()},
		{DDL_FIXEDPOINT_TYPE, 0}, // unused
		{DDL_STRING_TYPE, 0}, // custom

		{DDL_BOOL_TYPE_FIXUP, std::numeric_limits<bool>().max()},
		{DDL_INT8_TYPE_FIXUP, std::numeric_limits<std::int8_t>().max()},
		{DDL_UINT8_TYPE_FIXUP, std::numeric_limits<std::uint8_t>().max()},
		{DDL_INT16_TYPE_FIXUP, std::numeric_limits<std::int16_t>().max()},
		{DDL_UINT16_TYPE_FIXUP, std::numeric_limits<std::uint16_t>().max()},
	};

	std::unordered_map<std::string, uint32_t> type_names_reverse =
	{
		{"byte", DDL_BYTE_TYPE},
		{"short", DDL_SHORT_TYPE},
		{"uint", DDL_UINT_TYPE},
		{"int", DDL_INT_TYPE},
		{"uint64", DDL_UINT64_TYPE},
		{"float", DDL_FLOAT_TYPE},
		{"fixed", DDL_FIXEDPOINT_TYPE}, // unused
		{"string", DDL_STRING_TYPE},

		{"bool", DDL_BOOL_TYPE_FIXUP},
		{"int8", DDL_INT8_TYPE_FIXUP},
		{"uint8", DDL_UINT8_TYPE_FIXUP},
		{"int16", DDL_INT16_TYPE_FIXUP},
		{"uint16", DDL_UINT16_TYPE_FIXUP},
	};

	static inline std::string trim(std::string s)
	{
		auto not_space = [](unsigned char c) { return !std::isspace(c); };
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
		s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
		return s;
	}

	static inline std::vector<std::string> split_csv_commas(const std::string& s)
	{
		std::vector<std::string> out;
		size_t start = 0;
		while (start <= s.size())
		{
			size_t comma = s.find(',', start);
			std::string tok = (comma == std::string::npos) ? s.substr(start) : s.substr(start, comma - start);
			tok = trim(tok);
			if (!tok.empty()) out.push_back(tok);
			if (comma == std::string::npos) break;
			start = comma + 1;
		}
		return out;
	}

	static inline int ceil_div_int(int a, int b)
	{
		return (a + b - 1) / b;
	}

	static inline int ceil_log2_int(uint64_t x)
	{
		if (x <= 1) return 0;
		int n = 0;
		uint64_t v = x - 1;
		while (v > 0) { v >>= 1; ++n; }
		return n;
	}

	DDLFile parseDDLFile(const std::string& input, const std::string& name, zone_memory* mem)
	{
		DDLFile out{};
		out.name = mem->duplicate_string(name);
		std::vector<DDLDef> defs;

		// Split input into lines
		std::istringstream iss(input);
		std::vector<std::string> lines;
		std::string line;
		while (std::getline(iss, line)) lines.push_back(line);

		// Line index
		size_t i = 0;
		size_t lastVersionLineIndex = 0;
		while (i < lines.size())
		{
			const std::string& l = lines[i];
			if (i + 1 < lines.size() && l.find("version") != std::string::npos && lines[i + 1].find("{") != std::string::npos)
			{
				// Parse version number
				unsigned short versionNumber = static_cast<unsigned short>(std::stoi(l.substr(l.find("version") + 7)));

				// --- Extract header for this version ---
				std::string headerPart;
				for (size_t h = lastVersionLineIndex; h < i + 1; ++h)
				{
					if (lines[h].find("version") != std::string::npos && lines[h + 1].find("{") != std::string::npos)
						break; // stop at previous version block
					headerPart += lines[h] + "\n";
				}

				// Base def flags (per version)
				struct DefBase { std::uint8_t flags = 0; int userFlagsSize = 0; int reserveSize = 0; unsigned int checksum; } defBase{};

				std::smatch m;
				if (std::regex_search(headerPart, m, std::regex(R"(\[userflags\s+(\d+)\])")))
				{
					defBase.flags |= DDLFlags::DDL_FLAG_USER_FLAGS;
					defBase.userFlagsSize = std::stoi(m[1].str());
				}
				if (std::regex_search(headerPart, m, std::regex(R"(\[reserve\s+(\d+)\])")))
				{
					defBase.flags |= DDLFlags::DDL_FLAG_RESERVE;
					defBase.reserveSize = std::stoi(m[1].str());
				}
				if (std::regex_search(headerPart, std::regex(R"(\[codeversion\])"))) defBase.flags |= DDLFlags::DDL_FLAG_CODE_VERSION;
				if (std::regex_search(headerPart, std::regex(R"(\[nopadding\])")))   defBase.flags |= DDLFlags::DDL_FLAG_NO_PADDING;
				if (std::regex_search(headerPart, std::regex(R"(\[checksum\])")))    defBase.flags |= DDLFlags::DDL_FLAG_CHECKSUM;
				if (std::regex_search(headerPart, std::regex(R"(\[ddlchecksum\])"))) defBase.flags |= DDLFlags::DDL_FLAG_DDL_CHECKSUM;

				if (std::regex_search(headerPart, m, std::regex(R"(\[defchecksum\s+(\d+)\])")))
				{
					defBase.checksum = std::stoul(m[1].str());
				}

				// Header bit size (per version)
				int headerBits = 16;
				if (defBase.flags & DDLFlags::DDL_FLAG_CHECKSUM)     headerBits += 32;
				if (defBase.flags & DDLFlags::DDL_FLAG_DDL_CHECKSUM) headerBits += 32;
				if (defBase.flags & DDLFlags::DDL_FLAG_CODE_VERSION) headerBits += 16;
				if (defBase.flags & DDLFlags::DDL_FLAG_USER_FLAGS)   headerBits += defBase.userFlagsSize * 8;
				if (defBase.flags & DDLFlags::DDL_FLAG_RESERVE)      headerBits += defBase.reserveSize * 8;

				// --- Collect block lines with brace counting ---
				size_t braceCount = 1;
				std::string versionBody;
				size_t j = i + 1;
				for (; j < lines.size() && braceCount > 0; ++j)
				{
					versionBody += lines[j] + "\n";
					braceCount += std::count(lines[j].begin(), lines[j].end(), '{');
					braceCount -= std::count(lines[j].begin(), lines[j].end(), '}');
					if (braceCount == 1) break;
				}

				// --- Build DDLDef with per-version header ---
				DDLDef def{};
				def.name = out.name;
				def.version = versionNumber;
				def.flags = defBase.flags;
				def.headerBitSize = headerBits;
				def.headerByteSize = ceil_div_int(headerBits, 8);
				def.reserveSize = defBase.reserveSize;
				def.userFlagsSize = defBase.userFlagsSize;
				def.paddingUsed = !(def.flags & DDLFlags::DDL_FLAG_NO_PADDING);
				
				// use base checksum if present, otherwise we should compute it ourselves from the version block?
				def.checksum = defBase.checksum;

				std::vector<DDLEnum> enums;
				std::unordered_map<std::string, int> enumNameToIndex;

				// --- Parse enums ---
				size_t k = 0;
				while (k < versionBody.size())
				{
					std::smatch em;
					std::string remaining = versionBody.substr(k);
					std::regex enumLine(R"(enum\s+(\w+)\s*\{)");
					if (!std::regex_search(remaining, em, enumLine)) break;

					std::string enumName = em[1].str();
					size_t enumStart = em.position() + em.length();
					int enumBrace = 1;
					size_t idx = enumStart;
					while (idx < remaining.size() && enumBrace > 0)
					{
						if (remaining[idx] == '{') enumBrace++;
						else if (remaining[idx] == '}') enumBrace--;
						idx++;
					}
					std::string membersCsv = remaining.substr(enumStart, idx - enumStart - 1);
					DDLEnum e{};
					e.name = mem->duplicate_string(enumName);
					auto mems = split_csv_commas(membersCsv);
					e.memberCount = static_cast<int>(mems.size());
					e.members = mem->allocate<const char*>(e.memberCount);
					for (int mi = 0; mi < e.memberCount; ++mi)
						e.members[mi] = mem->duplicate_string(trim(mems[mi]));

					if (!enumNameToIndex.contains(e.name))
					{
						enumNameToIndex[e.name] = static_cast<int>(enums.size());
					}
					else
					{
						enumNameToIndex[e.name + "_dup_"s + std::to_string(enums.size())] = static_cast<int>(enums.size());
						ZONETOOL_WARNING("Duplicate enum name: %s in version %d", e.name, def.version);
					}

					enums.push_back(std::move(e));
					k += idx;
				}

				// --- Parse structs ---
				std::vector<std::pair<std::string, std::string>> structBodies;
				{
					std::regex structLine(R"(struct\s+(\w+)\s*\{)");
					size_t pos = 0;
					while (pos < versionBody.size())
					{
						std::smatch sm;
						std::string remaining = versionBody.substr(pos);
						if (!std::regex_search(remaining, sm, structLine)) break;

						std::string structName = sm[1].str();
						size_t structStart = sm.position() + sm.length();
						int structBrace = 1;
						size_t idx = structStart;
						while (idx < remaining.size() && structBrace > 0)
						{
							if (remaining[idx] == '{') structBrace++;
							else if (remaining[idx] == '}') structBrace--;
							idx++;
						}
						structBodies.emplace_back(structName, remaining.substr(structStart, idx - structStart - 1));
						pos += idx;
					}
				}

				// Struct index mapping
				std::unordered_map<std::string, int> structNameToIndex;
				int sidx = 0;
				for (const auto& kv : structBodies)
				{
					if (!structNameToIndex.contains(kv.first))
					{
						structNameToIndex[kv.first] = sidx++;
					}
					else
					{
						structNameToIndex[kv.first + "_dup_"s + std::to_string(sidx)] = sidx++;
						ZONETOOL_WARNING("Duplicate struct name: %s in version %d", kv.first.data(), def.version);
					}
				}

				// Parsed struct cache
				std::unordered_map<std::string, DDLStruct> parsedStructs;

				std::function<DDLStruct& (const std::string&)> parseStruct;
				parseStruct = [&](const std::string& name) -> DDLStruct&
				{
					auto itc = parsedStructs.find(name);
					if (itc != parsedStructs.end()) return itc->second;

					auto itBody = std::find_if(
						structBodies.begin(), structBodies.end(),
						[&name](const auto& pair) { return pair.first == name; }
					);
					if (itBody == structBodies.end())
						throw std::runtime_error("Struct " + name + " not found");

					DDLStruct st{};
					st.name = mem->duplicate_string(name);
					st.bitSize = 0;
					std::vector<DDLMember> members;
					int offset = 0, index = 0;

					std::regex fieldRe(R"(^\s*([\w]+)(?::(\d+))?(?:\((\d+)\))?\s+(\w+)(?:\[\s*(\w+)\s*\])?;)");
					std::string& body = itBody->second;
					for (std::sregex_iterator fit(body.begin(), body.end(), fieldRe), fend; fit != fend; ++fit)
					{
						auto fm = *fit;
						std::string rawType = fm[1].str();
						std::string rawBits = fm[2].matched ? fm[2].str() : "";
						std::string rawLimit = fm[3].matched ? fm[3].str() : "";
						std::string fieldName = fm[4].str();
						std::string arrayLen = fm[5].matched ? fm[5].str() : "";

						// Resolve type and external index
						int dtype = DDLType::DDL_BYTE_TYPE;
						int externalIndex = 0;
						if (auto itT = type_names_reverse.find(rawType); itT != type_names_reverse.end()) dtype = itT->second;
						else if (enumNameToIndex.contains(rawType))
						{
							dtype = DDLType::DDL_ENUM_TYPE;
							externalIndex = enumNameToIndex[rawType];
						}
						else if (structNameToIndex.contains(rawType))
						{
							dtype = DDLType::DDL_STRUCT_TYPE;
							externalIndex = structNameToIndex[rawType];
						}

						// Handle size overrides (string/int/uint with explicit limit)
						int sizeOverride = -1;

						// Handle limit override
						if (!rawLimit.empty())
						{
							if (rawType == "string")
							{
								sizeOverride = std::stoi(rawLimit) * 8;
								rawLimit.clear();
							}
						}

						// Handle bit size override
						if (!rawBits.empty())
						{
							if (dtype != DDLType::DDL_STRUCT_TYPE &&
								dtype != DDLType::DDL_ENUM_TYPE &&
								dtype != DDLType::DDL_STRING_TYPE)
							{
								sizeOverride = std::stoi(rawBits);
								if (sizeOverride > type_limits[dtype])
								{
									ZONETOOL_WARNING("Field %s in struct %s has bit size override %d exceeding type limit %llu, clamping",
										fieldName.data(), name.data(), sizeOverride, type_limits[dtype]);
									sizeOverride = static_cast<int>(type_limits[dtype]);
								}
							}
							else
							{
								ZONETOOL_WARNING("Field %s in struct %s has bit size override %d but type %s does not support it, ignoring",
									fieldName.data(), name.data(), std::stoi(rawBits), rawType.data());
							}
						}

						// Compute type size in bits
						int typeSizeBits = 0;
						if (dtype == DDLType::DDL_STRUCT_TYPE)
						{
							typeSizeBits = parseStruct(rawType).bitSize;
						}
						else if (dtype == DDLType::DDL_ENUM_TYPE)
						{
							const DDLEnum& e = enums[externalIndex];
							int count = static_cast<int>(e.memberCount);
							int bitsNeeded = std::max(1, ceil_log2_int(count));
							typeSizeBits = (count == 2) ? 1 : ceil_div_int(bitsNeeded, 8) * 8;
						}
						else
						{
							typeSizeBits = (sizeOverride >= 0) ? sizeOverride : type_sizes[dtype];
						}

						// Resolve array size
						int arraySize = 1;
						if (!arrayLen.empty())
						{
							try
							{
								arraySize = std::stoi(arrayLen);
							}
							catch (...)
							{
								if (enumNameToIndex.count(arrayLen))
								{
									arraySize = enums[enumNameToIndex[arrayLen]].memberCount;
								}
							}
						}

						// Resolve range/limits
						size_t limit = (!rawLimit.empty()) ? std::stoi(rawLimit)
							: (type_limits.count(dtype) ? type_limits[dtype] : 0);

						const auto maxSigned = [](int bits)
						{
							if (bits >= 32) return INT32_MAX;
							return static_cast<int32_t>((1ULL << (bits - 1)) - 1);
						};

						const auto maxUnsigned = [](int bits)
						{
							if (bits >= 32) return UINT32_MAX;
							return static_cast<uint32_t>((1ULL << bits) - 1);
						};

						if (rawLimit.empty())
						{
							if (dtype == DDLType::DDL_UINT_TYPE)
							{
								limit = maxUnsigned(typeSizeBits);
							}
							else if (dtype == DDLType::DDL_INT_TYPE)
							{
								limit = maxSigned(typeSizeBits);
							}
						}

						if (!sizeOverride && 
							dtype != DDLType::DDL_FLOAT_TYPE &&
							dtype != DDLType::DDL_STRUCT_TYPE &&
							dtype != DDLType::DDL_ENUM_TYPE &&
							dtype != DDLType::DDL_STRING_TYPE)
						{
							int bitsNeeded = ceil_log2_int(limit + 1);
							typeSizeBits = std::max(typeSizeBits, bitsNeeded);
						}

						// Compute total bit size
						int bitSize = arraySize * typeSizeBits;

						DDLMember member{};
						member.name = mem->duplicate_string(fieldName);
						member.index = (dtype == DDLType::DDL_ENUM_TYPE || dtype == DDLType::DDL_STRUCT_TYPE) ? 0 : index;
						member.bitSize = bitSize;
						member.limitSize = 0;
						member.offset = offset;
						member.externalIndex = externalIndex;
						member.rangeLimit = static_cast<unsigned int>(limit);
						member.serverDelta = static_cast<unsigned int>(limit);
						member.clientDelta = static_cast<unsigned int>(limit);
						member.arraySize = arraySize;
						member.enumIndex = 0;
						member.permission = 3;

						member.type = 
							dtype == DDL_BOOL_TYPE_FIXUP ? DDLType::DDL_UINT_TYPE :
							dtype == DDL_INT8_TYPE_FIXUP ? DDLType::DDL_UINT_TYPE :
							dtype == DDL_UINT8_TYPE_FIXUP ? DDLType::DDL_UINT_TYPE :
							dtype == DDL_INT16_TYPE_FIXUP ? DDLType::DDL_UINT_TYPE :
							dtype == DDL_UINT16_TYPE_FIXUP ? DDLType::DDL_UINT_TYPE :
							dtype;

						if (dtype != DDLType::DDL_STRUCT_TYPE && dtype != DDLType::DDL_ENUM_TYPE && dtype != DDLType::DDL_STRING_TYPE)
						{
							member.limitSize = typeSizeBits;
							if (member.type == DDLType::DDL_UINT_TYPE)
							{
								member.limitSize = ceil_log2_int(limit + 1);
							}
						}

						if (!arrayLen.empty() && enumNameToIndex.count(arrayLen))
							member.enumIndex = enumNameToIndex[arrayLen];
						else if (arraySize > 1)
							member.enumIndex = -1;

						members.push_back(member);
						offset += member.bitSize;
						++index;
					}

					if (def.paddingUsed)
					{
						int padBits = (8 - (offset % 8)) % 8;
						if (padBits > 0)
						{
							DDLMember pad{};
							pad.name = "__pad";
							pad.index = index++;
							pad.bitSize = pad.limitSize = padBits;
							pad.offset = offset;
							pad.type = DDLType::DDL_PAD_TYPE;
							pad.externalIndex = 0;
							pad.rangeLimit = pad.serverDelta = pad.clientDelta = 0;
							pad.arraySize = 1;
							pad.enumIndex = 0;
							pad.permission = 3;
							members.push_back(pad);
							offset += padBits;
						}
					}

					st.bitSize = offset;
					st.memberCount = static_cast<int>(members.size());
					st.members = mem->allocate<DDLMember>(st.memberCount);
					for (size_t mi = 0; mi < st.memberCount; ++mi)
						st.members[mi] = std::move(members[mi]);

					auto [itIns, _] = parsedStructs.emplace(name, std::move(st));
					return itIns->second;
				};

				std::vector<DDLStruct> structs;
				for (const auto& kv : structBodies) structs.push_back(parseStruct(kv.first));

				auto itRoot = std::find_if(structs.begin(), structs.end(), [](const DDLStruct& s) { return s.name == "root"s; });
				if (itRoot == structs.end()) throw std::runtime_error("Root struct not found");

				def.bitSize = itRoot->bitSize + def.headerBitSize;
				def.byteSize = ceil_div_int(def.bitSize, 8);

				// Copy structs
				def.structCount = static_cast<int>(structs.size());
				def.structList = mem->allocate<DDLStruct>(def.structCount);
				for (int si_ = 0; si_ < def.structCount; ++si_) def.structList[si_] = std::move(structs[si_]);

				// Copy enums
				def.enumCount = static_cast<int>(enums.size());
				def.enumList = mem->allocate<DDLEnum>(def.enumCount);
				for (int ei_ = 0; ei_ < def.enumCount; ++ei_) def.enumList[ei_] = std::move(enums[ei_]);

				// Generate hash tables
				generateHashTables(&def, mem);

				defs.push_back(std::move(def));
				i = j; // skip to end of version block
				lastVersionLineIndex = i;
			}
			else ++i;
		}

		out.ddlDef = mem->allocate<DDLDef>(defs.size());
		for (size_t di_ = 0; di_ < defs.size(); ++di_)
		{
			out.ddlDef[di_] = std::move(defs[di_]);
			out.ddlDef[di_].next = (di_ + 1 < defs.size()) ? &out.ddlDef[di_ + 1] : nullptr;
		}

		return out;
	}

	DDLFile* ddl::parse(const std::string& name, zone_memory* mem)
	{
		// parse from json if found
		if (auto* jfile = parse_from_json(name, mem))
		{
			return jfile;
		}

		auto file = filesystem::file(name);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing ddl \"%s\"...", name.data());

		// parse the DDL file as a string
		file.open("rb");
		std::string data{};
		file.read_string(&data);
		file.close();

		// parse ddl
		auto ddl = parseDDLFile(data, name, mem);
		auto asset = mem->allocate<DDLFile>();
		std::memcpy(asset, &ddl, sizeof(DDLFile));

		return asset;
	}

	void ddl::dump(DDLFile* asset)
	{
		assert(asset && asset->ddlDef);

#ifdef DUMP_DDL_JSON
		dump_as_json(asset);
		return;
#endif

		filesystem::file file(asset->name);
		file.open("wb");

		ddl_dumper dumper;

		// Local helper lambdas
		auto write_decorations = [&](const DDLDef* def)
		{
			if (def->flags & DDL_FLAG_USER_FLAGS)   dumper.write_decoration("userflags", def->userFlagsSize);
			if (def->flags & DDL_FLAG_RESERVE)      dumper.write_decoration("reserve", def->reserveSize);
			if (def->flags & DDL_FLAG_CODE_VERSION) dumper.write_decoration("codeversion");
			if (def->flags & DDL_FLAG_NO_PADDING)   dumper.write_decoration("nopadding");
			if (def->flags & DDL_FLAG_CHECKSUM)     dumper.write_decoration("checksum");
			if (def->flags & DDL_FLAG_DDL_CHECKSUM) dumper.write_decoration("ddlchecksum");

			// custom decorations
			if (def->checksum != 0)                 dumper.write_decoration("defchecksum", def->checksum);
		};

		auto write_enum = [&](const DDLEnum& e)
		{
			dumper.writeln("enum " + std::string(e.name));
			dumper.obracket();
			for (int mi = 0; mi < e.memberCount; ++mi)
			{
				dumper.write(e.members[mi]);
				dumper.writeln(mi < e.memberCount - 1 ? "," : "");
			}
			dumper.cbracket();
		};

		auto write_member = [&](const DDLDef* def, const DDLMember& m)
		{
			if (m.type == DDL_PAD_TYPE) return;

			std::string typeName;
			if (m.type == DDL_STRUCT_TYPE)      typeName = def->structList[m.externalIndex].name;
			else if (m.type == DDL_ENUM_TYPE)   typeName = def->enumList[m.externalIndex].name;
			else                                typeName = type_names.at(m.type);

			if (m.type == DDL_UINT_TYPE && m.bitSize / m.arraySize == type_sizes[DDL_BOOL_TYPE_FIXUP] && m.rangeLimit == type_limits[DDL_BOOL_TYPE_FIXUP]) typeName = "bool";
			else if (m.type == DDL_UINT_TYPE && m.bitSize / m.arraySize == type_sizes[DDL_INT8_TYPE_FIXUP]) if (m.rangeLimit == type_limits[DDL_INT8_TYPE_FIXUP]) typeName = "int8"; else typeName = "uint8";
			else if (m.type == DDL_UINT_TYPE && m.bitSize / m.arraySize == type_sizes[DDL_INT16_TYPE_FIXUP]) if (m.rangeLimit == type_limits[DDL_INT16_TYPE_FIXUP]) typeName = "int16"; else typeName = "uint16";

			std::uint32_t type = type_names_reverse[typeName];

			dumper.write(typeName);

			if (m.type != DDL_STRUCT_TYPE && m.type != DDL_ENUM_TYPE && m.type != DDL_PAD_TYPE)
			{
				if (m.type == DDL_STRING_TYPE)
					dumper.write("(" + std::to_string(m.bitSize / 8) + ")");
				else
				{
					if (type_sizes.at(type) != m.bitSize / m.arraySize)
						dumper.write(":" + std::to_string(m.bitSize / m.arraySize));
					if (m.limitSize != m.bitSize / m.arraySize && type_limits.at(type) != m.rangeLimit)
						dumper.write("(" + std::to_string(m.rangeLimit) + ")");
				}
			}

			dumper.write(" " + std::string(m.name));

			if (m.arraySize > 1)
			{
				if (m.enumIndex != -1)
					dumper.write("[ " + std::string(def->enumList[m.enumIndex].name) + " ]");
				else
					dumper.write("[ " + std::to_string(m.arraySize) + " ]");
			}

			dumper.writeln(";");
		};

		auto write_struct = [&](const DDLDef* def, const DDLStruct& s)
		{
			dumper.writeln("struct " + std::string(s.name));
			dumper.obracket();
			for (int mi = 0; mi < s.memberCount; ++mi)
				write_member(def, s.members[mi]);
			dumper.cbracket();
		};

		for (DDLDef* def = asset->ddlDef; def; def = def->next)
		{
			write_decorations(def);
			dumper.writeln("version " + std::to_string(def->version));
			dumper.obracket();

			// Enums
			for (int ei_ = 0; ei_ < def->enumCount; ++ei_)
			{
				write_enum(def->enumList[ei_]);
				if (ei_ < def->enumCount - 1) dumper.writeln();
			}

			if (def->enumCount && def->structCount) dumper.writeln();

			// Structs
			for (int si_ = 0; si_ < def->structCount; ++si_)
			{
				write_struct(def, def->structList[si_]);
				if (si_ < def->structCount - 1) dumper.writeln();
			}

			dumper.cbracket();
			if (def->next) dumper.writeln();
		}

		file.write(dumper.get_buffer());
		file.close();
	}

	void ddl::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).ddlFile;
		}
	}

	void ddl::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void ddl::load_depending(zone_base* zone)
	{
	}

	std::string ddl::name()
	{
		return this->name_;
	}

	std::int32_t ddl::type()
	{
		return ASSET_TYPE_DDL;
	}

	DDLDef* write_ddldef(zone_buffer* buf, DDLDef* data)
	{
		auto dest = buf->write(data);

		if (data->name)
		{
			dest->name = buf->write_str(data->name);
		}

		if (data->structList)
		{
			buf->align(7);

			auto* dest_struct_list = buf->write(data->structList, data->structCount);
			auto* data_struct_list = data->structList;
			for (auto struct_index = 0; struct_index < data->structCount; struct_index++)
			{
				if (data_struct_list[struct_index].name)
				{
					dest_struct_list[struct_index].name = buf->write_str(data_struct_list[struct_index].name);
				}

				if (data_struct_list[struct_index].members)
				{
					buf->align(7);

					auto* dest_members = buf->write(data_struct_list[struct_index].members, data_struct_list[struct_index].memberCount);
					auto* data_members = data_struct_list[struct_index].members;
					for (auto member_index = 0; member_index < data_struct_list[struct_index].memberCount; member_index++)
					{
						if (data_members[member_index].name)
						{
							dest_members[member_index].name = buf->write_str(data_members[member_index].name);
						}

						if (data_members[member_index].parent)
						{
							dest_members[member_index].parent = nullptr; // ??
						}
					}

					buf->clear_pointer(&dest_struct_list[struct_index].members);
				}

				if (data_struct_list[struct_index].hashTableUpper.list)
				{
					buf->align(3);
					buf->write(data_struct_list[struct_index].hashTableUpper.list, data_struct_list[struct_index].hashTableUpper.count);
					buf->clear_pointer(&dest_struct_list[struct_index].hashTableUpper.list);
				}

				if (data_struct_list[struct_index].hashTableLower.list)
				{
					buf->align(3);
					buf->write(data_struct_list[struct_index].hashTableLower.list, data_struct_list[struct_index].hashTableLower.count);
					buf->clear_pointer(&dest_struct_list[struct_index].hashTableLower.list);
				}
			}

			buf->clear_pointer(&dest->structList);
		}

		if (data->enumList)
		{
			buf->align(7);

			auto* dest_enum_list = buf->write(data->enumList, data->enumCount);
			auto* data_enum_list = data->enumList;
			for (auto enum_index = 0; enum_index < data->enumCount; enum_index++)
			{
				if (data_enum_list[enum_index].name)
				{
					dest_enum_list[enum_index].name = buf->write_str(data_enum_list[enum_index].name);
				}

				if (data_enum_list[enum_index].members)
				{
					buf->align(7);
					auto* strings = buf->write(data_enum_list[enum_index].members, data_enum_list[enum_index].memberCount);
					for (auto member_index = 0; member_index < data_enum_list[enum_index].memberCount; member_index++)
					{
						if (data_enum_list[enum_index].members[member_index])
						{
							strings[member_index] = buf->write_str(data_enum_list[enum_index].members[member_index]);
						}
					}
					buf->clear_pointer(&dest_enum_list[enum_index].members);
				}

				if (data_enum_list[enum_index].hashTable.list)
				{
					buf->align(3);
					buf->write(data_enum_list[enum_index].hashTable.list, data_enum_list[enum_index].hashTable.count);
					buf->clear_pointer(&dest_enum_list[enum_index].hashTable.list);
				}
			}

			buf->clear_pointer(&dest->enumList);
		}

		if (data->next)
		{
			buf->align(7);
			write_ddldef(buf, data->next);
			buf->clear_pointer(&dest->next);
		}

		return dest;
	}

	void ddl::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->ddlDef)
		{
			buf->align(7);
			write_ddldef(buf, data->ddlDef);
			buf->clear_pointer(&dest->ddlDef);
		}

		buf->pop_stream();
	}
}