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
	struct Xzone_memory
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

	template <typename T>
	T* get_x_gfx_globals_for_zone(int zone)
	{
		return reinterpret_cast<T*>(get_x_gfx_globals_for_zone(zone));
	}

	WEAK symbol<char*(unsigned int stringValue)> SL_ConvertToString;

	WEAK symbol<XStreamFile> stream_files;
	WEAK symbol<unsigned int> stream_file_index;
}
