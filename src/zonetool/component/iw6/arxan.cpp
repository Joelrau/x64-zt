#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/iw6/game.hpp"

#include "scheduler.hpp"

#include <utils/hook.hpp>

namespace iw6
{
	namespace arxan
	{
		namespace
		{
			utils::hook::detour nt_close_hook;
			utils::hook::detour nt_query_information_process_hook;

			NTSTATUS WINAPI nt_query_information_process_stub(const HANDLE handle, const PROCESSINFOCLASS info_class,
				const PVOID info,
				const ULONG info_length, const PULONG ret_length)
			{
				auto* orig = static_cast<decltype(NtQueryInformationProcess)*>(nt_query_information_process_hook.
					get_original());
				const auto status = orig(handle, info_class, info, info_length, ret_length);

				if (NT_SUCCESS(status))
				{
					if (info_class == ProcessBasicInformation)
					{
						static DWORD explorer_pid = 0;
						if (!explorer_pid)
						{
							auto* const shell_window = GetShellWindow();
							GetWindowThreadProcessId(shell_window, &explorer_pid);
						}

						static_cast<PPROCESS_BASIC_INFORMATION>(info)->Reserved3 = PVOID(DWORD64(explorer_pid));
					}
					else if (info_class == 30) // ProcessDebugObjectHandle
					{
						*static_cast<HANDLE*>(info) = nullptr;

						return 0xC0000353;
					}
					else if (info_class == 7) // ProcessDebugPort
					{
						*static_cast<HANDLE*>(info) = nullptr;
					}
					else if (info_class == 31)
					{
						*static_cast<ULONG*>(info) = 1;
					}
				}

				return status;
			}

			NTSTATUS NTAPI nt_close_stub(const HANDLE handle)
			{
				char info[16];
				if (NtQueryObject(handle, OBJECT_INFORMATION_CLASS(4), &info, 2, nullptr) >= 0)
				{
					auto* orig = static_cast<decltype(NtClose)*>(nt_close_hook.get_original());
					return orig(handle);
				}

				return STATUS_INVALID_HANDLE;
			}

			LONG WINAPI exception_filter(const LPEXCEPTION_POINTERS info)
			{
				if (info->ExceptionRecord->ExceptionCode == STATUS_INVALID_HANDLE)
				{
					return EXCEPTION_CONTINUE_EXECUTION;
				}

				return EXCEPTION_CONTINUE_SEARCH;
			}

			void hide_being_debugged()
			{
				auto* const peb = PPEB(__readgsqword(0x60));
				peb->BeingDebugged = false;
				*reinterpret_cast<PDWORD>(LPSTR(peb) + 0xBC) &= ~0x70;
			}

			void remove_hardware_breakpoints()
			{
				CONTEXT context;
				ZeroMemory(&context, sizeof(context));
				context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

				auto* const thread = GetCurrentThread();
				GetThreadContext(thread, &context);

				context.Dr0 = 0;
				context.Dr1 = 0;
				context.Dr2 = 0;
				context.Dr3 = 0;
				context.Dr6 = 0;
				context.Dr7 = 0;

				SetThreadContext(thread, &context);
			}

			BOOL WINAPI set_thread_context_stub(const HANDLE thread, CONTEXT* context)
			{
				if (context->ContextFlags == CONTEXT_DEBUG_REGISTERS)
				{
					return TRUE;
				}

				return SetThreadContext(thread, context);
			}
		}

		class component final : public component_interface
		{
		public:
			void* load_import(const std::string& library, const std::string& function) override
			{
				if (function == "SetThreadContext")
				{
					return set_thread_context_stub;
				}

				return nullptr;
			}

			void post_load() override
			{
				hide_being_debugged();
				scheduler::loop(hide_being_debugged, scheduler::pipeline::async);

				const utils::nt::library ntdll("ntdll.dll");
				nt_close_hook.create(ntdll.get_proc<void*>("NtClose"), nt_close_stub);
				nt_query_information_process_hook.create(ntdll.get_proc<void*>("NtQueryInformationProcess"),
					nt_query_information_process_stub);

				AddVectoredExceptionHandler(1, exception_filter);

				remove_hardware_breakpoints();
			}

			void post_unpack() override
			{
				utils::hook::jump(0x1404FE1E0, 0x1404FE2D0); // idk
				utils::hook::jump(0x140558C20, 0x140558CB0); // dwNetPump
				utils::hook::jump(0x140591850, 0x1405918E0); // dwLobbyPump
				utils::hook::jump(0x140589480, 0x140589490); // dwGetLogonStatus

				// Fix arxan crashes
				// Are these opaque predicates?
				utils::hook::nop(0x14AE2B384, 6); // 0000000140035EA7
				utils::hook::nop(0x14A31E98E, 4); // 000000014B1A892E
				utils::hook::nop(0x14A920E10, 4); // 000000014AEF4F39
				utils::hook::nop(0x14A1A2425, 4); // 000000014A0B52A8
				utils::hook::nop(0x14AE07CEA, 4); // 000000014A143BFF
			}
		};
	}
}

REGISTER_COMPONENT_IW6(iw6::arxan::component)