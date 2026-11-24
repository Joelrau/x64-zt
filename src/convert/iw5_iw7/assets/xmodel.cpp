#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "xsurface.hpp"
#include "clipmap_collision.hpp"

#include <iw7/assets/xmodel.hpp>
#include <iw7/assets/physics_asset.hpp>
#include <iw7/common/havok_builder.hpp>

namespace zonetool::iw5::converter::iw7
{
	namespace xmodel
	{
		namespace
		{
			namespace source = zonetool::iw5;
			namespace target = zonetool::iw7;

			constexpr target::SurfaceFlags surface_types[31] =
			{
				target::SURFACE_FLAG_DEFAULT, // 0x1406FA2F0
				target::SURFACE_FLAG_BARK,
				target::SURFACE_FLAG_BRICK,
				target::SURFACE_FLAG_CARPET_SOLID,
				target::SURFACE_FLAG_CLOTH,
				target::SURFACE_FLAG_CONCRETE_DRY,
				target::SURFACE_FLAG_DIRT,
				target::SURFACE_FLAG_FLESH,
				target::SURFACE_FLAG_FOLIAGE_DRY,
				target::SURFACE_FLAG_GLASS_PANE,
				target::SURFACE_FLAG_GRASS_SHORT,
				target::SURFACE_FLAG_GRAVEL,
				target::SURFACE_FLAG_ICE_SOLID,
				target::SURFACE_FLAG_METAL_THICK,
				target::SURFACE_FLAG_MUD,
				target::SURFACE_FLAG_PAPER,
				target::SURFACE_FLAG_PLASTER,
				target::SURFACE_FLAG_ROCK,
				target::SURFACE_FLAG_SAND,
				target::SURFACE_FLAG_SNOW,
				target::SURFACE_FLAG_WATER,
				target::SURFACE_FLAG_WOOD_SOLID,
				target::SURFACE_FLAG_ASPHALT_DRY,
				target::SURFACE_FLAG_CERAMIC,
				target::SURFACE_FLAG_PLASTIC,
				target::SURFACE_FLAG_RUBBER,
				target::SURFACE_FLAG_CUSHION,
				target::SURFACE_FLAG_FRUIT,
				target::SURFACE_FLAG_METAL_PAINTED,
				target::SURFACE_FLAG_RIOTSHIELD,
				target::SURFACE_FLAG_SLUSH,
			};

			int to_iw7_surface_flags(const int flags)
			{
				const auto type = (static_cast<unsigned int>(flags) >> 20) & 0x1Fu;
				int result = surface_types[type < std::size(surface_types) ? type : 0];

				const auto convert = [&](const unsigned int from, const unsigned int to)
				{
					if (from != 0 && (static_cast<unsigned int>(flags) & from) == from)
					{
						result |= static_cast<int>(to);
					}
				};

#define CONVERT_FLAG(__name__) convert(source::SURF_FLAG_##__name__, target::SURFACE_FLAG_##__name__)
				CONVERT_FLAG(CLIPMISSILE);
				CONVERT_FLAG(AI_NOSIGHT);
				CONVERT_FLAG(CLIPSHOT);
				CONVERT_FLAG(PLAYERCLIP);
				CONVERT_FLAG(MONSTERCLIP);
				CONVERT_FLAG(AICLIPALLOWDEATH);
				CONVERT_FLAG(VEHICLECLIP);
				CONVERT_FLAG(ITEMCLIP);
				CONVERT_FLAG(NODROP);
				CONVERT_FLAG(NONSOLID);
				CONVERT_FLAG(DETAIL);
				CONVERT_FLAG(STRUCTURAL);
				CONVERT_FLAG(PORTAL);
				CONVERT_FLAG(CANSHOOTCLIP);
				CONVERT_FLAG(ORIGIN);
				CONVERT_FLAG(SKY);
				CONVERT_FLAG(NOCASTSHADOW);
				CONVERT_FLAG(PHYSICSGEOM);
				CONVERT_FLAG(LIGHTPORTAL);
				CONVERT_FLAG(OUTDOORBOUNDS);
				CONVERT_FLAG(SLICK);
				CONVERT_FLAG(NOIMPACT);
				CONVERT_FLAG(NOMARKS);
				CONVERT_FLAG(NOPENETRATE);
				CONVERT_FLAG(LADDER);
				CONVERT_FLAG(NODAMAGE);
				CONVERT_FLAG(MANTLEON);
				CONVERT_FLAG(MANTLEOVER);
				CONVERT_FLAG(STAIRS);
				CONVERT_FLAG(SOFT);
				CONVERT_FLAG(NOSTEPS);
				CONVERT_FLAG(NODRAW);
				CONVERT_FLAG(NOLIGHTMAP);
				CONVERT_FLAG(NODLIGHT);
#undef CONVERT_FLAG

				return result;
			}
		}

