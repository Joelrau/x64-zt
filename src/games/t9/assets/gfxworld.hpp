#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	struct static_model_instance
	{
		XHash model;
		StaticModelPlacement placement;
		std::uint32_t deformation_index;
	};

	std::vector<static_model_instance> read_static_models(const Districts* asset);

	class gfx_world
	{
	public:
		static constexpr std::size_t alignment = 15;

		static void read(auto& reader, GfxWorld* asset);
		static void dump(GfxWorld* asset);
	};

	class group_lod_model
	{
	public:
		static void read(auto& reader, GroupLodModel* asset);
	};

	class static_level_fx_list
	{
	public:
		static void read(auto& reader, StaticLevelFxList* asset);
	};

	class districts
	{
	public:
		static void read(auto& reader, Districts* asset);
		static void dump(Districts* asset);
	};
}
