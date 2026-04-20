#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

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
		// TODO: re-enable zonetool init once we know the proxy DLL loads.
	}
	return TRUE;
}
