#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <stack>

#include "Templates.h"
#include "Execeptions.h"
#include "Token.h"
#include "Lexer.h"

/*
MACRO
*/
#define DEBUG 1

#ifdef DEBUG 
#define DEBUG_MSG(x) std::cerr << x << std::endl;
#else 
#define DEBUG_MSG(x)
#endif

class Interpreter
{
public:
	Interpreter() {};
	explicit Interpreter(std::string text) : m_text(text)
	{
	}
	virtual ~Interpreter() {};

	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void Error(const std::string& msg)
	{
		throw E::InterpreterExecption(msg);
	}

	/*
	Token related helper functions
	*/
public:

	/*
	Functionality: Simply call GetNextToken
	Return: current token equals __EOF___
	*/
	bool ConsumeToken()
	{
		m_CurrentToken = m_lexer.GetNextToken();
		return m_CurrentToken.GetType() != __EOF__;
	}

	/*
	Functionality: Simply call GetNextToken
	Return: current token equals __EOF___
	*/
	bool ConsumeTokenType(std::string type)
	{
		
		if (m_CurrentToken.GetType() == type)
		{
			m_CurrentToken = m_lexer.GetNextToken();
			return m_CurrentToken.GetType() != __EOF__;
		}
		else
		{
			Error("SynatxError: invalid syntax.");
		}
	}

	/*
	Functionality: express a basic interger binary operation
	Return: calculated result in string type
	*/
	Token exprBinaryIntOp(const Token& left, const Token& right, const Token& op)
	{
		int _left = std::stoi(left.GetValue());
		int _right = std::stoi(right.GetValue());

		switch (GetEnumIntOp(op.GetType()))
		{
		case ePLUS:
			return Token(INTEGER, T::Str(_left + _right));
		case eMINUS:
			return Token(INTEGER, T::Str(_left - _right));
		case eMULTIPLY:
			return Token(INTEGER, T::Str(_left * _right));
		case eDIVIDE:
			if (_right != 0)
			{
				return Token(INTEGER, T::Str(_left / _right));
			}
			else
			{
				Error("SyntaxError: Integer division by zero.");
			}
		default:
			Error("SyntaxError: " + op.ToString() + " is an UNKNOWN integer operation.\n");
		}
	}

	/*
		1st level of the Int Op expression:
		Handles integer/paratheses
	*/
	Token GetFactor()
	{
		Token result = m_CurrentToken;
		Token token = m_CurrentToken;
		std::string token_code[3] = { INTEGER, LEFT_PARATHESES, RIGHT_PARATHESES };

		if (token.GetType() == token_code[0])
		{
			ConsumeTokenType(token_code[0]);
		}
		else if (token.GetType() == token_code[1])
		{
			ConsumeTokenType(token_code[1]);
			result = GetExpr();
			ConsumeTokenType(token_code[2]);
		}
		else
		{
			Error("SynatxError: invalid syntax.");
		}
		return result;
	}

	/*
		2nd level of the Int Op expression:
		Handles integer mul/div
	*/
	Token GetTerm()
	{
		Token result = GetFactor();
		std::string token_code[2] = { MUL, DIV };

		while ((m_CurrentToken.GetType() == token_code[0]) ||
			(m_CurrentToken.GetType() == token_code[1]))
		{
			Token token = m_CurrentToken;
			if (token.GetType() == token_code[0])
			{
				ConsumeTokenType(token_code[0]);
				result = exprBinaryIntOp(result, GetFactor(), token);
			}
			else if (token.GetType() == token_code[1])
			{
				ConsumeTokenType(token_code[1]);
				result = exprBinaryIntOp(result, GetFactor(), token);
			}
		}
		return result;
	}

	/*
		3rd level of the Int Op expression:
		Handles integer plus/minus
	*/
	Token GetExpr()
	{
		Token result = GetTerm();
		std::string token_code[2] = { PLUS, MINUS };

		while ((m_CurrentToken.GetType() == token_code[0]) ||
			(m_CurrentToken.GetType() == token_code[1]))
		{
			Token token = m_CurrentToken;
			if (token.GetType() == token_code[0])
			{
				ConsumeTokenType(token_code[0]);
				result = exprBinaryIntOp(result, GetTerm(), token);
			}
			else if (token.GetType() == token_code[1])
			{
				ConsumeTokenType(token_code[1]);
				result = exprBinaryIntOp(result, GetTerm(), token);
			}
		}
		return result;
	}

	/*
	Functionality: an combnination of parsing and interpreting
	Return: algebraic result of the math expression
	*/
	std::string interpretIntOpStatement()
	{
		try
		{
		START:
			m_lexer = Lexer(m_text);
			m_CurrentToken = m_lexer.GetNextToken();
			// Special Case: Negative sign as the first token. Solved by adding an "0" at the begnining
			if (m_CurrentToken.GetType() == MINUS)
			{
				m_text.insert(0, "0");
				goto START;
			}

			return GetExpr().GetValue();
		}
		catch (const E::InterpreterExecption& e)
		{
			std::cerr << e.what() << std::endl;
			return "\0";
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			return "\0";
		}
	}

private:
	std::string m_text;
	Lexer m_lexer;
	Token m_CurrentToken;
};

