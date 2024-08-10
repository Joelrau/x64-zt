#include <std_include.hpp>
#include "zonetool/t7/converter/h1/include.hpp"
#include "xmodel.hpp"

#include "zonetool/h1/assets/xmodel.hpp"

#include <utils/string.hpp>

namespace zonetool::t7
{
	namespace converter::h1
	{
		namespace xmodel
		{
			zonetool::h1::XModel* convert(XModel* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::XModel>();

				assert(asset->numBones < 256);

				COPY_VALUE(numLods);
				if (new_asset->numLods > 6)
				{
					new_asset->numLods = 6;
				}

				REINTERPRET_CAST_SAFE(name);

				COPY_VALUE(numBones);
				COPY_VALUE(numRootBones);
				new_asset->numsurfs = 0;
				new_asset->numReactiveMotionParts = 0;
				new_asset->lodRampType = 0;
				new_asset->numBonePhysics = 0;
				new_asset->scale = asset->areaScale;
				memset(new_asset->noScalePartBits, 0, sizeof(float[8]));
				REINTERPRET_CAST_SAFE(boneNames);
				REINTERPRET_CAST_SAFE(parentList);
				new_asset->tagAngles = allocator.allocate_array<zonetool::h1::XModelAngle>(asset->numBones + asset->numCosmeticBones - asset->numRootBones);
				for (auto i = 0; i < asset->numBones + asset->numCosmeticBones - asset->numRootBones; i++)
				{
					new_asset->tagAngles[i].x = QuatInt16::ToInt16(half_float::half_to_float(asset->tagAngles[i].x));
					new_asset->tagAngles[i].y = QuatInt16::ToInt16(half_float::half_to_float(asset->tagAngles[i].y));
					new_asset->tagAngles[i].z = QuatInt16::ToInt16(half_float::half_to_float(asset->tagAngles[i].z));
					new_asset->tagAngles[i].base = QuatInt16::ToInt16(half_float::half_to_float(asset->tagAngles[i].w));
				}
				new_asset->tagPositions = allocator.allocate_array<zonetool::h1::XModelTagPos>(asset->numBones - asset->numRootBones);
				for (auto i = 0; i < asset->numBones - asset->numRootBones; i++)
				{
					new_asset->tagPositions[i].x = asset->tagPositions[i].x;
					new_asset->tagPositions[i].y = asset->tagPositions[i].y;
					new_asset->tagPositions[i].z = asset->tagPositions[i].z;
				}
				REINTERPRET_CAST_SAFE(partClassification);
				REINTERPRET_CAST_SAFE(baseMat);
				new_asset->reactiveMotionParts = nullptr;
				new_asset->reactiveMotionTweaks = nullptr;

				for (auto i = 0; i < 6; i++)
				{
					new_asset->lodInfo[i].dist = 1000000.0f;
				}

				for (auto i = 0; i < new_asset->numLods; i++)
				{
					// i made up this function, not sure how its calculated in bo3
					auto calc_lod_dist = [&]()
					{
						float constantFactor = 1000000.0f;
						return std::round(sqrtf(constantFactor / asset->averageTriArea[i]));
					};

					new_asset->lodInfo[i].dist = calc_lod_dist();
					new_asset->lodInfo[i].numsurfs = asset->meshes[i]->numSurfs;
					new_asset->lodInfo[i].surfIndex = 0;
					new_asset->lodInfo[i].modelSurfs = allocator.allocate<zonetool::h1::XModelSurfs>();
					new_asset->lodInfo[i].modelSurfs->name = allocator.duplicate_string(asset->meshes[i]->name);
					memcpy(&new_asset->lodInfo[i].partBits, &asset->meshes[i]->partBits, sizeof(float[8]));
				}

				std::vector<Material*> materials;
				std::vector<vec_t> himipInvSqRadiis;
				unsigned short surfIndex = 0;

				for (auto i = 0; i < new_asset->numLods; i++)
				{
					auto mesh_material = asset->meshMaterials[i];

					for (auto j = 0; j < mesh_material.numMaterials; j++)
					{
						auto* material = mesh_material.materials[j];
						materials.push_back(material);
						himipInvSqRadiis.push_back(mesh_material.himipInvSqRadii[j]);
					}

					new_asset->lodInfo[i].surfIndex = surfIndex;
					surfIndex += mesh_material.numMaterials;
				}

				new_asset->numsurfs = static_cast<unsigned char>(materials.size());
				new_asset->materialHandles = allocator.allocate_array<zonetool::h1::Material*>(materials.size());
				for (auto i = 0; i < materials.size(); i++)
				{
					new_asset->materialHandles[i] = reinterpret_cast<zonetool::h1::Material*>(materials[i]);
				}

				new_asset->collLod = 0xFFi8;
				if (asset->collLod)
				{
					for (char i = 0; i < new_asset->numLods; i++)
					{
						if (*asset->collLod == asset->meshes[i])
						{
							new_asset->collLod = i;
						}
					}
				}

				new_asset->numCompositeModels = 0;

				new_asset->flags = 0; //asset->flags; // not used in games

				new_asset->numCollSurfs = static_cast<short>(asset->numCollSurfs);
				new_asset->collSurfs = allocator.allocate_array<zonetool::h1::XModelCollSurf_s>(new_asset->numCollSurfs);
				for (auto i = 0; i < new_asset->numCollSurfs; i++)
				{
					//compute(&new_asset->collSurfs[i].bounds, asset->collSurfs[i].mins, asset->collSurfs[i].maxs);
					memcpy(&new_asset->collSurfs[i].bounds, &asset->collSurfs[i].mins, sizeof(zonetool::h1::Bounds)); // compute?
					new_asset->collSurfs[i].boneIdx = asset->collSurfs[i].boneIdx;
					new_asset->collSurfs[i].contents = asset->collSurfs[i].contents; // convert...
					new_asset->collSurfs[i].surfFlags = asset->collSurfs[i].surfFlags; // convert...
				}

				new_asset->contents = asset->contents; // convert...

				new_asset->boneInfo = allocator.allocate_array<zonetool::h1::XBoneInfo>(new_asset->numBones - asset->numRootBones);
				for (auto i = 0; i < new_asset->numBones - asset->numRootBones; i++)
				{
					//compute(&new_asset->boneInfo[i].bounds, asset->boneInfo[i].bounds[0], asset->boneInfo[i].bounds[1]);
					memcpy(&new_asset->boneInfo[i].bounds, asset->boneInfo[i].bounds, sizeof(zonetool::h1::Bounds)); // compute?
					new_asset->boneInfo[i].radiusSquared = asset->boneInfo[i].radiusSquared;
				}

				COPY_VALUE(radius);
				//compute(&new_asset->bounds, asset->mins, asset->maxs);
				memcpy(&new_asset->bounds, &asset->mins, sizeof(zonetool::h1::Bounds)); // compute?

				new_asset->invHighMipRadius = allocator.allocate_array<unsigned short>(new_asset->numsurfs);
				for (unsigned char i = 0; i < new_asset->numsurfs; i++)
				{
					auto val = sqrt(himipInvSqRadiis[i]);
					new_asset->invHighMipRadius[i] = half_float::float_to_half(val);
				}

				new_asset->memUsage = 0;
				new_asset->bad = 0;

				new_asset->targetCount = 0;
				new_asset->numberOfWeights = 0;
				new_asset->numberOfWeightMaps = 0;
				new_asset->weightNames = nullptr;
				new_asset->blendShapeWeightMap = nullptr;
				new_asset->physPreset = reinterpret_cast<zonetool::h1::PhysPreset*>(asset->physPreset);

				if (asset->physPreset)
				{
					new_asset->physPreset = allocator.allocate<zonetool::h1::PhysPreset>();
					new_asset->physPreset->name = allocator.duplicate_string(asset->physPreset->name);
				}

				if (asset->collmaps)
				{
					new_asset->physCollmap = allocator.allocate<zonetool::h1::PhysCollmap>();
					new_asset->physCollmap->name = allocator.duplicate_string(utils::string::va("%s_collmap", asset->name));
				}
				
				new_asset->mdaoVolumeCount = 0;
				new_asset->quantization = 0;
				new_asset->mdaoVolumes = nullptr;
				new_asset->skeletonScript = nullptr;
				new_asset->compositeModels = nullptr; // submodels
				new_asset->bonePhysics = nullptr;

				return new_asset;
			}

			void dump(XModel* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::xmodel::dump(converted_asset);
			}
		}
	}
}
