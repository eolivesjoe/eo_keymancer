#pragma once

#include <windows.h>
#include "../remapper/remapper.h"

namespace keyHook
{
	class KeyHook
	{
	public:
		KeyHook(remapper::Remapper& remapper);
		void run();

	private:
		static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
		static input::Input keyboardVkToInput(DWORD vk_code);

		static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);
		static input::Input mouseWParamToInput(WPARAM w_param);

	private:
		static remapper::Remapper* m_remapper;

	};
} // namespace remapper