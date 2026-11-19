#pragma once

namespace zonetool::h1::converter::s2::scriptable
{
	struct stream_allocator
	{
		unsigned int part_base;
		unsigned int cursor;
		unsigned int part_size;

		unsigned char allocate(const unsigned int size)
		{
			const auto index = this->cursor;
			this->cursor += size;
			this->part_size = std::max(this->part_size, this->cursor - this->part_base);
			return static_cast<unsigned char>(index);
		}
	};

	struct def_context
	{
		ScriptableDef* asset;
		std::vector<zonetool::s2::XModel*> models;
		std::vector<unsigned char> part_anim_entries;
	};

	zonetool::s2::ScriptableMapEnts convert_map_ents(const ScriptableMapEnts& map_ents, utils::memory::allocator& allocator);
	void dump(ScriptableDef* asset);
	void finish();
}
