#include "optimizer/umbraComputationParams.hpp"
#include "optimizer/umbraLocalComputation.hpp"
#include "optimizer/umbraScene.hpp"
#include "optimizer/umbraOptimizerTextCommand.hpp"
#include "runtime/umbraQuery.hpp"
#include "runtime/umbraTome.hpp"

#include "utils/umbra/scene.hpp"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#define NOMINMAX
#include <Windows.h>

#include <algorithm>
#include <cfloat>

namespace
{
	using namespace utils::umbra;

	Umbra::Vector3 vec3(const float x, const float y, const float z)
	{
		Umbra::Vector3 v;
		v.v[0] = x;
		v.v[1] = y;
		v.v[2] = z;
		return v;
	}

	class file_logger final : public Umbra::Logger
	{
	public:
		explicit file_logger(const char* path)
		{
			if (path && *path)
			{
				this->file_ = std::fopen(path, "w");
			}
		}

		~file_logger() override
		{
			if (this->file_)
			{
				std::fclose(this->file_);
			}
		}

		void log(Level level, const char* str) override
		{
			static const char* names[] = { "DEBUG", "INFO", "WARNING", "ERROR" };
			const auto* name = (level >= 0 && level < 4) ? names[level] : "?";
			if (this->file_)
			{
				std::fprintf(this->file_, "[%s] %s\n", name, str);
				std::fflush(this->file_);
			}
		}

	private:
		std::FILE* file_ = nullptr;
	};

	bool read_file(const char* path, std::vector<unsigned char>& out)
	{
		auto* file = std::fopen(path, "rb");
		if (!file)
		{
			return false;
		}

		std::fseek(file, 0, SEEK_END);
		const auto size = std::ftell(file);
		std::fseek(file, 0, SEEK_SET);
		out.resize(static_cast<std::size_t>(size));
		const auto read = std::fread(out.data(), 1, out.size(), file);
		std::fclose(file);
		return read == out.size();
	}

	bool write_file(const char* path, const void* data, std::size_t size)
	{
		auto* file = std::fopen(path, "wb");
		if (!file)
		{
			return false;
		}
		const auto written = std::fwrite(data, 1, size, file);
		std::fclose(file);
		return written == size;
	}

	struct loaded_scene
	{
		scene_header header{};
		Umbra::Scene* scene = nullptr;
		std::size_t triangle_count = 0;
		std::size_t occluder_count = 0;
		std::size_t target_count = 0;
		std::vector<Umbra::Vector3> seed_points;
		std::vector<std::vector<float>> model_vertices;
		std::vector<std::vector<std::uint32_t>> model_indices;
		std::vector<scene_object> objects;
	};

	bool load_scene(const char* path, loaded_scene& out)
	{
		std::vector<unsigned char> data;
		if (!read_file(path, data))
		{
			std::printf("error: cannot read scene \"%s\"\n", path);
			return false;
		}

		std::size_t offset = 0;
		const auto take = [&](void* dst, const std::size_t size)
		{
			if (offset + size > data.size())
			{
				return false;
			}
			std::memcpy(dst, data.data() + offset, size);
			offset += size;
			return true;
		};

		auto& header = out.header;
		if (!take(&header, sizeof(header)) || std::memcmp(header.magic, SCENE_MAGIC, sizeof(SCENE_MAGIC)) != 0
			|| header.version != SCENE_VERSION || header.header_size != sizeof(scene_header))
		{
			std::printf("error: \"%s\" is not a version %u scene\n", path, SCENE_VERSION);
			return false;
		}

		out.scene = Umbra::Scene::create();
		if (!out.scene)
		{
			std::printf("error: Umbra::Scene::create failed\n");
			return false;
		}

		std::vector<const Umbra::SceneModel*> models;
		models.reserve(header.model_count);

		std::vector<float> vertices;
		std::vector<std::uint32_t> indices;
		for (std::uint32_t i = 0; i < header.model_count; i++)
		{
			scene_model model{};
			if (!take(&model, sizeof(model)))
			{
				std::printf("error: truncated model %u\n", i);
				return false;
			}

			vertices.resize(static_cast<std::size_t>(model.vertex_count) * 3);
			indices.resize(static_cast<std::size_t>(model.triangle_count) * 3);
			if (!take(vertices.data(), vertices.size() * sizeof(float))
				|| !take(indices.data(), indices.size() * sizeof(std::uint32_t)))
			{
				std::printf("error: truncated model %u data\n", i);
				return false;
			}

			for (const auto index : indices)
			{
				if (index >= model.vertex_count)
				{
					std::printf("error: model %u index %u out of range (%u vertices)\n", i, index, model.vertex_count);
					return false;
				}
			}

			const auto* inserted = out.scene->insertModel(vertices.data(), indices.data(),
				static_cast<int>(model.vertex_count), static_cast<int>(model.triangle_count), sizeof(float) * 3);
			if (!inserted)
			{
				std::printf("error: Umbra::Scene::insertModel failed for model %u (%u verts, %u tris)\n",
					i, model.vertex_count, model.triangle_count);
				return false;
			}

			models.push_back(inserted);
			out.triangle_count += model.triangle_count;
			out.model_vertices.push_back(vertices);
			out.model_indices.push_back(indices);
		}

		Umbra::Matrix4x4 identity{};
		for (int i = 0; i < 4; i++)
		{
			identity.m[i][i] = 1.0f;
		}

		for (std::uint32_t i = 0; i < header.object_count; i++)
		{
			scene_object object{};
			if (!take(&object, sizeof(object)))
			{
				std::printf("error: truncated object %u\n", i);
				return false;
			}
			if (object.model >= models.size())
			{
				std::printf("error: object %u references model %u of %zu\n", i, object.model, models.size());
				return false;
			}

			if (!out.scene->insertObject(models[object.model], identity, object.user_id, object.flags))
			{
				std::printf("error: Umbra::Scene::insertObject failed for object %u (id 0x%08X flags %u)\n",
					i, object.user_id, object.flags);
				return false;
			}

			out.occluder_count += (object.flags & SCENE_OBJECT_OCCLUDER) ? 1 : 0;
			out.target_count += (object.flags & SCENE_OBJECT_TARGET) ? 1 : 0;
			out.objects.push_back(object);
		}

		for (std::uint32_t i = 0; i < header.view_volume_count; i++)
		{
			scene_view_volume volume{};
			if (!take(&volume, sizeof(volume)))
			{
				std::printf("error: truncated view volume %u\n", i);
				return false;
			}

			out.scene->insertViewVolume(vec3(volume.mins[0], volume.mins[1], volume.mins[2]),
				vec3(volume.maxs[0], volume.maxs[1], volume.maxs[2]), volume.id);
		}

		for (std::uint32_t i = 0; i < header.seed_point_count; i++)
		{
			scene_seed_point seed{};
			if (!take(&seed, sizeof(seed)))
			{
				std::printf("error: truncated seed point %u\n", i);
				return false;
			}

			out.scene->insertSeedPoint(vec3(seed.position[0], seed.position[1], seed.position[2]));
			out.seed_points.push_back(vec3(seed.position[0], seed.position[1], seed.position[2]));
		}

		if (offset != data.size())
		{
			std::printf("warning: %zu trailing bytes in scene\n", data.size() - offset);
		}

		return true;
	}

