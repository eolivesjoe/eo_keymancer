#include "keyhook.h"
#include "worker.h"
#include "../input/InputUtils.h"
#include "../logger/logger.h"

#include <iostream>

namespace keyhook
{
	remapper::Remapper* KeyHook::m_remapper = nullptr;
	std::atomic<bool> KeyHook::m_keymancerEnabled = false;
	std::atomic<bool> KeyHook::m_running = true;


	KeyHook::KeyHook(remapper::Remapper& remapper)
	{
		m_remapper = &remapper;
	}

	LRESULT CALLBACK KeyHook::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HC_ACTION)
		{
			KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;

			if (p->flags & LLKHF_INJECTED)
			{
				return CallNextHookEx(nullptr, nCode, wParam, lParam);
			}

			input::Input realInput = KeyboardVkToInput(p->vkCode, wParam);

			logger::Info("vkCode: " + std::to_string(p->vkCode));

			if (m_keymancerEnabled && m_remapper->HasMapping(realInput))
			{
				input::Input mapping = m_remapper->GetMappedKey(realInput);

				INPUT fakeInput = { 0 };
				fakeInput.type = INPUT_KEYBOARD;
				fakeInput.ki.wVk = mapping.code;

				if (mapping.state == input::State::Down)
				{
					fakeInput.ki.dwFlags = 0;
				}
				else
				{
					fakeInput.ki.dwFlags = KEYEVENTF_KEYUP;
				}

				Worker::QueueInput(fakeInput);
				return 1;
			}
		}
		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	input::Input KeyHook::KeyboardVkToInput(DWORD vkCode, WPARAM wParam)
	{
		input::State state;

		switch (wParam)
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

		return input::Input{ input::Type::Keyboard, static_cast<int>(vkCode), state };
	}


	LRESULT CALLBACK KeyHook::MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HC_ACTION)
		{
			MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam;

			if (p->flags & LLMHF_INJECTED) 
			{
				return CallNextHookEx(nullptr, nCode, wParam, lParam);
			}

			input::Input realInput = MouseWParamToInput(wParam);

			if (m_keymancerEnabled && m_remapper->HasMapping(realInput))
			{
				input::Input mapping = m_remapper->GetMappedKey(realInput);

				INPUT fakeInput = { 0 };
				fakeInput.type = INPUT_MOUSE;

				switch (mapping.code)
				{
				case input::MOUSE_LEFT:
					fakeInput.mi.dwFlags = (mapping.state == input::State::Down)
						? MOUSEEVENTF_LEFTDOWN
						: MOUSEEVENTF_LEFTUP;
					break;

				case input::MOUSE_RIGHT:
					fakeInput.mi.dwFlags = (mapping.state == input::State::Down)
						? MOUSEEVENTF_RIGHTDOWN
						: MOUSEEVENTF_RIGHTUP;
					break;

				case input::MOUSE_MIDDLE:
					fakeInput.mi.dwFlags = (mapping.state == input::State::Down)
						? MOUSEEVENTF_MIDDLEDOWN
						: MOUSEEVENTF_MIDDLEUP;
					break;

				case input::MOUSE_X1:
				case input::MOUSE_X2:
					fakeInput.mi.dwFlags = (mapping.state == input::State::Down)
						? MOUSEEVENTF_XDOWN
						: MOUSEEVENTF_XUP;

					fakeInput.mi.mouseData = (mapping.code == input::MOUSE_X1)
						? XBUTTON1
						: XBUTTON2;
					break;

				default:
					break;
				}
				Worker::QueueInput(fakeInput);
				return 1;
			}
		}
		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	input::Input KeyHook::MouseWParamToInput(WPARAM wParam)
	{
		switch (wParam)
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

	void KeyHook::Run()
	{
		HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, nullptr, 0);

		if (!keyboardHook)
		{
			logger::Error("failed to install keyboard hook...");
			return;
		}

		HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)MouseProc, nullptr, 0);

		if (!mouseHook)
		{
			logger::Error("failed to install mouse hook...");
			return;
		}

		RegisterHotKey(nullptr, 1, 0, VK_HOME);
		RegisterHotKey(nullptr, 2, 0, VK_DELETE);

		logger::Info("press HOME to toggle rebind...");
		logger::Info("press DEL twice to exit...");

		logger::Info("keymancer running...");

		MSG msg;

		while (GetMessage(&msg, nullptr, 0, 0) && m_running)
		{
			if (msg.message == WM_HOTKEY && msg.wParam == 1) 
			{
				m_keymancerEnabled = !m_keymancerEnabled;
				m_keymancerEnabled ? logger::Info("keymancer enabled...") : logger::Info("keymancer disabled...");
			}

			if (msg.message == WM_HOTKEY && msg.wParam == 2)
			{
				m_running = false;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		UnhookWindowsHookEx(mouseHook);
		UnhookWindowsHookEx(keyboardHook);

		UnregisterHotKey(nullptr, 1);
		UnregisterHotKey(nullptr, 2);
	}
}