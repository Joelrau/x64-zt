#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "../include.hpp"
#include <iw7/common/havok_builder.hpp>

namespace zonetool::iw5
{
	namespace converter::iw7
	{
		namespace collision
		{
			using havok_triangle = zonetool::iw7::havok::builder::triangle;

			unsigned int iw7_material_crc(int surface_flags);

			std::vector<havok_triangle> extract(clipMap_t* clipmap, float scale_override = 0.0f);

			struct convex_face
			{
				float plane[4];
				std::vector<unsigned char> indices;
			};

			struct convex_hull
			{
				std::vector<std::array<float, 3>> verts;
				std::vector<convex_face> faces;
			};

			struct havok_convex
			{
				std::vector<std::array<float, 3>> verts;
				unsigned short surface_tag;
				int contents;
				unsigned int material_crc;
				std::uint64_t user_data;
				std::vector<convex_face> faces;
			};

			struct world_collision
			{
				std::vector<havok_triangle> triangles;
				std::vector<havok_convex> convexes;
				std::size_t trisoup_count = 0;
			};

			world_collision extract_world(clipMap_t* clipmap);

			struct brush_model
			{
				unsigned int index = 0;
				int contents = 0;
				unsigned int material_crc = 0x1AB7BC33u;
				std::uint64_t surface_flags = 0;
				std::vector<convex_hull> hulls;
			};

			std::vector<brush_model> extract_brush_models(clipMap_t* clipmap);

			std::vector<convex_hull> extract_trigger_hulls(const MapTriggers& triggers,
				unsigned int model);

			std::vector<convex_hull> extract_phys_collmap(const PhysCollmap* collmap);

			bool obj_dump_enabled();

			std::string obj_dump_path(const char* asset_name, const char* suffix);

			void write_triangles_obj(const std::string& path,
				const std::vector<havok_triangle>& triangles, std::size_t trisoup_count,
				const std::vector<havok_convex>& convexes = {});

			struct hull_group
			{
				std::string name;
				std::vector<convex_hull> hulls;
			};

			void write_hulls_obj(const std::string& path,
				const std::vector<hull_group>& groups, float scale);

			void write_blob(const std::string& path, const std::uint8_t* data,
				std::size_t size);
		}
	}
}
