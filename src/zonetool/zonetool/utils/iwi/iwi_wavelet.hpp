#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

#include "iwi_wavelet_tables.hpp"

// decoder for the iw engine wavelet iwi codec (IMG_FORMAT_WAVELET_*), ported from
// the Image_DecodeWavelet implementation in LinkerMod's cod2rad
// (https://github.com/Nukem9/LinkerMod, components/cod2rad/r_image_wavelet.cpp).
//
// bitstream format: mip levels are decoded from the smallest up, each predicted
// from the previous. levels where either dimension is 1 are stored raw (channels
// bytes per pixel). at the first level with both dimensions > 1 a 16-bit lsb-first
// bit reservoir is seeded from the next 2 bytes. each coded level starts with one
// flag bit; if set, a per-pixel delta pass (alpha codec, every channel) corrects
// the previous level before it is used as the predictor. then, for each 2x2 output
// quad, per channel: one parity (rounding) bit plus three huffman-coded subband
// coefficients. the first channel uses the blue codec; red/green coefficients are
// deltas from the blue coefficients (redgreen codec); the last channel (alpha, or
// the only channel of single-channel images) uses the alpha codec. an inverse
// lifting step reconstructs the 4 pixels from twice the predictor pixel, the 3
// coefficients and the parity bit. huffman codes decode through a 12-bit lut with
// an escape code, after which the value is read raw (raw_bits wide, minus bias).

namespace iwi::wavelet
{
	struct huffman_entry
	{
		std::int16_t value;
		std::uint8_t bits;
	};

	// lsb-first prefix lut indexed by the low 12 bits of the bit reservoir
	using huffman_lut = std::array<huffman_entry, 4096>;

	template <std::size_t N>
	inline huffman_lut build_lut(const huffman_code (&codes)[N])
	{
		huffman_lut lut{};
		for (const auto& code : codes)
		{
			for (std::size_t i = code.index; i < lut.size(); i += 1ull << code.bits)
			{
				lut[i] = {code.value, code.bits};
			}
		}
		return lut;
	}

	struct codec
	{
		huffman_lut lut;
		unsigned int raw_bits;
		int bias;
	};

	inline const codec& blue_codec()
	{
		static const codec c{build_lut(blue_codes), 9, 255};
		return c;
	}

	inline const codec& redgreen_codec()
	{
		static const codec c{build_lut(redgreen_codes), 10, 510};
		return c;
	}

	inline const codec& alpha_codec()
	{
		static const codec c{build_lut(alpha_codes), 9, 255};
		return c;
	}

	struct bit_reader
	{
		const std::uint8_t* data;
		std::size_t size;
		std::size_t pos = 0;
		std::uint16_t reservoir = 0;
		std::uint8_t bit = 0;
	};

	// reads up to 4 bytes at the current position; missing bytes read as zero
	inline std::uint32_t peek_dword(const bit_reader& reader)
	{
		std::uint32_t value = 0;
		for (std::size_t i = 0; i < 4 && reader.pos + i < reader.size; i++)
		{
			value |= static_cast<std::uint32_t>(reader.data[reader.pos + i]) << (8 * i);
		}
		return value;
	}

	inline void consume_bits(bit_reader& reader, const unsigned int count)
	{
		reader.reservoir >>= count;
		reader.reservoir |= static_cast<std::uint16_t>((peek_dword(reader) >> reader.bit) << (16 - count));
		reader.bit += static_cast<std::uint8_t>(count);
		reader.pos += reader.bit >> 3;
		reader.bit &= 7;
	}

	inline int decode_value(const codec& codec, bit_reader& reader)
	{
		const auto& entry = codec.lut[reader.reservoir & 0xFFF];
		consume_bits(reader, entry.bits);

		int value = entry.value;
		if (value == huffman_escape)
		{
			value = static_cast<int>(reader.reservoir & ((1u << codec.raw_bits) - 1)) - codec.bias;
			consume_bits(reader, codec.raw_bits);
		}
		return value;
	}

	inline void add_delta_to_mipmap(std::uint8_t* inout, const unsigned int pixel_count, const unsigned int channels, bit_reader& reader)
	{
		for (unsigned int i = 0; i < pixel_count; i++)
		{
			for (unsigned int chan = 0; chan < channels; chan++)
			{
				inout[chan] = static_cast<std::uint8_t>(decode_value(alpha_codec(), reader) + inout[chan]);
			}
			inout += channels;
		}
	}

