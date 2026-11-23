#include <std_include.hpp>

#include "../include.hpp"
#include "sound.hpp"

#include <formats/audio.hpp>

namespace zonetool::s2::converter::h1::sound
{
	namespace
	{
		std::unordered_set<std::string> dumped_sounds;
		std::unordered_set<std::string> dumped_audio;
		std::unordered_set<std::string> dumped_curves;

		const std::unordered_map<std::string_view, std::string_view> s2_dsp_bus_renames
		{
			{"unocclu_no_timescale", "notimescale"},
		};

		template <typename T>
		T* make_stub(const char* name, utils::memory::allocator& allocator)
		{
			const auto stub = allocator.allocate<T>();
			stub->name = name;
			return stub;
		}

		const char* alias_name(const snd_alias_t& alias)
		{
			return alias.aliasName ? alias.aliasName : "null";
		}

		short convert_volume_mod(const snd_alias_t& alias)
		{
			if (const auto* name = zonetool::s2::sound::get_vol_mod_name(alias.volModIndex))
			{
				if (const auto index = zonetool::h1::sound::get_vol_mod_index_from_name(name); index >= 0)
				{
					return index;
				}
			}

			ZONETOOL_WARNING("%s: volume mod %d has no H1 group, the alias uses \"default\"", alias_name(alias), alias.volModIndex);
			return zonetool::h1::SND_VOLMOD_DEFAULT;
		}

		unsigned char convert_dsp_bus(const snd_alias_t& alias)
		{
			if (const auto* s2_name = zonetool::s2::sound::get_dsp_bus_name(alias.dspBusIndex))
			{
				std::string_view name = s2_name;
				if (const auto rename = s2_dsp_bus_renames.find(name); rename != s2_dsp_bus_renames.end())
				{
					name = rename->second;
				}

				if (const auto index = zonetool::h1::sound::get_dsp_bus_index_from_name(std::string(name).data()); index >= 0)
				{
					return static_cast<unsigned char>(index);
				}
			}

			ZONETOOL_WARNING("%s: dsp bus %u has no H1 bus, the alias uses \"default\"", alias_name(alias), alias.dspBusIndex);
			return zonetool::h1::SND_DSPBUS_DEFAULT;
		}

		int convert_flags(const int s2_flags)
		{
			const auto source = static_cast<std::uint32_t>(s2_flags);

			zonetool::h1::SoundAliasFlags flags{};
			flags.intValue = (source & 0x1BF) | ((source & 0x7E00) << 1);
			flags.packed.useContextList = 0;
			flags.packed.type = zonetool::h1::SAT_LOADED;
			return static_cast<int>(flags.intValue);
		}

		int convert_speaker(const int s2_speaker)
		{
			return s2_speaker < 4 ? s2_speaker : s2_speaker - 2;
		}

		zonetool::h1::SpeakerMap* convert_speaker_map(const SpeakerMap* asset, utils::memory::allocator& allocator)
		{
			if (!asset)
			{
				return nullptr;
			}

			auto* map = allocator.allocate<zonetool::h1::SpeakerMap>();
			map->name = asset->name ? asset->name : "";
			map->isDefault = !*map->name;

			for (auto input = 0; input < 2; input++)
			{
				auto& stereo = map->channelMaps[input][0];
				auto& surround = map->channelMaps[input][1];
				stereo.speakerCount = 2;
				surround.speakerCount = 6;
				for (auto speaker = 0; speaker < surround.speakerCount; speaker++)
				{
					surround.speakers[speaker] = {static_cast<char>(speaker), static_cast<char>(input + 1)};
				}

				for (const auto& source : asset->channelMaps[input])
				{
					if (source.speaker == 4 || source.speaker == 5 || source.speaker > 7)
					{
						continue;
					}

					std::ranges::copy(source.levels, surround.speakers[convert_speaker(source.speaker)].levels);
				}

				std::copy_n(surround.speakers, stereo.speakerCount, stereo.speakers);
			}

			return map;
		}

