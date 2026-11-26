#include <std_include.hpp>
#include "nativescriptpatch.hpp"

namespace zonetool::s4
{
	void native_script_patch::read(zone_reader& reader, NativeScriptPatch* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		reader.read_asset(ASSET_TYPE_SCRIPTFILE, asset->scriptFile);

		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0; i < asset->entryCount; i++)
			{
				reader.read_string(entries[i].name);
			}
		}
		reader.pop_stream();
	}
}
