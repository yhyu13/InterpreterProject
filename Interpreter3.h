#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <stack>

#include "Templates.h"
#include "Execeptions.h"

/*
Genreal MACRO
*/
#define DEBUG 1

#ifdef DEBUG 
#define DEBUG_MSG(x) std::cerr << x << std::endl;
#else 
#define DEBUG_MSG(x)
#endif
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
	std::vector<Token> fill_parser()
	{
		std::vector<Token> parser;
		// Push Tokens into the parser queue
		// Notice the __EOF__ Token is not pushed into the queue!
		while (ConsumeToken())
		{
			DEBUG_MSG("Comsumes " + m_CurrentToken.GetValue());
			parser.push_back(m_CurrentToken);
		}
		return parser;
	}

	/*
	Functionality: copy parser from begin to end
	*/
	std::vector<Token> copy_parser(const std::vector<Token>& orig_parser, unsigned int new_begin, unsigned int new_end)
	{
		if (new_end < orig_parser.size())
		{
			return std::vector<Token>(orig_parser.begin() + new_begin, orig_parser.begin() + new_end);
		}
		else
		{
			Error("copy_parser() : new_end parameter exceeds the size of orig_parser.");
		}
	}

	/*
	Functionality: get parser queue size
	*/
	int get_parser_size(const std::vector<Token>& parser)
	{
		return (int)parser.size();
	}

	/*
	Functionality: insert a token into the parser.
	*/
	void parser_insert_token(std::vector<Token>& parser, unsigned int pos,const Token & token)
	{
		parser.insert(parser.begin() + pos, token);
	}

	/*
	Functionality: erase a token from the parser.
	*/
	void parser_erase_token(std::vector<Token>& parser, unsigned int pos)
	{
		parser.erase(parser.begin() + pos);
	}

	/*
	Functionality: erase a range of tokens from the parser.
	*/
	void parser_erase_tokens_range(std::vector<Token>& parser, unsigned int begin, unsigned int end)
	{
		parser.erase(parser.begin() + begin, parser.begin() + end);
	}

	/*
	Functionality: get parser queue front element and pop front
	*/
	Token parser_pop_front(std::vector<Token>& parser)
	{
		if (parser.size() != 0)
		{
			Token temp = parser[0];
			parser_erase_token(parser,0);
			return temp;
		}
		else
		{
			Error("parser_pop_front() tries to pop an empty container.");
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
	Funtionality: find paratheses and recursively solve them
	*/
	void exprParatheses(std::vector<Token>& orig_parser)
	{
		int begin = -1;
		int end = -1;
		std::stack<int> s;

		start:
		for (int i = 0; i<get_parser_size(orig_parser); i++)
		{
			if (orig_parser[i].GetType() == LEFT_PARATHESES)
			{
				if (begin == -1) begin = i;
				s.push(INT_MIN);
			}
			if (orig_parser[i].GetType() == RIGHT_PARATHESES)
			{
				if (s.empty())
				{
					// ')' hits before '('
					Error("SyntaxError: ')' hits before '('.");
				}
				s.pop();
				if (s.empty())
				{
					end = i;
					if (begin + 1 >= end)
					{
						// Case like "()"
						Error("SyntaxError: empty '()' in expression.");
					}
					std::vector<Token> new_parser = copy_parser(orig_parser, begin+1, end);
					parser_erase_tokens_range(orig_parser, begin, end+1);

					DEBUG_MSG("new_parser:");
					for (Token& t : new_parser)
					{
						DEBUG_MSG(t.ToString());
					}

					DEBUG_MSG("orig_parser:");
					for (Token& t : orig_parser)
					{
						DEBUG_MSG(t.ToString());
					}

					parser_insert_token(orig_parser, begin, exprIntOptParser(new_parser));

					DEBUG_MSG("new_orig_parser:");
					for (Token& t : orig_parser)
					{
						DEBUG_MSG(t.ToString());
					}

					// Reset begin, end
					begin = -1;
					end = -1;
					goto start;
				}
			}
		}
	}

	Token exprIntOptParser(std::vector<Token>& parser)
	{
		// First, recursively solve paratheses
		exprParatheses(parser);

		// Then, start doing the real work
		Token temp1, temp2, temp3;
		bool first_token_check = true;
		// Interpreting
		while (get_parser_size(parser) > 0)
		{
			// Get the first token
			temp1 = parser_pop_front(parser);
			/*
			First token check:
			A Negative number,
			not a number
			*/
			if (first_token_check)
			{
				if (temp1.GetType() == MINUS)
				{
					temp2 = parser_pop_front(parser);
					if (temp2.GetType() == INTEGER)
					{
						parser_insert_token(parser, 0, Token(INTEGER, "-" + temp2.GetValue()));
					}
					else
					{
						DEBUG_MSG(temp2.ToString() + " is supposed to be a integer.");
						goto E;
					}
				}
				else if (temp1.GetType() != INTEGER)
				{
					DEBUG_MSG(temp1.ToString() + " is supposed to be a integer.");
					goto E;
				}
				else
				{
					first_token_check = false;
				}
			}
			// Case temp1 is the last element in parser queue
			if (get_parser_size(parser) == 0)
			{
				if (temp1.GetType() == INTEGER)
				{
					/*
					The final evaluated result is returned here.
					*/
					return temp1;
				}
				else
				{
					DEBUG_MSG(temp1.ToString() + " is supposed to be a integer.");
					goto E;
				}
			}
			// Case read INTEGER in temp1
			if (temp1.GetType() == INTEGER)
			{
				temp2 = parser_pop_front(parser);
				// Case read INTEGER operation in temp2, otherwise Error
				if (GetEnumIntOp(temp2.GetType()) != eUNKNOWN)
				{
					temp3 = parser_pop_front(parser);
					// Case read INTEGER in temp3, otherwise Error
					if (temp3.GetType() == INTEGER)
					{
						DEBUG_MSG("exprIntOp() returns " + exprBinaryIntOp(temp1, temp3, temp2).ToString());
						parser_insert_token(parser, 0, exprBinaryIntOp(temp1, temp3, temp2));
					}
					else
					{
						DEBUG_MSG(temp3.ToString() + " is supposed to be a integer.");
						goto E;
					}
				}
				else
				{
					DEBUG_MSG(temp2.ToString() + " is supposed to be a integer opeartor.");
					goto E;
				}
			}
		}
		// Error GOTO destination
		E:Error("SynatxError: invalid syntax.");
	}

	/*
	Functionality: an combnination of parsing and interpreting
	Return: algebraic result of the math expression
	*/
	std::string interpretIntOpStatement()
	{
		try
		{
			// Parsing
			std::vector<Token> parser = fill_parser();
			return exprIntOptParser(parser).GetValue();
		}
		catch (const E::InterpreterExecption & e)
		{
			std::cerr << e.what() << std::endl;
			return "\0";
		}
		catch (...)
		{
			return "\0";
		}
	}

private:
	std::string m_text;
	unsigned int m_pos;
	Token m_CurrentToken;
	char m_CurrentChar;
};

