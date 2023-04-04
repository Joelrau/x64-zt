#pragma once

#define WEAK __declspec(selectany)

#include "mode.hpp"

namespace zonetool
{
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
}
