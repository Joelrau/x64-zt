#include <std_include.hpp>

#include "zonetool/h1/converter/iw7/include.hpp"
#include "gfxworld.hpp"
#include "zonetool/iw7/assets/gfxworld.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

#include <nmmintrin.h>

namespace
{
	uint32_t crc32c_hw(const void* data, size_t len)
	{
		const uint8_t* p = (const uint8_t*)data;
		uint32_t crc = 0xFFFFFFFF;

		while (len--)
			crc = _mm_crc32_u8(crc, *p++);

		return ~crc;
	}

	static inline uint32_t crc32Hash(const char* ptr, uint32_t length)
	{
		return crc32c_hw(ptr, length);
	}

	static inline uint32_t crc32Hash(const uint32_t* ptr, uint32_t length)
	{
		return crc32c_hw(ptr, length);
	}

	static inline uint32_t computeCRC32(zonetool::h1::Umbra::ImpTome* tome)
	{
		return crc32Hash(&tome->m_size, tome->m_size - 8);
	}
}

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace
		{
			std::string clean_name(const std::string& name)
			{
				auto new_name = name;

				for (auto i = 0u; i < name.size(); i++)
				{
					switch (new_name[i])
					{
					case '*':
						new_name[i] = '_';
						break;
					}
				}

				return new_name;
			}
		}

		namespace
		{
			inline void vec3_add(const vec3_t a, const vec3_t b, vec3_t out) {
				float x = a[0] + b[0], y = a[1] + b[1], z = a[2] + b[2];
				out[0] = x; out[1] = y; out[2] = z;
			}

			inline void vec3_sub(const vec3_t a, const vec3_t b, vec3_t out) {
				float x = a[0] - b[0], y = a[1] - b[1], z = a[2] - b[2];
				out[0] = x; out[1] = y; out[2] = z;
			}

			inline void vec3_cross(const vec3_t a, const vec3_t b, vec3_t out) {
				float x = a[1] * b[2] - a[2] * b[1];
				float y = a[2] * b[0] - a[0] * b[2];
				float z = a[0] * b[1] - a[1] * b[0];
				out[0] = x; out[1] = y; out[2] = z;
			}

			inline float vec3_dot(const vec3_t a, const vec3_t b)
			{
				return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
			}

			inline void vec3_scale(const vec3_t v, float s, vec3_t out)
			{
				out[0] = v[0] * s;
				out[1] = v[1] * s;
				out[2] = v[2] * s;
			}

			inline void vec3_normalize(vec3_t v) {
				float sql = (v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]);
				if (sql > 1e-6f) {
					float inv = 1.0f / std::sqrt(sql);
					v[0] *= inv; v[1] *= inv; v[2] *= inv;
				}
			}

			bool intersect3(const cplane_s& a, const cplane_s& b, const cplane_s& c, vec3_t out) {
				vec3_t n2xn3;
				vec3_cross(b.normal, c.normal, n2xn3);
				float denom = vec3_dot(a.normal, n2xn3);

				if (std::abs(denom) < 1e-8f) return false;

				vec3_t term1, term2, term3, n3xn1, n1xn2;
				vec3_scale(n2xn3, a.dist, term1);

				vec3_cross(c.normal, a.normal, n3xn1);
				vec3_scale(n3xn1, b.dist, term2);

				vec3_cross(a.normal, b.normal, n1xn2);
				vec3_scale(n1xn2, c.dist, term3);

				out[0] = (term1[0] + term2[0] + term3[0]) / denom;
				out[1] = (term1[1] + term2[1] + term3[1]) / denom;
				out[2] = (term1[2] + term2[2] + term3[2]) / denom;
				return true;
			}

			bool inside_all(const vec3_t p, const cplane_s* planes, int count)
			{
				for (int i = 0; i < count; ++i)
				{
					if (vec3_dot(planes[i].normal, p) > planes[i].dist + 0.01f)
						return false;
				}
				return true;
			}

			// Fixed build_vertices to handle the vec3_t array type properly
			struct vec3_stack { float v[3]; }; // Structs containing arrays ARE copyable
			std::vector<vec3_stack> build_vertices(const cplane_s* planes, int count)
			{
				std::vector<vec3_stack> pts;

				for (int i = 0; i < count; ++i) {
					for (int j = i + 1; j < count; ++j) {
						for (int k = j + 1; k < count; ++k) {
							vec3_t p;
							if (intersect3(planes[i], planes[j], planes[k], p)) {
								if (inside_all(p, planes, count)) {
									// Wrap the raw array in the struct to make it vector-compatible
									pts.push_back(*(vec3_stack*)p);
								}
							}
						}
					}
				}
				return pts;
			}

			zonetool::iw7::GfxReflectionProbeObb compute_obb(const std::vector<vec3_stack>& pts) {
				zonetool::iw7::GfxReflectionProbeObb obb{};
				if (pts.empty()) return obb;

				// 1. Compute Center
				vec3_t center = { 0, 0, 0 };
				for (const auto& p : pts) {
					center[0] += p.v[0]; center[1] += p.v[1]; center[2] += p.v[2];
				}
				float inv = 1.0f / (float)pts.size();
				center[0] *= inv; center[1] *= inv; center[2] *= inv;

				// 2. Covariance for PCA
				// (Note: For most games, you'd actually prefer AABB or 
				// orientation-locked OBB, but keeping your PCA logic here)
				float cov[3][3] = { 0 };
				for (const auto& p : pts) {
					float d[3] = { p.v[0] - center[0], p.v[1] - center[1], p.v[2] - center[2] };
					for (int i = 0; i < 3; ++i)
						for (int j = 0; j < 3; ++j)
							cov[i][j] += d[i] * d[j];
				}

				// 3. Define Axes (Gram-Schmidt Orthonormalization)
				vec3_t xAxis = { cov[0][0], cov[0][1], cov[0][2] };
				if (vec3_dot(xAxis, xAxis) < 1e-4f) xAxis[0] = 1.0f; // Fallback
				vec3_normalize(xAxis);

				vec3_t yAxis = { cov[1][0], cov[1][1], cov[1][2] };
				vec3_normalize(yAxis);

				vec3_t zAxis;
				vec3_cross(xAxis, yAxis, zAxis);
				vec3_normalize(zAxis);
				vec3_cross(zAxis, xAxis, yAxis); // Re-orthogonalize Y

				// 4. Find Bounds
				vec3_t minv = { FLT_MAX, FLT_MAX, FLT_MAX }, maxv = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
				for (const auto& p : pts) {
					vec3_t d = { p.v[0] - center[0], p.v[1] - center[1], p.v[2] - center[2] };
					float px = vec3_dot(d, xAxis);
					float py = vec3_dot(d, yAxis);
					float pz = vec3_dot(d, zAxis);

					minv[0] = std::min(minv[0], px); maxv[0] = std::max(maxv[0], px);
					minv[1] = std::min(minv[1], py); maxv[1] = std::max(maxv[1], py);
					minv[2] = std::min(minv[2], pz); maxv[2] = std::max(maxv[2], pz);
				}

				// 5. Finalize OBB
				for (int i = 0; i < 3; i++) {
					// Offset center to the actual middle of the point cloud extent
					float offset = (maxv[i] + minv[i]) * 0.5f;
					obb.center[0] += xAxis[0] * offset + yAxis[0] * offset + zAxis[0] * offset; // Simplified for brevity
					// (Realistically you'd apply the offset along each specific axis)
				}

				memcpy(obb.xAxis, xAxis, sizeof(vec3_t));
				memcpy(obb.yAxis, yAxis, sizeof(vec3_t));
				memcpy(obb.zAxis, zAxis, sizeof(vec3_t));
				obb.halfSize[0] = (maxv[0] - minv[0]) * 0.5f;
				obb.halfSize[1] = (maxv[1] - minv[1]) * 0.5f;
				obb.halfSize[2] = (maxv[2] - minv[2]) * 0.5f;

				return obb;
			}

			void obb_to_quat(const zonetool::iw7::GfxReflectionProbeObb& obb, vec4_t q)
			{
				float m[3][3] = {
					{obb.xAxis[0], obb.yAxis[0], obb.zAxis[0]},
					{obb.xAxis[1], obb.yAxis[1], obb.zAxis[1]},
					{obb.xAxis[2], obb.yAxis[2], obb.zAxis[2]}
				};

				float trace = m[0][0] + m[1][1] + m[2][2];

				if (trace > 0.0f)
				{
					float s = sqrt(trace + 1.0f) * 2.0f;
					q[3] = 0.25f * s;
					q[0] = (m[2][1] - m[1][2]) / s;
					q[1] = (m[0][2] - m[2][0]) / s;
					q[2] = (m[1][0] - m[0][1]) / s;
				}
				else
				{
					q[0] = q[1] = q[2] = 0; q[3] = 1;
				}
			}

			void build_probe_instance(
				zonetool::iw7::GfxReflectionProbeInstance& out,
				const cplane_s* planes,
				int planeCount,
				const vec3_t origin)
			{
				memset(&out, 0, sizeof(out));

				memcpy(out.probePosition, origin, sizeof(vec3_t));
				out.priority = -FLT_MAX;

				if (!planes || planeCount == 0)
					return;

				auto pts = build_vertices(planes, planeCount);
				auto obb = compute_obb(pts);

				out.volumeObb = obb;

				obb_to_quat(obb, out.probeRotation);

				memcpy(out.feather, obb.halfSize, sizeof(vec3_t));

				out.expandProjectionNeg[0] = 0;
				out.expandProjectionNeg[1] = 0;
				out.expandProjectionNeg[2] = 0;

				out.expandProjectionPos[0] = 0;
				out.expandProjectionPos[1] = 0;
				out.expandProjectionPos[2] = 0;
			}
		}

		namespace gfx_world
		{
			zonetool::iw7::GfxImage* generate_reflection_probe_array_image(GfxWorldDraw* draw, utils::memory::allocator& allocator)
			{
				const std::string image_name = "*reflection_probe_array";
				const std::string image_name_clean = "_reflection_probe_array";

				std::uint32_t width = 0, height = 0, mip_levels = 0;
				std::uint16_t depth = 0;
				std::int32_t format = 0;
				bool once = false;

				std::vector<DirectX::Image> images{};

				for (unsigned int image_index = 1; image_index < draw->reflectionProbeCount; image_index++)
				{
					GfxImage* probe_image = draw->reflectionProbes[image_index];
					std::uint8_t* data = probe_image->pixelData;

					if (once) {
						assert(width == probe_image->width && height == probe_image->height && format == probe_image->imageFormat);
					}

					width = probe_image->width;
					height = probe_image->height;
					depth = probe_image->depth;
					mip_levels = probe_image->levelCount;
					format = probe_image->imageFormat;
					once = true;

					for (auto a = 0; a < 6; a++)
					{
						unsigned int divider = 1;
						for (auto i = 0; i < (int)probe_image->levelCount; i++)
						{
							DirectX::Image srcImg{};
							srcImg.width = std::max(1u, probe_image->width / divider);
							srcImg.height = std::max(1u, probe_image->height / divider);
							srcImg.format = DXGI_FORMAT(probe_image->imageFormat);
							srcImg.pixels = data;

							DirectX::ComputePitch(srcImg.format, srcImg.width, srcImg.height, srcImg.rowPitch, srcImg.slicePitch);

							DirectX::ScratchImage hdrTemp;
							auto hr = DirectX::Convert(srcImg, DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, hdrTemp);

							if (FAILED(hr)) return nullptr;

							auto* persistentPixels = allocator.allocate_array<uint8_t>(hdrTemp.GetPixelsSize());
							memcpy(persistentPixels, hdrTemp.GetPixels(), hdrTemp.GetPixelsSize());

							DirectX::Image finalImg{};
							finalImg.width = srcImg.width;
							finalImg.height = srcImg.height;
							finalImg.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
							finalImg.pixels = persistentPixels;
							DirectX::ComputePitch(finalImg.format, finalImg.width, finalImg.height, finalImg.rowPitch, finalImg.slicePitch);

							images.push_back(finalImg);

							data += srcImg.slicePitch;
							divider *= 2;
						}
					}
				}

				DirectX::TexMetadata mdata{};
				mdata.width = width;
				mdata.height = height;
				mdata.depth = depth;
				mdata.arraySize = (draw->reflectionProbeCount - 1) * 6;
				mdata.mipLevels = mip_levels;
				mdata.format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				mdata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
				mdata.miscFlags |= DirectX::TEX_MISC_TEXTURECUBE;

				DirectX::ScratchImage compressed;
				auto hr = DirectX::Compress(images.data(), images.size(), mdata, DXGI_FORMAT_BC6H_UF16, DirectX::TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, compressed);

				if (FAILED(hr)) return nullptr;

				std::string spath = filesystem::get_dump_path() + "images\\" + image_name_clean + ".dds";
				std::wstring wpath(spath.begin(), spath.end());
				std::filesystem::create_directories(filesystem::get_dump_path() + "images\\");

				hr = DirectX::SaveToDDSFile(compressed.GetImages(), compressed.GetImageCount(), compressed.GetMetadata(), DirectX::DDS_FLAGS_NONE, wpath.data());
				if (FAILED(hr)) return nullptr;

				auto* image = allocator.allocate<zonetool::iw7::GfxImage>();
				image->name = allocator.duplicate_string(image_name);
				return image;
			}

			zonetool::iw7::GfxWorld* convert(GfxWorld* asset, utils::memory::allocator& allocator)
			{
				auto new_asset = allocator.allocate<zonetool::iw7::GfxWorld>();

				REINTERPRET_CAST_SAFE(name);
				REINTERPRET_CAST_SAFE(baseName);
				new_asset->bspVersion = 159;
				COPY_VALUE(planeCount);
				COPY_VALUE(nodeCount);
				COPY_VALUE(surfaceCount);

				COPY_VALUE(skyCount);
				new_asset->skies = allocator.allocate_array<zonetool::iw7::GfxSky>(asset->skyCount);
				for (int i = 0; i < asset->skyCount; i++)
				{
					COPY_VALUE(skies[i].skySurfCount);
					REINTERPRET_CAST_SAFE(skies[i].skyStartSurfs);
					COPY_ASSET(skies[i].skyImage);
					COPY_VALUE(skies[i].skySamplerState);
				}

				COPY_VALUE(lastSunPrimaryLightIndex);
				COPY_VALUE(primaryLightCount);
				new_asset->movingScriptablePrimaryLightCount = 0;

				new_asset->sortKeyLitDecal = 7;
				new_asset->sortKeyEffectDecal = 14;
				new_asset->sortKeyTopDecal = 17;
				new_asset->sortKeyEffectAuto = 35;
				new_asset->sortKeyDistortion = 24;
				new_asset->sortKeyEffectDistortion = 36;
				new_asset->sortKey2D = 41;
				new_asset->sortKeyOpaqueBegin = 1;
				new_asset->sortKeyOpaqueEnd = 6;
				new_asset->sortKeyDecalBegin = 7;
				new_asset->sortKeyDecalEnd = 17;
				new_asset->sortKeyTransBegin = 18;
				new_asset->sortKeyTransEnd = 34;
				new_asset->sortKeyEmissiveBegin = 35;
				new_asset->sortKeyEmissiveEnd = 40;

				COPY_VALUE(dpvsPlanes.cellCount);
				REINTERPRET_CAST_SAFE(dpvsPlanes.planes);
				REINTERPRET_CAST_SAFE(dpvsPlanes.nodes);
				REINTERPRET_CAST_SAFE(dpvsPlanes.sceneEntCellBits);

				new_asset->cells = allocator.allocate_array<zonetool::iw7::GfxCell>(asset->dpvsPlanes.cellCount);
				for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
				{
					COPY_ARR(cells[i].bounds);
					COPY_VALUE(cells[i].portalCount);
					REINTERPRET_CAST_SAFE(cells[i].portals);
				}

				new_asset->cellTransientInfos = allocator.allocate_array<zonetool::iw7::GfxCellTransientInfo>(asset->dpvsPlanes.cellCount);
				for (unsigned short i = 0; i < asset->dpvsPlanes.cellCount; i++)
				{
					new_asset->cellTransientInfos[i].aabbTreeIndex = i;
					new_asset->cellTransientInfos[i].transientZone = 0;
				}

				assert(asset->draw.reflectionProbeCount);
				new_asset->draw.reflectionProbeData.reflectionProbeCount = asset->draw.reflectionProbeCount;
				new_asset->draw.reflectionProbeData.sharedReflectionProbeCount = 0;
				new_asset->draw.reflectionProbeData.reflectionProbes = allocator.allocate_array<zonetool::iw7::GfxReflectionProbe>(asset->draw.reflectionProbeCount);
				new_asset->draw.reflectionProbeData.reflectionProbeArrayImage = generate_reflection_probe_array_image(&asset->draw, allocator);

				new_asset->draw.reflectionProbeData.probeRelightingCount = 0;
				new_asset->draw.reflectionProbeData.probeRelightingData = nullptr;

				new_asset->draw.reflectionProbeData.reflectionProbeGBufferImageCount = 0;
				new_asset->draw.reflectionProbeData.reflectionProbeGBufferImages = nullptr;
				new_asset->draw.reflectionProbeData.reflectionProbeGBufferTextures = nullptr;

				new_asset->draw.reflectionProbeData.reflectionProbeLightgridSampleData = 
					allocator.allocate_array<zonetool::iw7::GfxReflectionProbeSampleData>(new_asset->draw.reflectionProbeData.reflectionProbeCount);
				new_asset->draw.reflectionProbeData.reflectionProbeLightgridSampleDataBuffer = nullptr;
				new_asset->draw.reflectionProbeData.reflectionProbeLightgridSampleDataBufferView = nullptr;
				new_asset->draw.reflectionProbeData.reflectionProbeLightgridSampleDataBufferRWView = nullptr;

				unsigned int totalInstanceCount = 1; // Start with 1 for the null/fallback probe

				// 1. First Pass: Calculate total instances needed
				for (unsigned int i = 0; i < asset->draw.reflectionProbeCount; i++)
				{
					auto& srcProbe = asset->draw.reflectionProbeOrigins[i];
					if (!srcProbe.probeVolumes || srcProbe.probeVolumeCount == 0)
						totalInstanceCount += 1;
					else
						totalInstanceCount += srcProbe.probeVolumeCount;
				}

				// 2. Allocation
				new_asset->draw.reflectionProbeData.reflectionProbeInstanceCount = totalInstanceCount;

				// Allocate the actual instance data
				auto* instances = allocator.allocate_array<zonetool::iw7::GfxReflectionProbeInstance>(totalInstanceCount);
				new_asset->draw.reflectionProbeData.reflectionProbeInstances = instances;

				// Allocate the index buffer (The size must match the total number of instances)
				auto* globalProbeInstanceIndices = allocator.allocate_array<unsigned int>(totalInstanceCount);

				// ------------------------------------------------------------
				// 3. Build Pass
				// ------------------------------------------------------------
				unsigned int instanceCursor = 0;

				for (unsigned int i = 0; i < new_asset->draw.reflectionProbeData.reflectionProbeCount; i++)
				{
					auto& dstProbe = new_asset->draw.reflectionProbeData.reflectionProbes[i];
					auto& srcProbe = asset->draw.reflectionProbeOrigins[i];

					dstProbe.livePath = nullptr;
					memcpy(dstProbe.origin, srcProbe.origin, sizeof(vec3_t));
					memset(dstProbe.angles, 0, sizeof(vec3_t));
					dstProbe.probeRelightingIndex = static_cast<unsigned int>(-1);

					// POINT TO THE CURRENT POSITION IN THE GLOBAL INDEX BUFFER
					dstProbe.probeInstances = &globalProbeInstanceIndices[instanceCursor];

					// --- Fallback / Null Probe ---
					if (!srcProbe.probeVolumes || srcProbe.probeVolumeCount == 0)
					{
						dstProbe.probeInstanceCount = 1;

						// Map the index to the current instance index
						dstProbe.probeInstances[0] = instanceCursor;

						auto& inst = instances[instanceCursor++];
						memset(&inst, 0, sizeof(inst));
						memcpy(inst.probePosition, srcProbe.origin, sizeof(vec3_t));
						inst.probeImageIndex = static_cast<unsigned short>(i);
						inst.priority = -1.0f;

						inst.probeRotation[0] = 0.0f;
						inst.probeRotation[1] = 0.0f;
						inst.probeRotation[2] = 0.0f;
						inst.probeRotation[3] = 1.0f;

						if (i == 0)
						{
							inst.priority = -FLT_MAX;

							memcpy(inst.volumeObb.center,
								new_asset->draw.reflectionProbeData.reflectionProbes[0].origin,
								sizeof(vec3_t));

							inst.volumeObb.halfSize[0] = 262144.0f;
							inst.volumeObb.halfSize[1] = 262144.0f;
							inst.volumeObb.halfSize[2] = 262144.0f;

							inst.volumeObb.xAxis[0] = 1.0f;
							inst.volumeObb.xAxis[1] = 0.0f;
							inst.volumeObb.xAxis[2] = 0.0f;

							inst.volumeObb.yAxis[0] = 0.0f;
							inst.volumeObb.yAxis[1] = 1.0f;
							inst.volumeObb.yAxis[2] = 0.0f;

							inst.volumeObb.zAxis[0] = 0.0f;
							inst.volumeObb.zAxis[1] = 0.0f;
							inst.volumeObb.zAxis[2] = 1.0f;

							inst.feather[0] = 8.0f; inst.feather[1] = 8.0f; inst.feather[2] = 8.0f;
						}

						continue;
					}

					// --- Real Volumes ---
					dstProbe.probeInstanceCount = srcProbe.probeVolumeCount;

					for (unsigned int v = 0; v < srcProbe.probeVolumeCount; v++)
					{
						// Map the local index to the global instance cursor
						dstProbe.probeInstances[v] = instanceCursor;

						auto& inst = instances[instanceCursor++];
						memset(&inst, 0, sizeof(inst));

						inst.probeImageIndex = static_cast<unsigned short>(i);
						memcpy(inst.probePosition, srcProbe.origin, sizeof(vec3_t));
						inst.priority = -1.0f;

						auto& volume = srcProbe.probeVolumes[v];

						// Plane collection and building
						std::vector<cplane_s> planes;
						planes.reserve(volume.probeVolumePlaneCount);
						for (unsigned int p = 0; p < volume.probeVolumePlaneCount; p++) {
							planes.push_back(asset->dpvsPlanes.planes[volume.probeVolumePlanes[p]]);
						}

						build_probe_instance(inst, planes.data(), (unsigned int)planes.size(), srcProbe.origin);
						memcpy(inst.feather, inst.volumeObb.halfSize, sizeof(vec3_t));
					}
				}

				// todo...
				new_asset->draw.lightmapReindexData;

				new_asset->draw.iesLookupTexture = allocator.allocate<zonetool::iw7::GfxImage>();
				new_asset->draw.iesLookupTexture->name = allocator.duplicate_string("*ieslookup");

				new_asset->draw.decalVolumeCollectionCount = 0;
				new_asset->draw.decalVolumeCollections = nullptr;
				
				COPY_ASSET(draw.lightmapOverridePrimary);
				COPY_ASSET(draw.lightmapOverrideSecondary);
				
				new_asset->draw.lightMapCount = asset->draw.lightmapCount;
				new_asset->draw.lightMaps = allocator.allocate_array<zonetool::iw7::GfxLightMap*>(asset->draw.lightmapCount);
				for (int i = 0; i < asset->draw.lightmapCount; i++)
				{
					new_asset->draw.lightMaps[i] = allocator.allocate<zonetool::iw7::GfxLightMap>();
					new_asset->draw.lightMaps[i]->name = allocator.duplicate_string(utils::string::va("*lightmap%d", i));
					if (asset->draw.lightmaps[i].primary) // primary
					{
						new_asset->draw.lightMaps[i]->textures[0] = allocator.allocate<zonetool::iw7::GfxImage>();
						new_asset->draw.lightMaps[i]->textures[0]->name = asset->draw.lightmaps[i].primary->name;
					}
					if (asset->draw.lightmaps[i].secondary) // secondary
					{
						new_asset->draw.lightMaps[i]->textures[1] = allocator.allocate<zonetool::iw7::GfxImage>();
						new_asset->draw.lightMaps[i]->textures[1]->name = asset->draw.lightmaps[i].secondary->name;
					}
					//new_asset->draw.lightMaps[i]->textures[2] = nullptr; // secondunorm
					new_asset->draw.lightMaps[i]->textures[2] = allocator.allocate<zonetool::iw7::GfxImage>();
					new_asset->draw.lightMaps[i]->textures[2]->name = allocator.duplicate_string(utils::string::va("*lightmap%d_secondunorm", i));
				}
				new_asset->draw.lightmapTextures = nullptr; // runtime data, allocated elsewhere

				new_asset->draw.unused1 = nullptr;
				new_asset->draw.unused2 = nullptr;
				new_asset->draw.unused3 = nullptr;

				new_asset->draw.transientZoneCount = 1;
				new_asset->draw.transientZones[0] = allocator.allocate<zonetool::iw7::GfxWorldTransientZone>();
				new_asset->draw.transientZones[0]->name = allocator.duplicate_string(filesystem::get_fastfile());
				new_asset->draw.transientZones[0]->transientZoneIndex = 0;

				new_asset->draw.transientZones[0]->vertexCount = asset->draw.vertexCount;
				new_asset->draw.transientZones[0]->vd.vertices = allocator.allocate_array<zonetool::iw7::GfxWorldVertex>(asset->draw.vertexCount);
				for (unsigned int i = 0; i < asset->draw.vertexCount; i++)
				{
					static_assert(sizeof(zonetool::h1::GfxWorldVertex) == sizeof(zonetool::iw7::GfxWorldVertex));
					memcpy(&new_asset->draw.transientZones[0]->vd.vertices[i], &asset->draw.vd.vertices[i], sizeof(zonetool::h1::GfxWorldVertex));

					//float default_visibility[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
					//new_asset->draw.transientZones[0]->vd.vertices[i].selfVisibility.packed = self_visibility::XSurfacePackSelfVisibility(default_visibility);
				}

				new_asset->draw.transientZones[0]->vertexLayerDataSize = asset->draw.vertexLayerDataSize;
				new_asset->draw.transientZones[0]->vld.data = asset->draw.vld.data;

				new_asset->draw.transientZones[0]->cellCount = asset->dpvsPlanes.cellCount;

				new_asset->draw.transientZones[0]->aabbTreeCounts = allocator.allocate_array<zonetool::iw7::GfxCellTreeCount>(asset->dpvsPlanes.cellCount);
				new_asset->draw.transientZones[0]->aabbTrees = allocator.allocate_array<zonetool::iw7::GfxCellTree>(asset->dpvsPlanes.cellCount);
				for (int i = 0; i < asset->dpvsPlanes.cellCount; i++)
				{
					new_asset->draw.transientZones[0]->aabbTreeCounts[i].aabbTreeCount = asset->aabbTreeCounts[i].aabbTreeCount;
					new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree = allocator.allocate_array<zonetool::iw7::GfxAabbTree>(asset->aabbTreeCounts[i].aabbTreeCount);
					for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
					{
						memcpy(&new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].bounds, &asset->aabbTrees[i].aabbTree[j].bounds, sizeof(float[2][3]));

						new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].startSurfIndex = asset->aabbTrees[i].aabbTree[j].startSurfIndex;
						new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].surfaceCount = asset->aabbTrees[i].aabbTree[j].surfaceCount;

						new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].smodelIndexCount = asset->aabbTrees[i].aabbTree[j].smodelIndexCount;
						new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].smodelIndexes = asset->aabbTrees[i].aabbTree[j].smodelIndexes;

						new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].childCount = asset->aabbTrees[i].aabbTree[j].childCount;

						// re-calculate childrenOffset
						auto offset = asset->aabbTrees[i].aabbTree[j].childrenOffset;
						int childrenIndex = offset / sizeof(zonetool::h1::GfxAabbTree);
						int childrenOffset = childrenIndex * sizeof(zonetool::iw7::GfxAabbTree);
						new_asset->draw.transientZones[0]->aabbTrees[i].aabbTree[j].childrenOffset = childrenOffset;
					}
				}

				new_asset->draw.indexCount = asset->draw.indexCount;
				new_asset->draw.indices = asset->draw.indices;

				new_asset->draw.volumetrics.volumetricCount = 0;
				new_asset->draw.volumetrics.volumetrics = nullptr;
				constexpr int unk_values[] = {0, 0, 5, 5, 6, 32, 32, 64, 0};
				memcpy(new_asset->lightGrid.unk, unk_values, sizeof(unk_values));
				COPY_VALUE(lightGrid.tableVersion);
				COPY_VALUE(lightGrid.paletteVersion);
				COPY_VALUE(lightGrid.rangeExponent8BitsEncoding);
				COPY_VALUE(lightGrid.rangeExponent12BitsEncoding);
				COPY_VALUE(lightGrid.rangeExponent16BitsEncoding);
				COPY_VALUE(lightGrid.stageCount);
				COPY_VALUE(lightGrid.stageLightingContrastGain);
				COPY_VALUE(lightGrid.paletteEntryCount);
				COPY_VALUE(lightGrid.paletteEntryAddress);
				COPY_VALUE(lightGrid.paletteBitstreamSize);
				COPY_VALUE(lightGrid.paletteBitstream);
				for (unsigned int j = 0; j < 56; j++)
				{
					auto& rgb = asset->lightGrid.skyLightGridColors.rgb[j];
					auto& dest_rgb = new_asset->lightGrid.skyLightGridColors.rgb[j];
					dest_rgb[0] = half_float::half_to_float(rgb[0]);
					dest_rgb[1] = half_float::half_to_float(rgb[1]);
					dest_rgb[2] = half_float::half_to_float(rgb[2]);
				}
				for (unsigned int j = 0; j < 56; j++)
				{
					auto& rgb = asset->lightGrid.defaultLightGridColors.rgb[j];
					auto& dest_rgb = new_asset->lightGrid.defaultLightGridColors.rgb[j];
					dest_rgb[0] = half_float::half_to_float(rgb[0]);
					dest_rgb[1] = half_float::half_to_float(rgb[1]);
					dest_rgb[2] = half_float::half_to_float(rgb[2]);
				}
				new_asset->lightGrid.tree.maxDepth = asset->lightGrid.tree[0].maxDepth;
				new_asset->lightGrid.tree.nodeCount = asset->lightGrid.tree[0].nodeCount;
				new_asset->lightGrid.tree.leafCount = asset->lightGrid.tree[0].leafCount;
				memcpy(&new_asset->lightGrid.tree.coordMinGridSpace, &asset->lightGrid.tree[0].coordMinGridSpace, sizeof(int[3]));
				memcpy(&new_asset->lightGrid.tree.coordMaxGridSpace, &asset->lightGrid.tree[0].coordMaxGridSpace, sizeof(int[3]));
				memcpy(&new_asset->lightGrid.tree.coordHalfSizeGridSpace, &asset->lightGrid.tree[0].coordHalfSizeGridSpace, sizeof(int[3]));
				new_asset->lightGrid.tree.defaultColorIndexBitCount = asset->lightGrid.tree[0].defaultColorIndexBitCount;
				new_asset->lightGrid.tree.defaultLightIndexBitCount = asset->lightGrid.tree[0].defaultLightIndexBitCount;
				new_asset->lightGrid.tree.p_nodeTable = asset->lightGrid.tree[0].p_nodeTable;
				new_asset->lightGrid.tree.leafTableSize = asset->lightGrid.tree[0].leafTableSize;
				new_asset->lightGrid.tree.p_leafTable = asset->lightGrid.tree[0].p_leafTable;

				memset(&new_asset->lightGrid.probeData, 0, sizeof(zonetool::iw7::GfxLightGridProbeData));
				// fixme somehow...
				new_asset->lightGrid.probeData.zoneCount = 1;
				new_asset->lightGrid.probeData.zones = allocator.allocate_array<zonetool::iw7::GfxGpuLightGridZone>(1);
				new_asset->lightGrid.probeData.zones->numProbes = 0;
				new_asset->lightGrid.probeData.zones->firstProbe = 0;
				new_asset->lightGrid.probeData.zones->numTetrahedrons = 0;
				new_asset->lightGrid.probeData.zones->firstTetrahedron = 0;
				new_asset->lightGrid.probeData.zones->firstVoxelTetrahedronIndex = 0;
				new_asset->lightGrid.probeData.zones->numVoxelTetrahedronIndices = 0;
				memset(new_asset->lightGrid.probeData.zones->fallbackProbeData.coeffs, 0, sizeof(new_asset->lightGrid.probeData.zones->fallbackProbeData.coeffs));
				memset(new_asset->lightGrid.probeData.zones->fallbackProbeData.pad, 0, sizeof(new_asset->lightGrid.probeData.zones->fallbackProbeData.pad));

				// don't need to fill this data i guess?
				new_asset->frustumLights = allocator.allocate_array<zonetool::iw7::GfxFrustumLights>(new_asset->primaryLightCount);
				new_asset->lightViewFrustums = allocator.allocate_array<zonetool::iw7::GfxLightViewFrustum>(new_asset->primaryLightCount);

				new_asset->voxelTreeCount = new_asset->skyCount;
				new_asset->voxelTree = allocator.allocate_array<zonetool::iw7::GfxVoxelTree>(new_asset->voxelTreeCount);
				for (auto i = 0; i < new_asset->skyCount; i++)
				{
					memcpy(&new_asset->voxelTree[i].zoneBound, &asset->skies[i].bounds, sizeof(Bounds));
					
					new_asset->voxelTree[i].voxelTopDownViewNodeCount = 1;
					new_asset->voxelTree[i].voxelInternalNodeCount = 1;
					new_asset->voxelTree[i].voxelLeafNodeCount = 1;
					new_asset->voxelTree[i].lightListArraySize = 1;

					new_asset->voxelTree[i].voxelTreeHeader = allocator.allocate<zonetool::iw7::GfxVoxelTreeHeader>();
					memset(&new_asset->voxelTree[i].voxelTreeHeader->rootNodeDimension, 0, sizeof(int[4]));
					memset(&new_asset->voxelTree[i].voxelTreeHeader->nodeCoordBitShift, 0, sizeof(int[4]));
					memset(&new_asset->voxelTree[i].voxelTreeHeader->boundMin, 0, sizeof(float[4]));
					memset(&new_asset->voxelTree[i].voxelTreeHeader->boundMax, 0, sizeof(float[4]));

					new_asset->voxelTree[i].voxelTopDownViewNodeArray = allocator.allocate_array<zonetool::iw7::GfxVoxelTopDownViewNode>(new_asset->voxelTree[i].voxelTopDownViewNodeCount);
					new_asset->voxelTree[i].voxelTopDownViewNodeArray->firstNodeIndex = -1;
					new_asset->voxelTree[i].voxelTopDownViewNodeArray->zMin = 2147483647;
					new_asset->voxelTree[i].voxelTopDownViewNodeArray->zMax = -2147483648;
					
					new_asset->voxelTree[i].voxelInternalNodeArray = allocator.allocate_array<zonetool::iw7::GfxVoxelInternalNode>(new_asset->voxelTree[i].voxelInternalNodeCount);
					new_asset->voxelTree[i].voxelInternalNodeArray->firstNodeIndex[0] = 0;
					new_asset->voxelTree[i].voxelInternalNodeArray->firstNodeIndex[1] = 0;
					new_asset->voxelTree[i].voxelInternalNodeArray->childNodeMask[0] = 0;
					new_asset->voxelTree[i].voxelInternalNodeArray->childNodeMask[1] = 0;

					new_asset->voxelTree[i].voxelLeafNodeArray = allocator.allocate_array<zonetool::iw7::GfxVoxelLeafNode>(new_asset->voxelTree[i].voxelLeafNodeCount);
					new_asset->voxelTree[i].voxelLeafNodeArray->lightListAddress = 0;

					new_asset->voxelTree[i].lightListArray = allocator.allocate_array<unsigned short>(new_asset->voxelTree[i].lightListArraySize);
					new_asset->voxelTree[i].lightListArray[0] = 0;

					new_asset->voxelTree[i].voxelInternalNodeDynamicLightList = allocator.allocate_array<unsigned int>(2 * new_asset->voxelTree[i].voxelInternalNodeCount);
					new_asset->voxelTree[i].voxelInternalNodeDynamicLightList[0] = 0;
					new_asset->voxelTree[i].voxelInternalNodeDynamicLightList[1] = 0;
				}

				new_asset->heightfieldCount = 0;
				new_asset->heightfields = nullptr;

				new_asset->unk01.unk01Count = 0;
				new_asset->unk01.unk01 = nullptr;
				new_asset->unk01.unk02Count = 0;
				new_asset->unk01.unk02 = nullptr;
				new_asset->unk01.unk03Count = 0;
				new_asset->unk01.unk03 = nullptr;
				
				COPY_VALUE(modelCount);
				new_asset->models = allocator.allocate_array<zonetool::iw7::GfxBrushModel>(asset->modelCount);
				for (int i = 0; i < asset->modelCount; i++)
				{
					COPY_ARR(models[i].bounds);
					COPY_VALUE(models[i].radius);
					COPY_VALUE(models[i].startSurfIndex);
					COPY_VALUE(models[i].surfaceCount);
				}

				std::memcpy(&new_asset->bounds, &asset->bounds, sizeof(Bounds));

				COPY_VALUE(checksum);
				COPY_VALUE(materialMemoryCount);
				REINTERPRET_CAST_SAFE(materialMemory);
				COPY_VALUE_CAST(sun);
				COPY_ARR(outdoorLookupMatrix);
				COPY_ASSET(outdoorImage);
				new_asset->dustMaterial = nullptr;
				new_asset->materialLod0SizeThreshold = 0.5f;

				REINTERPRET_CAST_SAFE(shadowGeomOptimized);
				REINTERPRET_CAST_SAFE(lightRegion);

				new_asset->lightAABB.nodeCount = 0;
				new_asset->lightAABB.lightCount = 0;
				new_asset->lightAABB.nodeArray = nullptr;
				new_asset->lightAABB.lightArray = nullptr;

				// dpvs
				{
					COPY_VALUE(dpvs.smodelCount);
					COPY_VALUE(dpvs.staticSurfaceCount);
					COPY_VALUE(dpvs.litOpaqueSurfsBegin);
					COPY_VALUE(dpvs.litOpaqueSurfsEnd);
					COPY_VALUE(dpvs.litDecalSurfsBegin);
					COPY_VALUE(dpvs.litDecalSurfsEnd);
					COPY_VALUE(dpvs.litTransSurfsBegin);
					COPY_VALUE(dpvs.litTransSurfsEnd);
					COPY_VALUE(dpvs.emissiveSurfsBegin);
					COPY_VALUE(dpvs.emissiveSurfsEnd);
					new_asset->dpvs.smodelVisDataCount = (new_asset->dpvs.smodelCount + 0x1F) >> 5;
					new_asset->dpvs.surfaceVisDataCount = (new_asset->surfaceCount + 0x1F) >> 5;
					new_asset->dpvs.primaryLightVisDataCount = (new_asset->primaryLightCount + 0x1F) >> 5;
					new_asset->dpvs.reflectionProbeVisDataCount = (new_asset->draw.reflectionProbeData.reflectionProbeInstanceCount + 0x1F) >> 5;
					new_asset->dpvs.volumetricVisDataCount = (new_asset->draw.volumetrics.volumetricCount + 0x1F) >> 5;
					new_asset->dpvs.decalVisDataCount = (new_asset->draw.decalVolumeCollectionCount + 0x1F) >> 5;
					REINTERPRET_CAST_SAFE(dpvs.lodData);
					REINTERPRET_CAST_SAFE(dpvs.sortedSurfIndex);
					REINTERPRET_CAST_SAFE(dpvs.smodelInsts);

					new_asset->dpvs.surfaces = allocator.allocate_array<zonetool::iw7::GfxSurface>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						COPY_VALUE(dpvs.surfaces[i].tris.vertexLayerData);
						COPY_VALUE(dpvs.surfaces[i].tris.firstVertex);
						COPY_VALUE(dpvs.surfaces[i].tris.maxEdgeLength);
						COPY_VALUE(dpvs.surfaces[i].tris.vertexCount);
						COPY_VALUE(dpvs.surfaces[i].tris.triCount);
						COPY_VALUE(dpvs.surfaces[i].tris.baseIndex);
						new_asset->dpvs.surfaces[i].material = reinterpret_cast<zonetool::iw7::Material*>(asset->dpvs.surfaces[i].material);
						new_asset->dpvs.surfaces[i].lightmapIndex = asset->dpvs.surfaces[i].laf.fields.lightmapIndex;
						new_asset->dpvs.surfaces[i].flags = asset->dpvs.surfaces[i].laf.fields.flags;

						new_asset->dpvs.surfaces[i].unk1 = 0;
						new_asset->dpvs.surfaces[i].unk2 = 0;
						new_asset->dpvs.surfaces[i].unk3 = 0;
						new_asset->dpvs.surfaces[i].unk4 = 0;

						new_asset->dpvs.surfaces[i].transientZone = 0;
					}

					new_asset->dpvs.surfacesBounds = allocator.allocate_array<zonetool::iw7::GfxSurfaceBounds>(asset->surfaceCount);
					for (unsigned int i = 0; i < asset->surfaceCount; i++)
					{
						COPY_ARR(dpvs.surfacesBounds[i].bounds);
					}

					new_asset->dpvs.smodelDrawInsts = allocator.allocate_array<zonetool::iw7::GfxStaticModelDrawInst>(asset->dpvs.smodelCount);
					for (unsigned int i = 0; i < asset->dpvs.smodelCount; i++)
					{
						COPY_VALUE_CAST(dpvs.smodelDrawInsts[i].placement);

						new_asset->dpvs.smodelDrawInsts[i].model = 
							reinterpret_cast<zonetool::iw7::XModel*>(asset->dpvs.smodelDrawInsts[i].model);

						auto& src_draw_inst = asset->dpvs.smodelDrawInsts[i];

						new_asset->dpvs.smodelDrawInsts[i].modelLightmapInfo.lightmapIndex = -1;

						if ((src_draw_inst.flags & STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING) != 0)
						{
							new_asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.numLightingValues = asset->dpvs.smodelLightingInsts[i].vertexLightingInfo.numLightingValues;
							new_asset->dpvs.smodelDrawInsts[i].vertexLightingInfo.lightingValues =
								reinterpret_cast<zonetool::iw7::GfxStaticModelVertexLighting*>(asset->dpvs.smodelLightingInsts[i].vertexLightingInfo.lightingValues);
						}
						else if ((src_draw_inst.flags & STATIC_MODEL_FLAG_LIGHTMAP_LIGHTING) != 0)
						{
							new_asset->dpvs.smodelDrawInsts[i].modelLightmapInfo.lightmapIndex = asset->dpvs.smodelLightingInsts[i].modelLightmapInfo.lightmapIndex;
							memcpy(&new_asset->dpvs.smodelDrawInsts[i].modelLightmapInfo.offset, &asset->dpvs.smodelLightingInsts[i].modelLightmapInfo.offset, sizeof(float[2]));
							memcpy(&new_asset->dpvs.smodelDrawInsts[i].modelLightmapInfo.scale, &asset->dpvs.smodelLightingInsts[i].modelLightmapInfo.scale, sizeof(float[2]));
						}

						new_asset->dpvs.smodelDrawInsts[i].lightingHandle = asset->dpvs.smodelDrawInsts[i].lightingHandle;
						new_asset->dpvs.smodelDrawInsts[i].cullDist = asset->dpvs.smodelDrawInsts[i].cullDist;
						new_asset->dpvs.smodelDrawInsts[i].flags = asset->dpvs.smodelDrawInsts[i].flags;
						new_asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex = asset->dpvs.smodelDrawInsts[i].primaryLightEnvIndex;
						new_asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex = asset->dpvs.smodelDrawInsts[i].reflectionProbeIndex;
						new_asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex = asset->dpvs.smodelDrawInsts[i].firstMtlSkinIndex;
						new_asset->dpvs.smodelDrawInsts[i].sunShadowFlags = asset->dpvs.smodelDrawInsts[i].sunShadowFlags;

						new_asset->dpvs.smodelDrawInsts[i].transientZone = 0;
					}

					//REINTERPRET_CAST_SAFE(dpvs.surfaceMaterials);
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadow);
					COPY_VALUE(dpvs.sunShadowOptCount);
					COPY_VALUE(dpvs.sunSurfVisDataCount);
					REINTERPRET_CAST_SAFE(dpvs.surfaceCastsSunShadowOpt);
					new_asset->dpvs.sortedSmodelIndices = allocator.allocate_array<unsigned short>(asset->dpvs.smodelCount); // todo...
					//REINTERPRET_CAST_SAFE(dpvs.constantBuffers);
					COPY_VALUE(dpvs.usageCount);
				}

				COPY_ARR(dpvsDyn.dynEntClientWordCount);
				COPY_ARR(dpvsDyn.dynEntClientCount);
				//COPY_ARR(dpvsDyn.dynEntCellBits);

				new_asset->dpvsDyn.dynEntClientCount[0] += 64; // reserve_dynents
				new_asset->dpvsDyn.dynEntClientWordCount[0] += 2; // reserve_dynents ( 64 >> 5 )

				new_asset->dpvsDyn.dynEntCellBits[0] = allocator.allocate_array<unsigned int>(new_asset->dpvsDyn.dynEntClientCount[0] * new_asset->dpvsPlanes.cellCount); // runtime
				new_asset->dpvsDyn.dynEntCellBits[1] = allocator.allocate_array<unsigned int>(new_asset->dpvsDyn.dynEntClientCount[1] * new_asset->dpvsPlanes.cellCount); // runtime

				// 0 - 3 are valid.
				new_asset->dpvsDyn.dynEntVisData[0][0] = allocator.allocate_array<unsigned char>(32 * new_asset->dpvsDyn.dynEntClientWordCount[0]); // runtime
				new_asset->dpvsDyn.dynEntVisData[0][1] = allocator.allocate_array<unsigned char>(32 * new_asset->dpvsDyn.dynEntClientWordCount[0]); // runtime
				new_asset->dpvsDyn.dynEntVisData[0][2] = allocator.allocate_array<unsigned char>(32 * new_asset->dpvsDyn.dynEntClientWordCount[0]); // runtime

				new_asset->dpvsDyn.dynEntVisData[1][0] = allocator.allocate_array<unsigned char>(32 * new_asset->dpvsDyn.dynEntClientWordCount[1]); // runtime
				new_asset->dpvsDyn.dynEntVisData[1][1] = allocator.allocate_array<unsigned char>(32 * new_asset->dpvsDyn.dynEntClientWordCount[1]); // runtime
				new_asset->dpvsDyn.dynEntVisData[1][2] = allocator.allocate_array<unsigned char>(32 * new_asset->dpvsDyn.dynEntClientWordCount[1]); // runtime

				COPY_VALUE(mapVtxChecksum);
				COPY_VALUE(heroOnlyLightCount);
				REINTERPRET_CAST_SAFE(heroOnlyLights);

				// Umbra needs to have valid data...
				new_asset->numUmbraGates = 0;
				new_asset->umbraGates = nullptr;
				COPY_VALUE(umbraTomeSize);
				REINTERPRET_CAST_SAFE(umbraTomeData);
				COPY_VALUE_CAST(umbraTomePtr);

				new_asset->numUmbraGates2 = 0;
				new_asset->umbraGates2 = nullptr;
				new_asset->umbraTomeSize2 = 0;
				new_asset->umbraTomeData2 = nullptr;
				new_asset->umbraTomePtr2 = nullptr;
				new_asset->umbraUnkSize = 0;
				new_asset->umbraUnkData = nullptr;

				// umbra gates might not be needed?
				// generate umbra gates from cells
				//new_asset->numUmbraGates = new_asset->dpvsPlanes.cellCount;
				//new_asset->umbraGates = allocator.allocate_array<zonetool::iw7::UmbraGate>(new_asset->numUmbraGates);
				//for (auto i = 0; i < new_asset->numUmbraGates; i++)
				//{
				//	// just use the cell bounds for the gate bounds, not sure if this is correct but it should work
				//	memcpy(&new_asset->umbraGates[i].bounds, &asset->cells[i].bounds, sizeof(float[2][3]));
				//	new_asset->umbraGates[i].objID = i;
				//	new_asset->umbraGates[i].closeDistance = 0;
				//}

				// generate umbra tome
				//if (asset->umbraTomeData) // for old tome data we just need to add bounds
				//{
				//  // i think this is not correct yet, also it seems to work even though new data is garbage.
				//	constexpr auto extra_data_size = sizeof(zonetool::iw7::Umbra::ImpTome) - sizeof(zonetool::h1::Umbra::ImpTome);
				//	const auto* old_tome = reinterpret_cast<const zonetool::h1::Umbra::ImpTome*>(asset->umbraTomeData);
				//
				//	new_asset->umbraTomeSize = asset->umbraTomeSize + extra_data_size;
				//	new_asset->umbraTomeData = allocator.manual_allocate<char>(new_asset->umbraTomeSize);
				//
				//	auto* new_tome = reinterpret_cast<zonetool::iw7::Umbra::ImpTome*>(new_asset->umbraTomeData);
				//
				//	// copy old data
				//	memcpy(new_asset->umbraTomeData, asset->umbraTomeData, sizeof(zonetool::h1::Umbra::ImpTome));
				//	memcpy(new_asset->umbraTomeData + sizeof(zonetool::iw7::Umbra::ImpTome), 
				//		asset->umbraTomeData + sizeof(zonetool::h1::Umbra::ImpTome), 
				//		asset->umbraTomeSize - sizeof(zonetool::h1::Umbra::ImpTome));
				//
				//	// add new data
				//	new_tome->m_boundsMin.x = new_asset->bounds.midPoint[0] - new_asset->bounds.halfSize[0];
				//	new_tome->m_boundsMin.y = new_asset->bounds.midPoint[1] - new_asset->bounds.halfSize[1];
				//	new_tome->m_boundsMin.z = new_asset->bounds.midPoint[2] - new_asset->bounds.halfSize[2];
				//	new_tome->m_boundsMax.x = new_asset->bounds.midPoint[0] + new_asset->bounds.halfSize[0];
				//	new_tome->m_boundsMax.y = new_asset->bounds.midPoint[1] + new_asset->bounds.halfSize[1];
				//	new_tome->m_boundsMax.z = new_asset->bounds.midPoint[2] + new_asset->bounds.halfSize[2];
				//
				//	new_tome->m_clusterCoordScale = 8.0f;
				//
				//	// fix offsets in tome data
				//	auto fix_offset = [&](std::uint32_t& offset)
				//	{
				//		if (offset > sizeof(zonetool::h1::Umbra::ImpTome))
				//			offset += extra_data_size;
				//	};
				//	fix_offset(new_tome->m_tileTree.m_treeData.m_offset);
				//	fix_offset(new_tome->m_tileTree.m_map.m_offset);
				//	fix_offset(new_tome->m_tileTree.m_splitValues.m_offset);
				//	fix_offset(new_tome->m_objBounds.m_offset);
				//	fix_offset(new_tome->m_objDistances.m_offset);
				//	fix_offset(new_tome->m_userIDStarts.m_offset);
				//	fix_offset(new_tome->m_userIDs.m_offset);
				//	fix_offset(new_tome->m_objectLists.m_offset);
				//	fix_offset(new_tome->m_clusterLists.m_offset);
				//	fix_offset(new_tome->m_gateIndexMap.m_offset);
				//	fix_offset(new_tome->m_gateVertices.m_offset);
				//	fix_offset(new_tome->m_gateIndices.m_offset);
				//	fix_offset(new_tome->m_clusters.m_offset);
				//	fix_offset(new_tome->m_clusterPortals.m_offset);
				//	fix_offset(new_tome->m_cellStarts.m_offset);
				//	fix_offset(new_tome->m_slotPaths.m_offset);
				//	fix_offset(new_tome->m_tileLodLevels.m_offset);
				//	fix_offset(new_tome->m_tiles.m_offset);
				//	fix_offset(new_tome->m_tileMatchingData.m_offset);
				//	fix_offset(new_tome->m_matchingTrees.m_offset);
				//	fix_offset(new_tome->m_tomeClusterStarts.m_offset);
				//	fix_offset(new_tome->m_tomeClusterPortalStarts.m_offset);
				//	fix_offset(new_tome->m_objectDepthmaps.m_offset);
				//	fix_offset(new_tome->m_depthmapFaces.m_offset);
				//	fix_offset(new_tome->m_depthmapPalettes.m_offset);
				//	fix_offset(new_tome->m_tilePortalExpands.m_offset);
				//
				//	auto fix_tree = [&](auto&& self, std::uint32_t offset) -> void
				//	{
				//		auto* tree = reinterpret_cast<zonetool::iw7::Umbra::ImpTile*>(new_asset->umbraTomeData + offset);
				//
				//		fix_offset(tree->m_viewTree.m_treeData.m_offset);
				//		fix_offset(tree->m_viewTree.m_map.m_offset);
				//		fix_offset(tree->m_viewTree.m_splitValues.m_offset);
				//		fix_offset(tree->m_cells.m_offset);
				//		fix_offset(tree->m_portals.m_offset);
				//		fix_offset(tree->m_cellIndices.m_offset);
				//		fix_offset(tree->u_.bsp.m_planesOffset); // let's hope coord system isn't used...
				//
				//		if (tree->m_viewTree.m_treeData.m_offset)
				//			self(self, tree->m_viewTree.m_treeData.m_offset);
				//	};
				//	if (new_tome->m_tileTree.m_treeData.m_offset)
				//		fix_tree(fix_tree, new_tome->m_tileTree.m_treeData.m_offset);
				//
				//	// update data
				//	new_tome->m_size = new_asset->umbraTomeSize;
				//	new_tome->m_crc32 = crc32Hash(&new_tome->m_size, new_tome->m_size - 8);
				//	assert(old_tome->m_crc32 == crc32Hash(&old_tome->m_size, old_tome->m_size - 8));
				//}
				//else
				//{
				//	// Todo...
				//}

				// re-calculate values
				auto AlignUp = [](auto value, auto alignment)
				{
					return (value + (alignment - 1)) & ~(alignment - 1);
				};

				const auto lights = new_asset->primaryLightCount
					- new_asset->lastSunPrimaryLightIndex
					- new_asset->movingScriptablePrimaryLightCount
					- 1;

				new_asset->staticSpotOmniPrimaryLightCountAligned = AlignUp(lights, 32);

				new_asset->primaryLightMotionDetectBitsEntries = new_asset->staticSpotOmniPrimaryLightCountAligned >> 4;;
				new_asset->primaryLightMotionDetectBits = allocator.allocate_array<unsigned int>(new_asset->primaryLightMotionDetectBitsEntries); // runtime

				new_asset->entityMotionBitsEntries = 134; // idk (seems to always be 134)
				new_asset->entityMotionBits = allocator.allocate_array<unsigned int>(new_asset->entityMotionBitsEntries); // runtime

				new_asset->numPrimaryLightEntityShadowVisEntries = new_asset->staticSpotOmniPrimaryLightCountAligned * 0x86;
				new_asset->primaryLightEntityShadowVis = allocator.allocate_array<unsigned int>(new_asset->numPrimaryLightEntityShadowVisEntries); // runtime

				new_asset->dynEntMotionBitsEntries[0] =
					((new_asset->dpvsDyn.dynEntClientCount[0] + 31) >> 5) * 2;
				new_asset->dynEntMotionBits[0] = allocator.allocate_array<unsigned int>(new_asset->dynEntMotionBitsEntries[0]); // runtime
				new_asset->dynEntMotionBitsEntries[1] =
					((new_asset->dpvsDyn.dynEntClientCount[1] + 31) >> 5) * 2;
				new_asset->dynEntMotionBits[1] = allocator.allocate_array<unsigned int>(new_asset->dynEntMotionBitsEntries[1]); // runtime

				new_asset->numPrimaryLightDynEntShadowVisEntries[0] =
					(new_asset->staticSpotOmniPrimaryLightCountAligned * new_asset->dpvsDyn.dynEntClientCount[0]) >> 4;
				new_asset->primaryLightDynEntShadowVis[0] = allocator.allocate_array<unsigned int>(new_asset->numPrimaryLightDynEntShadowVisEntries[0]); // runtime
				new_asset->numPrimaryLightDynEntShadowVisEntries[1] =
					(new_asset->staticSpotOmniPrimaryLightCountAligned * new_asset->dpvsDyn.dynEntClientCount[1]) >> 4;
				new_asset->primaryLightDynEntShadowVis[1] = allocator.allocate_array<unsigned int>(new_asset->numPrimaryLightDynEntShadowVisEntries[1]); // runtime

				return new_asset;
			}

			void dump(GfxWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::iw7::gfx_world::dump(converted_asset);

				for (int i = 0; i < asset->draw.lightmapCount; i++)
				{
					zonetool::iw7::gfx_light_map::dump(converted_asset->draw.lightMaps[i]);
				}

				zonetool::iw7::gfx_world_tr::dump(converted_asset->draw.transientZones[0]);
			}
		}
	}
}
