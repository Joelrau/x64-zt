#include <std_include.hpp>

#include "../include.hpp"
#include "lights.hpp"

namespace zonetool::t9::converter::iw8::lights
{
	namespace
	{
		namespace target = zonetool::iw8;

		using vec3 = std::array<float, 3>;

		template <typename T>
		T* make_array(const std::size_t count, utils::memory::allocator& allocator)
		{
			return count ? allocator.allocate_array<T>(count) : nullptr;
		}

		const Lighting* find_lighting(const Districts* asset)
		{
			if (!asset || !asset->world)
			{
				return nullptr;
			}

			const auto* loaded_world = static_cast<const GfxWorld*>(db_find_asset(ASSET_TYPE_GFX_MAP, xhash_name(asset->world->name)));
			const auto* world = loaded_world ? loaded_world : asset->world;
			if (!world->lighting)
			{
				return nullptr;
			}

			const auto* loaded = static_cast<const Lighting*>(db_find_asset(ASSET_TYPE_LIGHTING, xhash_name(world->lighting->name)));
			return loaded ? loaded : world->lighting;
		}

		const auto max_local_intensity = 252611.0f; // stock mp_rust maximum

		float luminance(const float* color)
		{
			return 0.2126f * color[0] + 0.7152f * color[1] + 0.0722f * color[2];
		}

		void set_color(target::ComPrimaryLight& light, const float* color, const float scale)
		{
			const auto color_luminance = luminance(color);
			if (color_luminance <= 0.0f)
			{
				light.colorLinearSrgb[0] = 1.0f;
				light.colorLinearSrgb[1] = 1.0f;
				light.colorLinearSrgb[2] = 1.0f;
				return;
			}

			for (auto i = 0; i < 3; i++)
			{
				light.colorLinearSrgb[i] = color[i] / color_luminance;
			}

			light.intensity = color_luminance * scale;
		}

		vec3 cross(const float* a, const float* b)
		{
			return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
		}

		double dot(const vec3& a, const vec3& b)
		{
			return static_cast<double>(a[0]) * b[0] + static_cast<double>(a[1]) * b[1] + static_cast<double>(a[2]) * b[2];
		}

		void orthonormal_up(const float* dir, const float* up, float* out)
		{
			const auto project = [&](const float* axis)
			{
				const auto along = axis[0] * dir[0] + axis[1] * dir[1] + axis[2] * dir[2];
				for (auto i = 0; i < 3; i++)
				{
					out[i] = axis[i] - along * dir[i];
				}
				return std::sqrt(out[0] * out[0] + out[1] * out[1] + out[2] * out[2]);
			};

			const auto dir_length = std::sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
			if (dir_length < 0.999f || dir_length > 1.001f)
			{
				std::memcpy(out, up, sizeof(float[3]));
				return;
			}

			auto length = project(up);
			if (length < 0.001f)
			{
				const float fallback[2][3] = {{0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}};
				length = project(fallback[std::abs(dir[2]) > 0.9f ? 1 : 0]);
			}

			for (auto i = 0; i < 3; i++)
			{
				out[i] /= length;
			}
		}

		target::ComPrimaryLight convert_sun(const LightingVolumeState& state)
		{
			target::ComPrimaryLight sun{};
			sun.type = target::GFX_LIGHT_TYPE_DIR;

			const auto yaw = state.sunYaw * std::numbers::pi_v<float> / 180.0f;
			const auto pitch = state.sunPitch * std::numbers::pi_v<float> / 180.0f;
			sun.dir[0] = -std::cos(pitch) * std::cos(yaw);
			sun.dir[1] = -std::cos(pitch) * std::sin(yaw);
			sun.dir[2] = std::sin(pitch);

			set_color(sun, state.sunColor, state.sunIntensity); // TODO: T9 sun intensity unit
			return sun;
		}

