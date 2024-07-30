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

	int string_table_hash(const std::string& string)
	{
		int hash = 0;
		const char* data = string.data();

		while (*data != 0)
		{
			hash = tolower(*data) + (31 * hash);
			data++;
		}

		return hash;
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
