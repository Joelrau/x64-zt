#pragma once

#include "structs.hpp"

namespace t7
{
	namespace game
	{
		template <typename T>
		class symbol
		{
		public:
			symbol(const size_t address)
				: address_(reinterpret_cast<T*>(address))
			{
			}

			T* get() const
			{
				return reinterpret_cast<T*>(reinterpret_cast<uint64_t>(address_));
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

		game::CmdArgs* Cmd_Args();

		int Cmd_Argc();
		const char* Cmd_Argv(int index);

		int SV_Cmd_Argc();
		const char* SV_Cmd_Argv(int index);
	}
}

#include "symbols.hpp"