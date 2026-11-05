#include <std_include.hpp>

#include "../include.hpp"
#include "common/stock_zone.hpp"
#include "material.hpp"
#include "physcollmap.hpp"
#include "physpreset.hpp"
#include "rig/bone_rig.hpp"
#include "xmodel.hpp"
#include "xsurface.hpp"

#include <h1/zone/loader.hpp>
#include <io/game_files.hpp>

namespace zonetool::h1::converter::s2::xmodel
{
	namespace
	{
		constexpr std::array<float, 4> grass_sway_params{2000.0f, 40.0f, 0.055f, 1.5f};

		const std::regex weapon_model_name{R"(wpn_h1_[a-z0-9]+_(.+_(?:vm|npc))(_.+)?)"};
		const std::regex default_part_name{R"(attach_h1_.+_default_(.+_(?:vm|npc))(_.+)?)"};
		const std::regex scope_name{R"(attach_h1_scope_(.+_(?:vm|npc))(_.+)?)"};

		std::unordered_set<std::string> dumped_models;
		std::map<std::pair<zonetool::s2::XAssetType, std::string>, std::string> converted_names;

		int get_lod_count(const XModel* asset)
		{
			return std::clamp(static_cast<int>(asset->numLods), 0, 6);
		}

		bool has_surface_flag(const XModel* asset, const unsigned int flag)
		{
			for (auto lod = 0; lod < get_lod_count(asset); lod++)
			{
				const auto* surfs = asset->lodInfo[lod].modelSurfs;
				for (auto i = 0; surfs && surfs->surfs && i < surfs->numsurfs; i++)
				{
					if (surfs->surfs[i].flags & flag)
					{
						return true;
					}
				}
			}

			return false;
		}

		unsigned short* convert_parent_list(const XModel* asset, utils::memory::allocator& allocator)
		{
			const auto child_bone_count = asset->numBones - asset->numRootBones;
			if (child_bone_count <= 0)
			{
				return nullptr;
			}

			const auto parent_list = allocator.allocate_array<unsigned short>(child_bone_count);
			for (auto i = 0; asset->parentList && i < child_bone_count; i++)
			{
				parent_list[i] = asset->parentList[i];
			}

			return parent_list;
		}

		using axis_corrections = std::unordered_map<std::string, cast::quat_t>;
		std::optional<axis_corrections> loaded_axis_corrections;
		bool axis_corrections_attempted = false;

		std::optional<axis_corrections> load_body_axis_corrections()
		{
			const auto h1_rig = bone_rig::load_h1_skeleton("body_usmc_desert_assault_mp");
			const auto s2_rig = bone_rig::load_s2_skeleton("usa_esc_m_assault_r1c1");
			if (!h1_rig || !s2_rig)
			{
				return {};
			}

			axis_corrections corrections;
			for (auto i = 0u; i < h1_rig->names.size(); i++)
			{
				if (const auto bone = s2_rig->find(h1_rig->names[i]); bone >= 0)
				{
					corrections.emplace(h1_rig->names[i], cast::quat_normalize(cast::quat_multiply(
						cast::quat_conjugate(h1_rig->bind_world[i].rotation), s2_rig->bind_world[bone].rotation)));
				}
			}

			return corrections;
		}

		const axis_corrections* body_axis_corrections()
		{
			if (!axis_corrections_attempted)
			{
				axis_corrections_attempted = true;
				loaded_axis_corrections = load_body_axis_corrections();
			}

			return loaded_axis_corrections ? &*loaded_axis_corrections : nullptr;
		}

		short encode_angle(const float value)
		{
			return static_cast<short>(std::clamp(std::lround(value * 32768.0f), -32767l, 32767l));
		}

		std::vector<bone_rig::transform> corrected_bind_pose(const XModel* asset)
		{
			const auto* corrections = body_axis_corrections();
			if (!corrections)
			{
				ZONETOOL_WARNING("xmodel \"%s\": body rig \"body_usmc_desert_assault_mp\" or \"usa_esc_m_assault_r1c1\" is not loaded, the model keeps the h1 bone axes",
					asset->name);
			}

			std::vector<bone_rig::transform> world(asset->numBones);
			for (auto i = 0; i < asset->numBones; i++)
			{
				world[i] = bone_rig::to_transform(asset->baseMat[i]);
				if (!corrections)
				{
					continue;
				}

				if (const auto correction = corrections->find(SL_ConvertToString(asset->boneNames[i])); correction != corrections->end())
				{
					world[i].rotation = cast::quat_normalize(cast::quat_multiply(world[i].rotation, correction->second));
				}
			}

			return world;
		}

