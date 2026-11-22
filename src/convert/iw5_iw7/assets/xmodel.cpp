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
				target::SURFACE_FLAG_NONE,
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

			// The surface type is a 5 bit field at bit 20, every other flag is a single bit
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
			// A collision triangle stores its corners as a face plane and two barycentric planes:
			// s = dot(svec, p) - svec[3], t = dot(tvec, p) - tvec[3], inside where s, t >= 0 and
			// s + t <= 1. The corners are where (s, t) is (0,0), (1,0) and (0,1) on the face plane.
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

			// Static collision only: a model collides as its collision triangles, built the way IW7
			// models ship (one static body). Dynamic bodies and the physics LOD are not generated.
			target::PhysicsAsset* generate_physics_asset(source::XModel* asset, utils::memory::allocator& allocator)
			{
				namespace builder = zonetool::iw7::havok::builder;

				constexpr auto model_scale = 0.03125f;
				constexpr auto contents_foliage = 0x2;
				constexpr auto contents_static_prop = 0x3180u;

				builder::mesh_input mesh{};
				for (auto i = 0; i < asset->numCollSurfs; i++)
				{
					const auto& surface = asset->collSurfs.get()[i];
					if (!surface.collTris.value || surface.numCollTris <= 0 || (surface.contents & ~contents_foliage) == 0)
					{
						continue; // pure foliage is something to push through
					}

					const auto material_crc = collision::iw7_material_crc(surface.surfFlags);
					const auto user_data = static_cast<std::uint64_t>(static_cast<std::uint32_t>(to_iw7_surface_flags(surface.surfFlags)));

					for (auto j = 0; j < surface.numCollTris; j++)
					{
						const auto& collision_triangle = surface.collTris.get()[j];
						float corners[3][3];
						if (!collision_triangle_corners(collision_triangle, corners))
						{
							continue;
						}

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

						const float normal[3] = {
							edge_a[1] * edge_b[2] - edge_a[2] * edge_b[1],
							edge_a[2] * edge_b[0] - edge_a[0] * edge_b[2],
							edge_a[0] * edge_b[1] - edge_a[1] * edge_b[0],
						};

						const auto length_squared = normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2];
						if (length_squared == 0.0f)
						{
							continue; // sliver
						}

						const auto& plane = collision_triangle.plane;
						if (normal[0] * plane[0] + normal[1] * plane[1] + normal[2] * plane[2] < 0.0f)
						{
							for (auto k = 0; k < 3; k++)
							{
								std::swap(triangle.verts[1][k], triangle.verts[2][k]);
							}
						}

						triangle.contents = surface.contents;
						triangle.material_crc = material_crc;
						triangle.surface_tag = static_cast<unsigned short>(i);
						triangle.user_data = user_data;
						mesh.triangles.emplace_back(triangle);
					}
				}

				if (mesh.triangles.empty())
				{
					return nullptr;
				}

				builder::physics_asset_input input{};
				input.body_name = asset->name.get();
				input.body_contents = contents_static_prop;

				const auto blob = builder::build_model_physics_asset(mesh, input);
				if (blob.empty())
				{
					return nullptr;
				}

				const auto physics_asset = allocator.allocate<target::PhysicsAsset>();
				physics_asset->name = allocator.duplicate_string(asset->name.get());
				physics_asset->havokDataSize = static_cast<unsigned int>(blob.size());
				physics_asset->havokData = allocator.allocate_array<char>(blob.size());
				std::memcpy(physics_asset->havokData, blob.data(), blob.size());
				physics_asset->numRigidBodies = 1;

				// the runtime indexes both event arrays without a null check on the array itself
				physics_asset->numSFXEventAssets = 1;
				physics_asset->sfxEventAssets = allocator.allocate_array<target::PhysicsSFXEventAsset*>(1);
				physics_asset->numVFXEventAssets = 1;
				physics_asset->vfxEventAssets = allocator.allocate_array<target::PhysicsVFXEventAsset*>(1);
				return physics_asset;
			}
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

			new_asset->physicsAsset = generate_physics_asset(asset, allocator);

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

			for (auto i = 0; i < asset->numLods; i++)
			{
				if (asset->lodInfo[i].modelSurfs)
				{
					xsurface::dump(asset->lodInfo[i].modelSurfs.get());
				}
			}
		}
	}
}
