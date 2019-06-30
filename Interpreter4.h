#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <stack>

#include "Templates.h"
#include "Execeptions.h"
#include "Config.h"
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
	explicit Interpreter(Lexer lexer) 
	{
		m_lexer = lexer;
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
	void parser_insert_token(std::vector<Token>& parser, unsigned int pos, const Token& token)
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
			parser_erase_token(parser, 0);
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

	START:
		for (int i = 0; i < get_parser_size(orig_parser); i++)
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
					std::vector<Token> new_parser = copy_parser(orig_parser, begin + 1, end);
					parser_erase_tokens_range(orig_parser, begin, end + 1);

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
					goto START;
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
						goto ERROR;
					}
				}
				else if (temp1.GetType() != INTEGER)
				{
					DEBUG_MSG(temp1.ToString() + " is supposed to be a integer.");
					goto ERROR;
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
					goto ERROR;
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
						goto ERROR;
					}
				}
				else
				{
					DEBUG_MSG(temp2.ToString() + " is supposed to be a integer opeartor.");
					goto ERROR;
				}
			}
		}
		// Error GOTO destination
		ERROR:Error("SynatxError: invalid syntax.");
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
		catch (const E::InterpreterExecption& e)
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
	Lexer m_lexer;
	Token m_CurrentToken;
};