		void convert_bind_pose(const XModel* asset, zonetool::s2::XModelBase* base, utils::memory::allocator& allocator)
		{
			const auto world = corrected_bind_pose(asset);
			const auto bone_count = static_cast<int>(asset->numBones);
			const auto root_count = static_cast<int>(asset->numRootBones);
			const auto child_count = bone_count - root_count;

			const auto mats = allocator.allocate_array<zonetool::s2::DObjAnimMat>(bone_count);
			for (auto i = 0; i < bone_count; i++)
			{
				std::ranges::copy(world[i].rotation.v, mats[i].quat);
				std::ranges::copy(world[i].position.v, mats[i].trans);
				mats[i].transWeight = 2.0f;
			}

			const auto angles = allocator.allocate_array<short>(4 * child_count);
			const auto positions = allocator.allocate_array<float>(3 * child_count);
			for (auto i = root_count; i < bone_count; i++)
			{
				const auto child = i - root_count;
				const auto parent = i - (asset->parentList ? asset->parentList[child] : 0);
				const auto local = bone_rig::compose(bone_rig::invert(world[parent]), world[i]);

				for (auto component = 0; component < 4; component++)
				{
					angles[child * 4 + component] = encode_angle(local.rotation.v[component]);
				}

				for (auto axis = 0; axis < 3; axis++)
				{
					positions[child * 3 + axis] = local.position.v[axis];
				}
			}

			base->baseMat = reinterpret_cast<char*>(mats);
			base->tagAngles = angles;
			base->tagPositions = positions;
		}

		void convert_bones(const XModel* asset, zonetool::s2::XModelBase* base, utils::memory::allocator& allocator)
		{
			base->numBones = asset->numBones;
			base->numRootBones = asset->numRootBones;
			base->numClientBones = 0;
			base->boneNames = retype<zonetool::s2::scr_string_t>(asset->boneNames);
			base->parentList = convert_parent_list(asset, allocator);
			base->partClassification = asset->partClassification;
			base->boneInfo = reinterpret_cast<char*>(asset->boneInfo);
			convert_bind_pose(asset, base, allocator);
		}

		void set_stock_lod_dist_sq(const XModel* asset, zonetool::s2::XModelBase* base)
		{
			const auto lod_count = get_lod_count(asset);
			const auto* lod0 = lod_count ? asset->lodInfo[0].modelSurfs : nullptr;
			const auto lod0_area = xsurface::surface_area(lod0);
			const auto lod0_tris = static_cast<float>(xsurface::triangle_count(lod0));
			for (auto i = 0; i < lod_count; i++)
			{
				const auto tris = xsurface::triangle_count(asset->lodInfo[i].modelSurfs);
				base->lodDistSq[i] = tris ? lod0_area * lod0_tris / static_cast<float>(tris) : 0.0f;
			}
		}

		void convert_lods(const XModel* asset, zonetool::s2::XModelBase* base, utils::memory::allocator& allocator)
		{
			for (auto& lod : base->lodInfo)
			{
				lod.dist = 1000000.0f;
			}

			const auto lod_count = get_lod_count(asset);
			for (auto i = 0; i < lod_count; i++)
			{
				const auto& lod = asset->lodInfo[i];
				auto& new_lod = base->lodInfo[i];
				new_lod.dist = lod.dist > 0.0f ? lod.dist : 1000000.0f;
				new_lod.numsurfs = lod.numsurfs;
				new_lod.surfIndex = lod.surfIndex;
				if (lod.modelSurfs)
				{
					new_lod.modelSurfs = make_stub<zonetool::s2::XModelSurfs>(
						converted_name(zonetool::s2::ASSET_TYPE_XMODEL_SURFS, lod.modelSurfs->name), allocator);
				}
			}

			set_stock_lod_dist_sq(asset, base);
			base->numLods = static_cast<unsigned char>(lod_count);
			base->collLod = asset->collLod;
			base->forcedLod = 0xFE;
			base->lodRampType = asset->lodRampType;
			base->lodDistScale = 1.0f;
			base->useLodInfoDist = 1;
		}

		void convert_coll_surfs(const XModel* asset, zonetool::s2::XModelBase* base, utils::memory::allocator& allocator)
		{
			const auto count = asset->collSurfs ? asset->numCollSurfs : 0;
			base->collSurfCount = static_cast<unsigned short>(count);
			if (!count)
			{
				return;
			}

			const auto coll_surfs = allocator.allocate_array<zonetool::s2::XModelCollSurf>(count);
			for (auto i = 0; i < count; i++)
			{
				const auto& surf = asset->collSurfs[i];
				auto& new_surf = coll_surfs[i];
				new_surf.bounds = std::bit_cast<zonetool::s2::Bounds>(surf.bounds);
				new_surf.boneIdx = surf.boneIdx;
				new_surf.contents = static_cast<std::uint32_t>(surf.contents);
				new_surf.surfFlags = convert_surface_flags(static_cast<std::uint32_t>(surf.surfFlags));
			}

			base->collSurfs = reinterpret_cast<char*>(coll_surfs);
		}

