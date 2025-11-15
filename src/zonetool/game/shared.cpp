#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "shared.hpp"

namespace zonetool
{
	namespace
	{
		std::unordered_map<std::uint32_t, void*> x_gfx_globals_map;

		namespace iw6
		{
			void initialize()
			{
				DB_FindXAssetHeader.set(0x14031F3A0);
				DB_FindXAssetEntry.set(0x14031F2D0);
				DB_IsXAssetDefault.set(0x1403204D0);

				SL_ConvertToString.set(0x1404317F0);

				stream_files.set(0x141E5BF40);
				stream_file_index.set(0x141E5BF14);
			}
		}

		namespace s1
		{
			void initialize()
			{
				DB_FindXAssetHeader.set(0x14026F0F0);
				DB_FindXAssetEntry.set(0x14026F020);
				DB_IsXAssetDefault.set(0x140270320);

				SL_ConvertToString.set(0x1403F0F10);

				stream_files.set(0x141E57A30);
				stream_file_index.set(0x141E57A04);
			}
		}

		namespace h1
		{
			void initialize()
			{
				DB_FindXAssetHeader.set(0x1402BAC70);
				DB_FindXAssetEntry.set(0x1402BABA0);
				DB_IsXAssetDefault.set(0x1402BC370);

				SL_ConvertToString.set(0x14043B170);

				stream_files.set(0x1432748B0);
				stream_file_index.set(0x143274884);
			}
		}

		namespace h2
		{
			void initialize()
			{
				DB_FindXAssetHeader.set(0x140412F60);
				DB_FindXAssetEntry.set(0x140412E90);
				DB_IsXAssetDefault.set(0x1404143C0);

				SL_ConvertToString.set(0x1405BFBB0);

				stream_files.set(0x1420B2820);
				stream_file_index.set(0x1420B27F4);
			}
		}

		namespace iw7
		{
			void initialize()
			{
				DB_FindXAssetHeader.set(0x140A76E00);
				DB_IsXAssetDefault.set(0x140A780D0);

				SL_ConvertToString.set(0x140C03300);
			}
		}

		namespace t7
		{
			void initialize()
			{
				DB_IsXAssetDefault.set(0x1401D7260);

				SL_ConvertToString.set(0x1401632C0);
			}
		}
	}

	void* get_x_gfx_globals_for_zone(int zone)
	{
		return x_gfx_globals_map[zone];
	}

	void insert_x_gfx_globals_for_zone(int zone, void* globals)
	{
		x_gfx_globals_map[zone] = globals;
	}

	const char* strip_template(const std::string& function_name)
	{
		static char new_string[0x2000] = {0};
		std::memset(new_string, 0, sizeof(new_string));

		for (auto i = 0; i < function_name.size(); i++)
		{
			if (function_name[i] == '<')
			{
				break;
			}

			new_string[i] = function_name[i];
		}

		return new_string;
	}

	std::uint32_t snd_hash_name(const char* name)
	{
		if (!name || !*name)
			return 0;

		std::uint32_t hash = 5381;

		while (*name)
		{
			unsigned char lowercase = static_cast<unsigned char>(std::tolower(static_cast<unsigned char>(*name)));

			hash = 65599 * hash + lowercase;
			++name;
		}

		return hash ? hash : 1;
	}

	std::uint32_t Com_HashString(const std::string& string)
	{
		std::uint32_t hash = 0;

		for (const char* p = string.data(); *p; ++p)
		{
			int c = static_cast<unsigned char>(*p);
			hash = hash * 31 + c;
		}

		return hash;
	}

	std::uint32_t Com_HashStringLower(const std::string& string)
	{
		std::uint32_t hash = 0;

		for (const char* p = string.data(); *p; ++p)
		{
			int c = std::tolower(static_cast<unsigned char>(*p));
			hash = hash * 31 + c;
		}

		return hash;
	}

