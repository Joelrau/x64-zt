#pragma once

namespace zonetool::audio
{
	struct flac_stream_info
	{
		std::uint16_t min_block_size;
		std::uint16_t max_block_size;
		std::uint32_t sample_rate;
		std::uint8_t channel_count;
		std::uint8_t bits_per_sample;
		std::uint64_t sample_count;
		std::uint64_t stream_size;
	};

	struct pcm_format
	{
		std::uint16_t channel_count;
		std::uint32_t sample_rate;
		std::uint16_t block_align;
		std::uint16_t bits_per_sample;
	};

	struct wav_file
	{
		pcm_format format;
		std::string samples;
	};

	bool is_flac(std::string_view data);
	std::optional<flac_stream_info> read_flac_stream_info(std::string_view data);
	std::optional<std::string> add_flac_frame_size_block(std::string_view data);

	std::string write_wav(const pcm_format& format, std::string_view samples);
	std::optional<wav_file> read_wav(std::string_view data);
}
