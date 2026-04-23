#include <std_include.hpp>

#include "game/mode.hpp"
#include "game/iw8/game.hpp"
#include "loader/component_loader.hpp"

#include <utils/hook.hpp>
#include <utils/nt.hpp>

namespace
{
	template <typename T>
	T get_proxy_export(const char* lib_name, const char* export_name)
	{
		char dir[MAX_PATH]{0};
		GetSystemDirectoryA(dir, sizeof(dir));

		std::string path = std::string(dir) + "\\" + lib_name;
		const HMODULE lib = LoadLibraryA(path.c_str());
		if (!lib) return nullptr;

		return reinterpret_cast<T>(GetProcAddress(lib, export_name));
	}

	BOOL WINAPI system_parameters_info_a(const UINT uiAction, const UINT uiParam, const PVOID pvParam, const UINT fWinIni)
	{
		static bool has_ran_unpack = false;
		if (!has_ran_unpack)
		{
			try
			{
				component_loader::post_unpack();
			}
			catch (const std::exception& e)
			{
				MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR);
				std::exit(1);
			}

			has_ran_unpack = true;
		}

		return SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
	}

	DWORD WINAPI init_thread(LPVOID)
	{
		::iw8::game::load_base_address();

		game::set_mode(game::iw8);

		component_loader::sort();

		if (!component_loader::post_start())
		{
			return 0;
		}

		auto* system_parameters_info = utils::nt::library{}.get_iat_entry("user32.dll", "SystemParametersInfoA");
		if (!system_parameters_info)
		{
			MessageBoxA(nullptr, "could not find import SystemParametersInfoA", "ERROR", MB_ICONERROR);
		}
		else
		{
			utils::hook::set(system_parameters_info, system_parameters_info_a);
		}

		if (!component_loader::post_load())
		{
			return 0;
		}

		return 0;
	}
}

extern "C" {
	__declspec(dllexport) DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, void* pCapabilities)
	{
		using func_t = DWORD(WINAPI*)(DWORD, DWORD, void*);
		static auto func = get_proxy_export<func_t>("XInput9_1_0.dll", "XInputGetCapabilities");
		if (!func) return ERROR_DEVICE_NOT_CONNECTED;
		return func(dwUserIndex, dwFlags, pCapabilities);
	}

	__declspec(dllexport) DWORD WINAPI XInputSetState(DWORD dwUserIndex, void* pVibration)
	{
		using func_t = DWORD(WINAPI*)(DWORD, void*);
		static auto func = get_proxy_export<func_t>("XInput9_1_0.dll", "XInputSetState");
		if (!func) return ERROR_DEVICE_NOT_CONNECTED;
		return func(dwUserIndex, pVibration);
	}

	__declspec(dllexport) DWORD WINAPI XInputGetState(DWORD dwUserIndex, void* pState)
	{
		using func_t = DWORD(WINAPI*)(DWORD, void*);
		static auto func = get_proxy_export<func_t>("XInput9_1_0.dll", "XInputGetState");
		if (!func) return ERROR_DEVICE_NOT_CONNECTED;
		return func(dwUserIndex, pState);
	}
}

BOOL WINAPI DllMain(HMODULE, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		CreateThread(nullptr, 0, init_thread, nullptr, 0, nullptr);
	}
	return TRUE;
}
