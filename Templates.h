#pragma once
#include <sstream>

namespace T
{
	template <typename T>
	std::string Str(const T& t)
	{
		std::stringstream ss;
		ss << t;
		return ss.str();
	}
}
