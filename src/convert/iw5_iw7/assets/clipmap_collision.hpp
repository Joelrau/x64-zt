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
			// The same triangle type lets world geometry move into the Havok builder
			// without keeping a second map-sized vector alive during construction.
			using havok_triangle = zonetool::iw7::havok::builder::triangle;

			// IW5 surface type (the top 12 bits of ClipMaterial::surfaceFlags) -> the IW7
			// material CRC to tag the surface with. See the note on the table in the
			// implementation for how the CRCs were identified.
			unsigned int iw7_material_crc(int surface_flags);

			// Flattens IW5 world collision (trisoup + brushes) into triangles in CoD
			// units, ready for the IW7 Havok mesh builder.
			// scale_override > 0 replaces the Havok world scale for this call and skips the
			// ZT_HAVOK_WORLD_SCALE env override - pass 1.0f to get the geometry in CoD units,
			// which is what the GfxWorld light-hull fitter wants. Leave it at 0 for the
			// normal path, which scales to Havok space.
			//
			// Brushes always come out here as triangulated faces, whatever
			// ZT_HAVOK_BRUSH_CONVEX says -- callers that want the triangles themselves (the
			// light-hull fitter) keep getting exactly what they got before convexes existed.
			std::vector<havok_triangle> extract(clipMap_t* clipmap, float scale_override = 0.0f);

			// ------------------------------------------------------- brush models
			//
			// Brush models are the other half of the collision port. They are deliberately
			// left out of the world mesh (they would freeze at their compile-time position),
			// and IW7 wants them as convex polytopes in MapEnts::havokEntsShapeData instead.
			// The same half-space-clipping that feeds `extract` produces the hulls, but here
			// the polygons are kept whole rather than triangulated: a polytope wants one
			// face per plane.

			// One face of a hull. `plane` is (normal, dist) with the normal pointing out,
			// and `indices` walks the face counter-clockwise about that normal.
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

			// ------------------------------------------------------- world collision
			//
			// Mirrors zonetool::iw7::havok::builder::convex: one world brush as a convex
			// custom primitive of the compressed mesh. IW7's player movement cast only
			// collides with those, never with the mesh's triangles, so a brush that reaches
			// the world blob as faces is shot but walked through.
			struct havok_convex
			{
				// Havok space (already scaled by the world scale), distinct points: the welded
				// vertex set of the brush's hull.
				std::vector<std::array<float, 3>> verts;
				unsigned short surface_tag;
				int contents;
				unsigned int material_crc;
				std::uint64_t user_data;
				// The hull faces over `verts`, kept only for the OBJ debug dump. The builder
				// does not use them: the runtime collides the point set.
				std::vector<convex_face> faces;
			};

			struct world_collision
			{
				// Trisoup first (the first trisoup_count entries), then the faces of every
				// brush emitted as triangles -- all of them when ZT_HAVOK_BRUSH_CONVEX=0,
				// otherwise only the brushes that fell back.
				std::vector<havok_triangle> triangles;
				std::vector<havok_convex> convexes;
				std::size_t trisoup_count = 0;
			};

			// The world blob's input. Same filters, contents, tags and scale as `extract`,
			// but with ZT_HAVOK_BRUSH_CONVEX on (the default) each surviving brush comes out
			// as one convex instead of its faces. A brush whose hull cannot be built, or that
			// the builder would reject, falls back to faces and is counted in the log.
			world_collision extract_world(clipMap_t* clipmap);

			// One IW5 brush model -- cmodels[index] -- and the hulls it is built from.
			// Coordinates are CoD units, translated so the hulls sit where the cmodel's own
			// bounds say they do (see the note in the implementation about which space IW5
			// stores brush-model brushes in).
			struct brush_model
			{
				unsigned int index = 0;
				int contents = 0;
				// The shape's ShapeTagData: the IW7 material CRC and surface-flag word taken
				// from the ClipMaterial its brushes carry. An ents shape has one tag for the
				// whole shape, so this is the material the most planes agreed on.
				unsigned int material_crc = 0x1AB7BC33u;
				std::uint64_t surface_flags = 0;
				std::vector<convex_hull> hulls;
			};

			// Every cmodel except the world (index 0) that yielded at least one usable hull.
			std::vector<brush_model> extract_brush_models(clipMap_t* clipmap);

			// One trigger model's volumes as convex hulls. A TriggerHull is an AABB
			// intersected with its slabs, and a TriggerSlab is a pair of parallel planes at
			// dot(p, dir) = midPoint +/- halfSize, so the whole thing is already a convex
			// polytope -- the same clipping that turns a brush into a hull applies directly.
			//
			// Trigger hulls are stored in entity space in both games, so unlike brush models
			// these need no rebasing. Coordinates are CoD units.
			std::vector<convex_hull> extract_trigger_hulls(const MapTriggers& triggers,
				unsigned int model);

			// ------------------------------------------------ model physics collision
			//
			// An XModel's own collision volumes, from PhysCollmap. This is what IW5 collides
			// static models against, and it is authored rather than derived: grass, foliage
			// and other decoration simply have no collmap, which is why they are walked
			// through. Using the render surfaces instead gives every leaf a collision hull.
			//
			// Coordinates are CoD units, in model space.
			std::vector<convex_hull> extract_phys_collmap(const PhysCollmap* collmap);

			// ------------------------------------------------------------ debug output
			//
			// Wavefront OBJ dumps of what the extractors actually produce, so the collision
			// can be opened next to the map in a mesh editor. Written only when
			// ZT_HAVOK_OBJ_DIR names a directory; everything is in the same space the Havok
			// builder is handed, so what you see is what gets serialised.

			bool obj_dump_enabled();

			// "<ZT_HAVOK_OBJ_DIR>\<flattened asset name><suffix>". Empty if unset.
			std::string obj_dump_path(const char* asset_name, const char* suffix);

			// The world mesh. Split into one object per source ("trisoup"/"brushes") and
			// contents mask, so one bad class of geometry can be isolated on its own.
			// `trisoup_count` is where the brush triangles start. Convexes, if any, follow as
			// "brush_convex_contents_0x........" objects in the same space.
			void write_triangles_obj(const std::string& path,
				const std::vector<havok_triangle>& triangles, std::size_t trisoup_count,
				const std::vector<havok_convex>& convexes = {});

			// A named set of hulls -- one brush model, or one trigger.
			struct hull_group
			{
				std::string name;
				std::vector<convex_hull> hulls;
			};

			// Convex hulls, one object per group. Faces come out as n-gons, matching the
			// polytope faces the builder serialises rather than a triangulation of them.
			//
			// `scale` should be the same factor the builder is going to apply (ents shapes
			// are stored at CoD units / 32), so this file and the world one land in the
			// same space and can be loaded together.
			void write_hulls_obj(const std::string& path,
				const std::vector<hull_group>& groups, float scale);

			// The finished Havok blob, so `tools/iw5cmconv/hkxtool.py` can decode what was
			// actually serialised rather than what went in. Same directory and gate.
			void write_blob(const std::string& path, const std::uint8_t* data,
				std::size_t size);
		}
	}
}