		target::ComPrimaryLight convert_light(const ComPrimaryLight& light)
		{
			target::ComPrimaryLight converted{};
			converted.spawnFlags = target::PRIMARY_LIGHT_SPAWN_NO_SHADOW;

			const auto omni = light.cosHalfFovOuter < 0.0f; // omni flag (0x7FF6E4543D9F)
			converted.type = omni ? target::GFX_LIGHT_TYPE_OMNI : target::GFX_LIGHT_TYPE_SPOT;

			const auto emitter_area = light.emitterSize[0] * light.emitterSize[1];
			set_color(converted, light.color, emitter_area); // TODO: T9 intensity unit, survey only
			converted.intensity = std::min(converted.intensity, max_local_intensity);

			for (auto i = 0; i < 3; i++)
			{
				converted.dir[i] = -light.dir[i];
				converted.origin[i] = light.origin[i];
			}

			orthonormal_up(converted.dir, light.heightAxis, converted.up);
			converted.radius = light.radius;
			converted.bulbRadius = std::sqrt(emitter_area / std::numbers::pi_v<float>); // TODO: T9 emitter shape

			const auto side = cross(converted.up, converted.dir);
			const auto side_length = std::sqrt(static_cast<float>(dot(side, side)));
			for (auto i = 0; side_length > 0.0f && i < 3; i++)
			{
				converted.bulbLength[i] = side[i] / side_length / 255.0f; // TODO: stock mp_rust pattern
			}

			if (omni)
			{
				converted.cosHalfFovOuter = 0.70710677f;
				converted.cosHalfFovInner = 1.0f;
			}
			else
			{
				const auto outer = std::acos(std::clamp(light.cosHalfFovOuter, -1.0f, 1.0f));
				converted.cosHalfFovOuter = light.cosHalfFovOuter;
				converted.cosHalfFovInner = std::cos(outer * (1.0f - std::clamp(light.feather, 0.0f, 1.0f))); // TODO: T9 feather
				converted.cosHalfFovCollimation = std::cos(std::numbers::pi_v<float> / 2.0f);
			}

			converted.shadowNearPlaneBias = 8.0f;
			converted.shadowSoftness = 0.55f;
			converted.shadowBias = 0.4f;
			converted.shadowArea = 0.0018f;
			converted.distanceFalloff = 0.2f;
			converted.rotationLimit = 1.0f;
			return converted;
		}

		bool is_bulb_light(const target::ComPrimaryLight& light)
		{
			return light.type == target::GFX_LIGHT_TYPE_OMNI && light.radius <= 50.0f;
		}

		target::ComPrimaryLight merge_lights(const std::vector<target::ComPrimaryLight>& lights, const std::vector<std::size_t>& members)
		{
			auto merged = lights[members.front()];
			auto total = 0.0f;
			auto weight_sum = 0.0f;
			vec3 origin{};
			vec3 color{};
			for (const auto index : members)
			{
				const auto& light = lights[index];
				const auto weight = std::max(light.intensity, 1.0f);
				total += light.intensity;
				weight_sum += weight;
				for (auto i = 0; i < 3; i++)
				{
					origin[i] += light.origin[i] * weight;
					color[i] += light.colorLinearSrgb[i] * weight;
				}
			}

			for (auto i = 0; i < 3; i++)
			{
				merged.origin[i] = origin[i] / weight_sum;
				merged.colorLinearSrgb[i] = color[i] / weight_sum;
			}

			merged.intensity = std::min(total, max_local_intensity);
			merged.radius = 0.0f;
			for (const auto index : members)
			{
				const auto& light = lights[index];
				const vec3 offset{light.origin[0] - merged.origin[0], light.origin[1] - merged.origin[1], light.origin[2] - merged.origin[2]};
				merged.radius = std::max(merged.radius, static_cast<float>(std::sqrt(dot(offset, offset))) + light.radius);
				merged.bulbRadius = std::max(merged.bulbRadius, light.bulbRadius);
			}

			return merged;
		}

		std::vector<target::ComPrimaryLight> merge_bulb_lights(const std::vector<target::ComPrimaryLight>& lights)
		{
			std::vector<target::ComPrimaryLight> result;
			std::map<std::array<int, 3>, std::vector<std::size_t>> cells;
			for (auto i = 0u; i < lights.size(); i++)
			{
				if (!is_bulb_light(lights[i]))
				{
					result.emplace_back(lights[i]);
					continue;
				}

				std::array<int, 3> cell{};
				for (auto axis = 0; axis < 3; axis++)
				{
					cell[axis] = static_cast<int>(std::floor(lights[i].origin[axis] / 128.0f));
				}

				cells[cell].emplace_back(i);
			}

			for (const auto& members : cells | std::views::values)
			{
				result.emplace_back(merge_lights(lights, members));
			}

			return result;
		}

		vec3 make_basis(const vec3& direction, vec3& v)
		{
			const auto reference = std::abs(direction[2]) < 0.999f ? vec3{0.0f, 0.0f, 1.0f} : vec3{0.0f, 1.0f, 0.0f};
			auto u = cross(reference.data(), direction.data());
			const auto length = std::sqrt(dot(u, u));
			for (auto& component : u)
			{
				component = static_cast<float>(component / length);
			}

			v = cross(direction.data(), u.data());
			return u;
		}