	struct quad
	{
		int parity;
		int coeff[3];
	};

	inline quad read_quad(const codec& codec, const int (&base_coeff)[3], bit_reader& reader)
	{
		quad q;
		q.parity = reader.reservoir & 1;
		consume_bits(reader, 1);

		q.coeff[0] = base_coeff[0] + decode_value(codec, reader);
		q.coeff[1] = base_coeff[1] + decode_value(codec, reader);
		q.coeff[2] = base_coeff[2] + decode_value(codec, reader);
		return q;
	}

	// base is twice the predictor pixel from the previous level
	inline void lift_quad(std::uint8_t* dst, const unsigned int chan, const unsigned int channels,
		const unsigned int stride, const int base, const quad& q)
	{
		std::uint8_t* out = dst + chan;

		out[0] = static_cast<std::uint8_t>(q.parity + ((q.coeff[2] + q.coeff[1] + q.coeff[0] + base) >> 1));
		out[channels] = static_cast<std::uint8_t>((q.coeff[0] + base - (q.coeff[2] + q.coeff[1])) >> 1);
		out[stride] = static_cast<std::uint8_t>((q.coeff[1] - q.coeff[2] + base - q.coeff[0]) >> 1);
		out[channels + stride] = static_cast<std::uint8_t>((base - q.coeff[0] - (q.coeff[1] - q.coeff[2])) >> 1);
	}

	// decodes one mip level; src is the previous (quarter size) level used as the
	// predictor, dst the destination. both use `channels` bytes per pixel.
	inline void decompress_level(std::uint8_t* src, std::uint8_t* dst, const unsigned int width, const unsigned int height,
		const unsigned int channels, bit_reader& reader)
	{
		if (width > 1 && height > 1)
		{
			const bool needs_mip_delta = (reader.reservoir & 1) != 0;
			consume_bits(reader, 1);
			if (needs_mip_delta)
			{
				add_delta_to_mipmap(src, width * height / 4, channels, reader);
			}

			const unsigned int stride = channels * width;
			const int zero[3] = {};

			for (unsigned int y = 0; y < height; y += 2)
			{
				for (unsigned int x = 0; x < width; x += 2)
				{
					if (channels != 1)
					{
						const quad blue = read_quad(blue_codec(), zero, reader);
						lift_quad(dst, 0, channels, stride, 2 * src[0], blue);

						if (channels >= 3)
						{
							const quad red = read_quad(redgreen_codec(), blue.coeff, reader);
							lift_quad(dst, 1, channels, stride, 2 * src[1], red);

							const quad green = read_quad(redgreen_codec(), blue.coeff, reader);
							lift_quad(dst, 2, channels, stride, 2 * src[2], green);
						}
					}

					if (channels != 3)
					{
						const quad alpha = read_quad(alpha_codec(), zero, reader);
						lift_quad(dst, channels - 1, channels, stride, 2 * src[channels - 1], alpha);
					}

					src += channels;
					dst += 2 * channels;
				}

				dst += stride;
			}
		}
		else
		{
			for (unsigned int i = 0; i < width * height; i++)
			{
				for (unsigned int chan = 0; chan < channels; chan++)
				{
					dst[chan] = reader.pos < reader.size ? reader.data[reader.pos] : 0ui8;
					reader.pos++;
				}
				dst += channels;
			}
		}
	}

	// decodes the top mip level of a wavelet-compressed image into `channels` bytes
	// per pixel (file channel order). returns an empty vector when the image cannot
	// be decoded (the codec requires a full mip chain).
	inline std::vector<std::uint8_t> decode_image(const std::uint8_t* data, const std::size_t size,
		const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int mip_count)
	{
		bit_reader reader{data, size};
		bool reader_initialized = false;

		std::vector<std::uint8_t> previous;
		for (int level = static_cast<int>(mip_count) - 1; level >= 0; level--)
		{
			const unsigned int w = std::max(width >> level, 1u);
			const unsigned int h = std::max(height >> level, 1u);

			if (w > 1 && h > 1)
			{
				if (previous.empty())
				{
					return {};
				}

				if (!reader_initialized)
				{
					reader.reservoir = static_cast<std::uint16_t>(peek_dword(reader));
					reader.bit = 0;
					reader.pos += 2;
					reader_initialized = true;
				}
			}

			std::vector<std::uint8_t> current(channels * w * h);
			decompress_level(previous.data(), current.data(), w, h, channels, reader);
			previous = std::move(current);
		}

		return previous;
	}
}
