#pragma once

namespace zonetool
{
	std::uint32_t snd_hash_name(const char* name);
	std::uint32_t string_table_hash(const std::string& string);
	std::uint32_t Com_HashString(const std::string& string);
	std::uint32_t Com_HashStringLower(const std::string& string);
	std::uint32_t Com_HashStringUpper(const std::string& string);
	std::uint32_t DDL_HashString(const char* str, int len = 0);

	void VectorToAngles(const float* vec, float* angles);
	void AxisToAngles(const float axis[3][3], float angles[3]);
	void AngleVectors(const float* angles, float* forward, float* right, float* up);

	namespace QuatInt16
	{
		short ToInt16(const float quat);
		float ToFloat(const short quat);
	}

	namespace half_float
	{
		typedef unsigned short ushort;
		typedef unsigned int uint;

		float half_to_float(const ushort x);
		ushort float_to_half(const float x);
	}

	namespace self_visibility
	{
		uint32_t XSurfacePackSelfVisibility(float* packed);
		void XSurfaceUnpackSelfVisibility(uint32_t src, float* result);
	}

	namespace Byte4
	{
		void Byte4UnpackRgba(float* result, unsigned char* arr);
	}

	namespace PackedVec
	{
		uint32_t Vec2PackTexCoords(float* in);
		void Vec2UnpackTexCoords(const uint32_t in, float* out);
		uint32_t Vec3PackUnitVec_H1(float* in);
		uint32_t Vec3PackUnitVecWithAlpha_H1(float* in, float alpha);
		void Vec3UnpackUnitVec_T6(const uint8_t* in, float* out);
		void Vec3UnpackUnitVec_IW8(const uint32_t in, float* out);
	}

	template<typename T>
	static void compute(T* bounds, float* mins, float* maxs)
	{
		for (int i = 0; i < 3; ++i)
		{
			bounds->halfSize[i] = (maxs[i] - mins[i]) / 2;
			bounds->midPoint[i] = bounds->halfSize[i] + mins[i];
		}
	}
}