	Umbra::Matrix4x4 make_world_to_clip(const float* eye, const float* forward, const float* up,
		const float near_z, const float far_z, const float fov_scale_x = 1.0f, const float fov_scale_y = 1.0f)
	{
		float f[3] = { forward[0], forward[1], forward[2] };
		const auto f_len = std::sqrt(f[0] * f[0] + f[1] * f[1] + f[2] * f[2]);
		for (auto& c : f) c /= f_len;

		float s[3] = { f[1] * up[2] - f[2] * up[1], f[2] * up[0] - f[0] * up[2], f[0] * up[1] - f[1] * up[0] };
		const auto s_len = std::sqrt(s[0] * s[0] + s[1] * s[1] + s[2] * s[2]);
		for (auto& c : s) c /= s_len;

		const float u[3] = { s[1] * f[2] - s[2] * f[1], s[2] * f[0] - s[0] * f[2], s[0] * f[1] - s[1] * f[0] };

		const auto dot = [](const float* a, const float* b)
		{
			return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
		};

		const float view[4][4] = {
			{ s[0], s[1], s[2], -dot(s, eye) },
			{ u[0], u[1], u[2], -dot(u, eye) },
			{ -f[0], -f[1], -f[2], dot(f, eye) },
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};

		const float proj[4][4] = {
			{ fov_scale_x, 0.0f, 0.0f, 0.0f },
			{ 0.0f, fov_scale_y, 0.0f, 0.0f },
			{ 0.0f, 0.0f, far_z / (near_z - far_z), near_z * far_z / (near_z - far_z) },
			{ 0.0f, 0.0f, -1.0f, 0.0f },
		};

		float clip[4][4]{};
		for (int r = 0; r < 4; r++)
		{
			for (int c = 0; c < 4; c++)
			{
				for (int k = 0; k < 4; k++)
				{
					clip[r][c] += proj[r][k] * view[k][c];
				}
			}
		}

		Umbra::Matrix4x4 out{};
		for (int r = 0; r < 4; r++)
		{
			for (int c = 0; c < 4; c++)
			{
				out.m[c][r] = clip[r][c];
			}
		}
		return out;
	}

	bool verify_tome(const std::vector<unsigned char>& tome_data, const loaded_scene& scene)
	{
		const auto* tome = Umbra::TomeLoader::loadFromBuffer(tome_data.data(), tome_data.size());
		if (!tome)
		{
			std::printf("verify: TomeLoader::loadFromBuffer failed\n");
			return false;
		}
		if (tome->getStatus() != Umbra::Tome::STATUS_OK)
		{
			std::printf("verify: tome status %d\n", static_cast<int>(tome->getStatus()));
			return false;
		}

		const auto object_count = tome->getObjectCount();
		std::printf("verify: version 0x%X size %u objects %d cells %d clusters %d tiles %d\n",
			tome->getVersion(), tome->getSize(), object_count, tome->getCellCount(),
			tome->getClusterCount(), tome->getTileCount());

		std::vector<Umbra::Vector3> cameras;
		for (int i = 0; i < scene.scene->getViewVolumeCount(); i++)
		{
			const auto& mn = scene.scene->getViewVolume(i)->getMin();
			const auto& mx = scene.scene->getViewVolume(i)->getMax();
			cameras.push_back(vec3((mn.v[0] + mx.v[0]) * 0.5f, (mn.v[1] + mx.v[1]) * 0.5f, (mn.v[2] + mx.v[2]) * 0.5f));
		}
		cameras.insert(cameras.end(), scene.seed_points.begin(), scene.seed_points.end());

		const float directions[6][3] = {
			{ 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, -1 },
		};

		std::vector<std::uint32_t> mask((object_count + 31) / 32 + 1);

		Umbra::Query query(tome);
		std::size_t queries = 0, errors = 0, total_visible = 0;
		int min_visible = object_count, max_visible = 0;
		for (const auto& eye : cameras)
		{
			for (const auto& dir : directions)
			{
				const float up_z[3] = { 0, 0, 1 };
				const float up_x[3] = { 1, 0, 0 };
				const auto* up = std::fabs(dir[2]) > 0.5f ? up_x : up_z;
				const Umbra::CameraTransform camera(make_world_to_clip(eye.v, dir, up, 1.0f, 100000.0f), eye);

				Umbra::Visibility visibility;
				std::fill(mask.begin(), mask.end(), 0u);
				visibility.setOutputObjectMask(mask.data());

				const auto error = query.queryPortalVisibility(0, visibility, camera);
				queries++;
				if (error != Umbra::Query::ERROR_OK)
				{
					errors++;
					std::printf("verify: query at (%g %g %g) dir (%g %g %g) failed with %d\n",
						eye.v[0], eye.v[1], eye.v[2], dir[0], dir[1], dir[2], static_cast<int>(error));
					continue;
				}

				int visible = 0;
				for (int i = 0; i < object_count; i++)
				{
					visible += (mask[i >> 5] >> (i & 31)) & 1;
				}
				total_visible += visible;
				min_visible = std::min(min_visible, visible);
				max_visible = std::max(max_visible, visible);
			}
		}

		if (queries)
		{
			std::printf("verify: %zu queries from %zu cameras, %zu failed, visible objects min %d avg %.1f max %d of %d\n",
				queries, cameras.size(), errors, errors == queries ? 0 : min_visible,
				queries != errors ? static_cast<double>(total_visible) / static_cast<double>(queries - errors) : 0.0,
				max_visible, object_count);
		}

		Umbra::TomeLoader::freeTome(tome);
		return errors == 0;
	}

