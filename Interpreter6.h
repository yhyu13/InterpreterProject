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
	std::string InterpretIntBinOp()
	{
		try
		{
			const AST* root = m_parser.GetIntBinOpAST();

			if (!root)
				return "\0";

			// Condition: is a binary operation
			const BinOp_AST* _root = dynamic_cast<const BinOp_AST*>(root);
			if (_root)
			{
				Token left = InterpretIntBinOpHelper(_root->GetLeft());
				Token right = InterpretIntBinOpHelper(_root->GetRight());
				Token op = InterpretIntBinOpHelper(_root->GetOp());
				return exprBinaryIntOp(left, right, op).GetValue();
			}
			// Condition: is a number
			else
			{
				return InterpretIntBinOpHelper(root).GetValue();
			}
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

	Token InterpretIntBinOpHelper(const AST* root)
	{
		if (!root)
			Error("SyntaxError: root of AST is null.");

		// Condition: is a binary operation
		const BinOp_AST* _root = dynamic_cast<const BinOp_AST*>(root);
		if (_root)
		{
			Token left = InterpretIntBinOpHelper(_root->GetLeft());
			Token right = InterpretIntBinOpHelper(_root->GetRight());
			Token op = InterpretIntBinOpHelper(_root->GetOp());
			return exprBinaryIntOp(left, right, op);
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

