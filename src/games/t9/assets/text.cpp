#include <std_include.hpp>
#include "text.hpp"

namespace zonetool::t9
{
	void raw_text_file::read(auto& reader, RawTextFile* asset)
	{
		reader.read_array(asset->buffer, 15, asset->length + 1);
	}

	void anim_tree::read(auto& reader, AnimTree* asset)
	{
		reader.read_array(asset->data, 15, asset->size);
	}

	void raw_string::read(auto& reader, RawString* asset)
	{
		reader.read_string(asset->string);
	}
}

namespace zonetool::t9
{
	template void raw_text_file::read(zone_reader&, RawTextFile*);
	template void anim_tree::read(zone_reader&, AnimTree*);
	template void raw_string::read(zone_reader&, RawString*);
	template void raw_text_file::read(zone_writer&, RawTextFile*);
	template void anim_tree::read(zone_writer&, AnimTree*);
	template void raw_string::read(zone_writer&, RawString*);
}
