#include <std_include.hpp>

#include "../include.hpp"
#include "xsurface.hpp"

#include <iw8/assets/xsurface.hpp>
#include <s4/xpak.hpp>

#include <shared.hpp>
#include <utils/cryptography.hpp>

namespace zonetool::s4::converter::iw8::xsurface
{
	namespace
	{
		namespace target = zonetool::iw8;

		std::vector<const XModelSurfs*> queued_surfs;
		std::unordered_map<std::string, std::vector<bool>> surface_layer_masks;

		std::optional<std::string> read_shared_data(const XModelSurfs* asset)
		{
			const auto shared = asset->shared;
			if (!shared || !shared->dataSize)
			{
				return {};
			}

			if (shared->flags & XSURFACE_SHARED_STREAMED)
			{
				return xpak::read(asset->xpakKey, shared->dataSize);
			}

			if (!shared->data.residentData)
			{
				return {};
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

		template <typename T>
		unsigned int append(std::string& data, const T* values, const std::size_t count)
		{
			const auto offset = static_cast<unsigned int>(data.size());
			data.append(reinterpret_cast<const char*>(values), count * sizeof(T));
			return offset;
		}

		bool has_bounds_position_scale(const XSurface& surface)
		{
			return surface.xyzScale == -1.0f;
		}

		float get_bounds_scale(const Bounds& bounds)
		{
			return std::max({ bounds.halfSize[0], bounds.halfSize[1], bounds.halfSize[2] });
		}

		std::array<float, 3> unpack_position(const std::uint64_t packed, const XSurface& surface)
		{
			const auto& bounds = surface.surfBounds;
			const auto bounds_scale = has_bounds_position_scale(surface);
			const auto scale = bounds_scale ? get_bounds_scale(bounds) : surface.xyzScale;

			std::array<float, 3> position{};
			for (auto i = 0; i < 3; i++)
			{
				const auto value = static_cast<float>((packed >> (21 * i)) & 0x1FFFFF);
				position[i] = (value / 2097151.0f * 2.0f - 1.0f) * scale + (bounds_scale ? bounds.midPoint[i] : 0.0f);
			}

			return position;
		}

		std::uint64_t pack_position(const std::array<float, 3>& position, const Bounds& bounds, const std::uint64_t source)
		{
			const auto scale = get_bounds_scale(bounds);

			auto result = source & ~((1ull << 63) - 1);
			for (auto i = 0; i < 3; i++)
			{
				const auto normalized = scale > 0.0f ? (position[i] - bounds.midPoint[i]) / scale : 0.0f;
				const auto value = std::clamp(std::round((normalized + 1.0f) * 0.5f * 2097151.0f), 0.0f, 2097151.0f);
				result |= static_cast<std::uint64_t>(value) << (21 * i);
			}

			return result;
		}

		std::uint64_t convert_position(const std::uint64_t packed, const XSurface& surface)
		{
			if (has_bounds_position_scale(surface))
			{
				return packed;
			}

			return pack_position(unpack_position(packed, surface), surface.surfBounds, packed);
		}

		std::uint16_t half_round_up(const float value)
		{
			const auto half = half_float::float_to_half(value);
			if (half_float::half_to_float(half) >= value)
			{
				return half;
			}

			return static_cast<std::uint16_t>(half + 1);
		}

		target::XSurfaceTriCluster convert_tri_cluster(const XSurfaceTriCluster& cluster, const XSurface& surface)
		{
			const auto mins = unpack_position(cluster.mins, surface);
			const auto maxs = unpack_position(cluster.maxs, surface);

			target::XSurfaceTriCluster result{};
			for (auto i = 0; i < 3; i++)
			{
				result.midPoint[i] = half_float::float_to_half((mins[i] + maxs[i]) * 0.5f);
				const auto mid_point = half_float::half_to_float(result.midPoint[i]);
				result.halfSize[i] = half_round_up(std::max(maxs[i] - mid_point, mid_point - mins[i]));
			}

			std::memcpy(result.cone, cluster.cone, sizeof(result.cone));
			return result;
		}

		std::uint16_t convert_flags(const XSurface& surface)
		{
			std::uint16_t result = target::SURF_FLAG_SELF_VISIBILITY;
			result |= surface.flags & (SURF_FLAG_VERTCOL | SURF_FLAG_SKINNED | SURF_FLAG_REACTIVE_MOTION | SURF_FLAG_LIGHTMAP_COORDS);
			if (surface.flags & SURF_FLAG_SECONDUV)
			{
				result |= target::SURF_FLAG_SECONDUV;
			}

			return result;
		}

		std::uint8_t get_layer_mask(const std::uint32_t color)
		{
			return static_cast<std::uint8_t>(color);
		}

		std::uint8_t get_wind_amplitude(const std::uint32_t color)
		{
			return static_cast<std::uint8_t>(color >> 24);
		}

		bool write_shared_data(const XSurface& surface, const bool layer_mask, const std::string& source, target::XSurface* result, std::string& data)
		{
			const auto indices = data_at<Face>(source, surface.sharedIndexDataOffset, surface.triCount);
			const auto positions = data_at<std::uint64_t>(source, surface.sharedXyzDataOffset, surface.vertCount);
			const auto self_visibility = data_at<std::uint32_t>(source, surface.sharedSelfVisibilityDataOffset, surface.vertCount);
			const auto tex_coords = data_at<std::uint32_t>(source, surface.sharedTexCoordDataOffset, surface.vertCount);
			const auto tangent_frames = data_at<std::uint32_t>(source, surface.sharedTangentFrameDataOffset, surface.vertCount);
			const auto cluster_count = (surface.triCount + 63u) / 64u;
			const auto clusters = data_at<XSurfaceTriCluster>(source, surface.sharedTriClusterDataOffset, cluster_count);
			if (!indices || !positions || !self_visibility || !tex_coords || !tangent_frames || !clusters)
			{
				return false;
			}

			std::vector<target::Face> target_indices(2 * surface.triCount);
			std::memcpy(target_indices.data(), indices, sizeof(target::Face) * surface.triCount);
			result->sharedIndexDataOffset = append(data, target_indices.data(), target_indices.size());

			std::vector<target::GfxPackedVertex> verts(surface.vertCount);
			for (auto i = 0; i < surface.vertCount; i++)
			{
				verts[i].xyz = convert_position(positions[i], surface);
				verts[i].selfVisibility = self_visibility[i];
				verts[i].texCoord = tex_coords[i];
				verts[i].tangentFrame = tangent_frames[i];
			}
			result->sharedVertDataOffset = append(data, verts.data(), verts.size());

			if (const auto colors = data_at<std::uint32_t>(source, surface.sharedColorDataOffset, surface.vertCount))
			{
				std::vector<std::uint32_t> target_colors(surface.vertCount);
				for (auto i = 0; i < surface.vertCount; i++)
				{
					target_colors[i] = convert_vertex_color(layer_mask ? get_layer_mask(colors[i]) : get_wind_amplitude(colors[i]));
				}
				result->sharedColorDataOffset = append(data, target_colors.data(), target_colors.size());
			}

			if (const auto second_uvs = data_at<std::uint32_t>(source, surface.sharedSecondUVDataOffset, surface.vertCount))
			{
				result->sharedSecondUVDataOffset = append(data, second_uvs, surface.vertCount);
			}

			if (const auto normal_transforms = data_at<std::uint32_t>(source, surface.sharedNormalTransformDataOffset, surface.vertCount))
			{
				result->sharedNormalTransformDataOffset = append(data, normal_transforms, surface.vertCount);
			}

			std::vector<target::XSurfaceTriCluster> target_clusters(cluster_count);
			for (auto i = 0u; i < cluster_count; i++)
			{
				target_clusters[i] = convert_tri_cluster(clusters[i], surface);
			}
			result->sharedTriClusterDataOffset = append(data, target_clusters.data(), target_clusters.size());

			const auto surface_data = data.data() + result->sharedIndexDataOffset;
			result->hash = utils::cryptography::crc32::compute(reinterpret_cast<const std::uint8_t*>(surface_data),
				data.size() - result->sharedIndexDataOffset);

			data.resize((data.size() + 15) & ~15ull);
			return true;
		}

		template <typename T>
		T* copy_array(const T* source, const std::size_t count, utils::memory::allocator& allocator)
		{
			if (!source || !count)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<T>(count);
			std::memcpy(result, source, sizeof(T) * count);
			return result;
		}

		target::Bounds* convert_child_bounds(const XSurface& surface, utils::memory::allocator& allocator)
		{
			if (!surface.childBounds || !surface.rigidVertListCount)
			{
				return nullptr;
			}

			const auto bounds = allocator.allocate_array<target::Bounds>(surface.rigidVertListCount);
			for (auto i = 0; i < surface.rigidVertListCount; i++)
			{
				for (auto j = 0; j < 3; j++)
				{
					bounds[i].midPoint[j] = half_float::half_to_float(surface.childBounds[i].midPoint[j]);
					bounds[i].halfSize[j] = half_float::half_to_float(surface.childBounds[i].halfSize[j]);
				}
			}

			return bounds;
		}

		bool convert_surface(const XSurface& surface, const bool layer_mask, const std::string& source, target::XSurface* result, std::string& data, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(target::XRigidVertList) == sizeof(XRigidVertList));
			static_assert(sizeof(target::PackedLmapCoords) == sizeof(PackedLmapCoords));

			result->flags = convert_flags(surface);
			result->vertCount = surface.vertCount;
			result->triCount = surface.triCount;
			result->rigidVertListCount = surface.rigidVertListCount;
			std::memcpy(result->blendVertCounts, surface.blendVertCounts, sizeof(result->blendVertCounts));
			result->blendVertSize = surface.blendVerts ? surface.blendVertSize : 0;
			result->blendVerts = copy_array(surface.blendVerts, surface.blendVertSize / sizeof(unsigned short), allocator);
			result->rigidVertLists = reinterpret_cast<target::XRigidVertList*>(copy_array(surface.rigidVertLists, surface.rigidVertListCount, allocator));
			result->lmapCoords = reinterpret_cast<target::PackedLmapCoords*>(copy_array(surface.lmapCoords, surface.vertCount, allocator));
			result->childBounds = convert_child_bounds(surface, allocator);
			std::memcpy(result->partBits.array, surface.partBits.array, sizeof(result->partBits.array));
			std::memcpy(&result->surfBounds, &surface.surfBounds, sizeof(result->surfBounds));

			result->sharedVertDataOffset = 0xFFFFFFFF;
			result->sharedIndexDataOffset = 0xFFFFFFFF;
			result->sharedTriClusterDataOffset = 0xFFFFFFFF;
			result->sharedColorDataOffset = 0xFFFFFFFF;
			result->sharedSecondUVDataOffset = 0xFFFFFFFF;
			result->sharedNormalTransformDataOffset = 0xFFFFFFFF;
			result->sharedTensionAccumTableOffset = 0xFFFFFFFF;
			result->sharedTensionDataOffset = 0xFFFFFFFF;
			return write_shared_data(surface, layer_mask, source, result, data);
		}
	}

