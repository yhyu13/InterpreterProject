#pragma once
#include <string>
#include "Execeptions.h"
#include "Token.h"

class Lexer
{
public:
	Lexer() : m_text(""), m_pos(0), m_CurrentChar(0) {};
	explicit Lexer(std::string text) : m_text(text), m_pos(0), m_CurrentChar(text[0])
	{
	}
	virtual ~Lexer() {};

	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void Error(const std::string& msg)
	{
		throw E::InterpreterExecption(msg);
	}

	/*
	Functionality: helper function to match current token type with labeled token type
	*/
	inline void CheckCurrentTokenType(std::string target, std::string label)
	{
		if (target != label)
		{
			Error("SyntaxError: token is type " + target + " but it should be type " + label + ".\n");
		}
	}

	/*
	Funtionality: helper function to advance m_CurrentChar
	*/
	inline void advance_currentChar()
	{
		++m_pos;
		if (m_pos > (m_text.size() - 1))
		{
			m_CurrentChar = '\0';
		}
		else
		{
			m_CurrentChar = m_text[m_pos];
		}
	}

	/*
	Funtionality: helper function to skip whitespace in m_text
	*/
	inline void skip_white_space()
	{
		while (std::isspace(m_CurrentChar) && m_CurrentChar != '\0')
		{
			advance_currentChar();
		}
	}

	/*
	Funtionality: get multi-digits integer from string
	Return: string type integer
	*/
	std::string GetIntegerToken()
	{
		std::string charBuffer = "";
		while (std::isdigit(m_CurrentChar) && m_CurrentChar != '\0')
		{
			charBuffer += T::Str(m_CurrentChar);
			advance_currentChar();
		}
		return charBuffer;
	}

	/*
	Funtionality: tokenize m_text
	Return: an known Token. Otherwise, throw an exception
	*/
	Token GetNextToken()
	{
		while (m_CurrentChar != '\0')
		{
			if (std::isspace(m_CurrentChar))
			{
				skip_white_space();
				continue;
			}
			else if (std::isdigit(m_CurrentChar))
			{
				return Token(INTEGER, GetIntegerToken());
			}
			else if (m_CurrentChar == '+')
			{
				advance_currentChar();
				return Token(PLUS, "+");
			}
			else if (m_CurrentChar == '-')
			{
				advance_currentChar();
				return Token(MINUS, "-");
			}
			else if (m_CurrentChar == '*')
			{
				advance_currentChar();
				return Token(MUL, "*");
			}
			else if (m_CurrentChar == '/')
			{
				advance_currentChar();
				return Token(DIV, "/");
			}
			else if (m_CurrentChar == '(')
			{
				advance_currentChar();
				return Token(LEFT_PARATHESES, "(");
			}
			else if (m_CurrentChar == ')')
			{
				advance_currentChar();
				return Token(RIGHT_PARATHESES, ")");
			}
			else
				// No Token returned, raise excpetion.
			{
				Error("SynatxError: invalid syntax.");
			}
		}
		// Reaching EOF
		return Token(__EOF__, "\0");
	}

private:
	std::string m_text;
	unsigned int m_pos;
	char m_CurrentChar;
};
