#include <std_include.hpp>
#include "sound_bank.hpp"

#include "../common/sound.hpp"

#include "utils/io.hpp"
#include "utils/bit_buffer.hpp"

namespace zonetool::iw7
{
	namespace ducks
	{
		bool parse_duck(const std::string& name, zone_memory* mem, SndDuck* asset);
	}

	namespace aliases
	{
		std::string LOAD_TYPES[] =
		{
			"loaded",
			"hybrid_pcm",
			"streamed",
			"primed",
		};

		std::string LOOP_TYPES[] =
		{
			"nonlooping",
			"looping",
			"unused",
			"rlooping",
		};

		std::string MUSIC_CONTENT[] =
		{
			"none",
			"music",
			"licensed",
		};

		std::string GPAD_OUTPUT[] =
		{
			"none",
			"ifavailable",
			"controlleronly",
		};

		std::string SHAPES[] =
		{
			"disc",
			"donut",
			"donut_log_fast",
			"donut_log_slow",
			"donut_flat_ended",
			"donut_flat_middle",
		};

		std::string FULLDRYLEVEL[] =
		{
			"",
			"fulldrylevel"
		};

		std::string SND_ALIAS_FIELDS[] =
		{
			"Name",
			"SecondaryAliasName",
			"StopAliasName",
			"Subtitle",
			"File",
			"vol_min",
			"vol_max",
			"vol_mod",
			"pitch_min",
			"pitch_max",
			"donut_fadeend",
			"dist_min",
			"dist_max",
			"Velocity_Min",
			"channel",
			"type",
			"loop",
			"Probability",
			"volumefalloffcurve",
			"lpfcurve",
			"hpfcurve",
			"reverbsendcurve",
			"StartDelay",
			"SpeakerMap",
			"reverb",
			"reverb_multiplier",
			"far_reverb_multiplier",
			"lfe_percentage",
			"center_percentage",
			"envelop_min",
			"envelop_max",
			"envelop_percentage",
			"shape",
			"ignoreDistanceCheck",
			"occlusionshape",
			"dopplerpreset",
			"smartPanDistance2d",
			"smartPanDistance3d",
			"smartPanAttenuation2d",
			"stereoSpreadMinDist",
			"stereoSpreadMaxDist",
			"stereoSpreadMidPoint",
			"stereoSpreadMaxAngle",
			"contexttype",
			"contextvalue",
			"precached",
			"duck",
			"MusicContent",
			"GPadOutput",
			"ForceSubtitle",
			"masterPriority",
			"masterPercentage",
			"masterSlavePercentage",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".aliases.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_ALIAS_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write_alias(SndBank* asset, SndAlias* alias, csv_writer& writer)
		{
			writer.write_column(alias->aliasName ? alias->aliasName : "");
			writer.write_column(alias->secondaryAliasName ? alias->secondaryAliasName : "");
			writer.write_column(alias->stopAliasName ? alias->stopAliasName : "");
			writer.write_column(alias->subtitle ? alias->subtitle : "");
			writer.write_column(alias->assetFileName ? alias->assetFileName : "");
			writer.write_column(alias->volMin);
			writer.write_column(alias->volMax);
			writer.write_column(VOLMODS[alias->volModIndex]);
			writer.write_column(alias->pitchMin);
			writer.write_column(alias->pitchMax);
			writer.write_column(alias->donutFadeEnd);
			writer.write_column(alias->distMin);
			writer.write_column(alias->distMax);
			writer.write_column(alias->velocityMin);
			writer.write_column(ENTCHANNELS[alias->flags.channel]);
			writer.write_column(LOAD_TYPES[alias->flags.type]);
			writer.write_column(LOOP_TYPES[alias->flags.looping]);
			writer.write_column(alias->probability);
			writer.write_column(vf_curves_s[alias->volumeFalloffCurveIndex]);
			writer.write_column(lpf_curves_s[alias->lpfCurveIndex]);
			writer.write_column(hpf_curves_s[alias->hpfCurveIndex]);
			writer.write_column(rvb_curves_s[alias->reverbSendCurveIndex]);
			writer.write_column(alias->startDelay);
			writer.write_column(speaker_maps_s[alias->speakerMapIndex]);
			writer.write_column(FULLDRYLEVEL[alias->flags.reverb]);
			writer.write_column(alias->reverbMultiplier);
			writer.write_column(alias->farReverbMultiplier);
			writer.write_column(alias->lfePercentage);
			writer.write_column(alias->centerPercentage);
			writer.write_column(alias->envelopMin);
			writer.write_column(alias->envelopMax);
			writer.write_column(alias->envelopPercentage);
			writer.write_column(SHAPES[alias->flags.shape]);
			writer.write_column(FALSE_TRUE[alias->flags.ignoreDistanceCheck]);
			writer.write_column(occlusion_shapes_s[alias->occlusionShapeIndex]);
			writer.write_column(alias->dopplerPresetIndex == 0xFF ? "" : doppler_presets_s[alias->dopplerPresetIndex]);
			writer.write_column(alias->smartPanDistance2d);
			writer.write_column(alias->smartPanDistance3d);
			writer.write_column(alias->smartPanAttenuation2d);
			writer.write_column(alias->stereoSpreadMinDist);
			writer.write_column(alias->stereoSpreadMaxDist);
			writer.write_column(alias->stereoSpreadMidPoint);
			writer.write_column(alias->stereoSpreadMaxAngle);
			writer.write_column(get_hashed_string(alias->contextType));
			writer.write_column(get_hashed_string(alias->contextValue));
			writer.write_column(FALSE_TRUE[alias->flags.precached]);
			writer.write_column(duck_hash_lookup(asset, alias->duck));
			writer.write_column(MUSIC_CONTENT[alias->flags.MusicContent]);
			writer.write_column(GPAD_OUTPUT[alias->flags.GPadOutput]);
			writer.write_column(FALSE_TRUE[alias->flags.ForceSubtitle]);
			writer.write_column(alias->masterPriority);
			writer.write_column(alias->masterPercentage);
			writer.write_column(alias->slavePercentage);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->aliasCount; i++)
			{
				auto* alias_list = &asset->alias[i];

				for (auto j = 0; j < alias_list->count; j++)
				{
					write_alias(asset, &alias_list->head[j], writer);
				}
			}

			writer.save(get_path(asset));
		}

		void parse_file(SndBank* asset, const std::string& path, std::vector<SndAlias*>& aliases, std::set<std::string>& ducks, zone_memory* mem)
		{
			ZONETOOL_INFO("Parsing sound alias csv \"%s\"...", path.data());

			const auto parse_path = filesystem::get_file_path(path) + path;
			csv::parser parser(parse_path);

			const auto row_count = parser.get_num_rows();
			if (row_count <= 1) return; // has at least one entry

			const auto max_fields = parser.get_max_columns();
			if (max_fields != std::extent_v<decltype(SND_ALIAS_FIELDS)>)
			{
				ZONETOOL_ERROR("Failed to parse file %s, csv doesn't have correct field count.", path.data());
				return;
			}

			const auto rows = parser.get_rows();

			for (auto i = 1; i < row_count; i++)
			{
				if (rows[i]->num_fields != max_fields)
				{
					ZONETOOL_ERROR("Tried to parse invalid line %u in csv file %s", i, path.data());
					continue;
				}

				SndAlias* alias = mem->allocate<SndAlias>();

				auto field_idx = 0;
				const auto get = [&]()
				{
					return rows[i]->fields[field_idx++];
				};

				const auto get_string = [&]() -> const char*
				{
					auto str = get();
					if (*str)
					{
						return mem->duplicate_string(str);
					}
					return nullptr;
				};

				alias->aliasName = get_string();
				alias->id = snd_hash_name(alias->aliasName);

				if (!alias->aliasName || !*alias->aliasName)
				{
					ZONETOOL_WARNING("Parsed an empty alias name on line %d for file %s", i, path.data());
				}

				ZONETOOL_INFO("Parsing sound \"%s\"...", alias->aliasName);

				alias->secondaryAliasName = get_string();
				alias->secondaryId = snd_hash_name(alias->secondaryAliasName);

				alias->stopAliasName = get_string();
				alias->stopAliasID = snd_hash_name(alias->stopAliasName);

				alias->subtitle = get_string();

				alias->assetFileName = get_string();
				alias->assetId = snd_hash_name(alias->assetFileName);

				alias->sequence = 0;

				alias->volMin = get_value<float>(get());
				alias->volMax = get_value<float>(get());
				alias->volModIndex = get_value_index<int>(get(), VOLMODS, std::extent_v<decltype(VOLMODS)>);
				alias->pitchMin = get_value<float>(get());
				alias->pitchMax = get_value<float>(get());
				alias->donutFadeEnd = get_value<float>(get());
				alias->distMin = get_value<float>(get());
				alias->distMax = get_value<float>(get());
				alias->velocityMin = get_value<float>(get());
				alias->flags.channel = get_value_index<unsigned int>(get(), ENTCHANNELS, std::extent_v<decltype(ENTCHANNELS)>);
				alias->flags.type = get_value_index<unsigned int>(get(), LOAD_TYPES, std::extent_v<decltype(LOAD_TYPES)>);
				alias->flags.looping = get_value_index<unsigned int>(get(), LOOP_TYPES, std::extent_v<decltype(LOOP_TYPES)>);
				alias->probability = get_value<float>(get());

				const auto default_vf_curve = get_value_index<unsigned char>("default", vf_curves_s, std::extent_v<decltype(vf_curves_s)>, 0);
				alias->volumeFalloffCurveIndex = get_value_index<unsigned char>(get(), vf_curves_s, std::extent_v<decltype(vf_curves_s)>, default_vf_curve);

				const auto default_lpf_curve = get_value_index<unsigned char>("default", lpf_curves_s, std::extent_v<decltype(lpf_curves_s)>, 0);
				alias->lpfCurveIndex = get_value_index<unsigned char>(get(), lpf_curves_s, std::extent_v<decltype(lpf_curves_s)>, default_lpf_curve);

				const auto default_hpf_curve = get_value_index<unsigned char>("default", hpf_curves_s, std::extent_v<decltype(hpf_curves_s)>, 0);
				alias->hpfCurveIndex = get_value_index<unsigned char>(get(), hpf_curves_s, std::extent_v<decltype(hpf_curves_s)>, default_hpf_curve);

				const auto default_reverb_send_curve = get_value_index<unsigned char>("default", rvb_curves_s, std::extent_v<decltype(rvb_curves_s)>, 0);
				alias->reverbSendCurveIndex = get_value_index<unsigned char>(get(), rvb_curves_s, std::extent_v<decltype(rvb_curves_s)>, default_reverb_send_curve);

				alias->startDelay = get_value<int>(get());

				const auto default_speaker_map = get_value_index<unsigned char>("default", speaker_maps_s, std::extent_v<decltype(speaker_maps_s)>, 0);
				alias->speakerMapIndex = get_value_index<unsigned char>(get(), speaker_maps_s, std::extent_v<decltype(speaker_maps_s)>, default_speaker_map);

				alias->flags.reverb = get_value_index<unsigned int>(get(), FULLDRYLEVEL, std::extent_v<decltype(FULLDRYLEVEL)>);
				alias->reverbMultiplier = get_value<float>(get());
				alias->farReverbMultiplier = get_value<float>(get());
				alias->lfePercentage = get_value<float>(get());
				alias->centerPercentage = get_value<float>(get());
				alias->envelopMin = get_value<float>(get());
				alias->envelopMax = get_value<float>(get());
				alias->envelopPercentage = get_value<float>(get());
				alias->flags.shape = get_value_index<unsigned int>(get(), SHAPES, std::extent_v<decltype(SHAPES)>);
				alias->flags.ignoreDistanceCheck = get_value_index<unsigned int>(get(), FALSE_TRUE, std::extent_v<decltype(FALSE_TRUE)>);

				const auto default_occlusion_shape = get_value_index<unsigned char>("default", occlusion_shapes_s, std::extent_v<decltype(occlusion_shapes_s)>, 0);
				alias->occlusionShapeIndex = get_value_index<unsigned char>(get(), occlusion_shapes_s, std::extent_v<decltype(occlusion_shapes_s)>, default_occlusion_shape);

				alias->dopplerPresetIndex = get_value_index<unsigned char>(get(), doppler_presets_s, std::extent_v<decltype(doppler_presets_s)>, 0xFF);

				alias->smartPanDistance2d = get_value<float>(get());
				alias->smartPanDistance3d = get_value<float>(get());
				alias->smartPanAttenuation2d = get_value<float>(get());
				alias->stereoSpreadMinDist = get_value<float>(get());
				alias->stereoSpreadMaxDist = get_value<float>(get());
				alias->stereoSpreadMidPoint = get_value<float>(get());
				alias->stereoSpreadMaxAngle = get_value<int>(get());
				alias->contextType = snd_hash_name(get());
				alias->contextValue = snd_hash_name(get());
				alias->flags.precached = get_value_index<unsigned int>(get(), FALSE_TRUE, std::extent_v<decltype(FALSE_TRUE)>);

				auto duck = get_string();
				if (duck)
				{
					ducks.insert(duck);
					alias->duck = snd_hash_name(duck);
				}

				alias->flags.MusicContent = get_value_index<unsigned int>(get(), MUSIC_CONTENT, std::extent_v<decltype(MUSIC_CONTENT)>);
				alias->flags.GPadOutput = get_value_index<unsigned int>(get(), GPAD_OUTPUT, std::extent_v<decltype(GPAD_OUTPUT)>);
				alias->flags.ForceSubtitle = get_value_index<unsigned int>(get(), FALSE_TRUE, std::extent_v<decltype(FALSE_TRUE)>);
				alias->masterPriority = get_value<int>(get());
				alias->masterPercentage = get_value<float>(get());
				alias->slavePercentage = get_value<float>(get());

				aliases.push_back(alias);
			}
		}