		void convert_materials(const XModel* asset, zonetool::s2::XModel* model, utils::memory::allocator& allocator)
		{
			model->numMaterials = asset->numsurfs;
			model->materialHandles = allocator.allocate_array<zonetool::s2::Material*>(asset->numsurfs);
			model->materialIndices = allocator.allocate_array<unsigned short>(asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				const auto material = asset->materialHandles ? asset->materialHandles[i] : nullptr;
				model->materialHandles[i] = make_stub<zonetool::s2::Material>(material && material->name ? material->name : "$default", allocator);
				model->materialIndices[i] = 0xFFFF;
			}

			model->runtimeSize = static_cast<std::uint16_t>(sizeof(zonetool::s2::XModel)
				+ asset->numsurfs * (sizeof(zonetool::s2::Material*) + sizeof(std::uint16_t)));
		}

		zonetool::s2::XModel* convert(const XModel* asset, utils::memory::allocator& allocator)
		{
			const auto base = allocator.allocate<zonetool::s2::XModelBase>();
			base->name = allocator.duplicate_string(std::string(asset->name) + "_h1base");
			base->scale = asset->scale;
			std::ranges::fill(base->unkScale, 1.0f);
			convert_bones(asset, base, allocator);
			convert_lods(asset, base, allocator);
			convert_coll_surfs(asset, base, allocator);

			base->contents = asset->contents;
			base->radius = asset->radius;
			std::memcpy(base->boundsMidPoint, asset->bounds.midPoint, sizeof(base->boundsMidPoint));
			std::memcpy(base->boundsHalfSize, asset->bounds.halfSize, sizeof(base->boundsHalfSize));
			base->boundsRadius = std::hypot(base->boundsHalfSize[0], base->boundsHalfSize[1], base->boundsHalfSize[2]);
			base->cullSizeScale = 0.0f;
			base->memUsage = static_cast<unsigned int>(asset->memUsage);
			base->physPreset = asset->physPreset ? physpreset::reference(asset->physPreset, asset, allocator) : nullptr;
			base->physCollmap = physcollmap::reference(asset->physCollmap, allocator);
			base->defaultPhysRigIndex = -1;
			base->lightingBone = 0xFFFF;
			base->lightingProbeSpacing = 96.0f;

			const auto sways = has_surface_flag(asset, SURF_FLAG_REACTIVE_MOTION);
			if (sways)
			{
				std::ranges::copy(grass_sway_params, base->swayParams);
			}

			const auto model = allocator.allocate<zonetool::s2::XModel>();
			model->name = converted_name(zonetool::s2::ASSET_TYPE_XMODEL, asset->name);
			model->base = base;
			model->flags = zonetool::s2::XMODEL_FLAG_UNK_0x800
				| (asset->flags & zonetool::s2::XMODEL_FLAG_UNK_0x8)
				| (has_surface_flag(asset, SURF_FLAG_LIGHTMAP_COORDS) ? 0x6000u : 0u)
				| (sways ? zonetool::s2::XMODEL_FLAG_SWAY : 0u);
			convert_materials(asset, model, allocator);
			return model;
		}

		void dump_lod_surfs(const XModel* asset)
		{
			for (auto i = 0; i < get_lod_count(asset); i++)
			{
				if (asset->lodInfo[i].modelSurfs)
				{
					xsurface::dump(asset->lodInfo[i].modelSurfs);
				}
			}
		}

		void dump_materials(const XModel* asset)
		{
			for (auto i = 0; asset->materialHandles && i < asset->numsurfs; i++)
			{
				if (const auto material = asset->materialHandles[i]; material && material->name)
				{
					material::dump(material);
				}
			}
		}

		void write_model(const XModel* asset, const char* name, utils::memory::allocator& allocator)
		{
			xsurface::assign_base_vert_indices(asset);
			dump_lod_surfs(asset);

			const auto converted = convert(asset, allocator);
			converted->name = name;
			zonetool::s2::xmodel_base::dump(converted->base);
			zonetool::s2::xmodel::dump(converted);
			add_csv_row("xmodel", converted->name);
			dump_materials(asset);
		}