		int convert_surface_flags(const int flags)
		{
			return to_iw7_surface_flags(flags);
		}

		namespace
		{
			bool collision_triangle_corners(const source::XModelCollTri_s& triangle, float (&corners)[3][3])
			{
				const auto cross = [](const float* a, const float* b, float (&out)[3])
				{
					out[0] = a[1] * b[2] - a[2] * b[1];
					out[1] = a[2] * b[0] - a[0] * b[2];
					out[2] = a[0] * b[1] - a[1] * b[0];
				};

				const float* rows[3] = {triangle.plane, triangle.svec, triangle.tvec};
				float c12[3], c20[3], c01[3];
				cross(rows[1], rows[2], c12);
				cross(rows[2], rows[0], c20);
				cross(rows[0], rows[1], c01);

				const auto determinant = rows[0][0] * c12[0] + rows[0][1] * c12[1] + rows[0][2] * c12[2];
				if (std::fabs(determinant) < 1e-12f)
				{
					return false;
				}

				constexpr float barycentric[3][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}};
				for (auto corner = 0; corner < 3; corner++)
				{
					const float right_hand_side[3] = {
						triangle.plane[3],
						barycentric[corner][0] + triangle.svec[3],
						barycentric[corner][1] + triangle.tvec[3],
					};

					for (auto axis = 0; axis < 3; axis++)
					{
						corners[corner][axis] = (right_hand_side[0] * c12[axis] + right_hand_side[1] * c20[axis]
							+ right_hand_side[2] * c01[axis]) / determinant;
					}
				}

				return true;
			}

			std::map<std::string, float> dynamic_box_requests;

			zonetool::iw7::havok::builder::polytope box_polytope(const source::Bounds& bounds)
			{
				zonetool::iw7::havok::builder::polytope box{};
				float mins[3], maxs[3];
				for (auto axis = 0; axis < 3; axis++)
				{
					mins[axis] = bounds.midPoint[axis] - bounds.halfSize[axis];
					maxs[axis] = bounds.midPoint[axis] + bounds.halfSize[axis];
				}

				for (auto i = 0; i < 8; i++)
				{
					box.verts.push_back({(i & 1) ? maxs[0] : mins[0], (i & 2) ? maxs[1] : mins[1], (i & 4) ? maxs[2] : mins[2]});
				}

				const struct { int axis; float sign; std::uint8_t corners[4]; } faces[6] =
				{
					{0, -1.0f, {0, 4, 6, 2}}, {0, 1.0f, {1, 3, 7, 5}},
					{1, -1.0f, {0, 1, 5, 4}}, {1, 1.0f, {2, 6, 7, 3}},
					{2, -1.0f, {0, 2, 3, 1}}, {2, 1.0f, {4, 5, 7, 6}},
				};

				for (const auto& face : faces)
				{
					zonetool::iw7::havok::builder::polytope_face new_face{};
					new_face.plane[face.axis] = face.sign;
					new_face.plane[3] = face.sign > 0.0f ? maxs[face.axis] : -mins[face.axis];
					new_face.indices.assign(face.corners, face.corners + 4);
					box.faces.emplace_back(std::move(new_face));
				}

				return box;
			}

			bool environment_off(const char* name)
			{
				const auto value = std::getenv(name);
				return value && value[0] == '0';
			}

