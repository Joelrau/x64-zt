#pragma once

#include <cstdint>
#include <vector>

namespace lightgrid_sh
{
	struct encoder_config
	{
		std::int8_t range_exp_8bits;
		std::int8_t range_exp_12bits;
		std::int8_t range_exp_16bits;
	};

	constexpr encoder_config default_encode_config{ 0, 0, 23 };

	struct sh_coeffs
	{
		float v[9][3];
	};

	extern const float grid_basis_dirs[56][3];

	unsigned int decode_entry(const encoder_config& cfg, const unsigned char* stream, sh_coeffs& out);

	int encode_entry(const encoder_config& cfg, const sh_coeffs& sh, std::vector<unsigned char>& stream);

	void sh_to_ldr_colors(const sh_coeffs& sh, unsigned char rgb[56][3]);

	void sh_to_hdr_colors(const sh_coeffs& sh, float rgb[56][3]);

	void hdr_colors_to_sh(const float rgb[56][3], sh_coeffs& out);

	void ldr_colors_to_sh(const unsigned char rgb[56][3], sh_coeffs& out);

	void ldr_colors_to_hdr(const unsigned char rgb[56][3], float out[56][3]);

	struct palette
	{
		encoder_config config;                
		std::vector<int> entry_address;       
		std::vector<unsigned char> bitstream; 
	};

	palette build_palette_from_ldr(const unsigned char* colors, unsigned int color_count);

	void extract_palette_to_ldr(const encoder_config& cfg, const unsigned char* bitstream,
		const int* entry_address, unsigned int entry_count, unsigned char* out_colors);

	struct roundtrip_stats
	{
		unsigned int entries;
		int max_byte_error;    
		double avg_byte_error;
	};

	roundtrip_stats verify_sh_roundtrip(const encoder_config& cfg, const unsigned char* bitstream,
		const int* entry_address, unsigned int entry_count);
}