		int find_bone(const XModel* asset, const scr_string_t name)
		{
			for (auto i = 0; i < asset->numBones; i++)
			{
				if (asset->boneNames[i] == name)
				{
					return i;
				}
			}

			return -1;
		}

		std::optional<std::pair<std::string, std::string>> split_model_name(const std::string& name, const std::regex& pattern)
		{
			std::smatch match;
			if (!std::regex_match(name, match, pattern))
			{
				return {};
			}

			return std::make_pair(match[1].str(), match[2].str());
		}

		int suffix_score(const std::string& model_suffix, const std::string& part_suffix)
		{
			return part_suffix == model_suffix ? 2 : model_suffix.ends_with(part_suffix) ? 1 : 0;
		}

		std::vector<default_part> find_default_parts(const XModel* asset)
		{
			const auto weapon = split_model_name(asset->name, weapon_model_name);
			if (!asset->boneNames || !asset->baseMat || !weapon)
			{
				return {};
			}

			std::map<int, std::pair<int, const XModel*>> best_parts;
			db_enum_assets(ASSET_TYPE_XMODEL, [&](void* header)
			{
				const auto* part = static_cast<const XModel*>(header);
				const auto split = split_model_name(part->name, default_part_name);
				if (!split || split->first != weapon->first || part->numBones != 1 || !part->boneNames || !part->baseMat || !get_lod_count(part))
				{
					return;
				}

				const auto tag = find_bone(asset, part->boneNames[0]);
				const auto score = suffix_score(weapon->second, split->second);
				const auto best = best_parts.find(tag);
				if (tag >= 0 && (best == best_parts.end() || score > best->second.first
					|| (score == best->second.first && std::string_view(part->name) < best->second.second->name)))
				{
					best_parts[tag] = {score, part};
				}
			});

			std::vector<default_part> parts;
			for (const auto& [tag, best] : best_parts)
			{
				const auto* part = best.second;
				const auto placement = bone_rig::compose(bone_rig::to_transform(asset->baseMat[tag]),
					bone_rig::invert(bone_rig::to_transform(part->baseMat[0])));
				if (std::abs(placement.rotation.v[3]) < 0.9999f)
				{
					ZONETOOL_WARNING("xmodel \"%s\": default part \"%s\" is rotated on its tag, it is not added", asset->name, part->name);
					continue;
				}

				parts.push_back({part, std::to_array(placement.position.v)});
			}

			return parts;
		}

		void offset_bounds(Bounds& bounds, const std::array<float, 3>& offset)
		{
			for (auto axis = 0; axis < 3; axis++)
			{
				bounds.midPoint[axis] += offset[axis];
			}
		}

		XModel* place_default_part(const XModel* asset, const default_part& part, utils::memory::allocator& allocator)
		{
			const auto placed = allocator.allocate<XModel>();
			*placed = *part.model;
			placed->boneNames = copy_array(asset->boneNames, 1, allocator);
			placed->baseMat = copy_array(asset->baseMat, 1, allocator);

			const auto distance = std::hypot(part.offset[0], part.offset[1], part.offset[2]);
			placed->radius += distance;
			offset_bounds(placed->bounds, part.offset);

			placed->boneInfo = copy_array(part.model->boneInfo, 1, allocator);
			if (placed->boneInfo)
			{
				offset_bounds(placed->boneInfo->bounds, part.offset);
				placed->boneInfo->radiusSquared = std::pow(std::sqrt(placed->boneInfo->radiusSquared) + distance, 2.0f);
			}

			placed->collSurfs = copy_array(part.model->collSurfs, part.model->numCollSurfs, allocator);
			for (auto i = 0; placed->collSurfs && i < placed->numCollSurfs; i++)
			{
				offset_bounds(placed->collSurfs[i].bounds, part.offset);
			}

			return placed;
		}

		const char* write_default_part(const XModel* asset, const default_part& part, utils::memory::allocator& allocator)
		{
			const auto name = converted_name(zonetool::s2::ASSET_TYPE_XMODEL, part.model->name);
			if (dumped_models.insert(part.model->name).second)
			{
				const auto placed = place_default_part(asset, part, allocator);
				for (auto i = 0; i < get_lod_count(placed); i++)
				{
					xsurface::set_position_offset(placed->lodInfo[i].modelSurfs, part.offset);
				}

				write_model(placed, name, allocator);
			}

			return name;
		}

