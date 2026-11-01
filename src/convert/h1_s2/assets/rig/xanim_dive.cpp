#include <std_include.hpp>

#include "../../include.hpp"
#include "bone_rig.hpp"
#include "xanim_dive.hpp"
#include "xanim_retarget.hpp"

namespace zonetool::h1::converter::s2::xanim_dive
{
	namespace
	{
		namespace cs = zonetool::cast;
		using namespace bone_rig;

		constexpr auto dive_free_hand_distance = 4.0f; // the stock left hand counts as off the gun this far from its idle grip
		constexpr auto dive_hand_speed = 3.0f; // units per frame, about the fastest the stock left hand moves
		constexpr auto dive_memory_size = 16ull * 1024 * 1024;

		struct dive_donor
		{
			std::vector<transform> idle;
			std::array<stock_anim, 2> anims;
		};

		std::unordered_map<std::string, std::optional<dive_donor>> dive_donors;

		const dive_donor* get_dive_donor(const skeleton& rig, const std::string& weapon)
		{
			const auto [entry, inserted] = dive_donors.try_emplace(weapon);
			if (inserted)
			{
				auto dive = load_stock_anim(std::format("va_{}_dive", weapon));
				if (!dive)
				{
					dive = load_stock_anim(std::format("va_{}_dive_in", weapon));
				}

				auto end = load_stock_anim(std::format("va_{}_dive_end", weapon));
				const auto idle = load_stock_anim(std::format("va_{}_idle", weapon));
				if (dive && end && idle)
				{
					entry->second = dive_donor{forward_kinematics(rig, sample_locals(rig, collect_keys(idle->animation), 0)), {std::move(*dive), std::move(*end)}};
				}
				else
				{
					ZONETOOL_WARNING("the stock s2 dive anims of \"%s\" are not loaded, h1 weapons get no dive anims from them", weapon.data());
				}
			}

			return entry->second ? &*entry->second : nullptr;
		}

		float hand_free(const skeleton& rig, const dive_donor& donor, const std::vector<transform>& stock)
		{
			const auto gun = rig.find("tag_weapon");
			const auto wrist = rig.find(side_bone("wrist", "le"));
			const auto grip = compose(invert(donor.idle[gun]), donor.idle[wrist]).position;
			const auto hand = compose(invert(stock[gun]), stock[wrist]).position;
			return smoothstep(length(sub(hand, grip)) / dive_free_hand_distance); // the stock left hand lets go of the gun to break the fall
		}

		void limit_rate(std::vector<float>& values, const float rate)
		{
			for (auto i = 1u; i < values.size(); i++)
			{
				values[i] = std::min(values[i], values[i - 1] + rate);
			}

			for (auto i = values.size(); i > 1; i--)
			{
				values[i - 2] = std::min(values[i - 2], values[i - 1] + rate);
			}
		}

