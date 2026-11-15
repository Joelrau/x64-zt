#include <std_include.hpp>

#include "../include.hpp"
#include "xsurface.hpp"

#include <iw7/assets/xsurface.hpp>
#include <iw8/xpak.hpp>

#include <shared.hpp>

namespace zonetool::iw8::converter::iw7::xsurface
{
	namespace
	{
		namespace target = zonetool::iw7;

		std::optional<std::string> read_shared_data(const XModelSurfs* asset)
		{
			const auto shared = asset->shared;
			if (!shared || !shared->dataSize)
			{
				return {};
			}

			if (shared->flags & XSURFACE_SHARED_STREAMED)
			{
				return xpak::read(asset->xpakEntry.key, shared->dataSize);
			}

			return std::string(shared->data.residentData, shared->dataSize);
		}

		template <typename T>
		const T* data_at(const std::string& data, const unsigned int offset, const std::size_t count)
		{
			if (offset == 0xFFFFFFFF || offset > data.size() || count * sizeof(T) > data.size() - offset)
			{
				return nullptr;
			}

			return reinterpret_cast<const T*>(data.data() + offset);
		}

		std::array<float, 3> unpack_position(const std::uint64_t packed, const Bounds& bounds)
		{
			const auto scale = std::max({ bounds.halfSize[0], bounds.halfSize[1], bounds.halfSize[2] });

			std::array<float, 3> position{};
			for (auto i = 0; i < 3; i++)
			{
				const auto value = static_cast<float>((packed >> (21 * i)) & 0x1FFFFF);
				position[i] = (value / 2097151.0f * 2.0f - 1.0f) * scale + bounds.midPoint[i];
			}

			return position;
		}

		template <typename T>
		T* copy_data(const std::string& data, const unsigned int offset, const std::size_t count, utils::memory::allocator& allocator)
		{
			const auto source = data_at<T>(data, offset, count);
			if (!source || !count)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<T>(count);
			std::memcpy(result, source, count * sizeof(T));
			return result;
		}

		template <typename T>
		T* copy_array(const T* source, const std::size_t count, utils::memory::allocator& allocator)
		{
			if (!source || !count)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<T>(count);
			std::memcpy(result, source, count * sizeof(T));
			return result;
		}

		unsigned int get_blend_vert_count(const XSurface& surface)
		{
			unsigned int count{};
			for (const auto value : surface.blendVertCounts)
			{
				count += value;
			}

			return count;
		}

		float (*convert_lmap_coords(const XSurface& surface, utils::memory::allocator& allocator))[2]
		{
			if (!(surface.flags & SURF_FLAG_LIGHTMAP_COORDS) || !surface.lmapCoords)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<float[2]>(surface.vertCount);
			for (auto i = 0; i < surface.vertCount; i++)
			{
				const auto packed = surface.lmapCoords[i].packed;
				result[i][0] = static_cast<float>(packed & 0xFFFF) / 65535.0f;
				result[i][1] = static_cast<float>(packed >> 16) / 65535.0f;
			}

			return result;
		}

		target::XSurfaceSubdivInfo* convert_subdiv(const XSurface& surface, utils::memory::allocator& allocator)
		{
			if (!(surface.flags & SURF_FLAG_SUBDIV) || !surface.subdiv || !surface.subdiv->levels || !surface.subdivLevelCount)
			{
				return nullptr;
			}

			const auto result = allocator.allocate<target::XSurfaceSubdivInfo>();
			result->flags = surface.subdiv->flags;
			result->cache.size = surface.subdiv->cache.size;
			result->levels = allocator.allocate_array<target::XSurfaceSubdivLevel>(surface.subdivLevelCount);

			for (auto i = 0; i < surface.subdivLevelCount; i++)
			{
				const auto& level = surface.subdiv->levels[i];
				auto& new_level = result->levels[i];

				new_level.rigidVertLists = reinterpret_cast<target::XSubdivRigidVertList*>(copy_array(level.rigidVertLists, surface.rigidVertListCount, allocator));
				new_level.faceCount = level.faceCount;
				new_level.regularPatchCount = level.regularPatchCount;
				new_level.regularPatchOffset = level.regularPatchOffset;
				new_level.facePointCount = level.facePointCount;
				new_level.facePointValence4Count = level.facePointValence4Count;
				new_level.facePointBufferSize = level.facePointBufferSize;
				new_level.edgePointCount = level.edgePointCount;
				new_level.edgePointSmoothEnd = level.edgePointSmoothEnd;
				new_level.edgePointUVBorderEnd = level.edgePointUVBorderEnd;
				new_level.vertexPointCount = level.vertexPointCount;
				new_level.vertexPointValence4Count = level.vertexPointValence4Count;
				new_level.vertexPointBufferSize = level.vertexPointBufferSize;
				new_level.normalCount = level.normalCount;
				new_level.normalBufferSize = level.normalBufferSize;
				new_level.transitionPointCount = level.transitionPointCount;
				new_level.vertCount = level.vertCount;
				new_level.vertOffset = level.vertOffset;
				new_level.faceIndices = copy_array(level.faceIndices, 6 * level.faceCount, allocator);
				new_level.regularPatchIndices = copy_array(level.regularPatchIndices, 16 * level.regularPatchCount, allocator);
				new_level.regularPatchFlags = copy_array(level.regularPatchFlags, level.regularPatchCount, allocator);
				new_level.facePoints = copy_array(level.facePoints, level.facePointBufferSize / sizeof(unsigned int), allocator);
				new_level.edgePoints = copy_array(level.edgePoints, 2 * level.edgePointCount, allocator);
				new_level.vertexPoints = copy_array(level.vertexPoints, level.vertexPointBufferSize / sizeof(unsigned int), allocator);
				new_level.normals = copy_array(level.normals, level.normalBufferSize / sizeof(unsigned int), allocator);
				new_level.transitionPoints = copy_array(level.transitionPoints, level.transitionPointCount, allocator);
				new_level.regularPatchCones = copy_array(level.regularPatchCones, 8 * level.regularPatchCount, allocator);
			}

			return result;
		}

