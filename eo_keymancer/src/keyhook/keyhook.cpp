#include "keyhook.h"
#include "worker.h"
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
			input::Input real_input = keyboardVkToInput(p->vkCode, wParam);

			if (m_remapper->hasMapping(real_input))
			{
				input::Input mapping = m_remapper->getMappedKey(real_input);

				INPUT fake_input = { 0 };
				fake_input.type = INPUT_KEYBOARD;
				fake_input.ki.wVk = mapping.code;

				if (mapping.state == input::State::Down)
				{
					fake_input.ki.dwFlags = 0;
				}
				else
				{
					fake_input.ki.dwFlags = KEYEVENTF_KEYUP;
				}

				Worker::queueInput(fake_input);
				return 1;
			}
		}
		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	input::Input KeyHook::keyboardVkToInput(DWORD vk_code, WPARAM w_param)
	{
		input::State state;

		switch (w_param)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			state = input::State::Down;
			break;

		case WM_KEYUP:
		case WM_SYSKEYUP:
			state = input::State::Up;
			break;

		default:
			state = input::State::Down;
			break;
		}

		return input::Input{ input::Type::Keyboard, static_cast<int>(vk_code), state };
	}


	LRESULT CALLBACK KeyHook::mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HC_ACTION)
		{
			MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam;
			input::Input real_input = mouseWParamToInput(wParam);

			if (m_remapper->hasMapping(real_input))
			{
				input::Input mapping = m_remapper->getMappedKey(real_input);

				INPUT fake_input = { 0 };
				fake_input.type = INPUT_MOUSE;

				switch (mapping.code)
				{
				case input::MOUSE_LEFT:
					fake_input.mi.dwFlags = (mapping.state == input::State::Down)
						? MOUSEEVENTF_LEFTDOWN
						: MOUSEEVENTF_LEFTUP;
					break;

				case input::MOUSE_RIGHT:
					fake_input.mi.dwFlags = (mapping.state == input::State::Down)
						? MOUSEEVENTF_RIGHTDOWN
						: MOUSEEVENTF_RIGHTUP;
					break;

				case input::MOUSE_MIDDLE:
					fake_input.mi.dwFlags = (mapping.state == input::State::Down)
						? MOUSEEVENTF_MIDDLEDOWN
						: MOUSEEVENTF_MIDDLEUP;
					break;

				case input::MOUSE_X1:
				case input::MOUSE_X2:
					fake_input.mi.dwFlags = (mapping.state == input::State::Down)
						? MOUSEEVENTF_XDOWN
						: MOUSEEVENTF_XUP;

					fake_input.mi.mouseData = (mapping.code == input::MOUSE_X1)
						? XBUTTON1
						: XBUTTON2;
					break;

				default:
					break;
				}
				Worker::queueInput(fake_input);
				return 1;
			}
		}
		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	input::Input KeyHook::mouseWParamToInput(WPARAM w_param)
	{
		switch (w_param)
		{
		case WM_LBUTTONDOWN:
			return input::Input{ input::Type::Mouse, input::MOUSE_LEFT, input::State::Down };
		case WM_LBUTTONUP:
			return input::Input{ input::Type::Mouse, input::MOUSE_LEFT, input::State::Up };
		case WM_RBUTTONDOWN:
			return input::Input{ input::Type::Mouse, input::MOUSE_RIGHT, input::State::Down };
		case WM_RBUTTONUP:
			return input::Input{ input::Type::Mouse, input::MOUSE_RIGHT, input::State::Up };
		case WM_MBUTTONDOWN:
			return input::Input{ input::Type::Mouse, input::MOUSE_MIDDLE, input::State::Down };
		case WM_MBUTTONUP:
			return input::Input{ input::Type::Mouse, input::MOUSE_MIDDLE, input::State::Up };
		case WM_XBUTTONDOWN:
			return input::Input{ input::Type::Mouse, input::MOUSE_X1, input::State::Down };
		case WM_XBUTTONUP:
			return input::Input{ input::Type::Mouse, input::MOUSE_X1, input::State::Up };
		default:
			return input::Input{ input::Type::Keyboard, 0, input::State::Down };
		}
	}

	void KeyHook::run()
	{
		HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)keyboardProc, nullptr, 0);

		if (!keyboardHook)
		{
			logger::error("failed to install keyboard hook...");
			return;
		}

		HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)mouseProc, nullptr, 0);

		if (!mouseHook)
		{
			logger::error("failed to install mouse hook...");
			return;
		}

		logger::info("keymancer running...");

		MSG msg;

		while (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		UnhookWindowsHookEx(mouseHook);
		UnhookWindowsHookEx(keyboardHook);
	}
}