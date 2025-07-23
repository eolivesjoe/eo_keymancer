#include "keyhook.h"
#include "../input/input_utils.h"
#include "../logger/logger.h"

#include <windows.h>
#include <iostream>

namespace keyHook
{
	remapper::Remapper* m_remapper = nullptr;

	KeyHook::KeyHook(remapper::Remapper& remapper)
	{
		m_remapper = &remapper;
	}

	LRESULT CALLBACK KeyHook::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HC_ACTION)
		{
			KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
			input::Input incoming_input = keyboardVkToInput(p->vkCode);

			if (m_remapper->hasMapping(incoming_input))
			{
				input::Input mapped_key = m_remapper->getMappedKey(incoming_input);

				INPUT new_input = { 0 };
				new_input.type = INPUT_KEYBOARD;
				new_input.ki.wVk = mapped_key.code;

				if (wParam == WM_KEYDOWN || WM_SYSKEYDOWN)
				{
					new_input.ki.dwFlags = 0;
				}
				else if (wParam == WM_KEYUP || WM_SYSKEYUP)\
				{
					new_input.ki.dwFlags = KEYEVENTF_KEYUP;
				}
				else
				{
					return CallNextHookEx(nullptr, nCode, wParam, lParam);
				}

				SendInput(1, &new_input, sizeof(INPUT));
				return 1;
			}
		}
		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	input::Input KeyHook::keyboardVkToInput(DWORD vk_code)
	{
		return input::Input{ input::InputType::Keyboard, static_cast<int>(vk_code) };
	}


	LRESULT CALLBACK KeyHook::mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HC_ACTION)
		{
			MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam;

			switch (wParam)
			{
			case WM_LBUTTONDOWN:
				// Example: Remap left-click to right-click
				INPUT input = { 0 };
				input.type = INPUT_MOUSE;
				input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
				SendInput(1, &input, sizeof(INPUT));
				return 1;
			}
		}
		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	input::Input KeyHook::mouseWParamToInput(WPARAM w_param)
	{
		using namespace input;
		switch (w_param)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			return Input{ InputType::Mouse, MOUSE_LEFT };
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			return Input{ InputType::Mouse, MOUSE_RIGHT };
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			return Input{ InputType::Mouse, MOUSE_MIDDLE };
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
			return Input{ InputType::Mouse, MOUSE_X1 };
		default:
			return Input{ InputType::Keyboard, 0 };
		}
	}

	void KeyHook::run()
	{
		HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProc, nullptr, 0);

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