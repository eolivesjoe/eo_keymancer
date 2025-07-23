#pragma once

#include "input.h"
#include <functional>

namespace std 
{
	template <>
	struct hash<input::Input> 
	{
		std::size_t operator()(const input::Input& input) const 
		{
			std::size_t h1 = std::hash<int>()(static_cast<int>(input.type));
			std::size_t h2 = std::hash<int>()(input.code);

			return (h1 << 1) ^ h2;
		}
	};
}