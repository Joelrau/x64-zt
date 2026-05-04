#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/iw8/game.hpp"

#include <utils/thread.hpp>

namespace iw8
{
	namespace console
	{
		static volatile bool ingame = false;
		static volatile bool exit_flag = false;

		HWND input_window = nullptr;
		HWND input_edit = nullptr;
		WNDPROC orig_edit_proc = nullptr;

		void Cbuf_AddText(int clientNum, const char* cmd_text)
		{
			auto& buf = game::s_cmd_textArray[clientNum];
			constexpr int CRITSECT_CBUF = 35;

			const int len = (int)strlen(cmd_text);
			const bool needsNewline = (len == 0 || cmd_text[len - 1] != '\n');
			const int total = len + (needsNewline ? 1 : 0);

			game::Sys_EnterCriticalSection(CRITSECT_CBUF);

			if (buf.cmdsize + total <= buf.maxsize)
			{
				char* dst = (char*)buf.data + buf.cmdsize;
				memcpy(dst, cmd_text, len);

				if (needsNewline) dst[len] = '\n';

				buf.cmdsize += total;
			}
			else
			{
				printf("Cbuf_AddText: overflow (adding '%s')\n", cmd_text);
			}

			game::Sys_LeaveCriticalSection(CRITSECT_CBUF);
		}

		void submit(const char* text)
		{
			if (!ingame || !text || !text[0])
			{
				return;
			}

			std::string cmd = text;
			cmd.append("\n");
			Cbuf_AddText(0, cmd.data());
		}

		LRESULT CALLBACK edit_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{
			if (msg == WM_CHAR && wparam == VK_RETURN)
			{
				char buf[1024]{};
				GetWindowTextA(hwnd, buf, sizeof(buf));
				submit(buf);
				SetWindowTextA(hwnd, "");
				return 0;
			}

			return CallWindowProcA(orig_edit_proc, hwnd, msg, wparam, lparam);
		}

		LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{
			switch (msg)
			{
			case WM_CREATE:
			{
				RECT rc;
				GetClientRect(hwnd, &rc);
				input_edit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
					WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
					4, 4, rc.right - 8, rc.bottom - 8,
					hwnd, nullptr, GetModuleHandleA(nullptr), nullptr);

				orig_edit_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(
					input_edit, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(edit_proc)));

				SetFocus(input_edit);
				return 0;
			}

			case WM_SIZE:
				if (input_edit)
				{
					RECT rc;
					GetClientRect(hwnd, &rc);
					MoveWindow(input_edit, 4, 4, rc.right - 8, rc.bottom - 8, TRUE);
				}
				return 0;

			case WM_SETFOCUS:
				if (input_edit) SetFocus(input_edit);
				return 0;

			case WM_DESTROY:
				input_window = nullptr;
				return 0;
			}

			return DefWindowProcA(hwnd, msg, wparam, lparam);
		}

		// this creates a temporary tiny cbuf window since cbuf is being weird in console with another mod loaded
		DWORD WINAPI console(LPVOID)
		{
			WNDCLASSA wc{};
			wc.lpfnWndProc = window_proc;
			wc.hInstance = GetModuleHandleA(nullptr);
			wc.lpszClassName = "IW8ZoneToolConsole";
			wc.hCursor = LoadCursorA(nullptr, IDC_ARROW);
			wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
			RegisterClassA(&wc);

			input_window = CreateWindowExA(0, wc.lpszClassName, "IW8-ZoneTool",
				WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
				CW_USEDEFAULT, CW_USEDEFAULT, 520, 80,
				nullptr, nullptr, wc.hInstance, nullptr);

			if (!input_window)
			{
				return 1;
			}

			ShowWindow(input_window, SW_SHOW);

			MSG msg;
			while (!exit_flag && GetMessageA(&msg, nullptr, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessageA(&msg);
			}

			return 0;
		}

		class component final : public component_interface
		{
		public:
			void post_start() override
			{
				const auto handle = CreateThread(0, 0, console, 0, 0, 0);
				utils::thread::set_name(handle, "Console");
			}

			void post_unpack() override
			{
				ingame = true;
			}

			void pre_destroy() override
			{
				ingame = false;
				exit_flag = true;
				if (input_window)
				{
					PostMessageA(input_window, WM_CLOSE, 0, 0);
				}
			}
		};
	}
}

REGISTER_COMPONENT_IW8(iw8::console::component)