		void write_variant_model(const XModel* asset, const char* name, const std::vector<const char*>& parts, utils::memory::allocator& allocator)
		{
			const auto model = allocator.allocate<zonetool::s2::XModel>();
			model->name = name;
			model->flags = zonetool::s2::XMODEL_FLAG_VARIANTS | zonetool::s2::XMODEL_FLAG_UNK_0x800 | 0x6000u | (asset->flags & zonetool::s2::XMODEL_FLAG_UNK_0x8);
			model->variantCount = static_cast<unsigned char>(parts.size());
			model->variants = allocator.allocate_array<zonetool::s2::XModelVariant>(parts.size());
			for (auto i = 0u; i < parts.size(); i++)
			{
				model->variants[i].model = make_stub<zonetool::s2::XModel>(parts[i], allocator);
				model->variants[i].unk_0x8[3] = 1.0f;
				model->variants[i].name = static_cast<zonetool::s2::scr_string_t>(SL_GetString(""));
			}

			model->runtimeSize = static_cast<std::uint16_t>(sizeof(zonetool::s2::XModel) + parts.size() * sizeof(zonetool::s2::XModelVariant));
			zonetool::s2::xmodel::dump(model);
			add_csv_row("xmodel", model->name);
		}

		std::vector<std::string> zone_models(const std::string& zone_name)
		{
			const auto path = game_files::find_zone(zone_name);
			if (!path)
			{
				return {};
			}

			const auto database = db_create();
			db_scope _(*database);
			const auto zone = load_zone(*path);

			std::vector<std::string> names;
			for (const auto& asset : zone->assets)
			{
				if (asset.type == ASSET_TYPE_XMODEL && !asset.referenced)
				{
					names.emplace_back(get_asset_name(asset.type, asset.header));
				}
			}

			return names;
		}
	}

	void dump(XModel* asset)
	{
		if (!dumped_models.insert(asset->name).second)
		{
			return;
		}

		utils::memory::allocator allocator;
		const auto name = converted_name(zonetool::s2::ASSET_TYPE_XMODEL, asset->name);
		const auto parts = find_default_parts(asset);
		if (parts.empty())
		{
			write_model(asset, name, allocator);
			return;
		}

		std::vector<const char*> variants{allocator.duplicate_string(std::string(name) + "_body")};
		write_model(asset, variants.front(), allocator);
		for (const auto& part : parts)
		{
			variants.emplace_back(write_default_part(asset, part, allocator));
		}

		write_variant_model(asset, name, variants, allocator);
	}

	zonetool::s2::XModel* reference(const XModel* asset, utils::memory::allocator& allocator)
	{
		if (!asset || !asset->name)
		{
			return nullptr;
		}

		dump_used(asset->name);
		return make_stub<zonetool::s2::XModel>(converted_name(zonetool::s2::ASSET_TYPE_XMODEL, asset->name), allocator);
	}

	void dump_used(const char* name)
	{
		auto* loaded = db_find_x_asset_header_safe(ASSET_TYPE_XMODEL, name).model;
		if (loaded && loaded->numLods > 0 && loaded->lodInfo[0].modelSurfs)
		{
			dump(loaded);
			return;
		}

		add_csv_row("xmodel", converted_name(zonetool::s2::ASSET_TYPE_XMODEL, name));
	}

	std::optional<std::string> find_built_in_scope(const std::string& weapon, const XModel* gun_model)
	{
		const auto gun = gun_model && gun_model->name ? split_model_name(gun_model->name, weapon_model_name) : std::nullopt;
		if (!gun)
		{
			return {};
		}

		const auto kind = gun->first.ends_with("_vm") ? "vm" : "wm";
		std::optional<std::string> best;
		auto best_score = 0;
		for (const auto& name : zone_models(std::format("mp_{}_{}_base_tr", kind, weapon)))
		{
			const auto scope = split_model_name(name.ends_with("_mp") ? name.substr(0, name.size() - 3) : name, scope_name);
			if (!scope || scope->first != gun->first)
			{
				continue;
			}

			const auto score = suffix_score(gun->second, scope->second);
			if (!best || score > best_score || (score == best_score && name < *best))
			{
				best = name;
				best_score = score;
			}
		}

		return best;
	}

	void clear()
	{
		dumped_models.clear();
		converted_names.clear();
		loaded_axis_corrections.reset();
		axis_corrections_attempted = false;
	}

	const char* converted_name(const zonetool::s2::XAssetType type, const char* name)
	{
		const auto key = std::make_pair(type, std::string(name));
		if (const auto found = converted_names.find(key); found != converted_names.end())
		{
			return found->second.data();
		}

		auto converted = std::string(name);
		if (stock_zone::is_stock_asset(type, name))
		{
			converted += "_h1";
		}

		return converted_names.emplace(key, std::move(converted)).first->second.data();
	}
}
