#include "remapper.h"
#include "../logger/logger.h"

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
				processMappingLine(line, remaps);
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

		size_t delim = line.find("->");

		return delim != std::string::npos && delim != 0 && delim + 2 < line.size();
	}

	void Remapper::processMappingLine(const std::string& line, std::unordered_map<int, int>& remaps)
	{
		char from = std::tolower(line[0]);
		char to = std::tolower(line[line.find("->") + 2]);

		remaps[VkKeyScanA(from) & 0xFF] = VkKeyScanA(to) & 0xFF;
	}

	bool Remapper::hasMapping(int virtual_key_code) const 
	{
		return remaps.find(virtual_key_code) != remaps.end();
	}

	int Remapper::getMappedKey(int virtual_key_code) const
	{
		auto it = remaps.find(virtual_key_code);
		return it != remaps.end() ? it->second : 0;
	}
}