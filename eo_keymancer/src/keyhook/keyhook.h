#pragma once

#include "../remapper/remapper.h"

#include <windows.h>
#include <atomic>

static remapper::Remapper* m_remapper;

namespace key_hook
{
	class KeyHook
	{
	public:
		KeyHook(remapper::Remapper& remapper);
		void Run();

	private:
		static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
		static input::Input KeyboardVkToInput(DWORD vkCode, WPARAM wCaram);

		static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
		static input::Input MouseWParamToInput(WPARAM wParam);

	private:
		static remapper::Remapper* m_remapper;
		static std::atomic<bool> m_keymancerEnabled;
		static std::atomic<bool> m_running;
	};
} // namespace key_hook