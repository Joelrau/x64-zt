#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace zonetool::iw7
{
	namespace havok
	{
		// Builds an IW7 world-collision blob (clipMap_t::havokWorldShapeData) from raw
		// triangles and convex volumes. See docs/iw7-havok-collision.md for where every constant below came
		// from; everything here is derived from IW7's own reflection tables and verified
		// against the shipped mp_paris / mp_afghan / mp_breakneck blobs.
		//
		// Geometry goes in in Havok space: IW7 stores world collision at CoD units / 32,
		// just like per-model PhysicsAssets. Winding must be CCW as
		// seen from the front face or surface normals invert.
		namespace builder
		{
			// One ShapeTagData record, exactly as it is written into a shape list's
			// shapeTagData array (24 bytes: collisionFilterInfo, materialCRC, materialId,
			// pad, userData). materialId is not stored here because it is 0xFFFF in every
			// entry the builder emits -- the loader resolves the material from the CRC.
			//
			// This is a shared type because IW7 has exactly ONE shape-tag decoder for the
			// whole map. HavokPhysics_SetMainShapeList points it at the main list's
			// m_shapeTagData, and every composite shape in the map -- whichever list it came
			// from -- decodes its primitives' raw tags against that single table, because
			// m_shapeTagCodecInfo is -1 in every blob, stock and ours. So the world blob's
			// table and the ents blob's table are not independent: shipped maps emit the
			// world table as the exact leading run of the ents table (mp_fallen 216 of 219,
			// mp_afghan 132 of 136, mp_frontend 7 of 7, byte-identical and in order), and
			// anything else hands the world mesh somebody else's filters and userData.
			struct shape_tag
			{
				std::uint32_t collision_filter = 0; // already masked, ready to write
				std::uint32_t material_crc = 0;
				std::uint64_t user_data = 0;

				bool operator==(const shape_tag& other) const
				{
					return collision_filter == other.collision_filter
						&& material_crc == other.material_crc
						&& user_data == other.user_data;
				}
			};

			struct triangle
			{
				float verts[3][3];
				// A quad is stored as the two triangles (v0,v1,v2) and (v0,v2,v3) sharing the
				// 0-2 diagonal, which is how Havok fans a 4-index primitive. Stock world meshes
				// are ~96% quads (afghan: 90,924 of 105,752 primitives), so emitting a face as
				// one quad rather than two triangles is what the shipped data looks like.
				bool is_quad = false;
				float vert3[3] = {};
				unsigned short surface_tag; // -> primitiveDataRuns, per-surface material id
				int contents = 1;           // CoD contents mask -> collisionFilterInfo
				// ShapeTagData::materialCRC. Drives footsteps, impact effects and
				// penetration; 0x1AB7BC33 is the value the shipped dummies use.
				std::uint32_t material_crc = 0x1AB7BC33u;
				// ShapeTagData::userData: IW7 surface flags in the low 32 bits and the brush
				// basis at bit 48 (see TAG_USERDATA_BRUSH_BASIS in the implementation).
				std::uint64_t user_data = 0;
			};

			// One convex volume -- in practice one brush -- stored inside the compressed mesh
			// as a CONVEX CUSTOM PRIMITIVE. This is not optional decoration: IW7's player
			// movement sweep (a capsule shape cast) only ever dispatches convex custom
			// primitives from the world mesh, never plain triangles or quads. Stock world
			// blobs keep every brush in this form beside their triangle/quad terrain, and a
			// mesh made only of triangles is hit by rays and bullets but walked through.
			//
			// Encoding (hk_2014.2.5-r1, decoded from stock mp_frontend / mp_afghan, 7,798 of
			// 7,798 customs):
			//   primitive indices    [r, r, r, r], r = section-local slot of the record
			//   sharedVerticesIndex  [r]   = (numVertices << 8) | 0x2   (type 2, layer 0)
			//                        [r+1] = page-relative start of the vertex run
			//   sharedVertices       [(page << 16) + start .. + numVertices) -- one contiguous
			//                        run inside the section's page, not listed in the index
			// The record's two words occupy local shared-index slots like listed vertices do.
			struct convex
			{
				std::vector<std::array<float, 3>> verts; // Havok space, already scaled, distinct points
				unsigned short surface_tag;
				int contents = 1;
				std::uint32_t material_crc = 0x1AB7BC33u;
				std::uint64_t user_data = 0;
			};

			// Why build_mesh_blob would refuse a convex: nullptr if it is acceptable, otherwise
			// a short reason. Fewer than 4 vertices, more than 255 (numVertices is the high byte
			// of a uint16), exactly repeated points, or every vertex within 1e-4 Havok units of
			// one plane. Exposed so a caller can choose its fallback BEFORE handing the convex
			// over -- the builder itself drops a rejected convex (and logs the count), it has
			// no geometry to fall back to.
			const char* convex_rejection(const std::vector<std::array<float, 3>>& verts);

			struct mesh_input
			{
				std::vector<triangle> triangles;
				// Emitted as convex custom primitives after the triangles, in the same
				// sections. HavokPhysicsShapeList::convexCounts is the number actually
				// emitted, counted by the builder. Empty for per-model physics assets and
				// XModel LOD blobs, which then serialise exactly as before.
				std::vector<convex> convexes;
				float convex_radius = 0.0f; // shipped stock world blobs use 0.0
			};

			// Produces a complete Havok 2014.2.5-r1 binary packfile. The returned buffer is
			// what havokWorldShapeData points at, and its size is havokWorldShapeDataSize.
			// Returns an empty vector (and logs) if the input is unusable.
			//
			// `out_tags`, when given, receives the blob's shapeTagData table in emitted
			// order -- the very records the write loop consumed, not a re-derivation. Hand
			// it to build_ents_shape_list as ents_input::world_tags; see the note on
			// shape_tag for why the two lists cannot pick their own orderings.
			// Left untouched when the build fails.
			std::vector<std::uint8_t> build_world_shape(const mesh_input& input,
				std::vector<shape_tag>* out_tags = nullptr);

			// MapEnts::havokEntsShapeData -- the second shape list, holding one hknpShape per
			// brush model and per trigger, indexed by cmodel_t::physicsShapeOverrideIdx and
			// TriggerModel::physicsShapeOverrideIdx. See docs/iw7-ents-shapes.md.
			//
			// Passing no shapes builds the empty form, which is not merely a placeholder:
			// CM_ContentsOfBrushModel dereferences this list unconditionally for any
			// "model" "*N" entity, so a null blob is a null read, while an empty list makes
			// it take the `idx >= shapeContents.size` branch and return a permissive default.
			// Shipped converted maps (mp_bog, mp_frontend) carry exactly that.

			// One face of a convex polytope: its outward plane, and a counter-clockwise run
			// of vertex indices around it.
			struct polytope_face
			{
				float plane[4]; // normal xyz, dist -- Havok stores dot(n,p) + d = 0
				std::vector<std::uint8_t> indices;
			};

			// hknpConvexPolytopeShape. Vertex indices are uint8, so at most 255 vertices.
			struct polytope
			{
				std::vector<std::array<float, 3>> verts;
				std::vector<polytope_face> faces; // exactly one per plane
			};

			// One entry of the shape list: an hknpDynamicCompoundShape wrapping one polytope
			// per brush. `contents` lands in shapeContents, which is what
			// CM_ContentsOfBrushModel hands back as the entity's contents mask.
			struct ents_shape
			{
				std::vector<polytope> convexes;
				// The per-surface mask its shapeTagData entry carries, and what the runtime
				// collides the shape as.
				int contents = 1;
				// What CM_ContentsOfBrushModel hands back as the entity's contents mask.
				// Stock separates the two: a trigger stores 0xC7FFBFFF here while a brush
				// model stores its real mask (breakneck: 63 triggers at 0xC7FFBFFF, 20 shapes
				// at 0x30200, 16 at 0x30000).
				unsigned int entity_contents = 1;
				// ShapeTagData::materialCRC, same meaning and same table as triangle::material_crc
				// on the world path. Stock ents blobs carry the full spread of world materials
				// here (mp_afghan 12 distinct over 136 tags), so leaving it at the default
				// makes every brush model and trigger sound and shatter like concrete.
				std::uint32_t material_crc = 0x1AB7BC33u;
				// ShapeTagData::userData -- surface flags plus the brush basis at bit 48.
				std::uint64_t user_data = 0;
				std::string name; // shapeNames; "<map>:entity N" in stock data
			};

			struct ents_input
			{
				std::vector<ents_shape> shapes;

				// The world blob's shapeTagData, straight out of build_world_shape. It is a
				// REQUIRED PREFIX, not a hint: the emitted table begins with these records
				// byte for byte and in this order, and only tags the world table does not
				// already carry are appended after them. Leaving it empty builds the table
				// from the ents shapes alone -- which is what per-model callers want, and
				// what a map wants only if it has no world blob at all.
				std::vector<shape_tag> world_tags;

				// Shapes are stored at CoD units / 32 -- the opposite of the world blob,
				// which is 1:1. Verified at exactly 32.0000 over 204 stock samples; see
				// docs/iw7-ents-shapes.md. Geometry goes in as CoD units and is scaled here.
				float scale = 1.0f / 32.0f;
			};

			// How the ents table came out, for the caller to log. `reused` counts the
			// distinct prefix records the ents shapes landed on, so reused + appended is the
			// number of distinct tags the shapes actually need.
			struct ents_tag_merge
			{
				std::size_t prefix = 0;   // records carried in from the world table
				std::size_t total = 0;    // records in the emitted table
				std::size_t reused = 0;   // prefix records an ents shape points at
				std::size_t appended = 0; // records appended after the prefix
			};

			std::vector<std::uint8_t> build_ents_shape_list(const ents_input& input,
				ents_tag_merge* out_merge = nullptr);

			// The .hkx beside a `physicsasset` dump. Only the dummy form is generated: a
			// static body wrapping a placeholder box, which is what IW7 attaches to script
			// brush models and triggers. `physicsShapeOverrideIdx` replaces the shape, so the
			// box is never collided against -- but the asset has to exist or the runtime
			// builds no body at all. Shipped dummies are byte-identical across maps and the
			// four variants differ only in these two fields.
			struct physics_asset_input
			{
				std::string body_name = "scriptbrushmodeldummy";
				std::uint32_t body_quality_crc = 0x7923E35Cu;
				std::uint32_t material_crc = 0x1AB7BC33u;
				// hknpBodyCinfo::collisionFilterInfo, the CoD contents mask the body collides
				// as. 1 (solid) is what the shipped brush-model and trigger dummies carry.
				std::uint32_t body_contents = 1;
			};

			std::vector<std::uint8_t> build_physics_asset(const physics_asset_input& input);

			// A model's own collision: the same compressed mesh the world blob uses, wrapped in
			// a HavokPhysicsAsset with a single static body. This is what IW7 attaches to an
			// XModel via XModel::physAsset, and without it a static model has no collision at
			// all -- 391 of mp_paris's 671 xmodels ship one. Geometry is CoD units / 32.
			std::vector<std::uint8_t> build_model_physics_asset(const mesh_input& input,
				const physics_asset_input& physics_asset);

			// A model that is meant to move: one dynamic body over the model's PhysCollmap
			// hulls (a single hknpConvexPolytopeShape, or an hknpDynamicCompoundShape of
			// them), every shape carrying hknpShapeMassProperties, plus the hknpMotionCinfo
			// a dynamic body needs. This is what IW7 attaches to clutter and other
			// physics-simulated props (stock com_junktire, tool_watercan_iw6); a static
			// compressed-mesh body cannot be simulated. Geometry is CoD units, model space,
			// scaled by `scale`. See docs/iw7-ents-shapes.md and docs/iw7-havok-collision.md.
			struct dynamic_physics_asset_input
			{
				std::vector<polytope> convexes;
				std::string body_name = "tag_origin"; // what every stock dynamic body is called
				// The body's mass. Stock values are author-set round numbers (5, 2, 25, 100),
				// the same convention as IW5's PhysPreset::mass, which is where it comes from.
				float mass = 5.0f;
				// hknpBodyCinfo::collisionFilterInfo. Stock clutter (com_junktire) uses the
				// same shot-only 0x3180 as static props: the player pushes clutter, it does
				// not block him.
				std::uint32_t body_contents = 0x3180u;
				std::uint32_t material_crc = 0x1AB7BC33u;
				std::uint32_t body_quality_crc = 0x7923E35Cu; // "default", as on the dummies
				// motionPropertiesNameCRCLookup[0]; 0x9F53AC92 on every dynamic stock asset
				std::uint32_t motion_properties_crc = 0x9F53AC92u;
				float scale = 1.0f / 32.0f;
			};

			std::vector<std::uint8_t> build_dynamic_physics_asset(const dynamic_physics_asset_input& input);

			// XModel::physicsLODData: a HavokPhysicsXModelLOD packfile, the per-model
			// "detail" collision IW7's static-model code instantiates into the detail world
			// (StaticModels_CreateClipmapShapes, 0x140574CF0: physicsAsset instances go to
			// the simulation list, and when physicsLODDataSize is non-zero the LOD's shapes
			// go to the detail list instead of a copy of them). Bullets, sight and the
			// crosshair query the detail world, so this is the blob whose tags decide the
			// surface type a shot prop reports.
			//
			// Unlike a PhysicsAsset mesh (tags 0xFFFF, decoded to userData 0), a LOD mesh
			// carries real tags into its own 8-byte table {surfaceFlags, collisionFilterInfo}
			// -- XModelCollisionTagData, measured over 864 stock LOD blobs. At load
			// (0x1405731A0) the table is CRC'd into g_havokPhysicsXModelLODShapeTagDatas and
			// its id is written to every composite shape's shapeTagCodecInfo, so a hit on
			// the LOD decodes to userData = surfaceFlags, materialId 0. The table here is
			// the mesh's tag palette: one record per distinct (contents, userData) with
			// surfaceFlags = user_data's low 32 bits.
			//
			// `bone_name` is the hknpShape's bodyNames entry: the bone the shape hangs from
			// ("tag_origin" on 829 of 864 stock single-shape LODs). The loader lower-cases
			// it, resolves it through SL_FindString + XModelGetBoneIndex, and reads the
			// instance transform from baseMat[bone] -- an unknown name yields bone 255 and
			// a garbage transform, so it must be a real bone of the model. It is NOT the
			// XModel::physicsLODDataNames entry, which is a separate LOD name.
			std::vector<std::uint8_t> build_model_physics_lod(const mesh_input& input,
				const std::string& bone_name);
		}
	}
}
