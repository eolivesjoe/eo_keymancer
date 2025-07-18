#pragma once

#include <unordered_map>
#include <string>

namespace remapper
{
	class Remapper
	{
	public:
		Remapper(const std::string& config_path);
		bool load();
		bool hasMapping(int virtual_key_code) const;
		int getMappedKey(int virtual_key_code) const;

	private:
		bool isValidMappingLine(const std::string& line); 
		void processMappingLine(const std::string& line, std::unordered_map<int, int>& remaps);

	private:
		std::string config_path;
		std::unordered_map<int, int> remaps;
	};
} // namespace remapper