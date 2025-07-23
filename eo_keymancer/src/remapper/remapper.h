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
		Remapper(const std::string& config_path);
		bool load();
		bool hasMapping(const input::Input& input) const;
		input::Input getMappedKey(const input::Input& input) const;

	private:
		bool isValidMappingLine(const std::string& line); 
		void processMappingLine(const std::string& line);
		input::Input parseInput(const std::string& s);

	private:
		std::string config_path;
		std::unordered_map<input::Input, input::Input> remaps;
	};
} // namespace remapper