#include <std_include.hpp>
#include "audio.hpp"

namespace zonetool::audio
{
	namespace
	{
		constexpr std::string_view flac_signature = "fLaC";
		constexpr std::string_view frame_size_application_id = "fsiz";

		constexpr std::uint8_t flac_last_block_flag = 0x80;
		constexpr std::uint8_t flac_streaminfo_block = 0;
		constexpr std::uint8_t flac_application_block = 2;
		constexpr std::uint8_t flac_seektable_block = 3;
		constexpr std::size_t flac_streaminfo_size = 34;

		constexpr std::uint16_t wav_format_pcm = 1;

		struct flac_block
		{
			std::uint8_t type;
			std::string_view data;
		};

		struct flac_layout
		{
			std::vector<flac_block> blocks;
			std::size_t frames_offset;
		};

		std::optional<flac_layout> read_flac_layout(const std::string_view data)
		{
			if (!is_flac(data))
			{
				return {};
			}

			flac_layout layout{};
			auto offset = flac_signature.size();
			for (auto last = false; !last;)
			{
				if (offset + 4 > data.size())
				{
					return {};
				}

				const auto* header = reinterpret_cast<const std::uint8_t*>(data.data() + offset);
				const std::size_t length = (header[1] << 16) | (header[2] << 8) | header[3];
				last = header[0] & flac_last_block_flag;
				offset += 4;

				if (offset + length > data.size())
				{
					return {};
				}

				layout.blocks.emplace_back(static_cast<std::uint8_t>(header[0] & ~flac_last_block_flag), data.substr(offset, length));
				offset += length;
			}

			layout.frames_offset = offset;
			return layout;
		}

		bool is_frame_size_block(const flac_block& block)
		{
			return block.type == flac_application_block && block.data.size() >= 8 && block.data.starts_with(frame_size_application_id);
		}

		std::optional<std::uint32_t> find_frame_size(const flac_layout& layout)
		{
			const auto block = std::ranges::find_if(layout.blocks, is_frame_size_block);
			if (block == layout.blocks.end())
			{
				return {};
			}

			std::uint32_t frame_size{};
			std::memcpy(&frame_size, block->data.data() + frame_size_application_id.size(), sizeof(frame_size));
			return frame_size;
		}

		void append_flac_block(std::string& out, const std::uint8_t type, const std::string_view data, const bool last)
		{
			out.push_back(static_cast<char>(type | (last ? flac_last_block_flag : 0)));
			out.push_back(static_cast<char>(data.size() >> 16));
			out.push_back(static_cast<char>(data.size() >> 8));
			out.push_back(static_cast<char>(data.size()));
			out.append(data);
		}

		template <typename T>
		void append_value(std::string& out, const T value)
		{
			out.append(reinterpret_cast<const char*>(&value), sizeof(value));
		}

		template <typename T>
		T read_value(const std::string_view data, const std::size_t offset)
		{
			T value{};
			std::memcpy(&value, data.data() + offset, sizeof(value));
			return value;
		}
	}

	bool is_flac(const std::string_view data)
	{
		return data.starts_with(flac_signature);
	}

	std::optional<flac_stream_info> read_flac_stream_info(const std::string_view data)
	{
		const auto layout = read_flac_layout(data);
		if (!layout || layout->blocks.empty() || layout->blocks[0].type != flac_streaminfo_block
			|| layout->blocks[0].data.size() < flac_streaminfo_size)
		{
			return {};
		}

		const auto* block = reinterpret_cast<const std::uint8_t*>(layout->blocks[0].data.data());
		std::uint64_t bits{}; // sample rate (20), channels - 1 (3), bits per sample - 1 (5), sample count (36)
		for (auto i = 10; i < 18; i++)
		{
			bits = (bits << 8) | block[i];
		}

		return flac_stream_info
		{
			.min_block_size = static_cast<std::uint16_t>((block[0] << 8) | block[1]),
			.max_block_size = static_cast<std::uint16_t>((block[2] << 8) | block[3]),
			.sample_rate = static_cast<std::uint32_t>(bits >> 44),
			.channel_count = static_cast<std::uint8_t>(((bits >> 41) & 0x7) + 1),
			.bits_per_sample = static_cast<std::uint8_t>(((bits >> 36) & 0x1F) + 1),
			.sample_count = bits & 0xFFFFFFFFF,
			.stream_size = layout->frames_offset + find_frame_size(*layout).value_or(data.size() - layout->frames_offset),
		};
	}

	std::optional<std::string> add_flac_frame_size_block(const std::string_view data)
	{
		const auto layout = read_flac_layout(data);
		if (!layout)
		{
			return {};
		}

		if (find_frame_size(*layout))
		{
			return std::string(data);
		}

		const auto frames = data.substr(layout->frames_offset);
		std::string_view seektable{};

		std::string result{flac_signature};
		for (const auto& block : layout->blocks)
		{
			if (block.type == flac_seektable_block)
			{
				seektable = block.data;
			}
			else if (!is_frame_size_block(block))
			{
				append_flac_block(result, block.type, block.data, false);
			}
		}

		std::string frame_size{frame_size_application_id};
		append_value(frame_size, static_cast<std::uint32_t>(frames.size()));
		append_flac_block(result, flac_application_block, frame_size, false);
		append_flac_block(result, flac_seektable_block, seektable, true);

		result.append(frames);
		return result;
	}

	std::string write_wav(const pcm_format& format, const std::string_view samples)
	{
		constexpr std::uint32_t fmt_chunk_size = 16;

		std::string result;
		result.append("RIFF");
		append_value(result, static_cast<std::uint32_t>(4 + (8 + fmt_chunk_size) + (8 + samples.size())));
		result.append("WAVE");

		result.append("fmt ");
		append_value(result, fmt_chunk_size);
		append_value(result, wav_format_pcm);
		append_value(result, format.channel_count);
		append_value(result, format.sample_rate);
		append_value(result, static_cast<std::uint32_t>(format.sample_rate * format.block_align));
		append_value(result, format.block_align);
		append_value(result, format.bits_per_sample);

		result.append("data");
		append_value(result, static_cast<std::uint32_t>(samples.size()));
		result.append(samples);
		return result;
	}

	std::optional<wav_file> read_wav(const std::string_view data)
	{
		if (data.size() < 12 || !data.starts_with("RIFF") || data.substr(8, 4) != "WAVE")
		{
			return {};
		}

		std::optional<pcm_format> format;
		for (std::size_t offset = 12; offset + 8 <= data.size();)
		{
			const auto id = data.substr(offset, 4);
			const auto size = read_value<std::uint32_t>(data, offset + 4);
			offset += 8;

			if (offset + size > data.size())
			{
				return {};
			}

			if (id == "fmt " && size >= 16)
			{
				if (read_value<std::uint16_t>(data, offset) != wav_format_pcm)
				{
					return {};
				}

				format = pcm_format
				{
					.channel_count = read_value<std::uint16_t>(data, offset + 2),
					.sample_rate = read_value<std::uint32_t>(data, offset + 4),
					.block_align = read_value<std::uint16_t>(data, offset + 12),
					.bits_per_sample = read_value<std::uint16_t>(data, offset + 14),
				};
			}
			else if (id == "data" && format)
			{
				return wav_file{*format, std::string(data.substr(offset, size))};
			}

			offset += size + (size & 1);
		}

		return {};
	}
}
