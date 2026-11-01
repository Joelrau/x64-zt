#include <std_include.hpp>

#include "../../include.hpp"
#include "bone_rig.hpp"
#include "xanim_melee.hpp"
#include "xanim_retarget.hpp"

namespace zonetool::h1::converter::s2::xanim_melee
{
	namespace
	{
		namespace cs = zonetool::cast;
		using namespace bone_rig;

		constexpr std::array melee_sides{"le", "ri"};
		constexpr std::array melee_chain{"clavicle", "shoulder", "elbow", "wrist"};
		constexpr auto melee_reach = 0.99f; // how far an arm stretches before its shoulder moves along
		constexpr auto stock_pose_share = 0.5f; // share of its biggest move from which the stock gun spot and shoulders take over fully
		constexpr auto regrip_distance = 2.0f; // how far a stock hand slides on its gun before it fully re-grips it
		constexpr auto free_hand_in = 0.2f; // share of the melee in which a free hand drops away from the gun
		constexpr auto free_hand_out = 0.35f; // share of the melee in which it comes back to its grip
		constexpr auto melee_memory_size = 8ull * 1024 * 1024;

		struct melee_donor
		{
			std::vector<transform> idle_locals;
			std::vector<transform> idle_world;
			stock_anim melee;
			std::vector<std::vector<transform>> locals; // the stock melee pose per frame
			std::vector<std::vector<transform>> world;
			std::vector<float> weights; // per frame, how much the pose takes the stock gun spot and shoulders
			std::vector<std::array<float, melee_sides.size()>> regrips; // per frame and hand, how much the hand takes the stock grip
			std::array<bool, melee_sides.size()> free_hands{};
		};

		struct h1_pose
		{
			std::vector<transform> locals;
			std::vector<transform> world;
			transform gun_offset; // the h1 gun in the stock gun space, as the idles hold them
			transform grip_offset; // the same with the h1 right grip on the stock right grip
		};

		std::unordered_map<std::string, std::optional<melee_donor>> melee_donors;

		// a stock hand that moves less with the body than along its gun lets go of the gun, like the pistol support hand
		bool is_free_hand(const skeleton& rig, const melee_donor& donor, const char* side)
		{
			const auto wrist = rig.find(side_bone("wrist", side));
			const auto moved = [&](const int space)
			{
				const auto rest = compose(invert(donor.idle_world[space]), donor.idle_world[wrist]).position;
				auto result = 0.0f;
				for (const auto& world : donor.world)
				{
					result = std::max(result, length(sub(compose(invert(world[space]), world[wrist]).position, rest)));
				}

				return result;
			};

			return moved(rig.find(body_tag)) < moved(rig.find("tag_weapon"));
		}

		// the bigger of the stock gun move and turn since its idle as a share of their peaks, so the h1 idle offsets fade out as the gun goes out
		std::vector<float> stock_weights(const skeleton& rig, const melee_donor& donor)
		{
			const auto gun = rig.find("tag_weapon");
			const auto& rest = donor.idle_world[gun];
			std::vector<std::pair<float, float>> moves;
			auto peak_move = epsilon;
			auto peak_turn = epsilon;
			for (const auto& world : donor.world)
			{
				const auto move = length(sub(world[gun].position, rest.position));
				const auto turn = 2.0f * std::acos(std::min(1.0f, std::abs(cs::quat_dot(world[gun].rotation, rest.rotation))));
				moves.emplace_back(move, turn);
				peak_move = std::max(peak_move, move);
				peak_turn = std::max(peak_turn, turn);
			}

			std::vector<float> weights;
			for (const auto& [move, turn] : moves)
			{
				weights.emplace_back(smoothstep(std::max(move / peak_move, turn / peak_turn) / stock_pose_share));
			}

			return weights;
		}