		vec3 volume_point(const vec3& origin, const vec3& direction, const vec3& u, const vec3& v, const double polar,
			const double azimuth, const double distance)
		{
			vec3 point{};
			for (auto i = 0; i < 3; i++)
			{
				point[i] = static_cast<float>(origin[i] + distance * (std::cos(polar) * direction[i]
					+ std::sin(polar) * (std::cos(azimuth) * u[i] + std::sin(azimuth) * v[i])));
			}
			return point;
		}

		void add_triangle(std::vector<unsigned short>& indices, const std::vector<vec3>& points, const vec3& inside,
			const unsigned int a, unsigned int b, unsigned int c)
		{
			vec3 edge1{}, edge2{}, to_inside{};
			for (auto i = 0; i < 3; i++)
			{
				edge1[i] = points[b][i] - points[a][i];
				edge2[i] = points[c][i] - points[a][i];
				to_inside[i] = inside[i] - points[a][i];
			}

			if (dot(cross(edge1.data(), edge2.data()), to_inside) < 0.0)
			{
				std::swap(b, c);
			}

			indices.push_back(static_cast<unsigned short>(a));
			indices.push_back(static_cast<unsigned short>(b));
			indices.push_back(static_cast<unsigned short>(c));
		}

		void build_spot_volume(const vec3& origin, const vec3& direction, const target::ComPrimaryLight& light, const float tube,
			std::vector<vec3>& points, std::vector<unsigned short>& indices)
		{
			const auto spot_segments = 8u;
			vec3 v{};
			const auto u = make_basis(direction, v);

			const auto outer = std::acos(std::clamp<double>(light.cosHalfFovOuter, 0.001, 0.998));
			const auto apex_offset = (light.bulbRadius + tube) / std::tan(outer);
			vec3 apex{};
			for (auto i = 0; i < 3; i++)
			{
				apex[i] = static_cast<float>(origin[i] - direction[i] * apex_offset);
			}

			const auto ring = std::cos(std::numbers::pi / spot_segments);
			const auto polar = std::atan(std::tan(outer) / ring);
			const auto rings = std::max(1u, static_cast<unsigned int>(std::ceil(polar / (std::numbers::pi / 6.0))));
			const auto step = polar / rings;
			const auto reach = (light.radius + tube + apex_offset) / (ring * std::cos(step * 0.5));

			points.push_back(apex);
			points.push_back(volume_point(apex, direction, u, v, 0.0, 0.0, reach));
			for (auto r = 1u; r <= rings; r++)
			{
				for (auto s = 0u; s < spot_segments; s++)
				{
					points.push_back(volume_point(apex, direction, u, v, step * r, 2.0 * std::numbers::pi * s / spot_segments, reach));
				}
			}

			vec3 inside{};
			for (auto i = 0; i < 3; i++)
			{
				inside[i] = static_cast<float>(apex[i] + direction[i] * reach * 0.5 * std::cos(polar));
			}

			const auto at = [](const unsigned int r, const unsigned int s)
			{
				return 2 + (r - 1) * spot_segments + s % spot_segments;
			};

			for (auto s = 0u; s < spot_segments; s++)
			{
				add_triangle(indices, points, inside, 1, at(1, s), at(1, s + 1));
				for (auto r = 1u; r < rings; r++)
				{
					add_triangle(indices, points, inside, at(r, s), at(r + 1, s), at(r + 1, s + 1));
					add_triangle(indices, points, inside, at(r, s), at(r + 1, s + 1), at(r, s + 1));
				}
				add_triangle(indices, points, inside, 0, at(rings, s + 1), at(rings, s));
			}
		}

		void build_omni_volume(const vec3& origin, const vec3& direction, const target::ComPrimaryLight& light, const float tube,
			std::vector<vec3>& points, std::vector<unsigned short>& indices)
		{
			const auto omni_segments = 6u;
			const auto omni_bands = 6u;
			vec3 v{};
			const auto u = make_basis(direction, v);

			const auto ring = std::cos(std::numbers::pi / omni_segments);
			const auto step = std::numbers::pi / omni_bands;
			const auto reach = (light.radius + tube) / (ring * std::cos(step * 0.5));

			points.push_back(volume_point(origin, direction, u, v, 0.0, 0.0, reach));
			points.push_back(volume_point(origin, direction, u, v, std::numbers::pi, 0.0, reach));
			for (auto r = 1u; r < omni_bands; r++)
			{
				for (auto s = 0u; s < omni_segments; s++)
				{
					points.push_back(volume_point(origin, direction, u, v, step * r, 2.0 * std::numbers::pi * s / omni_segments, reach));
				}
			}

			const auto at = [](const unsigned int r, const unsigned int s)
			{
				return 2 + (r - 1) * omni_segments + s % omni_segments;
			};

			for (auto s = 0u; s < omni_segments; s++)
			{
				add_triangle(indices, points, origin, 0, at(1, s), at(1, s + 1));
				for (auto r = 1u; r + 1 < omni_bands; r++)
				{
					add_triangle(indices, points, origin, at(r, s), at(r + 1, s), at(r + 1, s + 1));
					add_triangle(indices, points, origin, at(r, s), at(r + 1, s + 1), at(r, s + 1));
				}
				add_triangle(indices, points, origin, 1, at(omni_bands - 1, s + 1), at(omni_bands - 1, s));
			}
		}

