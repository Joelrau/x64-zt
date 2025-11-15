#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "physcollmap.hpp"

#include "zonetool/h1/assets/physcollmap.hpp"

#pragma warning(push)
#pragma warning(disable: 4101)

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace physcollmap
		{
			struct dmTransform
			{
				__m128 q;
				__m128 p;
			};

			__m128 _xmm_1_0{ .m128_f32 = {1.0f, 1.0f, 1.0f, 1.0f} };
			__m128 _xmm_4_0{ .m128_f32 = {4.0f, 4.0f, 4.0f, 4.0f} };
			__m128 _xmm_0_0492126{ .m128_f32 = {0.0492126f, 0.0492126f, 0.0492126f, 0.0492126f} };
			__m128 _xmm_0_0166667f{ .m128_f32 = {0.0166667f, 0.0166667f, 0.0166667f, 0.0166667f} };
			__m128 _xmm_0_00833333f{ .m128_f32 = {0.00833333f, 0.00833333f, 0.00833333f, 0.00833333f} };
			__m128 _xmm_0_166667f{ .m128_f32 = {0.166667f, 0.166667f, 0.166667f, 0.166667f} };
			__m128 _xmm_0_0416667f{ .m128_f32 = {0.0416667f, 0.0416667f, 0.0416667f, 0.0416667f} };

			__m128 dm_vec4_maxFloat{ .m128_f32 = {3.4028235e38f, 3.4028235e38f, 3.4028235e38f, 3.4028235e38f} };
			__m128 dm_vec4_oneHalf{ .m128_f32 = {0.5f, 0.5f, 0.5f, 0.5f} };
			__m128 dm_vec4_zeroSafe{ .m128_f32 = {1.1754944e-35f, 1.1754944e-35f, 1.1754944e-35f, 1.1754944e-35f} };
			__m128 dm_vec4_two{ .m128_f32 = {2.0f, 2.0f, 2.0f, 2.0f} };
			__m128 dm_vec4_three{ .m128_f32 = {3.0f, 3.0f, 3.0f, 3.0f} };
			__m128 dm_vec4_epsilon{ .m128_f32 = {1.1920929e-7f, 1.1920929e-7f, 1.1920929e-7f, 1.1920929e-7f} };
			__m128 dm_vec4_linearSlop{ .m128_f32 = {0.0049999999f, 0.0049999999f, 0.0049999999f, 0.0049999999f} };

			__m128 dm_vec4_unitX{ .m128_f32 = {1.0f, 0.0f, 0.0f, 0.0f} };
			__m128 dm_vec4_unitY{ .m128_f32 = {0.0f, 1.0f, 0.0f, 0.0f} };
			__m128 dm_vec4_unitZ{ .m128_f32 = {0.0f, 0.0f, 1.0f, 0.0f} };
			__m128 dm_quat_identity{ .m128_f32 = {0.0f, 0.0f, 0.0f, 1.0f} };

			__m128 _xmm1[8] =
			{
				{.m128_u32 = {0, 0, 0, 0}},
				{.m128_u32 = {0xFFFFFFFF, 0, 0, 0}},
				{.m128_u32 = {0xFFFFFFFF, 0, 0xFFFFFFFF, 0}},
				{.m128_u32 = {0, 0, 0xFFFFFFFF, 0}},
				{.m128_u32 = {0, 0xFFFFFFFF, 0, 0}},
				{.m128_u32 = {0xFFFFFFFF, 0xFFFFFFFF, 0, 0}},
				{.m128_u32 = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}},
				{.m128_u32 = {0, 0xFFFFFFFF, 0xFFFFFFFF, 0}}
			};

			__m128 _xmm2[6] =
			{
				{.m128_f32 = {0.0f, 0.0f, -1.0f, 0.0f}},
				{.m128_f32 = {0.0f, -1.0f, 0.0f, 0.0f}},
				{.m128_f32 = {1.0f, 0.0f, 0.0f, 0.0f}},
				{.m128_f32 = {-1.0f, 0.0f, 0.0f, 0.0f}},
				{.m128_f32 = {0.0f, 0.0f, 1.0f, 0.0f}},
				{.m128_f32 = {0.0f, 1.0f, 0.0f, 0.0f}}
			};

			void GetExtents(const Bounds& bounds, vec3_t& minExtent, vec3_t& maxExtent)
			{
				// Calculate the minimum extent (min corner)
				minExtent[0] = bounds.midPoint[0] - bounds.halfSize[0];
				minExtent[1] = bounds.midPoint[1] - bounds.halfSize[1];
				minExtent[2] = bounds.midPoint[2] - bounds.halfSize[2];

				// Calculate the maximum extent (max corner)
				maxExtent[0] = bounds.midPoint[0] + bounds.halfSize[0];
				maxExtent[1] = bounds.midPoint[1] + bounds.halfSize[1];
				maxExtent[2] = bounds.midPoint[2] + bounds.halfSize[2];
			}

			Bounds GetBounds(PhysGeomInfo* geom)
			{
				auto bounds = geom->bounds;
				if (geom->type == PHYS_GEOM_NONE || geom->type == PHYS_GEOM_BRUSHMODEL || geom->type == PHYS_GEOM_BRUSH)
				{
					bounds = geom->brushWrapper->bounds;
				}
				return bounds;
			}

			void ComputeIntegralsBox(PhysCollmap* asset, PhysGeomInfo* geom, zonetool::h1::PhysGeomInfo* h1_geom)
			{
				auto* data = h1_geom->data;
				auto bounds = GetBounds(geom);

				data->m_volume = data->m_vertexCount * bounds.halfSize[0] * bounds.halfSize[1] * bounds.halfSize[2];

				data->m_area = (bounds.halfSize[2] * bounds.halfSize[1]) +
					(bounds.halfSize[1] * bounds.halfSize[0]) +
					(bounds.halfSize[2] * bounds.halfSize[0]);
				data->m_area *= data->m_vertexCount;

				/*if (asset->count == 1)
				{
					memcpy(&data->m_centroid, &asset->mass.centerOfMass, sizeof(float[3]));

					data->m_inertiaMoments.x = data->m_volume * asset->mass.momentsOfInertia[0];
					data->m_inertiaMoments.y = data->m_volume * asset->mass.momentsOfInertia[1];
					data->m_inertiaMoments.z = data->m_volume * asset->mass.momentsOfInertia[2];

					data->m_inertiaProducts.x = data->m_volume * asset->mass.productsOfInertia[0];
					data->m_inertiaProducts.y = data->m_volume * asset->mass.productsOfInertia[1];
					data->m_inertiaProducts.z = data->m_volume * asset->mass.productsOfInertia[2];
				}*/

				vec3_t extents_min = {};
				vec3_t extents_max = {};
				GetExtents(bounds, extents_min, extents_max);

				zonetool::h1::dmFloat3 centroid = { 0.0f, 0.0f, 0.0f };
				zonetool::h1::dmFloat3 momentsOfInertia = { 0.0f, 0.0f, 0.0f };
				zonetool::h1::dmFloat3 productsOfInertia = { 0.0f, 0.0f, 0.0f };

				float mass = 1.0f;

				// Dimensions of the box
				float width = extents_max[0] - extents_min[0];
				float height = extents_max[1] - extents_min[1];
				float depth = extents_max[2] - extents_min[2];

				// Calculate Centroid (assuming geometric center for uniform box)
				//centroid.x = (extents_max[0] - extents_min[0]) / 2.0f;
				//centroid.y = (extents_min[1] + extents_max[1]) / 2.0f;
				//centroid.z = (extents_min[2] + extents_max[2]) / 2.0f;

				//centroid.x = bounds.midPoint[0];
				//centroid.y = bounds.midPoint[1];
				//centroid.z = bounds.midPoint[2];

				centroid.x = 0;
				centroid.y = 0;
				centroid.z = 0;

				// Moments of inertia for each axis (precomputed formula for a rectangular box)
				momentsOfInertia.x = (mass / 12.0f) * (height * height + depth * depth); // Ixx
				momentsOfInertia.y = (mass / 12.0f) * (width * width + depth * depth);   // Iyy
				momentsOfInertia.z = (mass / 12.0f) * (width * width + height * height); // Izz

				// Products of inertia - assuming no offset (they are zero for a symmetric box)
				productsOfInertia.x = 0.0f; // Ixy
				productsOfInertia.y = 0.0f; // Ixz
				productsOfInertia.z = 0.0f; // Iyz

				// Assign the calculated centroid, moments, and products of inertia to the dmPolytopeData structure
				data->m_centroid.x = centroid.x;
				data->m_centroid.y = centroid.y;
				data->m_centroid.z = centroid.z;

				data->m_inertiaMoments.x = data->m_volume * momentsOfInertia.x;
				data->m_inertiaMoments.y = data->m_volume * momentsOfInertia.y;
				data->m_inertiaMoments.z = data->m_volume * momentsOfInertia.z;

				data->m_inertiaProducts.x = data->m_volume * productsOfInertia.x;
				data->m_inertiaProducts.y = data->m_volume * productsOfInertia.y;
				data->m_inertiaProducts.z = data->m_volume * productsOfInertia.z;
			}

			void GetBoxVerticesAndPlanes(
				dmTransform* xf,
				__m128* extents,
				zonetool::h1::dmFloat4 vertices[8],
				zonetool::h1::dmPlane planes[6])
			{
				const __m128 q = xf->q;
				const __m128 p = xf->p;

				const __m128 q_w = _mm_shuffle_ps(q, q, 255);
				const __m128 q_210 = _mm_shuffle_ps(q, q, 210);
				const __m128 q_201 = _mm_shuffle_ps(q, q, 201);

				__m128 e = _mm_max_ps(*extents, _mm_mul_ps(_xmm_4_0, _xmm_0_0492126));
				__m128 e_neg = _mm_sub_ps(_mm_setzero_ps(), e);

				__m128 tmp[15];
				tmp[0] = e_neg;

				for (int i = 0; i < 8; ++i)
				{
					const __m128 sign_mask = _xmm1[i];

					const __m128 local = _mm_or_ps(
						_mm_andnot_ps(sign_mask, e_neg),
						_mm_and_ps(sign_mask, e));

					const __m128 t = _mm_add_ps(
						_mm_sub_ps(
							_mm_mul_ps(_mm_shuffle_ps(local, local, 210), q_201),
							_mm_mul_ps(_mm_shuffle_ps(local, local, 201), q_210)),
						_mm_mul_ps(q_w, local));

					const __m128 r = _mm_sub_ps(
						_mm_mul_ps(_mm_shuffle_ps(t, t, 210), q_201),
						_mm_mul_ps(_mm_shuffle_ps(t, t, 201), q_210));

					tmp[7 + i] = _mm_add_ps(_mm_add_ps(_mm_add_ps(r, r), local), p);
				}

				const __m128 q_w_p = q_w;
				const __m128 q_210_p = q_210;
				const __m128 q_201_p = q_201;

				for (int i = 0; i < 6; ++i)
				{
					const __m128 n = _xmm2[i];

					const bool all_non_negative =
						(_mm_movemask_ps(_mm_cmplt_ps(n, _mm_setzero_ps())) & 7) == 0;

					const __m128* ext_src = all_non_negative ? &e : &tmp[0];

					const __m128 scaled = _mm_mul_ps(*ext_src, n);

					__m128 v = _mm_shuffle_ps(n, n, 39);
					v.m128_f32[0] =
						_mm_shuffle_ps(scaled, scaled, 0).m128_f32[0]
						+ _mm_shuffle_ps(scaled, scaled, 85).m128_f32[0]
						+ _mm_shuffle_ps(scaled, scaled, 170).m128_f32[0];

					const __m128 v28 = _mm_shuffle_ps(v, v, 39);

					const __m128 t = _mm_add_ps(
						_mm_sub_ps(
							_mm_mul_ps(_mm_shuffle_ps(v28, v28, 210), q_201_p),
							_mm_mul_ps(_mm_shuffle_ps(v28, v28, 201), q_210_p)),
						_mm_mul_ps(q_w_p, v28));

					const __m128 r = _mm_sub_ps(
						_mm_mul_ps(_mm_shuffle_ps(t, t, 210), q_201_p),
						_mm_mul_ps(_mm_shuffle_ps(t, t, 201), q_210_p));

					const __m128 v31 = _mm_add_ps(_mm_add_ps(r, r), v28);

					__m128 v32 = _mm_shuffle_ps(v31, v31, 39);
					const __m128 dot = _mm_mul_ps(v31, p);

					v32.m128_f32[0] =
						_mm_shuffle_ps(dot, dot, 0).m128_f32[0]
						+ _mm_shuffle_ps(dot, dot, 85).m128_f32[0]
						+ _mm_shuffle_ps(dot, dot, 170).m128_f32[0]
						+ _mm_shuffle_ps(v28, v28, 255).m128_f32[0];

					tmp[1 + i] = _mm_shuffle_ps(v32, v32, 39);
				}

				for (int i = 0; i < 8; ++i)
				{
					const __m128 v = tmp[7 + i];
					vertices[i].x = v.m128_f32[0];
					vertices[i].y = v.m128_f32[1];
					vertices[i].z = v.m128_f32[2];
					vertices[i].w = v.m128_f32[3];
				}

				for (int i = 0; i < 6; ++i)
				{
					const __m128 v = tmp[1 + i];
					planes[i].normal.x = v.m128_f32[0];
					planes[i].normal.y = v.m128_f32[1];
					planes[i].normal.z = v.m128_f32[2];
					planes[i].offset = v.m128_f32[3];
				}
			}

			void GenerateBox(PhysCollmap* asset, PhysGeomInfo* geom, zonetool::h1::PhysGeomInfo* h1_geom, utils::memory::allocator& allocator)
			{
				auto* data = h1_geom->data;

				auto bounds = GetBounds(geom);

				const auto vertexCount = 8;
				const auto subEdgeCount = 24;
				const auto facesCount = 6;

				zonetool::h1::dmSubEdge edge[facesCount][4] = {};
				edge[0][0] = { .twinOffset = 1, .tail = 4, .left = 0, .next = 20 };
				edge[0][1] = { .twinOffset = -1, .tail = 5, .left = 5, .next = 3 };
				edge[0][2] = { .twinOffset = 1, .tail = 7, .left = 3, .next = 5 };
				edge[0][3] = { .twinOffset = -1, .tail = 4, .left = 5, .next = 9 };
				edge[1][0] = { .twinOffset = 1, .tail = 0, .left = 0, .next = 0 };
				edge[1][1] = { .twinOffset = -1, .tail = 4, .left = 3, .next = 15 };
				edge[1][2] = { .twinOffset = 1, .tail = 1, .left = 0, .next = 4 };
				edge[1][3] = { .twinOffset = -1, .tail = 0, .left = 1, .next = 16 };
				edge[2][0] = { .twinOffset = 1, .tail = 6, .left = 4, .next = 13 };
				edge[2][1] = { .twinOffset = -1, .tail = 7, .left = 5, .next = 23 };
				edge[2][2] = { .twinOffset = 1, .tail = 6, .left = 2, .next = 17 };
				edge[2][3] = { .twinOffset = -1, .tail = 2, .left = 4, .next = 8 };
				edge[3][0] = { .twinOffset = 1, .tail = 3, .left = 3, .next = 2 };
				edge[3][1] = { .twinOffset = -1, .tail = 7, .left = 4, .next = 19 };
				edge[3][2] = { .twinOffset = 1, .tail = 3, .left = 1, .next = 7 };
				edge[3][3] = { .twinOffset = -1, .tail = 0, .left = 3, .next = 12 };
				edge[4][0] = { .twinOffset = 1, .tail = 1, .left = 1, .next = 18 };
				edge[4][1] = { .twinOffset = -1, .tail = 2, .left = 2, .next = 21 };
				edge[4][2] = { .twinOffset = 1, .tail = 2, .left = 1, .next = 14 };
				edge[4][3] = { .twinOffset = -1, .tail = 3, .left = 4, .next = 11 };
				edge[5][0] = { .twinOffset = 1, .tail = 5, .left = 0, .next = 6 };
				edge[5][1] = { .twinOffset = -1, .tail = 1, .left = 2, .next = 22 };
				edge[5][2] = { .twinOffset = 1, .tail = 5, .left = 2, .next = 10 };
				edge[5][3] = { .twinOffset = -1, .tail = 6, .left = 5, .next = 1 };

				zonetool::h1::dm_uint8 faceSubEdges[facesCount] = {};
				faceSubEdges[0] = 0;
				faceSubEdges[1] = 7;
				faceSubEdges[2] = 10;
				faceSubEdges[3] = 2;
				faceSubEdges[4] = 8;
				faceSubEdges[5] = 1;

				zonetool::h1::dmFloat4 vertices[vertexCount]{};
				zonetool::h1::dmPlane planes[facesCount]{};

				__m128 extents{ bounds.halfSize[0], bounds.halfSize[1], bounds.halfSize[2] };
				dmTransform xf{};
				xf.q = { .m128_f32{0.0f, 0.0f, 0.0f, 1.0f} };
				xf.p = { .m128_f32{0.0f, 0.0f, 0.0f, 0.0f} };

				GetBoxVerticesAndPlanes(&xf, &extents, vertices, planes);

				data->m_vertexCount = vertexCount;
				data->m_subEdgeCount = subEdgeCount;
				data->m_faceCount = facesCount;

				data->m_aVertices = allocator.allocate_array<zonetool::h1::dmFloat4>(data->m_vertexCount);
				data->m_aPlanes = allocator.allocate_array<zonetool::h1::dmPlane>(data->m_faceCount);
				data->m_aSubEdges = allocator.allocate_array<zonetool::h1::dmSubEdge>(data->m_subEdgeCount);
				data->m_aFaceSubEdges = allocator.allocate_array<zonetool::h1::dm_uint8>(data->m_faceCount);

				for (auto i = 0; i < data->m_vertexCount; i++)
				{
					data->m_aVertices[i].x = vertices[i].x;
					data->m_aVertices[i].y = vertices[i].y;
					data->m_aVertices[i].z = vertices[i].z;
					data->m_aVertices[i].w = vertices[i].w;
				}

				for (auto i = 0; i < data->m_faceCount; i++)
				{
					data->m_aPlanes[i].normal.x = planes[i].normal.x;
					data->m_aPlanes[i].normal.y = planes[i].normal.y;
					data->m_aPlanes[i].normal.z = planes[i].normal.z;
					data->m_aPlanes[i].offset = planes[i].offset;
				}

				memcpy(data->m_aSubEdges, edge, sizeof(zonetool::h1::dmSubEdge) * data->m_subEdgeCount);
				memcpy(data->m_aFaceSubEdges, faceSubEdges, sizeof(zonetool::h1::dm_uint8) * data->m_faceCount);

				ComputeIntegralsBox(asset, geom, h1_geom);

				data->contents = -1;
			}

			void GenerateCylinder(PhysCollmap* asset, PhysGeomInfo* geom, zonetool::h1::PhysGeomInfo* h1_geom, utils::memory::allocator& allocator)
			{
				auto* data = h1_geom->data;

				// generate...

				data->contents = -1;
			}

			void GenerateCapsule(PhysCollmap* asset, PhysGeomInfo* geom, zonetool::h1::PhysGeomInfo* h1_geom, utils::memory::allocator& allocator)
			{
				auto* data = h1_geom->data;

				// generate...

				data->contents = -1;
			}

			void GenerateBrush(PhysCollmap* asset, PhysGeomInfo* geom, zonetool::h1::PhysGeomInfo* h1_geom, utils::memory::allocator& allocator)
			{
				auto* data = h1_geom->data;

				// generate...

				data->contents = -1;
			}

			zonetool::h1::PhysCollmap* convert(PhysCollmap* asset, utils::memory::allocator& allocator)
			{
				ZONETOOL_INFO("Generating PhysCollmap %s", asset->name);
				auto* new_asset = allocator.allocate<zonetool::h1::PhysCollmap>();

				new_asset->name = asset->name;

				new_asset->count = asset->count;
				memcpy(&new_asset->mass, &asset->mass, sizeof(PhysMass));
				memcpy(&new_asset->bounds, &asset->bounds, sizeof(Bounds));

				float center[3] = { 0.0f, 0.0f, 0.0f };
				float inertia1[3] = { 0.0f, 0.0f, 0.0f };
				float inertia2[3] = { 0.0f, 0.0f, 0.0f };
				//Bounds bounds{};

				new_asset->geoms = allocator.allocate_array<zonetool::h1::PhysGeomInfo>(new_asset->count);
				for (unsigned int i = 0; i < new_asset->count; i++)
				{
					auto* geom = &asset->geoms[i];
					auto* h1_geom = &new_asset->geoms[i];

					h1_geom->data = allocator.allocate<zonetool::h1::dmPolytopeData>();
					auto* data = h1_geom->data;

					/*auto type = geom->type;
					switch (type)
					{
					case PHYS_GEOM_NONE:
					{
						GenerateBrush(asset, geom, h1_geom, allocator);
						ZONETOOL_INFO("PhysCollmap geom %d (none?) for %s generated!", i, asset->name);
						break; // sphere...
					}
					case PHYS_GEOM_BOX:
					{
						GenerateBox(asset, geom, h1_geom, allocator);
						ZONETOOL_INFO("PhysCollmap geom %d (box) for %s generated!", i, asset->name);
						break;
					}
					case PHYS_GEOM_CYLINDER:
					{
						//GenerateCylinder(asset, geom, h1_geom, allocator);
						ZONETOOL_INFO("PhysCollmap geom %d (cylinder) for %s generated!", i, asset->name);
						break;
					}
					case PHYS_GEOM_CAPSULE:
					{
						GenerateCapsule(asset, geom, h1_geom, allocator);
						ZONETOOL_INFO("PhysCollmap geom %d (capsule) for %s generated!", i, asset->name);
						break;
					}
					case PHYS_GEOM_BRUSHMODEL:
					case PHYS_GEOM_BRUSH:
					{
						GenerateBrush(asset, geom, h1_geom, allocator);
						ZONETOOL_INFO("PhysCollmap geom %d (brush) for %s generated!", i, asset->name);
						break;
					}
					default:
						__debugbreak();
						break;
					}*/
					GenerateBox(asset, geom, h1_geom, allocator);

					center[0] += data->m_centroid.x;
					center[1] += data->m_centroid.y;
					center[2] += data->m_centroid.z;

					inertia1[0] += data->m_inertiaMoments.x / data->m_volume;
					inertia1[1] += data->m_inertiaMoments.y / data->m_volume;
					inertia1[2] += data->m_inertiaMoments.z / data->m_volume;

					inertia2[0] += data->m_inertiaProducts.x / data->m_volume;
					inertia2[1] += data->m_inertiaProducts.y / data->m_volume;
					inertia2[2] += data->m_inertiaProducts.z / data->m_volume;

					//bounds = GetBounds(geom);
				}

				asset->mass.centerOfMass[0] = center[0] / asset->count;
				asset->mass.centerOfMass[1] = center[1] / asset->count;
				asset->mass.centerOfMass[2] = center[2] / asset->count;

				asset->mass.momentsOfInertia[0] = (inertia1[0] / asset->count);
				asset->mass.momentsOfInertia[1] = (inertia1[1] / asset->count);
				asset->mass.momentsOfInertia[2] = (inertia1[2] / asset->count);

				asset->mass.productsOfInertia[0] = (inertia2[0] / asset->count);
				asset->mass.productsOfInertia[1] = (inertia2[1] / asset->count);
				asset->mass.productsOfInertia[2] = (inertia2[2] / asset->count);

				// re-calc bounds too?
				//asset->bounds = bounds;

				return new_asset;
			}

			void dump(PhysCollmap* asset)
			{
				utils::memory::allocator allocator;
				auto* converted_asset = convert(asset, allocator);
				zonetool::h1::phys_collmap::dump(converted_asset);
			}
		}
	}
}
#pragma warning(pop)