#include <std_include.hpp>
#include "zonetool/h1/converter/iw7/include.hpp"
#include "xsurface.hpp"

#include "zonetool/iw7/assets/xsurface.hpp"

#include "game/shared.hpp"

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace xsurface
		{
			// packs h1 blend verts (1/3/5/7 shorts per entry, vert idx pre-multiplied by 64)
			// into iw7 layout (2/4/6/8 shorts per entry, vert idx raw, trailing 0).
			// past weights4 are not handled by the iw7->h1 mate we mirror that and skip them
			void PackIW7BlendVerts(const zonetool::h1::XSurface* h1surf, zonetool::iw7::XSurface* iwsurf, utils::memory::allocator& mem)
			{
				const unsigned int iw7_short_count =
					2 * h1surf->blendVertCounts[0]
					+ 4 * h1surf->blendVertCounts[1]
					+ 6 * h1surf->blendVertCounts[2]
					+ 8 * h1surf->blendVertCounts[3];

				if (!iw7_short_count)
				{
					iwsurf->blendVerts = nullptr;
					iwsurf->blendVertSize = 0;
					return;
				}

				iwsurf->blendVerts = reinterpret_cast<zonetool::iw7::XBlendInfo*>(mem.allocate_array<unsigned short>(iw7_short_count));
				iwsurf->blendVertSize = iw7_short_count * sizeof(unsigned short);

				auto* out = reinterpret_cast<unsigned short*>(iwsurf->blendVerts);
				const auto* in = reinterpret_cast<const unsigned short*>(h1surf->blendVerts);

				unsigned int out__ = 0; // iw7
				unsigned int in__ = 0; // h1

				for (unsigned short i = 0; i < h1surf->blendVertCounts[0]; ++i)
				{
					out[out__++] = in[in__++] / 64;
					out[out__++] = 0;
				}

				for (unsigned short i = 0; i < h1surf->blendVertCounts[1]; ++i)
				{
					out[out__++] = in[in__++] / 64;
					out[out__++] = in[in__++] / 64;
					out[out__++] = in[in__++];
					out[out__++] = 0;
				}

				for (unsigned short i = 0; i < h1surf->blendVertCounts[2]; ++i)
				{
					out[out__++] = in[in__++] / 64;
					out[out__++] = in[in__++] / 64;
					out[out__++] = in[in__++];
					out[out__++] = in[in__++] / 64;
					out[out__++] = in[in__++];
					out[out__++] = 0;
				}

				for (unsigned short i = 0; i < h1surf->blendVertCounts[3]; ++i)
				{
					out[out__++] = in[in__++] / 64;
					out[out__++] = in[in__++] / 64;
					out[out__++] = in[in__++];
					out[out__++] = in[in__++] / 64;
					out[out__++] = in[in__++];
					out[out__++] = in[in__++] / 64;
					out[out__++] = in[in__++];
					out[out__++] = 0;
				}

				assert(out__ == iw7_short_count);
			}

			zonetool::iw7::XModelSurfs* convert(zonetool::h1::XModelSurfs* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::iw7::XModelSurfs>();

				REINTERPRET_CAST_SAFE(name);

				COPY_VALUE(numsurfs);

				new_asset->surfs = allocator.allocate_array<zonetool::iw7::XSurface>(asset->numsurfs);
				for (unsigned short i = 0; i < asset->numsurfs; i++)
				{
					auto* new_surf = &new_asset->surfs[i];
					auto* surf = &asset->surfs[i];

					unsigned short remapped_flags = surf->flags & 0x1Fu; // VERTCOL_GREY/NONE, SKINNED, REACTIVE_MOTION, LIGHTMAP_COORDS
					if (surf->flags & zonetool::h1::SURF_FLAG_TENSION)
					{
						remapped_flags |= zonetool::iw7::SURF_FLAG_TENSION; // 0x40
					}
					remapped_flags |= zonetool::iw7::SURF_FLAG_SELF_VISIBILITY; // 0x80
					new_surf->flags = remapped_flags;

					COPY_VALUE(surfs[i].vertCount);
					COPY_VALUE(surfs[i].triCount);
					COPY_VALUE(surfs[i].rigidVertListCount);
					COPY_VALUE(surfs[i].subdivLevelCount);

					for (auto j = 0; j < 8; j++)
					{
						new_surf->blendVertCounts[j] = static_cast<unsigned short>(surf->blendVertCounts[j]);
					}

					if ((surf->flags & zonetool::h1::SURF_FLAG_REACTIVE_MOTION) != 0)
					{
						// h1::GfxPackedMotionVertex == iw7::GfxPackedMotionVertex (no selfVisibility on this path)
						REINTERPRET_CAST_SAFE(surfs[i].verts0.packedMotionVerts0);
					}
					else
					{
						new_surf->verts0.packedVerts0 = allocator.allocate_array<zonetool::iw7::GfxPackedVertex>(surf->vertCount);
						for (unsigned short j = 0; j < surf->vertCount; ++j)
						{
							const auto& src = surf->verts0.packedVerts0[j];
							auto& dst = new_surf->verts0.packedVerts0[j];

							dst.xyz[0] = src.xyz[0];
							dst.xyz[1] = src.xyz[1];
							dst.xyz[2] = src.xyz[2];

							dst.color.packed = src.color.packed;
							dst.texCoord.packed = src.texCoord.packed;
							dst.normal.packed = src.normal.packed;
							dst.tangent.packed = src.tangent.packed;

							float default_visibility[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
							dst.selfVisibility.packed = self_visibility::XSurfacePackSelfVisibility(default_visibility);
						}
					}

					REINTERPRET_CAST_SAFE(surfs[i].triIndices);
					REINTERPRET_CAST_SAFE(surfs[i].rigidVertLists);

					PackIW7BlendVerts(surf, new_surf, allocator);

					if (surf->lmapUnwrap)
					{
						new_surf->lmapUnwrap = allocator.allocate_array<float[2]>(surf->vertCount);
						for (int j = 0; j < surf->vertCount; j++)
						{
							new_surf->lmapUnwrap[j][0] = static_cast<float>(surf->lmapUnwrap[j][0]);
							new_surf->lmapUnwrap[j][1] = static_cast<float>(surf->lmapUnwrap[j][1]);
						}
					}
					else
					{
						new_surf->lmapUnwrap = nullptr;
					}

					REINTERPRET_CAST_SAFE(surfs[i].subdiv);
					REINTERPRET_CAST_SAFE(surfs[i].tensionData);
					REINTERPRET_CAST_SAFE(surfs[i].tensionAccumTable);

					new_surf->unk = 0; // vertexLightingIndex?
					new_surf->pad = 0;
					std::memset(&new_surf->shProbeSimplexVertData, 0, sizeof(new_surf->shProbeSimplexVertData));

					COPY_ARR(surfs[i].partBits);
				}

				COPY_ARR(partBits);

				return new_asset;
			}

			void dump(zonetool::h1::XModelSurfs* asset)
			{
				utils::memory::allocator allocator;
				auto* converted_asset = convert(asset, allocator);
				zonetool::iw7::xsurface::dump(converted_asset);
			}
		}
	}
}
