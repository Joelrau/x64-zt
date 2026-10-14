#pragma once

namespace zonetool::s2
{
	class skeleton_script
	{
	public:
		static void read(zone_reader& reader, SkeletonScript* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);

			auto& code = asset->code;
			reader.read_array(code.bytes, 0, code.byteCount);
			reader.read_array(code.ints, 3, code.intCount);
			reader.read_array(code.floats, 3, code.floatCount);
			reader.read_script_string_array(code.scriptStrings, code.scriptStringCount);

			reader.read_array(asset->data, 3, asset->dataCount);
			reader.pop_stream();
		}
	};
}
