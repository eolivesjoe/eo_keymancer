#pragma once

#include <windows.h>
#include "../remapper/remapper.h"

namespace keyHook
{
	class KeyHook
	{
	public:
		KeyHook(remapper::Remapper & remapper);
		void run();

	private:
		static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	private:
		static remapper::Remapper * m_remapper;

	};
} // namespace remapper