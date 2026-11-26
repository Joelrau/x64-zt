#include <std_include.hpp>
#include "rawfile.hpp"

namespace zonetool::t9
{
	namespace
	{
		void write_file(const std::string& path, const void* data, const std::size_t size)
		{
			filesystem::file file(path);
			file.open("wb");
			if (file.get_fp() && data)
			{
				file.write(data, size, 1);
			}

			file.close();
		}
	}

	void rawfile::read(auto& reader, RawFile* asset)
	{
		reader.read_array(asset->buffer, 15, asset->len + 1);
	}

	void rawfile::dump(RawFile* asset)
	{
		write_file(std::format("rawfile/{}", xhash_name(asset->name)), asset->buffer, std::max(0, asset->len));
	}

	void script_parse_tree::read(auto& reader, ScriptParseTree* asset)
	{
		reader.read_array(asset->buffer, 31, asset->len + 1);
	}

	void script_parse_tree::dump(ScriptParseTree* asset)
	{
		write_file(std::format("scriptparsetree/{}.gscc", xhash_name(asset->name)), asset->buffer, std::max(0, asset->len));
	}
}

namespace zonetool::t9
{
	template void rawfile::read(zone_reader&, RawFile*);
	template void script_parse_tree::read(zone_reader&, ScriptParseTree*);
	template void rawfile::read(zone_writer&, RawFile*);
	template void script_parse_tree::read(zone_writer&, ScriptParseTree*);
}
