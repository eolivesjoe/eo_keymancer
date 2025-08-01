#pragma once

#include "../remapper/remapper.h"

#include <windows.h>
#include <atomic>

static remapper::Remapper* m_remapper;

namespace keyHook
{
	class KeyHook
	{
	public:
		KeyHook(remapper::Remapper& remapper);
		void run();

	private:
		static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
		static input::Input keyboardVkToInput(DWORD vk_code, WPARAM w_param);

		static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);
		static input::Input mouseWParamToInput(WPARAM w_param);

	private:
		static remapper::Remapper* m_remapper;
		static std::atomic<bool> m_keymancer_enabled;
		static std::atomic<bool> m_running;
	};
} // namespace keyHook