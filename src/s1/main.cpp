#include <std_include.hpp>
#include "loader/loader.hpp"
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include <utils/string.hpp>
#include <utils/flags.hpp>
#include <utils/io.hpp>

DECLSPEC_NORETURN void WINAPI exit_hook(const int code)
{
	component_loader::pre_destroy();
	exit(code);
}

BOOL WINAPI system_parameters_info_a(const UINT uiAction, const UINT uiParam, const PVOID pvParam, const UINT fWinIni)
{
	component_loader::post_unpack();
	
	return SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
}

FARPROC WINAPI get_proc_address(const HMODULE hModule, const LPCSTR lpProcName)
{
	if (lpProcName == "GlobalMemoryStatusEx"s)
	{
		component_loader::post_unpack();
	}

	return GetProcAddress(hModule, lpProcName);
}

FARPROC load_binary()
{
	loader loader;
	utils::nt::library self;

	loader.set_import_resolver([self](const std::string& library, const std::string& function) -> void*
	{
		if (library == "steam_api64.dll")
		{
			return self.get_proc<FARPROC>(function);
		}
		else if (function == "ExitProcess")
		{
			return exit_hook;
		}
		else if (function == "SystemParametersInfoA")
		{
			return system_parameters_info_a;
		}
		else if (function == "GetProcAddress")
		{
			return get_proc_address;
		}

		return component_loader::load_import(library, function);
	});

	std::string binary = "s1_mp64_ship.exe";

	std::string data;
	if (!utils::io::read_file(binary, &data))
	{
		throw std::runtime_error(utils::string::va(
			"Failed to read game binary (%s)!\nPlease select the correct path in the launcher settings.",
			binary.data()));
	}

	return loader.load_library(binary);
}

void remove_crash_file()
{
	utils::io::remove_file("__s1Exe");
}

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	FARPROC entry_point;

	std::srand(uint32_t(time(nullptr)));

	{
		auto premature_shutdown = true;
		const auto _ = gsl::finally([&premature_shutdown]
		{
			if (premature_shutdown)
			{
				component_loader::pre_destroy();
			}
		});

		try
		{
			remove_crash_file();

			if (!component_loader::post_start()) return 0;

			entry_point = load_binary();
			if (!entry_point)
			{
				throw std::runtime_error("Unable to load binary into memory");
			}

			if (!component_loader::post_load()) return 0;

			premature_shutdown = false;
		}
		catch (std::exception& e)
		{
			MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR);
			return 1;
		}
	}

	return static_cast<int>(entry_point());
}

int __stdcall WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
	return main();
}