		zonetool::h1::SndCurve* convert_curve(const SndCurve* curve, utils::memory::allocator& allocator)
		{
			if (!curve || !curve->name || !std::strcmp(curve->name, "$default"))
			{
				return make_stub<zonetool::h1::SndCurve>("$default", allocator);
			}

			const auto name = std::format("{}_s2", curve->name);
			if (dumped_curves.insert(name).second)
			{
				const auto* source = curve->knotCount ? curve : db_find_x_asset_header_safe(ASSET_TYPE_SOUND_CURVE, curve->name).sndCurve;
				if (!source)
				{
					ZONETOOL_WARNING("sound curve \"%s\" is not loaded, the alias uses \"$default\"", curve->name);
					return make_stub<zonetool::h1::SndCurve>("$default", allocator);
				}

				zonetool::h1::SndCurve result{};
				result.name = name.data();
				result.knotCount = source->knotCount;
				std::memcpy(result.knots, source->knots, sizeof(result.knots));
				zonetool::h1::sound_curve::dump(&result);
				add_csv_row(zonetool::h1::type_to_string(zonetool::h1::ASSET_TYPE_SOUND_CURVE), name);
			}

			return make_stub<zonetool::h1::SndCurve>(allocator.duplicate_string(name), allocator);
		}

		std::string audio_name(const LoadedSoundStreamInfo& info, const snd_alias_t& alias, const int index)
		{
			if (info.fileName && *info.fileName)
			{
				return info.name && *info.name ? std::format("{}\\{}", info.name, info.fileName) : info.fileName;
			}

			return std::format("streamed\\{}_{}", alias_name(alias), index);
		}

		std::optional<std::string> read_loaded_audio(const LoadedSound* asset)
		{
			if (asset->info.streamKey.hash)
			{
				return zonetool::s2::loaded_sound::read_stream(asset->info.streamKey);
			}

			if (!asset->data || !asset->dataSize)
			{
				return {};
			}

			return std::string(asset->data, asset->dataSize);
		}

		bool write_audio(const std::string& name, const std::string& data, const SndAudioFormat& format)
		{
			if (!dumped_audio.insert(name).second)
			{
				return true;
			}

			if (format.format == SND_FORMAT_FLAC || audio::is_flac(data))
			{
				write_file("loaded_sound\\" + name + ".flac", data);
				return true;
			}

			if (format.format == SND_FORMAT_PCM)
			{
				const audio::pcm_format pcm
				{
					.channel_count = format.channelCount,
					.sample_rate = format.sampleRate,
					.block_align = static_cast<std::uint16_t>(format.blockSize),
					.bits_per_sample = format.bitsPerSample,
				};
				write_file("loaded_sound\\" + name + ".wav", audio::write_wav(pcm, data));
				return true;
			}

			ZONETOOL_WARNING("loaded sound \"%s\": the audio format %u is not known", name.data(), format.format);
			dumped_audio.erase(name);
			return false;
		}

		std::optional<std::string> convert_audio(const snd_alias_t& alias, const int index)
		{
			const auto* file = alias.soundFile;
			switch (file->type)
			{
			case SAT_LOADED:
			{
				const auto* loaded = file->u.loadSnd;
				if (!loaded || !loaded->name)
				{
					return {};
				}

				if (dumped_audio.contains(loaded->name))
				{
					return loaded->name;
				}

				const auto data = read_loaded_audio(loaded);
				return data && write_audio(loaded->name, *data, loaded->audioFormat) ? std::optional<std::string>(loaded->name) : std::nullopt;
			}
			case SAT_STREAMED:
			{
				const auto& stream = file->u.streamSnd;
				const auto name = audio_name(stream.info, alias, index);
				if (dumped_audio.contains(name))
				{
					return name;
				}

				const auto data = zonetool::s2::loaded_sound::read_stream(stream.info.streamKey);
				return data && write_audio(name, *data, stream.audioFormat) ? std::optional<std::string>(name) : std::nullopt;
			}
			case SAT_PRIMED:
			{
				const auto& primed = file->u.primedSnd;
				const auto name = audio_name(primed.streamedPart, alias, index);
				if (dumped_audio.contains(name))
				{
					return name;
				}

				const auto data = zonetool::s2::loaded_sound::read_stream(primed.streamedPart.streamKey);
				const auto format = primed.loadedPart ? primed.loadedPart->audioFormat : SndAudioFormat{.format = SND_FORMAT_FLAC};
				return data && write_audio(name, *data, format) ? std::optional<std::string>(name) : std::nullopt;
			}
			default:
				return {};
			}
		}

		zonetool::h1::SoundFile* convert_sound_file(const snd_alias_t& alias, const int index, utils::memory::allocator& allocator)
		{
			if (!alias.soundFile)
			{
				return nullptr;
			}

			auto* file = allocator.allocate<zonetool::h1::SoundFile>();
			file->type = zonetool::h1::SAT_LOADED;
			file->exists = alias.soundFile->exists;

			const auto name = alias.soundFile->exists ? convert_audio(alias, index) : std::nullopt;
			if (!name)
			{
				if (alias.soundFile->exists)
				{
					ZONETOOL_WARNING("%s: the audio data is missing", alias_name(alias));
				}

				file->exists = false;
				return file;
			}

			file->u.loadSnd = make_stub<zonetool::h1::LoadedSound>(allocator.duplicate_string(*name), allocator);
			return file;
		}