	struct ray_triangle
	{
		float a[3], b[3], c[3];
		std::uint32_t object;
	};

	bool ray_hits_triangle(const float* origin, const float* dir, const ray_triangle& tri, float& t)
	{
		const float e1[3] = { tri.b[0] - tri.a[0], tri.b[1] - tri.a[1], tri.b[2] - tri.a[2] };
		const float e2[3] = { tri.c[0] - tri.a[0], tri.c[1] - tri.a[1], tri.c[2] - tri.a[2] };
		const float p[3] = { dir[1] * e2[2] - dir[2] * e2[1], dir[2] * e2[0] - dir[0] * e2[2], dir[0] * e2[1] - dir[1] * e2[0] };
		const auto det = e1[0] * p[0] + e1[1] * p[1] + e1[2] * p[2];
		if (std::fabs(det) < 1e-8f)
		{
			return false;
		}
		const auto inv = 1.0f / det;
		const float s[3] = { origin[0] - tri.a[0], origin[1] - tri.a[1], origin[2] - tri.a[2] };
		const auto u = (s[0] * p[0] + s[1] * p[1] + s[2] * p[2]) * inv;
		if (u < -1e-4f || u > 1.0001f)
		{
			return false;
		}
		const float q[3] = { s[1] * e1[2] - s[2] * e1[1], s[2] * e1[0] - s[0] * e1[2], s[0] * e1[1] - s[1] * e1[0] };
		const auto v = (dir[0] * q[0] + dir[1] * q[1] + dir[2] * q[2]) * inv;
		if (v < -1e-4f || u + v > 1.0001f)
		{
			return false;
		}
		t = (e2[0] * q[0] + e2[1] * q[1] + e2[2] * q[2]) * inv;
		return true;
	}

	int check(const int argc, char** argv)
	{
		if (argc < 4)
		{
			std::printf("usage: umbra-tomegen check <scene> <tome> [--cameras N] [--seed N]\n");
			return 2;
		}

		int camera_count = 200;
		unsigned int seed = 1;
		bool frustum_only = false;
		float clearance_limit = 32.0f;
		int jobs = 4;
		float accurate_threshold = 256.0f;

		for (int i = 4; i < argc; i++)
		{
			if (!std::strcmp(argv[i], "--cameras") && i + 1 < argc)
			{
				camera_count = std::atoi(argv[++i]);
			}
			else if (!std::strcmp(argv[i], "--clearance") && i + 1 < argc)
			{
				clearance_limit = static_cast<float>(std::atof(argv[++i]));
			}
			else if (!std::strcmp(argv[i], "--jobs") && i + 1 < argc)
			{
				jobs = std::max(1, std::atoi(argv[++i]));
			}
			else if (!std::strcmp(argv[i], "--threshold") && i + 1 < argc)
			{
				accurate_threshold = static_cast<float>(std::atof(argv[++i]));
			}
			else if (!std::strcmp(argv[i], "--frustum-only"))
			{
				frustum_only = true;
			}
			else if (!std::strcmp(argv[i], "--seed") && i + 1 < argc)
			{
				seed = static_cast<unsigned int>(std::atoi(argv[++i]));
			}
		}

		loaded_scene scene;
		if (!load_scene(argv[2], scene))
		{
			return 3;
		}

		std::vector<unsigned char> tome_data;
		if (!read_file(argv[3], tome_data))
		{
			std::printf("error: cannot read \"%s\"\n", argv[3]);
			return 3;
		}

		const auto* tome = Umbra::TomeLoader::loadFromBuffer(tome_data.data(), tome_data.size());
		if (!tome || tome->getStatus() != Umbra::Tome::STATUS_OK)
		{
			std::printf("error: tome did not load\n");
			return 4;
		}

		const auto object_count = tome->getObjectCount();
		std::vector<int> tome_to_scene(object_count, -1);
		{
			std::vector<std::uint32_t> ids(16);
			for (int i = 0; i < object_count; i++)
			{
				const auto n = tome->getObjectUserIDs(i, ids.data(), static_cast<int>(ids.size()));
				for (int k = 0; k < n && k < 16; k++)
				{
					for (std::size_t o = 0; o < scene.objects.size(); o++)
					{
						if (scene.objects[o].user_id == ids[k])
						{
							tome_to_scene[i] = static_cast<int>(o);
						}
					}
				}
			}
		}

		std::vector<ray_triangle> occluders;
		for (std::size_t o = 0; o < scene.objects.size(); o++)
		{
			if (!(scene.objects[o].flags & SCENE_OBJECT_OCCLUDER))
			{
				continue;
			}
			const auto& v = scene.model_vertices[scene.objects[o].model];
			const auto& idx = scene.model_indices[scene.objects[o].model];
			for (std::size_t t = 0; t + 2 < idx.size(); t += 3)
			{
				ray_triangle tri{};
				std::memcpy(tri.a, &v[idx[t] * 3], 12);
				std::memcpy(tri.b, &v[idx[t + 1] * 3], 12);
				std::memcpy(tri.c, &v[idx[t + 2] * 3], 12);
				tri.object = static_cast<std::uint32_t>(o);
				occluders.push_back(tri);
			}
		}
		std::printf("check: %zu occluder triangles, %d tome objects, %d cameras\n", occluders.size(), object_count, camera_count);

		Umbra::Vector3 mn = vec3(FLT_MAX, FLT_MAX, FLT_MAX), mx = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		for (const auto& tri : occluders)
		{
			for (const float* corner : { tri.a, tri.b, tri.c })
			{
				for (int a = 0; a < 3; a++)
				{
					mn.v[a] = std::min(mn.v[a], corner[a]);
					mx.v[a] = std::max(mx.v[a], corner[a]);
				}
			}
		}
		if (occluders.empty())
		{
			mn = scene.scene->getViewVolume(0)->getMin();
			mx = scene.scene->getViewVolume(0)->getMax();
		}
		for (int a = 0; a < 3; a++)
		{
			mn.v[a] -= 64.0f;
			mx.v[a] += 64.0f;
		}
		std::srand(seed);
		const auto frand = []() { return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX); };

