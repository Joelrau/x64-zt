#include <std_include.hpp>

#include "../../include.hpp"
#include "../techset.hpp"
#include "bone_rig.hpp"

namespace zonetool::h1::converter::s2::bone_rig
{
	namespace
	{
		template <typename Model, typename AnimMat>
		std::optional<skeleton> load_skeleton(const Model* model)
		{
			if (!model || !model->boneNames || !model->baseMat || (model->numBones > model->numRootBones && !model->parentList))
			{
				return {};
			}

			const auto* mats = reinterpret_cast<const AnimMat*>(model->baseMat);

			skeleton rig;
			for (auto i = 0; i < model->numBones; i++)
			{
				rig.names.emplace_back(SL_ConvertToString(model->boneNames[i]));
				rig.parents.emplace_back(i < model->numRootBones ? -1 : i - model->parentList[i - model->numRootBones]);
				rig.bind_world.emplace_back(to_transform(mats[i]));
			}

			rig.finish();
			return rig;
		}
	}

	void skeleton::finish()
	{
		this->bind_local.resize(this->names.size());
		this->anatomy.resize(this->names.size());
		for (auto i = 0u; i < this->names.size(); i++)
		{
			this->indices.emplace(this->names[i], static_cast<int>(i));
			this->bind_local[i] = this->parents[i] < 0
				? this->bind_world[i]
				: compose(invert(this->bind_world[this->parents[i]]), this->bind_world[i]);
		}
	}

	int skeleton::find(const std::string& name) const
	{
		const auto found = this->indices.find(name);
		return found == this->indices.end() ? -1 : found->second;
	}

	bool skeleton::has(const std::string& name) const
	{
		return this->indices.find(name) != this->indices.end();
	}

	bool skeleton::has_ancestor(const int bone, const int ancestor) const
	{
		for (auto index = bone; index >= 0; index = this->parents[index])
		{
			if (index == ancestor)
			{
				return true;
			}
		}

		return false;
	}

	transform compose(const transform& parent, const transform& local)
	{
		const auto rotated = cast::quat_rotate(parent.rotation, local.position);
		return {cast::quat_normalize(cast::quat_multiply(parent.rotation, local.rotation)),
			{{parent.position.v[0] + rotated.v[0], parent.position.v[1] + rotated.v[1], parent.position.v[2] + rotated.v[2]}}};
	}

	transform invert(const transform& value)
	{
		const auto rotation = cast::quat_conjugate(value.rotation);
		const auto position = cast::quat_rotate(rotation, {{-value.position.v[0], -value.position.v[1], -value.position.v[2]}});
		return {rotation, position};
	}

	std::optional<skeleton> load_h1_skeleton(const char* name)
	{
		const auto* model = db_find_x_asset_header_safe(ASSET_TYPE_XMODEL, name).model;
		return load_skeleton<XModel, DObjAnimMat>(model);
	}

	std::optional<skeleton> load_s2_skeleton(const char* name)
	{
		db_scope _(techset::common_zone().db());

		const auto* model = zonetool::s2::db_find_x_asset_header_safe(zonetool::s2::ASSET_TYPE_XMODEL, name).model;
		if (!model || (model->flags & zonetool::s2::XMODEL_FLAG_VARIANTS))
		{
			return {};
		}

		return load_skeleton<zonetool::s2::XModelBase, zonetool::s2::DObjAnimMat>(model->base);
	}
}