		unsigned int pack_inward_normal(const std::vector<vec3>& points, const std::vector<unsigned short>& indices, const std::size_t vertex)
		{
			vec3 sum{};
			for (auto t = 0u; t + 2 < indices.size(); t += 3)
			{
				if (indices[t] != vertex && indices[t + 1] != vertex && indices[t + 2] != vertex)
				{
					continue;
				}

				const auto& a = points[indices[t]];
				const auto& b = points[indices[t + 1]];
				const auto& c = points[indices[t + 2]];
				const vec3 edge1{b[0] - a[0], b[1] - a[1], b[2] - a[2]};
				const vec3 edge2{c[0] - a[0], c[1] - a[1], c[2] - a[2]};
				const auto normal = cross(edge1.data(), edge2.data());
				const auto length = std::sqrt(dot(normal, normal));
				for (auto i = 0; length > 0.0 && i < 3; i++)
				{
					sum[i] += static_cast<float>(normal[i] / length);
				}
			}

			const auto length = std::sqrt(dot(sum, sum));
			auto packed = 3u << 30;
			for (auto i = 0; length > 0.0 && i < 3; i++)
			{
				const auto quantized = std::clamp(std::lround((sum[i] / length + 1.0) * 1023.0 / 2.0), 0l, 1023l);
				packed |= static_cast<unsigned int>(quantized) << (10 * i);
			}
			return packed;
		}
	}

	std::vector<target::ComPrimaryLight> convert_primary_lights(const Districts* asset)
	{
		std::vector<target::ComPrimaryLight> lights(2);
		lights[1].type = target::GFX_LIGHT_TYPE_DIR;

		const auto* lighting = find_lighting(asset);
		if (!lighting)
		{
			return lights;
		}

		if (lighting->volumeCount && lighting->volumes)
		{
			lights[1] = convert_sun(lighting->volumes[0].states[0]);
		}

		std::vector<target::ComPrimaryLight> local_lights;
		for (auto i = 0u; lighting->primaryLights && i < lighting->primaryLightCount; i++)
		{
			local_lights.emplace_back(convert_light(lighting->primaryLights[i]));
		}

		std::ranges::copy(merge_bulb_lights(local_lights), std::back_inserter(lights));
		return lights;
	}

	void log_lossy_fields(const Districts* asset)
	{
		const auto* lighting = find_lighting(asset);
		if (!lighting)
		{
			ZONETOOL_WARNING("districts \"%s\" has no lighting, the map has no sun and no primary lights", asset ? xhash_name(asset->name).data() : "");
			return;
		}

		const auto name = xhash_name(lighting->name);
		if (!lighting->volumeCount || !lighting->volumes)
		{
			ZONETOOL_WARNING("lighting \"%s\" has no lighting volume, the sun is off", name.data());
		}
		else if (lighting->volumes[0].states[0].sunIntensity <= 0.0f)
		{
			ZONETOOL_WARNING("lighting \"%s\": sun of volume 0 state 0 has intensity 0, the sun is off", name.data());
		}

		ZONETOOL_WARNING("lighting \"%s\": the sun heat intensity has no T9 source and is 0", name.data());

		auto area_lights = 0u;
		auto cookies = 0u;
		auto lod_fades = 0u;
		auto clamped = 0u;
		auto bulbs = 0u;
		for (auto i = 0u; lighting->primaryLights && i < lighting->primaryLightCount; i++)
		{
			const auto& light = lighting->primaryLights[i];
			bulbs += is_bulb_light(convert_light(light));
			area_lights += light.type == COM_PRIMARY_LIGHT_TYPE_AREA;
			cookies += light.image != nullptr;
			lod_fades += light.lodFadeEnd > 0.0f;
			clamped += luminance(light.color) * light.emitterSize[0] * light.emitterSize[1] > max_local_intensity;
		}

		if (clamped)
		{
			ZONETOOL_WARNING("lighting \"%s\": %u primary light intensities are above the stock maximum %.0f and are clamped", name.data(), clamped,
				max_local_intensity);
		}

		if (bulbs)
		{
			ZONETOOL_WARNING("lighting \"%s\": %u omni lights with a radius of 50 or less are merged into one light for each cell of 128 units, "
				"IW8 draws at most 64 omni lights for each pixel", name.data(), bulbs);
		}

		if (area_lights)
		{
			ZONETOOL_WARNING("lighting \"%s\": %u area lights are converted to spot lights", name.data(), area_lights);
		}

		if (cookies)
		{
			ZONETOOL_WARNING("lighting \"%s\": %u light cookies are not converted", name.data(), cookies);
		}

		if (lod_fades)
		{
			ZONETOOL_WARNING("lighting \"%s\": %u light lod fade ranges have no IW8 field", name.data(), lod_fades);
		}

		ZONETOOL_WARNING("lighting \"%s\": %u primary lights have no shadow maps", name.data(), lighting->primaryLightCount);
	}

