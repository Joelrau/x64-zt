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
		// Builds IW7 Havok packfiles: the world collision blob, the map-ents shape list and per-model physics.
		// World geometry goes in at CoD units / 32, wound counter-clockwise seen from the front face.
		namespace builder
		{
			// One ShapeTagData record (collisionFilterInfo, materialCRC, userData; materialId is always 0xFFFF).
			// Shared because IW7 decodes every shape's tags against the main list's table, so the ents table must
			// start with the world table.
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
				// A quad is the triangles (v0,v1,v2) and (v0,v2,v3), the way Havok fans a 4-index primitive.
				bool is_quad = false;
				float vert3[3] = {};
				unsigned short surface_tag; // -> primitiveDataRuns, per-surface material id
				int contents = 1;           // CoD contents mask -> collisionFilterInfo
				// ShapeTagData::materialCRC: footsteps, impact effects and penetration.
				std::uint32_t material_crc = 0x1AB7BC33u;
				// ShapeTagData::userData: IW7 surface flags in the low 32 bits and the brush
				// basis at bit 48 (see TAG_USERDATA_BRUSH_BASIS in the implementation).
				std::uint64_t user_data = 0;
			};

			// One convex volume, in practice one brush, stored as a convex custom primitive. IW7's player movement
			// sweep only dispatches these from the world mesh: a mesh of triangles alone stops bullets but not
			// players.
			struct convex
			{
				std::vector<std::array<float, 3>> verts; // Havok space, already scaled, distinct points
				unsigned short surface_tag;
				int contents = 1;
				std::uint32_t material_crc = 0x1AB7BC33u;
				std::uint64_t user_data = 0;
			};

			// nullptr when build_mesh_blob accepts the convex, otherwise the reason: fewer than 4 vertices, more than
			// 255, repeated points, or all vertices on one plane. The builder drops a rejected convex.
			const char* convex_rejection(const std::vector<std::array<float, 3>>& verts);

			struct mesh_input
			{
				std::vector<triangle> triangles;
				// Emitted as convex custom primitives after the triangles, in the same sections.
				std::vector<convex> convexes;
				float convex_radius = 0.0f; // shipped stock world blobs use 0.0
			};

			// Returns a complete packfile for havokWorldShapeData, or an empty vector when the input is unusable.
			// out_tags receives the shapeTagData table in emitted order, for ents_input::world_tags.
			std::vector<std::uint8_t> build_world_shape(const mesh_input& input,
				std::vector<shape_tag>* out_tags = nullptr);

			// MapEnts::havokEntsShapeData: one hknpShape per brush model and trigger, indexed by
			// physicsShapeOverrideIdx. No shapes builds the empty list, which the game still needs:
			// CM_ContentsOfBrushModel reads the list for every "*N" model.

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

			// One entry of the shape list: an hknpDynamicCompoundShape wrapping one polytope per brush.
			struct ents_shape
			{
				std::vector<polytope> convexes;
				// The per-surface mask its shapeTagData entry carries, and what the runtime
				// collides the shape as.
				int contents = 1;
				// What CM_ContentsOfBrushModel hands back as the entity's contents mask. Stock triggers store 0xC7FFBFFF,
				// brush models their real mask.
				unsigned int entity_contents = 1;
				// ShapeTagData::materialCRC, as triangle::material_crc on the world path.
				std::uint32_t material_crc = 0x1AB7BC33u;
				// ShapeTagData::userData -- surface flags plus the brush basis at bit 48.
				std::uint64_t user_data = 0;
				std::string name; // shapeNames; "<map>:entity N" in stock data
			};

			struct ents_input
			{
				std::vector<ents_shape> shapes;

				// The world blob's shapeTagData from build_world_shape. The emitted table begins with these records in
				// this order; other tags are appended. Empty for per-model callers.
				std::vector<shape_tag> world_tags;

				// Shapes are stored at CoD units / 32. Geometry goes in as CoD units and is scaled here.
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

			// The dummy asset IW7 attaches to script brush models and triggers: a static body around a placeholder
			// box that physicsShapeOverrideIdx replaces. Without the asset the runtime builds no body.
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

			// A model's own collision: the compressed mesh the world blob uses, in a HavokPhysicsAsset with one static
			// body (XModel::physAsset). Geometry is CoD units / 32.
			std::vector<std::uint8_t> build_model_physics_asset(const mesh_input& input,
				const physics_asset_input& physics_asset);

			// A model that moves: one dynamic body over the model's PhysCollmap hulls, with mass properties and the
			// hknpMotionCinfo a dynamic body needs. Geometry is CoD units in model space, scaled by `scale`.
			struct dynamic_physics_asset_input
			{
				std::vector<polytope> convexes;
				std::string body_name = "tag_origin"; // what every stock dynamic body is called
				// The body's mass, from PhysPreset::mass.
				float mass = 5.0f;
				// hknpBodyCinfo::collisionFilterInfo. Stock clutter is shot-only (0x3180): the player pushes it.
				std::uint32_t body_contents = 0x3180u;
				std::uint32_t material_crc = 0x1AB7BC33u;
				std::uint32_t body_quality_crc = 0x7923E35Cu; // "default", as on the dummies
				// motionPropertiesNameCRCLookup[0]; 0x9F53AC92 on every dynamic stock asset
				std::uint32_t motion_properties_crc = 0x9F53AC92u;
				float scale = 1.0f / 32.0f;
			};

			std::vector<std::uint8_t> build_dynamic_physics_asset(const dynamic_physics_asset_input& input);

			// XModel::physicsLODData: the detail collision that bullets, sight and the crosshair query, so its tags
			// decide the surface type a shot prop reports. The mesh carries real tags into its own table of
			// {surfaceFlags, collisionFilterInfo}. bone_name must be a real bone of the model: the loader reads the
			// instance transform from it.
			std::vector<std::uint8_t> build_model_physics_lod(const mesh_input& input,
				const std::string& bone_name);
		}
	}
}
