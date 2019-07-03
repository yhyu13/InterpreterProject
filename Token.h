#pragma once
#include <string>
#include "Templates.h"
/*
Token class
*/
class Token
{
public:
	Token(): m_type("Undefined"),m_value("\0") {};
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

/*
Token types
*/
#define INTEGER "INT"
#define PLUS "PLUS"
#define MINUS "MINUS"
#define MUL "MUL"
#define DIV "DIV"
#define LEFT_PARATHESES "LEFT_PARATHESES"
#define RIGHT_PARATHESES "RIGHT_PARATHESES"

#define __EOF__ "__EOF__"

enum op_code
{
	eUNKNOWN = -1,
	ePLUS,
	eMINUS,
	eMULTIPLY,
	eDIVIDE
};

op_code GetEnumIntOp(std::string const& op)
{
	if (op == PLUS) return ePLUS;
	else if (op == MINUS) return eMINUS;
	else if (op == MUL) return eMULTIPLY;
	else if (op == DIV) return eDIVIDE;
	else
	{
		return eUNKNOWN;
	}
}