			target::PhysicsAsset* make_physics_asset(const char* name, const std::vector<std::uint8_t>& blob, utils::memory::allocator& allocator)
			{
				const auto physics_asset = allocator.allocate<target::PhysicsAsset>();
				physics_asset->name = allocator.duplicate_string(name);
				physics_asset->havokDataSize = static_cast<unsigned int>(blob.size());
				physics_asset->havokData = allocator.allocate_array<char>(blob.size());
				std::memcpy(physics_asset->havokData, blob.data(), blob.size());
				physics_asset->numRigidBodies = 1;
				physics_asset->numSFXEventAssets = 1;
				physics_asset->sfxEventAssets = allocator.allocate_array<target::PhysicsSFXEventAsset*>(1);
				physics_asset->numVFXEventAssets = 1;
				physics_asset->vfxEventAssets = allocator.allocate_array<target::PhysicsVFXEventAsset*>(1);
				return physics_asset;
			}

			target::PhysicsAsset* generate_dynamic_physics_asset(source::XModel* asset, utils::memory::allocator& allocator)
			{
				namespace builder = zonetool::iw7::havok::builder;

				if (environment_off("ZT_MODEL_DYNAMIC"))
				{
					return nullptr;
				}

				const auto collmap = asset->physCollmap.get();
				float box_mass = 0.0f;
				const auto box_fallback = !collmap && wants_dynamic_box(asset->name.get(), &box_mass);
				if (!collmap && !box_fallback)
				{
					return nullptr;
				}

				builder::dynamic_physics_asset_input dynamic{};
				if (box_fallback)
				{
					dynamic.convexes.emplace_back(box_polytope(asset->bounds));
				}

				for (const auto& hull : collision::extract_phys_collmap(collmap))
				{
					builder::polytope convex{};
					convex.verts = hull.verts;
					for (const auto& face : hull.faces)
					{
						builder::polytope_face new_face{};
						std::memcpy(new_face.plane, face.plane, sizeof(face.plane));
						new_face.indices.assign(face.indices.begin(), face.indices.end());
						convex.faces.emplace_back(std::move(new_face));
					}
					dynamic.convexes.emplace_back(std::move(convex));
				}

				if (dynamic.convexes.empty())
				{
					ZONETOOL_WARNING("XModel \"%s\": physCollmap produced no hulls, using the static collision", asset->name.get());
					return nullptr;
				}

				auto mass_scale = 0.3f;
				if (const auto scale_value = std::getenv("ZT_MODEL_MASS_SCALE"))
				{
					char* end = nullptr;
					const auto value = std::strtof(scale_value, &end);
					if (end != scale_value && value > 0.0f)
					{
						mass_scale = value;
					}
				}

				const auto preset = asset->physPreset.get();
				dynamic.mass = box_fallback && box_mass > 0.0f ? box_mass * mass_scale
					: preset && preset->mass > 0.0f ? preset->mass * mass_scale : 5.0f;
				dynamic.mass = std::max(dynamic.mass, 1.0f);

				if (asset->collSurfs && asset->numCollSurfs > 0)
				{
					dynamic.material_crc = collision::iw7_material_crc(asset->collSurfs.get()[0].surfFlags);
				}

				const auto blob = builder::build_dynamic_physics_asset(dynamic);
				if (blob.empty())
				{
					return nullptr;
				}

				ZONETOOL_INFO("XModel \"%s\": dynamic physics asset from %s (%zu hulls, mass %.2f)", asset->name.get(),
					box_fallback ? "a bounds box" : "the physCollmap", dynamic.convexes.size(), dynamic.mass);
				return make_physics_asset(asset->name.get(), blob, allocator);
			}

