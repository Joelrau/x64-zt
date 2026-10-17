#include <std_include.hpp>

#include "../include.hpp"
#include "loadedsound.hpp"

#include <formats/audio.hpp>

namespace zonetool::h1::converter::s2::loaded_sound
{
	namespace
	{
		std::optional<std::string> read_audio(const LoadedSound* asset)
		{
			if (asset->filename.fileIndex)
			{
				return zonetool::h1::loaded_sound::read_stream_file(asset->filename);
			}

			if (!asset->info.data || !asset->info.loadedSize)
			{
				return {};
			}

			const std::string_view data(asset->info.data, asset->info.loadedSize);
			if (audio::is_flac(data))
			{
				return std::string(data);
			}

			const audio::pcm_format format
			{
				.channel_count = asset->info.channels,
				.sample_rate = asset->info.sampleRate,
				.block_align = asset->info.blockAlign,
				.bits_per_sample = asset->info.numBits,
			};
			return audio::write_wav(format, data);
		}
	}

	void write_audio(const std::string& name, const std::string_view data)
	{
		auto file = filesystem::file("loaded_sound\\" + name + (audio::is_flac(data) ? ".flac" : ".wav"));
		file.open("wb");
		file.write(data.data(), data.size(), 1);
		file.close();

		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_LOADED_SOUND), name);
	}

	void dump(LoadedSound* asset)
	{
		const auto data = read_audio(asset);
		if (!data)
		{
			ZONETOOL_WARNING("%s: the audio data is missing", asset->name);
			return;
		}

		write_audio(asset->name, *data);
	}
}