	void build_frustum_lights(const std::vector<target::ComPrimaryLight>& lights, target::GfxFrustumLights& frustum_lights,
		utils::memory::allocator& allocator)
	{
		const auto light_count = static_cast<unsigned int>(lights.size());
		frustum_lights.lightCount = light_count;
		frustum_lights.indicesOffsets = make_array<unsigned int>(light_count, allocator);
		frustum_lights.verticesOffsets = make_array<unsigned int>(light_count, allocator);
		frustum_lights.indicesCounts = make_array<unsigned short>(light_count, allocator);
		frustum_lights.verticesCounts = make_array<unsigned short>(light_count, allocator);

		std::vector<unsigned short> indices;
		std::vector<std::array<unsigned int, 4>> vertices;
		auto skipped = 0u;
		for (auto index = 2u; index < light_count; index++)
		{
			const auto& light = lights[index];
			if (light.type != target::GFX_LIGHT_TYPE_SPOT && light.type != target::GFX_LIGHT_TYPE_OMNI)
			{
				continue;
			}

			const vec3 origin{light.origin[0], light.origin[1], light.origin[2]};
			const vec3 direction{-light.dir[0], -light.dir[1], -light.dir[2]};
			if (!std::isfinite(light.radius) || light.radius <= 0.0f || std::abs(dot(direction, direction) - 1.0) > 1e-3)
			{
				skipped++;
				continue;
			}

			const vec3 bulb_length{light.bulbLength[0], light.bulbLength[1], light.bulbLength[2]};
			const auto tube = static_cast<float>(std::sqrt(dot(bulb_length, bulb_length)));

			std::vector<vec3> points;
			std::vector<unsigned short> light_indices;
			if (light.type == target::GFX_LIGHT_TYPE_SPOT)
			{
				build_spot_volume(origin, direction, light, tube, points, light_indices);
			}
			else
			{
				build_omni_volume(origin, direction, light, tube, points, light_indices);
			}

			frustum_lights.indicesOffsets[index] = static_cast<unsigned int>(indices.size());
			frustum_lights.verticesOffsets[index] = static_cast<unsigned int>(vertices.size());
			frustum_lights.indicesCounts[index] = static_cast<unsigned short>(light_indices.size());
			frustum_lights.verticesCounts[index] = static_cast<unsigned short>(points.size());
			indices.insert(indices.end(), light_indices.begin(), light_indices.end());
			for (auto vertex = 0u; vertex < points.size(); vertex++)
			{
				const auto& point = points[vertex];
				vertices.push_back({std::bit_cast<unsigned int>(point[0]), std::bit_cast<unsigned int>(point[1]),
					std::bit_cast<unsigned int>(point[2]), pack_inward_normal(points, light_indices, vertex)});
			}
		}

		if (skipped)
		{
			ZONETOOL_WARNING("%u primary lights have an invalid radius or direction and no light volume", skipped);
		}

		frustum_lights.totalIndicesCount = static_cast<unsigned int>(indices.size());
		frustum_lights.totalVerticesCount = static_cast<unsigned int>(vertices.size());
		frustum_lights.indices = make_array<unsigned short>(indices.size(), allocator);
		frustum_lights.vertices = make_array<unsigned char>(vertices.size() * sizeof(vertices[0]), allocator);
		if (!indices.empty())
		{
			std::memcpy(frustum_lights.indices, indices.data(), indices.size() * sizeof(unsigned short));
			std::memcpy(frustum_lights.vertices, vertices.data(), vertices.size() * sizeof(vertices[0]));
		}
	}
}
