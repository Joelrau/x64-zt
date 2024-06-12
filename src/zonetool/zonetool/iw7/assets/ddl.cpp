#include <std_include.hpp>
#include "ddl.hpp"

namespace zonetool::iw7
{
	class ddl_dumper
	{
	private:
		std::string buffer;

		std::string tab = "\t";
		unsigned int tabs = 0;
		unsigned int col = 0;

		void write_internal(const std::string& str)
		{
			if (!col)
			{
				for (auto i = 0u; i < tabs; i++)
				{
					buffer.append(tab);
					col += static_cast<unsigned int>(tab.size());
				}
			}

			buffer.append(str);
			col += static_cast<unsigned int>(str.size());
		}

		void initialize()
		{
			buffer.clear();
			tabs = 0;
		}

	public:
		ddl_dumper()
		{
			initialize();
		};

		~ddl_dumper()
		{
			initialize();
		};

		void write(const std::string str)
		{
			write_internal(str);
		}

		void writeln(const std::string str)
		{
			write(str);
			write("\n");
			col = 0;
		}

		void writeln()
		{
			writeln("");
		}

		void obracket()
		{
			writeln("{");
			tabs++;
		}

		void cbracket()
		{
			tabs--;
			writeln("}");
		}

		void write_decoration(const std::string str)
		{
			writeln("["s + str + "]"s);
		}

		void write_decoration(const std::string str, const std::uint32_t value)
		{
			write_decoration(str + " " + std::to_string(value));
		}

		std::string& get_buffer()
		{
			return buffer;
		}
	};

	std::unordered_map<std::uint32_t, std::string> type_names =
	{
		{DDL_BYTE_TYPE, "byte"},
		{DDL_SHORT_TYPE, "short"},
		{DDL_BOOL_TYPE, "bool"},
		{DDL_INT_TYPE, "int"},
		{DDL_UINT64_TYPE, "uint64"},
		{DDL_FLOAT_TYPE, "float"},
		{DDL_FIXEDPOINT_TYPE, "fixed"}, // unused
		{DDL_STRING_TYPE, "string"},
	};

	std::unordered_map<std::uint32_t, std::uint32_t> type_sizes =
	{
		{DDL_BYTE_TYPE, 8},
		{DDL_SHORT_TYPE, 16},
		{DDL_BOOL_TYPE, 1},
		{DDL_INT_TYPE, 32},
		{DDL_UINT64_TYPE, 64},
		{DDL_FLOAT_TYPE, 32},
		{DDL_FIXEDPOINT_TYPE, 0}, // unused
		{DDL_STRING_TYPE, 1}, // custom
	};

	std::unordered_map<std::uint32_t, std::uint64_t> type_limits =
	{
		{DDL_BYTE_TYPE, std::numeric_limits<byte>().max()},
		{DDL_SHORT_TYPE, std::numeric_limits<short>().max()},
		{DDL_BOOL_TYPE, std::numeric_limits<bool>().max()},
		{DDL_INT_TYPE, std::numeric_limits<int>().max()},
		{DDL_UINT64_TYPE, std::numeric_limits<std::uint64_t>().max()},
		{DDL_FLOAT_TYPE, std::numeric_limits<float>().max()},
		{DDL_FIXEDPOINT_TYPE, 0}, // unused
		{DDL_STRING_TYPE, 0}, // custom
	};

	void ddl::dump(DDLFile* asset)
	{
		assert(asset->ddlDef != nullptr);

		const auto path = asset->name;
		auto file = filesystem::file(path);
		file.open("wb");

		ddl_dumper dumper;

		auto def = asset->ddlDef;

		// write header

		if ((def->flags & DDLFLags::DDL_FLAG_USER_FLAGS) != 0)
		{
			dumper.write_decoration("userflags", def->userFlagsSize);
		}
		if ((def->flags & DDLFLags::DDL_FLAG_RESERVE) != 0)
		{
			dumper.write_decoration("reserve", def->reserveSize);
		}
		if ((def->flags & DDLFLags::DDL_FLAG_CODE_VERSION) != 0)
		{
			dumper.write_decoration("codeversion");
		}
		if ((def->flags & DDLFLags::DDL_FLAG_NO_PADDING) != 0)
		{
			dumper.write_decoration("nopadding");
		}
		if ((def->flags & DDLFLags::DDL_FLAG_CHECKSUM) != 0)
		{
			dumper.write_decoration("checksum");
		}
		if ((def->flags & DDLFLags::DDL_FLAG_DDL_CHECKSUM) != 0)
		{
			dumper.write_decoration("ddlchecksum");
		}

		while (def)
		{
			// write version

			dumper.writeln("version "s + std::to_string(def->version));
			dumper.obracket();

			// write enum and structs

			for (auto enum_index = 0; enum_index < def->enumCount; enum_index++)
			{
				dumper.writeln("enum "s + def->enumList[enum_index].name);
				dumper.obracket();

				for (auto member_index = 0; member_index < def->enumList[enum_index].memberCount; member_index++)
				{
					dumper.writeln(def->enumList[enum_index].members[member_index] + ","s);
				}

				dumper.cbracket();

				if (enum_index < def->enumCount - 1)
				{
					dumper.writeln();
				}
			}

			if (def->enumCount && def->structCount)
			{
				dumper.writeln();
			}

			for (auto struct_index = 0; struct_index < def->structCount; struct_index++)
			{
				dumper.writeln("struct "s + def->structList[struct_index].name);
				dumper.obracket();

				for (auto member_index = 0; member_index < def->structList[struct_index].memberCount; member_index++)
				{
					auto* member = &def->structList[struct_index].members[member_index];

					if (member->type == DDL_PAD_TYPE)
					{
						continue;
					}
					else if (member->type == DDL_STRUCT_TYPE)
					{
						dumper.write(def->structList[member->externalIndex].name);
					}
					else if (member->type == DDL_ENUM_TYPE)
					{
						dumper.write(def->enumList[member->externalIndex].name);
					}
					else
					{
						dumper.write(type_names[member->type]);

						if (member->type == DDL_INT_TYPE || member->type == DDL_STRING_TYPE)
						{
							if (member->rangeLimit && member->rangeLimit != type_limits[member->type])
							{
								// does this not work or did no one ever use ranges?
								dumper.write("("s + std::to_string(member->rangeLimit) + ")"s);
							}
							else if (member->bitSize / member->arraySize != static_cast<int>(type_sizes[member->type]))
							{
								dumper.write(":"s + std::to_string(member->bitSize / member->arraySize));
							}
						}
					}

					dumper.write(" ");
					dumper.write(member->name);

					if (member->arraySize > 1)
					{
						dumper.write("[ "s + std::to_string(member->arraySize) + " ]"s);
					}

					dumper.writeln(";");
				}

				dumper.cbracket();

				if (struct_index < def->structCount - 1)
				{
					dumper.writeln();
				}
			}

			dumper.cbracket();

			def = def->next;

			if (def)
			{
				dumper.writeln();
			}
		}

		file.write(dumper.get_buffer());
		file.close();
	}
}