	std::uint32_t convert_vertex_color(const std::uint8_t alpha)
	{
		return 0x00FFFFFFu | static_cast<std::uint32_t>(alpha) << 24;
	}

	target::XModelSurfs* convert(const XModelSurfs* asset, const std::vector<bool>& layer_masks, std::string& data, utils::memory::allocator& allocator)
	{
		const auto source = read_shared_data(asset);
		if (!source)
		{
			ZONETOOL_ERROR("Cannot read the vertex data of xmodelsurfs \"%s\"", asset->name);
			return nullptr;
		}

		const auto result = allocator.allocate<target::XModelSurfs>();
		result->name = asset->name;
		result->numsurfs = asset->numsurfs;
		result->surfs = allocator.allocate_array<target::XSurface>(asset->numsurfs);
		std::memcpy(result->partBits.array, asset->partBits.array, sizeof(result->partBits.array));

		for (auto i = 0; i < asset->numsurfs; i++)
		{
			const auto& surface = asset->surfs[i];
			if (surface.flags & SURF_FLAG_SUBDIV)
			{
				ZONETOOL_WARNING("Surface %d of xmodelsurfs \"%s\" loses its subdivision data", i, asset->name);
			}

			const auto layer_mask = i < static_cast<int>(layer_masks.size()) && layer_masks[i];
			if (!convert_surface(surface, layer_mask, *source, &result->surfs[i], data, allocator))
			{
				ZONETOOL_ERROR("Surface %d of xmodelsurfs \"%s\" has no vertex data", i, asset->name);
				return nullptr;
			}
		}

		result->shared = allocator.allocate<target::XSurfaceShared>();
		result->shared->data.residentData = data.data();
		result->shared->dataSize = static_cast<unsigned int>(data.size());
		for (auto i = 0; i < asset->numsurfs; i++)
		{
			result->surfs[i].shared = result->shared;
		}

		return result;
	}

	void set_layer_masks(const XModelSurfs* asset, std::vector<bool> layer_masks)
	{
		const auto [entry, added] = surface_layer_masks.try_emplace(asset->name, std::move(layer_masks));
		if (!added && entry->second != layer_masks)
		{
			ZONETOOL_WARNING("xmodelsurfs \"%s\" is used with different vertex layer masks, the first model wins", asset->name);
		}
	}

	void queue(const XModelSurfs* asset)
	{
		queued_surfs.emplace_back(asset);
	}

	void dump_queued()
	{
		for (const auto* asset : queued_surfs)
		{
			utils::memory::allocator allocator;
			std::string data;
			const auto layer_masks = surface_layer_masks.find(asset->name);
			if (const auto converted = convert(asset, layer_masks == surface_layer_masks.end() ? std::vector<bool>() : layer_masks->second, data, allocator))
			{
				target::xmodel_surfs::dump(converted);
				add_csv_row(target::type_to_string(target::ASSET_TYPE_XMODEL_SURFS), asset->name);
			}
		}

		queued_surfs.clear();
		surface_layer_masks.clear();
	}
}
