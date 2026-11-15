#include <std_include.hpp>
#include "xsurface.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_shared(zone_reader& reader, XSurfaceShared*& field)
		{
			const auto shared = reader.read_single(field, 7);
			if (shared && !(shared->flags & XSURFACE_SHARED_STREAMED))
			{
				reader.read_array(shared->data.residentData, 15, shared->dataSize);
			}
		}

		void read_subdiv_level(zone_reader& reader, XSurfaceSubdivLevel* level, const XSurface* surface)
		{
			reader.read_array(level->rigidVertLists, 3, surface->rigidVertListCount);
			reader.read_array(level->faceIndices, 3, 6 * level->faceCount);
			reader.read_array(level->regularPatchIndices, 3, 16 * level->regularPatchCount);
			reader.read_array(level->regularPatchFlags, 3, level->regularPatchCount);
			reader.read_array(level->facePoints, 3, level->facePointBufferSize / sizeof(unsigned int));
			reader.read_array(level->edgePoints, 3, 2 * level->edgePointCount);
			reader.read_array(level->vertexPoints, 3, level->vertexPointBufferSize / sizeof(unsigned int));
			reader.read_array(level->normals, 3, level->normalBufferSize / sizeof(unsigned int));
			reader.read_array(level->transitionPoints, 3, level->transitionPointCount);
			reader.read_array(level->regularPatchCones, 3, 8 * level->regularPatchCount);
		}

		void read_subdiv(zone_reader& reader, XSurface* surface)
		{
			const auto subdiv = reader.read_single(surface->subdiv, 15);
			if (!subdiv)
			{
				return;
			}

			if (const auto levels = reader.read_array(subdiv->levels, 7, surface->subdivLevelCount))
			{
				for (auto i = 0; i < surface->subdivLevelCount; i++)
				{
					read_subdiv_level(reader, &levels[i], surface);
				}
			}
		}

		void read_blend_shapes(zone_reader& reader, XSurface* surface)
		{
			if (const auto per_vert = reader.read_single(surface->blendShapesPerVert, 15))
			{
				reader.read_array(per_vert->perVertBlendShapeData, 7, per_vert->perVertBlendShapeDataSize / sizeof(unsigned __int64));
				reader.read_array(per_vert->surfBlendShapesData, 1, per_vert->surfBlendShapeCount);
			}

			if (const auto tangent_frame = reader.read_single(surface->blendShapesRecalcTangentFrameData, 15))
			{
				reader.read_array(tangent_frame->topologicalData, 1, tangent_frame->bufferSize / sizeof(unsigned short));
			}
		}

		void read_surface(zone_reader& reader, XSurface* surface)
		{
			read_shared(reader, surface->shared);
			reader.read_array(surface->lmapCoords, 3, surface->vertCount);
			reader.read_array(surface->rigidVertLists, 1, surface->rigidVertListCount);
			reader.read_array(surface->blendVerts, 3, surface->blendVertSize / sizeof(unsigned short));
			read_subdiv(reader, surface);
			reader.read_array(surface->childBounds, 3, surface->rigidVertListCount);
			read_blend_shapes(reader, surface);
		}
	}

	void xmodel_surfs::read(zone_reader& reader, XModelSurfs* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto surfaces = reader.read_array(asset->surfs, 15, asset->numsurfs))
		{
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				read_surface(reader, &surfaces[i]);
			}
		}

		read_shared(reader, asset->shared);
		reader.pop_stream();
	}
}
