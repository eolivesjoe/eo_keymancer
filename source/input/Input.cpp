#include "Input.h"
#include "InputUtils.h"
#include "../logger/Logger.h"

#include <windows.h>

namespace input
{
	bool Input::operator==(const Input& other) const
	{
		return type == other.type && code == other.code && state == other.state;
	}
}