		void convert_alias(const snd_alias_t& alias, const int index, zonetool::h1::snd_alias_t& result, utils::memory::allocator& allocator)
		{
			result.aliasName = alias.aliasName;
			result.subtitle = alias.subtitle;
			result.secondaryAliasName = alias.secondaryAliasName;
			result.chainAliasName = alias.chainAliasName;
			result.soundFile = convert_sound_file(alias, index, allocator);
			result.squelchName = alias.squelchName;
			result.polyCount = alias.polyCount;
			result.polyGlobalCount = 174;
			result.dspBusIndex = convert_dsp_bus(alias);
			result.priority = alias.priority;
			result.playCount = alias.playCount;
			result.polyClass = alias.polyClass;
			result.volMin = alias.volMin;
			result.volMax = alias.volMax;
			result.volModIndex = convert_volume_mod(alias);
			result.pitchMin = alias.pitchMin;
			result.pitchMax = alias.pitchMax;
			result.distMin = alias.distMin;
			result.distMax = alias.distMax;
			result.velocityMin = alias.velocityMin;
			result.flags = convert_flags(alias.flags);
			result.masterPriority = alias.masterPriority;
			result.masterPercentage = alias.masterPercentage;
			result.slavePercentage = alias.slavePercentage;
			result.playbackPercentage = static_cast<unsigned char>(alias.playbackPercentage);
			result.probability = alias.probability;
			result.variationType = static_cast<unsigned char>(alias.variationType);
			result.sequence = alias.sequence;
			result.lfePercentage = alias.lfePercentage;
			result.centerPercentage = alias.centerPercentage;
			result.startDelay = alias.startDelay;
			result.volumeFalloffCurve = convert_curve(alias.volumeFalloffCurve, allocator);
			result.lpfCurve = make_stub<zonetool::h1::SndCurve>("$default", allocator);
			result.reverbSendCurve = make_stub<zonetool::h1::SndCurve>("$default", allocator);
			result.speakerMap = convert_speaker_map(alias.speakerMap, allocator);
			result.wetMixOverride = alias.wetMixOverride;
			result.focusPercentage = alias.focusPercentage;
			result.smartpanDistance2d = alias.smartpanDistance2d;
			result.smartpanDistance3d = alias.smartpanDistance3d;
			result.smartpanAttenuation3d = alias.smartpanAttenuation3d;
			result.minSmartpan2dContribution = alias.minSmartpan2dContribution;
			result.stereo3DAngle = alias.stereo3DAngle;
			result.stereo3DStart = alias.stereo3DStart;
			result.stereo3DEnd = alias.stereo3DEnd;
			result.threshold = alias.threshold;
			result.lockedLoopTime = alias.lockedLoopTime;
		}

		bool is_context_variant(const snd_alias_t& alias)
		{
			return alias.sndContext && alias.sndContext->name && *alias.sndContext->name;
		}

		void dump(const snd_alias_list_t* asset)
		{
			const auto keep_all = std::ranges::all_of(asset->head, asset->head + asset->count, is_context_variant);

			utils::memory::allocator allocator;
			zonetool::h1::snd_alias_list_t list{};
			list.name = asset->name;
			list.head = allocator.allocate_array<zonetool::h1::snd_alias_t>(asset->count);
			for (auto i = 0; i < asset->count; i++)
			{
				if (keep_all || !is_context_variant(asset->head[i]))
				{
					convert_alias(asset->head[i], i, list.head[list.count++], allocator);
				}
			}

			zonetool::h1::sound::dump(&list);
			add_csv_row(zonetool::h1::type_to_string(zonetool::h1::ASSET_TYPE_SOUND), asset->name);
		}
	}

	bool dump_used(const std::string& name)
	{
		if (name.empty() || dumped_sounds.contains(name))
		{
			return !name.empty();
		}

		const auto* asset = db_find_x_asset_header_safe(ASSET_TYPE_SOUND, name).sound;
		if (!asset || !asset->head)
		{
			ZONETOOL_WARNING("sound \"%s\" is not loaded", name.data());
			return false;
		}

		dumped_sounds.insert(name);
		dump(asset);
		for (auto i = 0; i < asset->count; i++)
		{
			for (const auto* linked : {asset->head[i].secondaryAliasName, asset->head[i].chainAliasName})
			{
				if (linked && *linked)
				{
					dump_used(linked);
				}
			}
		}

		return true;
	}
}
