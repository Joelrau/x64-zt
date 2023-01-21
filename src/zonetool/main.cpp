#include <std_include.hpp>
#include "loader/loader.hpp"
#include "loader/component_loader.hpp"

#include <utils/string.hpp>
#include <utils/flags.hpp>
#include <utils/io.hpp>

#include "game/mode.hpp"

namespace h1
{
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

	void get_old_exe(std::string* binary, std::string* data)
	{
		const auto patched_binary = "h1_mp64_zt.exe"s;

		try
		{
			if (!utils::io::file_exists(patched_binary) && !utils::io::write_file(patched_binary, *data, false))
			{
				throw std::runtime_error("Could not write file");
			}
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error(
				utils::string::va("Could not create game binary for %s! %s",
					binary->data(), e.what())
			);
		}

		*binary = patched_binary;
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

			return component_loader::load_import(library, function);
		});

		std::string binary = "h1_mp64_ship.exe";

		std::string data;
		if (!utils::io::read_file(binary, &data))
		{
			throw std::runtime_error(utils::string::va(
				"Failed to read game binary (%s)!\nPlease copy the h1-zonetool.exe into your Call of Duty: Modern Warfare Remastered installation folder and run it from there.",
				binary.data()));
		}

		data = utils::nt::load_resource(GAME_BINARY_EXE);
		if (data.empty())
		{
			throw std::runtime_error("Something went terribly wrong while loading the game binary...");
		}

		get_old_exe(&binary, &data);
		return loader.load_library(binary);
	}

	void remove_crash_file()
	{
		utils::io::remove_file("__h1Exe");
	}

	int main()
	{
		ShowWindow(GetConsoleWindow(), SW_HIDE);

		FARPROC entry_point;

		srand(uint32_t(time(nullptr)));
		remove_crash_file();

		{
			auto premature_shutdown = true;
			const auto _ = gsl::finally([&premature_shutdown]()
			{
				if (premature_shutdown)
				{
					component_loader::pre_destroy();
				}
			});

			try
			{
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
}

namespace h2
{
	DECLSPEC_NORETURN void WINAPI exit_hook(const int code)
	{
		component_loader::pre_destroy();
		exit(code);
	}

	BOOL WINAPI system_parameters_info_a(const UINT uiAction, const UINT uiParam, const PVOID pvParam, const UINT fWinIni)
	{
		return SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
	}

	FARPROC WINAPI get_proc_address(const HMODULE hModule, const LPCSTR lpProcName)
	{
		if (lpProcName == "InitializeCriticalSectionEx"s)
		{
			component_loader::post_unpack();
		}

		return GetProcAddress(hModule, lpProcName);
	}

	void apply_aslr_patch(std::string* data)
	{
		// sp binary
		if (data->size() != 0xE1E0C8)
		{
			throw std::runtime_error("File size mismatch, bad game files");
		}

		auto* dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(&data->at(0));
		auto* nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>(&data->at(dos_header->e_lfanew));
		auto* optional_header = &nt_headers->OptionalHeader;

		if (optional_header->DllCharacteristics & IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE)
		{
			optional_header->DllCharacteristics &= ~(IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE);
		}
	}

	void get_aslr_patched_binary(std::string* binary, std::string* data)
	{
		const auto patched_binary = "h2_sp_patched.exe"s;

		try
		{
			apply_aslr_patch(data);
			if (!utils::io::file_exists(patched_binary) && !utils::io::write_file(patched_binary, *data, false))
			{
				throw std::runtime_error("Could not write file");
			}
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error(
				utils::string::va("Could not create aslr patched binary for %s! %s",
					binary->data(), e.what())
			);
		}

		*binary = patched_binary;
	}

	FARPROC load_binary()
	{
		loader loader;
		utils::nt::library self;

		loader.set_import_resolver([self](const std::string& library, const std::string& function) -> void*
		{
			if (function == "ExitProcess")
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

		std::string binary = "MW2CR.exe";
		if (!utils::io::file_exists(binary))
		{
			binary = "h2_sp64_bnet_ship.exe";
		}

		std::string data;
		if (!utils::io::read_file(binary, &data))
		{
			throw std::runtime_error(utils::string::va(
				"Failed to read game binary (%s)!\nPlease copy the h2-mod.exe into your Call of Duty: Modern Warfare 2 Campaign Remastered installation folder and run it from there.",
				binary.data()));
		}

		get_aslr_patched_binary(&binary, &data);
		return loader.load_library(binary);
	}

	void remove_crash_file()
	{
		utils::io::remove_file("__h2Exe");
	}

	int main()
	{
		ShowWindow(GetConsoleWindow(), SW_HIDE);

		FARPROC entry_point;

		srand(uint32_t(time(nullptr)));
		remove_crash_file();

		{
			auto premature_shutdown = true;
			const auto _ = gsl::finally([&premature_shutdown]()
			{
				if (premature_shutdown)
				{
					component_loader::pre_destroy();
				}
			});

			try
			{
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
}

namespace s1
{
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
}

int main()
{
	int result = 1;
	if (utils::io::file_exists("h1_mp64_ship.exe"))
	{
		game::set_mode(game::game_mode::h1);
		result = h1::main();
	}
	else if (utils::io::file_exists("MW2CR.exe") || utils::io::file_exists("h2_sp64_bnet_ship.exe"))
	{
		game::set_mode(game::game_mode::h2);
		result = h2::main();
	}
	else if (utils::io::file_exists("s1_mp64_ship.exe"))
	{
		game::set_mode(game::game_mode::s1);
		result = s1::main();
	}
	else
	{
		MessageBoxA(nullptr, "no game binary found!\nmake sure your zonetool.exe is in the game directory!", "NO BINARY", MB_ICONERROR);
	}
	return result;
}

int __stdcall WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
	return main();
}
