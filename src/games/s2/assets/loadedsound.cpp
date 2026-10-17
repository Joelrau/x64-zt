#include <std_include.hpp>
#include "s2.hpp"
#include "json_fields.hpp"

#include <formats/audio.hpp>
#include <io/game_files.hpp>

namespace zonetool::s2
{
	namespace
	{
		constexpr std::string_view sound_toc_magic = "S2ffi200";
		constexpr std::string_view sound_file_prefix = "soundfile";
		constexpr std::uint16_t flac_block_size = 1024;

		struct sound_stream_location
		{
			std::filesystem::path pak;
			std::uint64_t offset;
			std::uint32_t size;
		};

		void read_sound_toc(const std::string& name, std::unordered_map<std::uint64_t, sound_stream_location>& locations)
		{
			const auto path = game_files::get().path(name);
			std::string data;
			if (!game_files::read(path, data) || data.size() < sizeof(SoundFileTocHeader))
			{
				return;
			}

			const auto* header = reinterpret_cast<const SoundFileTocHeader*>(data.data());
			if (std::string_view(header->magic, sizeof(header->magic)) != sound_toc_magic
				|| sizeof(SoundFileTocHeader) + header->entryCount * sizeof(SoundFileTocEntry) > data.size())
			{
				ZONETOOL_WARNING("Sound toc \"%s\" is not valid", name.data());
				return;
			}

			const auto file_name = path.filename().string();
			const auto pak_prefix = file_name.substr(0, file_name.find(sound_file_prefix));

			const auto* entries = reinterpret_cast<const SoundFileTocEntry*>(data.data() + sizeof(SoundFileTocHeader));
			for (auto i = 0u; i < header->entryCount; i++)
			{
				const auto pak_name = std::format("{}{}{}.pak", pak_prefix, sound_file_prefix, entries[i].fileIndex);
				locations.try_emplace(entries[i].hash[0], path.parent_path() / pak_name, entries[i].offset, entries[i].size);
			}
		}

		const std::unordered_map<std::uint64_t, sound_stream_location>& sound_stream_locations()
		{
			static const auto locations = []
			{
				std::unordered_map<std::uint64_t, sound_stream_location> result;
				for (const auto& name : game_files::get().list(".toc"))
				{
					if (std::filesystem::path(name).filename().string().contains(sound_file_prefix))
					{
						read_sound_toc(name, result);
					}
				}

				return result;
			}();

			return locations;
		}

		std::optional<std::string> read_sound_stream(const XStreamFileKey& key)
		{
			const auto& locations = sound_stream_locations();
			const auto location = locations.find(key.hash);
			if (location == locations.end())
			{
				return {};
			}

			std::string data(location->second.size, '\0');
			if (!game_files::read(location->second.pak, location->second.offset, data.size(), data.data()))
			{
				return {};
			}

			return data;
		}

		std::string json_path(const std::string& name)
		{
			return "loaded_sound\\" + name + ".json";
		}

		std::string audio_path(const std::string& name, const SndAudioFormatType format)
		{
			return "loaded_sound\\" + name + (format == SND_FORMAT_FLAC ? ".flac" : ".wav");
		}

		std::optional<std::string> read_file(const std::string& path)
		{
			auto file = filesystem::file(path);
			if (!file.exists())
			{
				return {};
			}

			file.open("rb");
			const auto bytes = file.read_bytes(file.size());
			file.close();

			return std::string(bytes.begin(), bytes.end());
		}

		void write_file(const std::string& path, const std::string_view data)
		{
			auto file = filesystem::file(path);
			file.open("wb");
			file.write(data.data(), data.size(), 1);
			file.close();
		}

		void set_audio_data(LoadedSound* asset, const std::string_view data, zone_memory* mem)
		{
			asset->data = mem->allocate<char>(data.size());
			std::memcpy(asset->data, data.data(), data.size());
			asset->dataSize = static_cast<int>(data.size());
			asset->audioFormat.dataLength = static_cast<unsigned int>(data.size());
		}

		bool parse_flac(const std::string& name, LoadedSound* asset, zone_memory* mem)
		{
			const auto file = read_file(audio_path(name, SND_FORMAT_FLAC));
			if (!file)
			{
				return false;
			}

			const auto data = audio::add_flac_frame_size_block(*file);
			const auto info = data ? audio::read_flac_stream_info(*data) : std::nullopt;
			if (!info)
			{
				ZONETOOL_FATAL("%s: the flac file is not valid", name.data());
			}

			if (info->min_block_size != flac_block_size || info->max_block_size != flac_block_size)
			{
				ZONETOOL_WARNING("%s: the flac block size must be %u (min %u, max %u)",
					name.data(), flac_block_size, info->min_block_size, info->max_block_size);
			}

			set_audio_data(asset, *data, mem);
			asset->audioFormat.dataLength = static_cast<unsigned int>(info->stream_size);
			asset->audioFormat.sampleCount = static_cast<unsigned int>(info->sample_count);
			asset->audioFormat.sampleRate = static_cast<unsigned short>(info->sample_rate);
			asset->audioFormat.channelCount = info->channel_count;
			asset->audioFormat.bitsPerSample = info->bits_per_sample;
			asset->audioFormat.blockSize = info->max_block_size;
			asset->audioFormat.format = SND_FORMAT_FLAC;
			return true;
		}