		void parse(SndBank* asset, zone_memory* mem, ordered_json& paths)
		{
			if (!paths.is_array() || !paths.size()) return;

			std::vector<SndAlias*> aliases;
			std::set<std::string> ducks;

			for (size_t i = 0; i < paths.size(); i++)
			{
				const auto path = paths[i].get<std::string>();

				if (!filesystem::file(path).exists())
				{
					ZONETOOL_ERROR("Could not find file %s", path.data());
					continue;
				}

				parse_file(asset, path, aliases, ducks, mem);
			}

			// add ducks
			asset->duckCount = static_cast<unsigned int>(ducks.size()); // since ducks is a std::set it will have no duplicates
			asset->ducks = mem->allocate<SndDuck>(asset->duckCount);
			unsigned int duck_index = 0;
			for (auto& duck : ducks)
			{
				ducks::parse_duck(duck, mem, &asset->ducks[duck_index++]);
			}
			ducks.clear();

			// create temp alias list
			std::vector<SndAliasList> alias_list;
			alias_list.reserve(aliases.size());
			for (size_t i = 0; i < aliases.size(); i++)
			{
				SndAliasList list{};
				unsigned int alias_count = 1;
				list.aliasName = mem->duplicate_string(aliases[i]->aliasName);
				list.id = snd_hash_name(list.aliasName);
				list.head = aliases[i];
				const auto startIndex = i;
				while (i + 1 < aliases.size())
				{
					if (!strcmp(aliases[i]->aliasName, aliases[i + 1]->aliasName))
					{
						alias_count++;
						i++;
					}
					else
						break;
				}
				if (alias_count > 1)
				{
					auto* heads = mem->allocate<SndAlias>(alias_count);
					for (unsigned int h = 0; h < alias_count; h++)
					{
						memcpy(&heads[h], aliases[startIndex + h], sizeof(SndAlias));
						heads[h].aliasName = list.aliasName; // Deduplicate string pointer
					}
					list.head = heads;
				}
				list.count = alias_count;
				list.sequence = 0;

				alias_list.push_back(list);
			}
			aliases.clear();

			if (alias_list.size() >= std::numeric_limits<unsigned short>::max())
			{
				ZONETOOL_FATAL("Zone has too many sound aliases! %d/%d", static_cast<unsigned int>(alias_list.size()), std::numeric_limits<unsigned short>::max() - 1);
			}

			// create final alias list
			asset->aliasCount = static_cast<unsigned int>(alias_list.size());
			asset->alias = mem->allocate<SndAliasList>(asset->aliasCount);
			for (auto i = 0u; i < asset->aliasCount; i++)
			{
				memcpy(&asset->alias[i], &alias_list[i], sizeof(SndAliasList));
			}
			alias_list.clear();

			// create alias index list
			asset->aliasIndex = mem->allocate<SndIndexEntry>(asset->aliasCount);
			memset(asset->aliasIndex, 0xFF, sizeof(SndIndexEntry) * asset->aliasCount);

			const auto setAliasIndexList = std::make_unique<bool[]>(asset->aliasCount);
			memset(setAliasIndexList.get(), false, asset->aliasCount);

			for (unsigned short i = 0; i < asset->aliasCount; i++)
			{
				const auto idx = asset->alias[i].id % asset->aliasCount;
				if (asset->aliasIndex[idx].value == std::numeric_limits<unsigned short>::max())
				{
					asset->aliasIndex[idx].value = i;
					asset->aliasIndex[idx].next = std::numeric_limits<unsigned short>::max();
					setAliasIndexList[i] = true;
				}
			}

			for (unsigned short i = 0u; i < asset->aliasCount; i++)
			{
				if (setAliasIndexList[i])
					continue;

				unsigned short idx = static_cast<unsigned short>(asset->alias[i].id % asset->aliasCount);
				while (asset->aliasIndex[idx].next != std::numeric_limits<unsigned short>::max())
				{
					idx = asset->aliasIndex[idx].next;
				}

				unsigned short offset = 1u;
				auto freeIdx = std::numeric_limits<unsigned short>::max();
				while (true)
				{
					freeIdx = (idx + offset) % asset->aliasCount;
					if (asset->aliasIndex[freeIdx].value == std::numeric_limits<unsigned short>::max())
						break;

					freeIdx = (idx + static_cast<unsigned short>(asset->aliasCount) - offset) % static_cast<unsigned short>(asset->aliasCount);
					if (asset->aliasIndex[freeIdx].value == std::numeric_limits<unsigned short>::max())
						break;

					offset++;
					freeIdx = std::numeric_limits<unsigned short>::max();

					if (offset >= asset->aliasCount)
						break;
				}

				if (freeIdx == std::numeric_limits<unsigned short>::max())
				{
					ZONETOOL_FATAL("Unable to allocate sound bank alias index list");
				}

				asset->aliasIndex[idx].next = freeIdx;
				asset->aliasIndex[freeIdx].value = i;
				asset->aliasIndex[freeIdx].next = std::numeric_limits<unsigned short>::max();
				setAliasIndexList[i] = true;
			}
		}
	}

