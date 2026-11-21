#include <std_include.hpp>

#include "../include.hpp"
#include "xsurface.hpp"

#include <iw8/assets/xsurface.hpp>

#include <shared.hpp>
#include <utils/cryptography.hpp>

namespace zonetool::iw7::converter::iw8::xsurface
{
	namespace
	{
		namespace target = zonetool::iw8;
	}

	vec3 unpack_unit_vec(const std::uint32_t packed)
	{
		vec3 result{};
		for (auto i = 0; i < 3; i++)
		{
			result[i] = static_cast<float>((packed >> (10 * i)) & 0x3FF) / 1023.0f * 2.0f - 1.0f;
		}

		return result;
	}

	namespace
	{
		float dot(const vec3& a, const vec3& b)
		{
			return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
		}

		vec3 cross(const vec3& a, const vec3& b)
		{
			return { a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0] };
		}

		vec3 normalize(const vec3& v, const vec3& fallback)
		{
			const auto length = std::sqrt(dot(v, v));
			if (length < 0.000001f)
			{
				return fallback;
			}

			return { v[0] / length, v[1] / length, v[2] / length };
		}

		vec3 any_perpendicular(const vec3& n)
		{
			return normalize(std::abs(n[0]) < 0.9f ? cross(n, { 1.0f, 0.0f, 0.0f }) : cross(n, { 0.0f, 1.0f, 0.0f }), { 1.0f, 0.0f, 0.0f });
		}

