#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <deque>

#include "Templates.h"
#include "Execeptions.h"

/*
Genreal MACRO
*/
//#define DEBUG 1

#ifdef DEBUG 
#define DEBUG_MSG(x) std::cerr << x << std::endl;
#else 
#define DEBUG_MSG(x)
#endif
/*
Token types
*/
#define INTEGER "INTEGER"
#define INTEGER_PLUS "INTEGER_PLUS"
#define INTEGER_MINUS "INTEGER_MINUS"
#define INTEGER_MULTIPLY "INTEGER_MULTIPLY"
#define INTEGER_DIVIDE "INTEGER_DIVIDE"
#define __EOF__ "__EOF__"

enum op_code
{
	eUNKNOWN = -1,
	eINTEGER_PLUS,
	eINTEGER_MINUS,
	eINTEGER_MULTIPLY,
	eINTEGER_DIVIDE
};

op_code GetEnumIntOp(std::string const& op)
{
	if (op == INTEGER_PLUS) return eINTEGER_PLUS;
	else if (op == INTEGER_MINUS) return eINTEGER_MINUS;
	else if (op == INTEGER_MULTIPLY) return eINTEGER_MULTIPLY;
	else if (op == INTEGER_DIVIDE) return eINTEGER_DIVIDE;
	else
	{
		return eUNKNOWN;
	}
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

	std::string ToString() const
	{
		return "Token( " + T::Str(m_type) + ", " + T::Str(m_value) + " )";
	}

	std::string GetType() const
	{
		return m_type;
	}

	std::string GetValue() const
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
	explicit Interpreter(std::string text) : m_text(text), m_pos(0), m_CurrentChar(text[0])
	{
	}
	virtual ~Interpreter() {};

	std::string GetCurrentTokenType()
	{
		return m_CurrentToken.GetType();
	}

/*
Token related helper functions
*/
public:

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
			Error("SyntaxError: token is " + target + " but it should be " + label + ".\n");
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
				return Token(INTEGER_PLUS, "+");
			}
			else if (m_CurrentChar == '-')
			{
				advance_currentChar();
				return Token(INTEGER_MINUS, "-");
			}
			else if (m_CurrentChar == '*')
			{
				advance_currentChar();
				return Token(INTEGER_MULTIPLY, "*");
			}
			else if (m_CurrentChar == '/')
			{
				advance_currentChar();
				return Token(INTEGER_DIVIDE, "/");
			}
			// No Token returned, raise excpetion.
			Error("SynatxError: invalid syntax.");
		}
		// Reaching EOF
		return Token(__EOF__, "\0");
	}

	/*
	Functionality: Simply call GetNextToken
	Return: current token equals __EOF___
	*/
	bool ConsumeToken()
	{
		m_CurrentToken = GetNextToken();
		return m_CurrentToken.GetType() != __EOF__;
	}


/*
Parser queue and Interpreter
*/
public:
	/*
	Functionality: push tokens into the parser queue
	*/
	void fill_parser()
	{
		// Push Tokens into the parser queue
		// Notice the __EOF__ Token is not pushed into the queue!
		while (ConsumeToken())
		{
			DEBUG_MSG("Comsumes "+m_CurrentToken.GetValue())
			m_q.push_back(m_CurrentToken);
		}
	}

	/*
	Functionality: empty parser queue
	*/
	void empty_parser()
	{
		m_q.empty();
	}
	
	/*
	Functionality: get parser queue size
	*/
	int get_parser_size()
	{
		return m_q.size();
	}

	void parser_push_front(const Token& t)
	{
		m_q.push_front(t);
	}

	/*
	Functionality: get parser queue front element and pop front
	*/
	Token parser_pop_front()
	{
		if (m_q.size() != 0)
		{
			Token temp = m_q.front();
			m_q.pop_front();
			return temp;
		}
		else
		{
			return Token(__EOF__, "\0");
		}
	}

	/*
	Functionality: express a basic interger binary operation
	Return: calculated result in string type
	*/
	std::string exprIntOp(const Token& left, const Token& right, const Token& op)
	{
		int _left = std::stoi(left.GetValue());
		int _right = std::stoi(right.GetValue());

		switch (GetEnumIntOp(op.GetType()))
		{
		case eINTEGER_PLUS:
			return T::Str(_left + _right);
		case eINTEGER_MINUS:
			return T::Str(_left - _right);
		case eINTEGER_MULTIPLY:
			return T::Str(_left * _right);
		case eINTEGER_DIVIDE:
			return T::Str(_left / _right);
		default:
			Error("SyntaxError: " + op.ToString() + " is an UNKNOWN integer operation.\n");
		}
	}

	/*
	Functionality: an combnination of parsing and interpreting
	Return: algebraic result of the math expression
	*/
	std::string interpretIntOpStatement()
	{
		try
		{
			Token temp1, temp2, temp3;
			// Parsing
			fill_parser();
			bool first_token_check = true;
			// Interpreting
			while (get_parser_size() > 0)
			{
				temp1 = parser_pop_front();

				if (first_token_check)
				{
					if (temp1.GetType() == INTEGER_MINUS)
					{
						temp2 = parser_pop_front();
						if (temp2.GetType() == INTEGER)
						{
							DEBUG_MSG("First number is negative.")
							parser_push_front(Token(INTEGER, "-" + temp2.GetValue()));
						}
						else
						{
							goto E;
						}
					}
					else if (temp1.GetType() != INTEGER)
					{
						goto E;
					}
					else
					{
						first_token_check = false;
					}
				}

				// Case temp1 is the last element in parser queue
				if (get_parser_size() == 0)
				{
					if (temp1.GetType() == INTEGER)
					{
						return temp1.GetValue();
					}
					else
					{
						goto E;
					}
				}

				// Case read INTEGER in temp1
				if (temp1.GetType() == INTEGER)
				{
					temp2 = parser_pop_front();
					// Case read INTEGER operation in temp2, otherwise Error
					if (GetEnumIntOp(temp2.GetType()) != eUNKNOWN)
					{
						temp3 = parser_pop_front();
						// Case read INTEGER in temp3, otherwise Error
						if (temp3.GetType() == INTEGER)
						{
							DEBUG_MSG("exprIntOp() returns "+exprIntOp(temp1, temp3, temp2));
							parser_push_front(Token(INTEGER, exprIntOp(temp1, temp3, temp2)));
						}
						else
						{
							goto E;
						}
					}
					else
					{
						goto E;
					}
				}
			}
			// Error GOTO destination
			E:Error("SynatxError: invalid syntax.");
		}
		catch (const E::InterpreterExecption & e)
		{
			std::cerr<< e.what() << std::endl;
			return "Error";
		}
		catch (...)
		{
			return "Error";
		}
	}

private:
	std::string m_text;
	unsigned int m_pos;
	Token m_CurrentToken;
	char m_CurrentChar;
	std::deque<Token> m_q;
};