		const float directions[6][3] = {
			{ 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, -1 },
		};
		std::vector<std::uint32_t> mask((object_count + 31) / 32 + 1);
		std::vector<std::uint32_t> job_mask(mask.size());
		Umbra::Query query(tome);

		const auto clearance = [&](const float* p)
		{
			float best = FLT_MAX;
			bool best_inside_solid = false;
			for (const auto& tri : occluders)
			{
				const float* corners[3] = { tri.a, tri.b, tri.c };
				const float e1[3] = { tri.b[0] - tri.a[0], tri.b[1] - tri.a[1], tri.b[2] - tri.a[2] };
				const float e2[3] = { tri.c[0] - tri.a[0], tri.c[1] - tri.a[1], tri.c[2] - tri.a[2] };
				float n[3] = { e1[1] * e2[2] - e1[2] * e2[1], e1[2] * e2[0] - e1[0] * e2[2], e1[0] * e2[1] - e1[1] * e2[0] };
				const auto nl = std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
				if (nl < 1e-6f)
				{
					continue;
				}
				for (auto& k : n) k /= nl;
				const auto dist = (p[0] - tri.a[0]) * n[0] + (p[1] - tri.a[1]) * n[1] + (p[2] - tri.a[2]) * n[2];
				const bool behind = dist > 0.0f;

				const float q[3] = { p[0] - dist * n[0], p[1] - dist * n[1], p[2] - dist * n[2] };
				bool inside = true;
				for (int k = 0; k < 3 && inside; k++)
				{
					const float* c0 = corners[k];
					const float* c1 = corners[(k + 1) % 3];
					const float e[3] = { c1[0] - c0[0], c1[1] - c0[1], c1[2] - c0[2] };
					const float w[3] = { q[0] - c0[0], q[1] - c0[1], q[2] - c0[2] };
					const float x[3] = { e[1] * w[2] - e[2] * w[1], e[2] * w[0] - e[0] * w[2], e[0] * w[1] - e[1] * w[0] };
					inside = (x[0] * n[0] + x[1] * n[1] + x[2] * n[2]) >= 0.0f;
				}

				float d;
				if (inside)
				{
					d = std::fabs(dist);
				}
				else
				{
					d = FLT_MAX;
					for (int k = 0; k < 3; k++)
					{
						const float* c0 = corners[k];
						const float* c1 = corners[(k + 1) % 3];
						const float e[3] = { c1[0] - c0[0], c1[1] - c0[1], c1[2] - c0[2] };
						const float w[3] = { p[0] - c0[0], p[1] - c0[1], p[2] - c0[2] };
						const auto ee = e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
						auto t = ee > 0.0f ? (w[0] * e[0] + w[1] * e[1] + w[2] * e[2]) / ee : 0.0f;
						t = std::min(1.0f, std::max(0.0f, t));
						const float r[3] = { w[0] - t * e[0], w[1] - t * e[1], w[2] - t * e[2] };
						d = std::min(d, std::sqrt(r[0] * r[0] + r[1] * r[1] + r[2] * r[2]));
					}
				}
				if (d < best)
				{
					best = d;
					best_inside_solid = behind;
				}
			}
			return best_inside_solid ? -best : best;
		};

