#pragma once

#include <string>

namespace input
{
	class Input
	{
	public:
		InputType type;
		int code;

		bool operator==(const Input& other) const;

		Input parse(const std::string& str);

	private:

	};

	enum class InputType
	{
		Keyboard,
		Mouse
	};
} // namespace input