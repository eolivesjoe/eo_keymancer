#include "keyhook.h"
#include "../logger/logger.h"

#include <windows.h>
#include <iostream>

namespace remapper
{
	Remapper* remapper::KeyHook::m_remapper = nullptr;

	KeyHook::KeyHook(Remapper& remapper)
	{
		m_remapper = &remapper;
	}

	LRESULT CALLBACK KeyHook::lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HC_ACTION)
		{
			KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;

			if (wParam == WM_KEYDOWN && m_remapper->hasMapping((int)p->vkCode))
			{
				int new_key = m_remapper->getMappedKey((int)p->vkCode);

				INPUT input = { 0 };
				input.type = INPUT_KEYBOARD;
				input.ki.wVk = new_key;
				SendInput(1, &input, sizeof(INPUT));

				return 1;
			}
		}
		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	void KeyHook::run()
	{
		HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, lowLevelKeyboardProc, nullptr, 0);

		if (!hook)
		{
			logger::error("failed to install hook...");
			return;
		}
		logger::info("keymancer running...");

		MSG msg;

		while (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		UnhookWindowsHookEx(hook);
	}
}