		std::size_t queries = 0, false_occlusions = 0, checked_hidden = 0, reported = 0;
		std::size_t front_facing_leaks = 0, clear_camera_leaks = 0, serious_leaks = 0;
		for (int c = 0; c < camera_count; c++)
		{
			const float eye[3] = { mn.v[0] + frand() * (mx.v[0] - mn.v[0]), mn.v[1] + frand() * (mx.v[1] - mn.v[1]),
				mn.v[2] + frand() * (mx.v[2] - mn.v[2]) };
			const auto camera_clearance = clearance(eye);
			for (int view = 0; view < 12; view++)
			{
				float dir[3];
				float fov_x = 1.0f, fov_y = 1.0f;
				if (view < 6)
				{
					std::memcpy(dir, directions[view], sizeof(dir));
				}
				else
				{
					const auto yaw = frand() * 6.2831853f;
					const auto pitch = (frand() - 0.5f) * 3.0f;
					dir[0] = std::cos(pitch) * std::cos(yaw);
					dir[1] = std::cos(pitch) * std::sin(yaw);
					dir[2] = std::sin(pitch);
					const auto fov = (60.0f + frand() * 40.0f) * 3.14159265f / 180.0f;
					fov_x = 1.0f / std::tan(fov * 0.5f);
					fov_y = fov_x * (16.0f / 9.0f);
				}
				const float up_z[3] = { 0, 0, 1 };
				const float up_x[3] = { 1, 0, 0 };
				const auto* up = std::fabs(dir[2]) > 0.95f ? up_x : up_z;
				const auto clip = make_world_to_clip(eye, dir, up, 1.0f, 100000.0f, fov_x, fov_y);
				const Umbra::CameraTransform camera(clip, vec3(eye[0], eye[1], eye[2]));

				std::fill(mask.begin(), mask.end(), 0u);
				auto error = Umbra::Query::ERROR_OK;
				if (frustum_only)
				{
					Umbra::Visibility visibility;
					visibility.setOutputObjectMask(mask.data());
					error = query.queryFrustumVisibility(0, visibility, camera);
				}
				else
				{
					for (int job = 0; job < jobs && error == Umbra::Query::ERROR_OK; job++)
					{
						std::fill(job_mask.begin(), job_mask.end(), 0u);
						Umbra::Visibility visibility;
						visibility.setOutputObjectMask(job_mask.data());
						error = query.queryPortalVisibility(0, visibility, camera, 0.0f, accurate_threshold, nullptr, job, jobs, jobs);
						for (std::size_t w = 0; w < mask.size(); w++)
						{
							mask[w] |= job_mask[w];
						}
					}
				}
				if (error != Umbra::Query::ERROR_OK)
				{
					continue;
				}
				queries++;

				const auto in_frustum = [&](const float* p)
				{
					float cl[4] = { 0, 0, 0, 0 };
					for (int r = 0; r < 4; r++)
					{
						cl[r] = clip.m[0][r] * p[0] + clip.m[1][r] * p[1] + clip.m[2][r] * p[2] + clip.m[3][r];
					}
					return cl[3] > 0.0f && std::fabs(cl[0]) <= cl[3] && std::fabs(cl[1]) <= cl[3] && cl[2] >= 0.0f && cl[2] <= cl[3];
				};

				for (int i = 0; i < object_count; i++)
				{
					if ((mask[i >> 5] >> (i & 31)) & 1)
					{
						continue;
					}
					const auto o = tome_to_scene[i];
					if (o < 0)
					{
						continue;
					}
					checked_hidden++;

					const auto& v = scene.model_vertices[scene.objects[o].model];
					const auto& idx = scene.model_indices[scene.objects[o].model];
					bool leak = false;
					float leak_point[3]{};
					float leak_facing = 0.0f;
					for (std::size_t t = 0; t + 2 < idx.size() && !leak; t += 3)
					{
						const float* a = &v[idx[t] * 3];
						const float* b = &v[idx[t + 1] * 3];
						const float* cc = &v[idx[t + 2] * 3];
						float samples[4][3];
						const float weights[4][3] = { { 1.f / 3, 1.f / 3, 1.f / 3 }, { 0.8f, 0.1f, 0.1f }, { 0.1f, 0.8f, 0.1f }, { 0.1f, 0.1f, 0.8f } };
						for (int k = 0; k < 4; k++)
						{
							for (int axis = 0; axis < 3; axis++)
							{
								samples[k][axis] = a[axis] * weights[k][0] + b[axis] * weights[k][1] + cc[axis] * weights[k][2];
							}
						}
						for (const auto& p : samples)
						{
							if (!in_frustum(p))
							{
								continue;
							}
							float d[3] = { eye[0] - p[0], eye[1] - p[1], eye[2] - p[2] };
							const auto len = std::sqrt(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
							if (len < 2.0f)
							{
								continue;
							}
							for (auto& k : d) k /= len;
							bool blocked = false;
							for (const auto& tri : occluders)
							{
								if (tri.object == static_cast<std::uint32_t>(o))
								{
									continue;
								}
								float th;
								if (ray_hits_triangle(p, d, tri, th) && th > 0.5f && th < len - 0.5f)
								{
									blocked = true;
									break;
								}
							}
							if (!blocked)
							{
								leak = true;
								std::memcpy(leak_point, p, sizeof(leak_point));
								const float n[3] = { (b[1] - a[1]) * (cc[2] - a[2]) - (b[2] - a[2]) * (cc[1] - a[1]),
									(b[2] - a[2]) * (cc[0] - a[0]) - (b[0] - a[0]) * (cc[2] - a[2]),
									(b[0] - a[0]) * (cc[1] - a[1]) - (b[1] - a[1]) * (cc[0] - a[0]) };
								leak_facing = -(n[0] * d[0] + n[1] * d[1] + n[2] * d[2]);
								break;
							}
						}
					}

					if (leak)
					{
						false_occlusions++;
						if (leak_facing > 0.0f) front_facing_leaks++;
						if (camera_clearance >= clearance_limit) clear_camera_leaks++;
						if (leak_facing > 0.0f && camera_clearance >= clearance_limit) serious_leaks++;
						if (reported < 60 && leak_facing > 0.0f && camera_clearance >= clearance_limit)
						{
							reported++;
							std::printf("  FALSE OCCLUSION: camera (%g %g %g) clearance %g dir (%g %g %g): object %d id 0x%08X front face visible at (%g %g %g)\n",
								eye[0], eye[1], eye[2], camera_clearance, dir[0], dir[1], dir[2], i, scene.objects[o].user_id,
								leak_point[0], leak_point[1], leak_point[2]);
						}
					}
				}
			}
		}

		std::printf("check: %zu queries, %zu hidden object reports checked, %zu false occlusions: %zu of front faces, "
			"%zu from cameras at least %g units clear of occluders, %zu both (the ones that would show in game)\n",
			queries, checked_hidden, false_occlusions, front_facing_leaks, clear_camera_leaks, static_cast<double>(clearance_limit), serious_leaks);
		Umbra::TomeLoader::freeTome(tome);
		return serious_leaks ? 8 : 0;
	}

