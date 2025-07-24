#include "input.h"
#include "input_utils.h"
#include "../logger/logger.h"

#include <windows.h>

namespace input
{
	bool Input::operator==(const Input& other) const
	{
		return type == other.type && code == other.code && state == other.state;
	}
}