		void convert_tension(const XSurface& surface, const std::string& data, target::XSurface* result, utils::memory::allocator& allocator)
		{
			if (!(surface.flags & SURF_FLAG_TENSION))
			{
				return;
			}

			result->tensionData = copy_data<float>(data, surface.sharedTensionDataOffset, get_blend_vert_count(surface), allocator);
			result->tensionAccumTable = copy_data<unsigned short>(data, surface.sharedTensionAccumTableOffset, 16 * surface.vertCount, allocator);
		}

		std::uint16_t convert_flags(const target::XSurface& surface)
		{
			std::uint16_t result = target::SURF_FLAG_SELF_VISIBILITY;
			if (surface.blendVertSize)
			{
				result |= target::SURF_FLAG_SKINNED;
			}

			if (surface.lmapUnwrap)
			{
				result |= target::SURF_FLAG_LIGHTMAP_COORDS;
			}

			if (surface.subdiv)
			{
				result |= target::SURF_FLAG_SUBDIV;
			}

			if (surface.tensionData && surface.tensionAccumTable)
			{
				result |= target::SURF_FLAG_TENSION;
			}

			return result;
		}

		target::GfxPackedVertex convert_vertex(const GfxPackedVertex& vertex, const Bounds& bounds, const std::uint32_t* color)
		{
			target::GfxPackedVertex result{};

			const auto position = unpack_position(vertex.xyz, bounds);
			std::memcpy(result.xyz, position.data(), sizeof(result.xyz));

			float default_visibility[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
			result.selfVisibility.packed = self_visibility::XSurfacePackSelfVisibility(default_visibility);

			result.color.packed = color ? *color : 0xFFFFFFFF;
			result.texCoord.packed = vertex.texCoord;

			const auto frame = unpack_tangent_frame(vertex.tangentFrame);
			result.normal.packed = pack_unit_vec(frame.normal, 3);
			result.tangent.packed = pack_unit_vec(frame.tangent, frame.binormal_sign < 0.0f ? 3 : 0);

			return result;
		}

		target::XRigidVertList* convert_rigid_vert_lists(const XSurface& surface, utils::memory::allocator& allocator)
		{
			if (!surface.rigidVertLists || !surface.rigidVertListCount)
			{
				return nullptr;
			}

			const auto lists = allocator.allocate_array<target::XRigidVertList>(surface.rigidVertListCount);
			for (auto i = 0; i < surface.rigidVertListCount; i++)
			{
				const auto& list = surface.rigidVertLists[i];
				lists[i].boneOffsetIndex = list.boneIndexOffset;
				lists[i].vertCount = list.vertCount;
				lists[i].triOffset = list.triOffset;
				lists[i].triCount = list.triCount;
				lists[i].collisionTree = nullptr;
			}

			return lists;
		}

		bool convert_surface(const XSurface& surface, const std::string& data, target::XSurface* result, utils::memory::allocator& allocator)
		{
			const auto vertices = data_at<GfxPackedVertex>(data, surface.sharedVertDataOffset, surface.vertCount);
			const auto indices = data_at<Face>(data, surface.sharedIndexDataOffset, surface.triCount);
			if (!vertices || !indices)
			{
				return false;
			}

			const auto colors = (surface.flags & SURF_FLAG_VERTCOL) ? data_at<std::uint32_t>(data, surface.sharedColorDataOffset, surface.vertCount) : nullptr;

			result->vertCount = surface.vertCount;
			result->triCount = surface.triCount;
			result->rigidVertListCount = surface.rigidVertListCount;
			std::memcpy(result->blendVertCounts, surface.blendVertCounts, sizeof(result->blendVertCounts));

			result->verts0.packedVerts0 = allocator.allocate_array<target::GfxPackedVertex>(surface.vertCount);
			for (auto i = 0; i < surface.vertCount; i++)
			{
				result->verts0.packedVerts0[i] = convert_vertex(vertices[i], surface.surfBounds, colors ? &colors[i] : nullptr);
			}

			result->triIndices = allocator.allocate_array<target::Face>(surface.triCount);
			std::memcpy(result->triIndices, indices, sizeof(target::Face) * surface.triCount);

			result->rigidVertLists = convert_rigid_vert_lists(surface, allocator);

			if (surface.blendVerts && surface.blendVertSize)
			{
				result->blendVertSize = surface.blendVertSize;
				result->blendVerts = allocator.allocate_array<target::XBlendInfo>(surface.blendVertSize / sizeof(target::XBlendInfo));
				std::memcpy(result->blendVerts, surface.blendVerts, surface.blendVertSize);
			}

			result->lmapUnwrap = convert_lmap_coords(surface, allocator);
			result->subdiv = convert_subdiv(surface, allocator);
			result->subdivLevelCount = result->subdiv ? surface.subdivLevelCount : 0;
			convert_tension(surface, data, result, allocator);
			result->flags = convert_flags(*result);

			std::memcpy(result->partBits, surface.partBits.array, sizeof(result->partBits));
			return true;
		}

		target::XModelSurfs* convert(const XModelSurfs* asset, utils::memory::allocator& allocator)
		{
			const auto data = read_shared_data(asset);
			if (!data)
			{
				ZONETOOL_ERROR("Cannot read the vertex data of xmodelsurfs \"%s\"", asset->name);
				return nullptr;
			}

			const auto result = allocator.allocate<target::XModelSurfs>();
			result->name = asset->name;
			result->numsurfs = asset->numsurfs;
			result->surfs = allocator.allocate_array<target::XSurface>(asset->numsurfs);
			std::memcpy(result->partBits, asset->partBits.array, sizeof(result->partBits));

			for (auto i = 0; i < asset->numsurfs; i++)
			{
				if (!convert_surface(asset->surfs[i], *data, &result->surfs[i], allocator))
				{
					ZONETOOL_ERROR("Surface %d of xmodelsurfs \"%s\" points outside its vertex data", i, asset->name);
					return nullptr;
				}
			}

			return result;
		}
	}

