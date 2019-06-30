#pragma once
#include <exception>
#include <string>

namespace E
{
	class InterpreterExecption : std::exception
	{
	private:
		std::string m_msg;

	public:
		explicit InterpreterExecption(const std::string& msg) : m_msg(msg) {};

		virtual const char* what() const override
		{
			return m_msg.c_str();
		}
	};
}