	int query_mode(const int argc, char** argv)
	{
		if (argc < 9)
		{
			std::printf("usage: umbra-tomegen query <scene> <tome> x y z yaw pitch [--fov deg] [--jobs N] [--threshold D]\n");
			return 2;
		}

		float eye[3] = { static_cast<float>(std::atof(argv[4])), static_cast<float>(std::atof(argv[5])), static_cast<float>(std::atof(argv[6])) };
		const auto yaw = static_cast<float>(std::atof(argv[7])) * 3.14159265f / 180.0f;
		const auto pitch = static_cast<float>(std::atof(argv[8])) * 3.14159265f / 180.0f;
		float fov_deg = 65.0f;
		int jobs = 4;
		float threshold = 256.0f;
		for (int i = 9; i < argc; i++)
		{
			if (!std::strcmp(argv[i], "--fov") && i + 1 < argc) fov_deg = static_cast<float>(std::atof(argv[++i]));
			else if (!std::strcmp(argv[i], "--jobs") && i + 1 < argc) jobs = std::max(1, std::atoi(argv[++i]));
			else if (!std::strcmp(argv[i], "--threshold") && i + 1 < argc) threshold = static_cast<float>(std::atof(argv[++i]));
		}

		loaded_scene scene;
		if (!load_scene(argv[2], scene))
		{
			return 3;
		}
		std::vector<unsigned char> tome_data;
		if (!read_file(argv[3], tome_data))
		{
			return 3;
		}
		const auto* tome = Umbra::TomeLoader::loadFromBuffer(tome_data.data(), tome_data.size());
		if (!tome || tome->getStatus() != Umbra::Tome::STATUS_OK)
		{
			std::printf("error: tome did not load\n");
			return 4;
		}

		const float dir[3] = { std::cos(pitch) * std::cos(yaw), std::cos(pitch) * std::sin(yaw), -std::sin(pitch) };
		const float up[3] = { 0, 0, 1 };
		const auto fov = fov_deg * 3.14159265f / 180.0f;
		const auto fov_x = 1.0f / std::tan(fov * 0.5f);
		const auto clip = make_world_to_clip(eye, dir, up, 1.0f, 100000.0f, fov_x, fov_x * (16.0f / 9.0f));
		const Umbra::CameraTransform camera(clip, vec3(eye[0], eye[1], eye[2]));

		const auto object_count = tome->getObjectCount();
		std::vector<std::uint32_t> mask((object_count + 31) / 32 + 1), job_mask(mask.size());
		std::vector<Umbra::OcclusionBuffer> buffers(jobs);
		Umbra::Query query(tome);
		for (int job = 0; job < jobs; job++)
		{
			std::fill(job_mask.begin(), job_mask.end(), 0u);
			Umbra::Visibility visibility;
			visibility.setOutputObjectMask(job_mask.data());
			visibility.setOutputBuffer(&buffers[job]);
			const auto error = query.queryPortalVisibility(0, visibility, camera, 0.0f, threshold, nullptr, job, jobs, jobs);
			if (error != Umbra::Query::ERROR_OK)
			{
				std::printf("query job %d failed with %d\n", job, static_cast<int>(error));
				return 5;
			}
			for (std::size_t w = 0; w < mask.size(); w++) mask[w] |= job_mask[w];
		}
		for (int job = 1; job < jobs; job++)
		{
			buffers[0].combine(buffers[job]);
		}
		{
			const auto& buffer = buffers[0];
			std::printf("occlusion buffer %dx%d\n", buffer.getWidth(), buffer.getHeight());
			const float boxes[][6] = {
				{ -816, -816, -16, 816, 816, 0 },
				{ eye[0] - 64, eye[1] - 64, eye[2] - 64, eye[0] + 64, eye[1] + 64, eye[2] + 64 },
				{ eye[0] - 64, eye[1] + 100, -16, eye[0] + 64, eye[1] + 300, 0 },
				{ -100, 100, -16, 100, 300, 0 },
				{ eye[0] - 8, eye[1] + 200, eye[2] - 8, eye[0] + 8, eye[1] + 216, eye[2] + 8 },
			};
			for (const auto& b : boxes)
			{
				float contribution = 0.0f;
				const auto result = buffer.testAABBVisibility(vec3(b[0], b[1], b[2]), vec3(b[3], b[4], b[5]), 0, &contribution);
				std::printf("  box (%g %g %g)-(%g %g %g): %s (contribution %g)\n", b[0], b[1], b[2], b[3], b[4], b[5],
					result == Umbra::OcclusionBuffer::OCCLUDED ? "OCCLUDED" : result == Umbra::OcclusionBuffer::FULLY_VISIBLE ? "fully visible" : "visible", contribution);
			}
		}

		std::vector<std::uint32_t> ids(16);
		int visible = 0;
		for (int i = 0; i < object_count; i++)
		{
			const auto n = tome->getObjectUserIDs(i, ids.data(), 16);
			Umbra::Vector3 mn, mx;
			tome->getObjectBounds(i, mn, mx);
			const bool vis = (mask[i >> 5] >> (i & 31)) & 1;
			visible += vis;
			std::printf("  object %2d id 0x%08X %-7s bounds (%g %g %g)-(%g %g %g)\n", i, n > 0 ? ids[0] : 0u,
				vis ? "visible" : "HIDDEN", mn.v[0], mn.v[1], mn.v[2], mx.v[0], mx.v[1], mx.v[2]);
		}
		std::printf("camera (%g %g %g) dir (%g %g %g): %d of %d visible\n", eye[0], eye[1], eye[2], dir[0], dir[1], dir[2], visible, object_count);
		Umbra::TomeLoader::freeTome(tome);
		return 0;
	}