	namespace send_effect_params
	{
		std::string SND_SENDEFFECT_FIELDS[] =
		{
			"name",
			"smoothing",
			"earlyTime",
			"lateTime",
			"earlyGain",
			"lateGain",
			"lateGainProx",
			"returnGain",
			"earlyLpf",
			"lateLpf",
			"inputLpf",
			"dampLpf",
			"wallReflect",
			"dryGain",
			"earlySize",
			"lateSize",
			"diffusion",
			"rearLevel",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".sendEffectParams.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_SENDEFFECT_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(SndSendEffectParams* asset, csv_writer& writer)
		{
			writer.write_column(reinterpret_cast<const char*>(asset->name));
			writer.write_column(asset->smoothing);
			writer.write_column(asset->earlyTime);
			writer.write_column(asset->lateTime);
			writer.write_column(asset->earlyGain);
			writer.write_column(asset->lateGain);
			writer.write_column(asset->lateGainProx);
			writer.write_column(asset->returnGain);
			writer.write_column(asset->earlyLpf);
			writer.write_column(asset->lateLpf);
			writer.write_column(asset->inputLpf);
			writer.write_column(asset->dampLpf);
			writer.write_column(asset->wallReflect);
			writer.write_column(asset->dryGain);
			writer.write_column(asset->earlySize);
			writer.write_column(asset->lateSize);
			writer.write_column(asset->diffusion);
			writer.write_column(asset->rearLevel);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->sendEffectCount; i++)
			{
				write(&asset->sendEffects[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace adsr_settings
	{
		std::string SND_TAB_ADSR_FIELDS[] =
		{
			"ADSR_Name",
			"Att_Len",
			"Att_Curve",
			"Dec_Len",
			"Dec_Curve",
			"Rel_Len",
			"Rel_Curve",
			"Sus_Len",
			"Sus_Level",
		};

		std::string ADSRCURVES[] =
		{
			"linear",
			"log_fast",
			"log_slow",
			"flat_ended",
			"flat_middle",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".adsr.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_TAB_ADSR_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(ADSRSetting* asset, csv_writer& writer)
		{
			writer.write_column(asset->name);
			writer.write_column(asset->attackLength);
			writer.write_column(ADSRCURVES[asset->attackCurve]);
			writer.write_column(asset->decayLength);
			writer.write_column(ADSRCURVES[asset->decayCurve]);
			writer.write_column(asset->releaseLength);
			writer.write_column(ADSRCURVES[asset->releaseCurve]);
			writer.write_column(asset->sustainLevel);
			writer.write_column(asset->sustainLength);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->soundTable.adsrCount; i++)
			{
				write(&asset->soundTable.adsrSettings[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace ambient_def
	{
		std::string SND_TAB_AMB_DEF_FIELDS[] =
		{
			"ambient_name",
			"Time_Min",
			"Time_Max",
			"EventIndex",
			"NumEvents",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".ambient.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_TAB_AMB_DEF_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(AmbientDef* asset, csv_writer& writer)
		{
			writer.write_column(asset->name);
			writer.write_column(asset->timeMin);
			writer.write_column(asset->timeMax);
			writer.write_column(asset->ambientEventIndex);
			writer.write_column(asset->numEvents);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->soundTable.ambientDefCount; i++)
			{
				write(&asset->soundTable.ambientDefs[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace ambient_event
	{
		std::string SND_TAB_AMB_EVT_FIELDS[] =
		{
			"idString",
			"Weight",
			"ElementIndex",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".ambientEvent.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_TAB_AMB_EVT_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(AmbientEvent* asset, csv_writer& writer)
		{
			writer.write_column(asset->id);
			writer.write_column(asset->weight);
			writer.write_column(asset->ambientElementIndex);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->soundTable.ambientEventCount; i++)
			{
				write(&asset->soundTable.ambientEvents[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace ambient_elem
	{
		std::string SND_TAB_AMB_ELEM_FIELDS[] =
		{
			"ambient_element",
			"Range_Min",
			"Range_Max",
			"Cone_Min",
			"Cone_Max",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".ambientElement.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_TAB_AMB_ELEM_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(AmbientElement* asset, csv_writer& writer)
		{
			writer.write_column(asset->id);
			writer.write_column(asset->rangeMin);
			writer.write_column(asset->rangeMax);
			writer.write_column(asset->coneMin);
			writer.write_column(asset->coneMax);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->soundTable.ambientElementsCount; i++)
			{
				write(&asset->soundTable.ambientElements[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace filter
	{
		std::string SND_TAB_FILTER_FIELDS[] =
		{
			"idString",
			"Channel",
			"Band",
			"Type",
			"Freq",
			"Gain",
			"Q",
		};

		std::string snd_eqTypeStrings[] =
		{
			"lowpass",
			"highpass",
			"lowshelf",
			"highshelf",
			"bell",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".filters.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_TAB_FILTER_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(FilterDef* asset, csv_writer& writer)
		{
			writer.write_column(asset->id);
			writer.write_column(ENTCHANNELS[asset->entChannelIdx]);
			writer.write_column(asset->band);
			writer.write_column(snd_eqTypeStrings[asset->type]);
			writer.write_column(asset->freq);
			writer.write_column(asset->gain);
			writer.write_column(asset->q);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->soundTable.filterCount; i++)
			{
				write(&asset->soundTable.filters[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace mix
	{
		std::string SND_TAB_MIX_FIELDS[] =
		{
			"idString",
			"VolMod",
			"Volume",
			"Fade",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".mixes.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_TAB_MIX_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(MixDef* asset, csv_writer& writer)
		{
			writer.write_column(asset->id);
			writer.write_column(VOLMODS[asset->volModIndex]);
			writer.write_column(asset->volume);
			writer.write_column(asset->fade);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->soundTable.mixCount; i++)
			{
				write(&asset->soundTable.mixes[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace adsr_zone
	{
		std::string lookup_hash_string(SndBank* bank, std::uint32_t hash)
		{
			for (auto i = 0u; i < bank->soundTable.zoneCount; i++)
			{
				if (snd_hash_name(bank->soundTable.zones[i].npcAdsrName) == hash)
				{
					return bank->soundTable.zones[i].npcAdsrName;
				}
				else if (snd_hash_name(bank->soundTable.zones[i].plrAdsrName) == hash)
				{
					return bank->soundTable.zones[i].plrAdsrName;
				}
			}
			return "";
		}

		void write(SndBank* bank, AdsrZoneEntry* asset, csv_writer& writer)
		{
			writer.write_column(lookup_hash_string(bank, asset->id));
			writer.write_column(asset->id);
			writer.write_column(asset->adsrIdx);
			writer.write_column(asset->weaponName ? asset->weaponName : "");
			writer.write_column(asset->weaponIdx);

			writer.increase_line();
		}

		namespace npc
		{
			std::string SND_TAB_ADSR_ZONE_NPC_FIELDS[] =
			{
				"Adsr_Zone_NPC",
				"idString",
				"ADSRIndex",
				"Weapon",
				"WeaponIdx",
			};

			std::string get_path(SndBank* asset)
			{
				return sound_path + asset->name + ".adsrZoneNPC.csv";
			}

			void write_header(csv_writer& writer)
			{
				for (const auto& field : SND_TAB_ADSR_ZONE_NPC_FIELDS)
				{
					writer.write_column(field);
				}
				writer.increase_line();
			}

			void dump(SndBank* asset)
			{
				csv_writer writer{};

				write_header(writer);

				for (auto i = 0u; i < asset->soundTable.npcADSRZoneCount; i++)
				{
					write(asset, &asset->soundTable.npcADSRZones[i], writer);
				}

				writer.save(get_path(asset));
			}
		}

		namespace plr
		{
			std::string SND_TAB_ADSR_ZONE_PLR_FIELDS[] =
			{
				"Adsr_Zone_Player",
				"idString",
				"ADSRIndex",
				"Weapon",
				"WeaponIdx",
			};

			std::string get_path(SndBank* asset)
			{
				return sound_path + asset->name + ".adsrZonePlayer.csv";
			}

			void write_header(csv_writer& writer)
			{
				for (const auto& field : SND_TAB_ADSR_ZONE_PLR_FIELDS)
				{
					writer.write_column(field);
				}
				writer.increase_line();
			}

			void dump(SndBank* asset)
			{
				csv_writer writer{};

				write_header(writer);

				for (auto i = 0u; i < asset->soundTable.playerADSRZoneCount; i++)
				{
					write(asset, &asset->soundTable.playerADSRZones[i], writer);
				}

				writer.save(get_path(asset));
			}
		}
	}

	namespace occlude_def
	{
		std::string SND_TAB_OCCL_FIELDS[] =
		{
			"idString",
			"Channel",
			"CutoffLevel1",
			"CutoffLevel2",
			"CutoffLevel3",
			"CutoffLevel4",
			"DbLevel1",
			"DbLevel2",
			"DbLevel3",
			"DbLevel4",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".occlusionFilters.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_TAB_OCCL_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(OccludeDef* asset, csv_writer& writer)
		{
			writer.write_column(asset->id);
			writer.write_column(ENTCHANNELS[asset->entChannelIdx]);
			writer.write_column(asset->cutoffLevel1);
			writer.write_column(asset->cutoffLevel2);
			writer.write_column(asset->cutoffLevel3);
			writer.write_column(asset->cutoffLevel4);
			writer.write_column(asset->attnLevel1);
			writer.write_column(asset->attnLevel2);
			writer.write_column(asset->attnLevel3);
			writer.write_column(asset->attnLevel4);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->soundTable.occlusionFilterCount; i++)
			{
				write(&asset->soundTable.occlusionFilters[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace reverb_def
	{
		std::string SND_TAB_REVERB_FIELDS[] =
		{
			"reverb_name",
			"idString",
			"Roomtype",
			"DryLevel",
			"WetLevel",
			"Fade",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".reverbs.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_TAB_REVERB_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		std::string lookup_hash_string(SndBank* bank, std::uint32_t hash)
		{
			for (auto i = 0u; i < bank->soundTable.zoneCount; i++)
			{
				if (snd_hash_name(bank->soundTable.zones[i].reverbName) == hash)
				{
					return bank->soundTable.zones[i].reverbName;
				}
			}
			return "";
		}

		void write(SndBank* bank, ReverbDef* asset, csv_writer& writer)
		{
			writer.write_column(lookup_hash_string(bank, asset->id));
			writer.write_column(asset->id);
			writer.write_column(std::string(asset->roomType));
			writer.write_column(asset->dryLevel);
			writer.write_column(asset->wetLevel);
			writer.write_column(asset->fadeTime);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->soundTable.reverbCount; i++)
			{
				write(asset, &asset->soundTable.reverbs[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace timescale
	{
		std::string SND_TAB_TIMESCALE_FIELDS[] =
		{
			"idString",
			"Timescale_Name",
			"Channel",
			"Scale",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".timescales.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_TAB_TIMESCALE_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(TimescaleEntry* asset, csv_writer& writer)
		{
			writer.write_column(asset->id);
			writer.write_column(asset->presetName);
			writer.write_column(ENTCHANNELS[asset->entChannelIdx]);
			writer.write_column(asset->scale);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->soundTable.timeScaleSettingCount; i++)
			{
				write(&asset->soundTable.timeScaleSettings[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace zone
	{
		std::string SND_TAB_ZONE_FIELDS[] =
		{
			"ReverbIndex",
			"OcclusionIndex",
			"NumOcclusion",
			"FilterIndex",
			"NumFilter",
			"MixIndex",
			"NumMix",
			"PlrADSRZoneIndex",
			"NumPlrADSRZone",
			"NPCADSRZoneIndex",
			"NumNPCADSRZone",
			"AmbientIndex",
			"UnkIndex",
			"DisableFullOccIndex",
			"NumDisableFullOcc",
			"PlayerBreathStateIndex",
			"NumPlayerBreathStates",
			"Weapon_Reflection",
			//"Exterior",
			"EntityContextType1",
			"EntityContextValue1",
			"EntityContextType2",
			"EntityContextValue2",
			"EntityContextType3",
			"EntityContextValue3",
			"EntityContextType4",
			"EntityContextValue4",
			//"Duck",
			"Zone_Name",
			"State",
			"Ambience",
			"Ambient_Name",
			"Mix",
			"Reverb",
			"Filter",
			"Occlusion",
			"Player_Adsr",
			"NPC_Adsr",
			"Weapon_Reflection",
			"Disable_Full_Occ",
			"PlayerBreathState",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".zone.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_TAB_ZONE_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(ZoneDef* asset, csv_writer& writer)
		{
			writer.write_column(asset->reverbIndex);
			writer.write_column(asset->startOcclusionIndex);
			writer.write_column(asset->numOcclusion);
			writer.write_column(asset->startFilterIndex);
			writer.write_column(asset->numFilter);
			writer.write_column(asset->startMixIndex);
			writer.write_column(asset->numMix);
			writer.write_column(asset->startAdsrZoneIndex);
			writer.write_column(asset->numAdsrZone);
			writer.write_column(asset->startNPCAdsrZoneIndex);
			writer.write_column(asset->numNPCAdsrZone);
			writer.write_column(asset->ambientDefIndex);
			writer.write_column(asset->unkIndex);
			writer.write_column(asset->startFullOccIndex);
			writer.write_column(asset->numDisableFullOcc);
			writer.write_column(asset->startPlayerBreathStateIndex);
			writer.write_column(asset->numPlayerBreathStates);
			writer.write_column(asset->weapReflId);
			// exterior
			writer.write_column(asset->contextType1);
			writer.write_column(asset->contextValue1);
			writer.write_column(asset->contextType2);
			writer.write_column(asset->contextValue2);
			writer.write_column(asset->contextType3);
			writer.write_column(asset->contextValue3);
			writer.write_column(asset->contextType4);
			writer.write_column(asset->contextValue4);
			//duck
			writer.write_column(asset->zoneName ? asset->zoneName : "");
			writer.write_column(asset->stateName ? asset->stateName : "");
			writer.write_column(asset->ambientStream ? asset->ambientStream : "");
			writer.write_column(asset->ambientName ? asset->ambientName : "");
			writer.write_column(asset->mixName ? asset->mixName : "");
			writer.write_column(asset->reverbName ? asset->reverbName : "");
			writer.write_column(asset->filterName ? asset->filterName : "");
			writer.write_column(asset->occlusionName ? asset->occlusionName : "");
			writer.write_column(asset->plrAdsrName ? asset->plrAdsrName : "");
			writer.write_column(asset->npcAdsrName ? asset->npcAdsrName : "");
			writer.write_column(asset->weapReflName ? asset->weapReflName : "");
			writer.write_column(asset->fullOccName ? asset->fullOccName : "");
			writer.write_column(asset->playerBreathStateName ? asset->playerBreathStateName : "");

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->soundTable.zoneCount; i++)
			{
				write(&asset->soundTable.zones[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace unk
	{
		std::string get_path(SndBank* asset)
		{
			return sound_path + "unk/"s + asset->name + ".json"s;
		}

		void dump(SndBank* asset)
		{
			auto file = filesystem::file(get_path(asset));
			file.open("wb");

			ordered_json data;

			for (auto i = 0u; i < asset->soundTable.unkCount; i++)
			{
				auto* unk = &asset->soundTable.unk[i];
				data[i]["id"] = unk->id;
				data[i]["name"] = unk->name;
				data[i]["unk3"] = unk->unk3;
				data[i]["unk4"] = unk->unk4;
				data[i]["unk5"] = unk->unk5;
				data[i]["unk6"] = unk->unk6;
				data[i]["unk7"] = unk->unk7;
				data[i]["unk8"] = unk->unk8;
				data[i]["unk9"] = unk->unk9;
				data[i]["unk10"] = unk->unk10;
				data[i]["unk11"] = unk->unk11;
				data[i]["unk12"] = unk->unk12;
			}

			file.write(data.dump(4));
			file.close();
		}
	}

	namespace full_occlusion_def
	{
		std::string SND_TAB_FULLOCC_FIELDS[] =
		{
			"idString",
			"PresetName",
			"Channel",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".fullOcclusions.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_TAB_FULLOCC_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(FullOcclusionDef* asset, csv_writer& writer)
		{
			writer.write_column(asset->id);
			writer.write_column(asset->presetName);
			writer.write_column(ENTCHANNELS[asset->entChannelIdx]);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->soundTable.fullOcclusionDefCount; i++)
			{
				write(&asset->soundTable.fullOcclusionDefs[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace plr_breath_state
	{
		std::string SND_TAB_PLRBREATHSTATE_FIELDS[] =
		{
			"StateType",
			"InhaleAlias",
			"ExhaleAlias",
			"MinBreaths",
		};

		std::string get_path(SndBank* asset)
		{
			return sound_path + asset->name + ".playerBreathStates.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_TAB_PLRBREATHSTATE_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(PlayerBreathStateDef* asset, csv_writer& writer)
		{
			writer.write_column(asset->stateType);
			writer.write_column(asset->inhaleAlias ? asset->inhaleAlias : "");
			writer.write_column(asset->exhaleAlias ? asset->exhaleAlias : "");
			writer.write_column(asset->minBreaths);

			writer.increase_line();
		}

		void dump(SndBank* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->soundTable.plrBreathStateDefCount; i++)
			{
				write(&asset->soundTable.plrBreathStateDefs[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace ducks
	{
		void dump_duck(SndDuck* asset)
		{
			const auto path = sound_path + "ducks/"s + asset->name + ".duk"s;
			auto file = filesystem::file(path);
			file.open("wb");

			ordered_json data;

			data["Name"] = asset->name;
			data["FadeIn"] = asset->fadeIn;
			data["FadeOut"] = asset->fadeOut;
			data["StartDelay"] = asset->startDelay;
			data["MinDistance"] = asset->minDistance;
			data["Distance"] = asset->distance;
			data["Length"] = asset->length;
			data["FadeInCurve"] = get_hashed_string(asset->fadeInCurve);
			data["FadeInCurveId"] = asset->fadeInCurve;
			data["FadeOutCurve"] = get_hashed_string(asset->fadeOutCurve);
			data["FadeOutCurveId"] = asset->fadeOutCurve;
			data["UpdateWhilePaused"] = asset->updateWhilePaused;
			data["TrackAmplitude"] = asset->trackAmplitude;
			data["DisableInSplitscreen"] = asset->disableInSplitscreen;
			data["DuckAliasAttenuation"] = asset->aliasAttenuation;
			data["DuckAliasLpf"] = asset->aliasLpf;
			data["DuckAlias"] = asset->duckAlias;
			data["Attenuation"] = {};
			if (asset->attenuation)
			{
				for (auto i = 0; i < 128; i++)
				{
					data["Attenuation"][i] = asset->attenuation[i];
				}
			}
			data["Lpf"] = {};
			if (asset->lpf)
			{
				for (auto i = 0; i < 128; i++)
				{
					data["Lpf"][i] = asset->lpf[i];
				}
			}

			file.write(data.dump(4));
			file.close();
		}

		void dump(SndBank* asset)
		{
			for (auto i = 0u; i < asset->duckCount; i++)
			{
				dump_duck(&asset->ducks[i]);
			}
		}

		bool parse_duck(const std::string& name, zone_memory* mem, SndDuck* asset)
		{
			const auto path = sound_path + "ducks/"s + name + ".duk"s;

			auto file = filesystem::file(path);
			if (!file.exists())
			{
				ZONETOOL_FATAL("Could not find sound duck file %s", path.data());
				return false;
			}

			ZONETOOL_INFO("Parsing duck \"%s\"...", name.data());

			// parse json file
			file.open("rb");
			ordered_json data = json::parse(file.read_bytes(file.size()));
			file.close();

			{
				auto name_str = get_json_value<std::string>(data["Name"]);
				memcpy_s(asset->name, sizeof(asset->name), name_str.data(), name_str.size());
				asset->id = snd_hash_name(asset->name);
			}
			asset->fadeIn = get_json_value<float>(data["FadeIn"]);
			asset->fadeOut = get_json_value<float>(data["FadeOut"]);
			asset->minDistance = get_json_value<float>(data["MinDistance"]);
			asset->distance = get_json_value<float>(data["Distance"]);
			asset->length = get_json_value<float>(data["Length"]);
			asset->fadeInCurve = get_json_value<std::uint32_t>(data["FadeInCurveId"]);
			{
				auto name_str = get_json_value<std::string>(data["FadeInCurve"]);
				if (!name_str.empty())
				{
					asset->fadeInCurve = snd_hash_name(name_str.data());
				}
			}
			asset->fadeOutCurve = get_json_value<std::uint32_t>(data["FadeOutCurveId"]);
			{
				auto name_str = get_json_value<std::string>(data["FadeOutCurve"]);
				if (!name_str.empty())
				{
					asset->fadeOutCurve = snd_hash_name(name_str.data());
				}
			}
			asset->updateWhilePaused = get_json_value<bool>(data["UpdateWhilePaused"]);
			asset->trackAmplitude = get_json_value<bool>(data["TrackAmplitude"]);
			asset->disableInSplitscreen = get_json_value<bool>(data["DisableInSplitscreen"]);
			asset->aliasAttenuation = get_json_value<float>(data["DuckAliasAttenuation"]);
			asset->aliasLpf = get_json_value<float>(data["DuckAliasLpf"]);
			asset->duckAlias = get_json_value<std::uint32_t>(data["DuckAlias"]);
			if (data["Attenuation"].is_array())
			{
				asset->attenuation = mem->allocate<float>(128);
				for (auto i = 0; i < 128; i++)
				{
					asset->attenuation[i] = get_json_value<float>(data["Attenuation"][i]);
				}
			}
			if (data["Lpf"].is_array())
			{
				asset->lpf = mem->allocate<float>(128);
				for (auto i = 0; i < 128; i++)
				{
					asset->lpf[i] = get_json_value<float>(data["Lpf"][i]);
				}
			}

			return true;
		}
	}

	namespace music
	{

	}

	namespace sound_asset_bank
	{
		constexpr unsigned int MAGIC = 0x23585532;
		constexpr unsigned int VERSION = 4;

		namespace flac
		{
			constexpr auto MARKER = "fLaC";
			constexpr auto MARKER_LEN = 4;
			constexpr auto BLOCK_SIZE = 0x400;
			constexpr auto FRAME_SIZE_UNKNOWN = 0;
			constexpr auto BITS_PER_SAMPLE_DEFAULT = 16;

			constexpr auto METADATA_STREAMINFO_MIN_BLOCK_SIZE_LEN_BITS = 16; /**< == 16 (bits) */
			constexpr auto METADATA_STREAMINFO_MAX_BLOCK_SIZE_LEN_BITS = 16; /**< == 16 (bits) */
			constexpr auto METADATA_STREAMINFO_MIN_FRAME_SIZE_LEN_BITS = 24; /**< == 24 (bits) */
			constexpr auto METADATA_STREAMINFO_MAX_FRAME_SIZE_LEN_BITS = 24; /**< == 24 (bits) */
			constexpr auto METADATA_STREAMINFO_SAMPLE_RATE_LEN_BITS = 20; /**< == 20 (bits) */
			constexpr auto METADATA_STREAMINFO_CHANNELS_LEN_BITS = 3; /**< == 3 (bits) */
			constexpr auto METADATA_STREAMINFO_BITS_PER_SAMPLE_LEN_BITS = 5; /**< == 5 (bits) */
			constexpr auto METADATA_STREAMINFO_TOTAL_SAMPLES_LEN_BITS = 36; /**< == 36 (bits) */
			constexpr auto METADATA_STREAMINFO_MD5SUM_LEN_BITS = 128; /**< == 128 (bits) */

			constexpr auto METADATA_STREAMINFO_LEN_BITS =
				METADATA_STREAMINFO_MIN_BLOCK_SIZE_LEN_BITS +
				METADATA_STREAMINFO_MAX_BLOCK_SIZE_LEN_BITS +
				METADATA_STREAMINFO_MIN_FRAME_SIZE_LEN_BITS +
				METADATA_STREAMINFO_MAX_FRAME_SIZE_LEN_BITS +
				METADATA_STREAMINFO_SAMPLE_RATE_LEN_BITS +
				METADATA_STREAMINFO_CHANNELS_LEN_BITS +
				METADATA_STREAMINFO_BITS_PER_SAMPLE_LEN_BITS +
				METADATA_STREAMINFO_TOTAL_SAMPLES_LEN_BITS +
				METADATA_STREAMINFO_MD5SUM_LEN_BITS;

			constexpr auto METADATA_STREAMINFO_LEN_BYTES = METADATA_STREAMINFO_LEN_BITS / 8;

			constexpr auto METADATA_BLOCK_HEADER_LEN_BYTES = 4;

			enum MetaDataBlockType : uint8_t
			{
				STREAMINFO = 0,
				PADDING = 1,
				APPLICATION = 2,
				SEEKTABLE = 3,
				VORBIS_COMMENT = 4,
				CUESHEET = 5,
				PICTURE = 6
			};

			struct MetaDataBlockHeader
			{
				bool isLastMetaDataBlock;
				MetaDataBlockType blockType;
				uint32_t blockLength;
			};

			struct StreamInfo
			{
				uint32_t min_blocksize, max_blocksize;
				uint32_t min_framesize, max_framesize;
				uint32_t sample_rate;
				uint32_t channels;
				uint32_t bits_per_sample;
				uint64_t total_samples;
				uint8_t md5sum[16];
			};

			std::vector<std::uint8_t> write_metadata_streaminfo(StreamInfo info)
			{
				std::vector<std::uint8_t> buffer{};
				buffer.resize(METADATA_STREAMINFO_LEN_BYTES);

				{
					// Write min/max blocksize
					uint16_t min = _byteswap_ushort((uint16_t)info.min_blocksize);
					uint16_t max = _byteswap_ushort((uint16_t)info.max_blocksize);

					memcpy(&buffer[0], &min, 2);
					memcpy(&buffer[2], &max, 2);
				}

				{
					// Write min/max framesize
					uint8_t b[6]{};
					b[0] = (char)((info.min_framesize >> 16) & 0xFF);
					b[1] = (char)((info.min_framesize >> 8) & 0xFF);
					b[2] = (char)(info.min_framesize & 0xFF);
					b[3] = (char)((info.max_framesize >> 16) & 0xFF);
					b[4] = (char)((info.max_framesize >> 8) & 0xFF);
					b[5] = (char)(info.max_framesize & 0xFF);

					memcpy(&buffer[4], b, 6);
				}

				{
					// Write sample rate and channels
					uint8_t b[3]{};
					b[0] = (char)(((info.sample_rate) >> 12) & 0xFF);
					b[1] = (char)(((info.sample_rate) >> 4) & 0xFF);
					b[2] = ((char)(info.channels - 1) << 1) | (char)((info.sample_rate << 4) & 0xF0);
					memcpy(&buffer[10], b, 3);
				}

				{
					// Write total samples and bits per sample
					uint8_t b[5]{};
					b[0] = ((char)(info.bits_per_sample - 1) << 4) | (char)((info.total_samples >> 32) & 0x0F);
					b[1] = (char)((info.total_samples >> 24) & 0xFF);
					b[2] = (char)((info.total_samples >> 16) & 0xFF);
					b[3] = (char)((info.total_samples >> 8) & 0xFF);
					b[4] = (char)(info.total_samples & 0xFF);

					memcpy(&buffer[13], b, 5);
				}

				{
					// Write md5
					memcpy(&buffer[18], info.md5sum, 16);
				}

				return buffer;
			}

			std::vector<std::uint8_t> write_metadata_block_header(bool is_last, MetaDataBlockType type, uint32_t length)
			{
				std::vector<std::uint8_t> buffer{};
				buffer.resize(METADATA_BLOCK_HEADER_LEN_BYTES);

				{
					uint8_t b[METADATA_BLOCK_HEADER_LEN_BYTES]{};
					b[0] = ((char)(type) << 0) | ((char)(is_last) << 7);
					b[1] = (char)((length >> 16) & 0xFF);
					b[2] = (char)((length >> 8) & 0xFF);
					b[3] = (char)(length & 0xFF);

					memcpy(&buffer[0], b, METADATA_BLOCK_HEADER_LEN_BYTES);
				}

				return buffer;
			}

			std::vector<std::uint8_t> write_marker()
			{
				std::vector<std::uint8_t> buffer{};
				buffer.resize(MARKER_LEN);

				memcpy(&buffer[0], MARKER, MARKER_LEN);

				return buffer;
			}

			void dump(SndAssetBankEntry& entry, const std::string& name, uint8_t* data, size_t size)
			{
				std::string buffer{};

				const auto write = [&](std::vector<std::uint8_t> data_)
				{
					buffer.append(reinterpret_cast<const char*>(data_.data()), data_.size());
				};

				write(write_marker());

				{
					bool has_seektable = false;
					if (entry.seekTableSize)
					{
						has_seektable = true;
					}

					write(write_metadata_block_header(has_seektable ? false : true, STREAMINFO, METADATA_STREAMINFO_LEN_BYTES));
					StreamInfo info{};
					info.min_blocksize = BLOCK_SIZE;
					info.max_blocksize = BLOCK_SIZE;
					info.min_framesize = FRAME_SIZE_UNKNOWN;
					info.max_framesize = FRAME_SIZE_UNKNOWN;
					info.sample_rate = entry.frameRate;
					info.channels = entry.channelCount;
					info.bits_per_sample = BITS_PER_SAMPLE_DEFAULT;
					info.total_samples = entry.frameCount;
					memset(info.md5sum, 0x00, sizeof(info.md5sum));
					write(write_metadata_streaminfo(info));

					if (has_seektable)
					{
						write(write_metadata_block_header(true, SEEKTABLE, entry.seekTableSize));
					}
				}

				buffer.append(reinterpret_cast<const char*>(data), size);

				const auto path = sound_path_assets + name + ".flac"s;
				auto file = filesystem::file(path);
				file.open("wb");
				file.write(buffer.data(), buffer.size());
				file.close();
			}

			bool verify_marker(std::vector<uint8_t>& data)
			{
				if (data.size() < MARKER_LEN)
				{
					return false;
				}

				return std::memcmp(data.data(), MARKER, MARKER_LEN) == 0;
			}

			bool parse(std::string& path, SndAssetBankEntry* entry, filesystem::file& out_file, unsigned char* checksum, unsigned char* source_checksum)
			{
				auto file = filesystem::file(path);

				if (!file.exists())
				{
					ZONETOOL_ERROR("Flac file %s could not be found", path.data());
					return false;
				}

				file.open("rb");
				auto data = file.read_bytes(file.size());
				file.close();

				const auto start_pos = data.data();
				const auto end_pos = start_pos + data.size();

				// verify marker
				if (!verify_marker(data))
				{
					ZONETOOL_ERROR("Failed to verify flac marker for file %s", path.data());
					return false;
				}
				auto pos = start_pos;
				pos += 4; // skip "fLaC"

				std::string buffer{};

				bool has_placed_seektable = false;
				while (pos < end_pos)
				{
					if (pos + METADATA_BLOCK_HEADER_LEN_BYTES > end_pos)
					{
						ZONETOOL_ERROR("unexpected eof");
						return false;
					}

					utils::bit_buffer header_buffer{ std::string(pos, pos + METADATA_BLOCK_HEADER_LEN_BYTES) }; // header
					MetaDataBlockHeader header{};
					header.isLastMetaDataBlock = header_buffer.read_bits<bool>(1);
					header.blockType = header_buffer.read_bits<MetaDataBlockType>(7);
					header.blockLength = header_buffer.read_bits<std::uint32_t>(24);
					pos += METADATA_BLOCK_HEADER_LEN_BYTES;

					if (header.blockType == STREAMINFO)
					{
						assert(header.blockLength == METADATA_STREAMINFO_LEN_BYTES);
						utils::bit_buffer block_buffer{std::string(pos, pos + header.blockLength)};
						StreamInfo info{};
						info.min_blocksize = block_buffer.read_bits<uint32_t>(16);
						info.max_blocksize = block_buffer.read_bits<uint32_t>(16);
						info.min_framesize = block_buffer.read_bits<uint32_t>(24);
						info.max_framesize = block_buffer.read_bits<uint32_t>(24);
						info.sample_rate = block_buffer.read_bits<uint32_t>(20);
						info.channels = static_cast<uint8_t>(block_buffer.read_bits<uint8_t>(3) + 1);
						info.bits_per_sample = static_cast<uint8_t>(block_buffer.read_bits<uint8_t>(5) + 1);
						info.total_samples = block_buffer.read_bits<uint64_t>(36);
						block_buffer.read_buffer(info.md5sum, 128);

						memcpy(source_checksum, info.md5sum, 16);

						entry->frameRate = static_cast<int>(info.sample_rate);
						entry->channelCount = static_cast<char>(info.channels);
						entry->frameCount = static_cast<unsigned int>(info.total_samples);
					}
					else if(header.blockType == SEEKTABLE)
					{
						if (has_placed_seektable)
						{
							ZONETOOL_FATAL("double seektable in flac file %s ?!", path.data());
						}

						entry->seekTableSize = header.blockLength;
						buffer.append(pos, pos + header.blockLength);

						has_placed_seektable = true;
					}

					pos += header.blockLength;

					if (header.isLastMetaDataBlock)
					{
						break;
					}
				}

				std::string sound_data(pos, end_pos);
				buffer.append(sound_data);

				if (!buffer.size())
				{
					ZONETOOL_ERROR("Failed to parse flac file %s", path.data());
					return false;
				}

				hash_state md{};
				md5_init(&md);
				md5_process(&md, reinterpret_cast<const unsigned char*>(buffer.data()), static_cast<unsigned int>(buffer.size()));
				md5_done(&md, checksum);

				out_file.write(buffer.data(), buffer.size());
				entry->size = static_cast<unsigned int>(buffer.size() - entry->seekTableSize);

				return true;
			}
		}

		namespace pcm
		{
			constexpr auto BLOCK_SIZE = 0x400;
			constexpr auto BITS_PER_SAMPLE_DEFAULT = 16;

			void dump(SndAssetBankEntry& entry, const std::string& name, uint8_t* data, size_t size)
			{
				const auto path = sound_path_assets + name + ".wav"s;
				auto file = filesystem::file(path);

				file.open("wb");

				char chunkID[] = { 'R', 'I', 'F', 'F' };
				file.write(chunkID, 4, 1);

				// ChunkSize
				int subchunk1Size = 16;
				int subchunk2Size = static_cast<int>(size);
				int chunkSize = 4 + (8 + subchunk1Size) + (8 + subchunk2Size);
				file.write(&chunkSize, 4, 1);

				// Format
				char format[] = { 'W', 'A', 'V', 'E' };
				file.write(format, 4, 1);

				// --- FMT SUBCHUNK
				// Subchunk1ID
				char subchunk1ID[] = { 'f', 'm', 't', ' ' };
				file.write(subchunk1ID, 4, 1);

				// Subchunk1Size
				file.write(&subchunk1Size, 4, 1);

				// AudioFormat
				short audioFormat = 1; // PCM
				file.write(&audioFormat, 2, 1);

				// NumChannels
				short numChannels = entry.channelCount;
				file.write(&numChannels, 2, 1);

				// SampleRate
				int sampleRate = entry.frameRate;
				file.write(&sampleRate, 4, 1);

				// ByteRate
				int byteRate = entry.frameRate * entry.channelCount * BITS_PER_SAMPLE_DEFAULT / 8;
				file.write(&byteRate, 4, 1);

				// BlockAlign
				short blockAlign = BLOCK_SIZE;
				file.write(&blockAlign, 2, 1);

				// BitsPerSample
				short bitsPerSample = BITS_PER_SAMPLE_DEFAULT;
				file.write(&bitsPerSample, 2, 1);

				// --- DATA SUBCHUNK
					// Subchunk2ID
				char subchunk2ID[] = { 'd', 'a', 't', 'a' };
				file.write(subchunk2ID, 4, 1);

				// Subchunk2Size
				file.write(&subchunk2Size, 4, 1);

				// Data
				file.write(data, size, 1);

				file.close();
			}

			bool parse(std::string& path, SndAssetBankEntry* entry, filesystem::file& out_file, unsigned char* checksum, unsigned char* source_checksum)
			{
				filesystem::file file(path);

				if (!file.exists())
				{
					ZONETOOL_ERROR("Wav file %s could not be found", path.data());
					return false;
				}

				file.open("rb");
				auto* fp = file.get_fp();

				unsigned int chunkIDBuffer = 0;
				unsigned int chunkSize = 0;
				short format = 0;
				short numChannels = 0;
				int sampleRate = 0;
				int byteRate = 0;
				short blockAlign = 0;
				short bitPerSample = 0;

				std::vector<std::uint8_t> buffer{};
				bool wrote_data = false;

				fread(&chunkIDBuffer, 4, 1, fp);
				if (chunkIDBuffer != 0x46464952) // RIFF
				{
					ZONETOOL_FATAL("%s: Invalid RIFF Header.", path.data());
				}

				fread(&chunkSize, 4, 1, fp);
				fread(&chunkIDBuffer, 4, 1, fp);

				if (chunkIDBuffer != 0x45564157) // WAVE
				{
					ZONETOOL_FATAL("%s: Invalid WAVE Header.", path.data());
				}

				while (!wrote_data && !feof(fp))
				{
					fread(&chunkIDBuffer, 4, 1, fp);
					fread(&chunkSize, 4, 1, fp);
					switch (chunkIDBuffer)
					{
					case 0x20746D66: // fmt
						if (chunkSize >= 16)
						{
							fread(&format, 2, 1, fp);
							if (format != 1 && format != 17)
							{
								ZONETOOL_FATAL("%s: Invalid wave format %i.", path.data(), format);
							}
							
							fread(&numChannels, 2, 1, fp);
							entry->channelCount = static_cast<char>(numChannels);
							
							fread(&sampleRate, 4, 1, fp);
							entry->frameRate = sampleRate;
							
							fread(&byteRate, 4, 1, fp);
							
							fread(&blockAlign, 2, 1, fp);
							
							fread(&bitPerSample, 2, 1, fp);

							if (chunkSize > 16)
							{
								fseek(fp, chunkSize - 16, SEEK_CUR);
							}
						}
						break;

					case 0x61746164: // data
						buffer.resize(chunkSize);
						fread(buffer.data(), 1, chunkSize, fp);

						entry->size = chunkSize;
						entry->frameCount = chunkSize / (numChannels * bitPerSample / 8);

						wrote_data = true;
						
						break;

					default:
						if (chunkSize > 0)
						{
							fseek(fp, chunkSize, SEEK_CUR);
						}
						break;
					}
				}

				file.close();

				if (!buffer.size())
				{
					ZONETOOL_ERROR("Failed to parse wav file %s", path.data());
					return false;
				}

				assert(static_cast<unsigned int>(buffer.size()) == chunkSize);

				hash_state md{};
				md5_init(&md);
				md5_process(&md, reinterpret_cast<const unsigned char*>(buffer.data()), static_cast<unsigned int>(buffer.size()));
				md5_done(&md, checksum);

				out_file.write(buffer.data(), buffer.size());
				entry->size = static_cast<unsigned int>(buffer.size() - entry->seekTableSize);
				
				return true;
			}
		}

		bool use_zone_dir()
		{
			bool use_zone = false;
			if (utils::io::directory_exists("zone")) use_zone = true;
			return use_zone;
		}

		std::string create_path(SndBank* bank, bool streamed)
		{
			std::string path;
			if (use_zone_dir())
			{
				path.append("zone/");
			}
			if (bank->soundLanguage && bank->soundLanguage != "all"s)
			{
				path.append(bank->soundLanguage);
				path.append("/");
			}
			if (bank->gameLanguage && bank->gameLanguage != "all"s)
			{
				path.append(bank->gameLanguage);
				path.append("_");
			}
			path.append(bank->zone);
			if (streamed)
			{
				path.append(".sabs");
			}
			else
			{
				path.append(".sabl");
			}

			return path;
		}

		std::string create_path(std::string name, std::string language_folder, std::string language_prefix)
		{
			std::string path;
			if (use_zone_dir())
			{
				path.append("zone/");
			}
			if (!language_folder.empty())
			{
				path.append(language_folder);
				if (!language_folder.ends_with("/") || !language_folder.ends_with("\\"))
				{
					path.append("/");
				}
			}
			if (!language_prefix.empty())
			{
				path.append(language_prefix);
				if (!language_prefix.ends_with("_"))
				{
					path.append("_");
				}
			}
			path.append(name);

			return path;
		}

		bool validate_header(SndAssetBankHeader* header)
		{
			if (header->magic != MAGIC)
			{
				ZONETOOL_ERROR("Sound Asset Bank has wrong header MAGIC");
				return false;
			}
			if (header->version != VERSION)
			{
				ZONETOOL_ERROR("Sound Asset Bank has wrong header VERSION");
				return false;
			}

			return true;
		}

		bool dump_internal(std::string name, std::string language_folder, std::string language_prefix)
		{
			const auto path = create_path(name, language_folder, language_prefix);
			auto file = filesystem::file(path);
			file.open("rb", false, false);
			if (!file.get_fp())
			{
				ZONETOOL_ERROR("Could not open file for read: %s", path.data());
				return false;
			}

			SndAssetBankHeader header{};
			file.read(&header, sizeof(SndAssetBankHeader));

			if (!validate_header(&header))
			{
				return false;
			}

			for (auto i = 0u; i < header.entryCount; i++)
			{
				SndAssetBankEntry entry{};
				file.seek(header.entryOffset + (sizeof(SndAssetBankEntry) * i), SEEK_SET);
				file.read(&entry, sizeof(SndAssetBankEntry));

				char asset_name[128]{};
				memset(asset_name, 0, sizeof(asset_name));
				file.seek(header.AssetNameOffset + (sizeof(asset_name) * i), SEEK_SET);
				file.read(asset_name, sizeof(asset_name));

				file.seek(entry.offset, SEEK_SET);
				auto data = file.read_bytes(entry.size + entry.seekTableSize + entry.hybridPcmSize);

				switch(entry.format)
				{
				case SND_ASSET_FORMAT_FLAC:
					flac::dump(entry, asset_name, data.data(), data.size());
					break;
				case SND_ASSET_FORMAT_PCMS16:
					//pcm::dump(entry, asset_name, data.data(), data.size());
					break;
				default:
					__debugbreak();
					break;
				}
			}

			return true;
		}

		std::string find_asset_file(std::string name, char* format)
		{
			name = sound_path_assets + name;
			if (filesystem::file(name + ".flac").exists())
			{
				*format = SND_ASSET_FORMAT_FLAC;
				return name + ".flac";
			}
			else if (filesystem::file(name + ".wav").exists())
			{
				*format = SND_ASSET_FORMAT_PCMS16;
				return name + ".wav";
			}
			return "";
		}

		size_t align_value(size_t value, unsigned int alignment)
		{
			return (value + alignment - 1) & ~(static_cast<size_t>(alignment) - 1);
		}

		const char* align_value(const char* value, unsigned int alignment)
		{
			return reinterpret_cast<const char*>(align_value(reinterpret_cast<size_t>(value), alignment));
		}

		void create_internal(SndBank* bank, bool streamed)
		{
			SndAssetBankHeader header{};
			header.magic = MAGIC;
			header.version = VERSION;
			header.buildVersion = VERSION;
			header.convertedAssetVersion = VERSION;
			header.entrySize = sizeof(SndAssetBankEntry);
			header.checksumSize = 16;
			header.dependencySize = 64;
			header.dependencyCount = 8;
			memcpy_s(header.zoneName, sizeof(header.zoneName), bank->zone, strlen(bank->zone));
			memcpy_s(header.platform, sizeof(header.platform), "pc", 2);
			if (bank->gameLanguage)
			{
				memcpy_s(header.language, sizeof(header.language), bank->gameLanguage, strlen(bank->gameLanguage));
			}

			const auto path = create_path(bank, streamed);
			if (utils::io::file_exists(path))
			{
				if (path.find(filesystem::get_fastfile()) == std::string::npos)
				{
					const int result = MessageBoxA(NULL, utils::string::va("You are about to overwrite file %s\nAre you sure?", path.data()), "WARNING", MB_YESNOCANCEL);

					switch (result)
					{
					case IDYES:
						break;
					case IDNO:
					case IDCANCEL:
						return;
						break;
					}
				}

				utils::io::remove_file(path);
			}

			ZONETOOL_INFO("Creating sound asset bank %s", path.data());

			auto file = filesystem::file(path);
			file.open("ab", false, false);

			std::uint64_t data_offset = 0;

			const auto padding = '\0';
			const auto align = [&](const unsigned int alignment)
			{
				auto align_val = align_value(data_offset, alignment) - data_offset;
				while (align_val)
				{
					align_val--;
					file.write(&padding);
					data_offset++;
				}
			};

			// write header, we will come back to this later
			file.write(&header);
			data_offset += sizeof(header);

			align(0x1000);

			assert(data_offset == 0x1000);

			typedef unsigned char checksum128_t[16];
			struct checksum128_s
			{
				checksum128_t md5;
			};

			std::vector<SndAssetBankEntry> entries{};
			std::vector<std::string> assets{};
			std::vector<checksum128_s> checksums{};
			std::vector<checksum128_s> source_checksums{};
			std::unordered_set<SndStringHash> inserted_sound;
			const auto asset_offset_start = data_offset;
			for (auto i = 0u; i < bank->aliasCount; i++)
			{
				for (auto j = 0; j < bank->alias[i].count; j++)
				{
					auto* alias = &bank->alias[i].head[j];
					if (!alias->assetFileName)
					{
						continue;
					}

					if (inserted_sound.contains(alias->assetId))
					{
						continue;
					}

					if (streamed)
					{
						if (alias->flags.type != SAT_STREAMED && alias->flags.type != SAT_PRIMED && alias->flags.type != SAT_HYBRID_PCM)
						{
							continue;
						}
					}

					if (!streamed)
					{
						if (alias->flags.type != SAT_LOADED)
						{
							continue;
						}
					}

					SndAssetBankEntry entry{};
					entry.id = alias->assetId;
					entry.looping = alias->flags.looping;
					entry.offset = data_offset;

					auto asset_file = find_asset_file(alias->assetFileName, &entry.format);
					if (asset_file.empty())
					{
						ZONETOOL_ERROR("Could not find sound asset %s", alias->assetFileName);
						continue;
					}

					checksum128_s checksum{};
					checksum128_s source_checksum{};

					switch (entry.format)
					{
					case SND_ASSET_FORMAT_FLAC:
						if (!flac::parse(asset_file, &entry, file, checksum.md5, source_checksum.md5))
						{
							continue;
						}
						break;
					case SND_ASSET_FORMAT_PCMS16:
						// I think I'm correctly parsing the file, 
						// but the game just doesn't seem to support any other audio format other than FLAC...
						// 
						//if (!pcm::parse(asset_file, &entry, file, checksum.md5, source_checksum.md5))
						//{
						//	continue;
						//}
						ZONETOOL_ERROR("Wav files are not supported! (%s)", asset_file.data());
						break;
					default:
						__debugbreak();
					}

					assets.push_back(alias->assetFileName);
					checksums.push_back(checksum);
					source_checksums.push_back(source_checksum);
					data_offset += entry.size + entry.seekTableSize + entry.hybridPcmSize;
					entries.push_back(entry);
					inserted_sound.insert(alias->assetId);
				}
				align(0x1000);
			}
			const auto asset_offset_end = data_offset;

			assert(file.size() == data_offset);

			const auto asset_section_size = asset_offset_end - asset_offset_start;
			header.assetSectionSize = static_cast<unsigned int>(asset_section_size);

			// write entries
			const auto entries_offset = data_offset;
			if (entries.size())
			{
				header.entryCount = static_cast<unsigned int>(entries.size());
				header.entryOffset = entries_offset;

				for (auto& entry : entries)
				{
					file.write(&entry);
					data_offset += sizeof(entry);
				}
				align(0x1000);
			}

			// write checksums
			const auto checksums_offset = data_offset;
			if (checksums.size())
			{
				header.checksumOffset = checksums_offset;

				for (auto& checksum : checksums)
				{
					file.write(&checksum.md5);
					data_offset += sizeof(checksum);
				}
				align(0x1000);
			}

			// write source checksums
			const auto source_checksums_offset = data_offset;
			if (source_checksums.size())
			{
				header.SourceChecksumOffset = source_checksums_offset;

				for (auto& checksum : source_checksums)
				{
					file.write(&checksum.md5);
					data_offset += sizeof(checksum);
				}
				align(0x1000);
			}

			// write names
			const auto names_offset = data_offset;
			if (assets.size())
			{
				header.AssetNameOffset = names_offset;
				char name_buffer[128]{};

				for (auto& asset : assets)
				{
					memset(name_buffer, 0, sizeof(name_buffer));
					memcpy(name_buffer, asset.data(), asset.size());
					file.write(name_buffer, sizeof(name_buffer));
					data_offset += sizeof(name_buffer);
				}
			}

			file.close();

			// fixup header

			file.open("rb+");
			header.fileSize = file.size();

			// calculate checksumChecksum
			memset(header.checksumChecksum, 0xCC, 16); // YOU THOUGHT!

			file.seek(0, SEEK_SET);
			file.write(&header);

			file.close();
		}

		namespace loaded
		{
			void dump(std::string name, std::string language_folder, std::string language_prefix)
			{
				if (!name.ends_with(".sabl")) name.append(".sabl");

				dump_internal(name, language_folder, language_prefix);
			}
		}

		namespace streamed
		{
			void dump(std::string name, std::string language_folder, std::string language_prefix)
			{
				if (!name.ends_with(".sabs")) name.append(".sabs");

				dump_internal(name, language_folder, language_prefix);
			}
		}

		void dump(const std::string& name, std::string language_folder = "", std::string language_prefix = "")
		{
			if (language_folder == "all")
			{
				language_folder.clear();
			}
			if (language_prefix == "all")
			{
				language_prefix.clear();
			}

			loaded::dump(name, language_folder, language_prefix);
			streamed::dump(name, language_folder, language_prefix);
		}

		void create(SndBank* bank)
		{
			if (!bank->aliasCount) return;

			create_internal(bank, false); // loaded
			create_internal(bank, true); // streamed
		}
	}

	SndBank* sound_bank::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "soundbank\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing soundbank \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<SndBank>();

		asset->name = mem->duplicate_string(get_json_value<std::string>(data["name"]));
		asset->zone = mem->duplicate_string(get_json_value<std::string>(data["zone"]));
		asset->gameLanguage = mem->duplicate_string(get_json_value<std::string>(data["gameLanguage"]));
		asset->soundLanguage = mem->duplicate_string(get_json_value<std::string>(data["soundLanguage"]));

		aliases::parse(asset, mem, data["aliases"]);

		// send_effect_params

		// adsr_settings

		// ambient_def

		// ambient_event

		// ambient_elem

		// filter

		// mix

		// adsr_zone::npc

		// adsr_zone::plr

		// occlude_def

		// reverb_def

		// timescale

		// zone

		// unk

		// full_occlusion_def

		// plr_breath_state

		// ducks are parsed from alias list

		// create sabl and sabs
		sound_asset_bank::create(asset);

		return asset;
	}

	void sound_bank::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).soundBank;
		}
	}

	void sound_bank::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void sound_bank::load_depending(zone_base* zone)
	{
	}

	std::string sound_bank::name()
	{
		return this->name_;
	}

	std::int32_t sound_bank::type()
	{
		return ASSET_TYPE_SOUND_BANK;
	}

	void sound_bank::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->zone)
		{
			dest->zone = buf->write_str(data->zone);
		}
		if (data->gameLanguage)
		{
			dest->gameLanguage = buf->write_str(data->gameLanguage);
		}
		if (data->soundLanguage)
		{
			dest->soundLanguage = buf->write_str(data->soundLanguage);
		}

		if (data->alias)
		{
			buf->align(7);
			dest->alias = buf->write(data->alias, data->aliasCount);
			for (auto i = 0u; i < data->aliasCount; i++)
			{
				if (data->alias[i].aliasName)
				{
					dest->alias[i].aliasName = buf->write_str(data->alias[i].aliasName);
				}
				if (data->alias[i].head)
				{
					buf->align(7);
					dest->alias[i].head = buf->write(data->alias[i].head, data->alias[i].count);
					for (auto j = 0; j < data->alias[i].count; j++)
					{
						if (data->alias[i].head[j].aliasName)
						{
							dest->alias[i].head[j].aliasName = buf->write_str(data->alias[i].head[j].aliasName);
						}
						if (data->alias[i].head[j].subtitle)
						{
							dest->alias[i].head[j].subtitle = buf->write_str(data->alias[i].head[j].subtitle);
						}
						if (data->alias[i].head[j].secondaryAliasName)
						{
							dest->alias[i].head[j].secondaryAliasName = buf->write_str(data->alias[i].head[j].secondaryAliasName);
						}
						if (data->alias[i].head[j].stopAliasName)
						{
							dest->alias[i].head[j].stopAliasName = buf->write_str(data->alias[i].head[j].stopAliasName);
						}
						if (data->alias[i].head[j].assetFileName)
						{
							dest->alias[i].head[j].assetFileName = buf->write_str(data->alias[i].head[j].assetFileName);
						}
					}
					buf->clear_pointer(&dest->alias[i].head);
				}
			}
			buf->clear_pointer(&dest->alias);
		}

		if (data->aliasIndex)
		{
			buf->align(7);
			buf->write(data->aliasIndex, data->aliasCount);
			buf->clear_pointer(&dest->aliasIndex);
		}

		if (data->sendEffects)
		{
			buf->align(3);
			buf->write(data->sendEffects, data->sendEffectCount);
			buf->clear_pointer(&dest->sendEffects);
		}

		if (data->soundTable.adsrSettings)
		{
			buf->align(7);
			dest->soundTable.adsrSettings = buf->write(data->soundTable.adsrSettings, data->soundTable.adsrCount);
			for (auto i = 0u; i < data->soundTable.adsrCount; i++)
			{
				if (data->soundTable.adsrSettings[i].name)
				{
					dest->soundTable.adsrSettings[i].name = buf->write_str(data->soundTable.adsrSettings[i].name);
				}
			}
			buf->clear_pointer(&dest->soundTable.adsrSettings);
		}

		if (data->soundTable.ambientDefs)
		{
			buf->align(7);
			dest->soundTable.ambientDefs = buf->write(data->soundTable.ambientDefs, data->soundTable.ambientDefCount);
			for (auto i = 0u; i < data->soundTable.ambientDefCount; i++)
			{
				if (data->soundTable.ambientDefs[i].name)
				{
					dest->soundTable.ambientDefs[i].name = buf->write_str(data->soundTable.ambientDefs[i].name);
				}
			}
			buf->clear_pointer(&dest->soundTable.ambientDefs);
		}

		if (data->soundTable.ambientEvents)
		{
			buf->align(3);
			buf->write(data->soundTable.ambientEvents, data->soundTable.ambientEventCount);
			buf->clear_pointer(&dest->soundTable.ambientEvents);
		}

		if (data->soundTable.ambientElements)
		{
			buf->align(7);
			dest->soundTable.ambientElements = buf->write(data->soundTable.ambientElements, data->soundTable.ambientElementsCount);
			for (auto i = 0u; i < data->soundTable.ambientElementsCount; i++)
			{
				if (data->soundTable.ambientElements[i].aliasName)
				{
					dest->soundTable.ambientElements[i].aliasName = buf->write_str(data->soundTable.ambientElements[i].aliasName);
				}
			}
			buf->clear_pointer(&dest->soundTable.ambientElements);
		}

		if (data->soundTable.filters)
		{
			buf->align(3);
			buf->write(data->soundTable.filters, data->soundTable.filterCount);
			buf->clear_pointer(&dest->soundTable.filters);
		}

		if (data->soundTable.mixes)
		{
			buf->align(3);
			buf->write(data->soundTable.mixes, data->soundTable.mixCount);
			buf->clear_pointer(&dest->soundTable.mixes);
		}

		if (data->soundTable.npcADSRZones)
		{
			buf->align(7);
			dest->soundTable.npcADSRZones = buf->write(data->soundTable.npcADSRZones, data->soundTable.npcADSRZoneCount);
			for (auto i = 0u; i < data->soundTable.npcADSRZoneCount; i++)
			{
				if (data->soundTable.npcADSRZones[i].name)
				{
					dest->soundTable.npcADSRZones[i].name = buf->write_str(data->soundTable.npcADSRZones[i].name);
				}
				if (data->soundTable.npcADSRZones[i].weaponName)
				{
					dest->soundTable.npcADSRZones[i].weaponName = buf->write_str(data->soundTable.npcADSRZones[i].weaponName);
				}
			}
			buf->clear_pointer(&dest->soundTable.npcADSRZones);
		}

		if (data->soundTable.occlusionFilters)
		{
			buf->align(3);
			buf->write(data->soundTable.occlusionFilters, data->soundTable.occlusionFilterCount);
			buf->clear_pointer(&dest->soundTable.occlusionFilters);
		}

		if (data->soundTable.playerADSRZones)
		{
			buf->align(7);
			dest->soundTable.playerADSRZones = buf->write(data->soundTable.playerADSRZones, data->soundTable.playerADSRZoneCount);
			for (auto i = 0u; i < data->soundTable.playerADSRZoneCount; i++)
			{
				if (data->soundTable.playerADSRZones[i].name)
				{
					dest->soundTable.playerADSRZones[i].name = buf->write_str(data->soundTable.playerADSRZones[i].name);
				}
				if (data->soundTable.playerADSRZones[i].weaponName)
				{
					dest->soundTable.playerADSRZones[i].weaponName = buf->write_str(data->soundTable.playerADSRZones[i].weaponName);
				}
			}
			buf->clear_pointer(&dest->soundTable.playerADSRZones);
		}

		if (data->soundTable.reverbs)
		{
			buf->align(3);
			buf->write(data->soundTable.reverbs, data->soundTable.reverbCount);
			buf->clear_pointer(&dest->soundTable.reverbs);
		}

		if (data->soundTable.timeScaleSettings)
		{
			buf->align(3);
			buf->write(data->soundTable.timeScaleSettings, data->soundTable.timeScaleSettingCount);
			buf->clear_pointer(&dest->soundTable.timeScaleSettings);
		}

		if (data->soundTable.zones)
		{
			buf->align(7);
			dest->soundTable.zones = buf->write(data->soundTable.zones, data->soundTable.zoneCount);
			for (auto i = 0u; i < data->soundTable.zoneCount; i++)
			{
				if (data->soundTable.zones[i].ambientStream)
				{
					dest->soundTable.zones[i].ambientStream = buf->write_str(data->soundTable.zones[i].ambientStream);
				}
				if (data->soundTable.zones[i].zoneName)
				{
					dest->soundTable.zones[i].zoneName = buf->write_str(data->soundTable.zones[i].zoneName);
				}
				if (data->soundTable.zones[i].stateName)
				{
					dest->soundTable.zones[i].stateName = buf->write_str(data->soundTable.zones[i].stateName);
				}
				if (data->soundTable.zones[i].ambientName)
				{
					dest->soundTable.zones[i].ambientName = buf->write_str(data->soundTable.zones[i].ambientName);
				}
				if (data->soundTable.zones[i].mixName)
				{
					dest->soundTable.zones[i].mixName = buf->write_str(data->soundTable.zones[i].mixName);
				}
				if (data->soundTable.zones[i].reverbName)
				{
					dest->soundTable.zones[i].reverbName = buf->write_str(data->soundTable.zones[i].reverbName);
				}
				if (data->soundTable.zones[i].filterName)
				{
					dest->soundTable.zones[i].filterName = buf->write_str(data->soundTable.zones[i].filterName);
				}
				if (data->soundTable.zones[i].occlusionName)
				{
					dest->soundTable.zones[i].occlusionName = buf->write_str(data->soundTable.zones[i].occlusionName);
				}
				if (data->soundTable.zones[i].plrAdsrName)
				{
					dest->soundTable.zones[i].plrAdsrName = buf->write_str(data->soundTable.zones[i].plrAdsrName);
				}
				if (data->soundTable.zones[i].npcAdsrName)
				{
					dest->soundTable.zones[i].npcAdsrName = buf->write_str(data->soundTable.zones[i].npcAdsrName);
				}
				if (data->soundTable.zones[i].weapReflName)
				{
					dest->soundTable.zones[i].weapReflName = buf->write_str(data->soundTable.zones[i].weapReflName);
				}
				if (data->soundTable.zones[i].fullOccName)
				{
					dest->soundTable.zones[i].fullOccName = buf->write_str(data->soundTable.zones[i].fullOccName);
				}
				if (data->soundTable.zones[i].playerBreathStateName)
				{
					dest->soundTable.zones[i].playerBreathStateName = buf->write_str(data->soundTable.zones[i].playerBreathStateName);
				}
			}
			buf->clear_pointer(&dest->soundTable.zones);
		}

		if (data->soundTable.unk)
		{
			buf->align(7);
			dest->soundTable.unk = buf->write(data->soundTable.unk, data->soundTable.unkCount);
			for (auto i = 0u; i < data->soundTable.unkCount; i++)
			{
				if (data->soundTable.unk[i].name)
				{
					dest->soundTable.unk[i].name = buf->write_str(data->soundTable.unk[i].name);
				}
			}
			buf->clear_pointer(&dest->soundTable.unk);
		}

		if (data->soundTable.fullOcclusionDefs)
		{
			buf->align(3);
			buf->write(data->soundTable.fullOcclusionDefs, data->soundTable.fullOcclusionDefCount);
			buf->clear_pointer(&dest->soundTable.fullOcclusionDefs);
		}

		if (data->soundTable.plrBreathStateDefs)
		{
			buf->align(7);
			dest->soundTable.plrBreathStateDefs = buf->write(data->soundTable.plrBreathStateDefs, data->soundTable.plrBreathStateDefCount);
			for (auto i = 0u; i < data->soundTable.plrBreathStateDefCount; i++)
			{
				if (data->soundTable.plrBreathStateDefs[i].inhaleAlias)
				{
					dest->soundTable.plrBreathStateDefs[i].inhaleAlias = buf->write_str(data->soundTable.plrBreathStateDefs[i].inhaleAlias);
				}
				if (data->soundTable.plrBreathStateDefs[i].exhaleAlias)
				{
					dest->soundTable.plrBreathStateDefs[i].exhaleAlias = buf->write_str(data->soundTable.plrBreathStateDefs[i].exhaleAlias);
				}
			}
			buf->clear_pointer(&dest->soundTable.plrBreathStateDefs);
		}

		if (data->ducks)
		{
			buf->align(7);
			dest->ducks = buf->write(data->ducks, data->duckCount);
			for (auto i = 0u; i < data->duckCount; i++)
			{
				if (data->ducks[i].attenuation)
				{
					buf->align(31);
					buf->write(data->ducks[i].attenuation, 128);
					buf->clear_pointer(&dest->ducks[i].attenuation);
				}
				if (data->ducks[i].lpf)
				{
					buf->align(31);
					buf->write(data->ducks[i].lpf, 128);
					buf->clear_pointer(&dest->ducks[i].lpf);
				}
			}
			buf->clear_pointer(&dest->ducks);
		}

		if (data->musicSets)
		{
			buf->align(7);
			dest->musicSets = buf->write(data->musicSets, data->musicSetCount);
			for (auto i = 0u; i < data->musicSetCount; i++)
			{
				if (data->musicSets[i].states)
				{
					buf->align(7);
					dest->musicSets[i].states = buf->write(data->musicSets[i].states, data->musicSets[i].stateCount);
					for (auto j = 0u; j < data->musicSets[i].stateCount; j++)
					{
						if (data->musicSets[i].states[j].loops)
						{
							buf->align(3);
							buf->write(data->musicSets[i].states[j].loops, data->musicSets[i].states[j].loopCount);
							buf->clear_pointer(&dest->musicSets[i].states[j].loops);
						}
					}
					buf->clear_pointer(&dest->musicSets[i].states);
				}
			}
			buf->clear_pointer(&dest->musicSets);
		}

		buf->pop_stream();
	}

	void sound_bank::dump(SndBank* asset)
	{
		auto file = filesystem::file("soundbank\\"s + asset->name + ".json"s);
		file.open("wb");

		ordered_json data;

		data["name"] = asset->name;
		data["zone"] = asset->zone;
		data["gameLanguage"] = asset->gameLanguage;
		data["soundLanguage"] = asset->soundLanguage;

		aliases::dump(asset);
		data["aliases"][0] = aliases::get_path(asset);

		send_effect_params::dump(asset);
		data["sendEffectParams"][0] = send_effect_params::get_path(asset);

		// sound table
		adsr_settings::dump(asset);
		data["ADSRSettings"][0] = adsr_settings::get_path(asset);

		ambient_def::dump(asset);
		data["AmbientDefs"][0] = ambient_def::get_path(asset);

		ambient_event::dump(asset);
		data["AmbientEvents"][0] = ambient_event::get_path(asset);

		ambient_elem::dump(asset);
		data["AmbientElements"][0] = ambient_elem::get_path(asset);

		filter::dump(asset);
		data["FilterDefs"][0] = filter::get_path(asset);

		mix::dump(asset);
		data["MixDefs"][0] = mix::get_path(asset);

		adsr_zone::npc::dump(asset);
		data["NPCAdsrZoneEntry"][0] = adsr_zone::npc::get_path(asset);

		adsr_zone::plr::dump(asset);
		data["PLRAdsrZoneEntry"][0] = adsr_zone::plr::get_path(asset);

		occlude_def::dump(asset);
		data["OccludeDefs"][0] = occlude_def::get_path(asset);

		reverb_def::dump(asset);
		data["ReverbDefs"][0] = reverb_def::get_path(asset);

		timescale::dump(asset);
		data["TimescaleEntrys"][0] = timescale::get_path(asset);

		zone::dump(asset);
		data["ZoneDefs"][0] = zone::get_path(asset);

		unk::dump(asset);
		data["Unks"][0] = unk::get_path(asset);

		full_occlusion_def::dump(asset);
		data["FullOcclusionDefs"][0] = full_occlusion_def::get_path(asset);

		plr_breath_state::dump(asset);
		data["PlayerBreathStateDefs"][0] = plr_breath_state::get_path(asset);

		ducks::dump(asset);
		// ducks are parsed from alias list

		// music

		sound_asset_bank::dump(asset->zone, asset->soundLanguage, asset->gameLanguage);

		file.write(data.dump(4));
		file.close();
	}
}