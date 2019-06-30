#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "Templates.h"
#include "Execeptions.h"

/*
Token types
*/
#define INTEGER "INTEGER"
#define PLUS "PLUS"
#define MINUS "MINUS"
#define EOF R"(EOF)"

enum op_code
{
	ePlus,
	eMinus
};

op_code hasHit(std::string const& op)
{
	if (op == PLUS) return ePlus;
	else if (op == MINUS) return eMinus;
}

/*
Token class
*/
class Token
{
public:
	Token() {};
	explicit Token(std::string type, std::string value) : m_type(type), m_value(value) {};
	virtual ~Token() {};

	std::string ToString()
	{
		/*std::stringstream ss;
		ss << "Token( " << m_type << ", " << m_value << ")";
		return ss.str();*/
		return "Token( " + T::Str(m_type) + ", " + T::Str(m_value) + " )";
	}
	
	std::string GetType()
	{
		return m_type;
	}

	std::string GetValue()
	{
		return m_value;
	}

private:
	std::string m_type;
	std::string m_value;
};

class Interpreter
{
public:
	explicit Interpreter(std::string text): m_text(text),m_pos(0)
	{
	}
	virtual ~Interpreter() {};

	inline void Error(const std::string& msg)
	{
		throw E::InterpreterExecption(msg);
	}

	inline void CheckTokenType(std::string target, std::string label)
	{
		if (target != label)
		{
			Error("Token is " + target + "but it should be " + label);
		}
	}

	Token GetNextToken()
	{
		/*Lexical analyzer (also known as scanner or tokenizer)

        This method is responsible for breaking a sentence
        apart into tokens. One token at a time.
		*/

		/*
		# is self.pos index past the end of the self.text ?
        # if so, then return EOF token because there is no more
        # input left to convert into tokens
		*/
		if (m_pos > (m_text.size() - 1))
		{
			return Token(EOF, "");
		}

		/*
		# get a character at the position self.pos and decide
        # what token to create based on the single character
		*/
		char currentChar = m_text[m_pos];
		std::string charBuffer = "";
		
		/*
		Skip white space
		*/
		while (std::isspace(currentChar))
		{
			currentChar = m_text[++m_pos];
		}
		// Double check reaching the end of user text inputs because a user can put white space in the end of inputs
		if (m_pos > (m_text.size() - 1))
		{
			return Token(EOF, "");
		}

		/*
		Return Token
		*/

		// Check for the addition operator
		if (currentChar == '+')
		{
			++m_pos;
			return Token(PLUS, T::Str(currentChar));
		}
		// Check for the subtraction operator
		if (currentChar == '-')
		{
			++m_pos;
			return Token(MINUS, T::Str(currentChar));
		}

		// Check for serial digits
		if (std::isdigit(currentChar))
		{
			while (std::isdigit(currentChar))
			{
				charBuffer += T::Str(currentChar);
				currentChar = m_text[++m_pos];
			}
			return Token(INTEGER, charBuffer);
		}
		
		// No Token returned, raise excpetion.
		Error("GetNextToken() fails to return any KNOWN token.");
	}

	std::string Eat(std::string type)
	{
		m_CurrentToken = GetNextToken();

		if (m_CurrentToken.GetType() == type)
		{
			return type;
		}
		else if (m_CurrentToken.GetType() == EOF)
		{
			Error("Eat() get an unexpected EOF token.");
		}
		else
		{
			return m_CurrentToken.GetType();
		}
	}

	int expr()
	{
		try
		{
			CheckTokenType(Eat(INTEGER),INTEGER);
			Token Left = m_CurrentToken;
			
			std::string OperationType = Eat(PLUS);

			CheckTokenType(Eat(INTEGER), INTEGER);
			Token Right = m_CurrentToken;
		
			switch (hasHit(OperationType))
			{
			case ePlus:
				return std::stoi(Left.GetValue()) + std::stoi(Right.GetValue());
			case eMinus:
				return std::stoi(Left.GetValue()) - std::stoi(Right.GetValue());
			default:
				Error("expr() encounters an UNKNOWN operation");
			}
		
		}
		catch (const E::InterpreterExecption& e)
		{
			std::cout << e.what() << std::endl;
			return -1;
		}
		catch (...)
		{
			return -1;
		}
	}

private:
	std::string m_text;
	unsigned int m_pos;
	Token m_CurrentToken;
};