	int generate(const int argc, char** argv)
	{
		if (argc < 4)
		{
			std::printf("usage: umbra-tomegen generate <scene> <out.tome> [--log <file>] [--threads N] [--verify]\n");
			return 2;
		}

		const char* scene_path = argv[2];
		const char* tome_path = argv[3];
		const char* log_path = nullptr;
		int threads = 0;
		bool verify = false;
		for (int i = 4; i < argc; i++)
		{
			if (!std::strcmp(argv[i], "--log") && i + 1 < argc)
			{
				log_path = argv[++i];
			}
			else if (!std::strcmp(argv[i], "--threads") && i + 1 < argc)
			{
				threads = std::atoi(argv[++i]);
			}
			else if (!std::strcmp(argv[i], "--verify"))
			{
				verify = true;
			}
			else
			{
				std::printf("error: unknown argument \"%s\"\n", argv[i]);
				return 2;
			}
		}

		file_logger logger(log_path);

		const auto started = std::chrono::steady_clock::now();
		loaded_scene scene;
		if (!load_scene(scene_path, scene))
		{
			return 3;
		}

		Umbra::Vector3 bounds_min, bounds_max;
		scene.scene->getBounds(bounds_min, bounds_max);
		std::printf("scene: %u models, %zu triangles, %u objects (%zu occluders, %zu targets), %u view volumes, "
			"%u seed points, bounds (%g %g %g) - (%g %g %g)\n",
			scene.header.model_count, scene.triangle_count, scene.header.object_count, scene.occluder_count,
			scene.target_count, scene.header.view_volume_count, scene.header.seed_point_count,
			bounds_min.v[0], bounds_min.v[1], bounds_min.v[2], bounds_max.v[0], bounds_max.v[1], bounds_max.v[2]);

		const auto& params = scene.header.params;
		std::printf("params: smallest occluder %g, smallest hole %g, backface limit %g%%, cluster size %g, "
			"group cost %g, min accurate distance %g, output flags 0x%X\n",
			params.smallest_occluder, params.smallest_hole, params.backface_limit, params.cluster_size,
			params.object_group_cost, params.minimum_accurate_distance, params.output_flags);

		Umbra::ComputationParams computation_params;
		computation_params.setParam(Umbra::ComputationParams::SMALLEST_OCCLUDER, params.smallest_occluder);
		computation_params.setParam(Umbra::ComputationParams::SMALLEST_HOLE, params.smallest_hole);
		computation_params.setParam(Umbra::ComputationParams::BACKFACE_LIMIT, params.backface_limit);
		if (params.cluster_size > 0.0f)
		{
			computation_params.setParam(Umbra::ComputationParams::CLUSTER_SIZE, params.cluster_size);
		}
		computation_params.setParam(Umbra::ComputationParams::OBJECT_GROUP_COST, params.object_group_cost);
		if (params.minimum_accurate_distance > 0.0f)
		{
			computation_params.setParam(Umbra::ComputationParams::MINIMUM_ACCURATE_DISTANCE, params.minimum_accurate_distance);
		}
		computation_params.setParam(Umbra::ComputationParams::OUTPUT_FLAGS, static_cast<std::uint32_t>(params.output_flags));

		if (threads <= 0)
		{
			threads = static_cast<int>(params.thread_count);
		}
		if (threads <= 0)
		{
			threads = static_cast<int>(std::thread::hardware_concurrency());
		}
		if (threads <= 0)
		{
			threads = 1;
		}

		std::string temp_path = tome_path;
		const auto slash = temp_path.find_last_of("\\/");
		temp_path = slash == std::string::npos ? "." : temp_path.substr(0, slash);

		Umbra::LocalComputation::Params local_params;
		local_params.scene = scene.scene;
		local_params.computationParams = &computation_params;
		local_params.logger = &logger;
		local_params.numThreads = threads;
		local_params.silent = false;
		local_params.tempPath = temp_path.c_str();
		local_params.tempFilePrefix = "umbra-tomegen-";

		Umbra::g_reachabilityAnalysisThreshold = 0.0f;

		auto* computation = Umbra::LocalComputation::create(local_params);
		if (!computation)
		{
			std::printf("error: Umbra::LocalComputation::create failed\n");
			return 4;
		}

		const auto result = computation->waitForResult();
		const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
		if (result.error != Umbra::Computation::ERROR_OK)
		{
			std::printf("error: computation failed after %.1fs: error %d \"%s\"\n", elapsed,
				static_cast<int>(result.error), result.errorStr);
			computation->release();
			return 5;
		}

		std::vector<unsigned char> tome(reinterpret_cast<const unsigned char*>(result.tome),
			reinterpret_cast<const unsigned char*>(result.tome) + result.tomeSize);
		computation->release();

		if (!write_file(tome_path, tome.data(), tome.size()))
		{
			std::printf("error: cannot write \"%s\"\n", tome_path);
			return 6;
		}

		std::printf("generated %u byte tome in %.1fs using %d threads\n", result.tomeSize, elapsed, threads);

		auto ok = true;
		if (verify)
		{
			ok = verify_tome(tome, scene);
		}

		scene.scene->release();
		return ok ? 0 : 7;
	}

	struct tome_header
	{
		std::uint32_t version_magic;
		std::uint32_t crc32;
		std::uint32_t size;
		float lod_base_distance;
		std::uint32_t flags;
		float tree_min[3];
		float tree_max[3];
		std::uint32_t tile_tree_node_count_map_width;
		std::uint32_t tile_tree_data;
		std::uint32_t tile_tree_map;
		std::uint32_t tile_tree_num_split_values;
		std::uint32_t tile_tree_split_values;
		std::int32_t num_objects;
		std::uint32_t obj_bounds;
		std::uint32_t obj_distances;
		std::uint32_t user_id_starts;
		std::uint32_t user_ids;
		std::uint32_t list_widths;
		std::uint32_t object_lists;
		std::int32_t object_list_size;
		std::uint32_t cluster_lists;
		std::int32_t cluster_list_size;
		std::int32_t num_gates;
		std::uint32_t gate_index_map;
		std::uint32_t gate_vertices;
		std::int32_t num_gate_vertices;
		std::uint32_t gate_indices;
		std::int32_t num_clusters;
		std::uint32_t clusters;
		std::uint32_t cluster_portals;
		std::uint32_t cell_starts;
		std::int32_t num_leaf_tiles;
		std::int32_t num_tiles;
		std::int32_t bits_per_slot_path;
		std::uint32_t slot_paths;
		std::uint32_t tile_lod_levels;
		std::uint32_t tiles;
		std::uint32_t tile_matching_data;
		std::uint32_t matching_trees;
		std::int32_t num_matching_trees;
		std::int32_t num_tomes;
		std::uint32_t tome_cluster_starts;
		std::uint32_t tome_cluster_portal_starts;
		char computation_string[128];
		std::uint32_t object_depthmaps;
		std::uint32_t depthmap_faces;
		std::uint32_t depthmap_palettes;
		std::int32_t num_faces;
		std::uint32_t tile_portal_expands;
		float bounds_min[3];
		float bounds_max[3];
		float cluster_coord_scale;
		std::int32_t pad;
	};
	static_assert(sizeof(tome_header) == 368);

	struct tile_header
	{
		float tree_min[3];
		float tree_max[3];
		std::uint32_t view_tree_node_count_map_width;
		std::uint32_t view_tree_data;
		std::uint32_t view_tree_map;
		std::uint32_t view_tree_num_split_values;
		std::uint32_t view_tree_split_values;
		std::int32_t size_and_flags;
		float portal_expand;
		std::int32_t num_cells_and_clusters;
		std::uint32_t cells;
		std::uint32_t portals;
	};
	static_assert(sizeof(tile_header) == 64);