		std::vector<transform> dive_pose(const skeleton& rig, const std::vector<transform>& idle_locals, const std::vector<transform>& idle_world,
			const dive_donor& donor, const std::vector<transform>& stock_locals, const float free)
		{
			const auto torso = rig.find(body_tag);
			const auto clavicle = rig.find(side_bone("clavicle", "le"));
			const auto shoulder = rig.find(side_bone("shoulder", "le"));
			const auto elbow = rig.find(side_bone("elbow", "le"));
			const auto wrist = rig.find(side_bone("wrist", "le"));
			const auto stock = forward_kinematics(rig, stock_locals);
			const auto motion = compose(stock[torso], invert(donor.idle[torso])); // the stock gun, right arm and body move as one with tag_torso

			std::vector<transform> world(rig.names.size());
			for (auto i = 0u; i < rig.names.size(); i++)
			{
				const auto parent = rig.parents[i] < 0 ? transform{} : world[rig.parents[i]];
				if (std::ranges::find(camera_bones, rig.names[i]) != camera_bones.end())
				{
					world[i] = compose(parent, stock_locals[i]);
				}
				else if (rig.has_ancestor(static_cast<int>(i), torso))
				{
					world[i] = compose(motion, idle_world[i]);
				}
				else
				{
					world[i] = compose(parent, idle_locals[i]);
				}
			}

			if (free <= 0.0f)
			{
				return world;
			}

			const auto rigid_normal = bend_normal(world[shoulder].position, world[elbow].position, world[wrist].position);
			const auto pole = normalize(lerp(rigid_normal, bend_normal(stock[shoulder].position, stock[elbow].position, stock[wrist].position), free));
			const auto start = lerp(world[shoulder].position, stock[shoulder].position, free);
			const auto arm = solve_two_bone(start, lerp(world[wrist].position, stock[wrist].position, free), length(rig.bind_local[elbow].position),
				length(rig.bind_local[wrist].position), length(pole) > epsilon ? pole : rigid_normal);

			const auto upper_arm = blend(world[shoulder], stock[shoulder], free);
			const auto forearm = blend(world[elbow], stock[elbow], free);
			world[clavicle] = blend(world[clavicle], stock[clavicle], free);
			world[shoulder] = swing_to(upper_arm, rig.bind_local[elbow].position, start, arm.elbow);
			world[elbow] = swing_to(forearm, rig.bind_local[wrist].position, arm.elbow, arm.wrist);
			world[wrist] = {blend(world[wrist], stock[wrist], free).rotation, arm.wrist};
			for (auto i = 0u; i < rig.names.size(); i++)
			{
				const auto bone = static_cast<int>(i);
				if (bone != clavicle && bone != shoulder && bone != elbow && bone != wrist && rig.has_ancestor(bone, clavicle))
				{
					world[i] = compose(world[rig.parents[i]], blend(idle_locals[i], stock_locals[i], free));
				}
			}

			return world;
		}
	}

	std::optional<std::array<zonetool::s2::XAnimParts*, 2>> build(const zonetool::s2::XAnimParts* idle, const std::string& donor,
		const std::array<std::string, 2>& names, std::optional<zone_memory>& memory)
	{
		const auto* arm_rig = xanim_retarget::s2_arm_rig();
		if (!arm_rig || !arm_rig->has("tag_weapon") || !arm_rig->has(side_bone("clavicle", "le")))
		{
			return {};
		}

		const auto* stock = get_dive_donor(*arm_rig, donor);
		if (!stock)
		{
			return {};
		}

		const auto& rig = *arm_rig;
		const auto animation = zonetool::s2::cast_xanim::to_animation(idle);
		const auto idle_keys = collect_keys(animation);
		const auto idle_locals = sample_locals(rig, idle_keys, 0);
		const auto idle_world = forward_kinematics(rig, idle_locals);
		const auto wrist = rig.find(side_bone("wrist", "le"));
		const auto travel = length(sub(idle_world[wrist].position, stock->idle[wrist].position));

		std::array<std::vector<std::vector<transform>>, 2> stock_poses;
		std::vector<float> free;
		for (auto i = 0u; i < stock_poses.size(); i++)
		{
			const auto keys = collect_keys(stock->anims[i].animation);
			for (auto frame = 0u; frame <= stock->anims[i].last_frame; frame++)
			{
				const auto& locals = stock_poses[i].emplace_back(sample_locals(rig, keys, frame));
				free.emplace_back(hand_free(rig, *stock, forward_kinematics(rig, locals)));
			}
		}

		limit_rate(free, travel > dive_hand_speed ? dive_hand_speed / travel : 1.0f); // an h1 left hand far from the stock one takes longer to get there

		auto& dive_memory = memory.emplace(dive_memory_size);
		std::array<zonetool::s2::XAnimParts*, 2> anims{};
		for (auto i = 0u, first = 0u; i < anims.size(); first += static_cast<std::uint32_t>(stock_poses[i].size()), i++)
		{
			std::vector<std::vector<transform>> poses;
			for (auto frame = 0u; frame < stock_poses[i].size(); frame++)
			{
				poses.emplace_back(to_locals(rig, dive_pose(rig, idle_locals, idle_world, *stock, stock_poses[i][frame], free[first + frame])));
			}

			anims[i] = zonetool::s2::cast_xanim::from_animation(pose_animation(rig, animation, idle_keys, stock->anims[i].animation, poses, names[i]), &dive_memory);
		}

		return anims;
	}

	void clear()
	{
		dive_donors.clear();
	}
}
