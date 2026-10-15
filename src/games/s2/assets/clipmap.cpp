#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		template <typename V>
		void visit_shared_info(const V& w, const std::size_t offset)
		{
			w.in_stream(XFILE_BLOCK_TEMP, [&]
			{
				w.shared_entry(offset, 7, sizeof(ClipInfo), [](const auto& info, std::size_t)
				{
					clip_info_layout::visit(info);
				});
			});
		}

		template <typename V>
		void visit_dyn_entity_defs(const V& w, const std::size_t index)
		{
			const auto count = w.template at<unsigned short>(offsetof(clipMap_t, dynEntCount) + index * sizeof(unsigned short));
			w.entries(offsetof(clipMap_t, dynEntDefList) + index * sizeof(void*), sizeof(DynEntityDef), count, [](const auto& def, std::size_t)
			{
				def.asset(ASSET_TYPE_XMODEL, offsetof(DynEntityDef, xModel));
				def.asset(ASSET_TYPE_FX, offsetof(DynEntityDef, destroyFx));
				def.asset(ASSET_TYPE_SOUND, offsetof(DynEntityDef, sound));
				def.asset(ASSET_TYPE_PHYSPRESET, offsetof(DynEntityDef, physPreset));
				def.array(S2_ARRAY_FIELD(DynEntityDef, hinge), 3, 1);
				def.array(S2_ARRAY_FIELD(DynEntityDef, linkTo), 3, 1);
			});
		}

		template <typename V>
		void visit_runtime_lists(const V& w, const std::size_t offset, const std::size_t element_size, const std::size_t alignment)
		{
			for (auto i = 0u; i < 2; i++)
			{
				const auto count = w.template at<unsigned short>(offsetof(clipMap_t, dynEntCount) + i * sizeof(unsigned short));
				w.runtime_array(offset + i * sizeof(void*), element_size, alignment, count);
			}
		}
	}

	template <typename V>
	void clip_map_layout::visit(const V& w)
	{
		const auto count = [&](const std::size_t offset)
		{
			return w.template at<unsigned int>(offset);
		};

		w.string(offsetof(clipMap_t, name));
		clip_info_layout::visit(w.sub(offsetof(clipMap_t, info)));
		visit_shared_info(w, offsetof(clipMap_t, pInfo));

		w.entries(offsetof(clipMap_t, nodes), sizeof(cNode_t), count(offsetof(clipMap_t, numNodes)), [](const auto& node, std::size_t)
		{
			node.array(S2_ARRAY_FIELD(cNode_t, plane), 3, 1);
		});
		w.array(S2_ARRAY_FIELD(clipMap_t, leafs), 7, count(offsetof(clipMap_t, numLeafs)));
		w.entries(offsetof(clipMap_t, cmodels), sizeof(cmodel_t), count(offsetof(clipMap_t, numSubModels)), [](const auto& model, std::size_t)
		{
			visit_shared_info(model, offsetof(cmodel_t, info));
		});
		w.asset(ASSET_TYPE_MAP_ENTS, offsetof(clipMap_t, mapEnts));
		w.entries(offsetof(clipMap_t, staticModelNames), sizeof(ClipStaticModelName), w.template at<unsigned char>(offsetof(clipMap_t, staticModelNameCount)), [](const auto& name, std::size_t)
		{
			name.string(offsetof(ClipStaticModelName, name));
		});

		const auto static_models = w.sub(offsetof(clipMap_t, staticModelData));
		static_models.array(S2_ARRAY_FIELD(ClipStaticModelData, nodes), 7, static_models.template at<unsigned int>(offsetof(ClipStaticModelData, nodeCount)));
		static_models.array(S2_ARRAY_FIELD(ClipStaticModelData, leafs), 7, static_models.template at<unsigned int>(offsetof(ClipStaticModelData, leafCount)));
		static_models.array(S2_ARRAY_FIELD(ClipStaticModelData, bounds), 3, static_models.template at<unsigned int>(offsetof(ClipStaticModelData, boundsCount)));

		visit_dyn_entity_defs(w, 0);
		visit_dyn_entity_defs(w, 1);
		visit_runtime_lists(w, offsetof(clipMap_t, dynEntPoseList), sizeof(*clipMap_t{}.dynEntPoseList[0]), 3);
		visit_runtime_lists(w, offsetof(clipMap_t, dynEntClientList), sizeof(*clipMap_t{}.dynEntClientList[0]), 7);
		visit_runtime_lists(w, offsetof(clipMap_t, dynEntCollList), sizeof(*clipMap_t{}.dynEntCollList[0]), 3);

		w.script_strings(offsetof(clipMap_t, scrStrings), count(offsetof(clipMap_t, scrStringCount)));
		w.aligned_entries(offsetof(clipMap_t, scrEntries), 3, sizeof(ClipMapScrEntry), count(offsetof(clipMap_t, scrEntryCount)), [](const auto& entry, std::size_t)
		{
			entry.script_string(offsetof(ClipMapScrEntry, name));
		});
		w.entries(offsetof(clipMap_t, mayhems), sizeof(ClipMayhemInstance), count(offsetof(clipMap_t, mayhemCount)), [](const auto& mayhem, std::size_t)
		{
			mayhem.asset(ASSET_TYPE_MAYHEM, offsetof(ClipMayhemInstance, mayhem));
			mayhem.array(S2_ARRAY_FIELD(ClipMayhemInstance, transform), 3, 1);
			mayhem.script_string(offsetof(ClipMayhemInstance, name));
		});
	}

	template void clip_map_layout::visit(const field_reader&);
	template void clip_map_layout::visit(const field_writer&);
	template void clip_map_layout::visit(const field_dumper&);
	template void clip_map_layout::visit(const field_parser&);
	template void clip_map_layout::visit(const field_dependencies&);
	template void clip_map_layout::visit(const field_preparer&);
}
