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

	input::Input KeyHook::keyboardVkToInput(DWORD vk_code)
	{
		return input::Input{ input::InputType::Keyboard, static_cast<int>(vk_code) };
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