#include "remapper.h"
#include "../logger/logger.h"
#include "../input/input_utils.h"

#include <fstream>
#include <iostream>
#include <cctype>
#include <windows.h>

namespace remapper
{
	Remapper::Remapper(const std::string& config_path) : config_path(config_path) { }

	bool Remapper::load()
	{
		std::ifstream file(config_path);

		if (!file.is_open())
		{
			logger::error("Failed to open config: ");
			logger::error(config_path);
			return false;
		}

		std::string line;

		while (std::getline(file, line))
		{
			if (isValidMappingLine(line))
			{
				processMappingLine(line);
			}
		}
		return true;
	}

	bool Remapper::isValidMappingLine(const std::string& line)
	{
		if (line.empty() || line[0] == '#')
		{
			return false;
		}

		size_t delim = line.find("--");

		return delim != std::string::npos && delim != 0 && delim + 2 < line.size();
	}

	void Remapper::processMappingLine(const std::string& line)
	{
		size_t delim = line.find("--");

		if (delim == std::string::npos || delim == 0 || delim + 2 >= line.size())
		{
			logger::warn("there is an error in the cfg, skipping line...");
			return;
		}

		std::string fromStr = line.substr(0, delim);
		std::string toStr = line.substr(delim + 2);

		input::Input from = parseInput(fromStr);
		input::Input to = parseInput(toStr);


		remaps[from] = to;
	}

	bool Remapper::hasMapping(const input::Input& input) const
	{
		return remaps.find(input) != remaps.end();
	}

	input::Input Remapper::getMappedKey(const input::Input& input) const
	{
		auto it = remaps.find(input);
		return it != remaps.end() ? it->second : input;
	}

	input::Input Remapper::parseInput(const std::string& s)
	{
		if (s.size() == 1)
		{
			char c = s[0];
			short vk = VkKeyScanA(c);
			return input::Input{ input::InputType::Keyboard, vk & 0xFF };
		}

		if (s.rfind("mouse") == 0)
		{
			if (s == "mouse1") return input::Input{ input::InputType::Mouse, input::MOUSE_MIDDLE };
			if (s == "mouse2") return input::Input{ input::InputType::Mouse, input::MOUSE_RIGHT };
			if (s == "mouse3") return input::Input{ input::InputType::Mouse, input::MOUSE_MIDDLE };
			if (s == "mouse4") return input::Input{ input::InputType::Mouse, input::MOUSE_X1 };
			if (s == "mouse5") return input::Input{ input::InputType::Mouse, input::MOUSE_X2 };
		}
		else
		{
			if (s == "space") return input::Input{ input::InputType::Keyboard, VK_SPACE };
			if (s == "ctrl")  return input::Input{ input::InputType::Keyboard, VK_CONTROL };
			if (s == "shift") return input::Input{ input::InputType::Keyboard, VK_SHIFT };
		}

		logger::warn("unrecognized key in cfg");
		return input::Input{ input::InputType::Keyboard, 0 };
	}
}