		// a gun hand that slides away from its stock idle grip re-grips the gun there, with the stock hand turn and finger shape
		std::vector<std::array<float, melee_sides.size()>> stock_regrips(const skeleton& rig, const melee_donor& donor)
		{
			const auto gun = rig.find("tag_weapon");
			std::vector<std::array<float, melee_sides.size()>> regrips(donor.world.size());
			for (auto side = 0u; side < melee_sides.size(); side++)
			{
				const auto wrist = rig.find(side_bone("wrist", melee_sides[side]));
				const auto rest = compose(invert(donor.idle_world[gun]), donor.idle_world[wrist]).position;
				for (auto frame = 0u; frame < donor.world.size() && !donor.free_hands[side]; frame++)
				{
					const auto moved = compose(invert(donor.world[frame][gun]), donor.world[frame][wrist]).position;
					regrips[frame][side] = smoothstep(length(sub(moved, rest)) / regrip_distance);
				}
			}

			return regrips;
		}

		const melee_donor* get_melee_donor(const skeleton& rig, const std::string& weapon)
		{
			const auto [entry, inserted] = melee_donors.try_emplace(weapon);
			if (inserted)
			{
				auto melee = load_stock_anim(std::format("va_{}_melee", weapon));
				if (!melee)
				{
					melee = load_stock_anim(std::format("va_{}_melee_a", weapon));
				}

				const auto idle = load_stock_anim(std::format("va_{}_idle", weapon));
				if (melee && idle)
				{
					auto& donor = entry->second.emplace();
					donor.idle_locals = sample_locals(rig, collect_keys(idle->animation), 0);
					donor.idle_world = forward_kinematics(rig, donor.idle_locals);
					const auto keys = collect_keys(melee->animation);
					for (auto frame = 0u; frame <= melee->last_frame; frame++)
					{
						donor.world.emplace_back(forward_kinematics(rig, donor.locals.emplace_back(sample_locals(rig, keys, frame))));
					}

					donor.melee = std::move(*melee);
					donor.weights = stock_weights(rig, donor);
					for (auto side = 0u; side < melee_sides.size(); side++)
					{
						donor.free_hands[side] = is_free_hand(rig, donor, melee_sides[side]);
					}

					donor.regrips = stock_regrips(rig, donor);
				}
				else
				{
					ZONETOOL_WARNING("the stock s2 melee anim of \"%s\" is not loaded, h1 weapons get no melee anim from it", weapon.data());
				}
			}

			return entry->second ? &*entry->second : nullptr;
		}

		// the stock move since the stock idle, put on top of the h1 idle
		transform carry_world(const transform& stock, const transform& stock_idle, const transform& idle)
		{
			return compose(compose(stock, invert(stock_idle)), idle);
		}

		transform carry_local(const transform& stock, const transform& stock_idle, const transform& idle)
		{
			return {multiply(idle.rotation, multiply(inverse(stock_idle.rotation), stock.rotation)), add(idle.position, sub(stock.position, stock_idle.position))};
		}

		// the same for a bone inside a space bone: a hand keeps its offset on the gun or by the body
		transform carry_in(const std::vector<transform>& stock, const std::vector<transform>& stock_idle, const std::vector<transform>& idle,
			const int space, const int bone)
		{
			const auto moved = compose(invert(stock[space]), stock[bone]);
			const auto rest = compose(invert(stock_idle[space]), stock_idle[bone]);
			const auto own = compose(invert(idle[space]), idle[bone]);
			return {multiply(multiply(moved.rotation, inverse(rest.rotation)), own.rotation), add(own.position, sub(moved.position, rest.position))};
		}

		// how much an arm follows the stock arm: a free hand only while it is off the gun, it drops away at the start and is back on its grip at the end
		float arm_follow(const melee_donor& donor, const std::size_t side, const std::uint32_t frame)
		{
			if (!donor.free_hands[side])
			{
				return 1.0f;
			}

			const auto t = donor.melee.last_frame ? static_cast<float>(frame) / static_cast<float>(donor.melee.last_frame) : 0.0f;
			return smoothstep(t / free_hand_in) * smoothstep((1.0f - t) / free_hand_out);
		}