			zonetool::iw7::havok::builder::mesh_input collision_mesh(source::XModel* asset, const float model_scale)
			{
				namespace builder = zonetool::iw7::havok::builder;

				constexpr auto contents_solid = 0x1;
				constexpr auto contents_foliage = 0x2;

				builder::mesh_input mesh{};

				const auto add_triangle = [&mesh, model_scale](const float (&corners)[3][3], const float* normal, const int contents,
					const unsigned int material_crc, const unsigned short tag, const std::uint64_t user_data)
				{
					builder::triangle triangle{};
					for (auto c = 0; c < 3; c++)
					{
						for (auto k = 0; k < 3; k++)
						{
							triangle.verts[c][k] = corners[c][k] * model_scale;
						}
					}

					float edge_a[3], edge_b[3];
					for (auto k = 0; k < 3; k++)
					{
						edge_a[k] = triangle.verts[1][k] - triangle.verts[0][k];
						edge_b[k] = triangle.verts[2][k] - triangle.verts[0][k];
					}

					const float cross[3] = {
						edge_a[1] * edge_b[2] - edge_a[2] * edge_b[1],
						edge_a[2] * edge_b[0] - edge_a[0] * edge_b[2],
						edge_a[0] * edge_b[1] - edge_a[1] * edge_b[0],
					};

					if (cross[0] * cross[0] + cross[1] * cross[1] + cross[2] * cross[2] == 0.0f)
					{
						return;
					}

					if (normal && cross[0] * normal[0] + cross[1] * normal[1] + cross[2] * normal[2] < 0.0f)
					{
						for (auto k = 0; k < 3; k++)
						{
							std::swap(triangle.verts[1][k], triangle.verts[2][k]);
						}
					}

					triangle.contents = contents;
					triangle.material_crc = material_crc;
					triangle.surface_tag = tag;
					triangle.user_data = user_data;
					mesh.triangles.emplace_back(triangle);
				};

				for (auto i = 0; i < asset->numCollSurfs; i++)
				{
					const auto& surface = asset->collSurfs.get()[i];
					if (!surface.collTris.value || surface.numCollTris <= 0 || (surface.contents & ~contents_foliage) == 0)
					{
						continue;
					}

					const auto material_crc = collision::iw7_material_crc(surface.surfFlags);
					const auto user_data = static_cast<std::uint64_t>(static_cast<std::uint32_t>(to_iw7_surface_flags(surface.surfFlags)));

					for (auto j = 0; j < surface.numCollTris; j++)
					{
						const auto& collision_triangle = surface.collTris.get()[j];
						float corners[3][3];
						if (collision_triangle_corners(collision_triangle, corners))
						{
							add_triangle(corners, collision_triangle.plane, surface.contents, material_crc, static_cast<unsigned short>(i), user_data);
						}
					}
				}

				const auto collmap = asset->physCollmap.get();
				if (mesh.triangles.empty() && collmap)
				{
					const auto contents = asset->contents ? asset->contents : contents_solid;
					const auto user_data = (asset->collSurfs && asset->numCollSurfs > 0)
						? static_cast<std::uint64_t>(static_cast<std::uint32_t>(to_iw7_surface_flags(asset->collSurfs.get()[0].surfFlags)))
						: 0ull;

					for (const auto& hull : collision::extract_phys_collmap(collmap))
					{
						for (const auto& face : hull.faces)
						{
							for (std::size_t i = 2; i < face.indices.size(); i++)
							{
								const std::size_t corner_index[3] = {face.indices[0], face.indices[i - 1], face.indices[i]};
								if (corner_index[0] >= hull.verts.size() || corner_index[1] >= hull.verts.size()
									|| corner_index[2] >= hull.verts.size())
								{
									continue;
								}

								float corners[3][3];
								for (auto c = 0; c < 3; c++)
								{
									for (auto k = 0; k < 3; k++)
									{
										corners[c][k] = hull.verts[corner_index[c]][k];
									}
								}
								add_triangle(corners, nullptr, contents, 0x1AB7BC33u, 0, user_data);
							}
						}
					}
				}

				return mesh;
			}

			unsigned int static_body_contents()
			{
				const auto value = std::getenv("ZT_MODELS_SOLID");
				return value && value[0] == '1' ? 0x32180u : 0x3180u;
			}

			target::PhysicsAsset* generate_static_physics_asset(source::XModel* asset, const zonetool::iw7::havok::builder::mesh_input& mesh,
				utils::memory::allocator& allocator)
			{
				namespace builder = zonetool::iw7::havok::builder;

				if (mesh.triangles.empty())
				{
					return nullptr;
				}

				builder::physics_asset_input input{};
				input.body_name = asset->name.get();
				input.body_contents = static_body_contents();

				const auto blob = builder::build_model_physics_asset(mesh, input);
				return blob.empty() ? nullptr : make_physics_asset(asset->name.get(), blob, allocator);
			}

