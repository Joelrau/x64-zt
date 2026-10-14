#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_phys_rig(zone_reader& reader, XModelPhysRig* rig)
		{
			reader.read_asset(ASSET_TYPE_PHYSCOLLMAP, rig->collmap);

			if (const auto bodies = reader.read_array(rig->bodies, 7, rig->bodyCount))
			{
				for (auto i = 0; i < rig->bodyCount; i++)
				{
					reader.read_asset(ASSET_TYPE_PHYSPRESET, bodies[i].physPreset);
					reader.read_script_string(bodies[i].name);
				}
			}

			if (const auto constraints = reader.read_array(rig->constraints, 7, rig->constraintCount))
			{
				for (auto i = 0; i < rig->constraintCount; i++)
				{
					reader.read_asset(ASSET_TYPE_PHYSCONSTRAINT, constraints[i].physConstraint);
					reader.read_script_string(constraints[i].bone0);
					reader.read_script_string(constraints[i].bone1);
				}
			}

			reader.read_array(rig->indices, 1, rig->indexCount);
		}

		void read_cloth_sim(zone_reader& reader, XModelClothSim* sim)
		{
			reader.read_stream(sim->particles, 15, 384, sim->particleCount);
			reader.read_stream(sim->constraints0, 15, 32, sim->constraint0Count);
			reader.read_stream(sim->constraints1, 15, 48, sim->constraint1Count);
			reader.read_stream(sim->constraints2, 15, 64, sim->constraint2Count);
			reader.read_array(sim->indices0, 1, sim->particleCount);
			reader.read_stream(sim->constraints3, 15, 176, sim->constraint3Count);
			reader.read_stream(sim->constraints4, 15, 128, sim->constraint4Count);
			reader.read_array(sim->indices1, 1, sim->particleCount);
			reader.read_array(sim->indices2, 1, sim->indices2Count);
		}

		void read_cloth(zone_reader& reader, XModelCloth* cloth)
		{
			reader.read_script_string(cloth->name);
			reader.read_script_string_array(cloth->boneNames, cloth->sim.particleCount);
			reader.read_script_string_array(cloth->attachNames, cloth->attachCount);
			reader.read_stream(cloth->attachData, 15, 32, cloth->attachCount);
			read_cloth_sim(reader, &cloth->sim);
		}

		void read_rig(zone_reader& reader, XModelRig* rig)
		{
			reader.read_script_string(rig->name);
			reader.read_script_string_array(rig->boneNames, rig->boneCount);
			reader.read_stream(rig->boneData, 15, 32, rig->boneCount);

			if (const auto entries = reader.read_array(rig->entries, 7, rig->entryCount))
			{
				for (auto i = 0; i < rig->entryCount; i++)
				{
					reader.read_stream(entries[i].data, 15, 48, entries[i].dataCount);
					reader.read_script_string(entries[i].name);
				}
			}

			if (const auto tail = reader.read_single(rig->tail, 3))
			{
				for (auto& name : tail->names)
				{
					reader.read_script_string(name);
				}
			}
		}
	}

	void xmodel_base::read(zone_reader& reader, XModelBase* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		const std::size_t bone_count = asset->numBones + asset->numClientBones;
		const auto child_bone_count = bone_count - asset->numRootBones;

		reader.read_script_string_array(asset->boneNames, bone_count);
		reader.read_array(asset->parentList, 1, child_bone_count);
		reader.read_stream(asset->tagAngles, 1, 8, child_bone_count);
		reader.read_stream(asset->tagPositions, 3, 12, child_bone_count);
		reader.read_array(asset->partClassification, 0, asset->numBones);
		reader.read_stream(asset->baseMat, 3, 32, bone_count);

		for (auto& lod : asset->lodInfo)
		{
			reader.read_asset(ASSET_TYPE_XMODEL_SURFS, lod.modelSurfs);
		}

		reader.read_stream(asset->collSurfs, 7, 48, asset->collSurfCount);
		reader.read_stream(asset->boneInfo, 3, 28, bone_count);
		reader.read_script_string_array(asset->extraBoneNames, asset->extraBoneNameCount);
		reader.read_stream(asset->extraData, 3, 8, asset->extraDataCount);
		reader.read_asset(ASSET_TYPE_PHYSPRESET, asset->physPreset);
		reader.read_asset(ASSET_TYPE_PHYSCOLLMAP, asset->physCollmap);
		reader.read_script_string_array(asset->physRigNames, asset->physRigCount);

		if (const auto rigs = reader.read_array(asset->physRigs, 7, asset->physRigCount))
		{
			for (auto i = 0; i < asset->physRigCount; i++)
			{
				if (const auto rig = reader.read_single(rigs[i], 7))
				{
					read_phys_rig(reader, rig);
				}
			}
		}

		reader.read_asset(ASSET_TYPE_PHYS_CLOTH_TUNING, asset->physClothTuning);

		if (const auto cloth = reader.read_single(asset->cloth, 7))
		{
			read_cloth(reader, cloth);
		}

		if (const auto rig = reader.read_single(asset->rig, 7))
		{
			read_rig(reader, rig);
		}

		if (const auto image_refs = reader.read_array(asset->imageRefs, 7, asset->imageRefCount))
		{
			for (auto i = 0; i < asset->imageRefCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, image_refs[i].image);
			}
		}

		for (auto& script : asset->skeletonScripts)
		{
			reader.read_asset(ASSET_TYPE_SKELETON_SCRIPT, script);
		}

		reader.pop_stream();
	}

	void xmodel::read(zone_reader& reader, XModel* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if ((asset->flags & 0x200) == 0)
		{
			reader.read_asset(ASSET_TYPE_XMODEL_BASE, asset->base);
		}
		else if (const auto variants = reader.read_array(asset->variants, 7, asset->variantCount))
		{
			for (auto i = 0; i < asset->variantCount; i++)
			{
				reader.read_asset(ASSET_TYPE_XMODEL, variants[i].model);
				reader.read_script_string(variants[i].name);
			}
		}

		reader.read_asset_array(ASSET_TYPE_MATERIAL, asset->materialHandles, asset->numMaterials);
		reader.read_array(asset->materialIndices, 1, asset->numMaterials);
		reader.pop_stream();
	}
}
