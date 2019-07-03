#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <stack>

#include "Debug.h"
#include "Templates.h"
#include "Execeptions.h"
#include "Token.h"
#include "Lexer.h"
#include "AST.h"

class Parser
{
public:
	Parser():m_pAST(nullptr) {};
	explicit Parser(std::string text) : m_text(text), m_pAST(nullptr)
	{
	}
	virtual ~Parser()
	{
		if (m_pAST)
			delete m_pAST;
	}
	void Reset()
	{
		if (m_pAST)
			delete m_pAST;
		m_pAST = nullptr;
		m_text = "\0";
	}
	void SetText(std::string text)
	{
		m_text = text;
	}


protected:
	/*
	Funtionality: helper function to throw exception with a specific message
	*/
	inline void Error(const std::string& msg)
	{
		throw E::InterpreterExecption(msg);
	}

	inline void PtrError(void* ptr,const std::string& msg)
	{
		if (!ptr)
			throw E::InterpreterExecption(msg);
	}

	/*
	Token related helper functions
	*/

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
		1st level of the Int Op expression:
		Handles integer/paratheses
	*/
	const AST* GetFactor()
	{
		Token token = m_CurrentToken;
		std::string token_code[3] = { INTEGER, LEFT_PARATHESES, RIGHT_PARATHESES };

		if (token.GetType() == token_code[0])
		{
			ConsumeTokenType(token_code[0]);
			return (const AST*)new AST(token);
		}
		else if (token.GetType() == token_code[1])
		{
			ConsumeTokenType(token_code[1]);
			const AST* result2 = GetExpr();
			m_pAST = result2;
			ConsumeTokenType(token_code[2]);
			return result2;
		}
		else
		{
			Error("SynatxError: invalid syntax.");
		}
	}

	/*
		2nd level of the Int Op expression:
		Handles integer mul/div
	*/
	const AST* GetTerm()
	{
		const AST* temp = nullptr;
		const AST* result = GetFactor();
		m_pAST = result;
		std::string token_code[2] = { MUL, DIV };

		while ((m_CurrentToken.GetType() == token_code[0]) ||
			(m_CurrentToken.GetType() == token_code[1]))
		{
			Token token = m_CurrentToken;
			if (token.GetType() == token_code[0])
			{
				ConsumeTokenType(token_code[0]);
				temp = GetFactor();
				result = DEBUG_NEW BinOp_AST(result, temp, DEBUG_NEW AST(token));
				m_pAST = result;
			}
			else if (token.GetType() == token_code[1])
			{
				ConsumeTokenType(token_code[1]);
				temp = GetFactor();
				result = DEBUG_NEW BinOp_AST(result, temp, DEBUG_NEW AST(token));
				m_pAST = result;
			}
		}
		return result;
	}

	/*
		3rd level of the Int Op expression:
		Handles integer plus/minus
	*/
	const AST* GetExpr()
	{
		const AST* temp = nullptr;
		const AST* result = GetTerm();
		m_pAST = result;
		std::string token_code[2] = { PLUS, MINUS };

		while ((m_CurrentToken.GetType() == token_code[0]) ||
			(m_CurrentToken.GetType() == token_code[1]))
		{
			Token token = m_CurrentToken;
			if (token.GetType() == token_code[0])
			{
				ConsumeTokenType(token_code[0]);
				temp = GetTerm();
				result = DEBUG_NEW BinOp_AST(result, temp, DEBUG_NEW AST(token));
				m_pAST = result;
			}
			else if (token.GetType() == token_code[1])
			{
				ConsumeTokenType(token_code[1]);
				temp = GetTerm();
				result = DEBUG_NEW BinOp_AST(result, temp, DEBUG_NEW AST(token));
				m_pAST = result;
			}
		}
		return result;
	}

public:
	/*
	Functionality: an combnination of parsing and interpreting
	Return: algebraic result of the math expression
	*/
	const AST* GetIntBinOpAST()
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
			m_pAST = GetExpr();
			return m_pAST;
		}
		catch (const E::InterpreterExecption& e)
		{
			std::cerr << e.what() << std::endl;
			return nullptr;
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			return nullptr;
		}
	}

private:
	std::string m_text;
	Lexer m_lexer;
	Token m_CurrentToken;
	const AST* m_pAST;
};

