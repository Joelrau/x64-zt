#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "weapondef.hpp"

#include "zonetool/h1/assets/weapondef.hpp"

#include "zonetool/iw6/functions.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace weapondef
		{
			// Overloads
#define COPY_FIELD_1(_field) \
	h1_asset->_field = asset->_field;

#define COPY_FIELD_2(_field, _field2) \
	h1_asset->_field = asset->_field2;

// Chooser + forced expansion (MSVC needs this)
#define _COPY_FIELD_GET_MACRO(_1, _2, NAME, ...) NAME
#define _EXPAND(x) x
#define COPY_FIELD(...) \
	_EXPAND(_COPY_FIELD_GET_MACRO(__VA_ARGS__, COPY_FIELD_2, COPY_FIELD_1)(__VA_ARGS__))


// Helper: destination field's plain type (no ref, no cv)
#define _FIELD_TYPE(_field) std::remove_cv_t<std::remove_reference_t<decltype(h1_asset->_field)>>

// Overloads
#define COPY_FIELD_CAST_1(_field) \
	(h1_asset->_field) = static_cast<_FIELD_TYPE(_field)>((asset->_field))

#define COPY_FIELD_CAST_2(_field, _field2) \
	(h1_asset->_field) = static_cast<_FIELD_TYPE(_field)>((asset->_field2))

// Chooser + forced expansion (MSVC friendly)
#define _COPY_FIELD_CAST_GET_MACRO(_1, _2, NAME, ...) NAME
#define _EXPAND(x) x
#define COPY_FIELD_CAST(...) \
	_EXPAND(_COPY_FIELD_CAST_GET_MACRO(__VA_ARGS__, COPY_FIELD_CAST_2, COPY_FIELD_CAST_1)(__VA_ARGS__))


#define REINTERPRET_CAST_1(_field) \
	h1_asset->_field = reinterpret_cast<decltype(h1_asset->_field)>(asset->_field);

#define REINTERPRET_CAST_2(_field, _field2) \
	h1_asset->_field = reinterpret_cast<decltype(h1_asset->_field)>(asset->_field2);

		// Chooser + forced expansion (MSVC needs this)
#define _REINTERPRET_CAST_GET_MACRO(_1, _2, NAME, ...) NAME
#define _EXPAND(x) x
#define REINTERPRET_CAST(...) \
	_EXPAND(_REINTERPRET_CAST_GET_MACRO(__VA_ARGS__, REINTERPRET_CAST_2, REINTERPRET_CAST_1)(__VA_ARGS__))


#define COPY_ARR_(_field, _field2) \
	static_assert(sizeof(h1_asset->_field) == sizeof(asset->_field2)); \
	std::memcpy(&h1_asset->_field, &asset->_field2, sizeof(h1_asset->_field));


#define MEMBER_SPAN_SIZE_T(T, first, last) \
	(offsetof(T, last) + sizeof(((T*)0)->last) - offsetof(T, first))

			zonetool::h1::weapAnimFiles_t get_anim(weapAnimFiles_t iw5_anim)
			{
				static std::unordered_map<weapAnimFiles_t, zonetool::h1::weapAnimFiles_t> mapped_anims =
				{
					{WEAP_ANIM_ROOT, zonetool::h1::WEAP_ANIM_ROOT},
					{WEAP_ANIM_IDLE, zonetool::h1::WEAP_ANIM_IDLE},
					{WEAP_ANIM_EMPTY_IDLE, zonetool::h1::WEAP_ANIM_EMPTY_IDLE},
					{WEAP_ANIM_FIRE, zonetool::h1::WEAP_ANIM_FIRE},
					{WEAP_ANIM_HOLD_FIRE, zonetool::h1::WEAP_ANIM_HOLD_FIRE},
					{WEAP_ANIM_LASTSHOT, zonetool::h1::WEAP_ANIM_LASTSHOT},
					{WEAP_ANIM_RECHAMBER, zonetool::h1::WEAP_ANIM_RECHAMBER},
					{WEAP_ANIM_GRENADE_PRIME, zonetool::h1::WEAP_ANIM_GRENADE_PRIME},
					{WEAP_ANIM_GRENADE_PRIME_READY_TO_THROW, zonetool::h1::WEAP_ANIM_GRENADE_PRIME_READY_TO_THROW},
					{WEAP_ANIM_MELEE_SWIPE, zonetool::h1::WEAP_ANIM_MELEE_SWIPE},
					{WEAP_ANIM_MELEE_HIT, zonetool::h1::WEAP_ANIM_MELEE_HIT},
					{WEAP_ANIM_MELEE_FATAL, zonetool::h1::WEAP_ANIM_MELEE_FATAL},
					{WEAP_ANIM_MELEE_MISS, zonetool::h1::WEAP_ANIM_MELEE_MISS},
					{WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT, zonetool::h1::WEAP_ANIM_MELEE_VICTIM_CROUCHING_HIT},
					{WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL, zonetool::h1::WEAP_ANIM_MELEE_VICTIM_CROUCHING_FATAL},
					{WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS, zonetool::h1::WEAP_ANIM_MELEE_VICTIM_CROUCHING_MISS},
					{WEAP_ANIM_RELOAD, zonetool::h1::WEAP_ANIM_RELOAD},
					{WEAP_ANIM_RELOAD_EMPTY, zonetool::h1::WEAP_ANIM_RELOAD_EMPTY},
					{WEAP_ANIM_RELOAD_START, zonetool::h1::WEAP_ANIM_RELOAD_START},
					{WEAP_ANIM_RELOAD_END, zonetool::h1::WEAP_ANIM_RELOAD_END},
					{WEAP_ANIM_FAST_RELOAD, zonetool::h1::WEAP_ANIM_FAST_RELOAD},
					{WEAP_ANIM_FAST_RELOAD_EMPTY, zonetool::h1::WEAP_ANIM_FAST_RELOAD_EMPTY},
					{WEAP_ANIM_FAST_RELOAD_START, zonetool::h1::WEAP_ANIM_FAST_RELOAD_START},
					{WEAP_ANIM_FAST_RELOAD_END, zonetool::h1::WEAP_ANIM_FAST_RELOAD_END},
					{WEAP_ANIM_RAISE, zonetool::h1::WEAP_ANIM_RAISE},
					{WEAP_ANIM_FIRST_RAISE, zonetool::h1::WEAP_ANIM_FIRST_RAISE},
					{WEAP_ANIM_BREACH_RAISE, zonetool::h1::WEAP_ANIM_BREACH_RAISE},
					{WEAP_ANIM_DROP, zonetool::h1::WEAP_ANIM_DROP},
					{WEAP_ANIM_ALT_RAISE, zonetool::h1::WEAP_ANIM_ALT_RAISE},
					{WEAP_ANIM_ALT_DROP, zonetool::h1::WEAP_ANIM_ALT_DROP},
					{WEAP_ANIM_ALT_OVERRIDE, zonetool::h1::WEAP_ANIM_ALT_OVERRIDE},
					//{WEAP_ANIM_TOGGLE_OVERRIDE, zonetool::h1::WEAP_ANIM_TOGGLE_OVERRIDE},
					//{WEAP_ANIM_SCOPE_TOGGLE_ON, zonetool::h1::WEAP_ANIM_SCOPE_TOGGLE_ON},
					//{WEAP_ANIM_SCOPE_TOGGLE_OFF, zonetool::h1::WEAP_ANIM_SCOPE_TOGGLE_OFF},
					{WEAP_ANIM_QUICK_RAISE, zonetool::h1::WEAP_ANIM_QUICK_RAISE},
					{WEAP_ANIM_QUICK_DROP, zonetool::h1::WEAP_ANIM_QUICK_DROP},
					{WEAP_ANIM_EMPTY_RAISE, zonetool::h1::WEAP_ANIM_EMPTY_RAISE},
					{WEAP_ANIM_EMPTY_DROP, zonetool::h1::WEAP_ANIM_EMPTY_DROP},
					{WEAP_ANIM_SPRINT_IN, zonetool::h1::WEAP_ANIM_SPRINT_IN},
					{WEAP_ANIM_SPRINT_IN_CANCEL, zonetool::h1::WEAP_ANIM_SPRINT_IN_CANCEL},
					{WEAP_ANIM_SPRINT_LOOP, zonetool::h1::WEAP_ANIM_SPRINT_LOOP},
					{WEAP_ANIM_SPRINT_OUT, zonetool::h1::WEAP_ANIM_SPRINT_OUT},
					{WEAP_ANIM_STUNNED_START, zonetool::h1::WEAP_ANIM_STUNNED_START},
					{WEAP_ANIM_STUNNED_LOOP, zonetool::h1::WEAP_ANIM_STUNNED_LOOP},
					{WEAP_ANIM_STUNNED_END, zonetool::h1::WEAP_ANIM_STUNNED_END},
					{WEAP_ANIM_DETONATE, zonetool::h1::WEAP_ANIM_DETONATE},
					{WEAP_ANIM_NIGHTVISION_WEAR, zonetool::h1::WEAP_ANIM_NIGHTVISION_WEAR},
					{WEAP_ANIM_NIGHTVISION_REMOVE, zonetool::h1::WEAP_ANIM_NIGHTVISION_REMOVE},
					{WEAP_ANIM_ADS_FIRE, zonetool::h1::WEAP_ANIM_ADS_FIRE},
					{WEAP_ANIM_ADS_LASTSHOT, zonetool::h1::WEAP_ANIM_ADS_LASTSHOT},
					{WEAP_ANIM_ADS_RECHAMBER, zonetool::h1::WEAP_ANIM_ADS_RECHAMBER},
					{WEAP_ANIM_BLAST_FRONT, zonetool::h1::WEAP_ANIM_BLAST_FRONT},
					{WEAP_ANIM_BLAST_RIGHT, zonetool::h1::WEAP_ANIM_BLAST_RIGHT},
					{WEAP_ANIM_BLAST_BACK, zonetool::h1::WEAP_ANIM_BLAST_BACK},
					{WEAP_ANIM_BLAST_LEFT, zonetool::h1::WEAP_ANIM_BLAST_LEFT},
					{WEAP_ANIM_SLIDE_IN, zonetool::h1::WEAP_ANIM_SLIDE_IN},
					{WEAP_ANIM_SLIDE_LOOP, zonetool::h1::WEAP_ANIM_SLIDE_LOOP},
					{WEAP_ANIM_SLIDE_OUT, zonetool::h1::WEAP_ANIM_SLIDE_OUT},
					{WEAP_ANIM_RECOIL_SETTLE, zonetool::h1::WEAP_ANIM_RECOIL_SETTLE},
					{WEAP_ANIM_SWIM_LOOP, zonetool::h1::WEAP_ANIM_SWIM_LOOP},
					{WEAP_ANIM_MANTLE_UP_64, zonetool::h1::WEAP_ANIM_MANTLE_UP_64},
					{WEAP_ANIM_MANTLE_UP_56, zonetool::h1::WEAP_ANIM_MANTLE_UP_56},
					{WEAP_ANIM_MANTLE_UP_48, zonetool::h1::WEAP_ANIM_MANTLE_UP_48},
					{WEAP_ANIM_MANTLE_UP_40, zonetool::h1::WEAP_ANIM_MANTLE_UP_40},
					{WEAP_ANIM_MANTLE_UP_32, zonetool::h1::WEAP_ANIM_MANTLE_UP_32},
					{WEAP_ANIM_MANTLE_UP_24, zonetool::h1::WEAP_ANIM_MANTLE_UP_24},
					{WEAP_ANIM_MANTLE_UP_16, zonetool::h1::WEAP_ANIM_MANTLE_UP_16},
					{WEAP_ANIM_MANTLE_OVER_64, zonetool::h1::WEAP_ANIM_MANTLE_OVER_64},
					{WEAP_ANIM_MANTLE_OVER_56, zonetool::h1::WEAP_ANIM_MANTLE_OVER_56},
					{WEAP_ANIM_MANTLE_OVER_48, zonetool::h1::WEAP_ANIM_MANTLE_OVER_48},
					{WEAP_ANIM_MANTLE_OVER_40, zonetool::h1::WEAP_ANIM_MANTLE_OVER_40},
					{WEAP_ANIM_MANTLE_OVER_32, zonetool::h1::WEAP_ANIM_MANTLE_OVER_32},
					{WEAP_ANIM_MANTLE_OVER_24, zonetool::h1::WEAP_ANIM_MANTLE_OVER_24},
					{WEAP_ANIM_MANTLE_OVER_16, zonetool::h1::WEAP_ANIM_MANTLE_OVER_16},
					{WEAP_ANIM_RELOAD_MULTIPLE_1, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_1},
					{WEAP_ANIM_RELOAD_MULTIPLE_2, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_2},
					{WEAP_ANIM_RELOAD_MULTIPLE_3, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_3},
					{WEAP_ANIM_RELOAD_MULTIPLE_4, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_4},
					{WEAP_ANIM_RELOAD_MULTIPLE_5, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_5},
					{WEAP_ANIM_RELOAD_MULTIPLE_6, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_6},
					{WEAP_ANIM_RELOAD_MULTIPLE_7, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_7},
					{WEAP_ANIM_RELOAD_MULTIPLE_8, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_8},
					{WEAP_ANIM_RELOAD_MULTIPLE_FAST_1, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_1},
					{WEAP_ANIM_RELOAD_MULTIPLE_FAST_2, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_2},
					{WEAP_ANIM_RELOAD_MULTIPLE_FAST_3, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_3},
					{WEAP_ANIM_RELOAD_MULTIPLE_FAST_4, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_4},
					{WEAP_ANIM_RELOAD_MULTIPLE_FAST_5, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_5},
					{WEAP_ANIM_RELOAD_MULTIPLE_FAST_6, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_6},
					{WEAP_ANIM_RELOAD_MULTIPLE_FAST_7, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_7},
					{WEAP_ANIM_RELOAD_MULTIPLE_FAST_8, zonetool::h1::WEAP_ANIM_RELOAD_MULTIPLE_FAST_8},
					{WEAP_ANIM_ADS_UP, zonetool::h1::WEAP_ANIM_ADS_UP},
					{WEAP_ANIM_ADS_DOWN, zonetool::h1::WEAP_ANIM_ADS_DOWN},
					{WEAP_ANIM_RECOIL, zonetool::h1::WEAP_ANIM_RECOIL},
					{WEAP_ALT_ANIM_ADJUST, zonetool::h1::WEAP_ALT_ANIM_ADJUST},
					{WEAP_ANIM_ADDITIVE_ADS_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_ADS_ROOT},
					{WEAP_ANIM_ADDITIVE_ADS_UP, zonetool::h1::WEAP_ANIM_ADDITIVE_ADS_UP},
					{WEAP_ANIM_ADDITIVE_DRAG_LEFT_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_LEFT_ROOT},
					{WEAP_ANIM_ADDITIVE_DRAG_LEFT, zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_LEFT},
					{WEAP_ANIM_ADDITIVE_DRAG_RIGHT_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_RIGHT_ROOT},
					{WEAP_ANIM_ADDITIVE_DRAG_RIGHT, zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_RIGHT},
					{WEAP_ANIM_ADDITIVE_DRAG_UP_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_UP_ROOT},
					{WEAP_ANIM_ADDITIVE_DRAG_UP, zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_UP},
					{WEAP_ANIM_ADDITIVE_DRAG_DOWN_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_DOWN_ROOT},
					{WEAP_ANIM_ADDITIVE_DRAG_DOWN, zonetool::h1::WEAP_ANIM_ADDITIVE_DRAG_DOWN},
					{WEAP_ANIM_ADDITIVE_SWIM_FORWARD_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_SWIM_FORWARD_ROOT},
					{WEAP_ANIM_ADDITIVE_SWIM_FORWARD, zonetool::h1::WEAP_ANIM_ADDITIVE_SWIM_FORWARD},
					{WEAP_ANIM_ADDITIVE_SWIM_BACKWARD_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_SWIM_BACKWARD_ROOT},
					{WEAP_ANIM_ADDITIVE_SWIM_BACKWARD, zonetool::h1::WEAP_ANIM_ADDITIVE_SWIM_BACKWARD},
					{WEAP_ANIM_ADDITIVE_JUMP_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_JUMP_ROOT},
					{WEAP_ANIM_ADDITIVE_JUMP, zonetool::h1::WEAP_ANIM_ADDITIVE_JUMP},
					{WEAP_ANIM_ADDITIVE_JUMP_LAND_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_JUMP_LAND_ROOT},
					{WEAP_ANIM_ADDITIVE_JUMP_LAND, zonetool::h1::WEAP_ANIM_ADDITIVE_JUMP_LAND},
					{WEAP_ANIM_ADDITIVE_WALK_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_WALK_ROOT},
					{WEAP_ANIM_ADDITIVE_WALK, zonetool::h1::WEAP_ANIM_ADDITIVE_WALK},
					{WEAP_ANIM_ADDITIVE_CRAWL_IN_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_IN_ROOT},
					{WEAP_ANIM_ADDITIVE_CRAWL_IN, zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_IN},
					{WEAP_ANIM_ADDITIVE_CRAWL_LOOP_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_ROOT},
					{WEAP_ANIM_ADDITIVE_CRAWL_LOOP, zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP},
					{WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT_ROOT},
					{WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT, zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_LEFT},
					{WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT_ROOT},
					{WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT, zonetool::h1::WEAP_ANIM_ADDITIVE_CRAWL_LOOP_RIGHT},
					{WEAP_ANIM_ADDITIVE_PRONE_DROP_ROOT, zonetool::h1::WEAP_ANIM_ADDITIVE_PRONE_DROP_ROOT},
					{WEAP_ANIM_ADDITIVE_PRONE_DROP, zonetool::h1::WEAP_ANIM_ADDITIVE_PRONE_DROP},
					{NUM_WEAP_ANIMS, zonetool::h1::NUM_WEAP_ANIMS},
				};

				if (mapped_anims.contains(iw5_anim))
				{
					return mapped_anims[iw5_anim];
				}

				return zonetool::h1::WEAP_ANIM_INVALID;
			}

			void convert_anims(zonetool::h1::XAnimParts** h1_anims, XAnimParts** anims, utils::memory::allocator& mem)
			{
				for (auto i = 0; i < NUM_WEAP_ANIMS; i++)
				{
					const auto iw5_anim = static_cast<weapAnimFiles_t>(i);
					const auto h1_anim = get_anim(iw5_anim);

					if (anims[iw5_anim] && h1_anim != zonetool::h1::WEAP_ANIM_INVALID)
					{
						h1_anims[h1_anim] = mem.allocate<zonetool::h1::XAnimParts>();
						h1_anims[h1_anim]->name = anims[iw5_anim]->name;
					}
				}
			}

			zonetool::h1::PhysPreset* get_phys_preset(weapClass_t weapon_class, utils::memory::allocator& mem)
			{
				std::string phys_preset_name = "default";
				switch (weapon_class)
				{
				case WEAPCLASS_PISTOL:
					phys_preset_name = "weapon_light";
					break;
				case WEAPCLASS_RIFLE:
				case WEAPCLASS_SNIPER:
				case WEAPCLASS_MG:
				case WEAPCLASS_SMG:
				case WEAPCLASS_SPREAD:
				case WEAPCLASS_GRENADE:
				case WEAPCLASS_TURRET:
					phys_preset_name = "weapon_heavy";
					break;
				case WEAPCLASS_THROWINGKNIFE:
					phys_preset_name = "weapon_clip_empty";
					break;
				default:
					phys_preset_name = "default";
					break;
				}

				auto* asset = mem.manual_allocate<zonetool::h1::PhysPreset>(sizeof(const char*));
				asset->name = mem.duplicate_string(phys_preset_name);
				return asset;
			}

			zonetool::h1::playerAnimType_t convert_playeranim(int animType)
			{
				static std::unordered_map<int, zonetool::h1::playerAnimType_t> mapped_anims =
				{
					{PLAYERANIMTYPE_NONE, zonetool::h1::PLAYERANIMTYPE_NONE},
					{PLAYERANIMTYPE_OTHER, zonetool::h1::PLAYERANIMTYPE_OTHER},
					{PLAYERANIMTYPE_PISTOL, zonetool::h1::PLAYERANIMTYPE_PISTOL},
					{PLAYERANIMTYPE_SMG, zonetool::h1::PLAYERANIMTYPE_SMG},
					{PLAYERANIMTYPE_AUTORIFLE, zonetool::h1::PLAYERANIMTYPE_AUTORIFLE},
					//{PLAYERANIMTYPE_MG, zonetool::h1::PLAYERANIMTYPE_MG},
					{PLAYERANIMTYPE_SNIPER, zonetool::h1::PLAYERANIMTYPE_SNIPER},
					//{PLAYERANIMTYPE_ROCKETLAUNCHER, zonetool::h1::PLAYERANIMTYPE_ROCKETLAUNCHER},
					//{PLAYERANIMTYPE_EXPLOSIVE, zonetool::h1::PLAYERANIMTYPE_EXPLOSIVE},
					{PLAYERANIMTYPE_GRENADE, zonetool::h1::PLAYERANIMTYPE_GRENADE},
					//{PLAYERANIMTYPE_TURRET, zonetool::h1::PLAYERANIMTYPE_TURRET},
					//{PLAYERANIMTYPE_C4, zonetool::h1::PLAYERANIMTYPE_C4},
					{PLAYERANIMTYPE_M203, zonetool::h1::PLAYERANIMTYPE_M203},
					{PLAYERANIMTYPE_HOLD, zonetool::h1::PLAYERANIMTYPE_HOLD},
					{PLAYERANIMTYPE_BRIEFCASE, zonetool::h1::PLAYERANIMTYPE_BRIEFCASE},
					{PLAYERANIMTYPE_RIOTSHIELD, zonetool::h1::PLAYERANIMTYPE_RIOTSHIELD},
					{PLAYERANIMTYPE_LAPTOP, zonetool::h1::PLAYERANIMTYPE_LAPTOP},
					{PLAYERANIMTYPE_THROWINGKNIFE, zonetool::h1::PLAYERANIMTYPE_THROWINGKNIFE},
					//{PLAYERANIMTYPE_MORTAR, zonetool::h1::PLAYERANIMTYPE_MORTAR},
					//{PLAYERANIMTYPE_JUGGERNAUTMANIAC, zonetool::h1::PLAYERANIMTYPE_JUGGERNAUTMANIAC},
					//{PLAYERANIMTYPE_CLAYMORE, zonetool::h1::PLAYERANIMTYPE_CLAYMORE},
					{PLAYERANIMTYPE_MINIGUN, zonetool::h1::PLAYERANIMTYPE_MINIGUN},
					//{PLAYERANIMTYPE_DRILL, zonetool::h1::PLAYERANIMTYPE_DRILL},
					//{PLAYERANIMTYPE_DOGWHISTLE, zonetool::h1::PLAYERANIMTYPE_DOGWHISTLE},
					{PLAYERANIMTYPE_SMG_BULLPUP, zonetool::h1::PLAYERANIMTYPE_SMG_BULLPUP},
					//{PLAYERANIMTYPE_AUTORIFLE_BULLPUP, zonetool::h1::PLAYERANIMTYPE_AUTORIFLE_BULLPUP},
					{PLAYERANIMTYPE_SNIPER_BULLPUP, zonetool::h1::PLAYERANIMTYPE_SNIPER_BULLPUP},
					{PLAYERANIMTYPE_KILLSTREAKTRIGGER, zonetool::h1::PLAYERANIMTYPE_KILLSTREAKTRIGGER},
					{PLAYERANIMTYPE_TROPHYSYSTEM, zonetool::h1::PLAYERANIMTYPE_TROPHYSYSTEM},
					{PLAYERANIMTYPE_NUM, zonetool::h1::PLAYERANIMTYPE_NUM},
				};

				if (mapped_anims.contains(animType))
				{
					return mapped_anims[animType];
				}

				return zonetool::h1::PLAYERANIMTYPE_NONE;
			}

			zonetool::h1::OffhandClass convert_offhand_class(OffhandClass offhandClass)
			{
				static std::unordered_map< OffhandClass, zonetool::h1::OffhandClass> mapped =
				{
					{OFFHAND_CLASS_NONE, zonetool::h1::OFFHAND_CLASS_NONE},
					{OFFHAND_CLASS_FRAG_GRENADE, zonetool::h1::OFFHAND_CLASS_FRAG_GRENADE},
					{OFFHAND_CLASS_SMOKE_GRENADE, zonetool::h1::OFFHAND_CLASS_SMOKE_GRENADE},
					{OFFHAND_CLASS_FLASH_GRENADE, zonetool::h1::OFFHAND_CLASS_FLASH_GRENADE},
					{OFFHAND_CLASS_THROWINGKNIFE, zonetool::h1::OFFHAND_CLASS_THROWINGKNIFE},
					{OFFHAND_CLASS_OTHER, zonetool::h1::OFFHAND_CLASS_OTHER},
				};

				if (mapped.contains(offhandClass))
				{
					return mapped[offhandClass];
				}

				__debugbreak();
				return zonetool::h1::OFFHAND_CLASS_NONE;
			}

			void copy_state_timer(zonetool::h1::StateTimers* h1_asset, StateTimers* asset, WeaponCompleteDef* def, bool akimbo)
			{
				COPY_FIELD(fireDelay, iFireDelay);
				COPY_FIELD(meleeDelay, iMeleeDelay);
				COPY_FIELD(meleeChargeDelay, meleeChargeDelay);
				COPY_FIELD(detonateDelay, iDetonateDelay);
				h1_asset->fireTime = akimbo ? def->iFireTimeAkimbo : def->iFireTime;

				COPY_FIELD(rechamberTime, iRechamberTime);
				COPY_FIELD(rechamberTimeOneHanded, rechamberTimeOneHanded);
				COPY_FIELD(rechamberBoltTime, iRechamberBoltTime);
				COPY_FIELD(holdFireTime, iHoldFireTime);
				h1_asset->grenadePrimeReadyToThrowTime = def->weapDef->weapClass == WEAPCLASS_GRENADE ? 100 : 0;

				COPY_FIELD(detonateTime, iDetonateTime);
				COPY_FIELD(meleeTime, iMeleeTime);
				COPY_FIELD(meleeChargeTime, meleeChargeTime);

				COPY_FIELD(reloadTime, iReloadTime);
				COPY_FIELD(reloadShowRocketTime, reloadShowRocketTime);
				COPY_FIELD(reloadEmptyTime, iReloadEmptyTime);
				COPY_FIELD(reloadAddTime, iReloadAddTime);
				h1_asset->reloadEmptyAddTime = 0;

				// sometimes this bugs out
				if (h1_asset->reloadAddTime > h1_asset->reloadTime)
				{
					h1_asset->reloadAddTime = h1_asset->reloadTime - 50;
				}

				COPY_FIELD(reloadStartTime, iReloadStartTime);
				COPY_FIELD(reloadStartAddTime, iReloadStartAddTime);
				COPY_FIELD(reloadEndTime, iReloadEndTime);
				h1_asset->reloadTimeDualWield = 0;
				h1_asset->reloadAddTimeDualWield = 0;
				h1_asset->reloadEmptyDualMag = 2000;
				h1_asset->reloadEmptyAddTimeDualMag = 0;
				h1_asset->speedReloadTime = def->weapDef->weapClass == WEAPCLASS_GRENADE ? 0 : 2000;
				h1_asset->speedReloadAddTime = 0;

				COPY_FIELD(dropTime, iDropTime);
				COPY_FIELD(raiseTime, iRaiseTime);
				COPY_FIELD(altDropTime, iAltDropTime);
				h1_asset->altRaiseTime = akimbo ? def->iAltRaiseTimeAkimbo : def->iAltRaiseTime;

				COPY_FIELD(quickDropTime, quickDropTime);
				COPY_FIELD(quickRaiseTime, quickRaiseTime);
				h1_asset->firstRaiseTime = akimbo ? def->iFirstRaiseTimeAkimbo : def->iFirstRaiseTime;
				COPY_FIELD(breachRaiseTime, iBreachRaiseTime);
				COPY_FIELD(emptyRaiseTime, iEmptyRaiseTime);
				COPY_FIELD(emptyDropTime, iEmptyDropTime);

				COPY_FIELD(sprintInTime, sprintInTime);
				COPY_FIELD(sprintLoopTime, sprintLoopTime);
				COPY_FIELD(sprintOutTime, sprintOutTime);

				COPY_FIELD(stunnedTimeBegin, stunnedTimeBegin);
				COPY_FIELD(stunnedTimeLoop, stunnedTimeLoop);
				COPY_FIELD(stunnedTimeEnd, stunnedTimeEnd);

				COPY_FIELD(nightVisionWearTime, nightVisionWearTime);
				COPY_FIELD(nightVisionWearTimeFadeOutEnd, nightVisionWearTimeFadeOutEnd);
				COPY_FIELD(nightVisionWearTimePowerUp, nightVisionWearTimePowerUp);
				COPY_FIELD(nightVisionRemoveTime, nightVisionRemoveTime);
				COPY_FIELD(nightVisionRemoveTimePowerDown, nightVisionRemoveTimePowerDown);
				COPY_FIELD(nightVisionRemoveTimeFadeInStart, nightVisionRemoveTimeFadeInStart);

				COPY_FIELD(aiFuseTime, aiFuseTime);
				COPY_FIELD(fuseTime, fuseTime);
				h1_asset->missileTime = def->weapDef->weapClass == WEAPCLASS_ROCKETLAUNCHER ? 4000 : 0;
				h1_asset->primeTime = 0;
				h1_asset->bHoldFullPrime = false;

				COPY_FIELD(blastFrontTime, blastFrontTime);
				COPY_FIELD(blastRightTime, blastRightTime);
				COPY_FIELD(blastBackTime, blastBackTime);
				COPY_FIELD(blastLeftTime, blastLeftTime);

				h1_asset->slideInTime = 500;
				h1_asset->slideLoopTime = 500;
				h1_asset->slideOutTime = 500;

				h1_asset->highJumpInTime = 600;
				h1_asset->highJumpDropInTime = 100;
				h1_asset->highJumpDropLoopTime = 1000;
				h1_asset->highJumpDropLandTime = 1000;

				h1_asset->dodgeTime = 500;
				h1_asset->landDipTime = 600;

				h1_asset->hybridSightInTime = 0;
				h1_asset->hybridSightOutTime = 0;

				h1_asset->offhandSwitchTime = 0;

				h1_asset->heatCooldownInTime = 500;
				h1_asset->heatCooldownOutTime = 500;
				h1_asset->heatCooldownOutReadyTime = 500;

				h1_asset->overheatOutTime = 500;
				h1_asset->overheatOutReadyTime = 500;
			}

			bool is_gun(weapClass_t weapon_class)
			{
				if (weapon_class <= WEAPCLASS_PISTOL || weapon_class == WEAPCLASS_ROCKETLAUNCHER)
				{
					return true;
				}

				return false;
			}

			bool is_explosive(weapClass_t weapon_class)
			{
				if (weapon_class == WEAPCLASS_GRENADE || weapon_class == WEAPCLASS_ROCKETLAUNCHER)
				{
					return true;
				}

				return false;
			}

			bool is_sniper(weapClass_t weapon_class)
			{
				return weapon_class == WEAPCLASS_SNIPER;
			}

			void convert_surf_sounds(zonetool::h1::snd_alias_list_t** h1_sounds, snd_alias_list_t** sounds, utils::memory::allocator& mem)
			{
				static std::unordered_map<zonetool::h1::materialSurfType_t, materialSurfType_t> mapped =
				{
					//{ zonetool::h1::SURF_TYPE_DEFAULT, SURF_TYPE_DEFAULT },
					{zonetool::h1::SURF_TYPE_BARK, SURF_TYPE_BARK},
					{zonetool::h1::SURF_TYPE_BRICK, SURF_TYPE_BRICK},
					{zonetool::h1::SURF_TYPE_CARPET, SURF_TYPE_CARPET},
					{zonetool::h1::SURF_TYPE_CLOTH, SURF_TYPE_CLOTH},
					{zonetool::h1::SURF_TYPE_CONCRETE, SURF_TYPE_CONCRETE},
					{zonetool::h1::SURF_TYPE_DIRT, SURF_TYPE_DIRT},
					{zonetool::h1::SURF_TYPE_FLESH, SURF_TYPE_FLESH},
					{zonetool::h1::SURF_TYPE_FOLIAGE_DEBRIS, SURF_TYPE_FOLIAGE},
					{zonetool::h1::SURF_TYPE_GLASS, SURF_TYPE_GLASS},
					{zonetool::h1::SURF_TYPE_GRASS, SURF_TYPE_GRASS},
					{zonetool::h1::SURF_TYPE_GRAVEL, SURF_TYPE_GRAVEL},
					{zonetool::h1::SURF_TYPE_ICE, SURF_TYPE_ICE},
					{zonetool::h1::SURF_TYPE_METAL_SOLID, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_METAL_GRATE, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_MUD, SURF_TYPE_MUD},
					{zonetool::h1::SURF_TYPE_PAPER, SURF_TYPE_PAPER},
					{zonetool::h1::SURF_TYPE_PLASTER, SURF_TYPE_PLASTER},
					{zonetool::h1::SURF_TYPE_ROCK, SURF_TYPE_ROCK},
					{zonetool::h1::SURF_TYPE_SAND, SURF_TYPE_SAND},
					{zonetool::h1::SURF_TYPE_SNOW, SURF_TYPE_SNOW},
					{zonetool::h1::SURF_TYPE_WATER_WAIST, SURF_TYPE_WATER},
					{zonetool::h1::SURF_TYPE_WOOD_SOLID, SURF_TYPE_WOOD},
					{zonetool::h1::SURF_TYPE_ASPHALT, SURF_TYPE_ASPHALT},
					{zonetool::h1::SURF_TYPE_CERAMIC, SURF_TYPE_CERAMIC},
					{zonetool::h1::SURF_TYPE_PLASTIC_SOLID, SURF_TYPE_PLASTIC},
					{zonetool::h1::SURF_TYPE_RUBBER, SURF_TYPE_RUBBER},
					{zonetool::h1::SURF_TYPE_FRUIT, SURF_TYPE_FRUIT},
					//{ zonetool::h1::SURF_TYPE_PAINTEDMETAL, SURF_TYPE_PAINTED_METAL },
					//{ zonetool::h1::SURF_TYPE_RIOTSHIELD, SURF_TYPE_RIOT_SHIELD },
					{zonetool::h1::SURF_TYPE_SLUSH, SURF_TYPE_SLUSH},
					{zonetool::h1::SURF_TYPE_ASPHALT_WET, SURF_TYPE_ASPHALT},
					{zonetool::h1::SURF_TYPE_ASPHALT_DEBRIS, SURF_TYPE_ASPHALT},
					{zonetool::h1::SURF_TYPE_CONCRETE_WET, SURF_TYPE_CONCRETE},
					{zonetool::h1::SURF_TYPE_CONCRETE_DEBRIS, SURF_TYPE_CONCRETE},
					{zonetool::h1::SURF_TYPE_FOLIAGE_VEGETATION, SURF_TYPE_FOLIAGE},
					{zonetool::h1::SURF_TYPE_FOLIAGE_LEAVES, SURF_TYPE_FOLIAGE},
					{zonetool::h1::SURF_TYPE_GRASS_TALL, SURF_TYPE_GRASS},
					{zonetool::h1::SURF_TYPE_METAL_HOLLOW, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_METAL_VEHICLE, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_METAL_THIN, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_METAL_WET, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_METAL_DEBRIS, SURF_TYPE_METAL},
					{zonetool::h1::SURF_TYPE_PLASTIC_HOLLOW, SURF_TYPE_PLASTIC},
					{zonetool::h1::SURF_TYPE_PLASTIC_TARP, SURF_TYPE_PLASTIC},
					{zonetool::h1::SURF_TYPE_ROCK_WET, SURF_TYPE_ROCK},
					{zonetool::h1::SURF_TYPE_ROCK_DEBRIS, SURF_TYPE_ROCK},
					{zonetool::h1::SURF_TYPE_WATER_ANKLE, SURF_TYPE_WATER},
					{zonetool::h1::SURF_TYPE_WATER_KNEE, SURF_TYPE_WATER},
					{zonetool::h1::SURF_TYPE_WOOD_HOLLOW, SURF_TYPE_WOOD},
					{zonetool::h1::SURF_TYPE_WOOD_WET, SURF_TYPE_WOOD},
					{zonetool::h1::SURF_TYPE_WOOD_DEBRIS, SURF_TYPE_WOOD},
					{zonetool::h1::SURF_TYPE_CUSHION, SURF_TYPE_CUSHION},
				};

				for (auto& [h1_type, iw5_type] : mapped)
				{
					if (sounds[iw5_type])
					{
						h1_sounds[h1_type] = mem.manual_allocate<zonetool::h1::snd_alias_list_t>(sizeof(const char*));
						h1_sounds[h1_type]->name = mem.duplicate_string(sounds[iw5_type]->aliasName);
					}
				}
			}

			zonetool::h1::WeaponDef* convert(WeaponCompleteDef* asset, utils::memory::allocator& mem)
			{
				// allocate H1 WeaponDef structure
				const auto h1_asset = mem.allocate<zonetool::h1::WeaponDef>();

				COPY_FIELD(szInternalName);
				COPY_FIELD(szDisplayName);
				COPY_FIELD(szAltWeaponName);
				if (asset->weapDef->gunXModel)
				{
					h1_asset->gunModel = mem.allocate_array<zonetool::h1::XModel*>(2);
					h1_asset->gunModel[0] = reinterpret_cast<zonetool::h1::XModel*>(asset->weapDef->gunXModel[0]);
					h1_asset->gunModel[1] = nullptr;
				}
				REINTERPRET_CAST(handModel, weapDef->handXModel);
				// persistentArmXModel
				// reticleViewModels
				// lobWorldModelName
				if (asset->weapDef->szXAnimsRightHanded)
				{
					h1_asset->szXAnimsRightHanded = mem.allocate_array<zonetool::h1::XAnimParts*>(zonetool::h1::NUM_WEAP_ANIMS);
					convert_anims(h1_asset->szXAnimsRightHanded, asset->weapDef->szXAnimsRightHanded, mem);
				}
				if (asset->weapDef->szXAnimsLeftHanded)
				{
					h1_asset->szXAnimsLeftHanded = mem.allocate_array<zonetool::h1::XAnimParts*>(zonetool::h1::NUM_WEAP_ANIMS);
					convert_anims(h1_asset->szXAnimsLeftHanded, asset->weapDef->szXAnimsLeftHanded, mem);
				}
				REINTERPRET_CAST(hideTags);

				// attachments
				const auto getScopes = [](WeaponCompleteDef* def, std::vector<WeaponAttachment*>& attachmentsOut)
				{
					if (def->scopes)
					{
						for (auto i = 0; i < 6; i++)
						{
							if (def->scopes[i])
							{
								attachmentsOut.push_back(def->scopes[i]);
							}
						}
					}
				};
				const auto getUnderBarrels = [](WeaponCompleteDef* def, std::vector<WeaponAttachment*>& attachmentsOut)
				{
					if (def->underBarrels)
					{
						for (auto i = 0; i < 3; i++)
						{
							if (def->underBarrels[i])
							{
								attachmentsOut.push_back(def->underBarrels[i]);
							}
						}
					}
				};
				const auto getOthers = [](WeaponCompleteDef* def, std::vector<WeaponAttachment*>& attachmentsOut)
				{
					if (def->others)
					{
						for (auto i = 0; i < 4; i++)
						{
							if (def->others[i])
							{
								attachmentsOut.push_back(def->others[i]);
							}
						}
					}
				};
				const auto getAllAttachments = [&](WeaponCompleteDef* def, std::vector<WeaponAttachment*>& attachmentsOut)
				{
					getScopes(def, attachmentsOut);
					getUnderBarrels(def, attachmentsOut);
					getOthers(def, attachmentsOut);
				};

				std::vector<WeaponAttachment*> attachments;
				getAllAttachments(asset, attachments);
				assert(attachments.size() < 0xFF);

				h1_asset->numAttachments = static_cast<unsigned char>(attachments.size());
				h1_asset->attachments = mem.allocate_array<zonetool::h1::WeaponAttachment*>(h1_asset->numAttachments);
				for (auto i = 0; i < h1_asset->numAttachments; i++)
				{
					h1_asset->attachments[i] = mem.manual_allocate<zonetool::h1::WeaponAttachment>(sizeof(const char*));
					h1_asset->attachments[i]->szInternalName = mem.duplicate_string(attachments[i]->szInternalName);
				}

				if (asset->szXAnims)
				{
					h1_asset->szXAnims = mem.allocate_array<zonetool::h1::XAnimParts*>(zonetool::h1::NUM_WEAP_ANIMS);
					convert_anims(h1_asset->szXAnims, asset->szXAnims, mem);
				}

				const auto convertAttachmentValue = [&](int value) -> unsigned char
				{
					return static_cast<unsigned char>(value);
				};

				h1_asset->numAnimOverrides = static_cast<unsigned char>(asset->numAnimOverrides);
				h1_asset->animOverrides = mem.allocate_array<zonetool::h1::AnimOverrideEntry>(h1_asset->numAnimOverrides);
				for (auto i = 0; i < h1_asset->numAnimOverrides; i++)
				{
					h1_asset->animOverrides[i].animHand = 0;

					h1_asset->animOverrides[i].attachment1 = convertAttachmentValue(asset->animOverrides[i].attachment1);
					h1_asset->animOverrides[i].attachment2 = convertAttachmentValue(asset->animOverrides[i].attachment2);

					const auto anim = get_anim(static_cast<weapAnimFiles_t>(asset->animOverrides[i].animTreeType));
					assert(anim != zonetool::h1::WEAP_ANIM_INVALID);
					h1_asset->animOverrides[i].animTreeType = static_cast<unsigned char>(anim);

					if (asset->animOverrides[i].overrideAnim)
					{
						h1_asset->animOverrides[i].overrideAnim = mem.allocate_array<zonetool::h1::XAnimParts>(sizeof(const char*));
						h1_asset->animOverrides[i].overrideAnim->name = asset->animOverrides[i].overrideAnim->name;
					}

					if (asset->animOverrides[i].altmodeAnim)
					{
						h1_asset->animOverrides[i].altmodeAnim = mem.allocate_array<zonetool::h1::XAnimParts>(sizeof(const char*));
						h1_asset->animOverrides[i].altmodeAnim->name = asset->animOverrides[i].altmodeAnim->name;
					}

					COPY_FIELD_CAST(animOverrides[i].animTime);
					COPY_FIELD_CAST(animOverrides[i].altTime);
				}

				h1_asset->numSoundOverrides = static_cast<unsigned char>(asset->numSoundOverrides);
				h1_asset->soundOverrides = mem.allocate_array<zonetool::h1::SoundOverrideEntry>(h1_asset->numSoundOverrides);
				for (auto i = 0; i < h1_asset->numSoundOverrides; i++)
				{
					h1_asset->soundOverrides[i].attachment1 = convertAttachmentValue(asset->soundOverrides[i].attachment1);
					h1_asset->soundOverrides[i].attachment2 = convertAttachmentValue(asset->soundOverrides[i].attachment2);
					COPY_FIELD_CAST(soundOverrides[i].soundType);
					REINTERPRET_CAST(soundOverrides[i].overrideSound, soundOverrides[i].overrideSound);
					REINTERPRET_CAST(soundOverrides[i].altmodeSound, soundOverrides[i].altmodeSound);
				}

				h1_asset->numFXOverrides = static_cast<unsigned char>(asset->numFXOverrides);
				h1_asset->fxOverrides = mem.allocate_array<zonetool::h1::FXOverrideEntry>(h1_asset->numFXOverrides);
				for (auto i = 0; i < h1_asset->numFXOverrides; i++)
				{
					h1_asset->fxOverrides[i].attachment1 = convertAttachmentValue(asset->fxOverrides[i].attachment1);
					h1_asset->fxOverrides[i].attachment2 = convertAttachmentValue(asset->fxOverrides[i].attachment2);
					COPY_FIELD_CAST(fxOverrides[i].fxType);
					REINTERPRET_CAST(fxOverrides[i].overrideFX, fxOverrides[i].overrideFX);
					REINTERPRET_CAST(fxOverrides[i].altmodeFX, fxOverrides[i].altmodeFX);
				}

				h1_asset->numReloadStateTimerOverrides = static_cast<unsigned char>(asset->numReloadStateTimerOverrides);
				h1_asset->reloadOverrides = mem.allocate_array<zonetool::h1::ReloadStateTimerEntry>(h1_asset->numReloadStateTimerOverrides);
				for (auto i = 0; i < h1_asset->numReloadStateTimerOverrides; i++)
				{
					h1_asset->reloadOverrides[i].attachment = convertAttachmentValue(asset->reloadOverrides[i].attachment);
					COPY_FIELD(reloadOverrides[i].reloadAddTime);
					h1_asset->reloadOverrides[i].reloadEmptyAddTime = 0;
					COPY_FIELD(reloadOverrides[i].reloadStartAddTime);
				}

				h1_asset->numNotetrackOverrides = static_cast<unsigned char>(asset->numNotetrackOverrides);
				h1_asset->notetrackOverrides = mem.allocate_array<zonetool::h1::NoteTrackToSoundEntry>(h1_asset->numNotetrackOverrides);
				for (auto i = 0; i < h1_asset->numNotetrackOverrides; i++)
				{
					h1_asset->notetrackOverrides[i].attachment = convertAttachmentValue(asset->notetrackOverrides[i].attachment);
					h1_asset->notetrackOverrides[i].notetrackSoundMapKeys = mem.allocate_array<zonetool::h1::scr_string_t>(36);
					h1_asset->notetrackOverrides[i].notetrackSoundMapValues = mem.allocate_array<zonetool::h1::scr_string_t>(36);
					for (auto j = 0; j < 24; j++)
					{
						h1_asset->notetrackOverrides[i].notetrackSoundMapKeys[j] = asset->notetrackOverrides[i].notetrackSoundMapKeys[j];
						h1_asset->notetrackOverrides[i].notetrackSoundMapValues[j] = asset->notetrackOverrides[i].notetrackSoundMapValues[j];
					}
					//memcpy(h1_asset->notetrackOverrides[i].notetrackSoundMapKeys, asset->notetrackOverrides[i].notetrackSoundMapKeys, sizeof(scr_string_t) * 24);
					//memcpy(h1_asset->notetrackOverrides[i].notetrackSoundMapValues, asset->notetrackOverrides[i].notetrackSoundMapValues, sizeof(scr_string_t) * 24);
				}

				// animOverrides
				// soundOverrides
				// fxOverrides
				// reloadOverrides
				// notetrackOverrides
				h1_asset->notetrackSoundMapKeys = mem.allocate_array<zonetool::h1::scr_string_t>(36);
				h1_asset->notetrackSoundMapValues = mem.allocate_array<zonetool::h1::scr_string_t>(36);
				for (auto i = 0; i < 24; i++)
				{
					h1_asset->notetrackSoundMapKeys[i] = asset->weapDef->notetrackSoundMapKeys[i];
					h1_asset->notetrackSoundMapValues[i] = asset->weapDef->notetrackSoundMapValues[i];
				}
				//memcpy(h1_asset->notetrackSoundMapKeys, asset->weapDef->notetrackSoundMapKeys, sizeof(scr_string_t) * 24);
				//memcpy(h1_asset->notetrackSoundMapValues, asset->weapDef->notetrackSoundMapValues, sizeof(scr_string_t) * 24);
				h1_asset->notetrackRumbleMapKeys = mem.allocate_array<zonetool::h1::scr_string_t>(16);
				h1_asset->notetrackRumbleMapValues = mem.allocate_array<zonetool::h1::scr_string_t>(16);
				for (auto i = 0; i < 16; i++)
				{
					h1_asset->notetrackRumbleMapKeys[i] = asset->weapDef->notetrackRumbleMapKeys[i];
					h1_asset->notetrackRumbleMapValues[i] = asset->weapDef->notetrackRumbleMapValues[i];
				}
				//memcpy(h1_asset->notetrackRumbleMapKeys, asset->weapDef->notetrackRumbleMapKeys, sizeof(scr_string_t) * 16);
				//memcpy(h1_asset->notetrackRumbleMapValues, asset->weapDef->notetrackRumbleMapValues, sizeof(scr_string_t) * 16);
				// notetrackFXMapKeys
				// notetrackFXMapValues
				// notetrackFXMapTagValues
				// notetrackHideTagKeys
				// notetrackHideTagValues
				// notetrackHideTagTagValues
				h1_asset->szAdsrBaseSetting = asset->szAdsrBaseSetting;
				REINTERPRET_CAST(viewFlashEffect, weapDef->viewFlashEffect);
				h1_asset->viewBodyFlashEffect = nullptr;
				REINTERPRET_CAST(worldFlashEffect, weapDef->worldFlashEffect);
				REINTERPRET_CAST(viewFlashADSEffect, weapDef->viewFlashADSEffect);
				// signatureViewFlashEffect
				// signatureViewBodyFlashEffect
				// signatureWorldFlashEffect
				// signatureViewFlashADSEffect
				// signatureViewBodyFlashADSEffect
				h1_asset->meleeHitEffect = nullptr;
				h1_asset->meleeMissEffect = nullptr;
				REINTERPRET_CAST(pickupSound, weapDef->pickupSound);
				REINTERPRET_CAST(pickupSoundPlayer, weapDef->pickupSoundPlayer);
				REINTERPRET_CAST(ammoPickupSound, weapDef->ammoPickupSound);
				REINTERPRET_CAST(ammoPickupSoundPlayer, weapDef->ammoPickupSoundPlayer);
				REINTERPRET_CAST(projectileSound, weapDef->projectileSound);
				REINTERPRET_CAST(pullbackSound, weapDef->pullbackSound);
				REINTERPRET_CAST(pullbackSoundPlayer, weapDef->pullbackSoundPlayer);
				//REINTERPRET_CAST(pullbackSoundQuick, weapDef->pullbackSoundQuick);
				//REINTERPRET_CAST(pullbackSoundQuickPlayer, weapDef->pullbackSoundQuickPlayer);
				REINTERPRET_CAST(fireSound, weapDef->fireSound);
				REINTERPRET_CAST(fireSoundPlayer, weapDef->fireSoundPlayer);
				REINTERPRET_CAST(fireSoundPlayerAkimbo, weapDef->fireSoundPlayerAkimbo);
				//REINTERPRET_CAST(fireMedSound, weapDef->fireMedSound);
				//REINTERPRET_CAST(fireMedSoundPlayer, weapDef->fireMedSoundPlayer);
				//REINTERPRET_CAST(fireHighSound, weapDef->fireHighSound);
				//REINTERPRET_CAST(fireHighSoundPlayer, weapDef->fireHighSoundPlayer);
				REINTERPRET_CAST(fireLoopSound, weapDef->fireLoopSound);
				REINTERPRET_CAST(fireLoopSoundPlayer, weapDef->fireLoopSoundPlayer);
				//REINTERPRET_CAST(fireMedLoopSound, weapDef->fireMedLoopSound);
				//REINTERPRET_CAST(fireMedLoopSoundPlayer, weapDef->fireMedLoopSoundPlayer);
				//REINTERPRET_CAST(fireHighLoopSound, weapDef->fireHighLoopSound);
				//REINTERPRET_CAST(fireHighLoopSoundPlayer, weapDef->fireHighLoopSoundPlayer);
				//REINTERPRET_CAST(fireLoopEndPointSound, weapDef->fireLoopEndPointSound);
				//REINTERPRET_CAST(fireLoopEndPointSoundPlayer, weapDef->fireLoopEndPointSoundPlayer);
				REINTERPRET_CAST(fireStopSound, weapDef->fireStopSound);
				REINTERPRET_CAST(fireStopSoundPlayer, weapDef->fireStopSoundPlayer);
				//REINTERPRET_CAST(fireMedStopSound, weapDef->fireMedStopSound);
				//REINTERPRET_CAST(fireMedStopSoundPlayer, weapDef->fireMedStopSoundPlayer);
				//REINTERPRET_CAST(fireHighStopSound, weapDef->fireHighStopSound);
				//REINTERPRET_CAST(fireHighStopSoundPlayer, weapDef->fireHighStopSoundPlayer);
				REINTERPRET_CAST(fireLastSound, weapDef->fireLastSound);
				REINTERPRET_CAST(fireLastSoundPlayer, weapDef->fireLastSoundPlayer);
				//REINTERPRET_CAST(fireFirstSound, weapDef->fireFirstSound);
				//REINTERPRET_CAST(fireFirstSoundPlayer, weapDef->fireFirstSoundPlayer);
				//REINTERPRET_CAST(fireCustomSound, weapDef->fireCustomSound);
				//REINTERPRET_CAST(fireCustomSoundPlayer, weapDef->fireCustomSoundPlayer);
				REINTERPRET_CAST(emptyFireSound, weapDef->emptyFireSound);
				REINTERPRET_CAST(emptyFireSoundPlayer, weapDef->emptyFireSoundPlayer);
				//REINTERPRET_CAST(adsRequiredFireSoundPlayer, weapDef->adsRequiredFireSoundPlayer);
				REINTERPRET_CAST(meleeSwipeSound, weapDef->meleeSwipeSound);
				REINTERPRET_CAST(meleeSwipeSoundPlayer, weapDef->meleeSwipeSoundPlayer);
				REINTERPRET_CAST(meleeHitSound, weapDef->meleeHitSound);
				//REINTERPRET_CAST(meleeHitSoundPlayer, weapDef->meleeHitSoundPlayer);
				REINTERPRET_CAST(meleeMissSound, weapDef->meleeMissSound);
				//REINTERPRET_CAST(meleeMissSoundPlayer, weapDef->meleeMissSoundPlayer);
				REINTERPRET_CAST(rechamberSound, weapDef->rechamberSound);
				REINTERPRET_CAST(rechamberSoundPlayer, weapDef->rechamberSoundPlayer);
				REINTERPRET_CAST(reloadSound, weapDef->reloadSound);
				REINTERPRET_CAST(reloadSoundPlayer, weapDef->reloadSoundPlayer);
				REINTERPRET_CAST(reloadEmptySound, weapDef->reloadEmptySound);
				REINTERPRET_CAST(reloadEmptySoundPlayer, weapDef->reloadEmptySoundPlayer);
				REINTERPRET_CAST(reloadStartSound, weapDef->reloadStartSound);
				REINTERPRET_CAST(reloadStartSoundPlayer, weapDef->reloadStartSoundPlayer);
				REINTERPRET_CAST(reloadEndSound, weapDef->reloadEndSound);
				REINTERPRET_CAST(reloadEndSoundPlayer, weapDef->reloadEndSoundPlayer);
				REINTERPRET_CAST(detonateSound, weapDef->detonateSound);
				REINTERPRET_CAST(detonateSoundPlayer, weapDef->detonateSoundPlayer);
				REINTERPRET_CAST(nightVisionWearSound, weapDef->nightVisionWearSound);
				REINTERPRET_CAST(nightVisionWearSoundPlayer, weapDef->nightVisionWearSoundPlayer);
				REINTERPRET_CAST(nightVisionRemoveSound, weapDef->nightVisionRemoveSound);
				REINTERPRET_CAST(nightVisionRemoveSoundPlayer, weapDef->nightVisionRemoveSoundPlayer);
				REINTERPRET_CAST(raiseSound, weapDef->raiseSound);
				REINTERPRET_CAST(raiseSoundPlayer, weapDef->raiseSoundPlayer);
				REINTERPRET_CAST(firstRaiseSound, weapDef->firstRaiseSound);
				REINTERPRET_CAST(firstRaiseSoundPlayer, weapDef->firstRaiseSoundPlayer);
				REINTERPRET_CAST(altSwitchSound, weapDef->altSwitchSound);
				REINTERPRET_CAST(altSwitchSoundPlayer, weapDef->altSwitchSoundPlayer);
				REINTERPRET_CAST(putawaySound, weapDef->putawaySound);
				REINTERPRET_CAST(putawaySoundPlayer, weapDef->putawaySoundPlayer);
				REINTERPRET_CAST(scanSound, weapDef->scanSound);
				REINTERPRET_CAST(changeVariableZoomSound, weapDef->changeVariableZoomSound);
				//REINTERPRET_CAST(adsUpSound, weapDef->adsUpSound);
				//REINTERPRET_CAST(adsDownSound, weapDef->adsDownSound);
				//REINTERPRET_CAST(adsCrosshairEnemySound, weapDef->adsCrosshairEnemySound);

				if (asset->weapDef->bounceSound)
				{
					h1_asset->bounceSound = mem.allocate_array<zonetool::h1::snd_alias_list_t*>(53);
					convert_surf_sounds(h1_asset->bounceSound, asset->weapDef->bounceSound, mem);
				}
				if (asset->weapDef->rollingSound)
				{
					h1_asset->rollingSound = mem.allocate_array<zonetool::h1::snd_alias_list_t*>(53);
					convert_surf_sounds(h1_asset->rollingSound, asset->weapDef->rollingSound, mem);
				}

				REINTERPRET_CAST(viewShellEjectEffect, weapDef->viewShellEjectEffect);
				REINTERPRET_CAST(worldShellEjectEffect, weapDef->worldShellEjectEffect);
				REINTERPRET_CAST(viewLastShotEjectEffect, weapDef->viewLastShotEjectEffect);
				REINTERPRET_CAST(worldLastShotEjectEffect, weapDef->worldLastShotEjectEffect);
				// viewMagEjectEffect
				REINTERPRET_CAST(reticleCenter, weapDef->reticleCenter);
				REINTERPRET_CAST(reticleSide, weapDef->reticleSide);
				if (asset->weapDef->worldModel)
				{
					h1_asset->worldModel = mem.allocate_array<zonetool::h1::XModel*>(2);
					h1_asset->worldModel[0] = reinterpret_cast<zonetool::h1::XModel*>(asset->weapDef->worldModel[0]);
					h1_asset->worldModel[1] = nullptr;
				}
				REINTERPRET_CAST(worldClipModel, weapDef->worldClipModel);
				REINTERPRET_CAST(rocketModel, weapDef->rocketModel);
				REINTERPRET_CAST(knifeModel, weapDef->knifeModel);
				REINTERPRET_CAST(worldKnifeModel, weapDef->worldKnifeModel);
				REINTERPRET_CAST(hudIcon, weapDef->hudIcon);
				REINTERPRET_CAST(pickupIcon, weapDef->pickupIcon);
				//REINTERPRET_CAST(minimapIconFriendly, weapDef->minimapIconFriendly);
				//REINTERPRET_CAST(minimapIconEnemy, weapDef->minimapIconEnemy);
				//REINTERPRET_CAST(minimapIconNeutral, weapDef->minimapIconNeutral);
				REINTERPRET_CAST(ammoCounterIcon, weapDef->ammoCounterIcon);
				REINTERPRET_CAST(szAmmoName, weapDef->szAmmoName);
				REINTERPRET_CAST(szClipName, weapDef->szClipName);
				REINTERPRET_CAST(szSharedAmmoCapName, weapDef->szSharedAmmoCapName);
				REINTERPRET_CAST(physCollmap, weapDef->physCollmap);
				h1_asset->physPreset = get_phys_preset(asset->weapDef->weapClass, mem);
				REINTERPRET_CAST(szUseHintString, weapDef->szUseHintString);
				REINTERPRET_CAST(dropHintString, weapDef->dropHintString);

				// locationDamageMultipliers
				h1_asset->locationDamageMultipliers = mem.allocate_array<float>(22);
				std::fill_n(h1_asset->locationDamageMultipliers, 22, 1.0f);
				for (auto i = 0; i < 20; i++)
				{
					h1_asset->locationDamageMultipliers[i] = asset->weapDef->locationDamageMultipliers[i];
				}
				//memcpy(h1_asset->locationDamageMultipliers, asset->weapDef->locationDamageMultipliers, sizeof(float) * 20);

				REINTERPRET_CAST(fireRumble, weapDef->fireRumble);
				// fireMedRumble
				// fireHighRumble
				REINTERPRET_CAST(meleeImpactRumble, weapDef->meleeImpactRumble);
				REINTERPRET_CAST(tracerType, weapDef->tracerType);
				// signatureTracerType
				// laserType
				REINTERPRET_CAST(turretOverheatSound, weapDef->turretOverheatSound);
				REINTERPRET_CAST(turretOverheatEffect, weapDef->turretOverheatEffect);
				REINTERPRET_CAST(turretBarrelSpinRumble, weapDef->turretBarrelSpinRumble);
				REINTERPRET_CAST(turretBarrelSpinMaxSnd, weapDef->turretBarrelSpinMaxSnd);
				std::memcpy(h1_asset->turretBarrelSpinUpSnd, asset->weapDef->turretBarrelSpinUpSnd, sizeof(asset->weapDef->turretBarrelSpinUpSnd));
				std::memcpy(h1_asset->turretBarrelSpinDownSnd, asset->weapDef->turretBarrelSpinDownSnd, sizeof(asset->weapDef->turretBarrelSpinDownSnd));
				REINTERPRET_CAST(missileConeSoundAlias, weapDef->missileConeSoundAlias);
				REINTERPRET_CAST(missileConeSoundAliasAtBase, weapDef->missileConeSoundAliasAtBase);
				REINTERPRET_CAST(stowOffsetModel, weapDef->stowOffsetModel);
				// turretHydraulicSettings

				COPY_FIELD(altWeapon);
				// numAttachments
				// numAnimOverrides
				// numSoundOverrides
				// numFXOverrides
				// numReloadStateTimerOverrides
				// numNotetrackOverrides

				h1_asset->playerAnimType = convert_playeranim(asset->weapDef->playerAnimType);
				COPY_FIELD_CAST(weapType, weapDef->weapType); // same enum values
				COPY_FIELD_CAST(weapClass, weapDef->weapClass); // same enum values up to a certain point
				COPY_FIELD_CAST(penetrateType, weapDef->penetrateType); // same enum values
				h1_asset->penetrateDepth = asset->weapDef->penetrateType == PENETRATE_TYPE_NONE ? 0.0f : 1.0f;
				COPY_FIELD_CAST(impactType, impactType); // same enum values
				h1_asset->impactType = static_cast<zonetool::h1::ImpactType>(asset->impactType > 6 ? asset->impactType + 1 : asset->impactType);
				COPY_FIELD_CAST(inventoryType, weapDef->inventoryType); // same enum values
				h1_asset->fireType = static_cast<zonetool::h1::weapFireType_t>(asset->weapDef->fireType > 4 ? asset->weapDef->fireType + 1 : asset->weapDef->fireType);
				h1_asset->fireBarrels = asset->weapDef->fireType == WEAPON_FIRETYPE_DOUBLEBARREL ? zonetool::h1::WEAPON_FIREBARREL_DOUBLE : zonetool::h1::WEAPON_FIREBARREL_SINGLE;
				h1_asset->adsFireMode = zonetool::h1::WEAPADSFIREMODE_DEFAULT;
				h1_asset->burstFireCooldown = asset->weapDef->burstFireCooldown;
				h1_asset->greebleType = zonetool::h1::WEAPON_GREEBLE_NONE;
				h1_asset->autoReloadType = zonetool::h1::WEAPON_AUTORELOAD_ALWAYS;
				h1_asset->autoHolsterType = zonetool::h1::WEAPON_AUTOHOLSTER_ALWAYS;
				h1_asset->offhandClass = convert_offhand_class(asset->weapDef->offhandClass);
				COPY_FIELD_CAST(stance, weapDef->stance); // same enum values

				COPY_FIELD(reticleCenterSize, weapDef->iReticleCenterSize);
				COPY_FIELD(reticleSideSize, weapDef->iReticleSideSize);
				COPY_FIELD(reticleMinOfs, weapDef->iReticleMinOfs);
				COPY_FIELD_CAST(activeReticleType, weapDef->activeReticleType); // same enum values

				constexpr std::size_t size = MEMBER_SPAN_SIZE_T(WeaponDef, vStandMove, fPosProneRotRate);
				std::memcpy(&h1_asset->standMove, &asset->weapDef->vStandMove, size);

				COPY_FIELD_CAST(hudIconRatio, weapDef->hudIconRatio); // same enum values
				COPY_FIELD_CAST(pickupIconRatio, weapDef->pickupIconRatio); // ^
				COPY_FIELD_CAST(ammoCounterIconRatio, weapDef->ammoCounterIconRatio); // ^
				COPY_FIELD_CAST(ammoCounterClip, weapDef->ammoCounterClip); // ^
				COPY_FIELD(startAmmo, weapDef->iStartAmmo);
				// iAmmoIndex (runtime)
				// iClipIndex (runtime)
				COPY_FIELD(maxAmmo, weapDef->iMaxAmmo);
				h1_asset->minAmmoReq = 1;
				COPY_FIELD(clipSize, iClipSize);
				COPY_FIELD(shotCount, weapDef->shotCount);
				// sharedAmmoCapIndex (runtime)
				COPY_FIELD(sharedAmmoCap, weapDef->iSharedAmmoCap);
				COPY_FIELD(damage, weapDef->damage);
				COPY_FIELD(playerDamage, weapDef->playerDamage);
				COPY_FIELD(meleeDamage, weapDef->iMeleeDamage);
				COPY_FIELD(damageType, weapDef->iDamageType);

				copy_state_timer(&h1_asset->stateTimers, &asset->weapDef->stateTimers, asset, false);
				copy_state_timer(&h1_asset->akimboStateTimers, &asset->weapDef->stateTimers, asset, true);

				COPY_FIELD(autoAimRange, weapDef->autoAimRange);
				COPY_FIELD(aimAssistRange, weapDef->aimAssistRange);
				COPY_FIELD(aimAssistRangeAds, weapDef->aimAssistRangeAds);
				COPY_FIELD(aimPadding, weapDef->aimPadding);
				COPY_FIELD(enemyCrosshairRange, weapDef->enemyCrosshairRange);
				COPY_FIELD(moveSpeedScale, weapDef->moveSpeedScale);
				COPY_FIELD(adsMoveSpeedScale, weapDef->adsMoveSpeedScale);
				COPY_FIELD(sprintDurationScale, weapDef->sprintDurationScale);
				COPY_FIELD(adsZoomFov, fAdsZoomFov);
				COPY_FIELD(adsZoomInFrac, weapDef->fAdsZoomInFrac);
				COPY_FIELD(adsZoomOutFrac, weapDef->fAdsZoomOutFrac);
				h1_asset->adsSceneBlurStrength = 0.0f;
				if (is_gun(asset->weapDef->weapClass))
				{
					h1_asset->adsSceneBlurPhysicalScale = 1.0f;
				}
				// pad3

				REINTERPRET_CAST(overlay.shader, weapDef->overlay.shader);
				REINTERPRET_CAST(overlay.shaderLowRes, weapDef->overlay.shaderLowRes);
				REINTERPRET_CAST(overlay.shaderEMP, weapDef->overlay.shaderEMP);
				REINTERPRET_CAST(overlay.shaderEMPLowRes, weapDef->overlay.shaderEMPLowRes);
				COPY_FIELD_CAST(overlay.reticle, weapDef->overlay.reticle);
				COPY_FIELD(overlay.width, weapDef->overlay.width);
				COPY_FIELD(overlay.height, weapDef->overlay.height);
				COPY_FIELD(overlay.widthSplitscreen, weapDef->overlay.widthSplitscreen);
				COPY_FIELD(overlay.heightSplitscreen, weapDef->overlay.heightSplitscreen);

				COPY_FIELD(adsBobFactor, weapDef->fAdsBobFactor);
				COPY_FIELD(adsViewBobMult, weapDef->fAdsViewBobMult);
				COPY_FIELD(hipSpreadStandMin, weapDef->fHipSpreadStandMin);
				COPY_FIELD(hipSpreadDuckedMin, weapDef->fHipSpreadDuckedMin);
				COPY_FIELD(hipSpreadProneMin, weapDef->fHipSpreadProneMin);
				COPY_FIELD(hipSpreadStandMax, weapDef->hipSpreadStandMax);
				h1_asset->hipSpreadSprintMax = 0.0f;
				h1_asset->hipSpreadSlideMax = 0.0f;
				COPY_FIELD(hipSpreadDuckedMax, weapDef->hipSpreadDuckedMax);
				COPY_FIELD(hipSpreadProneMax, weapDef->hipSpreadProneMax);
				COPY_FIELD(hipSpreadDecayRate, weapDef->fHipSpreadDecayRate);
				COPY_FIELD(hipSpreadFireAdd, weapDef->fHipSpreadFireAdd);
				COPY_FIELD(hipSpreadTurnAdd, weapDef->fHipSpreadTurnAdd);
				COPY_FIELD(hipSpreadMoveAdd, weapDef->fHipSpreadMoveAdd);
				COPY_FIELD(hipSpreadDuckedDecay, weapDef->fHipSpreadDuckedDecay);
				COPY_FIELD(hipSpreadProneDecay, weapDef->fHipSpreadProneDecay);
				COPY_FIELD(hipReticleSidePos, weapDef->fHipReticleSidePos);
				COPY_FIELD(adsIdleAmount, weapDef->fAdsIdleAmount);
				COPY_FIELD(hipIdleAmount, weapDef->fHipIdleAmount);
				COPY_FIELD(adsIdleSpeed, weapDef->adsIdleSpeed);
				COPY_FIELD(hipIdleSpeed, weapDef->hipIdleSpeed);
				COPY_FIELD(idleCrouchFactor, weapDef->fIdleCrouchFactor);
				COPY_FIELD(idleProneFactor, weapDef->fIdleProneFactor);
				COPY_FIELD(gunMaxPitch, weapDef->fGunMaxPitch);
				COPY_FIELD(gunMaxYaw, weapDef->fGunMaxYaw);
				COPY_FIELD(adsIdleLerpStartTime, weapDef->adsIdleLerpStartTime);
				COPY_FIELD(adsIdleLerpTime, weapDef->adsIdleLerpTime);
				COPY_FIELD(adsTransInTime, iAdsTransInTime);
				h1_asset->adsTransInFromSprintTime = asset->iAdsTransInTime;
				COPY_FIELD(adsTransOutTime, iAdsTransOutTime);
				COPY_FIELD(swayMaxAngleSteadyAim, weapDef->swayMaxAngleSteadyAim);
				COPY_FIELD(swayMaxAngle, weapDef->swayMaxAngle);
				COPY_FIELD(swayLerpSpeed, weapDef->swayLerpSpeed);
				COPY_FIELD(swayPitchScale, weapDef->swayPitchScale);
				COPY_FIELD(swayYawScale, weapDef->swayYawScale);
				COPY_FIELD(swayVertScale, weapDef->swayVertScale);
				COPY_FIELD(swayHorizScale, weapDef->swayHorizScale);
				COPY_FIELD(swayShellShockScale, weapDef->swayShellShockScale);
				COPY_FIELD(adsSwayMaxAngle, weapDef->adsSwayMaxAngle);
				COPY_FIELD(adsSwayLerpSpeed, weapDef->adsSwayLerpSpeed);
				COPY_FIELD(adsSwayPitchScale, weapDef->adsSwayPitchScale);
				COPY_FIELD(adsSwayYawScale, weapDef->adsSwayYawScale);
				COPY_FIELD(adsSwayHorizScale, weapDef->adsSwayHorizScale);
				COPY_FIELD(adsSwayVertScale, weapDef->adsSwayVertScale);
				COPY_FIELD(adsViewErrorMin, weapDef->adsFireRateScale);
				COPY_FIELD(adsViewErrorMax, weapDef->adsDamageRangeScale);
				COPY_FIELD(dualWieldViewModelOffset, weapDef->dualWieldViewModelOffset);
				COPY_FIELD(adsFireAnimFrac, weapDef->adsFireAnimFrac);
				COPY_FIELD(scopeDriftDelay, weapDef->fScopeDriftDelay);
				COPY_FIELD(scopeDriftLerpInTime, weapDef->fScopeDriftLerpInTime);
				COPY_FIELD(scopeDriftSteadyTime, weapDef->fScopeDriftSteadyTime);
				COPY_FIELD(scopeDriftLerpOutTime, weapDef->fScopeDriftLerpOutTime);
				COPY_FIELD(scopeDriftSteadyFactor, weapDef->fScopeDriftSteadyFactor);
				COPY_FIELD(scopeDriftUnsteadyFactor, weapDef->fScopeDriftUnsteadyFactor);
				h1_asset->bobVerticalFactor = 1.0f;
				h1_asset->bobHorizontalFactor = 1.0f;
				h1_asset->bobViewVerticalFactor = 1.0f;
				h1_asset->bobViewHorizontalFactor = 1.0f;
				if (is_gun(asset->weapDef->weapClass))
				{
					h1_asset->stationaryZoomFov = 0.0f;
					h1_asset->stationaryZoomDelay = 1.0f;
					h1_asset->stationaryZoomLerpInTime = 2.0f;
					h1_asset->stationaryZoomLerpOutTime = 0.5f;
				}
				COPY_FIELD(adsDofStart);
				COPY_FIELD(adsDofEnd);

				REINTERPRET_CAST(killIcon);
				REINTERPRET_CAST(dpadIcon);
				h1_asset->hudProximityWarningIcon = nullptr;

				COPY_FIELD_CAST(killIconRatio, weapDef->killIconRatio);
				COPY_FIELD_CAST(dpadIconRatio);
				COPY_FIELD(fireAnimLength);
				COPY_FIELD(fireAnimLengthAkimbo);
				h1_asset->inspectAnimTime = 0;
				COPY_FIELD(reloadAmmoAdd, weapDef->iReloadAmmoAdd);
				COPY_FIELD(reloadStartAdd, weapDef->iReloadStartAdd);
				COPY_FIELD(ammoDropStockMin, weapDef->ammoDropStockMin);
				COPY_FIELD(ammoDropStockMax, ammoDropStockMax);
				COPY_FIELD(ammoDropClipPercentMin, weapDef->ammoDropClipPercentMin);
				COPY_FIELD(ammoDropClipPercentMax, weapDef->ammoDropClipPercentMax);
				COPY_FIELD(explosionRadius, weapDef->iExplosionRadius);
				COPY_FIELD(explosionRadiusMin, weapDef->iExplosionRadiusMin);
				COPY_FIELD(explosionInnerDamage, weapDef->iExplosionInnerDamage);
				COPY_FIELD(explosionOuterDamage, weapDef->iExplosionOuterDamage);
				COPY_FIELD(damageConeAngle, weapDef->damageConeAngle);
				COPY_FIELD(bulletExplDmgMult, weapDef->bulletExplDmgMult);
				COPY_FIELD(bulletExplRadiusMult, weapDef->bulletExplRadiusMult);
				COPY_FIELD(projectileSpeed, weapDef->iProjectileSpeed);
				COPY_FIELD(projectileSpeedUp, weapDef->iProjectileSpeedUp);
				COPY_FIELD(projectileSpeedForward, weapDef->iProjectileSpeedForward);
				COPY_FIELD(projectileActivateDist, weapDef->iProjectileActivateDist);
				COPY_FIELD(projLifetime, weapDef->projLifetime);
				COPY_FIELD(timeToAccelerate, weapDef->timeToAccelerate);
				COPY_FIELD(projectileCurvature, weapDef->projectileCurvature);
				h1_asset->projectileName = nullptr;
				REINTERPRET_CAST(projectileModel, weapDef->projectileModel);
				REINTERPRET_CAST(projExplosionEffect, weapDef->projExplosionEffect);
				REINTERPRET_CAST(projDudEffect, weapDef->projDudEffect);
				REINTERPRET_CAST(projExplosionSound, weapDef->projExplosionSound);
				REINTERPRET_CAST(projDudSound, weapDef->projDudSound);
				COPY_FIELD_CAST(projExplosion, weapDef->projExplosion); // same enum values
				COPY_FIELD_CAST(stickiness, weapDef->stickiness); // same enum values
				COPY_FIELD(lowAmmoWarningThreshold, weapDef->lowAmmoWarningThreshold);
				COPY_FIELD(ricochetChance, weapDef->ricochetChance);
				COPY_FIELD(riotShieldHealth, weapDef->riotShieldHealth);
				COPY_FIELD(riotShieldDamageMult, weapDef->riotShieldDamageMult);

				std::vector<float> parallelBounceArray =
				{
					0.5,
					0.6000000238418579,
					0.6000000238418579,
					0.6000000238418579,
					0.20000000298023224,
					0.6000000238418579,
					0.44999998807907104,
					0.20000000298023224,
					0.05000000074505806,
					0.4000000059604645,
					0.3499999940395355,
					0.5,
					0.6000000238418579,
					0.6000000238418579,
					0.5,
					0.20000000298023224,
					0.20000000298023224,
					0.5,
					0.6000000238418579,
					0.30000001192092896,
					0.20000000298023224,
					0.20000000298023224,
					0.6000000238418579,
					0.6000000238418579,
					0.5,
					0.5,
					0.5,
					0.5,
					0.5,
					0.5,
					0.5,
					0.6000000238418579,
					0.6000000238418579,
					0.6000000238418579,
					0.6000000238418579,
					0.05000000074505806,
					0.05000000074505806,
					0.3499999940395355,
					0.6000000238418579,
					0.6000000238418579,
					0.6000000238418579,
					0.6000000238418579,
					0.6000000238418579,
					0.5,
					0.5,
					0.6000000238418579,
					0.6000000238418579,
					0.20000000298023224,
					0.20000000298023224,
					0.6000000238418579,
					0.6000000238418579,
					0.6000000238418579,
					0.0
				};

				std::vector<float> perpendicularBounceArray =
				{
					0.25,
					0.25,
					0.25,
					0.25,
					0.20000000298023224,
					0.25,
					0.22499999403953552,
					0.20000000298023224,
					0.05000000074505806,
					0.20000000298023224,
					0.15000000596046448,
					0.25,
					0.30000001192092896,
					0.25,
					0.5,
					0.10000000149011612,
					0.20000000298023224,
					0.25,
					0.25,
					0.10000000149011612,
					0.10000000149011612,
					0.20000000298023224,
					0.25,
					0.25,
					0.5,
					0.5,
					0.5,
					0.5,
					0.5,
					0.5,
					0.5,
					0.25,
					0.25,
					0.25,
					0.25,
					0.05000000074505806,
					0.05000000074505806,
					0.15000000596046448,
					0.25,
					0.25,
					0.25,
					0.25,
					0.25,
					0.5,
					0.5,
					0.25,
					0.25,
					0.20000000298023224,
					0.20000000298023224,
					0.25,
					0.25,
					0.25,
					0.0
				};

				h1_asset->parallelBounce = mem.allocate_array<float>(53);
				if (asset->weapDef->weapClass == WEAPCLASS_ROCKETLAUNCHER)
				{
					std::fill_n(h1_asset->parallelBounce, 52, 0.5f);
				}
				else if (asset->weapDef->weapClass == WEAPCLASS_GRENADE || asset->weapDef->weapClass == WEAPCLASS_THROWINGKNIFE)
				{
					std::copy_n(parallelBounceArray.data(), std::min<std::size_t>(52, parallelBounceArray.size()), h1_asset->parallelBounce);
				}

				h1_asset->perpendicularBounce = mem.allocate_array<float>(53);
				if (asset->weapDef->weapClass == WEAPCLASS_ROCKETLAUNCHER)
				{
					std::fill_n(h1_asset->perpendicularBounce, 52, 0.5f);
				}
				else if (asset->weapDef->weapClass == WEAPCLASS_GRENADE || asset->weapDef->weapClass == WEAPCLASS_THROWINGKNIFE)
				{
					std::copy_n(perpendicularBounceArray.data(), std::min<std::size_t>(52, perpendicularBounceArray.size()), h1_asset->perpendicularBounce);
				}

				REINTERPRET_CAST(projTrailEffect, weapDef->projTrailEffect);
				REINTERPRET_CAST(projBeaconEffect, weapDef->projBeaconEffect);
				COPY_ARR_(projectileColor, weapDef->vProjectileColor);
				COPY_FIELD_CAST(guidedMissileType, weapDef->guidedMissileType);
				COPY_FIELD(maxSteeringAccel, weapDef->maxSteeringAccel);
				COPY_FIELD(projIgnitionDelay, weapDef->projIgnitionDelay);
				REINTERPRET_CAST(projIgnitionEffect, weapDef->projIgnitionEffect);
				REINTERPRET_CAST(projIgnitionSound, weapDef->projIgnitionSound);

				COPY_FIELD(adsAimPitch, weapDef->fAdsAimPitch);
				COPY_FIELD(adsCrosshairInFrac, weapDef->fAdsCrosshairInFrac);
				COPY_FIELD(adsCrosshairOutFrac, weapDef->fAdsCrosshairOutFrac);
				COPY_FIELD(adsGunKickReducedKickBullets, weapDef->adsGunKickReducedKickBullets);
				COPY_FIELD(adsGunKickReducedKickPercent, weapDef->adsGunKickReducedKickPercent);
				COPY_FIELD(adsGunKickPitchMin, weapDef->fAdsGunKickPitchMin);
				COPY_FIELD(adsGunKickPitchMax, weapDef->fAdsGunKickPitchMax);
				COPY_FIELD(adsGunKickYawMin, weapDef->fAdsGunKickYawMin);
				COPY_FIELD(adsGunKickYawMax, weapDef->fAdsGunKickYawMax);
				COPY_FIELD(adsGunKickMagMin, weapDef->fAdsGunKickMagMin);
				COPY_FIELD(adsGunKickAccel, weapDef->fAdsGunKickAccel);
				COPY_FIELD(adsGunKickSpeedMax, weapDef->fAdsGunKickSpeedMax);
				COPY_FIELD(adsGunKickSpeedDecay, weapDef->fAdsGunKickSpeedDecay);
				COPY_FIELD(adsGunKickStaticDecay, weapDef->fAdsGunKickStaticDecay);
				COPY_FIELD(adsViewKickPitchMin, weapDef->fAdsViewKickPitchMin);
				COPY_FIELD(adsViewKickPitchMax, weapDef->fAdsViewKickPitchMax);
				COPY_FIELD(adsViewKickYawMin, weapDef->fAdsViewKickYawMin);
				COPY_FIELD(adsViewKickYawMax, weapDef->fAdsViewKickYawMax);
				COPY_FIELD(adsViewKickMagMin, weapDef->fAdsViewKickMagMin);
				COPY_FIELD(adsViewKickCenterSpeed, fAdsViewKickCenterSpeed);
				COPY_FIELD(adsViewScatterMin, weapDef->fAdsViewScatterMin);
				COPY_FIELD(adsViewScatterMax, weapDef->fAdsViewScatterMax);
				COPY_FIELD(adsSpread, weapDef->fAdsSpread);
				COPY_FIELD(hipGunKickReducedKickBullets, weapDef->hipGunKickReducedKickBullets);
				COPY_FIELD(hipGunKickReducedKickPercent, weapDef->hipGunKickReducedKickPercent);
				COPY_FIELD(hipGunKickPitchMin, weapDef->fHipGunKickPitchMin);
				COPY_FIELD(hipGunKickPitchMax, weapDef->fHipGunKickPitchMax);
				COPY_FIELD(hipGunKickYawMin, weapDef->fHipGunKickYawMin);
				COPY_FIELD(hipGunKickYawMax, weapDef->fHipGunKickYawMax);
				COPY_FIELD(hipGunKickMagMin, weapDef->fHipGunKickMagMin);
				COPY_FIELD(hipGunKickAccel, weapDef->fHipGunKickAccel);
				COPY_FIELD(hipGunKickSpeedMax, weapDef->fHipGunKickSpeedMax);
				COPY_FIELD(hipGunKickSpeedDecay, weapDef->fHipGunKickSpeedDecay);
				COPY_FIELD(hipGunKickStaticDecay, weapDef->fHipGunKickStaticDecay);
				COPY_FIELD(hipViewKickPitchMin, weapDef->fHipViewKickPitchMin);
				COPY_FIELD(hipViewKickPitchMax, weapDef->fHipViewKickPitchMax);
				COPY_FIELD(hipViewKickYawMin, weapDef->fHipViewKickYawMin);
				COPY_FIELD(hipViewKickYawMax, weapDef->fHipViewKickYawMax);
				COPY_FIELD(hipViewKickMagMin, weapDef->fHipViewKickMagMin);
				COPY_FIELD(hipViewKickCenterSpeed, fHipViewKickCenterSpeed);
				COPY_FIELD(hipViewScatterMin, weapDef->fHipViewScatterMin);
				COPY_FIELD(hipViewScatterMax, weapDef->fHipViewScatterMax);
				if (is_gun(asset->weapDef->weapClass))
				{
					h1_asset->viewKickScale = 1.0f;
				}
				COPY_FIELD(positionReloadTransTime, weapDef->iPositionReloadTransTime);
				COPY_FIELD(fightDist, weapDef->fightDist);
				COPY_FIELD(maxDist, weapDef->maxDist);
				REINTERPRET_CAST(accuracyGraphName[0], weapDef->accuracyGraphName[0]);
				REINTERPRET_CAST(accuracyGraphName[1], weapDef->accuracyGraphName[1]);
				REINTERPRET_CAST(accuracyGraphKnots[0], weapDef->originalAccuracyGraphKnots[0]);
				REINTERPRET_CAST(accuracyGraphKnots[1], weapDef->originalAccuracyGraphKnots[1]);
				h1_asset->originalAccuracyGraphKnots[0] = h1_asset->accuracyGraphKnots[0];
				h1_asset->originalAccuracyGraphKnots[1] = h1_asset->accuracyGraphKnots[1];
				COPY_FIELD(accuracyGraphKnotCount[0], weapDef->originalAccuracyGraphKnotCount[0]);
				COPY_FIELD(accuracyGraphKnotCount[1], weapDef->originalAccuracyGraphKnotCount[1]);
				COPY_FIELD(leftArc, weapDef->leftArc);
				COPY_FIELD(rightArc, weapDef->rightArc);
				COPY_FIELD(topArc, weapDef->topArc);
				COPY_FIELD(bottomArc, weapDef->bottomArc);
				COPY_FIELD(accuracy, weapDef->accuracy);
				COPY_FIELD(aiSpread, weapDef->aiSpread);
				COPY_FIELD(playerSpread, weapDef->playerSpread);
				COPY_ARR_(minTurnSpeed, weapDef->minTurnSpeed);
				COPY_ARR_(maxTurnSpeed, weapDef->maxTurnSpeed);
				COPY_FIELD(pitchConvergenceTime, weapDef->pitchConvergenceTime);
				COPY_FIELD(yawConvergenceTime, weapDef->yawConvergenceTime);
				COPY_FIELD(suppressTime, weapDef->suppressTime);
				COPY_FIELD(maxRange, weapDef->maxRange);
				COPY_FIELD(animHorRotateInc, weapDef->fAnimHorRotateInc);
				COPY_FIELD(playerPositionDist, weapDef->fPlayerPositionDist);
				// useHintStringIndex (runtime)
				// dropHintStringIndex (runtime)
				COPY_FIELD(horizViewJitter, weapDef->horizViewJitter);
				COPY_FIELD(vertViewJitter, weapDef->vertViewJitter);
				COPY_FIELD(scanSpeed, weapDef->scanSpeed);
				COPY_FIELD(scanAccel, weapDef->scanAccel);
				COPY_FIELD(scanPauseTime, weapDef->scanPauseTime);
				REINTERPRET_CAST(szScript, weapDef->szScript);
				COPY_FIELD(minDamage, weapDef->minDamage);
				COPY_FIELD(midDamage, weapDef->minDamage); // midDamage doesn't exist, using minDamage as a placeholder
				COPY_FIELD(minPlayerDamage, weapDef->minPlayerDamage);
				COPY_FIELD(midPlayerDamage, weapDef->minPlayerDamage); // midPlayerDamage doesn't exist, using minPlayerDamage as a placeholder
				COPY_FIELD(maxDamageRange, weapDef->fMaxDamageRange);
				COPY_FIELD(minDamageRange, weapDef->fMinDamageRange);
				h1_asset->signatureAmmoInClip = 0;
				if (is_gun(asset->weapDef->weapClass))
				{
					h1_asset->signatureDamage = 30;
					h1_asset->signatureMidDamage = 30;
					h1_asset->signatureMinDamage = 30;
					h1_asset->signatureMaxDamageRange = 15000.0f;
					h1_asset->signatureMinDamageRange = 16000.0f;
				}
				COPY_FIELD(destabilizationRateTime, weapDef->destabilizationRateTime);
				COPY_FIELD(destabilizationCurvatureMax, weapDef->destabilizationCurvatureMax);
				COPY_FIELD(destabilizeDistance, weapDef->destabilizeDistance);

				COPY_FIELD(turretADSTime, weapDef->turretADSTime);
				COPY_FIELD(turretFov, weapDef->turretFov);
				COPY_FIELD(turretFovADS, weapDef->turretFovADS);
				COPY_FIELD(turretScopeZoomRate, weapDef->turretScopeZoomRate);
				COPY_FIELD(turretScopeZoomMin, weapDef->turretScopeZoomMin);
				COPY_FIELD(turretScopeZoomMax, weapDef->turretScopeZoomMax);
				COPY_FIELD(overheatUpRate, weapDef->turretOverheatUpRate);
				COPY_FIELD(overheatDownRate, weapDef->turretOverheatDownRate);
				h1_asset->overheatCooldownRate = 0.0f; // check this later for turrets
				COPY_FIELD(overheatPenalty, weapDef->turretOverheatPenalty);
				COPY_FIELD(turretBarrelSpinSpeed, weapDef->turretBarrelSpinSpeed);
				COPY_FIELD(turretBarrelSpinUpTime, weapDef->turretBarrelSpinUpTime);
				COPY_FIELD(turretBarrelSpinDownTime, weapDef->turretBarrelSpinDownTime);
				COPY_FIELD(missileConeSoundRadiusAtTop, weapDef->missileConeSoundRadiusAtTop);
				COPY_FIELD(missileConeSoundRadiusAtBase, weapDef->missileConeSoundRadiusAtBase);
				COPY_FIELD(missileConeSoundHeight, weapDef->missileConeSoundHeight);
				COPY_FIELD(missileConeSoundOriginOffset, weapDef->missileConeSoundOriginOffset);
				COPY_FIELD(missileConeSoundVolumescaleAtCore, weapDef->missileConeSoundVolumescaleAtCore);
				COPY_FIELD(missileConeSoundVolumescaleAtEdge, weapDef->missileConeSoundVolumescaleAtEdge);
				COPY_FIELD(missileConeSoundVolumescaleCoreSize, weapDef->missileConeSoundVolumescaleCoreSize);
				COPY_FIELD(missileConeSoundPitchAtTop, weapDef->missileConeSoundPitchAtTop);
				COPY_FIELD(missileConeSoundPitchAtBottom, weapDef->missileConeSoundPitchAtBottom);
				COPY_FIELD(missileConeSoundPitchTopSize, weapDef->missileConeSoundPitchTopSize);
				COPY_FIELD(missileConeSoundPitchBottomSize, weapDef->missileConeSoundPitchBottomSize);
				COPY_FIELD(missileConeSoundCrossfadeTopSize, weapDef->missileConeSoundCrossfadeTopSize);
				COPY_FIELD(missileConeSoundCrossfadeBottomSize, weapDef->missileConeSoundCrossfadeBottomSize);
				COPY_FIELD(aim_automelee_lerp, weapDef->aim_automelee_lerp);
				COPY_FIELD(aim_automelee_range, weapDef->aim_automelee_range);
				COPY_FIELD(aim_automelee_region_height, weapDef->aim_automelee_region_height);
				COPY_FIELD(aim_automelee_region_width, weapDef->aim_automelee_region_width);
				COPY_FIELD(player_meleeHeight, weapDef->player_meleeHeight);
				COPY_FIELD(player_meleeRange, weapDef->player_meleeRange);
				COPY_FIELD(player_meleeWidth, weapDef->player_meleeWidth);
				h1_asset->changedFireTime = 0.0f;
				h1_asset->changedFireTimeNumBullets = 0;
				h1_asset->fireTimeInterpolationType = zonetool::h1::WEAPON_FIRETIME_INTERPOLATION_NONE;
				h1_asset->generateAmmo = 0;
				h1_asset->ammoPerShot = 1;
				if (is_explosive(asset->weapDef->weapClass))
				{
					h1_asset->explodeCount = 1;
				}
				h1_asset->batteryDischargeRate = 0;
				h1_asset->extendedBattery = 0;
				h1_asset->bulletsPerTag = 0;
				h1_asset->maxTags = 1;
				COPY_FIELD_CAST(stowTag, weapDef->stowTag);
				COPY_FIELD_CAST(rattleSoundType, weapDef->rattleSoundType);
				COPY_FIELD_CAST(adsShouldShowCrosshair, weapDef->adsShouldShowCrosshair);
				h1_asset->adsCrosshairShouldScale = true;
				COPY_FIELD(turretADSEnabled, weapDef->turretADSEnabled);
				h1_asset->knifeAttachTagLeft = false;
				COPY_FIELD(knifeAlwaysAttached, weapDef->knifeAlwaysAttached);
				COPY_FIELD(meleeOverrideValues, weapDef->meleeOverrideValues);
				COPY_FIELD(riotShieldEnableDamage, weapDef->riotShieldEnableDamage);
				h1_asset->allowPrimaryWeaponPickup = false;
				COPY_FIELD(sharedAmmo, weapDef->sharedAmmo);
				COPY_FIELD(lockonSupported, weapDef->lockonSupported);
				COPY_FIELD(requireLockonToFire, weapDef->requireLockonToFire);
				COPY_FIELD(isAirburstWeapon, weapDef->isAirburstWeapon);
				COPY_FIELD(bigExplosion, weapDef->bigExplosion);
				COPY_FIELD(noAdsWhenMagEmpty, weapDef->noAdsWhenMagEmpty);
				COPY_FIELD(avoidDropCleanup, weapDef->avoidDropCleanup);
				COPY_FIELD(inheritsPerks, weapDef->inheritsPerks);
				COPY_FIELD(crosshairColorChange, weapDef->crosshairColorChange);
				COPY_FIELD(rifleBullet, weapDef->bRifleBullet);
				COPY_FIELD(armorPiercing, weapDef->armorPiercing);
				COPY_FIELD(boltAction, weapDef->bBoltAction);
				COPY_FIELD(aimDownSight, weapDef->aimDownSight);
				COPY_FIELD(canHoldBreath, weapDef->canHoldBreath);
				COPY_FIELD(meleeOnly, weapDef->meleeOnly);
				h1_asset->quickMelee = false;
				h1_asset->bU_086 = false;
				COPY_FIELD(canVariableZoom, weapDef->canVariableZoom);
				COPY_FIELD(rechamberWhileAds, weapDef->bRechamberWhileAds);
				COPY_FIELD(bulletExplosiveDamage, weapDef->bBulletExplosiveDamage);
				COPY_FIELD(cookOffHold, weapDef->bCookOffHold);
				h1_asset->useBattery = false;
				h1_asset->reticleSpin45 = false;
				COPY_FIELD(clipOnly, weapDef->bClipOnly);
				COPY_FIELD(noAmmoPickup, weapDef->noAmmoPickup);
				COPY_FIELD(disableSwitchToWhenEmpty, weapDef->disableSwitchToWhenEmpty);
				COPY_FIELD(suppressAmmoReserveDisplay, weapDef->suppressAmmoReserveDisplay);
				COPY_FIELD(motionTracker, motionTracker);
				COPY_FIELD(markableViewmodel, weapDef->markableViewmodel);
				COPY_FIELD(noDualWield, weapDef->noDualWield);
				COPY_FIELD(flipKillIcon, weapDef->flipKillIcon);
				h1_asset->actionSlotShowAmmo = true;
				COPY_FIELD(noPartialReload, weapDef->bNoPartialReload);
				COPY_FIELD(segmentedReload, weapDef->bSegmentedReload);
				COPY_FIELD(multipleReload, weapDef->bMultipleReload);
				COPY_FIELD(blocksProne, weapDef->blocksProne);
				COPY_FIELD(silenced, weapDef->silenced);
				COPY_FIELD(isRollingGrenade, weapDef->isRollingGrenade);
				COPY_FIELD(projExplosionEffectForceNormalUp, weapDef->projExplosionEffectForceNormalUp);
				COPY_FIELD(projExplosionEffectInheritParentDirection, weapDef->projExplosionEffectInheritParentDirection);
				COPY_FIELD(projImpactExplode, weapDef->bProjImpactExplode);
				COPY_FIELD(projTrajectoryEvents, weapDef->bProjTrajectoryEvents);
				COPY_FIELD(projWhizByEnabled, weapDef->bProjWhizByEnabled);
				COPY_FIELD(stickToPlayers, weapDef->stickToPlayers);
				COPY_FIELD(stickToVehicles, weapDef->stickToVehicles);
				COPY_FIELD(stickToTurrets, weapDef->stickToTurrets);
				COPY_FIELD(thrownSideways, weapDef->thrownSideways);
				h1_asset->hasDetonatorEmptyThrow = false; // figure this out? should be true for c4
				h1_asset->hasDetonatorDoubleTap = false; // figure this out? should be true for c4
				COPY_FIELD(disableFiring, weapDef->disableFiring);
				COPY_FIELD(timedDetonation, weapDef->timedDetonation);
				h1_asset->noCrumpleMissile = false;
				h1_asset->fuseLitAfterImpact = false;
				COPY_FIELD(rotate, weapDef->rotate);
				COPY_FIELD(holdButtonToThrow, weapDef->holdButtonToThrow);
				COPY_FIELD(freezeMovementWhenFiring, weapDef->freezeMovementWhenFiring);
				COPY_FIELD(thermalScope, weapDef->thermalScope);
				COPY_FIELD(thermalToggle, weapDef->thermalToggle);
				COPY_FIELD(outlineEnemies, weapDef->outlineEnemies);
				COPY_FIELD(altModeSameWeapon, weapDef->altModeSameWeapon);
				COPY_FIELD(turretBarrelSpinEnabled, weapDef->turretBarrelSpinEnabled);
				COPY_FIELD(missileConeSoundEnabled, weapDef->missileConeSoundEnabled);
				COPY_FIELD(missileConeSoundPitchshiftEnabled, weapDef->missileConeSoundPitchshiftEnabled);
				COPY_FIELD(missileConeSoundCrossfadeEnabled, weapDef->missileConeSoundCrossfadeEnabled);
				COPY_FIELD(offhandHoldIsCancelable, weapDef->offhandHoldIsCancelable);
				COPY_FIELD(doNotAllowAttachmentsToOverrideSpread, weapDef->doNotAllowAttachmentsToOverrideSpread);
				COPY_FIELD(useFastReloadAnims, weapDef->useFastReloadAnims);
				h1_asset->dualMagReloadSupported = false;
				h1_asset->reloadStopsAlt = false;
				COPY_FIELD(useScopeDrift, weapDef->useScopeDrift);
				COPY_FIELD(alwaysShatterGlassOnImpact, weapDef->alwaysShatterGlassOnImpact);
				COPY_FIELD(oldWeapon, weapDef->oldWeapon);
				h1_asset->raiseToHold = false;
				h1_asset->notifyOnPlayerImpact = false;
				h1_asset->decreasingKick = false;
				h1_asset->counterSilencer = false;
				h1_asset->projSuppressedByEMP = false;
				h1_asset->projDisabledByEMP = false;
				h1_asset->autosimDisableVariableRate = false;
				h1_asset->projPlayTrailEffectForOwnerOnly = false;
				h1_asset->projPlayBeaconEffectForOwnerOnly = false;
				h1_asset->projKillTrailEffectOnDeath = false;
				h1_asset->projKillBeaconEffectOnDeath = false;
				h1_asset->reticleDetonateHide = false;
				h1_asset->cloaked = false;
				h1_asset->adsHideWeapon = false;
				h1_asset->adsHideHands = false;
				h1_asset->adsSceneBlur = false;
				h1_asset->usesSniperScope = false;
				h1_asset->bU_108 = false;
				h1_asset->hasTransientModels = false; // modify this later if needed
				h1_asset->signatureAmmoAlternate = false;
				h1_asset->useScriptCallbackForHit = false;
				h1_asset->useBulletTagSystem = false;
				h1_asset->hideBulletTags = false;
				h1_asset->adsDofPhysicalFstop = 45.0f;
				h1_asset->adsDofPhysicalFocusDistance = 10.0f;
				h1_asset->autosimSpeedScale = 1.0f;
				h1_asset->reactiveMotionRadiusScale = 0.0f;
				h1_asset->reactiveMotionFrequencyScale = 0.0f;
				h1_asset->reactiveMotionAmplitudeScale = 0.0f;
				h1_asset->reactiveMotionFalloff = 0.0f;
				h1_asset->reactiveMotionLifetime = 0.0f;
				h1_asset->fU_3604[0] = 30.0f;
				h1_asset->fU_3604[1] = 0.0f;
				h1_asset->fU_3604[2] = 0.5899999737739563f;

				return h1_asset;
			}

			void dump(WeaponCompleteDef* asset)
			{
				utils::memory::allocator allocator;
				auto* converted_asset = convert(asset, allocator);
				zonetool::h1::weapon_def::dump(converted_asset);
			}
		}
	}
}
