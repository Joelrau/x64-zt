#include <std_include.hpp>
#include "zonetool/s1/converter/h1/include.hpp"
#include "sound.hpp"

#include "zonetool/h1/assets/sound.hpp"

#include "game/s1/game.hpp"

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace sound
		{
			zonetool::h1::snd_alias_list_t* convert(snd_alias_list_t* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::snd_alias_list_t>();

				std::memcpy(new_asset, asset, sizeof(snd_alias_list_t));
				new_asset->head = allocator.allocate_array<zonetool::h1::snd_alias_t>(asset->count);

				for (auto i = 0u; i < asset->count; i++)
				{
					const auto head = &asset->head[i];
					const auto new_head = &new_asset->head[i];

					new_head->aliasName = head->aliasName;
					new_head->subtitle = head->subtitle;
					new_head->secondaryAliasName = head->secondaryAliasName;
					new_head->chainAliasName = head->chainAliasName;

					new_head->soundFile = allocator.allocate<zonetool::h1::SoundFile>();
					std::memcpy(new_head->soundFile, head->soundFile, sizeof(zonetool::s1::SoundFile));

					new_head->squelchName = head->squelchName;
					new_head->polyCount = head->polyCount == 142 ? 174 : head->polyCount;
					new_head->polyGlobalCount = 174;
					new_head->polyEntityType = 0;
					new_head->polyGlobalType = 0;
					new_head->dspBusIndex = head->dspBusIndex > 8 ? head->dspBusIndex + 1 : head->dspBusIndex;
					new_head->priority = head->priority;
					new_head->playCount = head->playCount;
					new_head->polyClass = head->polyClass;
					new_head->unk = 0;
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
					new_head->variationType = head->variationType;
					new_head->sndContext = reinterpret_cast<zonetool::h1::SndContext*>(head->sndContext);
					new_head->sequence = head->sequence;
					new_head->lfePercentage = head->lfePercentage;
					new_head->centerPercentage = head->centerPercentage;
					new_head->startDelay = head->startDelay;
					new_head->volumeFalloffCurve = reinterpret_cast<zonetool::h1::SndCurve*>(head->volumeFalloffCurve);
					new_head->envelopMin = head->envelopMin;
					new_head->envelopMax = head->envelopMax;
					new_head->lpfCurve = reinterpret_cast<zonetool::h1::SndCurve*>(head->lpfCurve);
					new_head->reverbSendCurve = reinterpret_cast<zonetool::h1::SndCurve*>(head->reverbSendCurve);
					new_head->speakerMap = reinterpret_cast<zonetool::h1::SpeakerMap*>(head->speakerMap);
					new_head->wetMixOverride = head->wetMixOverride;
					new_head->focusPercentage = head->focusPercentage;
					new_head->smartpanDistance2d = head->smartpanDistance2d;
					new_head->smartpanDistance3d = head->smartpanDistance3d;
					new_head->smartpanAttenuation3d = head->smartpanAttenuation3d;
					new_head->minSmartpan2dContribution = head->minSmartpan2dContribution;
					new_head->stereo3DAngle = head->stereo3DAngle;
					new_head->stereo3DStart = head->stereo3DStart;
					new_head->stereo3DEnd = head->stereo3DEnd;
					new_head->allowDoppler = head->allowDoppler;
					new_head->dopplerPreset = reinterpret_cast<zonetool::h1::DopplerPreset*>(head->dopplerPreset);
					new_head->threshold = head->threshold;
					new_head->lockedLoopTime = head->lockedLoopTime;

					const auto convert_and_dump_streamed = [&]() -> bool
					{
						const auto alias_name = head->aliasName ? head->aliasName : "null";

						// dump soundfile
						if (!head->soundFile->exists)
						{
							ZONETOOL_ERROR("streamed file doesn't exist for sound %s", alias_name);
							return false;
						}

						const auto soundfile_index = head->soundFile->u.streamSnd.filename.fileIndex;
						if (!soundfile_index)
						{
							ZONETOOL_ERROR("Could not convert streamed data for sound %s", alias_name);
							return false;
						}

						std::string soundfile_path = utils::string::va("soundfile%d.pak", soundfile_index);
						if (head->soundFile->u.streamSnd.filename.isLocalized)
						{
							soundfile_path = utils::string::va("%s/%s_%s", 
								::s1::game::SEH_GetCurrentLanguageName(),
								::s1::game::SEH_GetCurrentLanguageCode(), 
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

						const auto offset = head->soundFile->u.streamSnd.filename.info.packed.offset;
						const auto length = head->soundFile->u.streamSnd.filename.info.packed.length;

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

						std::string loaded_name = utils::string::va("%s_%d", alias_name, i);
						if (head->soundFile->u.streamSnd.filename.isLocalized)
						{
							loaded_name = utils::string::va("%s/%s_%s_%d", 
								::s1::game::SEH_GetCurrentLanguageName(), 
								::s1::game::SEH_GetCurrentLanguageCode(), 
								alias_name, 
								i);
						}
						loaded_name = utils::string::va("streamed/%s", loaded_name.data());

						const auto path = utils::string::va("loaded_sound/%s%s", loaded_name.data(), ".flac");

						auto file = filesystem::file(path);
						file.open("wb");
						file.write(snd_data.data(), snd_data.size(), 1);
						file.close();

						// convert sound
						std::memset(&new_head->soundFile->u.streamSnd, 0, sizeof(new_head->soundFile->u.streamSnd));
						new_head->soundFile->type = zonetool::h1::SAT_LOADED;

						SoundAliasFlags flags{0};
						flags.intValue = new_head->flags;
						flags.packed.type = zonetool::h1::SAT_LOADED;
						new_head->flags = flags.intValue;

						new_head->soundFile->u.loadSnd = allocator.allocate<zonetool::h1::LoadedSound>();
						const auto new_loaded = new_head->soundFile->u.loadSnd;
						new_loaded->name = allocator.duplicate_string(loaded_name);

						return true;
					};

					if (head->soundFile)
					{
						if (head->soundFile->type == SAT_STREAMED)
						{
							if (!convert_and_dump_streamed())
							{
								new_head->soundFile = allocator.allocate<zonetool::h1::SoundFile>();
								new_head->soundFile->exists = false;
								new_head->soundFile->type = zonetool::h1::SAT_STREAMED;
								new_head->soundFile->u.streamSnd.filename.info.raw.dir = "";
								new_head->soundFile->u.streamSnd.filename.info.raw.name = "";
							}
						}
					}
				}

				return new_asset;
			}

			void dump(snd_alias_list_t* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::sound::dump(converted_asset);
			}
		}
	}
}
