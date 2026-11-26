#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	void read_clip_map_mesh(auto& reader, ClipMapMesh& mesh);
	void read_clip_map_table(auto& reader, ClipMapTable& table);
	void read_clip_map_models(auto& reader, ClipMapModel**& models, std::size_t count);

	class clip_map
	{
	public:
		static void read(auto& reader, ClipMap* asset);
		static void dump(ClipMap* asset);
	};
}
