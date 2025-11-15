#pragma once

#define WEAK __declspec(selectany)

#include "mode.hpp"

namespace zonetool
{
#pragma pack(push, 1)
	struct XFileHeader
	{
		char header[8];
		std::uint32_t version;
		std::uint8_t compress;
		std::uint8_t compressType;
		std::uint8_t sizeOfPointer;
		std::uint8_t sizeOfLong;
		std::uint32_t fileTimeHigh;
		std::uint32_t fileTimeLow;
		std::uint32_t imageCount;
		std::uint64_t baseFileLen;
		std::uint64_t totalFileLen;
	};

	template <std::size_t num_streams>
	struct XZoneMemory
	{
		std::uint64_t size;
		std::uint64_t externalsize;
		std::uint64_t streams[num_streams];
	};

	struct XStreamFile
	{
		std::uint16_t isLocalized;
		std::uint16_t fileIndex;
		char pad[4];
		std::uint64_t offset;
		std::uint64_t offsetEnd;
	};

	struct DB_AuthSignature
	{
		unsigned char bytes[256];
	};

	struct DB_AuthHash
	{
		unsigned char bytes[32];
	};

	struct XPakHeader
	{
		char header[8];
		std::int32_t version;
		unsigned char unknown[16];
		DB_AuthHash hash;
		DB_AuthSignature signature;
	};
#pragma pack(pop)

	enum XFileBlock
	{
		XFILE_BLOCK_TEMP = 0x0,
		XFILE_BLOCK_PHYSICAL = 0x1,
		XFILE_BLOCK_RUNTIME = 0x2,
		XFILE_BLOCK_VIRTUAL = 0x3,
		XFILE_BLOCK_LARGE = 0x4,
		XFILE_BLOCK_CALLBACK = 0x5,
		XFILE_BLOCK_SCRIPT = 0x6,
		MAX_XFILE_COUNT = 0x7,
	};

	template <typename T>
	class symbol
	{
	public:
		symbol()
			: address_(reinterpret_cast<T*>(0))
		{
		}

		symbol(const size_t address)
			: address_(reinterpret_cast<T*>(address))
		{
		}

		T* get() const
		{
			return reinterpret_cast<T*>(
				reinterpret_cast<size_t>(address_));
		}

		void set(const size_t address)
		{
			this->address_ = reinterpret_cast<T*>(address);
		}

		operator T* () const
		{
			return this->get();
		}

		T* operator->() const
		{
			return this->get();
		}

	private:
		T* address_;
	};

	template <typename ...Args, size_t N = sizeof...(Args)>
	auto select(Args... values)
	{
		static_assert(N == game::game_mode::count);
		using T = std::common_type_t<Args...>;
		const std::array<T, N> v = {values...};
		const auto mode = game::get_mode();
		assert(mode >= 0);
		return v[mode];
	}

	void* get_x_gfx_globals_for_zone(int zone);
	void insert_x_gfx_globals_for_zone(int zone, void* globals);

	const char* strip_template(const std::string& function_name);

	template <typename T>
	T* get_x_gfx_globals_for_zone(int zone)
	{
		return reinterpret_cast<T*>(get_x_gfx_globals_for_zone(zone));
	}

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

	WEAK symbol<void*(int type, const char* name, int createDefault)> DB_FindXAssetHeader;
	WEAK symbol<void*(int type, const char* name)> DB_FindXAssetEntry;
	WEAK symbol<bool(int type, const char* name)> DB_IsXAssetDefault;

	WEAK symbol<char*(unsigned int stringValue)> SL_ConvertToString;

	WEAK symbol<XStreamFile> stream_files;
	WEAK symbol<unsigned int> stream_file_index;

	template <typename T>
	T db_find_x_asset_header(int type, const char* name, int create_default)
	{
		return static_cast<T>(DB_FindXAssetHeader(type, name, create_default));
	}

	template <typename T>
	T* db_find_x_asset_entry(int type, const char* name)
	{
		return reinterpret_cast<T*>(DB_FindXAssetEntry(type, name));
	}

	// First version of the function template
	template<typename H, typename E>
	auto db_find_x_asset_header_safe_impl(int type, const std::string& name) -> decltype(std::declval<E>().asset.header, H{}) 
	{
		const auto asset_entry = db_find_x_asset_entry<E>(type, name.data());

		if (asset_entry) 
		{
			return asset_entry->asset.header;
		}

		return db_find_x_asset_header<H>(type, name.data(), 1);
	}

	// Second version of the function template
	template<typename H, typename E>
	auto db_find_x_asset_header_safe_impl(int type, const std::string& name) -> decltype(std::declval<E>().header, H{}) 
	{
		const auto asset_entry = db_find_x_asset_entry<E>(type, name.data());

		if (asset_entry) 
		{
			return asset_entry->header;
		}

		return db_find_x_asset_header<H>(type, name.data(), 1);
	}

	template<typename H, typename E>
	H db_find_x_asset_header_safe(int type, const std::string& name) 
	{
		return db_find_x_asset_header_safe_impl<H, E>(type, name);
	}
}
