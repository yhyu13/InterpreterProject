/*
AST Parser Interpreter
*/


#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "Parser.h"

class Interpreter
{
public:
	Interpreter()
	{
		m_parser = Parser();
	}
	explicit Interpreter(std::string text)
	{
		m_parser = Parser(text);
	}
	virtual ~Interpreter() {};

	void Reset()
	{
		m_parser.Reset();
	}

	void SetText(std::string text)
	{
		m_parser.SetText(text);
	}

protected:
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

	/*
	Functionality: express a basic interger binary operation
	Return: calculated result in string type
	*/
	Token exprBinaryIntOp(const Token& left, const Token& right, const Token& op)
	{

		if (!(left.GetType() == INTEGER && right.GetType() == INTEGER))
		{
			Error("SyntaxError: " + left.ToString() + " or " + right.ToString() + " is an not a integer or both.\n");
		}

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

public:
	/*
Functionality: an combnination of parsing and interpreting
Return: algebraic result of the math expression
*/
	std::string InterpretIntOp()
	{
		try
		{
			const AST* root = m_parser.GetIntOpAST();
			Token result = InterpretIntOpHelper(root);
			return result.GetValue();
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

protected:

	Token InterpretIntOpHelper(const AST* root)
	{
		if (!root)
			Error("SyntaxError: root of AST is null.");

		// Condition: is a binary operation
		if (const BinOp_AST * root_1 = dynamic_cast<const BinOp_AST*>(root))
		{
			Token left = InterpretIntOpHelper(root_1->GetLeft());
			Token right = InterpretIntOpHelper(root_1->GetRight());
			Token op = InterpretIntOpHelper(root_1->GetOp());
			DEBUG_MSG("Left: " + left.ToString() + " Right: " + right.ToString() + " OP: " + op.ToString());
			return exprBinaryIntOp(left, right, op);
		}
		// Condition: is a unary operation
		else if (const UnaryOp_AST * root_2 = dynamic_cast<const UnaryOp_AST*>(root))
		{
			Token result = InterpretIntOpHelper(root_2->GetExpr());
			if (result.GetType() != INTEGER)
				Error("SyntaxError: unary operator syntax is not valid.");

			// e.g when "---+++1" as a input, the interpreter receives it as reversed by how the parser works.
			// so '1' is the first input, and all '+' signs are ignored by purpose
			// we then iterativly resolve all '-' sign to get the correct expression, which is '-1'
			DEBUG_MSG("Before Unary handle: " + result.ToString());
			if (root_2->GetToken().GetType() == MINUS)
			{
				if (result.GetValue().front() == '-')
				{
					std::string s(1, result.GetValue().back());
					result.SetValue(s);
				}
				else
					result.SetValue(result.GetValue().insert(0, "-"));
			}
			DEBUG_MSG("After Unary handle: " + result.ToString());

			return result;
		}
		// Condition: is a number
		else
		{
			return root->GetToken();
		}
	}

private:
	Parser m_parser;
};

