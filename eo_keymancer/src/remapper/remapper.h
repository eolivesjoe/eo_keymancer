#pragma once

#include "../input/input.h"
#include "../input/input_hash.h"

#include <unordered_map>
#include <string>

namespace remapper
{
	class Remapper
	{
	public:
		Remapper(const std::string& configPath);
		bool Load();
		bool HasMapping(const input::Input& input) const;
		input::Input GetMappedKey(const input::Input& input) const;

	private:
		bool IsValidMappingLine(const std::string& line); 
		void ProcessMappingLine(const std::string& line);
		input::Input ParseInput(const std::string& s, input::State state);

	private:
		std::string m_configName;
		std::unordered_map<input::Input, input::Input> m_remaps;
	};
} // namespace remapper