	std::uint32_t Com_HashStringUpper(const std::string& string)
	{
		std::uint32_t hash = 0;

		for (const char* p = string.data(); *p; ++p)
		{
			int c = std::toupper(static_cast<unsigned char>(*p));
			hash = hash * 31 + c;
		}

		return hash;
	}

	std::uint32_t string_table_hash(const std::string& string)
	{
		return Com_HashStringLower(string);
	}

	std::uint32_t DDL_HashString(const char* str, int len)
	{
		if (!str)
			return 0;

		std::uint32_t hash = 0;
		const char* p = str;
		const char* end = (len > 0) ? str + len : nullptr;

		while (*p)
		{
			if (end && p >= end)
				break;

			hash = hash * 31 + static_cast<unsigned char>(*p);
			++p;
		}

		return hash;
	}

	void VectorToAngles(const float* vec, float* angles)
	{
		const float rad_to_deg = 57.295776f;
		const float x = vec[0];
		const float y = vec[1];
		const float z = vec[2];

		float pitch = 0.0f;
		float yaw = 0.0f;

		if (x == 0.0f && y == 0.0f)
		{
			yaw = 0.0f;
			pitch = (z > 0.0f) ? 270.0f : 90.0f;
		}
		else
		{
			yaw = std::atan2f(y, x) * rad_to_deg;
			if (yaw < 0.0f)
				yaw += 360.0f;

			const float forward = std::sqrtf(x * x + y * y);

			pitch = std::atan2f(z, forward) * -rad_to_deg;
			if (pitch < 0.0f)
				pitch += 360.0f;
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0.0f;
	}

	void AxisToAngles(const float axis[3][3], float angles[3])
	{
		VectorToAngles(reinterpret_cast<const float*>(axis), angles);

		float yawRad = angles[1] * -0.0174533f;
		float sinYaw = sinf(yawRad), cosYaw = cosf(yawRad);

		float forwardX = axis[1][0], forwardY = axis[1][1], forwardZ = axis[1][2];
		float newX = (cosYaw * forwardX) - (sinYaw * forwardY);
		float newY = (cosYaw * forwardY) + (sinYaw * forwardX);

		float pitchRad = angles[0] * -0.0174533f;
		float sinPitch = sinf(pitchRad), cosPitch = cosf(pitchRad);
		float rotatedZ = (forwardZ * sinPitch) + (newX * cosPitch);
		float rotatedX = (forwardZ * cosPitch) - (newX * sinPitch);

		float rollAngle = (newY == 0.0f && rotatedZ == 0.0f)
			? ((rotatedX < 0.0f) ? -90.0f : 90.0f)
			: atan2f(rotatedX, sqrtf(newY * newY + rotatedZ * rotatedZ)) * -57.295776f;

		angles[2] = (newY >= 0.0f) ? -rollAngle : ((rollAngle >= 0.0f) ? (-180.0f + rollAngle) : (180.0f + rollAngle));
	}

	void AngleVectors(const float* angles, float* forward, float* right, float* up)
	{
		const float deg_to_rad = 0.017453292f;
		const float pitch = angles[0] * deg_to_rad;
		const float yaw = angles[1] * deg_to_rad;
		const float roll = angles[2] * deg_to_rad;

		const float sp = std::sinf(pitch);
		const float cp = std::cosf(pitch);
		const float sy = std::sinf(yaw);
		const float cy = std::cosf(yaw);

		if (forward)
		{
			forward[0] = cp * cy;
			forward[1] = cp * sy;
			forward[2] = -sp;
		}

		if (right || up)
		{
			const float sr = std::sinf(roll);
			const float cr = std::cosf(roll);

			if (right)
			{
				right[0] = cr * sy - sr * sp * cy;
				right[1] = -cr * cy - sr * sp * sy;
				right[2] = -sr * cp;
			}

			if (up)
			{
				up[0] = cr * sp * cy + sr * sy;
				up[1] = cr * sp * sy - sr * cy;
				up[2] = cr * cp;
			}
		}
	}

	namespace QuatInt16
	{
		short ToInt16(const float quat)
		{
			return static_cast<short>(quat * INT16_MAX);
		}

		float ToFloat(const short quat)
		{
			return static_cast<float>(quat) / static_cast<float>(INT16_MAX);
		}
	}

	namespace half_float
	{
		uint as_uint(const float x) {
			return *(uint*)&x;
		}
		float as_float(const uint x) {
			return *(float*)&x;
		}

		float half_to_float(const ushort x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
			const uint e = (x & 0x7C00) >> 10; // exponent
			const uint m = (x & 0x03FF) << 13; // mantissa
			const uint v = as_uint((float)m) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
			return as_float((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))); // sign : normalized : denormalized
		}
		ushort float_to_half(const float x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
			const uint b = as_uint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
			const uint e = (b & 0x7F800000) >> 23; // exponent
			const uint m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
			return (ushort)((b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF); // sign : normalized : denormalized : saturate
		}
	}

	namespace self_visibility
	{
		// Packing function
		uint32_t XSurfacePackSelfVisibility(float* packed)
		{
			return (uint32_t)(float)((float)(fminf(fmaxf((float)(*packed + 1.0f) * 0.5f, 0.0f), 1.0f) * 511.0f) + 0.5f) |
				(((((int)(float)((float)(fminf(fmaxf(packed[2], 0.0f), 1.0f) * 127.0f) + 0.5f) |
					((int)(float)((float)(fminf(fmaxf(packed[3], 0.0f), 1.0f) * 127.0f) + 0.5f) << 7)) << 9) |
					(unsigned int)(int)(float)((float)(fminf(fmaxf((float)(packed[1] + 1.0f) * 0.5f, 0.0f), 1.0f) * 511.0f) + 0.5f)) << 9);
		}

		// Unpacking function
		void XSurfaceUnpackSelfVisibility(uint32_t src, float* result)
		{
			result[2] = static_cast<float>((src >> 18) & 0x7F) * 0.0078740157f;
			result[3] = static_cast<float>((src >> 25) & 0x7F) * 0.0078740157f;
			result[0] = (static_cast<float>(src & 0x1FF) * 0.0019569471f * 2.0f) - 1.0f;
			result[1] = (static_cast<float>((src >> 9) & 0x1FF) * 0.0019569471f * 2.0f) - 1.0f;
		}
	}

	namespace Byte4
	{
		void Byte4UnpackRgba(float* result, unsigned char* arr)
		{
			result[0] = arr[0] * 0.003921568859368563f;
			result[1] = arr[1] * 0.003921568859368563f;
			result[2] = arr[2] * 0.003921568859368563f;
			result[3] = arr[3] * 0.003921568859368563f;
		}
	}

	namespace PackedVec
	{
		uint32_t Vec2PackTexCoords(float* in)
		{
			__m128 xmm0 = _mm_loadu_ps(in);
			__m128i xmmi0 = _mm_cvtps_ph(xmm0, 3);
			uint32_t result = _mm_cvtsi128_si32(xmmi0);
			return result;
		}

		void Vec2UnpackTexCoords(const uint32_t in, float* out) // iw5 func
		{
			uint32_t val;

			if (LOWORD(in))
				val = ((LOWORD(in) & 0x8000) << 16) | (((((in & 0x3FFF) << 14) - (~(LOWORD(in) << 14) & 0x10000000)) ^ 0x80000001) >> 1);
			else
				val = 0;

			out[0] = *reinterpret_cast<float*>(&val);

			if (HIWORD(in))
				val = ((HIWORD(in) & 0x8000) << 16) | (((((HIWORD(in) & 0x3FFF) << 14)
					- (~(HIWORD(in) << 14) & 0x10000000)) ^ 0x80000001) >> 1);
			else
				val = 0;

			out[1] = *reinterpret_cast<float*>(&val);
		}

		uint32_t Vec3PackUnitVec_H1(float* in) // h1 func
		{
			uint32_t bits;

			bits = ((uint32_t)floor(((((fmaxf(-1.0f, fminf(1.0f, in[2])) + 1.0f) * 0.5f) * 1023.0f) + 0.5f)) | 0xFFFFFC00) << 10;
			bits = ((uint32_t)floor(((((fmaxf(-1.0f, fminf(1.0f, in[1])) + 1.0f) * 0.5f) * 1023.0f) + 0.5f)) | bits) << 10;
			bits = ((uint32_t)floor(((((fmaxf(-1.0f, fminf(1.0f, in[0])) + 1.0f) * 0.5f) * 1023.0f) + 0.5f)) | bits);

			return bits;
		}

		uint32_t Vec3PackUnitVecWithAlpha_H1(float* in, float alpha) // h1 func
		{
			uint32_t bits;

			bits = ((uint32_t)floor(((((fmaxf(-1.0f, fminf(1.0f, alpha)) + 1.0f) * 0.5f) * 1023.0f) + 0.5f)) | 0xFFFFFC00) << 10;
			bits = ((uint32_t)floor(((((fmaxf(-1.0f, fminf(1.0f, in[2])) + 1.0f) * 0.5f) * 1023.0f) + 0.5f)) | bits) << 10;
			bits = ((uint32_t)floor(((((fmaxf(-1.0f, fminf(1.0f, in[1])) + 1.0f) * 0.5f) * 1023.0f) + 0.5f)) | bits) << 10;
			bits = ((uint32_t)floor(((((fmaxf(-1.0f, fminf(1.0f, in[0])) + 1.0f) * 0.5f) * 1023.0f) + 0.5f)) | bits);

			return bits;
		}

		void Vec3UnpackUnitVec_T6(const uint8_t* in, float* out) // t6 func
		{
			float decodeScale;

			decodeScale = (in[3] - -192.0f) / 32385.0f;
			out[0] = (in[0] - 127.0f) * decodeScale;
			out[1] = (in[1] - 127.0f) * decodeScale;
			out[2] = (in[2] - 127.0f) * decodeScale;
		}

		void Vec3UnpackUnitVec_IW8(const uint32_t in, float* out) // iw8 func
		{
			// Constants based on the original code
			const float g_packMask1010102[4] = { 0x3FF, 0x3FF, 0x3FF, 0.0f }; // Mask for unpacking
			const float g_packAdd1010102[4] = { 0.5f, 0.5f, 0.5f, 0.0f };     // Addition after unpacking
			const float g_packMul1010102[4] = { 2.0f, 2.0f, 2.0f, 1.0f };     // Multiplication factor after unpacking

			// Unpack the packed value
			uint32_t packedValue = in;

			// Apply mask and convert to float
			float unpacked[3];
			unpacked[0] = static_cast<float>(packedValue & 0x3FF);
			unpacked[1] = static_cast<float>((packedValue >> 10) & 0x3FF);
			unpacked[2] = static_cast<float>((packedValue >> 20) & 0x3FF);

			// Add the constant value (equivalent to adding g_packAdd1010102)
			unpacked[0] += g_packAdd1010102[0];
			unpacked[1] += g_packAdd1010102[1];
			unpacked[2] += g_packAdd1010102[2];

			// Multiply by the scale factor (equivalent to multiplying by g_packMul1010102)
			unpacked[0] *= g_packMul1010102[0];
			unpacked[1] *= g_packMul1010102[1];
			unpacked[2] *= g_packMul1010102[2];

			// Assign the result to the output vector
			out[0] = unpacked[0];
			out[1] = unpacked[1];
			out[2] = unpacked[2];
		}
	}

	class shared final : public component_interface
	{ 
	public:
		void post_start() override
		{
			select(iw6::initialize, s1::initialize, h1::initialize, h2::initialize, iw7::initialize, t7::initialize)();
		}
	};

	REGISTER_COMPONENT(shared);
}
