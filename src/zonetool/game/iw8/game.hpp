#pragma once

#include "structs.hpp"

namespace iw8
{
	namespace game
	{
		extern uint64_t base_address;
		void load_base_address();

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
				return reinterpret_cast<T*>((uint64_t)address_ + base_address);
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

		int Cmd_Argc();
		const char* Cmd_Argv(int index);

		int SV_Cmd_Argc();
		const char* SV_Cmd_Argv(int index);

		const char* Com_GameMode_GetActiveGameModeStr();
		const char* Com_GameMode_GetGameModeStr(GameModeType gameMode);

		void Cbuf_AddText(int clientNum, const char* cmd_text);
	}
}

size_t operator""_b(const size_t ptr);
size_t reverse_b(const size_t ptr);
size_t reverse_b(const void* ptr);

#include "symbols.hpp"