#include "remapper.h"
#include "../logger/logger.h"
#include "../input/input_utils.h"

#include <fstream>
#include <iostream>
#include <cctype>
#include <windows.h>

namespace remapper
{
	Remapper::Remapper(const std::string& configName) : m_configName(configName) {}

	bool Remapper::Load()
	{
		std::ifstream file(m_configName);

		if (!file.is_open())
		{
			logger::Error("Failed to open config: " + m_configName);
			return false;
		}

		std::string line;

		while (std::getline(file, line))
		{
			if (IsValidMappingLine(line))
			{
				ProcessMappingLine(line);
			}
		}
		return true;
	}

	bool Remapper::IsValidMappingLine(const std::string& line)
	{
		if (line.empty() || line.rfind("//", 0) == 0)
		{
			return false;
		}

		size_t delim = line.find("--");

		return delim != std::string::npos && delim != 0 && delim + 2 < line.size();
	}

	void Remapper::ProcessMappingLine(const std::string& line)
	{
		size_t delim = line.find("--");

		if (delim == std::string::npos || delim == 0 || delim + 2 >= line.size())
		{
			logger::Warn("this is not a rebind, skipping line...");
			return;
		}

		std::string fromStr = line.substr(0, delim);
		std::string toStr = line.substr(delim + 2);

		for (auto state : { input::State::Down, input::State::Up })
		{
			input::Input from = ParseInput(fromStr, state);
			input::Input to = ParseInput(toStr, state);

			m_remaps[from] = to;
		}


	}

	bool Remapper::HasMapping(const input::Input& input) const
	{
		return m_remaps.find(input) != m_remaps.end();
	}

	input::Input Remapper::GetMappedKey(const input::Input& input) const
	{
		auto it = m_remaps.find(input);
		return it != m_remaps.end() ? it->second : input;
	}

	input::Input Remapper::ParseInput(const std::string& s, input::State state)
	{
		if (s.size() == 1)
		{
			char c = s[0];
			short vk = VkKeyScanA(c);
			return input::Input{ input::Type::Keyboard, vk & 0xFF, state };
		}

		// https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
		if (s.rfind("mouse") == 0)
		{
			if (s == "mouse1") return input::Input{ input::Type::Mouse, input::MOUSE_LEFT, state };
			if (s == "mouse2") return input::Input{ input::Type::Mouse, input::MOUSE_RIGHT, state };
			if (s == "mouse3") return input::Input{ input::Type::Mouse, input::MOUSE_MIDDLE, state };
			if (s == "mouse4") return input::Input{ input::Type::Mouse, input::MOUSE_X1, state };
			if (s == "mouse5") return input::Input{ input::Type::Mouse, input::MOUSE_X2, state };
		}
		else
		{
			if (s == "space") return input::Input{ input::Type::Keyboard, VK_SPACE, state };
			if (s == "lctrl")  return input::Input{ input::Type::Keyboard, VK_LCONTROL, state };
			if (s == "rctrl")  return input::Input{ input::Type::Keyboard, VK_RCONTROL, state };
			if (s == "lshift") return input::Input{ input::Type::Keyboard, VK_LSHIFT, state };
			if (s == "rshift") return input::Input{ input::Type::Keyboard, VK_RSHIFT, state };
			if (s == "lalt") return input::Input{ input::Type::Keyboard, VK_LMENU, state };
			if (s == "ralt") return input::Input{ input::Type::Keyboard, VK_RMENU, state };
			if (s == "caps") return input::Input{ input::Type::Keyboard, VK_CAPITAL, state };
		}

		logger::Warn("unrecognized key in config");
		return input::Input{ input::Type::Keyboard, 0, state };
	}
}