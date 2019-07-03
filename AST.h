#pragma once

#include <string>
#include <sstream>
#include "Token.h"

class AST
{
public:
	AST() {};
	virtual ~AST() {};

	explicit AST(Token token)
		:
		m_token(token),
		m_value(token.GetValue())
	{
	}
	virtual Token GetToken() const
	{
		return m_token;
	}
	virtual std::string ToString() const
	{
		return m_value;
	}
private:
	Token m_token;
	std::string m_value;
};


class BinOp_AST : public AST
{
public:
	explicit BinOp_AST(const AST *left, const AST *right, const AST *op)
		:
		m_left(left),
		m_right(right),
		m_op(op)
	{
	}

	~BinOp_AST() override
	{
		if (m_left)
			delete m_left;
		if (m_right)
			delete m_right;
		if (m_op)
			delete m_op;
	}

	const AST* GetLeft() const
	{
		return m_left;
	}

	const AST* GetRight() const
	{
		return m_right;
	}

	const AST* GetOp() const
	{
		return m_op;
	}

	Token GetToken() const override
	{
		return Token();
	}

	std::string ToString() const override
	{
		std::ostringstream oss;
		oss << " ( " << m_left->ToString() << " " << m_op->ToString() << " " << m_right->ToString() << " ) ";
		return oss.str();
	}
private:
	const AST *m_left;
	const AST *m_right;
	const AST *m_op;
};