#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "sound.hpp"

#include "zonetool/h1/assets/sound.hpp"

#include "game/iw6/game.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace sound
		{
			namespace
			{
				std::string remove_extension(const std::string& filename)
				{
					size_t lastdot = filename.find_last_of(".");
					if (lastdot == std::string::npos) return filename;
					return filename.substr(0, lastdot);
				};
			}

			bool channel_is_3d[zonetool::iw6::SoundChannel::SND_CHANNEL_COUNT]
			{
				true, //SND_CHANNEL_PHYSICS,
				true, //SND_CHANNEL_AMBDIST1,
				true, //SND_CHANNEL_AMBDIST2,
				true, //SND_CHANNEL_ALARM,
				true, //SND_CHANNEL_AUTO,
				true, //SND_CHANNEL_AUTO2,
				false, //SND_CHANNEL_AUTO2D,
				true, //SND_CHANNEL_AUTODOG,
				true, //SND_CHANNEL_EXPLOSIONDIST1,
				true, //SND_CHANNEL_EXPLOSIONDIST2,
				true, //SND_CHANNEL_EXPLOSIVEIMPACT,
				true, //SND_CHANNEL_ELEMENT,
				true, //SND_CHANNEL_ELEMENT_INT,
				true, //SND_CHANNEL_ELEMENT_EXT,
				true, //SND_CHANNEL_FOLEY_PLR_MVMT,
				true, //SND_CHANNEL_FOLEY_PLR_WEAP,
				true, //SND_CHANNEL_FOLEY_NPC_MVMT,
				true, //SND_CHANNEL_FOLEY_NPC_WEAP,
				true, //SND_CHANNEL_FOLEY_DOG_MVMT,
				true, //SND_CHANNEL_ELEMENT_LIM,
				false, //SND_CHANNEL_ELEMENT2D,
				true, //SND_CHANNEL_VOICE_DOG_DIST,
				true, //SND_CHANNEL_BULLETFLESH1NPC_NPC,
				true, //SND_CHANNEL_BULLETFLESH2NPC_NPC,
				true, //SND_CHANNEL_BULLETIMPACT,
				true, //SND_CHANNEL_BULLETFLESH1NPC,
				true, //SND_CHANNEL_BULLETFLESH2NPC,
				true, //SND_CHANNEL_BULLETFLESH1,
				true, //SND_CHANNEL_BULLETFLESH2,
				true, //SND_CHANNEL_VEHICLE,
				true, //SND_CHANNEL_VEHICLELIMITED,
				false, //SND_CHANNEL_MENU,
				false, //SND_CHANNEL_MENULIM1,
				false, //SND_CHANNEL_MENULIM2,
				true, //SND_CHANNEL_BULLETWHIZBYIN,
				true, //SND_CHANNEL_BULLETWHIZBYOUT,
				true, //SND_CHANNEL_BODY,
				false, //SND_CHANNEL_BODY2D,
				true, //SND_CHANNEL_RELOAD,
				false, //SND_CHANNEL_RELOAD2D,
				true, //SND_CHANNEL_FOLEY_PLR_STEP,
				true, //SND_CHANNEL_FOLEY_PLR_STEP_UNRES,
				true, //SND_CHANNEL_FOLEY_NPC_STEP,
				true, //SND_CHANNEL_FOLEY_DOG_STEP,
				true, //SND_CHANNEL_ITEM,
				true, //SND_CHANNEL_WEAPON_DRONE,
				true, //SND_CHANNEL_EXPLOSION1,
				true, //SND_CHANNEL_EXPLOSION2,
				true, //SND_CHANNEL_EXPLOSION3,
				true, //SND_CHANNEL_EXPLOSION4,
				true, //SND_CHANNEL_EXPLOSION5,
				true, //SND_CHANNEL_EFFECTS1,
				true, //SND_CHANNEL_EFFECTS2,
				true, //SND_CHANNEL_EFFECTS3,
				false, //SND_CHANNEL_EFFECTS2D1,
				false, //SND_CHANNEL_EFFECTS2D2,
				true, //SND_CHANNEL_NORESTRICT,
				false, //SND_CHANNEL_NORESTRICT2D,
				true, //SND_CHANNEL_AIRCRAFT,
				false, //SND_CHANNEL_VEHICLE2D,
				true, //SND_CHANNEL_WEAPON_DIST,
				true, //SND_CHANNEL_WEAPON_MID,
				true, //SND_CHANNEL_WEAPON,
				false, //SND_CHANNEL_WEAPON2D,
				true, //SND_CHANNEL_NONSHOCK,
				true, //SND_CHANNEL_NONSHOCK2,
				true, //SND_CHANNEL_EFFECTS2DLIM,
				true, //SND_CHANNEL_VOICE_DOG,
				true, //SND_CHANNEL_MUSIC_EMITTER,
				true, //SND_CHANNEL_VOICE_DOG_ATTACK,
				true, //SND_CHANNEL_VOICE,
				false, //SND_CHANNEL_LOCAL,
				false, //SND_CHANNEL_LOCAL2,
				false, //SND_CHANNEL_LOCAL3,
				false, //SND_CHANNEL_AMBIENT,
				false, //SND_CHANNEL_PLR_WEAP_FIRE_2D,
				false, //SND_CHANNEL_PLR_WEAP_MECH_2D,
				false, //SND_CHANNEL_HURT,
				false, //SND_CHANNEL_PLAYER1,
				false, //SND_CHANNEL_PLAYER2,
				false, //SND_CHANNEL_MUSIC,
				false, //SND_CHANNEL_MUSICNOPAUSE,
				true, //SND_CHANNEL_MISSION,
				true, //SND_CHANNEL_MISSIONFX,
				false, //SND_CHANNEL_ANNOUNCER,
				false, //SND_CHANNEL_SHELLSHOCK,
			};

			zonetool::h1::SoundDspBus channel_to_dspbus_index[zonetool::iw6::SoundChannel::SND_CHANNEL_COUNT]
			{
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_CHANNEL_PHYSICS,
				zonetool::h1::SoundDspBus::SND_DSPBUS_AMBIENCE, //SND_CHANNEL_AMBDIST1,
				zonetool::h1::SoundDspBus::SND_DSPBUS_AMBIENCE, //SND_CHANNEL_AMBDIST2,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_ALARM,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_AUTO,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_AUTO2,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_AUTO2D,
				zonetool::h1::SoundDspBus::SND_DSPBUS_ANIMALS, //SND_CHANNEL_AUTODOG,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_CHANNEL_EXPLOSIONDIST1,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_CHANNEL_EXPLOSIONDIST2,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_CHANNEL_EXPLOSIVEIMPACT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_CHANNEL_ELEMENT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_CHANNEL_ELEMENT_INT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_CHANNEL_ELEMENT_EXT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_FOLEY_PLR_MVMT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_FOLEY_PLR_WEAP,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_FOLEY_NPC_MVMT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_FOLEY_NPC_WEAP,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_FOLEY_DOG_MVMT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_CHANNEL_ELEMENT_LIM,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_CHANNEL_ELEMENT2D,
				zonetool::h1::SoundDspBus::SND_DSPBUS_ANIMALS, //SND_CHANNEL_VOICE_DOG_DIST,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_CHANNEL_BULLETFLESH1NPC_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_CHANNEL_BULLETFLESH2NPC_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_CHANNEL_BULLETIMPACT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_CHANNEL_BULLETFLESH1NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_CHANNEL_BULLETFLESH2NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_CHANNEL_BULLETFLESH1,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_CHANNEL_BULLETFLESH2,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VEHICLES, //SND_CHANNEL_VEHICLE,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VEHICLES, //SND_CHANNEL_VEHICLELIMITED,
				zonetool::h1::SoundDspBus::SND_DSPBUS_INTERFACE, //SND_CHANNEL_MENU,
				zonetool::h1::SoundDspBus::SND_DSPBUS_INTERFACE, //SND_CHANNEL_MENULIM1,
				zonetool::h1::SoundDspBus::SND_DSPBUS_INTERFACE, //SND_CHANNEL_MENULIM2,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WHIZBYS, //SND_CHANNEL_BULLETWHIZBYIN,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WHIZBYS, //SND_CHANNEL_BULLETWHIZBYOUT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_BODY,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_BODY2D,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_CHANNEL_RELOAD,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_CHANNEL_RELOAD2D,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_FOLEY_PLR_STEP,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_FOLEY_PLR_STEP_UNRES,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_FOLEY_NPC_STEP,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_FOLEY_DOG_STEP,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_ITEM,
				zonetool::h1::SoundDspBus::SND_DSPBUS_AIR_VEHICLES, //SND_CHANNEL_WEAPON_DRONE,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_CHANNEL_EXPLOSION1,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_CHANNEL_EXPLOSION2,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_CHANNEL_EXPLOSION3,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_CHANNEL_EXPLOSION4,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_CHANNEL_EXPLOSION5,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_CHANNEL_EFFECTS1,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_CHANNEL_EFFECTS2,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_CHANNEL_EFFECTS3,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_CHANNEL_EFFECTS2D1,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_CHANNEL_EFFECTS2D2,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_NORESTRICT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_NORESTRICT2D,
				zonetool::h1::SoundDspBus::SND_DSPBUS_AIR_VEHICLES, //SND_CHANNEL_AIRCRAFT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VEHICLES, //SND_CHANNEL_VEHICLE2D,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS_DIST, //SND_CHANNEL_WEAPON_DIST,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_CHANNEL_WEAPON_MID,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_CHANNEL_WEAPON,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_CHANNEL_WEAPON2D,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_NONSHOCK,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_NONSHOCK2,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_CHANNEL_EFFECTS2DLIM,
				zonetool::h1::SoundDspBus::SND_DSPBUS_ANIMALS, //SND_CHANNEL_VOICE_DOG,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_CHANNEL_MUSIC_EMITTER,
				zonetool::h1::SoundDspBus::SND_DSPBUS_ANIMALS, //SND_CHANNEL_VOICE_DOG_ATTACK,
				zonetool::h1::SoundDspBus::SND_DSPBUS_BATTLECHATTER, //SND_CHANNEL_VOICE,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_LOCAL,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_LOCAL2,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_LOCAL3,
				zonetool::h1::SoundDspBus::SND_DSPBUS_AMBIENCE, //SND_CHANNEL_AMBIENT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_CHANNEL_PLR_WEAP_FIRE_2D,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_CHANNEL_PLR_WEAP_MECH_2D,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_CHANNEL_HURT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_PLAYER1,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_PLAYER2,
				zonetool::h1::SoundDspBus::SND_DSPBUS_MUSIC, //SND_CHANNEL_MUSIC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_MUSIC, //SND_CHANNEL_MUSICNOPAUSE,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_MISSION,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_MISSIONFX,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VOICES_CRITICAL, //SND_CHANNEL_ANNOUNCER,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_CHANNEL_SHELLSHOCK,
			};

			zonetool::h1::SoundDspBus volmod_to_dspbus[zonetool::iw6::SoundVolMod::SND_VOLMOD_COUNT]
			{
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_VOLMOD_FOLEY,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_VOLMOD_WPNAI,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_VOLMOD_WPNPLYR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_INTERFACE, //SND_VOLMOD_HUD,
				zonetool::h1::SoundDspBus::SND_DSPBUS_INTERFACE, //SND_VOLMOD_INTERFACE,
				zonetool::h1::SoundDspBus::SND_DSPBUS_INTERFACE, //SND_VOLMOD_INTERFACE_MUSIC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_MUSIC, //SND_VOLMOD_MUSIC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_MUSIC, //SND_VOLMOD_MUSIC_EMITTER,
				zonetool::h1::SoundDspBus::SND_DSPBUS_AMBIENCE, //SND_VOLMOD_AMBIENCE,
				zonetool::h1::SoundDspBus::SND_DSPBUS_AMBIENCE, //SND_VOLMOD_AMBIENCE_DIST,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_VOLMOD_ELEMENT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_VOLMOD_EMITTER,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_VOLMOD_PHYSICS,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_VOLMOD_BODYFALL,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_VOLMOD_FOLEY_PLR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_VOLMOD_FOLEYMP_PLR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_VOLMOD_FOLEY_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_VOLMOD_FOLEYMP_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_VOLMOD_FOLEY_WPN_PLR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_FOLEY, //SND_VOLMOD_FOLEY_WPN_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_FOOTSTEP_PLR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_FOOTSTEP_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_FOOTSTEPMP_PLR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_FOOTSTEPMP_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_MELEE_PLR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_MELEE_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VOICES, //SND_VOLMOD_CHATTERAL,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VOICES, //SND_VOLMOD_CHATTERAX,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VOICES, //SND_VOLMOD_REACTIONAL,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VOICES, //SND_VOLMOD_REACTIONAX,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VOICES, //SND_VOLMOD_VOICEOVER,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VOICES, //SND_VOLMOD_VOICEOVER_RADIO,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VOICES_CRITICAL, //SND_VOLMOD_VOICEOVER_CRITICAL,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VOICES, //SND_VOLMOD_VOICEOVER_AMB,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_DESTRUCT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_VOLMOD_EXPLOSION,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_VOLMOD_EXPLOSION_GRENADE,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_VOLMOD_EXPLOSION_FLASHBANG,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_VOLMOD_EXPLOSION_ROCKET,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EXPLOSIONS, //SND_VOLMOD_EXPLOSION_CAR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_EMITTERS, //SND_VOLMOD_REX_EMITTERS,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_VOLMOD_IMPACT,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_VOLMOD_IMPACT_PLR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_VOLMOD_IMPACT_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_VOLMOD_IMPACTMP,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_VOLMOD_IMPACTMP_PLR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_PHYSICS, //SND_VOLMOD_IMPACTMP_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WHIZBYS, //SND_VOLMOD_WHIZBY,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WHIZBYS, //SND_VOLMOD_WHIZBYMP,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VEHICLES, //SND_VOLMOD_VEHICLE_PLR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VEHICLES, //SND_VOLMOD_VEHICLE_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VEHICLES, //SND_VOLMOD_VEHICLE_WPN_PLR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VEHICLES, //SND_VOLMOD_VEHICLE_WPN_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_VEHICLES, //SND_VOLMOD_VEHICLE,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_VOLMOD_GRENADEBOUNCE,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_VOLMOD_GRENADEBOUNCEMP,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_VOLMOD_SHELLCASINGS,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_VOLMOD_SHELLCASINGSMP,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_VOLMOD_WPN_PLR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_VOLMOD_WPNMP_PLR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_VOLMOD_WPN_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_VOLMOD_WPNMP_NPC,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_VOLMOD_WPN_PROJECTILE,
				zonetool::h1::SoundDspBus::SND_DSPBUS_WEAPONS, //SND_VOLMOD_WPNMP_PROJECTILE,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_NA,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_MAX,
				zonetool::h1::SoundDspBus::SND_DSPBUS_SCRIPTED1, //SND_VOLMOD_SCRIPTED1,
				zonetool::h1::SoundDspBus::SND_DSPBUS_SCRIPTED2, //SND_VOLMOD_SCRIPTED2,
				zonetool::h1::SoundDspBus::SND_DSPBUS_SCRIPTED3, //SND_VOLMOD_SCRIPTED3,
				zonetool::h1::SoundDspBus::SND_DSPBUS_SCRIPTED4, //SND_VOLMOD_SCRIPTED4,
				zonetool::h1::SoundDspBus::SND_DSPBUS_SCRIPTED5, //SND_VOLMOD_SCRIPTED5,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_FULLVOLUME,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_PERKMP_QUIET,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_DEATHSDOOR,
				zonetool::h1::SoundDspBus::SND_DSPBUS_DEFAULT, //SND_VOLMOD_DEFAULT,
			};

			zonetool::h1::SoundVolMod volmod_to_volmod_index[zonetool::iw6::SoundVolMod::SND_VOLMOD_COUNT]
			{
				zonetool::h1::SoundVolMod::SND_VOLMOD_FOLEY_NPC, //SND_VOLMOD_FOLEY,
				zonetool::h1::SoundVolMod::SND_VOLMOD_WPN_FOLEY_NPC, //SND_VOLMOD_WPNAI,
				zonetool::h1::SoundVolMod::SND_VOLMOD_WPN_FOLEY_PLR, //SND_VOLMOD_WPNPLYR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_HUD, //SND_VOLMOD_HUD,
				zonetool::h1::SoundVolMod::SND_VOLMOD_FRONTEND_SFX, //SND_VOLMOD_INTERFACE,
				zonetool::h1::SoundVolMod::SND_VOLMOD_FRONTEND_MUSIC, //SND_VOLMOD_INTERFACE_MUSIC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MUSIC, //SND_VOLMOD_MUSIC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MUSIC_EMITTER, //SND_VOLMOD_MUSIC_EMITTER,
				zonetool::h1::SoundVolMod::SND_VOLMOD_AMB_QUAD, //SND_VOLMOD_AMBIENCE,
				zonetool::h1::SoundVolMod::SND_VOLMOD_AMB_DIST, //SND_VOLMOD_AMBIENCE_DIST,
				zonetool::h1::SoundVolMod::SND_VOLMOD_AMB_EMITTER, //SND_VOLMOD_ELEMENT,
				zonetool::h1::SoundVolMod::SND_VOLMOD_AMB_EMITTER, //SND_VOLMOD_EMITTER,
				zonetool::h1::SoundVolMod::SND_VOLMOD_PHYSICS, //SND_VOLMOD_PHYSICS,
				zonetool::h1::SoundVolMod::SND_VOLMOD_BODYFALL, //SND_VOLMOD_BODYFALL,
				zonetool::h1::SoundVolMod::SND_VOLMOD_FOLEY_PLR, //SND_VOLMOD_FOLEY_PLR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MP_FOLEY_PLR, //SND_VOLMOD_FOLEYMP_PLR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_FOLEY_NPC, //SND_VOLMOD_FOLEY_NPC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_FOLEYMP_NPC, //SND_VOLMOD_FOLEYMP_NPC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_WPN_FOLEY_PLR, //SND_VOLMOD_FOLEY_WPN_PLR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_WPN_FOLEY_NPC, //SND_VOLMOD_FOLEY_WPN_NPC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_FOOTSTEP_PLR, //SND_VOLMOD_FOOTSTEP_PLR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_FOOTSTEP_NPC, //SND_VOLMOD_FOOTSTEP_NPC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MP_FOOTSTEP_PLR, //SND_VOLMOD_FOOTSTEPMP_PLR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MP_FOOTSTEP_NPC, //SND_VOLMOD_FOOTSTEPMP_NPC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MELEE_PLR, //SND_VOLMOD_MELEE_PLR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MELEE_NPC, //SND_VOLMOD_MELEE_NPC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_CHATTER_FRIENDLY, //SND_VOLMOD_CHATTERAL,
				zonetool::h1::SoundVolMod::SND_VOLMOD_CHATTER_ENEMY, //SND_VOLMOD_CHATTERAX,
				zonetool::h1::SoundVolMod::SND_VOLMOD_VOX_FRIENDLY, //SND_VOLMOD_REACTIONAL,
				zonetool::h1::SoundVolMod::SND_VOLMOD_VOX_ENEMY, //SND_VOLMOD_REACTIONAX,
				zonetool::h1::SoundVolMod::SND_VOLMOD_DIALOG, //SND_VOLMOD_VOICEOVER,
				zonetool::h1::SoundVolMod::SND_VOLMOD_DIALOG_RADIO, //SND_VOLMOD_VOICEOVER_RADIO,
				zonetool::h1::SoundVolMod::SND_VOLMOD_DIALOG_CRITICAL, //SND_VOLMOD_VOICEOVER_CRITICAL,
				zonetool::h1::SoundVolMod::SND_VOLMOD_DIALOG_AMB, //SND_VOLMOD_VOICEOVER_AMB,
				zonetool::h1::SoundVolMod::SND_VOLMOD_DESTRUCT, //SND_VOLMOD_DESTRUCT,
				zonetool::h1::SoundVolMod::SND_VOLMOD_EXPLOSION, //SND_VOLMOD_EXPLOSION,
				zonetool::h1::SoundVolMod::SND_VOLMOD_EXPLOSION_GRENADE, //SND_VOLMOD_EXPLOSION_GRENADE,
				zonetool::h1::SoundVolMod::SND_VOLMOD_EXPLOSION_FLASHBANG, //SND_VOLMOD_EXPLOSION_FLASHBANG,
				zonetool::h1::SoundVolMod::SND_VOLMOD_EXPLOSION_ROCKET, //SND_VOLMOD_EXPLOSION_ROCKET,
				zonetool::h1::SoundVolMod::SND_VOLMOD_EXPLOSION_CAR, //SND_VOLMOD_EXPLOSION_CAR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_AMB_EMITTER, //SND_VOLMOD_REX_EMITTERS,
				zonetool::h1::SoundVolMod::SND_VOLMOD_BULLET_IMPACT_GEO, //SND_VOLMOD_IMPACT,
				zonetool::h1::SoundVolMod::SND_VOLMOD_BULLET_IMPACT_PLR, //SND_VOLMOD_IMPACT_PLR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_BULLET_IMPACT_NPC, //SND_VOLMOD_IMPACT_NPC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_BULLET_IMPACT_GEO, //SND_VOLMOD_IMPACTMP,
				zonetool::h1::SoundVolMod::SND_VOLMOD_BULLET_IMPACT_PLR, //SND_VOLMOD_IMPACTMP_PLR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_BULLET_IMPACT_NPC, //SND_VOLMOD_IMPACTMP_NPC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_BULLET_WHIZBY, //SND_VOLMOD_WHIZBY,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MP_BULLET_WHIZBY, //SND_VOLMOD_WHIZBYMP,
				zonetool::h1::SoundVolMod::SND_VOLMOD_VEHICLE_PLR, //SND_VOLMOD_VEHICLE_PLR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_VEHICLE_NPC, //SND_VOLMOD_VEHICLE_NPC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_WPN_VEHICLE_PLR, //SND_VOLMOD_VEHICLE_WPN_PLR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_WPN_VEHICLE_NPC, //SND_VOLMOD_VEHICLE_WPN_NPC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_VEHICLE, //SND_VOLMOD_VEHICLE,
				zonetool::h1::SoundVolMod::SND_VOLMOD_GRENADEBOUNCE, //SND_VOLMOD_GRENADEBOUNCE,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MP_GRENADEBOUNCE, //SND_VOLMOD_GRENADEBOUNCEMP,
				zonetool::h1::SoundVolMod::SND_VOLMOD_SHELLCASINGS, //SND_VOLMOD_SHELLCASINGS,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MP_SHELLCASINGS, //SND_VOLMOD_SHELLCASINGSMP,
				zonetool::h1::SoundVolMod::SND_VOLMOD_WPN_PLR, //SND_VOLMOD_WPN_PLR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MP_WPN_PLR, //SND_VOLMOD_WPNMP_PLR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_WPN_NPC, //SND_VOLMOD_WPN_NPC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MP_WPN_NPC, //SND_VOLMOD_WPNMP_NPC,
				zonetool::h1::SoundVolMod::SND_VOLMOD_WPN_PROJECTILE, //SND_VOLMOD_WPN_PROJECTILE,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MP_WPN_PROJECTILE, //SND_VOLMOD_WPNMP_PROJECTILE,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MW3_DEPRECATED_NA, //SND_VOLMOD_NA,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MW3_DEPRECATED_MAX, //SND_VOLMOD_MAX,
				zonetool::h1::SoundVolMod::SND_VOLMOD_SCRIPTED1, //SND_VOLMOD_SCRIPTED1,
				zonetool::h1::SoundVolMod::SND_VOLMOD_SCRIPTED2, //SND_VOLMOD_SCRIPTED2,
				zonetool::h1::SoundVolMod::SND_VOLMOD_SCRIPTED3, //SND_VOLMOD_SCRIPTED3,
				zonetool::h1::SoundVolMod::SND_VOLMOD_SCRIPTED4, //SND_VOLMOD_SCRIPTED4,
				zonetool::h1::SoundVolMod::SND_VOLMOD_SCRIPTED5, //SND_VOLMOD_SCRIPTED5,
				zonetool::h1::SoundVolMod::SND_VOLMOD_FULLVOLUME, //SND_VOLMOD_FULLVOLUME,
				zonetool::h1::SoundVolMod::SND_VOLMOD_MP_PERK_QUIET, //SND_VOLMOD_PERKMP_QUIET,
				zonetool::h1::SoundVolMod::SND_VOLMOD_DEFAULT, //SND_VOLMOD_DEATHSDOOR,
				zonetool::h1::SoundVolMod::SND_VOLMOD_DEFAULT, //SND_VOLMOD_DEFAULT,
			};

			void convert_alias(snd_alias_t* asset, zonetool::h1::snd_alias_t* new_asset, int index, utils::memory::allocator& mem)
			{
				COPY_VALUE(aliasName);
				COPY_VALUE(subtitle);
				COPY_VALUE(secondaryAliasName);
				COPY_VALUE(chainAliasName);
				// mixerGroup is not present in H1
				new_asset->squelchName = nullptr;

				new_asset->soundFile;
				new_asset->soundFile = mem.allocate<zonetool::h1::SoundFile>();

				new_asset->soundFile->exists = asset->soundFile->exists;
				new_asset->soundFile->type = static_cast<zonetool::h1::snd_alias_type_t>(asset->soundFile->type);

				switch (asset->soundFile->type)
				{
				case SAT_LOADED:
				{
					const std::string s_name = asset->soundFile->u.loadSnd->name;
					const std::string s_name_no_ext = remove_extension(s_name);

					new_asset->soundFile->u.loadSnd = mem.allocate<zonetool::h1::LoadedSound>();
					new_asset->soundFile->u.loadSnd->name = mem.duplicate_string(s_name_no_ext);
					break;
				}
				}

				new_asset->polyCount = 1;
				new_asset->polyGlobalCount = 174;
				new_asset->polyEntityType = 0;
				new_asset->polyGlobalType = 0;

				new_asset->dspBusIndex = 0; // default;
				new_asset->priority = 1;

				new_asset->playCount = 0;
				new_asset->polyClass = 0;
				new_asset->unk = 0;

				new_asset->volMin = asset->volMin;
				new_asset->volMax = asset->volMax;
				new_asset->volModIndex = 0; // default

				new_asset->pitchMin = asset->pitchMin;
				new_asset->pitchMax = asset->pitchMax;

				new_asset->distMin = asset->distMin;
				new_asset->distMax = asset->distMax;

				new_asset->velocityMin = asset->velocityMin;

				new_asset->masterPriority = asset->masterPriority;
				new_asset->masterPercentage = asset->masterPercentage;

				new_asset->slavePercentage = asset->slavePercentage;

				new_asset->playbackPercentage = 100;
				new_asset->probability = asset->probability;

				new_asset->sndContext = nullptr;

				new_asset->sequence = index + 1; // alias->sequence;
				new_asset->lfePercentage = asset->lfePercentage;
				new_asset->centerPercentage = asset->centerPercentage;
				new_asset->startDelay = asset->startDelay;

				auto* default_sndcurve = mem.allocate<zonetool::h1::SndCurve>();
				default_sndcurve->name = "$default";
				default_sndcurve->isDefault = 1;

				zonetool::h1::SndCurve* sndcurve = nullptr;
				if (asset->volumeFalloffCurve && asset->volumeFalloffCurve->filename != ""s)
				{
					sndcurve = mem.allocate<zonetool::h1::SndCurve>();
					sndcurve->filename = asset->volumeFalloffCurve->filename;
					sndcurve->isDefault = 0;
					sndcurve->knotCount = asset->volumeFalloffCurve->knotCount;
					memcpy(sndcurve->knots, asset->volumeFalloffCurve->knots, sizeof(float[16][2]));
				}
				else
				{
					sndcurve = default_sndcurve;
				}

				new_asset->volumeFalloffCurve = sndcurve;
				new_asset->lpfCurve = default_sndcurve;
				new_asset->reverbSendCurve = default_sndcurve;

				new_asset->speakerMap = nullptr;
				if (asset->speakerMap)
				{
					new_asset->speakerMap = mem.allocate<zonetool::h1::SpeakerMap>();
					new_asset->speakerMap->isDefault = asset->speakerMap->isDefault;
					new_asset->speakerMap->name = asset->speakerMap->name;
					new_asset->speakerMap->orientation = 0;

					for (char x = 0; x < 2; x++)
					{
						for (char y = 0; y < 2; y++)
						{
							new_asset->speakerMap->channelMaps[x][y].speakerCount = asset->speakerMap->channelMaps[x][y].speakerCount;
							for (char i = 0; i < 6; i++)
							{
								new_asset->speakerMap->channelMaps[x][y].speakers[i].speaker = static_cast<char>(asset->speakerMap->channelMaps[x][y].speakers[i].speaker);
								new_asset->speakerMap->channelMaps[x][y].speakers[i].numLevels = static_cast<char>(asset->speakerMap->channelMaps[x][y].speakers[i].numLevels);
								new_asset->speakerMap->channelMaps[x][y].speakers[i].levels[0] = asset->speakerMap->channelMaps[x][y].speakers[i].levels[0];
								new_asset->speakerMap->channelMaps[x][y].speakers[i].levels[1] = asset->speakerMap->channelMaps[x][y].speakers[i].levels[1];
							}
						}
					}
				}

				new_asset->envelopMin = asset->envelopMin;
				new_asset->envelopMax = asset->envelopMax;

				new_asset->wetMixOverride = asset->wetMixOverride;
				new_asset->focusPercentage = 1.0f;
				new_asset->smartpanDistance2d = 0.0f;
				new_asset->smartpanDistance3d = 0.0f;
				new_asset->smartpanAttenuation3d = 1.0f;
				new_asset->minSmartpan2dContribution = 0.0f;

				new_asset->stereo3DAngle = 0;
				new_asset->stereo3DStart = 0.0f;
				new_asset->stereo3DEnd = 0.0f;

				COPY_VALUE(allowDoppler);
				REINTERPRET_CAST_SAFE(dopplerPreset);

				new_asset->variationType = 4;
				new_asset->threshold = 0.5011872053146362f;
				new_asset->lockedLoopTime = 0;

				// convert flags
				zonetool::h1::SoundAliasFlags h1_flags{ 0 };
				zonetool::iw6::SoundAliasFlags iw6_flags{ 0 };

				h1_flags.intValue = 0;
				iw6_flags.intValue = asset->flags;

				h1_flags.packed.looping = iw6_flags.packed.looping;
				h1_flags.packed.isMaster = iw6_flags.packed.isMaster;
				h1_flags.packed.isSlave = iw6_flags.packed.isSlave;
				h1_flags.packed.fullDryLevel = iw6_flags.packed.fullDryLevel;
				h1_flags.packed.noWetLevel = iw6_flags.packed.noWetLevel;
				h1_flags.packed.randomLooping = iw6_flags.packed.randomLooping;
				h1_flags.packed.spatializedRangeCheck = iw6_flags.packed.spatialize;
				h1_flags.packed.spatializedIs3D = !channel_is_3d[iw6_flags.packed.channel]; // dont ask...
				h1_flags.packed.unk9 = 1;
				h1_flags.packed.inheritPitch = 0;
				h1_flags.packed.inheritVolume = 0;
				h1_flags.packed.useContextList = 0;
				h1_flags.packed.useNoPanning2D = 0;
				h1_flags.packed.useOldPanning = 0;
				h1_flags.packed.useNoPanning3D = 0;
				h1_flags.packed.type = new_asset->soundFile->type;
				h1_flags.packed.unused = 0;

				new_asset->flags = h1_flags.intValue;

				new_asset->volModIndex = volmod_to_volmod_index[asset->volModIndex];
				new_asset->dspBusIndex = channel_to_dspbus_index[iw6_flags.packed.channel];
				if (new_asset->dspBusIndex == zonetool::h1::SND_DSPBUS_DEFAULT)
				{
					new_asset->dspBusIndex = volmod_to_dspbus[asset->volModIndex];
				}

				const auto convert_and_dump_streamed = [&]() -> bool
				{
					const auto alias_name = asset->aliasName ? asset->aliasName : "null";

					// dump soundfile
					if (!asset->soundFile->exists)
					{
						ZONETOOL_ERROR("streamed file doesn't exist for sound %s", alias_name);
						return false;
					}

					const auto soundfile_index = asset->soundFile->u.streamSnd.filename.fileIndex;
					if (!soundfile_index)
					{
						ZONETOOL_ERROR("Could not convert streamed data for sound %s", alias_name);
						return false;
					}

					std::string soundfile_path = utils::string::va("soundfile%d.pak", soundfile_index);
					if (asset->soundFile->u.streamSnd.filename.isLocalized)
					{
						soundfile_path = utils::string::va("%s/%s_%s",
							::iw6::game::SEH_GetCurrentLanguageName(),
							::iw6::game::SEH_GetCurrentLanguageCode(),
							soundfile_path.data());
					}

					auto soundfile = filesystem::file(soundfile_path);
					soundfile.open("rb", false, true);

					auto* fp = soundfile.get_fp();
					if (!fp)
					{
						ZONETOOL_FATAL("could not open %s for read.", soundfile_path.data());
						return false;
					}

					const auto offset = asset->soundFile->u.streamSnd.filename.info.packed.offset;
					const auto length = asset->soundFile->u.streamSnd.filename.info.packed.length;

					std::vector<std::uint8_t> snd_data;
					snd_data.resize(length);

					// get data from offset
					std::fseek(fp, static_cast<long>(offset), SEEK_SET);
					std::fread(snd_data.data(), length, 1, fp);

					if (std::strncmp(reinterpret_cast<char*>(snd_data.data()), "fLaC", 4))
					{
						ZONETOOL_FATAL("streamed sound data for sound %s is not in fLaC format!", alias_name);
						return false;
					}

					std::string loaded_name = utils::string::va("%s_%d", alias_name, index);
					if (asset->soundFile->u.streamSnd.filename.isLocalized)
					{
						loaded_name = utils::string::va("%s/%s_%s_%d",
							::iw6::game::SEH_GetCurrentLanguageName(),
							::iw6::game::SEH_GetCurrentLanguageCode(),
							alias_name,
							index);
					}
					loaded_name = utils::string::va("streamed/%s", loaded_name.data());

					const auto path = utils::string::va("loaded_sound/%s%s", loaded_name.data(), ".flac");

					auto file = filesystem::file(path);
					file.open("wb");
					file.write(snd_data.data(), snd_data.size(), 1);
					file.close();

					// convert sound
					std::memset(&new_asset->soundFile->u.streamSnd, 0, sizeof(new_asset->soundFile->u.streamSnd));
					new_asset->soundFile->type = zonetool::h1::SAT_LOADED;

					zonetool::h1::SoundAliasFlags flags{ 0 };
					flags.intValue = new_asset->flags;
					flags.packed.type = zonetool::h1::SAT_LOADED;
					new_asset->flags = flags.intValue;

					new_asset->soundFile->u.loadSnd = mem.allocate<zonetool::h1::LoadedSound>();
					const auto new_loaded = new_asset->soundFile->u.loadSnd;
					new_loaded->name = mem.duplicate_string(loaded_name);

					return true;
				};

				if (asset->soundFile)
				{
					if (asset->soundFile->type == SAT_STREAMED)
					{
						if (!convert_and_dump_streamed())
						{
							new_asset->soundFile = mem.allocate<zonetool::h1::SoundFile>();
							new_asset->soundFile->exists = false;
							new_asset->soundFile->type = zonetool::h1::SAT_STREAMED;
							new_asset->soundFile->u.streamSnd.filename.info.raw.dir = "";
							new_asset->soundFile->u.streamSnd.filename.info.raw.name = "";
						}
					}
				}
			}

			zonetool::h1::snd_alias_list_t* convert(snd_alias_list_t* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::snd_alias_list_t>();

				REINTERPRET_CAST_SAFE(name);

				new_asset->count = static_cast<unsigned char>(asset->count);

				new_asset->head = allocator.allocate_array<zonetool::h1::snd_alias_t>(new_asset->count);
				for (unsigned char i = 0; i < new_asset->count; i++)
				{
					convert_alias(&asset->head[i], &new_asset->head[i], i, allocator);
				}

				new_asset->contextList = nullptr;
				new_asset->contextListCount = 0;

				return new_asset;
			}

			void dump(snd_alias_list_t* asset)
			{
				utils::memory::allocator allocator;
				auto* converted_asset = convert(asset, allocator);
				zonetool::h1::sound::dump(converted_asset);
			}
		}
	}
}