#include <std_include.hpp>
#include "zonetool/s1/converter/h1/include.hpp"
#include "sound.hpp"

#include "zonetool/h1/assets/sound.hpp"

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace sound
		{
			zonetool::h1::snd_alias_list_t* convert(snd_alias_list_t* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::snd_alias_list_t>();

				std::memcpy(new_asset, asset, sizeof(snd_alias_list_t));
				new_asset->head = mem->Alloc<zonetool::h1::snd_alias_t>(asset->count);
				for (unsigned char i = 0; i < asset->count; i++)
				{
					auto* head = &asset->head[i];
					auto* new_head = &new_asset->head[i];

					new_head->aliasName = head->aliasName;
					new_head->subtitle = head->subtitle;
					new_head->secondaryAliasName = head->secondaryAliasName;
					new_head->chainAliasName = head->chainAliasName;
					new_head->soundFile = reinterpret_cast<zonetool::h1::SoundFile*>(head->soundFile);
					new_head->mixerGroup = head->chainAliasName;
					new_head->poly = head->poly;
					new_head->polyGlobal = 174;
					new_head->polyEntityType = 0;
					new_head->polyGlobalType = 0;
					new_head->dspBusIndex = head->dspBusIndex > 8 ? head->dspBusIndex + 1 : head->dspBusIndex;
					new_head->priority = head->priority;
					// pad/unknown
					new_head->volMin = head->volMin;
					new_head->volMax = head->volMax;
					new_head->volModIndex = head->volModIndex; // should be the same
					new_head->pitchMin = head->pitchMin;
					new_head->pitchMax = head->pitchMax;
					new_head->distMin = head->distMin;
					new_head->distMax = head->distMax;
					new_head->velocityMin = head->velocityMin;
					new_head->flags = head->flags; // convert?
					new_head->masterPriority = head->masterPriority;
					new_head->masterPercentage = head->masterPercentage;
					new_head->slavePercentage = head->slavePercentage;
					new_head->playbackPercentage = head->playbackPercentage;
					new_head->probability = head->probability;
					new_head->u1 = head->u1;
					new_head->sndContext = reinterpret_cast<zonetool::h1::SndContext*>(head->sndContext);
					new_head->sequence = head->sequence;
					new_head->lfePercentage = head->lfePercentage;
					new_head->centerPercentage = head->centerPercentage;
					new_head->startDelay = head->startDelay;
					new_head->sndCurve = reinterpret_cast<zonetool::h1::SndCurve*>(head->sndCurve);
					new_head->envelopMin = head->envelopMin;
					new_head->envelopMax = head->envelopMax;
					new_head->lpfCurve = reinterpret_cast<zonetool::h1::SndCurve*>(head->lpfCurve);
					new_head->reverbSendCurve = reinterpret_cast<zonetool::h1::SndCurve*>(head->reverbSendCurve);
					new_head->speakerMap = reinterpret_cast<zonetool::h1::SpeakerMap*>(head->speakerMap);
					new_head->reverbWetMixOverride = head->reverbWetMixOverride;
					new_head->reverbMultiplier = head->focusPercentage; // should be same
					new_head->smartPanDistance2d = head->smartPanDistance2d;
					new_head->smartPanDistance3d = head->smartPanDistance3d;
					new_head->smartPanAttenuation3d = head->smartPanAttenuation3d;
					new_head->envelopPercentage = head->envelopPercentage;
					new_head->stereo3dAngle = head->stereo3dAngle;
					new_head->stereo3dStart = head->stereo3dStart;
					new_head->stereo3dEnd = head->stereo3dEnd;
					new_head->allowDoppler = head->allowDoppler;
					new_head->dopplerPreset = reinterpret_cast<zonetool::h1::DopplerPreset*>(head->dopplerPreset);
					new_head->u2 = head->u2;

					// TODO:
					if (head->soundFile)
					{
						if (head->soundFile->type == SAT_STREAMED)
						{
							new_head->soundFile = mem->Alloc<zonetool::h1::SoundFile>();
							new_head->soundFile->exists = false;
							new_head->soundFile->type = zonetool::h1::SAT_STREAMED;
							new_head->soundFile->u.streamSnd.filename.info.raw.dir = "";
							new_head->soundFile->u.streamSnd.filename.info.raw.name = "";
						}
					}
				}

				return new_asset;
			}

			void dump(snd_alias_list_t* asset, ZoneMemory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::ISound::dump(converted_asset);
			}
		}
	}
}