#include <std_include.hpp>
#include "crash.hpp"

#include <DbgHelp.h>

#pragma comment(lib, "dbghelp.lib")

namespace zonetool::cli
{
	namespace
	{
		constexpr auto max_frames = 32;

		std::string describe_address(const HANDLE process, const DWORD64 address)
		{
			alignas(SYMBOL_INFO) char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME]{};
			const auto symbol = reinterpret_cast<SYMBOL_INFO*>(buffer);
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol->MaxNameLen = MAX_SYM_NAME;

			DWORD64 displacement{};
			if (!SymFromAddr(process, address, &displacement, symbol))
			{
				return std::format("0x{:X}", address);
			}

			IMAGEHLP_LINE64 line{.SizeOfStruct = sizeof(IMAGEHLP_LINE64)};
			DWORD line_displacement{};
			if (!SymGetLineFromAddr64(process, address, &line_displacement, &line))
			{
				return std::format("{}+0x{:X}", symbol->Name, displacement);
			}

			return std::format("{} ({}:{})", symbol->Name, line.FileName, line.LineNumber);
		}

		LONG WINAPI print_crash(EXCEPTION_POINTERS* exception)
		{
			std::fflush(stdout);

			const auto process = GetCurrentProcess();
			const auto thread = GetCurrentThread();
			SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
			SymInitialize(process, nullptr, TRUE);

			std::fprintf(stderr, "crash: exception 0x%08lX\n", exception->ExceptionRecord->ExceptionCode);

			auto context = *exception->ContextRecord;
			STACKFRAME64 frame{};
			frame.AddrPC = {context.Rip, 0, AddrModeFlat};
			frame.AddrFrame = {context.Rbp, 0, AddrModeFlat};
			frame.AddrStack = {context.Rsp, 0, AddrModeFlat};

			for (auto i = 0; i < max_frames; i++)
			{
				if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &frame, &context, nullptr,
					SymFunctionTableAccess64, SymGetModuleBase64, nullptr) || !frame.AddrPC.Offset)
				{
					break;
				}

				std::fprintf(stderr, "  %s\n", describe_address(process, frame.AddrPC.Offset).data());
			}

			std::fflush(stderr);
			return EXCEPTION_EXECUTE_HANDLER;
		}
	}

	void install_crash_handler()
	{
		SetUnhandledExceptionFilter(print_crash);
	}
}
