#include "stdafx.h"
#include "Polish.h"
#include <sstream>

extern Log::LOG logfile;

namespace Poliz
{
	bool Poliz(int i, Lexer::LEX& lex, unsigned char stopLexema)
	{
		std::stack<LT::Entry> stack;
		std::queue<LT::Entry> queue;

		LT::Entry temp;
		temp.idxTI = LT_TI_NULLIDX;
		temp.lexema = '#';
		temp.line = lex.lextable.table[i].line;

		LT::Entry func;
		func.idxTI = LT_TI_NULLIDX;
		func.lexema = '@';
		func.line = lex.lextable.table[i].line;

		int countLex = 0;
		int countParm = 1;
		int posLex = i;
		char* buf = new char[1];

		bool findComma = false;

		for (; lex.lextable.table[i].lexema != stopLexema; i++, countLex++)
		{
			switch (lex.lextable.table[i].lexema)
			{
			case LEX_ID:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_COMBINE:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_MEASURE:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_LEXCOMP:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_RANDOMIZE:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_EXPONENT:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_ABS:
			case LEX_SQRT:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_LITERAL:
			{
				queue.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_MINUS:
			case LEX_PLUS:
			case LEX_DIVIDE:
			case LEX_OSTATOC:
			case LEX_TIMES:
			{
				while (!stack.empty() && lex.lextable.table[i].priority <= stack.top().priority)																					
				{
					queue.push(stack.top());
					stack.pop();
				}
				stack.push(lex.lextable.table[i]);
				continue;
			}

			case LEX_COMMA:
			{
				findComma = true;
				continue;
			}
			case LEX_LEFTBRACKET:
			{
				stack.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_RIGHTBRACKET:
			{
				while (stack.top().lexema != LEX_LEFTBRACKET)
				{
					queue.push(stack.top());
					stack.pop();
					if (stack.empty())
						return false;
				}
				stack.pop();
				continue;
			}
			case LEX_LEFTTHESIS:
			{
				stack.push(lex.lextable.table[i]);
				continue;
			}
			case LEX_RIGHTTHESIS:
			{
				if (lex.lextable.table[i - 1].lexema == LEX_LEFTTHESIS)
					countParm--;
				while (stack.top().lexema != LEX_LEFTTHESIS)
				{
					queue.push(stack.top());
					stack.pop();
					if (stack.empty())
						return false;
				}
				if (findComma)
					countParm++;

				lex.lextable.table[i] = func;
				queue.push(lex.lextable.table[i]);
				_itoa_s(countParm, buf, 2, 10);
				stack.top().lexema = buf[0];
				stack.top().idxTI = func.idxTI;
				stack.top().line = func.line;
				queue.push(stack.top());
				stack.pop();
				continue;
			}
			}
		}

		while (!stack.empty())
		{
			if (stack.top().lexema == LEX_LEFTTHESIS || stack.top().lexema == LEX_RIGHTTHESIS || stack.top().lexema == LEX_LEFTBRACKET || stack.top().lexema == LEX_RIGHTBRACKET)
				return false;
			queue.push(stack.top());
			stack.pop();
		}

		int polizLength = countLex;
		while (countLex)
		{
			if (!queue.empty())
			{
				lex.lextable.table[posLex++] = queue.front();
				queue.pop();
			}
			else
			{
				lex.lextable.table[posLex++] = temp;
			}
			countLex--;
		}

		if (logfile.stream)
		{
			std::ostringstream oss;
			oss << "POLIZ: ";
			int start = posLex - polizLength;
			int end = posLex;
			if (start < 0) start = 0;
			for (int k = start; k < end; ++k)
			{
				auto& entry = lex.lextable.table[k];
				oss << static_cast<char>(entry.lexema);
				if (entry.idxTI != LT_TI_NULLIDX)
				{
					oss << '(' << reinterpret_cast<char*>(lex.idtable.table[entry.idxTI].id) << ')';
				}
				oss << ' ';
			}
			*logfile.stream << oss.str() << std::endl;
		}

		for (int i = 0; i < posLex; i++)
		{
			if (lex.lextable.table[i].lexema == LEX_LITERAL)
				lex.idtable.table[lex.lextable.table[i].idxTI].idxfirstLE = i;
		}
		return true;
	}

	bool Poliz(int i, Lexer::LEX& lex)
	{
		return Poliz(i, lex, LEX_SEMICOLON);
	}

	bool StartPoliz(Lexer::LEX& lex)
	{
		bool rc = false;
		bool equ = false;
		for (int i = 0; i < lex.lextable.size; i++)
		{
			if (lex.lextable.table[i].lexema == LEX_EQUAL || (lex.lextable.table[i].lexema == LEX_ID && lex.lextable.table[i - 1].lexema != LEX_EQUAL &&
				lex.idtable.table[lex.lextable.table[i].idxTI].idtype == IT::F && lex.lextable.table[i - 1].lexema != LEX_PROCEDURE && lex.lextable.table[i - 2].lexema != LEX_FUNCTION))
			{
				rc = Poliz(i + 1, lex);
			}
			else if (lex.lextable.table[i].lexema == LEX_SWITCH)
			{
				int start = i + 2;
				int end = start;
				while (end < lex.lextable.size && lex.lextable.table[end].lexema != LEX_RIGHTTHESIS)
					end++;
				if (end < lex.lextable.size)
					rc = Poliz(start, lex, LEX_RIGHTTHESIS);
			}
		}
		return true;
	}
}