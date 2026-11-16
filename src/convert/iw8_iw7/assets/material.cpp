#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"
#include "techset.hpp"
#include "gfximage.hpp"

#include <iw7/assets/techset.hpp>

#include <DirectXPackedVector.h>

#include <utils/flags.hpp>

namespace zonetool::iw8::converter::iw7::material
{
	namespace
	{
		namespace target = zonetool::iw7;

		// TODO: stock IW7 techset, convert natively
		const stock_techset lambert_model_techset
		{
			"mo_l_sm_replace_i0c0",
			true,
			{144, 2, 57, 23},
			target::CAMERA_REGION_LIT_OPAQUE,
			19,
			{
				{
					{17, 0xE49E490D, 0, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE49E490D, 0xC012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xC012012, 0, 0, 0},
					2,
				},
				{
					{17, 0xE49E490D, 0x3012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x3012012, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE49E490D, 0xF012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{17, 61, 0, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0, 0, 0, 0},
					14,
				},
				{
					{81, 13, 0xF012012, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{81, 12, 0x1F012012, 0, 0, 0},
					{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
						0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE00E124C, 0x4012012, 0, 0, 0},
					{0x1111E00E124C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E00E124C, 0x808E00E124C,
						0x8080004004C, 0x8080004004C, 0x8080E00E124C, 0x1111E00E124D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x4012012, 0, 0, 0},
					2,
				},
				{
					{49, 0xE49E492C, 0x7012012, 0, 0, 0},
					{0x1111E49E490C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E49E490C, 0x808E49E490C,
						0x8080004004C, 0x8080004004C, 0x8080E49E490C, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x7012012, 0, 0, 0},
					26,
				},
			},
			{
				{0, 0}, {1, 1}, {2, 1}, {3, 2}, {4, 3}, {5, 3}, {6, 4}, {8, 5}, {13, 6}, {14, 6}, {15, 6}, {16, 6}, {17, 6}, {18, 6}, {19, 6},
				{20, 6}, {21, 6}, {22, 6}, {23, 6}, {24, 6}, {25, 6}, {26, 5}, {27, 5}, {28, 7}, {29, 7}, {30, 8}, {38, 0}, {41, 2}, {44, 4},
				{46, 5}, {51, 6}, {52, 6}, {53, 6}, {54, 6}, {55, 6}, {56, 6}, {57, 6}, {58, 6}, {59, 6}, {60, 6}, {61, 6}, {62, 6}, {63, 6},
				{64, 5}, {65, 5}, {68, 8},
			},
			{
				8, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 38, 41, 44, 46, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
				61, 62, 63, 64, 65, 68,
			},
		};

		// TODO: stock IW7 techset, convert natively
		const stock_techset lambert_world_techset
		{
			"w_l_sm_replace_i0c0",
			false,
			{146, 2, 57, 68},
			target::CAMERA_REGION_LIT_OPAQUE,
			19,
			{
				{
					{17, 0xE49E490D, 0, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE49E490D, 0xC012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xC012012, 0, 0, 0},
					2,
				},
				{
					{17, 0xE49E490D, 0x3012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x3012012, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE49E490D, 0xF012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{9, 61, 0, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0, 0, 0, 0},
					13,
				},
				{
					{81, 13, 0xF012012, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{81, 12, 0x1F012012, 0, 0, 0},
					{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
						0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE00E124C, 0x4012012, 0, 0, 0},
					{0x1111E00E124C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E00E124C, 0x808E00E124C,
						0x8080004004C, 0x8080004004C, 0x8080E00E124C, 0x1111E00E124D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x4012012, 0, 0, 0},
					2,
				},
				{
					{49, 0xE49E492C, 0x7012012, 0, 0, 0},
					{0x1111E49E490C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E49E490C, 0x808E49E490C,
						0x8080004004C, 0x8080004004C, 0x8080E49E490C, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x7012012, 0, 0, 0},
					26,
				},
			},
			{
				{0, 0}, {1, 1}, {2, 1}, {3, 2}, {4, 3}, {5, 3}, {6, 4}, {8, 5}, {13, 6}, {14, 6}, {15, 6}, {16, 6}, {17, 6}, {18, 6}, {19, 6},
				{20, 6}, {21, 6}, {22, 6}, {23, 6}, {24, 6}, {25, 6}, {26, 5}, {28, 7}, {29, 7}, {30, 8}, {114, 0}, {115, 1}, {116, 1}, {117, 2},
				{118, 3}, {119, 3}, {120, 4}, {122, 5}, {127, 6}, {128, 6}, {129, 6}, {130, 6}, {131, 6}, {132, 6}, {133, 6}, {134, 6}, {135, 6},
				{136, 6}, {137, 6}, {138, 6}, {139, 6}, {140, 5}, {142, 7}, {143, 7}, {144, 8},
			},
			{
				8, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 30, 114, 115, 116, 117, 118, 119, 120, 122, 127, 128, 129, 130,
				131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 142, 143, 144,
			},
		};
		// TODO: stock IW7 techset, convert natively
		const stock_techset lit_model_techset
		{
			"mo_l_sm_replace_i0c0s0o0n0p0",
			true,
			{144, 2, 57, 37},
			target::CAMERA_REGION_LIT_OPAQUE,
			243,
			{
				{
					{17, 0xE49E490D, 0, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x12012, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE49E490D, 0xC012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xC012012, 0, 0, 0},
					2,
				},
				{
					{17, 0xE49E490D, 0x3012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x3012012, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE49E490D, 0xF012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{17, 61, 0, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x12012, 0, 0, 0},
					14,
				},
				{
					{81, 13, 0xF012012, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{81, 12, 0x1F012012, 0, 0, 0},
					{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
						0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE00E124C, 0x4012012, 0, 0, 0},
					{0x1111E00E124C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E00E124C, 0x808E00E124C,
						0x8080004004C, 0x8080004004C, 0x8080E00E124C, 0x1111E00E124D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x4012012, 0, 0, 0},
					2,
				},
				{
					{49, 0xE49E492C, 0x7012012, 0, 0, 0},
					{0x1111E49E490C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E49E490C, 0x808E49E490C,
						0x8080004004C, 0x8080004004C, 0x8080E49E490C, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x7012012, 0, 0, 0},
					26,
				},
			},
			{
				{0, 0}, {1, 1}, {2, 1}, {3, 2}, {4, 3}, {5, 3}, {6, 4}, {8, 5}, {13, 6}, {14, 6}, {15, 6}, {16, 6}, {17, 6}, {18, 6}, {19, 6},
				{20, 6}, {21, 6}, {22, 6}, {23, 6}, {24, 6}, {25, 6}, {26, 5}, {27, 5}, {28, 7}, {29, 7}, {30, 8}, {38, 0}, {41, 2}, {44, 4},
				{46, 5}, {51, 6}, {52, 6}, {53, 6}, {54, 6}, {55, 6}, {56, 6}, {57, 6}, {58, 6}, {59, 6}, {60, 6}, {61, 6}, {62, 6}, {63, 6},
				{64, 5}, {65, 5}, {68, 8},
			},
			{
				8, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 38, 41, 44, 46, 51, 52, 53, 54, 55, 56, 57, 58, 59,
				60, 61, 62, 63, 64, 65, 68,
			},
		};

		// TODO: stock IW7 techset, convert natively
		const stock_techset lit_world_techset
		{
			"w_l_sm_replace_i0c0s0o0n0p0",
			false,
			{146, 2, 57, 68},
			target::CAMERA_REGION_LIT_OPAQUE,
			19,
			{
				{
					{17, 0xE49E490D, 0, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE49E490D, 0xC012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xC012012, 0, 0, 0},
					2,
				},
				{
					{17, 0xE49E490D, 0x3012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x3012012, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE49E490D, 0xF012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{9, 61, 0, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0, 0, 0, 0},
					13,
				},
				{
					{81, 13, 0xF012012, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{81, 12, 0x1F012012, 0, 0, 0},
					{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
						0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE00E124C, 0x4012012, 0, 0, 0},
					{0x1111E00E124C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E00E124C, 0x808E00E124C,
						0x8080004004C, 0x8080004004C, 0x8080E00E124C, 0x1111E00E124D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x4012012, 0, 0, 0},
					2,
				},
				{
					{49, 0xE49E492C, 0x7012012, 0, 0, 0},
					{0x1111E49E490C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E49E490C, 0x808E49E490C,
						0x8080004004C, 0x8080004004C, 0x8080E49E490C, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x7012012, 0, 0, 0},
					26,
				},
			},
			{
				{0, 0}, {1, 1}, {2, 1}, {3, 2}, {4, 3}, {5, 3}, {6, 4}, {8, 5}, {13, 6}, {14, 6}, {15, 6}, {16, 6}, {17, 6}, {18, 6}, {19, 6},
				{20, 6}, {21, 6}, {22, 6}, {23, 6}, {24, 6}, {25, 6}, {26, 5}, {28, 7}, {29, 7}, {30, 8}, {114, 0}, {115, 1}, {116, 1}, {117, 2},
				{118, 3}, {119, 3}, {120, 4}, {122, 5}, {127, 6}, {128, 6}, {129, 6}, {130, 6}, {131, 6}, {132, 6}, {133, 6}, {134, 6}, {135, 6},
				{136, 6}, {137, 6}, {138, 6}, {139, 6}, {140, 5}, {142, 7}, {143, 7}, {144, 8},
			},
			{
				8, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 30, 114, 115, 116, 117, 118, 119, 120, 122, 127, 128, 129, 130,
				131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 142, 143, 144,
			},
		};

		// TODO: stock IW7 techset, convert natively
		const stock_techset alpha_tested_model_techset
		{
			"mo_l_sm_atest_i0c0s0o0n0pa0",
			true,
			{144, 2, 57, 23},
			target::CAMERA_REGION_LIT_OPAQUE,
			243,
			{
				{
					{22, 0xE49E490D, 0x12012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0, 0, 0, 0},
					2,
				},
				{
					{1046, 0xE49E490D, 0xC012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xC012012, 0, 0, 0},
					2,
				},
				{
					{22, 0xE49E490D, 0x3012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x3012012, 0, 0, 0},
					2,
				},
				{
					{1046, 0xE49E490D, 0xF012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{22, 61, 0x12012, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0, 0, 0, 0},
					14,
				},
				{
					{86, 13, 0xF012012, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{86, 8, 0x1F012012, 0, 0, 0},
					{0x111100000008, 0x7676E02E0248, 0x5656E02E0248, 0x1010E02E0248, 0x404E02E0248, 0x11110000000C, 0x8080000000C,
						0x80800040048, 0x8080004004C, 0x808000000008, 0x111100000009, 0x7676E02E0249, 0x5656E02E0249, 0x1010E02E0249},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{1046, 0xE00E124C, 0x4012012, 0, 0, 0},
					{0x1111E00E124C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E00E124C, 0x808E00E124C,
						0x8080004004C, 0x8080004004C, 0x8080E00E124C, 0x1111E00E124D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x4012012, 0, 0, 0},
					2,
				},
				{
					{49, 0xE49E492C, 0x7012012, 0, 0, 0},
					{0x1111E49E490C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E49E490C, 0x808E49E490C,
						0x8080004004C, 0x8080004004C, 0x8080E49E490C, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x7012012, 0, 0, 0},
					26,
				},
			},
			lit_model_techset.technique_states,
			lit_model_techset.constant_buffer_techniques,
		};

		// TODO: stock IW7 techset, convert natively
		const stock_techset alpha_tested_world_techset
		{
			"w_l_sm_atest_i0c0s0o0n0pa0",
			false,
			{146, 2, 56, 68},
			target::CAMERA_REGION_LIT_OPAQUE,
			19,
			{
				{
					{14, 0xE49E490D, 0x12012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0, 0, 0, 0},
					1,
				},
				{
					{1038, 0xE49E490D, 0xC012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xC012012, 0, 0, 0},
					1,
				},
				{
					{14, 0xE49E490D, 0x3012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x3012012, 0, 0, 0},
					1,
				},
				{
					{1038, 0xE49E490D, 0xF012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					1,
				},
				{
					{14, 61, 0x12012, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0, 0, 0, 0},
					13,
				},
				{
					{78, 13, 0xF012012, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					1,
				},
				{
					{78, 8, 0x1F012012, 0, 0, 0},
					{0x111100000008, 0x7676E02E0248, 0x5656E02E0248, 0x1010E02E0248, 0x404E02E0248, 0x11110000000C, 0x8080000000C,
						0x80800040048, 0x8080004004C, 0x808000000008, 0x111100000009, 0x7676E02E0249, 0x5656E02E0249, 0x1010E02E0249},
					{0xF012012, 0, 0, 0},
					1,
				},
				{
					{1038, 0xE00E124C, 0x4012012, 0, 0, 0},
					{0x1111E00E124C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E00E124C, 0x808E00E124C,
						0x8080004004C, 0x8080004004C, 0x8080E00E124C, 0x1111E00E124D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x4012012, 0, 0, 0},
					1,
				},
				{
					{49, 0xE49E492C, 0x7012012, 0, 0, 0},
					{0x1111E49E490C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E49E490C, 0x808E49E490C,
						0x8080004004C, 0x8080004004C, 0x8080E49E490C, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x7012012, 0, 0, 0},
					26,
				},
			},
			lit_world_techset.technique_states,
			lit_world_techset.constant_buffer_techniques,
		};

		// TODO: stock IW7 techset, convert natively
		const stock_techset emissive_model_techset
		{
			"mo_l_sm_replace_i0c0s0o0n0e0p0",
			true,
			lit_model_techset.flags,
			target::CAMERA_REGION_LIT_OPAQUE,
			lit_model_techset.sampler_state,
			lit_model_techset.states,
			lit_model_techset.technique_states,
			lit_model_techset.constant_buffer_techniques,
		};

		// TODO: stock IW7 techset, convert natively
		const stock_techset decal_model_techset
		{
			"mo_l_sm_ndw_blend_i0c0s0o0n0pa0",
			true,
			{17, 10, 53, 23},
			target::CAMERA_REGION_LIT_DECAL,
			19,
			{
				{
					{82, 28, 0xF012165, 0, 0, 0},
					{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
						0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012165, 0, 0, 0},
					6,
				},
				{
					{82, 28, 0xF012162, 0xF012162, 0, 0},
					{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
						0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012162, 0xF012162, 0, 0},
					6,
				},
				{
					{1042, 0xE00E125C, 0x4012012, 0, 0, 0},
					{0x1111E00E124C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E00E124C, 0x808E00E124C,
						0x8080004004C, 0x8080004004C, 0x8080E00E124C, 0x1111E00E124D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x4012012, 0, 0, 0},
					6,
				},
				{
					{49, 0xE49E492C, 0x7012012, 0, 0, 0},
					{0x1111E49E490C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E49E490C, 0x808E49E490C,
						0x8080004004C, 0x8080004004C, 0x8080E49E490C, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x7012012, 0, 0, 0},
					26,
				},
			},
			{
				{8, 0}, {13, 1}, {14, 1}, {15, 1}, {16, 1}, {17, 1}, {18, 1}, {19, 1}, {20, 1}, {21, 1}, {22, 1}, {23, 1}, {24, 1}, {25, 1},
				{26, 1}, {27, 0}, {28, 2}, {29, 2}, {30, 3}, {46, 0}, {51, 1}, {52, 1}, {53, 1}, {54, 1}, {55, 1}, {56, 1}, {57, 1}, {58, 1},
				{59, 1}, {60, 1}, {61, 1}, {62, 1}, {63, 1}, {64, 1}, {65, 0}, {68, 3},
			},
			{
				8, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 46, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
				64, 65, 68,
			},
		};

		// TODO: stock IW7 techset, convert natively
		const stock_techset blended_model_techset
		{
			"mo_l_sm_ndw_blend_i0c0s0o0n0pa0",
			true,
			{16, 30, 49, 23},
			target::CAMERA_REGION_LIT_TRANS,
			decal_model_techset.sampler_state,
			{
				{
					{82, 12, 0xF012165, 0, 0, 0},
					{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
						0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012165, 0, 0, 0},
					2,
				},
				{
					{82, 12, 0xF012162, 0xF012162, 0, 0},
					{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
						0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012162, 0xF012162, 0, 0},
					2,
				},
				{
					{1042, 0xE00E124C, 0x4012012, 0, 0, 0},
					{0x1111E00E124C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E00E124C, 0x808E00E124C,
						0x8080004004C, 0x8080004004C, 0x8080E00E124C, 0x1111E00E124D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x4012012, 0, 0, 0},
					2,
				},
				{
					{49, 0xE49E492C, 0x7012012, 0, 0, 0},
					{0x1111E49E490C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E49E490C, 0x808E49E490C,
						0x8080004004C, 0x8080004004C, 0x8080E49E490C, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x7012012, 0, 0, 0},
					26,
				},
			},
			decal_model_techset.technique_states,
			decal_model_techset.constant_buffer_techniques,
		};

		// TODO: stock IW7 techset, convert natively
		const stock_techset two_layer_world_techset
		{
			"w_l_sm_replace_r0c0s0o0nl0p0_b1c1s1ol1nl1v1p1",
			false,
			lit_world_techset.flags,
			target::CAMERA_REGION_LIT_OPAQUE,
			lit_world_techset.sampler_state,
			{
				{
					{17, 0xE49E490D, 2, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE49E490D, 0xC012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xC012012, 0, 0, 0},
					2,
				},
				{
					{17, 0xE49E490D, 0x3012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x3012012, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE49E490D, 0xF012012, 0, 0, 0},
					{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
						0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{9, 61, 2, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0, 0, 0, 0},
					13,
				},
				{
					{81, 13, 0xF012012, 0, 0, 0},
					{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
						0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{81, 12, 0x1F012012, 0, 0, 0},
					{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
						0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{81, 12, 0xF012012, 0, 0, 0},
					{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
						0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0xF012012, 0, 0, 0},
					2,
				},
				{
					{1041, 0xE00E124C, 0x4012012, 0, 0, 0},
					{0x1111E00E124C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E00E124C, 0x808E00E124C,
						0x8080004004C, 0x8080004004C, 0x8080E00E124C, 0x1111E00E124D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x4012012, 0, 0, 0},
					2,
				},
				{
					{49, 0xE49E492C, 0x7012012, 0, 0, 0},
					{0x1111E49E490C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x1111E49E490C, 0x808E49E490C,
						0x8080004004C, 0x8080004004C, 0x8080E49E490C, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
					{0x7012012, 0, 0, 0},
					26,
				},
			},
			{
				{0, 0}, {1, 1}, {2, 1}, {3, 2}, {4, 3}, {5, 3}, {6, 4}, {8, 5}, {13, 6}, {14, 6}, {15, 6}, {16, 6}, {17, 6}, {18, 6}, {19, 6},
				{20, 6}, {21, 6}, {22, 6}, {23, 6}, {24, 6}, {25, 6}, {26, 7}, {28, 8}, {29, 8}, {30, 9}, {114, 0}, {115, 1}, {116, 1}, {117, 2},
				{118, 3}, {119, 3}, {120, 4}, {122, 5}, {127, 6}, {128, 6}, {129, 6}, {130, 6}, {131, 6}, {132, 6}, {133, 6}, {134, 6}, {135, 6},
				{136, 6}, {137, 6}, {138, 6}, {139, 6}, {140, 7}, {142, 8}, {143, 8}, {144, 9},
			},
			lit_world_techset.constant_buffer_techniques,
		};

		// TODO: stock IW7 techset, convert natively
		const stock_techset three_layer_world_techset
		{
			"w_l_sm_replace_r0c0s0o0nl0p0_b1c1s1ol1nl1v1p1_b2c2s2ol2nl2v2p2",
			false,
			two_layer_world_techset.flags,
			target::CAMERA_REGION_LIT_OPAQUE,
			two_layer_world_techset.sampler_state,
			two_layer_world_techset.states,
			two_layer_world_techset.technique_states,
			two_layer_world_techset.constant_buffer_techniques,
		};

		// TODO: stock IW7 techset, convert natively
		const stock_techset four_layer_world_techset
		{
			"w_l_sm_replace_r0c0s0o0nl0p0_b1c1s1ol1nl1v1p1_b2c2s2ol2nl2v2p2_b3c3s3ol3nl3v3p3",
			false,
			two_layer_world_techset.flags,
			target::CAMERA_REGION_LIT_OPAQUE,
			two_layer_world_techset.sampler_state,
			two_layer_world_techset.states,
			two_layer_world_techset.technique_states,
			two_layer_world_techset.constant_buffer_techniques,
		};

		const std::array layered_world_techsets{&two_layer_world_techset, &three_layer_world_techset, &four_layer_world_techset};

		const stock_techset two_d_techset
		{
			"2d",
			true,
			{0, 41, 64, 0, 1},
			target::CAMERA_REGION_NONE,
			243,
			{
				{
					{10, 0xE00E0002, 0xF012165, 0, 0, 0},
					{0x1111E00E0002, 0x7676E02E0242, 0x5656E02E0242, 0x1010E02E0242, 0x404E02E0242, 0x1111E00E000C, 0x808E00E000C,
						0x80800040042, 0x8080004004C, 0x8080E00E0002, 0x1111E00E0003, 0x7676E02E0243, 0x5656E02E0243, 0x1010E02E0243},
					{0xF012165, 0, 0, 0},
					1,
				},
				{
					{10, 0xE00E0002, 0xF012165, 0xF012165, 0, 0},
					{0x1111E00E0002, 0x7676E02E0242, 0x5656E02E0242, 0x1010E02E0242, 0x404E02E0242, 0x1111E00E000C, 0x808E00E000C,
						0x80800040042, 0x8080004004C, 0x8080E00E0002, 0x1111E00E0003, 0x7676E02E0243, 0x5656E02E0243, 0x1010E02E0243},
					{0xF012165, 0xF012165, 0, 0},
					1,
				},
			},
			{{8, 0}, {9, 0}, {10, 0}, {11, 1}, {12, 0}},
			{},
		};

		// TODO: values of the stock opaque world states (w_l_sm_replace_i0c0s0n0p0) and sky state (w_sky), the stencil bits are not decoded
		const std::array<state_definition, techset::GENERATED_TECHNIQUE_COUNT> technique_states
		{{
			{
				{17, 0xE49E490D, 0, 0, 0, 0},
				{0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x1111E49E490D, 0x808E49E490D,
					0x8080004004D, 0x8080004004D, 0x8080E49E490D, 0x1111E49E490D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
				{},
				target::RASTERIZER_STATE_CULL_BACK,
			},
			{
				{9, 61, 0, 0, 0, 0},
				{0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D, 0x404E02E024D, 0x11110000000D, 0x8080000000D,
					0x8080004004D, 0x8080004004D, 0x80800000000D, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
				{},
				13,
			},
			{
				{81, 12, 0x1F012012, 0, 0, 0},
				{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
					0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
				{0xF012012, 0, 0, 0},
				target::RASTERIZER_STATE_CULL_BACK,
			},
			{
				{81, 12, 0x1F012012, 0, 0, 0},
				{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
					0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
				{0xF012012, 0, 0, 0},
				target::RASTERIZER_STATE_CULL_BACK,
			},
			{
				{81, 12, 0xF012012, 0, 0, 0},
				{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
					0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
				{0xF012012, 0, 0, 0},
				target::RASTERIZER_STATE_CULL_BACK,
			},
			{
				{82, 28, 0xF012162, 0, 0, 0},
				{0x11110000000C, 0x7676E02E024C, 0x5656E02E024C, 0x1010E02E024C, 0x404E02E024C, 0x11110000000C, 0x8080000000C,
					0x8080004004C, 0x8080004004C, 0x80800000000C, 0x11110000000D, 0x7676E02E024D, 0x5656E02E024D, 0x1010E02E024D},
				{0xF012162, 0, 0, 0},
				6,
			},
		}};

		material_flags get_material_flags(const Material* asset, const techset::generated_techset techset_type)
		{
			const unsigned char opaque_state = target::STATE_FLAG_CULL_BACK | target::STATE_FLAG_WRITES_DEPTH | target::STATE_FLAG_USES_DEPTH_BUFFER
				| target::STATE_FLAG_USES_STENCIL_BUFFER;

			switch (techset_type)
			{
			case techset::GENERATED_TECHSET_WORLD:
				return {146, 2, opaque_state, 68};
			case techset::GENERATED_TECHSET_MODEL:
				return {144, 2, opaque_state, 23};
			case techset::GENERATED_TECHSET_DECAL:
				return {19, asset->sortKey, target::STATE_FLAG_CULL_BACK | target::STATE_FLAG_DECAL | target::STATE_FLAG_USES_DEPTH_BUFFER
					| target::STATE_FLAG_USES_STENCIL_BUFFER, 68};
			default:
				return {12, 3, target::STATE_FLAG_CULL_BACK | target::STATE_FLAG_USES_DEPTH_BUFFER, 68};
			}
		}

		bool is_hidden(const Material* asset)
		{
			const auto is_world = (asset->materialType & 0xFF0000) == 0x210000;
			return (asset->surfaceFlags & SURFACE_FLAG_NODRAW) || (is_world && asset->cameraRegion == CAMERA_REGION_LIT_TRANS); // TODO: lit trans world
		}

		unsigned char get_camera_region(const Material* asset, const techset::generated_techset techset_type)
		{
			if (is_hidden(asset))
			{
				return target::CAMERA_REGION_NONE_2;
			}

			return techset_type == techset::GENERATED_TECHSET_DECAL ? target::CAMERA_REGION_LIT_DECAL : target::CAMERA_REGION_LIT_OPAQUE;
		}

		std::optional<techset::generated_techset> get_world_techset(const Material* asset)
		{
			if (asset->contents & PHYSICS_CONTENTS_SKY)
			{
				return techset::GENERATED_TECHSET_SKY;
			}

			switch (asset->cameraRegion)
			{
			case CAMERA_REGION_LIT_OPAQUE:
				return techset::GENERATED_TECHSET_WORLD;
			case CAMERA_REGION_LIT_DECAL:
				return techset::GENERATED_TECHSET_DECAL;
			default:
				return techset::GENERATED_TECHSET_WORLD; // TODO: distortion and other transparent world materials
			}
		}

		std::optional<techset::generated_techset> get_techset(const Material* asset)
		{
			if (!asset->techniqueSet)
			{
				return {};
			}

			switch (asset->materialType & 0xFF0000)
			{
			case 0x210000:
				return get_world_techset(asset);
			case 0x080000:
			case 0x090000:
				if (asset->materialType & 0x10) // lit models only, 0x90008 is tml/model_shadowcaster
				{
					return techset::GENERATED_TECHSET_MODEL;
				}

				return {};
			default:
				return {};
			}
		}

		GfxImage* find_texture(const Material* asset, const unsigned char index)
		{
			for (auto i = 0; i < asset->textureCount; i++)
			{
				if (asset->textureTable[i].index == index)
				{
					return asset->textureTable[i].image;
				}
			}

			return nullptr;
		}

		std::optional<std::array<std::uint32_t, 4>> find_constant(const Material* asset, const unsigned char index)
		{
			for (auto i = 0; i < asset->constantCount; i++)
			{
				if (asset->constantTable[i].index == index)
				{
					return std::bit_cast<std::array<std::uint32_t, 4>>(asset->constantTable[i].literal);
				}
			}

			return {};
		}

		float half_to_float(const std::uint16_t value)
		{
			return DirectX::PackedVector::XMConvertHalfToFloat(value);
		}

		float byte_of(const std::uint32_t value, const int byte)
		{
			return static_cast<float>((value >> (byte * 8)) & 0xFF);
		}

		float get_reveal_scale(const float tint_alpha)
		{
			return std::bit_cast<float>(0x7EF311C2u - std::bit_cast<std::uint32_t>(tint_alpha * std::bit_cast<float>(0x1BBDDBD2u)));
		}

		void decode_uv(const std::array<std::uint32_t, 4>& bits, surface_constants& constants)
		{
			constants.uv_rotation = {half_to_float(bits[1] & 0xFFFF), half_to_float(bits[1] >> 16), 0.0f, 0.0f};
			constants.uv_transform = {half_to_float(bits[2] & 0xFFFF), half_to_float(bits[2] >> 16),
				half_to_float(bits[3] & 0xFFFF), half_to_float(bits[3] >> 16)};
		}

		surface_constants decode_constants(const Material* asset)
		{
			surface_constants constants;

			auto use_tint = true;
			if (const auto uv = find_constant(asset, 0))
			{
				const auto& bits = *uv;
				use_tint = (bits[0] & 64) == 0;
				constants.decal_params[0] = bits[0] & 8 ? 1.0f : 0.0f;
				constants.decal_params[1] = bits[0] & 16 ? 1.0f : 0.0f;
				decode_uv(bits, constants);
			}

			const auto tint = find_constant(asset, 9);
			constants.decal_params[2] = get_reveal_scale(tint ? byte_of((*tint)[0], 3) : 0.0f);
			if (tint && use_tint)
			{
				const auto bits = (*tint)[0];
				constants.color_tint[0] = byte_of(bits, 0) / 255.0f;
				constants.color_tint[1] = byte_of(bits, 1) / 255.0f;
				constants.color_tint[2] = byte_of(bits, 2) / 255.0f;
			}

			if (const auto ranges = find_constant(asset, 54))
			{
				const auto& bits = *ranges;
				constants.surface_ranges[0] = (byte_of(bits[0], 0) + 1.0f) / 256.0f;
				constants.surface_ranges[1] = byte_of(bits[0], 1) / 256.0f;
				constants.surface_ranges[2] = (byte_of(bits[1], 0) + 1.0f) / 256.0f;
				constants.surface_ranges[3] = (byte_of(bits[1], 2) + 1.0f) / 256.0f;
				constants.color_tint[3] = byte_of(bits[1], 1) / 256.0f;
			}

			return constants;
		}

		ordered_json make_texture(const char* name, const char* image, const target::TextureSemantic semantic, const unsigned char sampler_state)
		{
			const std::string_view type_name(name);
			return
			{
				{"image", image},
				{"semantic", semantic},
				{"samplerState", sampler_state},
				{"lastCharacter", type_name.back()},
				{"firstCharacter", type_name.front()},
				{"typeHash", techset::hash_material_name(type_name)},
			};
		}

		void write_json(const std::string& path, const ordered_json& data)
		{
			filesystem::file file(path);
			file.open("wb");
			file.write(data.dump(4));
			file.close();
		}

		std::vector<techset::generated_technique> get_used_techniques(const techset::generated_techset techset_type)
		{
			std::vector<techset::generated_technique> used;
			for (auto i = 0u; i < target::TECHNIQUE_COUNT; i++)
			{
				const auto technique = techset::get_generated_technique(techset_type, i);
				if (technique && std::ranges::find(used, *technique) == used.end())
				{
					used.emplace_back(*technique);
				}
			}

			std::ranges::sort(used);
			return used;
		}

		void write_states(const std::string& techset_name, const std::string& name, std::array<unsigned char, target::TECHNIQUE_COUNT>& entries,
			const std::vector<state_definition>& states, const unsigned char state_flags)
		{
			target::techset::dump_statebits(techset_name, name, entries.data());

			ordered_json state_map = ordered_json::array();
			for (const auto& state : states)
			{
				state_map.push_back(
				{
					{"loadBits", state.load_bits},
					{"depthStencilStateBits", state.depth_stencil_bits},
					{"blendStateBits", state.blend_bits},
					{"rasterizerState", state.rasterizer_state},
				});
			}

			write_json(std::format("techsets\\state\\{}\\{}.statebitsmap", techset_name, name), state_map);
			write_json(std::format("techsets\\state\\{}\\{}.stateinfo", techset_name, name), {{"stateFlags", state_flags}});
		}

		void dump_states(const techset::generated_techset techset_type, const std::string& techset_name, const std::string& name,
			const unsigned char state_flags)
		{
			const auto used = get_used_techniques(techset_type);

			std::array<unsigned char, target::TECHNIQUE_COUNT> entries{};
			for (auto i = 0u; i < entries.size(); i++)
			{
				const auto technique = techset::get_generated_technique(techset_type, i);
				entries[i] = technique ? static_cast<unsigned char>(std::ranges::find(used, *technique) - used.begin()) : 0xFF;
			}

			std::vector<state_definition> states;
			for (const auto technique : used)
			{
				states.emplace_back(technique_states[technique]);
			}

			write_states(techset_name, name, entries, states, state_flags);
		}

		void write_constant_buffer(const std::string& techset_name, const std::string& name, std::array<unsigned char, target::TECHNIQUE_COUNT>& indexes,
			const std::span<const char* const> names, const ordered_json& constants)
		{
			target::techset::dump_constant_buffer_indexes(techset_name, name, indexes.data());

			std::vector<unsigned char> data(names.size() * 16);
			std::vector<unsigned short> offsets(names.size());
			for (auto i = 0u; i < constants.size(); i++)
			{
				const auto row = std::ranges::find(names, constants[i]["name"].get<std::string>()) - names.begin();
				offsets[i] = static_cast<unsigned short>(row * 16);

				const auto literal = constants[i]["literal"].get<std::array<float, 4>>();
				std::memcpy(&data[offsets[i]], literal.data(), sizeof(literal));
			}

			target::MaterialConstantBufferDef buffer{};
			buffer.psDataSize = static_cast<unsigned int>(data.size());
			buffer.psData = data.data();
			buffer.psOffsetDataSize = static_cast<unsigned int>(offsets.size());
			buffer.psOffsetData = offsets.data();
			target::techset::dump_constant_buffer_def_array(techset_name, name, 1, &buffer);
		}

		void dump_constant_buffer(const techset::generated_techset techset_type, const std::string& techset_name, const std::string& name,
			const ordered_json& constants)
		{
			std::array<unsigned char, target::TECHNIQUE_COUNT> indexes{};
			for (auto i = 0u; i < indexes.size(); i++)
			{
				const auto technique = techset::get_generated_technique(techset_type, i);
				const auto uses_material_buffer = technique == techset::GENERATED_LIT || technique == techset::GENERATED_UNLIT || technique == techset::GENERATED_SKY
					|| technique == techset::GENERATED_DECAL;
				indexes[i] = uses_material_buffer ? 0 : 0xFF;
			}

			write_constant_buffer(techset_name, name, indexes, techset::get_constant_names(techset_type), constants);
		}

		unsigned char get_rasterizer_cull(const Material* asset)
		{
			const auto techset = asset->techniqueSet;
			if (!techset || !techset->maskedTechniques)
			{
				return target::RASTERIZER_STATE_CULL_BACK;
			}

			for (auto i = 0u; i < get_technique_count(techset); i++)
			{
				const auto technique = techset->maskedTechniques[i];
				if (!technique || get_technique_type(techset, i) != TECHNIQUE_DEPTH_PREPASS)
				{
					continue;
				}

				switch (technique->stateBits.rasterizerBits & GFX_RASTERIZER_CULL_MASK)
				{
				case GFX_RASTERIZER_CULL_NONE:
					return target::RASTERIZER_STATE_CULL_NONE;
				case GFX_RASTERIZER_CULL_FRONT:
					return target::RASTERIZER_STATE_CULL_FRONT;
				default:
					return target::RASTERIZER_STATE_CULL_BACK;
				}
			}

			return target::RASTERIZER_STATE_CULL_BACK;
		}

		unsigned char get_cull_state_flags(const unsigned char state_flags, const unsigned char cull)
		{
			const auto cull_flags = cull == target::RASTERIZER_STATE_CULL_NONE ? 0
				: cull == target::RASTERIZER_STATE_CULL_FRONT ? target::STATE_FLAG_CULL_FRONT : target::STATE_FLAG_CULL_BACK;
			return static_cast<unsigned char>((state_flags & ~target::STATE_FLAG_CULL_MASK) | cull_flags);
		}

		void dump_stock_states(const stock_techset& stock, const std::string& name, const unsigned char cull)
		{
			std::array<unsigned char, target::TECHNIQUE_COUNT> entries;
			entries.fill(0xFF);
			for (const auto& [technique, state] : stock.technique_states)
			{
				entries[technique] = state;
			}

			auto states = stock.states;
			for (auto& state : states)
			{
				state.load_bits[0] = (state.load_bits[0] & ~target::GFXS0_CULL_MASK) | (cull << target::GFXS0_CULL_SHIFT);
				state.rasterizer_state = static_cast<unsigned char>((state.rasterizer_state & ~target::RASTERIZER_STATE_CULL_MASK) | cull);
			}

			write_states(stock.name, name, entries, states, get_cull_state_flags(stock.flags.state_flags, cull));
		}

		void dump_stock_constant_buffer(const stock_techset& stock, const std::string& name, const std::vector<ordered_json>& rows,
			const ordered_json& constants)
		{
			std::array<unsigned char, target::TECHNIQUE_COUNT> indexes;
			indexes.fill(0xFF);
			for (const auto technique : stock.constant_buffer_techniques)
			{
				indexes[technique] = 0;
			}

			std::vector<std::string> constant_names;
			for (const auto& row : rows)
			{
				constant_names.emplace_back(row["name"].get<std::string>());
			}

			std::vector<const char*> names;
			for (const auto& constant_name : constant_names)
			{
				names.emplace_back(constant_name.data());
			}

			write_constant_buffer(stock.name, name, indexes, names, constants);
		}

		ordered_json make_constants(const surface_constants& values, const std::span<const char* const> names)
		{
			const std::array<const std::array<float, 4>*, techset::decal_constant_names.size()> rows
			{
				&values.uv_transform, &values.uv_rotation, &values.color_tint, &values.surface_ranges, &values.decal_params,
			};

			std::vector<std::pair<std::uint32_t, std::size_t>> order;
			for (auto i = 0u; i < names.size(); i++)
			{
				order.emplace_back(techset::hash_material_name(names[i]), i);
			}

			std::ranges::sort(order);

			ordered_json constants = ordered_json::array();
			for (const auto& [hash, row] : order)
			{
				constants.push_back({{"name", names[row]}, {"nameHash", hash}, {"literal", *rows[row]}});
			}

			return constants;
		}

		const char* get_image_name(const GfxImage* image, const char* code_image)
		{
			if (image)
			{
				return image->name;
			}

			add_csv_row("image", std::format(",{}", code_image));
			return code_image;
		}

		ordered_json make_textures(const Material* asset, const techset::generated_techset techset_type)
		{
			const auto color = get_image_name(find_texture(asset, 0), "$white");
			const auto normal = get_image_name(find_texture(asset, 9), "$identitynormalmap");

			std::vector<ordered_json> textures
			{
				make_texture(techset::material_texture_names[0], color, target::TS_COLOR_SPECULAR_MAP, 19),
				make_texture(techset::material_texture_names[1], normal, target::TS_NORMAL_OCCLUSSION_GLOSS_MAP, 20),
			};

			if (techset_type == techset::GENERATED_TECHSET_DECAL)
			{
				const auto alpha_reveal = get_image_name(find_texture(asset, 27), "$white");
				textures.emplace_back(make_texture(techset::decal_texture_names[2], alpha_reveal, target::TS_ALPHA_REVEAL_THICKNESS_MAP, 19));
			}

			std::ranges::sort(textures, {}, [](const ordered_json& texture) { return texture["typeHash"].get<std::uint32_t>(); });
			return textures;
		}

		ordered_json make_sky_constants(const Material* asset)
		{
			const auto params = find_constant(asset, 128);
			const auto literal = params ? std::bit_cast<std::array<float, 4>>(*params) : std::array<float, 4>{1.0f, 0.0f, 0.0f, 0.0f};

			const auto name = techset::sky_constant_names[0];
			return ordered_json::array({{{"name", name}, {"nameHash", techset::hash_material_name(name)}, {"literal", literal}}});
		}

		ordered_json make_sky_textures(const Material* asset)
		{
			const auto sky = get_image_name(find_texture(asset, 151), "$black");
			return ordered_json::array({make_texture(techset::material_texture_names[0], sky, target::TS_COLOR_MAP, 243)});
		}

		std::vector<unsigned char> get_dumped_texture_indexes(const techset::generated_techset techset_type)
		{
			switch (techset_type)
			{
			case techset::GENERATED_TECHSET_SKY:
				return {151};
			case techset::GENERATED_TECHSET_DECAL:
				return {0, 9, 27};
			default:
				return {0, 9};
			}
		}

		ordered_json make_header(const std::string& techset_name, const material_flags& flags, const unsigned char camera_region)
		{
			ordered_json data;
			data["techniqueSet->name"] = techset_name;
			data["gameFlags"] = flags.game_flags;
			data["unkFlags"] = 0;
			data["sortKey"] = flags.sort_key;
			data["renderFlags"] = flags.render_flags;
			data["textureAtlasRowCount"] = 1;
			data["textureAtlasColumnCount"] = 1;
			data["textureAtlasFrameBlend"] = 0;
			data["textureAtlasAsArray"] = 0;
			data["surfaceTypeBits"] = 0; // TODO: convert from surfaceFlags
			data["stateFlags"] = flags.state_flags;
			data["cameraRegion"] = camera_region;
			data["materialType"] = flags.material_type;
			data["assetFlags"] = 0;
			return data;
		}

		bool has_plain_uv(const surface_constants& constants)
		{
			return constants.uv_transform == std::array{1.0f, 1.0f, 0.0f, 0.0f} && constants.uv_rotation[0] == 1.0f && constants.uv_rotation[1] == 0.0f;
		}

		bool has_plain_layer_uv(const Material* asset, const unsigned int layer)
		{
			surface_constants constants;
			if (const auto uv = find_constant(asset, static_cast<unsigned char>(layer)))
			{
				decode_uv(*uv, constants);
			}

			return has_plain_uv(constants);
		}

		unsigned int get_layer_count(const Material* asset)
		{
			const auto max_count = std::min<unsigned int>(asset->layerCount, static_cast<unsigned int>(layered_world_techsets.size()) + 1);

			auto count = 1u;
			while (count < max_count && has_plain_layer_uv(asset, count))
			{
				count++;
			}

			return count;
		}

		unsigned int get_stock_layer_count(const stock_techset* stock)
		{
			const auto layered = std::ranges::find(layered_world_techsets, stock);
			return layered != layered_world_techsets.end() ? static_cast<unsigned int>(layered - layered_world_techsets.begin()) + 2 : 1;
		}

		std::uint32_t get_layer_flags(const Material* asset, const unsigned int layer)
		{
			const auto uv = find_constant(asset, static_cast<unsigned char>(layer));
			return uv ? (*uv)[0] : 0;
		}

		std::array<float, 4> get_layer_tint(const Material* asset, const unsigned int layer)
		{
			const auto tint = find_constant(asset, static_cast<unsigned char>(9 + layer));
			if (!tint || (get_layer_flags(asset, layer) & MATERIAL_LAYER_TINT_MASK))
			{
				return {1.0f, 1.0f, 1.0f, 1.0f};
			}

			const auto bits = (*tint)[0];
			return {byte_of(bits, 0) / 255.0f, byte_of(bits, 1) / 255.0f, byte_of(bits, 2) / 255.0f, 1.0f};
		}

		GfxImage* find_loaded_image(const GfxImage* handle)
		{
			const auto image = handle ? static_cast<GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, handle->name)) : nullptr;
			return image && image->width ? image : nullptr;
		}

		GfxImage* find_layer_mask(const Material* asset, const unsigned int layer)
		{
			return find_loaded_image(find_texture(asset, static_cast<unsigned char>(27 + layer)));
		}

		bool uses_mask_blend(const Material* asset, const unsigned int layer)
		{
			return (get_layer_flags(asset, layer) & MATERIAL_LAYER_BLEND_MASK) && find_layer_mask(asset, layer);
		}

		bool uses_height_blend(const Material* asset, const unsigned int layer)
		{
			return (get_layer_flags(asset, layer) & MATERIAL_LAYER_BLEND_HEIGHT) && find_layer_mask(asset, layer);
		}

		std::string get_reveal_image(const Material* asset, const unsigned int layer)
		{
			return gfximage::dump_layer_reveal(find_layer_mask(asset, layer), uses_mask_blend(asset, layer), uses_height_blend(asset, layer));
		}

		float get_reveal_sharpness(const Material* asset, const unsigned int layer)
		{
			if (!uses_height_blend(asset, layer))
			{
				return 0.85f; // fit of the height blend to the vertex weight
			}

			const auto tint = find_constant(asset, static_cast<unsigned char>(9 + layer));
			return tint ? byte_of((*tint)[0], 3) / 255.0f : 0.0f;
		}

		std::string get_color_image(const Material* asset)
		{
			const auto handle = find_texture(asset, 0);
			const auto image = find_loaded_image(handle);
			if (!image)
			{
				return get_image_name(handle, "$white");
			}

			if (const auto name = gfximage::dump_color(image))
			{
				return *name;
			}

			gfximage::dump(image);
			return image->name;
		}

		std::string get_color_specular_image(const Material* asset, const unsigned int layer)
		{
			const auto handle = find_texture(asset, static_cast<unsigned char>(layer));
			const auto image = find_loaded_image(handle);
			if (!image)
			{
				return get_image_name(handle, "$white");
			}

			const auto ranges = find_constant(asset, static_cast<unsigned char>(54 + layer));
			const auto scale = static_cast<unsigned char>(ranges ? (*ranges)[0] & 0xFF : 0xFF);
			const auto offset = static_cast<unsigned char>(ranges ? ((*ranges)[0] >> 8) & 0xFF : 0);
			if (const auto name = gfximage::dump_color_specular(image, scale, offset))
			{
				return *name;
			}

			gfximage::dump(image);
			return image->name;
		}

		std::string get_normal_image(const Material* asset, const unsigned int layer)
		{
			const auto handle = find_texture(asset, static_cast<unsigned char>(9 + layer));
			const auto image = find_loaded_image(handle);
			if (!image)
			{
				return get_image_name(handle, "$identitynormalmap");
			}

			const auto ranges = find_constant(asset, static_cast<unsigned char>(54 + layer));
			const auto gloss_scale = static_cast<unsigned char>(ranges ? (*ranges)[1] & 0xFF : 0xFF);
			const auto gloss_offset = static_cast<unsigned char>(ranges ? ((*ranges)[1] >> 8) & 0xFF : 0);
			const auto normal_scale = static_cast<unsigned char>(ranges ? ((*ranges)[1] >> 16) & 0xFF : 0xFF);
			if (const auto name = gfximage::dump_normal_gloss(image, gloss_scale, gloss_offset, normal_scale))
			{
				return *name;
			}

			gfximage::dump(image);
			return image->name;
		}

		std::string get_alpha_image(const Material* asset)
		{
			const auto handle = find_texture(asset, 27);
			if (const auto image = find_loaded_image(handle))
			{
				gfximage::dump(image);
				return image->name;
			}

			return get_image_name(handle, "$white");
		}

		std::array<float, 4> get_emissive_tint(const Material* asset)
		{
			const auto tint = find_constant(asset, 36);
			return tint ? std::bit_cast<std::array<float, 4>>(*tint) : std::array{1.0f, 1.0f, 1.0f, 1.0f};
		}

		float get_emissive_intensity(const Material* asset)
		{
			const auto tint = get_emissive_tint(asset);
			return std::max({tint[0], tint[1], tint[2]});
		}

		std::string get_emissive_image(const Material* asset)
		{
			const auto handle = find_texture(asset, 18);
			const auto image = find_loaded_image(handle);
			const auto tint = get_emissive_tint(asset);
			const auto intensity = get_emissive_intensity(asset);
			if (!image || intensity <= 0.0f)
			{
				return get_image_name(handle, "$black");
			}

			if (const auto name = gfximage::dump_emissive(image, {tint[0] / intensity, tint[1] / intensity, tint[2] / intensity}))
			{
				return *name;
			}

			gfximage::dump(image);
			return image->name;
		}

		ordered_json make_constant(const char* name, const std::array<float, 4>& literal)
		{
			return {{"name", name}, {"nameHash", techset::hash_material_name(name)}, {"literal", literal}};
		}

		bool is_lambert(const stock_techset& stock)
		{
			return &stock == &lambert_model_techset || &stock == &lambert_world_techset;
		}

		bool uses_alpha_map(const stock_techset& stock)
		{
			return &stock == &alpha_tested_model_techset || &stock == &alpha_tested_world_techset || &stock == &decal_model_techset
				|| &stock == &blended_model_techset;
		}

		void dump_stock_material(const Material* asset, const stock_techset& stock)
		{
			if (mark_dumped(ASSET_TYPE_TECHNIQUE_SET, stock.name))
			{
				if (stock.in_common_zone)
				{
					add_csv_row("require", "techsets_common");
				}
				add_csv_row("techset", stock.in_common_zone ? std::format(",{}", stock.name) : stock.name);
			}

			std::vector<ordered_json> textures;
			std::vector<ordered_json> constants;
			for (auto layer = 0u; layer < get_stock_layer_count(&stock); layer++)
			{
				if (is_lambert(stock))
				{
					const auto color = get_color_image(asset);
					textures.emplace_back(make_texture("colorMap", color.data(), target::TS_COLOR_MAP, stock.sampler_state));
					textures.emplace_back(make_texture("normalMap", get_image_name(nullptr, "$identitynormalmap"), target::TS_NORMAL_MAP, 1));
					constants.emplace_back(make_constant("colorTint", get_layer_tint(asset, layer)));
					continue;
				}

				const auto suffix = layer ? std::to_string(layer) : std::string();
				const auto color = get_color_specular_image(asset, layer);
				const auto normal = get_normal_image(asset, layer);
				textures.emplace_back(make_texture(("colorMap" + suffix).data(), color.data(), target::TS_COLOR_SPECULAR_MAP, stock.sampler_state));
				textures.emplace_back(make_texture(("normalMap" + suffix).data(), normal.data(), target::TS_NORMAL_OCCLUSSION_GLOSS_MAP, stock.sampler_state));
				constants.emplace_back(make_constant(("colorTint" + suffix).data(), get_layer_tint(asset, layer)));
				if (!layer)
				{
					continue;
				}

				const auto reveal = get_reveal_image(asset, layer);
				textures.emplace_back(make_texture(("specOcclusionMap" + suffix).data(), reveal.data(), target::TS_ALPHA_REVEAL_THICKNESS_MAP, stock.sampler_state));
				constants.emplace_back(make_constant(("revealParams" + suffix).data(), {get_reveal_sharpness(asset, layer), 0.0f, 0.0f, 0.0f}));
			}

			if (uses_alpha_map(stock))
			{
				const auto alpha = get_alpha_image(asset);
				textures.emplace_back(make_texture("specOcclusionMap", alpha.data(), target::TS_ALPHA_REVEAL_THICKNESS_MAP, stock.sampler_state));
			}

			if (&stock == &emissive_model_techset)
			{
				const auto emissive = get_emissive_image(asset);
				textures.emplace_back(make_texture("emissiveMap", emissive.data(), target::TS_COLOR_MAP, stock.sampler_state));
				constants.push_back(make_constant("emissiveParams", {get_emissive_intensity(asset), 0.0f, 0.0f, 0.0f}));
			}

			std::ranges::sort(textures, {}, [](const ordered_json& texture) { return texture["typeHash"].get<std::uint32_t>(); });

			auto constant_table = constants;
			std::ranges::sort(constant_table, {}, [](const ordered_json& constant) { return constant["nameHash"].get<std::uint32_t>(); });

			const auto cull = get_rasterizer_cull(asset);
			auto flags = stock.flags;
			flags.state_flags = get_cull_state_flags(flags.state_flags, cull);

			const std::string name = asset->name;
			auto data = make_header(stock.name, flags, stock.camera_region);
			data["constantTable"] = constant_table;
			data["textureTable"] = textures;

			write_json("materials\\" + name + ".json", data);
			dump_stock_states(stock, name, cull);
			dump_stock_constant_buffer(stock, name, constants, data["constantTable"]);
		}

		bool is_alpha_tested(const Material* asset)
		{
			return asset->cameraRegion == CAMERA_REGION_LIT_OPAQUE && find_texture(asset, 27);
		}

		bool is_emissive(const Material* asset)
		{
			return find_texture(asset, 18) && get_emissive_intensity(asset) > 0.0f;
		}

		const stock_techset* find_stock_techset(const Material* asset, const techset::generated_techset techset_type)
		{
			if (is_hidden(asset) || !has_plain_uv(decode_constants(asset)))
			{
				return nullptr;
			}

			switch (techset_type)
			{
			case techset::GENERATED_TECHSET_MODEL:
				if (asset->cameraRegion == CAMERA_REGION_LIT_DECAL)
				{
					return &decal_model_techset;
				}

				if (asset->cameraRegion == CAMERA_REGION_LIT_TRANS || asset->cameraRegion == CAMERA_REGION_EMISSIVE)
				{
					return &blended_model_techset;
				}

				if (is_alpha_tested(asset))
				{
					return &alpha_tested_model_techset;
				}

				if (is_emissive(asset))
				{
					return &emissive_model_techset;
				}

				return utils::flags::has_flag("pbr") ? &lit_model_techset : &lambert_model_techset;
			case techset::GENERATED_TECHSET_WORLD:
				if (is_alpha_tested(asset))
				{
					return &alpha_tested_world_techset;
				}

				if (!utils::flags::has_flag("pbr"))
				{
					return &lambert_world_techset;
				}

				if (const auto layer_count = get_layer_count(asset); layer_count > 1)
				{
					return layered_world_techsets[layer_count - 2];
				}

				return &lit_world_techset;
			default:
				return nullptr;
			}
		}

		bool is_2d(const Material* asset)
		{
			return asset->techniqueSet && asset->materialType == MTL_TYPE_DEFAULT && find_texture(asset, 158);
		}

		void dump_2d_material(const Material* asset)
		{
			const auto& stock = two_d_techset;
			if (mark_dumped(ASSET_TYPE_TECHNIQUE_SET, stock.name))
			{
				add_csv_row("techset", std::format(",{}", stock.name));
			}

			const auto handle = find_texture(asset, 158);
			const auto image = find_loaded_image(handle);
			if (image)
			{
				gfximage::dump(image);
			}

			const auto tint = find_constant(asset, 126);
			const auto color_tint = tint ? std::bit_cast<std::array<float, 4>>(*tint) : std::array{1.0f, 1.0f, 1.0f, 1.0f};
			const auto constants = std::vector{make_constant("colorTint", color_tint)};

			const std::string name = asset->name;
			auto data = make_header(stock.name, stock.flags, stock.camera_region);
			data["constantTable"] = constants;
			data["textureTable"] = ordered_json::array({make_texture("colorMap", image ? image->name : get_image_name(handle, "$white"), target::TS_2D, stock.sampler_state)});

			write_json("materials\\" + name + ".json", data);
			dump_stock_states(stock, name, target::RASTERIZER_STATE_CULL_NONE);
			dump_stock_constant_buffer(stock, name, constants, data["constantTable"]);
			add_csv_row("material", name);
		}

		const stock_techset* find_stock_techset(const Material* asset)
		{
			const auto techset_type = get_techset(asset);
			return techset_type ? find_stock_techset(asset, *techset_type) : nullptr;
		}
	}

	bool is_supported(const Material* asset)
	{
		return get_techset(asset).has_value() || is_2d(asset);
	}

	bool uses_lightmap(const Material* asset)
	{
		const auto stock = find_stock_techset(asset);
		return stock == &lit_world_techset || stock == &lambert_world_techset || stock == &alpha_tested_world_techset
			|| get_stock_layer_count(stock) > 1;
	}

	unsigned int get_vertex_layer_count(const Material* asset)
	{
		return get_stock_layer_count(find_stock_techset(asset));
	}

	bool dump(Material* asset)
	{
		if (is_2d(asset))
		{
			if (mark_dumped(ASSET_TYPE_MATERIAL, asset->name))
			{
				dump_2d_material(asset);
			}

			return true;
		}

		const auto techset_type = get_techset(asset);
		if (!techset_type)
		{
			return false;
		}

		if (!mark_dumped(ASSET_TYPE_MATERIAL, asset->name))
		{
			return true;
		}

		add_csv_row("material", asset->name);

		if (const auto stock = find_stock_techset(asset, *techset_type))
		{
			dump_stock_material(asset, *stock);
			return true;
		}

		techset::dump_techset(*techset_type);

		const auto is_sky = *techset_type == techset::GENERATED_TECHSET_SKY;
		const auto flags = get_material_flags(asset, *techset_type);
		const std::string techset_name = techset::get_techset_name(*techset_type);
		const std::string name = asset->name;

		auto data = make_header(techset_name, flags, get_camera_region(asset, *techset_type));
		data["constantTable"] = is_sky ? make_sky_constants(asset) : make_constants(decode_constants(asset), techset::get_constant_names(*techset_type));
		data["textureTable"] = is_sky ? make_sky_textures(asset) : make_textures(asset, *techset_type);

		write_json("materials\\" + name + ".json", data);
		dump_states(*techset_type, techset_name, name, flags.state_flags);
		dump_constant_buffer(*techset_type, techset_name, name, data["constantTable"]);

		for (const auto index : get_dumped_texture_indexes(*techset_type))
		{
			if (const auto image = find_loaded_image(find_texture(asset, index)))
			{
				gfximage::dump(image);
			}
		}

		return true;
	}
}
