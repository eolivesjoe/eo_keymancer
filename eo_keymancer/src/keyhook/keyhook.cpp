#include "keyhook.h"
#include <windows.h>
#include <iostream>

namespace remapper
{
	static Remapper* g_remapper = nullptr;

	LRESULT CALLBACK KeyHook::lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HC_ACTION)
		{
			KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;

			if (wParam == WM_KEYDOWN && g_remapper->hasMapping((int)p->vkCode))
			{
				int new_key = g_remapper->getMappedKey((int)p->vkCode);

				INPUT input = { 0 };
				input.type = INPUT_KEYBOARD;
				input.ki.wVk = new_key;
				SendInput(1, &input, sizeof(INPUT));

				// Swallow original key
				return 1;
			}
		}
		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	KeyHook::KeyHook(Remapper& remapper)
	{
		g_remapper = &remapper;
	}

	void KeyHook::run()
	{
		HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, lowLevelKeyboardProc, nullptr, 0);
		if (!hook)
		{
			std::cerr << "Failed to install hook." << std::endl;
			return;
		}
		std::cout << "Key remapper running. Press ESC to exit." << std::endl;

		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		UnhookWindowsHookEx(hook);
	}
}