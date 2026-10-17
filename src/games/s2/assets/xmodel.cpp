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

	namespace
	{
		constexpr unsigned int xmodel_flag_variants = 0x200;

		std::size_t bone_count(const XModelBase* asset)
		{
			return asset->numBones + asset->numClientBones;
		}

		std::size_t child_bone_count(const XModelBase* asset)
		{
			return bone_count(asset) - asset->numRootBones;
		}

		void prepare_cloth(script_string_preparer& prepare, XModelCloth* cloth)
		{
			prepare(cloth->name);
			prepare(cloth->boneNames, cloth->sim.particleCount);
			prepare(cloth->attachNames, cloth->attachCount);
		}

		void prepare_rig(script_string_preparer& prepare, XModelRig* rig)
		{
			prepare(rig->name);
			prepare(rig->boneNames, rig->boneCount);
			for (auto i = 0; rig->entries && i < rig->entryCount; i++)
			{
				prepare(rig->entries[i].name);
			}
			if (rig->tail)
			{
				for (auto& name : rig->tail->names)
				{
					prepare(name);
				}
			}
		}

		void write_phys_rig(zone_base* zone, zone_buffer* buf, XModelPhysRig* rig, XModelPhysRig* dest)
		{
			dest->collmap = asset_reference(zone, ASSET_TYPE_PHYSCOLLMAP, rig->collmap);

			if (rig->bodies)
			{
				buf->align(7);
				auto* bodies = buf->write(rig->bodies, rig->bodyCount);
				for (auto i = 0; i < rig->bodyCount; i++)
				{
					bodies[i].physPreset = asset_reference(zone, ASSET_TYPE_PHYSPRESET, rig->bodies[i].physPreset);
				}
				buf->clear_pointer(&dest->bodies);
			}

			if (rig->constraints)
			{
				buf->align(7);
				auto* constraints = buf->write(rig->constraints, rig->constraintCount);
				for (auto i = 0; i < rig->constraintCount; i++)
				{
					constraints[i].physConstraint = asset_reference(zone, ASSET_TYPE_PHYSCONSTRAINT, rig->constraints[i].physConstraint);
				}
				buf->clear_pointer(&dest->constraints);
			}

			write_inline(buf, 1, rig->indices, rig->indexCount, dest->indices);
		}

		void write_cloth_sim(zone_buffer* buf, XModelClothSim* sim, XModelClothSim* dest)
		{
			write_inline_bytes(buf, 15, sim->particles, 384 * sim->particleCount, dest->particles);
			write_inline_bytes(buf, 15, sim->constraints0, 32 * sim->constraint0Count, dest->constraints0);
			write_inline_bytes(buf, 15, sim->constraints1, 48 * sim->constraint1Count, dest->constraints1);
			write_inline_bytes(buf, 15, sim->constraints2, 64 * sim->constraint2Count, dest->constraints2);
			write_inline(buf, 1, sim->indices0, sim->particleCount, dest->indices0);
			write_inline_bytes(buf, 15, sim->constraints3, 176 * sim->constraint3Count, dest->constraints3);
			write_inline_bytes(buf, 15, sim->constraints4, 128 * sim->constraint4Count, dest->constraints4);
			write_inline(buf, 1, sim->indices1, sim->particleCount, dest->indices1);
			write_inline(buf, 1, sim->indices2, sim->indices2Count, dest->indices2);
		}

		void write_cloth(zone_buffer* buf, XModelCloth* cloth, XModelCloth* dest)
		{
			write_inline(buf, 3, cloth->boneNames, cloth->sim.particleCount, dest->boneNames);
			write_inline(buf, 3, cloth->attachNames, cloth->attachCount, dest->attachNames);
			write_inline_bytes(buf, 15, cloth->attachData, 32 * cloth->attachCount, dest->attachData);
			write_cloth_sim(buf, &cloth->sim, &dest->sim);
		}

		void write_rig(zone_buffer* buf, XModelRig* rig, XModelRig* dest)
		{
			write_inline(buf, 3, rig->boneNames, rig->boneCount, dest->boneNames);
			write_inline_bytes(buf, 15, rig->boneData, 32 * rig->boneCount, dest->boneData);

			if (rig->entries)
			{
				buf->align(7);
				auto* entries = buf->write(rig->entries, rig->entryCount);
				for (auto i = 0; i < rig->entryCount; i++)
				{
					write_inline_bytes(buf, 15, rig->entries[i].data, 48 * rig->entries[i].dataCount, entries[i].data);
				}
				buf->clear_pointer(&dest->entries);
			}

			write_inline(buf, 3, rig->tail, 1, dest->tail);
		}

		void dump_phys_rig(assetmanager::dumper& write, XModelPhysRig* rig)
		{
			write.dump_asset(rig->collmap);
			write.dump_array(rig->bodies, rig->bodyCount);
			for (auto i = 0; rig->bodies && i < rig->bodyCount; i++)
			{
				write.dump_asset(rig->bodies[i].physPreset);
				dump_script_string(write, rig->bodies[i].name);
			}
			write.dump_array(rig->constraints, rig->constraintCount);
			for (auto i = 0; rig->constraints && i < rig->constraintCount; i++)
			{
				write.dump_asset(rig->constraints[i].physConstraint);
				dump_script_string(write, rig->constraints[i].bone0);
				dump_script_string(write, rig->constraints[i].bone1);
			}
			write.dump_array(rig->indices, rig->indexCount);
		}

		void parse_phys_rig(assetmanager::reader& read, XModelPhysRig* rig)
		{
			rig->collmap = read.read_asset<PhysCollmap>();
			rig->bodies = read.read_array<XModelPhysBody>();
			for (auto i = 0; rig->bodies && i < rig->bodyCount; i++)
			{
				rig->bodies[i].physPreset = read.read_asset<PhysPreset>();
				rig->bodies[i].name = parse_script_string(read);
			}
			rig->constraints = read.read_array<XModelPhysConstraint>();
			for (auto i = 0; rig->constraints && i < rig->constraintCount; i++)
			{
				rig->constraints[i].physConstraint = read.read_asset<PhysConstraint>();
				rig->constraints[i].bone0 = parse_script_string(read);
				rig->constraints[i].bone1 = parse_script_string(read);
			}
			rig->indices = read.read_array<unsigned short>();
		}

		void dump_cloth(assetmanager::dumper& write, XModelCloth* cloth)
		{
			auto& sim = cloth->sim;
			dump_script_string(write, cloth->name);
			dump_script_string_array(write, cloth->boneNames, sim.particleCount);
			dump_script_string_array(write, cloth->attachNames, cloth->attachCount);
			write.dump_array(cloth->attachData, 32 * cloth->attachCount);
			write.dump_array(sim.particles, 384 * sim.particleCount);
			write.dump_array(sim.constraints0, 32 * sim.constraint0Count);
			write.dump_array(sim.constraints1, 48 * sim.constraint1Count);
			write.dump_array(sim.constraints2, 64 * sim.constraint2Count);
			write.dump_array(sim.indices0, sim.particleCount);
			write.dump_array(sim.constraints3, 176 * sim.constraint3Count);
			write.dump_array(sim.constraints4, 128 * sim.constraint4Count);
			write.dump_array(sim.indices1, sim.particleCount);
			write.dump_array(sim.indices2, sim.indices2Count);
		}

		void parse_cloth(assetmanager::reader& read, zone_memory* mem, XModelCloth* cloth)
		{
			auto& sim = cloth->sim;
			cloth->name = parse_script_string(read);
			cloth->boneNames = parse_script_string_array(read, mem, sim.particleCount);
			cloth->attachNames = parse_script_string_array(read, mem, cloth->attachCount);
			cloth->attachData = read.read_array<char>();
			sim.particles = read.read_array<char>();
			sim.constraints0 = read.read_array<char>();
			sim.constraints1 = read.read_array<char>();
			sim.constraints2 = read.read_array<char>();
			sim.indices0 = read.read_array<unsigned short>();
			sim.constraints3 = read.read_array<char>();
			sim.constraints4 = read.read_array<char>();
			sim.indices1 = read.read_array<unsigned short>();
			sim.indices2 = read.read_array<unsigned short>();
		}

		void dump_rig(assetmanager::dumper& write, XModelRig* rig)
		{
			dump_script_string(write, rig->name);
			dump_script_string_array(write, rig->boneNames, rig->boneCount);
			write.dump_array(rig->boneData, 32 * rig->boneCount);
			write.dump_array(rig->entries, rig->entryCount);
			for (auto i = 0; rig->entries && i < rig->entryCount; i++)
			{
				write.dump_array(rig->entries[i].data, 48 * rig->entries[i].dataCount);
				dump_script_string(write, rig->entries[i].name);
			}
			write.dump_single(rig->tail);
			for (auto i = 0; rig->tail && i < 4; i++)
			{
				dump_script_string(write, rig->tail->names[i]);
			}
		}

		void parse_rig(assetmanager::reader& read, zone_memory* mem, XModelRig* rig)
		{
			rig->name = parse_script_string(read);
			rig->boneNames = parse_script_string_array(read, mem, rig->boneCount);
			rig->boneData = read.read_array<char>();
			rig->entries = read.read_array<XModelRigBoneEntry>();
			for (auto i = 0; rig->entries && i < rig->entryCount; i++)
			{
				rig->entries[i].data = read.read_array<char>();
				rig->entries[i].name = parse_script_string(read);
			}
			rig->tail = read.read_single<XModelRigTail>();
			for (auto i = 0; rig->tail && i < 4; i++)
			{
				rig->tail->names[i] = parse_script_string(read);
			}
		}
	}

	XModelBase* xmodel_base::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_XMODEL_BASE, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing xmodelbase \"%s\"...", name.data());

		auto* asset = read.read_single<XModelBase>();
		asset->name = read.read_string();

		const auto bones = bone_count(asset);
		asset->boneNames = parse_script_string_array(read, mem, bones);
		asset->parentList = read.read_array<unsigned short>();
		asset->tagAngles = read.read_array<short>();
		asset->tagPositions = read.read_array<float>();
		asset->partClassification = read.read_array<unsigned char>();
		asset->baseMat = read.read_array<char>();

		for (auto& lod : asset->lodInfo)
		{
			lod.modelSurfs = read.read_asset<XModelSurfs>();
		}

		asset->collSurfs = read.read_array<char>();
		asset->boneInfo = read.read_array<char>();
		asset->extraBoneNames = parse_script_string_array(read, mem, asset->extraBoneNameCount);
		asset->extraData = read.read_array<char>();
		asset->physPreset = read.read_asset<PhysPreset>();
		asset->physCollmap = read.read_asset<PhysCollmap>();
		asset->physRigNames = parse_script_string_array(read, mem, asset->physRigCount);

		asset->physRigs = read.read_array<XModelPhysRig*>();
		for (auto i = 0; asset->physRigs && i < asset->physRigCount; i++)
		{
			asset->physRigs[i] = read.read_single<XModelPhysRig>();
			if (asset->physRigs[i])
			{
				parse_phys_rig(read, asset->physRigs[i]);
			}
		}

		asset->physClothTuning = read.read_asset<PhysClothTuning>();

		asset->cloth = read.read_single<XModelCloth>();
		if (asset->cloth)
		{
			parse_cloth(read, mem, asset->cloth);
		}

		asset->rig = read.read_single<XModelRig>();
		if (asset->rig)
		{
			parse_rig(read, mem, asset->rig);
		}

		asset->imageRefs = read.read_array<XModelImageRef>();
		for (auto i = 0; asset->imageRefs && i < asset->imageRefCount; i++)
		{
			asset->imageRefs[i].image = read.read_asset<GfxImage>();
		}

		for (auto& script : asset->skeletonScripts)
		{
			script = read.read_asset<SkeletonScript>();
		}

		read.close();
		return asset;
	}

	void xmodel_base::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* asset = this->asset_;
		script_string_preparer prepare(buf);

		prepare(asset->boneNames, bone_count(asset));
		prepare(asset->extraBoneNames, asset->extraBoneNameCount);
		prepare(asset->physRigNames, asset->physRigCount);

		for (auto i = 0; asset->physRigs && i < asset->physRigCount; i++)
		{
			auto* rig = asset->physRigs[i];
			for (auto j = 0; rig && rig->bodies && j < rig->bodyCount; j++)
			{
				prepare(rig->bodies[j].name);
			}
			for (auto j = 0; rig && rig->constraints && j < rig->constraintCount; j++)
			{
				prepare(rig->constraints[j].bone0);
				prepare(rig->constraints[j].bone1);
			}
		}

		if (asset->cloth)
		{
			prepare_cloth(prepare, asset->cloth);
		}

		if (asset->rig)
		{
			prepare_rig(prepare, asset->rig);
		}
	}

	void xmodel_base::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;

		for (const auto& lod : asset->lodInfo)
		{
			add_dependency(zone, ASSET_TYPE_XMODEL_SURFS, lod.modelSurfs);
		}

		add_dependency(zone, ASSET_TYPE_PHYSPRESET, asset->physPreset);
		add_dependency(zone, ASSET_TYPE_PHYSCOLLMAP, asset->physCollmap);

		for (auto i = 0; asset->physRigs && i < asset->physRigCount; i++)
		{
			const auto* rig = asset->physRigs[i];
			if (!rig)
			{
				continue;
			}

			add_dependency(zone, ASSET_TYPE_PHYSCOLLMAP, rig->collmap);
			for (auto j = 0; rig->bodies && j < rig->bodyCount; j++)
			{
				add_dependency(zone, ASSET_TYPE_PHYSPRESET, rig->bodies[j].physPreset);
			}
			for (auto j = 0; rig->constraints && j < rig->constraintCount; j++)
			{
				add_dependency(zone, ASSET_TYPE_PHYSCONSTRAINT, rig->constraints[j].physConstraint);
			}
		}

		add_dependency(zone, ASSET_TYPE_PHYS_CLOTH_TUNING, asset->physClothTuning);

		for (auto i = 0; asset->imageRefs && i < asset->imageRefCount; i++)
		{
			add_dependency(zone, ASSET_TYPE_IMAGE, asset->imageRefs[i].image);
		}

		for (const auto* script : asset->skeletonScripts)
		{
			add_dependency(zone, ASSET_TYPE_SKELETON_SCRIPT, script);
		}
	}

	void xmodel_base::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);

		const auto bones = bone_count(data);
		const auto child_bones = child_bone_count(data);
		write_inline(buf, 3, data->boneNames, bones, dest->boneNames);
		write_inline(buf, 1, data->parentList, child_bones, dest->parentList);
		write_inline_bytes(buf, 1, data->tagAngles, 8 * child_bones, dest->tagAngles);
		write_inline_bytes(buf, 3, data->tagPositions, 12 * child_bones, dest->tagPositions);
		write_inline(buf, 0, data->partClassification, data->numBones, dest->partClassification);
		write_inline_bytes(buf, 3, data->baseMat, 32 * bones, dest->baseMat);

		for (auto i = 0u; i < std::size(data->lodInfo); i++)
		{
			dest->lodInfo[i].modelSurfs = asset_reference(zone, ASSET_TYPE_XMODEL_SURFS, data->lodInfo[i].modelSurfs);
		}

		write_inline_bytes(buf, 7, data->collSurfs, 48 * data->collSurfCount, dest->collSurfs);
		write_inline_bytes(buf, 3, data->boneInfo, 28 * bones, dest->boneInfo);
		write_inline(buf, 3, data->extraBoneNames, data->extraBoneNameCount, dest->extraBoneNames);
		write_inline_bytes(buf, 3, data->extraData, 8 * data->extraDataCount, dest->extraData);
		dest->physPreset = asset_reference(zone, ASSET_TYPE_PHYSPRESET, data->physPreset);
		dest->physCollmap = asset_reference(zone, ASSET_TYPE_PHYSCOLLMAP, data->physCollmap);
		write_inline(buf, 3, data->physRigNames, data->physRigCount, dest->physRigNames);

		if (data->physRigs)
		{
			buf->align(7);
			auto* rigs = buf->write(data->physRigs, data->physRigCount);
			for (auto i = 0; i < data->physRigCount; i++)
			{
				if (!data->physRigs[i])
				{
					continue;
				}

				buf->align(7);
				auto* rig = buf->write(data->physRigs[i]);
				write_phys_rig(zone, buf, data->physRigs[i], rig);
				buf->clear_pointer(&rigs[i]);
			}
			buf->clear_pointer(&dest->physRigs);
		}

		dest->physClothTuning = asset_reference(zone, ASSET_TYPE_PHYS_CLOTH_TUNING, data->physClothTuning);

		if (data->cloth)
		{
			buf->align(7);
			auto* cloth = buf->write(data->cloth);
			write_cloth(buf, data->cloth, cloth);
			buf->clear_pointer(&dest->cloth);
		}

		if (data->rig)
		{
			buf->align(7);
			auto* rig = buf->write(data->rig);
			write_rig(buf, data->rig, rig);
			buf->clear_pointer(&dest->rig);
		}

		if (data->imageRefs)
		{
			buf->align(7);
			auto* image_refs = buf->write(data->imageRefs, data->imageRefCount);
			for (auto i = 0; i < data->imageRefCount; i++)
			{
				image_refs[i].image = asset_reference(zone, ASSET_TYPE_IMAGE, data->imageRefs[i].image);
			}
			buf->clear_pointer(&dest->imageRefs);
		}

		for (auto i = 0u; i < std::size(data->skeletonScripts); i++)
		{
			dest->skeletonScripts[i] = asset_reference(zone, ASSET_TYPE_SKELETON_SCRIPT, data->skeletonScripts[i]);
		}

		buf->pop_stream();
	}

	void xmodel_base::dump(XModelBase* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_XMODEL_BASE, asset->name)))
		{
			return;
		}

		const auto bones = static_cast<std::uint32_t>(bone_count(asset));
		const auto child_bones = static_cast<std::uint32_t>(child_bone_count(asset));

		write.dump_single(asset);
		write.dump_string(asset->name);
		dump_script_string_array(write, asset->boneNames, bones);
		write.dump_array(asset->parentList, child_bones);
		write.dump_array(asset->tagAngles, 4 * child_bones);
		write.dump_array(asset->tagPositions, 3 * child_bones);
		write.dump_array(asset->partClassification, asset->numBones);
		write.dump_array(asset->baseMat, 32 * bones);

		for (auto& lod : asset->lodInfo)
		{
			write.dump_asset(lod.modelSurfs);
		}

		write.dump_array(asset->collSurfs, 48 * asset->collSurfCount);
		write.dump_array(asset->boneInfo, 28 * bones);
		dump_script_string_array(write, asset->extraBoneNames, asset->extraBoneNameCount);
		write.dump_array(asset->extraData, 8 * asset->extraDataCount);
		write.dump_asset(asset->physPreset);
		write.dump_asset(asset->physCollmap);
		dump_script_string_array(write, asset->physRigNames, asset->physRigCount);

		write.dump_array(asset->physRigs, asset->physRigCount);
		for (auto i = 0; asset->physRigs && i < asset->physRigCount; i++)
		{
			write.dump_single(asset->physRigs[i]);
			if (asset->physRigs[i])
			{
				dump_phys_rig(write, asset->physRigs[i]);
			}
		}

		write.dump_asset(asset->physClothTuning);

		write.dump_single(asset->cloth);
		if (asset->cloth)
		{
			dump_cloth(write, asset->cloth);
		}

		write.dump_single(asset->rig);
		if (asset->rig)
		{
			dump_rig(write, asset->rig);
		}

		write.dump_array(asset->imageRefs, asset->imageRefCount);
		for (auto i = 0; asset->imageRefs && i < asset->imageRefCount; i++)
		{
			write.dump_asset(asset->imageRefs[i].image);
		}

		for (auto* script : asset->skeletonScripts)
		{
			write.dump_asset(script);
		}

		write.close();
	}

	XModel* xmodel::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_XMODEL, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing xmodel \"%s\"...", name.data());

		auto* asset = read.read_single<XModel>();
		asset->name = read.read_string();

		if ((asset->flags & xmodel_flag_variants) == 0)
		{
			asset->base = read.read_asset<XModelBase>();
		}
		else
		{
			asset->variants = read.read_array<XModelVariant>();
			for (auto i = 0; asset->variants && i < asset->variantCount; i++)
			{
				asset->variants[i].model = read.read_asset<XModel>();
				asset->variants[i].name = parse_script_string(read);
			}
		}

		asset->materialHandles = read.read_array<Material*>();
		for (auto i = 0; asset->materialHandles && i < asset->numMaterials; i++)
		{
			asset->materialHandles[i] = read.read_asset<Material>();
		}
		asset->materialIndices = read.read_array<unsigned short>();

		read.close();
		return asset;
	}

	void xmodel::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* asset = this->asset_;
		if ((asset->flags & xmodel_flag_variants) == 0)
		{
			return;
		}

		script_string_preparer prepare(buf);
		for (auto i = 0; asset->variants && i < asset->variantCount; i++)
		{
			prepare(asset->variants[i].name);
		}
	}

	void xmodel::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;

		if ((asset->flags & xmodel_flag_variants) == 0)
		{
			add_dependency(zone, ASSET_TYPE_XMODEL_BASE, asset->base);
		}
		else
		{
			for (auto i = 0; asset->variants && i < asset->variantCount; i++)
			{
				add_dependency(zone, ASSET_TYPE_XMODEL, asset->variants[i].model);
			}
		}

		for (auto i = 0; asset->materialHandles && i < asset->numMaterials; i++)
		{
			add_dependency(zone, ASSET_TYPE_MATERIAL, asset->materialHandles[i]);
		}
	}

	void xmodel::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);

		if ((data->flags & xmodel_flag_variants) == 0)
		{
			dest->base = asset_reference(zone, ASSET_TYPE_XMODEL_BASE, data->base);
		}
		else if (data->variants)
		{
			buf->align(7);
			auto* variants = buf->write(data->variants, data->variantCount);
			for (auto i = 0; i < data->variantCount; i++)
			{
				variants[i].model = asset_reference(zone, ASSET_TYPE_XMODEL, data->variants[i].model);
			}
			buf->clear_pointer(&dest->variants);
		}

		if (data->materialHandles)
		{
			buf->align(7);
			auto* materials = buf->write(data->materialHandles, data->numMaterials);
			for (auto i = 0; i < data->numMaterials; i++)
			{
				materials[i] = asset_reference(zone, ASSET_TYPE_MATERIAL, data->materialHandles[i]);
			}
			buf->clear_pointer(&dest->materialHandles);
		}

		write_inline(buf, 1, data->materialIndices, data->numMaterials, dest->materialIndices);

		buf->pop_stream();
	}

	void xmodel::dump(XModel* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_XMODEL, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		if ((asset->flags & xmodel_flag_variants) == 0)
		{
			write.dump_asset(asset->base);
		}
		else
		{
			write.dump_array(asset->variants, asset->variantCount);
			for (auto i = 0; asset->variants && i < asset->variantCount; i++)
			{
				write.dump_asset(asset->variants[i].model);
				dump_script_string(write, asset->variants[i].name);
			}
		}

		write.dump_array(asset->materialHandles, asset->numMaterials);
		for (auto i = 0; asset->materialHandles && i < asset->numMaterials; i++)
		{
			write.dump_asset(asset->materialHandles[i]);
		}
		write.dump_array(asset->materialIndices, asset->numMaterials);

		write.close();
	}
}