		void solve_arm(const skeleton& rig, const std::size_t side, const melee_donor& donor, const std::uint32_t frame, const h1_pose& h1,
			std::vector<transform>& world)
		{
			const auto torso = rig.find(body_tag);
			const auto gun = rig.find("tag_weapon");
			const auto clavicle = rig.find(side_bone("clavicle", melee_sides[side]));
			const auto shoulder = rig.find(side_bone("shoulder", melee_sides[side]));
			const auto elbow = rig.find(side_bone("elbow", melee_sides[side]));
			const auto wrist = rig.find(side_bone("wrist", melee_sides[side]));
			const auto& stock = donor.world[frame];
			const auto follow = arm_follow(donor, side, frame);
			const auto weight = donor.weights[frame] * follow;

			// the hand moves along its gun like the stock hand (the re-grips), a free hand leaves its grip for the stock rest spot by the body
			auto hand = compose(world[gun], carry_in(stock, donor.idle_world, h1.world, gun, wrist));
			if (donor.free_hands[side])
			{
				const auto grip = compose(world[gun], compose(invert(h1.world[gun]), h1.world[wrist]));
				hand = blend(grip, compose(world[torso], carry_in(stock, donor.idle_world, h1.world, torso, wrist)), follow);
			}
			else if (const auto regrip = donor.regrips[frame][side]; regrip > 0.0f)
			{
				const auto stock_turn = compose(world[gun], compose(invert(stock[gun]), stock[wrist])).rotation; // the stock hand turn on the h1 gun
				hand.rotation = cs::quat_normalize(cs::quat_slerp(hand.rotation, stock_turn, regrip));
			}

			// the h1 shoulders sit lower and closer in, so they go to the stock spots while the gun is out and the arms reach like the stock arms
			auto start = add(stock[shoulder].position, scale(sub(h1.world[shoulder].position, donor.idle_world[shoulder].position), 1.0f - weight));
			const auto upper = length(rig.bind_local[elbow].position);
			const auto lower = length(rig.bind_local[wrist].position);
			const auto reach = sub(hand.position, start);
			if (const auto excess = length(reach) - melee_reach * (upper + lower); excess > 0.0f)
			{
				start = add(start, scale(normalize(reach), excess)); // the h1 grip is out of reach, so the shoulder follows like stock shoulders do
			}

			// the bend plane and the arm twist turn like the stock arm, and are the stock ones while the gun is out
			const auto stock_pole = bend_normal(stock[shoulder].position, stock[elbow].position, stock[wrist].position);
			const auto idle_pole = bend_normal(h1.world[shoulder].position, h1.world[elbow].position, h1.world[wrist].position);
			const auto turn = multiply(stock[shoulder].rotation, inverse(donor.idle_world[shoulder].rotation));
			auto pole = normalize(lerp(idle_pole, lerp(cs::quat_rotate(turn, idle_pole), stock_pole, weight), follow));
			if (length(pole) <= epsilon)
			{
				pole = stock_pole;
			}

			const auto arm = solve_two_bone(start, hand.position, upper, lower, pole);
			const auto upper_arm = blend(carry_world(stock[shoulder], donor.idle_world[shoulder], h1.world[shoulder]), stock[shoulder], weight);
			const auto forearm = blend(carry_world(stock[elbow], donor.idle_world[elbow], h1.world[elbow]), stock[elbow], weight);
			const auto collar = carry_local(donor.locals[frame][clavicle], donor.idle_locals[clavicle], h1.locals[clavicle]);
			world[clavicle] = compose(world[rig.parents[clavicle]], blend(h1.locals[clavicle], collar, follow));
			world[shoulder] = swing_to(blend(h1.world[shoulder], upper_arm, follow), rig.bind_local[elbow].position, start, arm.elbow);
			world[elbow] = swing_to(blend(h1.world[elbow], forearm, follow), rig.bind_local[wrist].position, arm.elbow, arm.wrist);
			world[wrist] = {hand.rotation, arm.wrist};
		}