			void generate_physics_lod(source::XModel* asset, const zonetool::iw7::havok::builder::mesh_input& mesh,
				target::XModel* new_asset, utils::memory::allocator& allocator)
			{
				namespace builder = zonetool::iw7::havok::builder;

				if (environment_off("ZT_MODEL_PHYSICS_LOD") || mesh.triangles.empty() || !new_asset->physicsAsset)
				{
					return;
				}

				const auto bone = asset->numBones > 0 && asset->boneNames
					? SL_ConvertToString(static_cast<std::uint32_t>(asset->boneNames.get()[0])) : nullptr;
				if (!bone || !*bone)
				{
					return;
				}

				std::string bone_name = bone;
				std::transform(bone_name.begin(), bone_name.end(), bone_name.begin(),
					[](const unsigned char c) { return static_cast<char>(std::tolower(c)); });

				auto lod_mesh = mesh;
				for (auto& triangle : lod_mesh.triangles)
				{
					triangle.contents |= static_cast<int>(static_body_contents());
				}

				const auto blob = builder::build_model_physics_lod(lod_mesh, bone_name);
				if (blob.empty())
				{
					return;
				}

				new_asset->physicsLODDataSize = static_cast<unsigned int>(blob.size());
				new_asset->physicsLODData = allocator.allocate_array<char>(blob.size());
				std::memcpy(new_asset->physicsLODData, blob.data(), blob.size());
				new_asset->physicsLODDataNameCount = 1;
				new_asset->physicsLODDataNames = allocator.allocate_array<target::scr_string_t>(1);
				new_asset->physicsLODDataNames[0] = static_cast<target::scr_string_t>(SL_GetString((std::string(asset->name.get()) + "_lod0").data()));
			}
		}

		void request_dynamic_box(const std::string& model, const float mass)
		{
			dynamic_box_requests[model] = mass;
		}

		bool wants_dynamic_box(const std::string& model, float* mass)
		{
			const auto request = dynamic_box_requests.find(model);
			if (request == dynamic_box_requests.end())
			{
				return false;
			}

			if (mass)
			{
				*mass = request->second;
			}
			return true;
		}

