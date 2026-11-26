#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "material.hpp"

#include <iw8/assets/xmodel.hpp>
#include <iw8/assets/xsurface.hpp>

#include <shared.hpp>
#include <utils/cryptography.hpp>
#include <zone/script_strings.hpp>

namespace zonetool::t9::converter::iw8::xmodel
{
	namespace
	{
		namespace target = zonetool::iw8;

		using vec3 = std::array<float, 3>;
		using position = float[3];

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

	std::array<float, 3> unpack_unit_vec(const std::uint32_t packed)
	{
		vec3 result{};
		for (auto i = 0; i < 3; i++)
		{
			result[i] = static_cast<float>((packed >> (10 * i)) & 0x3FF) / 1023.0f * 2.0f - 1.0f;
		}

		return result;
	}

	std::uint32_t pack_tangent_frame(const std::array<float, 3>& normal, const std::array<float, 3>& tangent, const bool negative_binormal)
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

		target::Bounds compute_bounds(const position* positions, const std::size_t count)
		{
			float mins[3]{ FLT_MAX, FLT_MAX, FLT_MAX };
			float maxs[3]{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
			for (auto i = 0u; i < count; i++)
			{
				for (auto j = 0; j < 3; j++)
				{
					mins[j] = std::min(mins[j], positions[i][j]);
					maxs[j] = std::max(maxs[j], positions[i][j]);
				}
			}

			target::Bounds bounds{};
			if (count)
			{
				compute(&bounds, mins, maxs);
			}

			return bounds;
		}

		target::Bounds merge_bounds(const target::Bounds& a, const target::Bounds& b)
		{
			float mins[3]{};
			float maxs[3]{};
			for (auto i = 0; i < 3; i++)
			{
				mins[i] = std::min(a.midPoint[i] - a.halfSize[i], b.midPoint[i] - b.halfSize[i]);
				maxs[i] = std::max(a.midPoint[i] + a.halfSize[i], b.midPoint[i] + b.halfSize[i]);
			}

			target::Bounds bounds{};
			compute(&bounds, mins, maxs);
			return bounds;
		}

		target::XSurfaceTriCluster make_tri_cluster(const position* positions, const target::Face* faces, const unsigned int first_tri, const unsigned int tri_count)
		{
			float mins[3]{ FLT_MAX, FLT_MAX, FLT_MAX };
			float maxs[3]{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
			for (auto i = first_tri; i < first_tri + tri_count; i++)
			{
				for (const auto index : { faces[i].v1, faces[i].v2, faces[i].v3 })
				{
					for (auto j = 0; j < 3; j++)
					{
						mins[j] = std::min(mins[j], positions[index][j]);
						maxs[j] = std::max(maxs[j], positions[index][j]);
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

		template <typename T>
		unsigned int append(std::string& data, const T* values, const std::size_t count)
		{
			const auto offset = static_cast<unsigned int>(data.size());
			data.append(reinterpret_cast<const char*>(values), count * sizeof(T));
			return offset;
		}

		void write_shared_data(const position* positions, const XSurfaceVertex* vertices, const std::uint16_t* indices, target::XSurface* result, std::string& data)
		{
			const auto& bounds = result->surfBounds;

			std::vector<target::Face> faces(2 * result->triCount);
			std::memcpy(faces.data(), indices, sizeof(target::Face) * result->triCount);
			result->sharedIndexDataOffset = append(data, faces.data(), faces.size());

			float default_visibility[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
			const auto visibility = self_visibility::XSurfacePackSelfVisibility(default_visibility);

			std::vector<target::GfxPackedVertex> verts(result->vertCount);
			for (auto i = 0u; i < verts.size(); i++)
			{
				const auto& vertex = vertices[i];
				verts[i].xyz = pack_position(positions[i], bounds);
				verts[i].selfVisibility = visibility;
				verts[i].texCoord = vertex.texCoord[0] | (static_cast<std::uint32_t>(vertex.texCoord[1]) << 16);
				verts[i].tangentFrame = pack_tangent_frame(unpack_unit_vec(vertex.normal), unpack_unit_vec(vertex.tangent), (vertex.tangent >> 30) == 3);
			}
			result->sharedVertDataOffset = append(data, verts.data(), verts.size());

			if (result->flags & target::SURF_FLAG_VERTCOL)
			{
				std::vector<std::uint32_t> colors(result->vertCount);
				for (auto i = 0u; i < colors.size(); i++)
				{
					colors[i] = vertices[i].color;
				}
				result->sharedColorDataOffset = append(data, colors.data(), colors.size());
			}

			std::vector<target::XSurfaceTriCluster> clusters((result->triCount + 63) / 64);
			for (auto i = 0u; i < clusters.size(); i++)
			{
				const auto first_tri = i * 64;
				clusters[i] = make_tri_cluster(positions, faces.data(), first_tri, std::min(64u, static_cast<unsigned int>(result->triCount) - first_tri));
			}
			result->sharedTriClusterDataOffset = append(data, clusters.data(), clusters.size());

			const auto surface_data = data.data() + result->sharedIndexDataOffset;
			result->hash = utils::cryptography::crc32::compute(reinterpret_cast<const std::uint8_t*>(surface_data),
				data.size() - result->sharedIndexDataOffset);

			data.resize((data.size() + 15) & ~15ull);
		}

		std::uint16_t convert_flags(const XSurfaceVertex* vertices, const std::size_t count)
		{
			std::uint16_t result = target::SURF_FLAG_SELF_VISIBILITY;
			if (std::any_of(vertices, vertices + count, [](const XSurfaceVertex& vertex) { return vertex.color != 0xFFFFFFFF; }))
			{
				result |= target::SURF_FLAG_VERTCOL;
			}

			return result;
		}

		float max_length(const position* positions, const std::size_t count)
		{
			auto result = 0.0f;
			for (auto i = 0u; i < count; i++)
			{
				const vec3 p{ positions[i][0], positions[i][1], positions[i][2] };
				result = std::max(result, std::sqrt(dot(p, p)));
			}

			return result;
		}

		void convert_surface(const position* positions, const XSurfaceVertex* vertices, const std::uint16_t* indices, const unsigned int vert_count,
			const unsigned int tri_count, target::XSurface* result, std::string& shared_data)
		{
			result->flags = convert_flags(vertices, vert_count);
			result->vertCount = static_cast<unsigned short>(vert_count);
			result->triCount = static_cast<unsigned short>(tri_count);
			result->partBits.array[0] = 0x80000000;
			result->surfBounds = compute_bounds(positions, vert_count);

			result->sharedVertDataOffset = 0xFFFFFFFF;
			result->sharedIndexDataOffset = 0xFFFFFFFF;
			result->sharedTriClusterDataOffset = 0xFFFFFFFF;
			result->sharedColorDataOffset = 0xFFFFFFFF;
			result->sharedSecondUVDataOffset = 0xFFFFFFFF;
			result->sharedNormalTransformDataOffset = 0xFFFFFFFF;
			result->sharedTensionAccumTableOffset = 0xFFFFFFFF;
			result->sharedTensionDataOffset = 0xFFFFFFFF;
			write_shared_data(positions, vertices, indices, result, shared_data);
		}

		void convert_surface(const XSurface* surface, const std::string& data, const XModelMeshEntry& entry, target::XSurface* result, std::string& shared_data)
		{
			const auto indices = reinterpret_cast<const std::uint16_t*>(data.data()) + entry.indexStart;
			const auto positions = reinterpret_cast<const position*>(data.data() + surface->positionOffset) + entry.vertexStart;
			const auto vertices = reinterpret_cast<const XSurfaceVertex*>(data.data() + surface->vertexOffset) + entry.vertexStart;
			convert_surface(positions, vertices, indices, entry.vertCount, entry.triCount, result, shared_data);
		}

		const XModelMesh* resolve_mesh(const XModelMesh* mesh)
		{
			if (!mesh || (mesh->entries && mesh->surface))
			{
				return mesh;
			}

			const auto loaded = static_cast<const XModelMesh*>(db_find_asset(ASSET_TYPE_XMODELMESH, xhash_name(mesh->name)));
			return loaded ? loaded : mesh;
		}

		Material* find_material(const XModel* asset, const std::size_t slot, const std::uint32_t index)
		{
			if (slot >= asset->numLods || !asset->lodMaterials)
			{
				return nullptr;
			}

			const auto& materials = asset->lodMaterials[slot];
			return index < materials.count && materials.materials ? materials.materials[index] : nullptr;
		}

		bool has_no_draw_list(const Material* asset)
		{
			const auto* loaded = asset->techniqueSet || asset->textureCount
				? asset
				: static_cast<const Material*>(db_find_asset(ASSET_TYPE_MATERIAL, xhash_name(asset->name)));
			return loaded && loaded->drawList == DRAW_LIST_NONE;
		}

		bool has_drawn_mesh(const XModelMesh* mesh)
		{
			return mesh && mesh->entries && mesh->surface;
		}

		std::vector<std::uint64_t> get_drawn_materials(const XModel* asset, const std::size_t slot)
		{
			std::vector<std::uint64_t> result;
			const auto mesh = resolve_mesh(asset->meshes[slot]);
			for (auto i = 0u; has_drawn_mesh(mesh) && i < mesh->entryCount; i++)
			{
				const auto material = find_material(asset, slot, i);
				if (material && !has_no_draw_list(material))
				{
					result.emplace_back(material->name.value);
				}
			}

			std::ranges::sort(result);
			result.erase(std::ranges::unique(result).begin(), result.end());
			return result;
		}

		float get_max_threshold_above(const XModel* asset, const std::size_t slot)
		{
			auto result = 0.0f;
			for (auto above = slot + 1; above < std::min<std::size_t>(asset->numLods, std::size(asset->lodThresholds)); above++)
			{
				result = std::max(result, asset->lodThresholds[above]);
			}

			return result;
		}

		bool is_reachable_lod(const XModel* asset, const std::size_t slot)
		{
			return slot == 0 || asset->lodThresholds[slot] > get_max_threshold_above(asset, slot);
		}

		std::optional<std::size_t> find_top_slot(const XModel* asset)
		{
			for (auto slot = std::size(asset->meshes); slot-- > 0;)
			{
				if (has_drawn_mesh(resolve_mesh(asset->meshes[slot])))
				{
					return slot;
				}
			}

			return {};
		}

		std::vector<std::size_t> find_lod_slots(const XModel* asset)
		{
			const auto top = find_top_slot(asset);
			if (!top)
			{
				return {};
			}

			const auto materials = get_drawn_materials(asset, *top);
			for (auto slot = 0u; slot < *top; slot++)
			{
				if (is_reachable_lod(asset, slot) && has_drawn_mesh(resolve_mesh(asset->meshes[slot])) && get_drawn_materials(asset, slot) == materials)
				{
					return {*top, slot};
				}
			}

			return {*top};
		}

		float get_lod_dist(const XModel* asset, const std::vector<std::size_t>& slots, const std::size_t lod, const float radius)
		{
			if (lod + 1 >= slots.size())
			{
				return 1000000.0f;
			}

			return radius + std::sqrt(get_max_threshold_above(asset, slots[lod + 1]));
		}

		std::unordered_map<std::string, static_model_info> static_models;

		void add_collision_geometry(const XSurface* surface, const std::string& data, const XModelMeshEntry& entry, static_model_info& info)
		{
			const auto indices = reinterpret_cast<const std::uint16_t*>(data.data()) + entry.indexStart;
			const auto positions = reinterpret_cast<const position*>(data.data() + surface->positionOffset) + entry.vertexStart;
			const auto first_vertex = static_cast<std::uint32_t>(info.positions.size());

			for (auto i = 0u; i < entry.vertCount; i++)
			{
				info.positions.push_back({positions[i][0], positions[i][1], positions[i][2]});
			}

			for (auto i = 0u; i < 3 * entry.triCount; i++)
			{
				info.indices.push_back(first_vertex + indices[i]);
			}
		}

		std::string get_surfs_name(const XModelMesh* mesh, const std::vector<std::uint32_t>& entries)
		{
			const auto name = xhash_name(mesh->name);
			if (entries.size() == mesh->entryCount)
			{
				return name;
			}

			const auto hash = utils::cryptography::crc32::compute(reinterpret_cast<const std::uint8_t*>(entries.data()),
				entries.size() * sizeof(std::uint32_t));
			return std::format("{}_{:08x}", name, hash);
		}

		target::XModelSurfs* convert_surfs(const XModel* asset, const std::size_t slot, std::vector<const Material*>& materials, float& radius,
			std::string& shared_data, static_model_lod& lod, static_model_info* collision, utils::memory::allocator& allocator)
		{
			const auto name = xhash_name(asset->name);
			const auto mesh = resolve_mesh(asset->meshes[slot]);
			const auto data = read_mesh_data(mesh);
			if (!data)
			{
				ZONETOOL_WARNING("XModel \"%s\" has no mesh data for lod slot %zu", name.data(), slot);
				return nullptr;
			}

			const auto first_material = materials.size();
			std::vector<std::uint32_t> entries;
			for (auto i = 0u; i < mesh->entryCount; i++)
			{
				const auto& entry = mesh->entries[i];
				const auto material = find_material(asset, slot, i);
				if (material && has_no_draw_list(material))
				{
					continue;
				}

				if (!material)
				{
					ZONETOOL_WARNING("XModel \"%s\": skipping mesh entry %u, it has no material", name.data(), i);
					continue;
				}

				if (!entry.triCount || entry.triCount > 0xFFFF || !mesh_entry_fits(mesh->surface, *data, entry))
				{
					ZONETOOL_WARNING("XModel \"%s\": skipping mesh entry %u, %u triangles at index %u, %u vertices at %u do not fit the mesh data",
						name.data(), i, entry.triCount, entry.indexStart, entry.vertCount, entry.vertexStart);
					continue;
				}

				entries.emplace_back(i);
				materials.emplace_back(material);
			}

			if (entries.empty())
			{
				ZONETOOL_WARNING("XModel \"%s\" has no drawn mesh entry in lod slot %zu", name.data(), slot);
				return nullptr;
			}

			const auto result = allocator.allocate<target::XModelSurfs>();
			result->name = allocator.duplicate_string(get_surfs_name(mesh, entries));
			result->numsurfs = static_cast<unsigned short>(entries.size());
			result->surfs = allocator.allocate_array<target::XSurface>(entries.size());
			result->partBits.array[0] = 0x80000000;

			for (auto i = 0u; i < entries.size(); i++)
			{
				const auto& entry = mesh->entries[entries[i]];
				convert_surface(mesh->surface, *data, entry, &result->surfs[i], shared_data);
				lod.surfaces.push_back({result->surfs[i].surfBounds, xhash_name(materials[first_material + i]->name)});
				if (collision)
				{
					add_collision_geometry(mesh->surface, *data, entry, *collision);
				}

				const auto positions = reinterpret_cast<const position*>(data->data() + mesh->surface->positionOffset) + entry.vertexStart;
				radius = std::max(radius, max_length(positions, entry.vertCount));
			}

			result->shared = allocator.allocate<target::XSurfaceShared>();
			result->shared->data.residentData = shared_data.data();
			result->shared->dataSize = static_cast<unsigned int>(shared_data.size());
			for (auto i = 0u; i < entries.size(); i++)
			{
				result->surfs[i].shared = result->shared;
			}

			return result;
		}

		target::Material** convert_materials(const std::vector<const Material*>& materials, utils::memory::allocator& allocator)
		{
			std::unordered_map<std::string, target::Material*> stubs;
			const auto result = allocator.allocate_array<target::Material*>(materials.size());
			for (auto i = 0u; i < materials.size(); i++)
			{
				material::dump(materials[i]);
				const auto name = material::dump_model(materials[i]);
				auto& stub = stubs[name];
				if (!stub)
				{
					stub = allocator.allocate<target::Material>();
					stub->name = allocator.duplicate_string(name);
				}

				result[i] = stub;
			}

			return result;
		}

		void convert_bones(const target::Bounds& bounds, const float radius, target::XModel* result, utils::memory::allocator& allocator)
		{
			result->numBones = 1;
			result->numRootBones = 1;
			result->boneNames = allocator.allocate_array<target::scr_string_t>(1);
			result->boneNames[0] = static_cast<target::scr_string_t>(SL_GetString("tag_origin"));
			result->partClassification = allocator.allocate_array<unsigned char>(1);

			result->baseMat = allocator.allocate_array<target::DObjAnimMat>(1);
			result->baseMat[0].quat[3] = 1.0f;
			result->baseMat[0].transWeight = 2.0f;

			result->boneInfo = allocator.allocate_array<target::XBoneInfo>(1);
			result->boneInfo[0].bounds = bounds;
			result->boneInfo[0].radius.radiusSquared = radius * radius;
		}

		std::vector<target::XModelSurfs*> convert_lods(const XModel* asset, std::vector<const Material*>& materials, float& radius,
			std::deque<std::string>& shared_data, static_model_info& info, utils::memory::allocator& allocator)
		{
			const auto name = xhash_name(asset->name);
			const auto slots = find_lod_slots(asset);
			if (slots.empty())
			{
				ZONETOOL_WARNING("XModel \"%s\" has no mesh", name.data());
				return {};
			}

			if (!(resolve_mesh(asset->meshes[slots[0]])->surface->flags & XSURFACE_FLAG_RIGID))
			{
				ZONETOOL_WARNING("XModel \"%s\" is skinned, converting it as rigid", name.data());
			}

			std::vector<target::XModelSurfs*> lods;
			std::vector<std::size_t> converted_slots;
			for (const auto slot : slots)
			{
				static_model_lod lod{};
				const auto surfs = convert_surfs(asset, slot, materials, radius, shared_data.emplace_back(), lod, lods.empty() ? &info : nullptr, allocator);
				if (surfs)
				{
					lods.emplace_back(surfs);
					converted_slots.emplace_back(slot);
					info.lods.emplace_back(std::move(lod));
				}
			}

			for (auto i = 0u; i < info.lods.size(); i++)
			{
				info.lods[i].dist = get_lod_dist(asset, converted_slots, i, radius);
			}

			return lods;
		}

		target::XModel* convert(const std::string& name, const std::vector<target::XModelSurfs*>& lods, const static_model_info& info,
			const std::vector<const Material*>& materials, const float radius, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::XModel>();
			result->name = allocator.duplicate_string(name);
			result->numsurfs = static_cast<unsigned short>(materials.size());
			result->numLods = static_cast<unsigned char>(lods.size());
			result->shadowCutoffLod = result->numLods;
			result->scale = 1.0f;
			result->radius = radius;

			result->bounds = lods[0]->surfs[0].surfBounds;
			for (auto i = 1; i < lods[0]->numsurfs; i++)
			{
				result->bounds = merge_bounds(result->bounds, lods[0]->surfs[i].surfBounds);
			}

			convert_bones(result->bounds, radius, result, allocator);
			result->materialHandles = convert_materials(materials, allocator);

			auto first_surface = 0u;
			for (auto i = 0u; i < lods.size(); i++)
			{
				auto& lod = result->lodInfo[i];
				lod.dist = info.lods[i].dist;
				lod.numsurfs = lods[i]->numsurfs;
				lod.surfIndex = static_cast<unsigned short>(first_surface);
				lod.partBits.array[0] = 0x80000000;
				lod.modelSurfsStaging = allocator.allocate<target::XModelSurfs>();
				lod.modelSurfsStaging->name = lods[i]->name;
				first_surface += lods[i]->numsurfs;
			}

			return result;
		}
	}

	void dump(XModel* asset)
	{
		const auto name = xhash_name(asset->name);
		if (zonetool::t9::xmodel::is_placeholder(asset))
		{
			asset = static_cast<XModel*>(db_find_asset(ASSET_TYPE_XMODEL, name));
			if (!asset || zonetool::t9::xmodel::is_placeholder(asset))
			{
				ZONETOOL_WARNING("XModel \"%s\" is a name stub, and no loaded zone holds the model", name.data());
				return;
			}
		}

		if (static_models.contains(name))
		{
			return;
		}

		utils::memory::allocator allocator;
		std::vector<const Material*> materials;
		std::deque<std::string> shared_data;
		auto radius = 0.0f;
		static_model_info info{};
		const auto lods = convert_lods(asset, materials, radius, shared_data, info, allocator);
		if (lods.empty() || !mark_dumped(zonetool::iw8::ASSET_TYPE_XMODEL, name))
		{
			return;
		}

		const auto converted = convert(name, lods, info, materials, radius, allocator);
		zonetool::iw8::xmodel::dump(converted);
		add_csv_row("xmodel", name);

		info.name = name;
		info.bounds = converted->bounds;
		static_models.emplace(name, std::move(info));

		for (const auto surfs : lods)
		{
			if (mark_dumped(zonetool::iw8::ASSET_TYPE_XMODEL_SURFS, surfs->name))
			{
				zonetool::iw8::xmodel_surfs::dump(surfs);
			}
		}
	}

	const static_model_info* dump_static(XModel* asset)
	{
		dump(asset);
		const auto found = static_models.find(xhash_name(asset->name));
		return found != static_models.end() ? &found->second : nullptr;
	}
}