		bool parse_wav(const std::string& name, LoadedSound* asset, zone_memory* mem)
		{
			const auto file = read_file(audio_path(name, SND_FORMAT_PCM));
			if (!file)
			{
				return false;
			}

			const auto wav = audio::read_wav(*file);
			if (!wav || !wav->format.block_align)
			{
				ZONETOOL_FATAL("%s: the wav file is not valid pcm audio", name.data());
			}

			set_audio_data(asset, wav->samples, mem);
			asset->audioFormat.sampleCount = static_cast<unsigned int>(wav->samples.size() / wav->format.block_align);
			asset->audioFormat.sampleRate = static_cast<unsigned short>(wav->format.sample_rate);
			asset->audioFormat.channelCount = static_cast<unsigned char>(wav->format.channel_count);
			asset->audioFormat.bitsPerSample = static_cast<unsigned char>(wav->format.bits_per_sample);
			asset->audioFormat.blockSize = wav->format.block_align;
			asset->audioFormat.format = SND_FORMAT_PCM;
			return true;
		}

		bool parse_audio(const std::string& name, LoadedSound* asset, zone_memory* mem)
		{
			return parse_flac(name, asset, mem) || parse_wav(name, asset, mem);
		}

		LoadedSound* parse_bin(const std::string& name, zone_memory* mem)
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_LOADED_SOUND, name.data())))
			{
				return nullptr;
			}

			auto* asset = read.read_single<LoadedSound>();
			asset->name = read.read_string();
			asset->info.name = read.read_string();
			asset->info.fileName = read.read_string();
			asset->data = read.read_array<char>();
			asset->seekTable = read.read_array<unsigned int>();
			read.close();

			return asset;
		}

		template <typename J>
		void visit_loaded_sound_json(const J& j, LoadedSound& value)
		{
			j.string("name", value.name);
			j.object("info", value.info, loaded_sound::visit_stream_info_json<J>);
			j.object("audioFormat", value.audioFormat, loaded_sound::visit_audio_format_json<J>);
			j.value("dataSize", value.dataSize);
			j.array_list("seekTable", value.seekTable, value.seekTableCount);
		}
	}

	LoadedSound* loaded_sound::parse(const std::string& name, zone_memory* mem)
	{
		if (const auto file = read_file(json_path(name)))
		{
			ZONETOOL_INFO("Parsing loadedsound \"%s\"...", name.data());

			auto* asset = mem->allocate<LoadedSound>();
			visit_loaded_sound_json(json_field_parser(json::parse(*file), mem), *asset);

			if (asset->info.streamKey.hash)
			{
				asset->data = mem->allocate<char>(asset->dataSize); // the game streams the audio from the soundfile pak, the zone holds zeros
			}
			else if (!parse_audio(name, asset, mem) && asset->dataSize)
			{
				ZONETOOL_FATAL("%s: the audio file is missing", name.data());
			}

			return asset;
		}

		auto* asset = mem->allocate<LoadedSound>();
		if (parse_audio(name, asset, mem))
		{
			ZONETOOL_INFO("Parsing loadedsound \"%s\"...", name.data());
			asset->name = mem->duplicate_string(name);
			return asset;
		}

		if (auto* bin = parse_bin(name, mem))
		{
			ZONETOOL_INFO("Parsing loadedsound \"%s\"...", name.data());
			return bin;
		}

		return nullptr;
	}

	void loaded_sound::dump(LoadedSound* asset)
	{
		ordered_json fields;
		visit_loaded_sound_json(json_field_dumper(fields), *asset);
		write_file(json_path(asset->name), fields.dump(4));

		std::optional<std::string> data;
		if (asset->info.streamKey.hash)
		{
			data = read_sound_stream(asset->info.streamKey);
			if (!data)
			{
				ZONETOOL_WARNING("%s: the soundfile stream %016llX is missing", asset->name, asset->info.streamKey.hash);
				return;
			}
		}
		else if (asset->data && asset->dataSize)
		{
			data.emplace(asset->data, asset->dataSize);
		}
		else
		{
			return;
		}

		if (asset->audioFormat.format == SND_FORMAT_FLAC)
		{
			write_file(audio_path(asset->name, SND_FORMAT_FLAC), *data);
		}
		else if (asset->audioFormat.format == SND_FORMAT_PCM)
		{
			const audio::pcm_format format
			{
				.channel_count = asset->audioFormat.channelCount,
				.sample_rate = asset->audioFormat.sampleRate,
				.block_align = asset->audioFormat.blockSize,
				.bits_per_sample = asset->audioFormat.bitsPerSample,
			};
			write_file(audio_path(asset->name, SND_FORMAT_PCM), audio::write_wav(format, *data));
		}
		else
		{
			ZONETOOL_WARNING("%s: the audio format %u is not known", asset->name, asset->audioFormat.format);
		}
	}
}