	void inspect_tome(const unsigned char* data, const std::size_t size)
	{
		tome_header header{};
		std::memcpy(&header, data, std::min(sizeof(header), size));
		const auto version = header.version_magic & 0xFFFF;

		std::printf("  version 0x%04X (magic 0x%08X) size %u (buffer %zu) crc 0x%08X flags 0x%X lodBase %g\n",
			version, header.version_magic, header.size, size, header.crc32, header.flags, header.lod_base_distance);
		std::printf("  tree (%g %g %g) - (%g %g %g)\n", header.tree_min[0], header.tree_min[1], header.tree_min[2],
			header.tree_max[0], header.tree_max[1], header.tree_max[2]);
		if (version >= 0x14)
		{
			std::printf("  bounds (%g %g %g) - (%g %g %g) clusterCoordScale %g tilePortalExpands @%u\n",
				header.bounds_min[0], header.bounds_min[1], header.bounds_min[2], header.bounds_max[0],
				header.bounds_max[1], header.bounds_max[2], header.cluster_coord_scale, header.tile_portal_expands);
		}
		std::printf("  objects %d (bounds @%u distances @%u userIDStarts @%u userIDs @%u)\n", header.num_objects,
			header.obj_bounds, header.obj_distances, header.user_id_starts, header.user_ids);
		std::printf("  object lists: %d pairs, index bits %u, run bits %u; cluster lists %d pairs\n",
			header.object_list_size, header.list_widths & 31, (header.list_widths >> 5) & 31, header.cluster_list_size);
		std::printf("  tiles %d (leaf %d), tile tree nodes %u, slot path bits %d\n", header.num_tiles,
			header.num_leaf_tiles, header.tile_tree_node_count_map_width >> 5, header.bits_per_slot_path);
		std::printf("  clusters %d, gates %d (%d vertices), tomes %d, faces %d\n", header.num_clusters,
			header.num_gates, header.num_gate_vertices, header.num_tomes, header.num_faces);
		std::printf("  computation \"%.128s\"\n", header.computation_string);

		if (header.num_objects > 0 && header.user_ids && header.user_ids < size)
		{
			std::size_t type_counts[8]{};
			std::size_t id_count = 0;
			const auto* ids = reinterpret_cast<const std::uint32_t*>(data + header.user_ids);
			if (header.user_id_starts && header.user_id_starts + (header.num_objects + 1) * 4 <= size)
			{
				const auto* starts = reinterpret_cast<const std::uint32_t*>(data + header.user_id_starts);
				id_count = starts[header.num_objects];
			}
			else
			{
				id_count = header.num_objects;
			}
			if (header.user_ids + id_count * 4 <= size)
			{
				for (std::size_t i = 0; i < id_count; i++)
				{
					type_counts[(ids[i] >> 28) & 7]++;
				}
				std::printf("  user IDs %zu: surfaces %zu, smodels %zu, type2 %zu, volumetrics %zu, lights %zu, "
					"probes %zu, decals %zu, type7 %zu\n", id_count, type_counts[0], type_counts[1], type_counts[2],
					type_counts[3], type_counts[4], type_counts[5], type_counts[6], type_counts[7]);
			}
		}

		const auto node_count = header.tile_tree_node_count_map_width >> 5;
		if (header.tiles && header.tiles + node_count * 4 <= size)
		{
			const auto* tile_offsets = reinterpret_cast<const std::uint32_t*>(data + header.tiles);
			std::size_t leaf_tiles = 0, cells = 0, portals = 0, max_cells = 0, max_portals = 0, empty_cells = 0;
			for (std::uint32_t i = 0; i < node_count; i++)
			{
				if (!tile_offsets[i] || tile_offsets[i] + sizeof(tile_header) > size)
				{
					continue;
				}
				tile_header tile{};
				std::memcpy(&tile, data + tile_offsets[i], sizeof(tile));
				const auto flags = static_cast<std::uint32_t>(tile.size_and_flags) & 0xFF;
				if (!(flags & 1))
				{
					continue;
				}
				leaf_tiles++;
				const auto tile_cells = static_cast<std::uint32_t>(tile.num_cells_and_clusters) & 0xFFFF;
				cells += tile_cells;
				max_cells = std::max<std::size_t>(max_cells, tile_cells);
				if (tile.cells && tile_offsets[i] + tile.cells + tile_cells * 36 <= size)
				{
					const auto* cell = reinterpret_cast<const std::uint32_t*>(data + tile_offsets[i] + tile.cells);
					for (std::uint32_t c = 0; c < tile_cells; c++, cell += 9)
					{
						portals += cell[1];
						max_portals = std::max<std::size_t>(max_portals, cell[1]);
						empty_cells += cell[1] == 0 ? 1 : 0;
					}
				}
			}
			std::printf("  leaf tiles %zu: %zu cells (max %zu per tile), %zu cell portals (max %zu per cell), "
				"%zu cells without portals\n", leaf_tiles, cells, max_cells, portals, max_portals, empty_cells);
		}
	}

	int inspect(const int argc, char** argv)
	{
		if (argc < 3)
		{
			std::printf("usage: umbra-tomegen inspect <tome | gfxmap>\n");
			return 2;
		}

		std::vector<unsigned char> data;
		if (!read_file(argv[2], data))
		{
			std::printf("error: cannot read \"%s\"\n", argv[2]);
			return 3;
		}

		std::size_t found = 0;
		for (std::size_t offset = 0; offset + sizeof(tome_header) <= data.size(); offset++)
		{
			std::uint32_t magic;
			std::memcpy(&magic, data.data() + offset, 4);
			if ((magic & 0xFFFF0000) != 0xD6000000 || (magic & 0xFFFF) < 0x12 || (magic & 0xFFFF) > 0x14)
			{
				continue;
			}
			std::uint32_t tome_size;
			std::memcpy(&tome_size, data.data() + offset + 8, 4);
			if (tome_size < sizeof(tome_header) || offset + tome_size > data.size())
			{
				continue;
			}

			std::printf("tome at offset 0x%zX:\n", offset);
			inspect_tome(data.data() + offset, tome_size);
			found++;
			offset += tome_size - 1;
		}

		if (!found)
		{
			std::printf("no tome found in \"%s\"\n", argv[2]);
			return 4;
		}
		return 0;
	}
}

int main(const int argc, char** argv)
{
	if (argc >= 2 && !std::strcmp(argv[1], "generate"))
	{
		return generate(argc, argv);
	}
	if (argc >= 2 && !std::strcmp(argv[1], "inspect"))
	{
		return inspect(argc, argv);
	}
	if (argc >= 2 && !std::strcmp(argv[1], "check"))
	{
		return check(argc, argv);
	}
	if (argc >= 2 && !std::strcmp(argv[1], "query"))
	{
		return query_mode(argc, argv);
	}

	std::printf("usage:\n  umbra-tomegen generate <scene> <out.tome> [--log <file>] [--threads N] [--verify]\n"
		"  umbra-tomegen inspect <tome | gfxmap>\n"
		"  umbra-tomegen check <scene> <tome> [--cameras N] [--seed N]\n");
	return 2;
}
