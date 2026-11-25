#include <std_include.hpp>
#include "scriptfile.hpp"

namespace zonetool::iw5
{
	void scriptfile::read(zone_reader32& reader, ScriptFile* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		reader.push_stream(XFILE_BLOCK_SCRIPT);
		reader.read_array(asset->buffer, 0, asset->compressedLen);
		reader.pop_stream();

		reader.push_stream(XFILE_BLOCK_SCRIPT);
		reader.read_array(asset->bytecode, 0, asset->bytecodeLen);
		reader.pop_stream();

		reader.pop_stream();
	}
}