		std::array<float, 4> matrix_to_quat(const vec3& x_axis, const vec3& y_axis, const vec3& z_axis)
		{
			const float m[3][3] =
			{
				{ x_axis[0], y_axis[0], z_axis[0] },
				{ x_axis[1], y_axis[1], z_axis[1] },
				{ x_axis[2], y_axis[2], z_axis[2] },
			};

			std::array<float, 4> q{};
			const auto trace = m[0][0] + m[1][1] + m[2][2];
			if (trace > 0.0f)
			{
				const auto s = 0.5f / std::sqrt(trace + 1.0f);
				q = { (m[2][1] - m[1][2]) * s, (m[0][2] - m[2][0]) * s, (m[1][0] - m[0][1]) * s, 0.25f / s };
			}
			else if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
			{
				const auto s = 2.0f * std::sqrt(1.0f + m[0][0] - m[1][1] - m[2][2]);
				q = { 0.25f * s, (m[0][1] + m[1][0]) / s, (m[0][2] + m[2][0]) / s, (m[2][1] - m[1][2]) / s };
			}
			else if (m[1][1] > m[2][2])
			{
				const auto s = 2.0f * std::sqrt(1.0f + m[1][1] - m[0][0] - m[2][2]);
				q = { (m[0][1] + m[1][0]) / s, 0.25f * s, (m[1][2] + m[2][1]) / s, (m[0][2] - m[2][0]) / s };
			}
			else
			{
				const auto s = 2.0f * std::sqrt(1.0f + m[2][2] - m[0][0] - m[1][1]);
				q = { (m[0][2] + m[2][0]) / s, (m[1][2] + m[2][1]) / s, 0.25f * s, (m[1][0] - m[0][1]) / s };
			}

			const auto length = std::sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
			for (auto& value : q)
			{
				value /= length;
			}

			return q;
		}
	}

	std::uint32_t pack_tangent_frame(const vec3& normal, const vec3& tangent, const bool negative_binormal)
	{
		const auto n = normalize(normal, { 0.0f, 0.0f, 1.0f });
		const auto projected = vec3{ tangent[0] - n[0] * dot(n, tangent), tangent[1] - n[1] * dot(n, tangent), tangent[2] - n[2] * dot(n, tangent) };
		const auto t = normalize(projected, any_perpendicular(n));
		const auto b = cross(n, t);

		auto q = matrix_to_quat(t, b, n);

		auto largest_index = 0u;
		for (auto i = 1u; i < q.size(); i++)
		{
			if (std::abs(q[i]) > std::abs(q[largest_index]))
			{
				largest_index = i;
			}
		}

		if (q[largest_index] < 0.0f)
		{
			for (auto& value : q)
			{
				value = -value;
			}
		}

		const auto pack_component = [](const float value, const float max)
		{
			const auto scaled = (std::clamp(value / 0.707107f, -1.0f, 1.0f) + 1.0f) * 0.5f * max;
			return static_cast<std::uint32_t>(std::floor(scaled + 0.5f));
		};

		std::uint32_t remaining[3]{};
		for (auto i = 0u, j = 0u; i < q.size(); i++)
		{
			if (i != largest_index)
			{
				remaining[j++] = i;
			}
		}

		return pack_component(q[remaining[0]], 1023.0f)
			| (pack_component(q[remaining[1]], 1023.0f) << 10)
			| (pack_component(q[remaining[2]], 511.0f) << 20)
			| (negative_binormal ? 1u << 29 : 0u)
			| (largest_index << 30);
	}

	namespace
	{
		std::uint64_t pack_position(const float* xyz, const target::Bounds& bounds)
		{
			const auto scale = std::max({ bounds.halfSize[0], bounds.halfSize[1], bounds.halfSize[2] });

			std::uint64_t packed{};
			for (auto i = 0; i < 3; i++)
			{
				const auto normalized = scale > 0.0f ? (xyz[i] - bounds.midPoint[i]) / scale : 0.0f;
				const auto value = std::floor((std::clamp(normalized, -1.0f, 1.0f) + 1.0f) * 0.5f * 2097151.0f + 0.5f);
				packed |= static_cast<std::uint64_t>(value) << (21 * i);
			}

			return packed;
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

		target::Bounds compute_bounds(const GfxPackedVertex* verts, const std::size_t count)
		{
			float mins[3]{ FLT_MAX, FLT_MAX, FLT_MAX };
			float maxs[3]{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
			for (auto i = 0u; i < count; i++)
			{
				for (auto j = 0; j < 3; j++)
				{
					mins[j] = std::min(mins[j], verts[i].xyz[j]);
					maxs[j] = std::max(maxs[j], verts[i].xyz[j]);
				}
			}

			target::Bounds bounds{};
			if (count)
			{
				compute(&bounds, mins, maxs);
			}

			return bounds;
		}

		target::XSurfaceTriCluster make_tri_cluster(const XSurface& surface, const unsigned int first_tri, const unsigned int tri_count)
		{
			float mins[3]{ FLT_MAX, FLT_MAX, FLT_MAX };
			float maxs[3]{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
			for (auto i = first_tri; i < first_tri + tri_count; i++)
			{
				const auto& face = surface.triIndices[i];
				for (const auto index : { face.v1, face.v2, face.v3 })
				{
					const auto* xyz = surface.verts0.packedVerts0[index].xyz;
					for (auto j = 0; j < 3; j++)
					{
						mins[j] = std::min(mins[j], xyz[j]);
						maxs[j] = std::max(maxs[j], xyz[j]);
					}
				}
			}

			target::XSurfaceTriCluster cluster{};
			for (auto j = 0; j < 3; j++)
			{
				cluster.midPoint[j] = half_float::float_to_half((mins[j] + maxs[j]) * 0.5f);
				const auto mid_point = half_float::half_to_float(cluster.midPoint[j]);
				cluster.halfSize[j] = half_round_up(std::max(maxs[j] - mid_point, mid_point - mins[j]));
			}

			const unsigned char no_cone[12] = { 0x00, 0x02, 0x08, 0xE0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0xFF }; // TODO: compute the normal cone
			std::memcpy(cluster.cone, no_cone, sizeof(cluster.cone));
			return cluster;
		}

		bool has_vertex_colors(const XSurface& surface)
		{
			if (surface.flags & (SURF_FLAG_VERTCOL_NONE | SURF_FLAG_REACTIVE_MOTION))
			{
				return false;
			}

			for (auto i = 0; i < surface.vertCount; i++)
			{
				if (surface.verts0.packedVerts0[i].color.packed != 0xFFFFFFFF)
				{
					return true;
				}
			}

			return false;
		}

		template <typename T>
		unsigned int append(std::string& data, const T* values, const std::size_t count)
		{
			const auto offset = static_cast<unsigned int>(data.size());
			data.append(reinterpret_cast<const char*>(values), count * sizeof(T));
			return offset;
		}

		void write_shared_data(const XSurface& surface, target::XSurface* result, std::string& data)
		{
			const auto& bounds = result->surfBounds;

			std::vector<target::Face> indices(2 * surface.triCount);
			std::memcpy(indices.data(), surface.triIndices, sizeof(target::Face) * surface.triCount);
			result->sharedIndexDataOffset = append(data, indices.data(), indices.size());

			float default_visibility[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
			const auto motion = (surface.flags & (SURF_FLAG_REACTIVE_MOTION | SURF_FLAG_SELF_VISIBILITY)) == SURF_FLAG_REACTIVE_MOTION;

			std::vector<target::GfxPackedVertex> verts(surface.vertCount);
			for (auto i = 0; i < surface.vertCount; i++)
			{
				const auto& vertex = surface.verts0.packedVerts0[i];
				verts[i].xyz = pack_position(vertex.xyz, bounds);
				verts[i].selfVisibility = motion ? self_visibility::XSurfacePackSelfVisibility(default_visibility) : vertex.selfVisibility.packed;
				verts[i].texCoord = vertex.texCoord.packed;
				verts[i].tangentFrame = pack_tangent_frame(unpack_unit_vec(vertex.normal.packed), unpack_unit_vec(vertex.tangent.packed),
					(vertex.tangent.packed >> 30) == 3);
			}
			result->sharedVertDataOffset = append(data, verts.data(), verts.size());

			if (result->flags & target::SURF_FLAG_VERTCOL)
			{
				std::vector<std::uint32_t> colors(surface.vertCount);
				for (auto i = 0; i < surface.vertCount; i++)
				{
					colors[i] = surface.verts0.packedVerts0[i].color.packed;
				}
				result->sharedColorDataOffset = append(data, colors.data(), colors.size());
			}

			std::vector<target::XSurfaceTriCluster> clusters((surface.triCount + 63) / 64);
			for (auto i = 0u; i < clusters.size(); i++)
			{
				const auto first_tri = i * 64;
				clusters[i] = make_tri_cluster(surface, first_tri, std::min(64u, static_cast<unsigned int>(surface.triCount) - first_tri));
			}
			result->sharedTriClusterDataOffset = append(data, clusters.data(), clusters.size());

			const auto surface_data = data.data() + result->sharedIndexDataOffset;
			result->hash = utils::cryptography::crc32::compute(reinterpret_cast<const std::uint8_t*>(surface_data),
				data.size() - result->sharedIndexDataOffset);

			data.resize((data.size() + 15) & ~15ull);
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
				lists[i].boneIndexOffset = list.boneOffsetIndex;
				lists[i].vertCount = list.vertCount;
				lists[i].triOffset = list.triOffset;
				lists[i].triCount = list.triCount;
			}

			return lists;
		}

		target::Bounds* convert_child_bounds(const XSurface& surface, utils::memory::allocator& allocator)
		{
			if (!surface.rigidVertLists || !surface.rigidVertListCount)
			{
				return nullptr;
			}

			const auto bounds = allocator.allocate_array<target::Bounds>(surface.rigidVertListCount);
			for (auto i = 0; i < surface.rigidVertListCount; i++)
			{
				const auto& list = surface.rigidVertLists[i];
				std::vector<GfxPackedVertex> verts;
				for (auto tri = list.triOffset; tri < list.triOffset + list.triCount; tri++)
				{
					const auto& face = surface.triIndices[tri];
					for (const auto index : { face.v1, face.v2, face.v3 })
					{
						verts.emplace_back(surface.verts0.packedVerts0[index]);
					}
				}
				bounds[i] = compute_bounds(verts.data(), verts.size());
			}

			return bounds;
		}

		unsigned short* convert_blend_verts(const XSurface& surface, utils::memory::allocator& allocator)
		{
			if (!surface.blendVerts || !surface.blendVertSize)
			{
				return nullptr;
			}

			auto size = 0u;
			for (auto bone_count = 1u; bone_count <= 8; bone_count++)
			{
				size += surface.blendVertCounts[bone_count - 1] * bone_count * 2;
			}

			if (size * sizeof(XBlendInfo) > surface.blendVertSize)
			{
				ZONETOOL_WARNING("Surface blend verts need %zu bytes, but have %u", size * sizeof(XBlendInfo), surface.blendVertSize);
				return nullptr;
			}

			const auto result = allocator.allocate_array<unsigned short>(surface.blendVertSize / sizeof(XBlendInfo));
			auto offset = 0u;
			for (auto bone_count = 1u; bone_count <= 8; bone_count++)
			{
				const auto vert_count = surface.blendVertCounts[bone_count - 1];
				for (auto vert = 0u; vert < vert_count; vert++)
				{
					const auto* blend = surface.blendVerts + offset + vert * bone_count * 2;
					for (auto bone = 0u; bone < bone_count; bone++)
					{
						auto* influence = result + offset + (bone * vert_count + vert) * 2;
						influence[0] = bone ? blend[bone * 2 - 1] : blend[0];
						influence[1] = bone ? blend[bone * 2] : 0;
					}
				}

				offset += vert_count * bone_count * 2;
			}

			return result;
		}

		std::uint16_t convert_flags(const XSurface& surface)
		{
			std::uint16_t result = target::SURF_FLAG_SELF_VISIBILITY;
			if (surface.blendVertSize)
			{
				result |= target::SURF_FLAG_SKINNED;
			}

			if (has_vertex_colors(surface))
			{
				result |= target::SURF_FLAG_VERTCOL;
			}

			return result;
		}

		bool convert_surface(const XModel* model, const XSurface& surface, target::XSurface* result, std::string& data, utils::memory::allocator& allocator)
		{
			if (!surface.verts0.packedVerts0 || !surface.triIndices)
			{
				return false;
			}

			result->flags = convert_flags(surface);
			result->vertCount = surface.vertCount;
			result->triCount = surface.triCount;
			result->rigidVertListCount = surface.rigidVertListCount;
			std::memcpy(result->blendVertCounts, surface.blendVertCounts, sizeof(result->blendVertCounts));
			result->blendVertSize = surface.blendVertSize;
			result->blendVerts = convert_blend_verts(surface, allocator);
			result->rigidVertLists = convert_rigid_vert_lists(surface, allocator);
			result->childBounds = convert_child_bounds(surface, allocator);
			std::memcpy(result->partBits.array, surface.partBits, sizeof(result->partBits.array));
			result->surfBounds = get_bounds(model, surface);

			result->sharedVertDataOffset = 0xFFFFFFFF;
			result->sharedIndexDataOffset = 0xFFFFFFFF;
			result->sharedTriClusterDataOffset = 0xFFFFFFFF;
			result->sharedColorDataOffset = 0xFFFFFFFF;
			result->sharedSecondUVDataOffset = 0xFFFFFFFF;
			result->sharedNormalTransformDataOffset = 0xFFFFFFFF;
			result->sharedTensionAccumTableOffset = 0xFFFFFFFF;
			result->sharedTensionDataOffset = 0xFFFFFFFF;
			write_shared_data(surface, result, data);
			return true;
		}

		bool fits(const target::Bounds& inner, const target::Bounds& outer)
		{
			for (auto i = 0; i < 3; i++)
			{
				if (std::abs(inner.midPoint[i] - outer.midPoint[i]) + inner.halfSize[i] > outer.halfSize[i])
				{
					return false;
				}
			}

			return true;
		}

		target::XModelSurfs* convert(const XModel* model, const XModelSurfs* asset, std::string& data, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::XModelSurfs>();
			result->name = asset->name;
			result->numsurfs = asset->numsurfs;
			result->surfs = allocator.allocate_array<target::XSurface>(asset->numsurfs);
			std::memcpy(result->partBits.array, asset->partBits, sizeof(result->partBits.array));

			for (auto i = 0; i < asset->numsurfs; i++)
			{
				const auto& surface = asset->surfs[i];
				if (surface.flags & (SURF_FLAG_REACTIVE_MOTION | SURF_FLAG_SUBDIV | SURF_FLAG_TENSION))
				{
					ZONETOOL_WARNING("Surface %d of xmodelsurfs \"%s\" has the unsupported flags 0x%X", i, asset->name, surface.flags);
				}

				if (!convert_surface(model, surface, &result->surfs[i], data, allocator))
				{
					ZONETOOL_ERROR("Surface %d of xmodelsurfs \"%s\" has no vertex data", i, asset->name);
					return nullptr;
				}

				if (!fits(compute_bounds(surface.verts0.packedVerts0, surface.vertCount), result->surfs[i].surfBounds))
				{
					ZONETOOL_WARNING("Surface %d of xmodelsurfs \"%s\" does not fit the bounds of xmodel \"%s\"", i, asset->name, model->name);
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
	}

	bool is_skinned(const XModel* model)
	{
		for (auto i = 0; i < model->numLods; i++)
		{
			const auto* surfs = model->lodInfo[i].modelSurfs;
			for (auto surf = 0; surfs && surfs->surfs && surf < surfs->numsurfs; surf++)
			{
				if (surfs->surfs[surf].blendVertSize)
				{
					return true;
				}
			}
		}

		return false;
	}

	std::optional<zonetool::iw8::Bounds> get_skinned_bounds(const XModel* model)
	{
		if (!is_skinned(model))
		{
			return {};
		}

		const auto size = model->radius * 1.5f; // TODO: stock rule (all skinned mp_rust models), loader code is not found
		return zonetool::iw8::Bounds{ { 0.0f, 0.0f, 0.0f }, { size, size, size } };
	}

	zonetool::iw8::Bounds get_bounds(const XModel* model, const XSurface& surface)
	{
		const auto skinned_bounds = get_skinned_bounds(model);
		return skinned_bounds ? *skinned_bounds : compute_bounds(surface.verts0.packedVerts0, surface.vertCount);
	}

	void dump(const XModel* model, XModelSurfs* asset)
	{
		if (!asset || !mark_dumped(ASSET_TYPE_XMODEL_SURFS, asset->name))
		{
			return;
		}

		utils::memory::allocator allocator;
		std::string data;
		if (const auto converted = convert(model, asset, data, allocator))
		{
			zonetool::iw8::xmodel_surfs::dump(converted);
		}
	}
}