		std::vector<transform> melee_pose(const skeleton& rig, const melee_donor& donor, const std::uint32_t frame, const h1_pose& h1)
		{
			const auto torso = rig.find(body_tag);
			const auto gun = rig.find("tag_weapon");
			const auto& stock_locals = donor.locals[frame];
			const auto& stock = donor.world[frame];
			const auto motion = compose(stock[torso], invert(donor.idle_world[torso]));

			std::vector<transform> world(rig.names.size());
			for (auto i = 0u; i < rig.names.size(); i++)
			{
				const auto bone = static_cast<int>(i);
				const auto parent = rig.parents[i] < 0 ? transform{} : world[rig.parents[i]];
				if (std::ranges::find(camera_bones, rig.names[i]) != camera_bones.end())
				{
					world[i] = compose(parent, carry_local(stock_locals[i], donor.idle_locals[i], h1.locals[i]));
				}
				else if (bone == gun)
				{
					world[i] = compose(stock[gun], blend(h1.gun_offset, h1.grip_offset, donor.weights[frame])); // the h1 gun hangs off the stock gun
				}
				else if (rig.has_ancestor(bone, gun) || !rig.has_ancestor(bone, torso))
				{
					world[i] = compose(parent, h1.locals[i]);
				}
				else
				{
					world[i] = compose(motion, h1.world[i]);
				}
			}

			std::vector<int> chain;
			for (auto side = 0u; side < melee_sides.size(); side++)
			{
				solve_arm(rig, side, donor, frame, h1, world);
				for (const auto* name : melee_chain)
				{
					chain.emplace_back(rig.find(side_bone(name, melee_sides[side])));
				}
			}

			for (auto i = 0u; i < rig.names.size(); i++)
			{
				const auto bone = static_cast<int>(i);
				for (auto side = 0u; side < melee_sides.size(); side++)
				{
					if (rig.has_ancestor(bone, rig.find(side_bone("clavicle", melee_sides[side]))) && std::ranges::find(chain, bone) == chain.end())
					{
						const auto local = blend(h1.locals[i], carry_local(stock_locals[i], donor.idle_locals[i], h1.locals[i]), arm_follow(donor, side, frame));
						world[i] = compose(world[rig.parents[i]], blend(local, stock_locals[i], donor.regrips[frame][side])); // fingers and twist bones
					}
				}
			}

			return world;
		}
	}

	zonetool::s2::XAnimParts* build(const zonetool::s2::XAnimParts* idle, const std::string& donor, const std::string& name,
		std::optional<zone_memory>& memory)
	{
		const auto* arm_rig = xanim_retarget::s2_arm_rig();
		const auto has_arms = arm_rig && std::ranges::all_of(melee_sides, [&](const char* side)
		{
			return std::ranges::all_of(melee_chain, [&](const char* bone) { return arm_rig->has(side_bone(bone, side)); });
		});

		if (!has_arms || !arm_rig->has("tag_weapon") || !arm_rig->has(body_tag))
		{
			return nullptr;
		}

		const auto* stock = get_melee_donor(*arm_rig, donor);
		if (!stock)
		{
			return nullptr;
		}

		const auto& rig = *arm_rig;
		const auto gun = rig.find("tag_weapon");
		const auto grip = rig.find(side_bone("wrist", "ri"));
		const auto animation = zonetool::s2::cast_xanim::to_animation(idle);
		const auto idle_keys = collect_keys(animation);
		auto locals = sample_locals(rig, idle_keys, 0);
		auto world = forward_kinematics(rig, locals);
		const auto gun_offset = compose(invert(stock->idle_world[gun]), world[gun]);
		const auto stock_grip = compose(invert(stock->idle_world[gun]), stock->idle_world[grip]).position;
		const auto own_grip = compose(invert(world[gun]), world[grip]).position;
		const h1_pose h1{std::move(locals), std::move(world), gun_offset, {gun_offset.rotation, sub(stock_grip, cs::quat_rotate(gun_offset.rotation, own_grip))}};

		std::vector<std::vector<transform>> poses;
		for (auto frame = 0u; frame <= stock->melee.last_frame; frame++)
		{
			poses.emplace_back(to_locals(rig, melee_pose(rig, *stock, frame, h1)));
		}

		auto& melee_memory = memory.emplace(melee_memory_size);
		return zonetool::s2::cast_xanim::from_animation(pose_animation(rig, animation, idle_keys, stock->melee.animation, poses, name), &melee_memory);
	}

	void clear()
	{
		melee_donors.clear();
	}
}
