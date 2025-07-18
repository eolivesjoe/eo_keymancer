#pragma once

#include <windows.h>
#include "../remapper/remapper.h"

namespace remapper
{
	class KeyHook
	{
	public:
		KeyHook(Remapper& remapper);
		void run();

	private:
		static LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

	private:
		static Remapper* m_remapper;

	};
} // namespace remapper