	tangent_frame unpack_tangent_frame(const std::uint32_t packed)
	{
		const std::array<float, 3> remaining
		{
			(static_cast<float>(packed & 0x3FF) * (2.0f / 1023.0f) - 1.0f) * 0.707107f,
			(static_cast<float>((packed >> 10) & 0x3FF) * (2.0f / 1023.0f) - 1.0f) * 0.707107f,
			(static_cast<float>((packed >> 20) & 0x1FF) * (2.0f / 511.0f) - 1.0f) * 0.707107f,
		};

		const auto remaining_length = remaining[0] * remaining[0] + remaining[1] * remaining[1] + remaining[2] * remaining[2];
		const auto largest_index = packed >> 30;

		std::array<float, 4> q{};
		for (auto i = 0u, j = 0u; i < q.size(); i++)
		{
			q[i] = i == largest_index ? std::sqrt(1.0f - std::min(remaining_length, 1.0f)) : remaining[j++];
		}

		tangent_frame frame{};
		frame.normal = { 2.0f * (q[1] * q[3] + q[0] * q[2]), 2.0f * (q[1] * q[2] - q[0] * q[3]), 1.0f - 2.0f * (q[0] * q[0] + q[1] * q[1]) };
		frame.tangent = { 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]), 2.0f * (q[2] * q[3] + q[0] * q[1]), 2.0f * (q[0] * q[2] - q[1] * q[3]) };
		frame.binormal_sign = (packed >> 29) & 1 ? -1.0f : 1.0f;
		return frame;
	}

	std::uint32_t pack_unit_vec(const std::array<float, 3>& value, const std::uint32_t alpha)
	{
		const auto pack_component = [](const float component)
		{
			return static_cast<std::uint32_t>(std::floor((std::clamp(component, -1.0f, 1.0f) + 1.0f) * 0.5f * 1023.0f + 0.5f));
		};

		return pack_component(value[0]) | (pack_component(value[1]) << 10) | (pack_component(value[2]) << 20) | (alpha << 30);
	}

	void dump(XModelSurfs* asset)
	{
		if (!mark_dumped(ASSET_TYPE_XMODEL_SURFS, asset->name))
		{
			return;
		}

		utils::memory::allocator allocator;
		if (const auto converted = convert(asset, allocator))
		{
			zonetool::iw7::xsurface::dump(converted);
		}
	}
}
