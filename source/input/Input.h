#pragma once

#include <string>

namespace input
{
	enum class Type
	{
		Keyboard,
		Mouse
	};

	enum class State
	{
		Down,
		Up
	};

	class Input
	{
	public:
		Type type;
		int code;
		State state;

		bool operator==(const Input& other) const;

	private:

	};
} // namespace input