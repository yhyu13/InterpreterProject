#pragma once

#include <string>
#include "Token.h"

class AST
{
public:
	AST() {};
	virtual ~AST() {};
};


class Number_AST : public AST
{
public:
	Number_AST(Token token)
		:
		m_token(token),
		m_value(token.GetValue())
	{
	}
private:
	Token m_token;
	std::string m_value;
};

class BinOp_AST : public AST
{
public:
	BinOP_AST(Token left, Token right, Token, op)
		:
		m_left(left),
		m_right(right),
		m_op(op)
	{
	}
private:
	Token m_left;
	Token m_right;
	Token m_op;
};