		target::XModel* convert(source::XModel* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<target::XModel>();

			new_asset->name = asset->name.get();
			new_asset->numBones = asset->numBones;
			new_asset->numRootBones = asset->numRootBones;
			new_asset->numsurfs = asset->numsurfs;
			new_asset->scale = asset->scale;
			static_assert(sizeof(new_asset->noScalePartBits) == sizeof(asset->noScalePartBits) + sizeof(int) * 2);
			std::memcpy(new_asset->noScalePartBits, asset->noScalePartBits, sizeof(asset->noScalePartBits));

			new_asset->boneNames = allocator.allocate_array<target::scr_string_t>(asset->numBones);
			for (auto i = 0; i < asset->numBones; i++)
			{
				new_asset->boneNames[i] = static_cast<target::scr_string_t>(asset->boneNames.get()[i]);
			}

			static_assert(sizeof(short[4]) == sizeof(target::XModelAngle));
			static_assert(sizeof(float[3]) == sizeof(target::XModelTagPos));
			static_assert(sizeof(source::DObjAnimMat) == sizeof(target::DObjAnimMat));
			new_asset->parentList = asset->parentList.get();
			new_asset->tagAngles = reinterpret_cast<target::XModelAngle*>(asset->quats.get());
			new_asset->tagPositions = reinterpret_cast<target::XModelTagPos*>(asset->trans.get());
			new_asset->partClassification = asset->partClassification.get();
			new_asset->baseMat = reinterpret_cast<target::DObjAnimMat*>(asset->baseMat.get());

			new_asset->materialHandles = allocator.allocate_array<target::Material*>(asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				const auto material = asset->materialHandles.get()[i].get();
				if (material)
				{
					new_asset->materialHandles[i] = allocator.allocate<target::Material>();
					new_asset->materialHandles[i]->name = allocator.duplicate_string(material->info.name.get());
				}
			}

			for (auto& lod : new_asset->lodInfo)
			{
				lod.dist = 1000000.0f;
			}

			for (auto i = 0; i < asset->numLods; i++)
			{
				const auto& lod = asset->lodInfo[i];
				auto& new_lod = new_asset->lodInfo[i];

				new_lod.dist = lod.dist;
				new_lod.numsurfs = lod.numsurfs;
				new_lod.surfIndex = lod.surfIndex;
				new_lod.modelSurfs = allocator.allocate<target::XModelSurfs>();
				new_lod.modelSurfs->name = lod.modelSurfs->name.get();
				std::memcpy(new_lod.partBits, lod.partBits, sizeof(lod.partBits));
			}

			new_asset->maxLoadedLod = asset->maxLoadedLod;
			new_asset->numLods = asset->numLods;
			new_asset->collLod = asset->collLod;
			new_asset->flags = asset->flags;

			new_asset->numCollSurfs = static_cast<unsigned short>(asset->numCollSurfs);
			new_asset->collSurfs = allocator.allocate_array<target::XModelCollSurf_s>(asset->numCollSurfs);
			for (auto i = 0; i < asset->numCollSurfs; i++)
			{
				const auto& surface = asset->collSurfs.get()[i];
				std::memcpy(&new_asset->collSurfs[i].bounds, &surface.bounds, sizeof(surface.bounds));
				new_asset->collSurfs[i].boneIdx = surface.boneIdx;
				new_asset->collSurfs[i].contents = surface.contents;
				new_asset->collSurfs[i].surfFlags = to_iw7_surface_flags(surface.surfFlags);
			}

			const auto mesh = collision_mesh(asset, 0.03125f);
			new_asset->physicsAsset = generate_dynamic_physics_asset(asset, allocator);
			if (!new_asset->physicsAsset)
			{
				new_asset->physicsAsset = generate_static_physics_asset(asset, mesh, allocator);
			}
			generate_physics_lod(asset, mesh, new_asset, allocator);

			new_asset->contents = asset->contents;

			static_assert(sizeof(source::XBoneInfo) == sizeof(target::XBoneInfo));
			new_asset->boneInfo = allocator.allocate_array<target::XBoneInfo>(asset->numBones);
			std::memcpy(new_asset->boneInfo, asset->boneInfo.get(), sizeof(target::XBoneInfo) * asset->numBones);

			new_asset->radius = asset->radius;
			std::memcpy(&new_asset->bounds, &asset->bounds, sizeof(asset->bounds));
			new_asset->memUsage = asset->memUsage;

			new_asset->invHighMipRadius = allocator.allocate_array<unsigned short>(asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				new_asset->invHighMipRadius[i] = 0xFFFF;
			}

			new_asset->hasLods = asset->numLods != 0;
			new_asset->shadowCutoffLod = 6;
			new_asset->characterCollBoundsType = target::CharCollBoundsType_Human;
			new_asset->unknownIndex = 0xFF;
			new_asset->unknownIndex2 = 0xFF;

			return new_asset;
		}

		void dump(source::XModel* asset)
		{
			utils::memory::allocator allocator;
			const auto converted = convert(asset, allocator);
			target::xmodel::dump(converted);

			if (converted->physicsAsset)
			{
				target::physics_asset::dump(converted->physicsAsset);
			}

			auto vertex_lighting_index = 0u;
			for (auto i = 0; i < asset->numLods; i++)
			{
				const auto surfaces = asset->lodInfo[i].modelSurfs.get();
				if (!surfaces)
				{
					continue;
				}

				xsurface::dump(surfaces, vertex_lighting_index);
				for (auto j = 0; j < surfaces->numsurfs; j++)
				{
					vertex_lighting_index += surfaces->surfs.get()[j].vertCount;
				}
			}
		}

		std::vector<zonetool::iw7::havok::builder::triangle> collision_triangles(zonetool::iw5::XModel* asset)
		{
			return collision_mesh(asset, 1.0f).triangles;
		}
	}
}
