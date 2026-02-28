#include "stdafx.h"
#include "SemAnalysis.h"
#define LEX_�YCLE 'u'

namespace Semantics
{
	bool Analyze(Lexer::LEX lex, Log::LOG log)
	{
		bool isError = false;
		auto isNumericLike = [](IT::IDDATATYPE type) -> bool
			{
				return type == IT::INT || type == IT::UINT || type == IT::CHR || type == IT::BOOL;
			};
		for (int i = 0; i < lex.lextable.size; i++)
		{
			switch (lex.lextable.table[i].lexema)
			{
			case LEX_MEASURE:
			case LEX_COMBINE:
			case LEX_LEXCOMP:
			case LEX_RANDOMIZE:
			case LEX_EXPONENT:
			case LEX_SQRT:
			case LEX_ID:
			{
				if ((lex.idtable.table[lex.lextable.table[i].idxTI].idtype == IT::F || lex.idtable.table[lex.lextable.table[i].idxTI].idtype == IT::S)
					&& lex.lextable.table[i - 2].lexema != LEX_FUNCTION && lex.lextable.table[i - 1].lexema != LEX_PROCEDURE)
				{
					IT::Entry e = lex.idtable.table[lex.lextable.table[i].idxTI];
					int countParm = 0;
					for (int j = i + 2; lex.lextable.table[j].lexema != LEX_RIGHTTHESIS; j++)
					{
						if (lex.lextable.table[j].lexema == LEX_ID || lex.lextable.table[j].lexema == LEX_LITERAL)
						{
							countParm++;
							if (countParm > e.value.params.count) {
								isError = true;
								Log::WriteError(log, Error::geterrorin(316, lex.lextable.table[j].line, -1));
							}
							if (lex.idtable.table[lex.lextable.table[j].idxTI].iddatatype != e.value.params.types[countParm - 1]) {
								isError = true;
								Log::WriteError(log, Error::geterrorin(315, lex.lextable.table[j].line, -1));
							}
						}

					}
					if (countParm < e.value.params.count) {
						isError = true;
						Log::WriteError(log, Error::geterrorin(317, lex.lextable.table[i].line, -1));
					}
				}
				break;
			}

			case LEX_EQUAL:
			{
				IT::IDDATATYPE lefttype = lex.idtable.table[lex.lextable.table[i - 1].idxTI].iddatatype;
				bool ignore = false;
				auto areIntCompatible = [](IT::IDDATATYPE l, IT::IDDATATYPE r) -> bool
					{
						return (l == IT::INT && r == IT::UINT) ||
							(l == IT::UINT && r == IT::INT) ||
							(l == IT::UINT && r == IT::UINT) ||
							(l == IT::INT && r == IT::INT);
					};

				for (int k = i + 1; lex.lextable.table[k].lexema != LEX_SEMICOLON; k++)
				{
					if (lex.lextable.table[k].lexema == LEX_ID || lex.lextable.table[k].lexema == LEX_LITERAL ||
						lex.lextable.table[k].lexema == LEX_COMBINE || lex.lextable.table[k].lexema == LEX_MEASURE|| lex.lextable.table[k].lexema == LEX_LEXCOMP)
					{
						if (!ignore)
						{
							IT::IDDATATYPE righttype = lex.idtable.table[lex.lextable.table[k].idxTI].iddatatype;
							if (lefttype != righttype && !areIntCompatible(lefttype, righttype)) {
								if (
									lefttype == IT::STR &&
									righttype == IT::CHR
									) {

								}
								else {
									isError = true;
									Log::WriteError(log, Error::geterrorin(320, lex.lextable.table[i].line, -1));
								}
							}
						}

						if (lex.lextable.table[k + 1].lexema == LEX_LEFTTHESIS)
						{
							ignore = true;
							continue;
						}

						if (ignore && lex.lextable.table[k + 1].lexema == LEX_RIGHTTHESIS)
						{
							ignore = false;
							continue;
						}
					}

					if (lefttype == IT::IDDATATYPE::STR || lefttype == IT::IDDATATYPE::BOOL)
					{
						char l = lex.lextable.table[k].lexema;
						if (l == LEX_PLUS || l == LEX_MINUS || l == LEX_DIVIDE || l == LEX_OSTATOC || l == LEX_TIMES) {
							isError = true;
							Log::WriteError(log, Error::geterrorin(321, lex.lextable.table[i].line, -1));
						}
					}
				}
				break;
			}

			case LEX_SWITCH:
			{
				for (int j = i + 2; j < lex.lextable.size && lex.lextable.table[j].lexema != LEX_RIGHTTHESIS; j++)
				{
					if (lex.lextable.table[j].lexema == LEX_ID || lex.lextable.table[j].lexema == LEX_LITERAL)
					{
						if (!isNumericLike(lex.idtable.table[lex.lextable.table[j].idxTI].iddatatype))
						{
							isError = true;
							Log::WriteError(log, Error::geterrorin(320, lex.lextable.table[j].line, -1));
							break;
						}
					}
				}
				break;
			}

			case LEX_FUNCTION:
			{
				if (lex.lextable.table[i + 2].lexema == LEX_ID)
				{
					IT::Entry e = lex.idtable.table[lex.lextable.table[i + 2].idxTI];
					i++;
					while (i <= lex.lextable.size && (lex.lextable.table[i].lexema != LEX_FUNCTION || lex.lextable.table[i].lexema != LEX_MAIN || lex.lextable.table[i].lexema != LEX_PROCEDURE))
					{
						if (lex.lextable.table[i].lexema == LEX_GIVE && (lex.lextable.table[i + 2].lexema == LEX_ID || lex.lextable.table[i + 2].lexema == LEX_LITERAL))
						{
							int next = lex.lextable.table[i + 2].idxTI;
							if (next != TI_NULLIDX)
							{
								if (lex.idtable.table[next].iddatatype != e.iddatatype) {
									isError = true;
									Log::WriteError(log, Error::geterrorin(313, lex.lextable.table[i].line, -1));
								}
							}
							break;
						}
						i++;
					}
					for (int k = i + 3; lex.lextable.table[k].lexema != LEX_BRACELET; k++) {
						if(lex.lextable.table[k].lexema == LEX_ID)
							if (lex.lextable.table[k + 1].lexema == LEX_LEFTTHESIS) {
								isError = true;
								Log::WriteError(log, Error::geterrorin(325, lex.lextable.table[i].line, -1));
							}
					}
				}
				break;
			}

			case LEX_LOGOPERATOR:
			{
				bool flag = true;
				if (lex.lextable.table[i - 1].lexema == LEX_ID || lex.lextable.table[i - 1].lexema == LEX_LITERAL)
				{
					if (lex.idtable.table[lex.lextable.table[i - 1].idxTI].iddatatype != IT::IDDATATYPE::INT
						&& lex.idtable.table[lex.lextable.table[i - 1].idxTI].iddatatype != IT::IDDATATYPE::UINT
						&& lex.idtable.table[lex.lextable.table[i - 1].idxTI].iddatatype != IT::IDDATATYPE::CHR)
						flag = false;
				}
				if (lex.lextable.table[i + 1].lexema == LEX_ID || lex.lextable.table[i + 1].lexema == LEX_LITERAL)
				{
					if (lex.idtable.table[lex.lextable.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::INT
						&& lex.idtable.table[lex.lextable.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::UINT
						&& lex.idtable.table[lex.lextable.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::CHR)
						flag = false;
				}
				if (!flag)
				{
					isError = true;
					Log::WriteError(log, Error::geterrorin(322, lex.lextable.table[i].line, -1));
				}
				break;
			}

			case LEX_DIVIDE:
			case LEX_OSTATOC:
			{
				if (lex.lextable.table[i].operation == LT::DIV || lex.lextable.table[i].operation == LT::REST)
				{
					LT::Entry e;
					IT::Entry et;
					bool equ = false;
					if (lex.lextable.table[i + 1].lexema == LEX_LITERAL || lex.lextable.table[i + 1].lexema == LEX_ID)
						e = lex.lextable.table[i + 1];
					else
						e = lex.lextable.table[i + 2];
					et = lex.idtable.table[e.idxTI];
					if (lex.lextable.table[i + 1].lexema == LEX_ID && lex.idtable.table[lex.lextable.table[i + 1].idxTI].idtype == IT::V)
					{
						for (int n = i; n >= 0; n--)
						{
							if (lex.lextable.table[n].lexema == LEX_ID && !strcmp((char*)lex.idtable.table[lex.lextable.table[n].idxTI].id, (char*)et.id)
								&& lex.lextable.table[n + 1].lexema == LEX_EQUAL)
							{
							if (lex.lextable.table[n + 2].lexema == LEX_LITERAL && (lex.idtable.table[lex.lextable.table[n + 2].idxTI].iddatatype == IT::INT || lex.idtable.table[lex.lextable.table[n + 2].idxTI].iddatatype == IT::UINT))
								{
									equ = true;
									if (lex.idtable.table[lex.lextable.table[n + 2].idxTI].value.vint == 0) {
										isError = true;
										Log::WriteError(log, Error::geterrorin(323, lex.lextable.table[i].line, -1));
									}

									break;
								}
								if (lex.lextable.table[n + 2].lexema == LEX_ID && (lex.idtable.table[lex.lextable.table[n + 2].idxTI].iddatatype == IT::INT
									|| lex.idtable.table[lex.lextable.table[n + 2].idxTI].iddatatype == IT::UINT)
									&& lex.idtable.table[lex.lextable.table[n + 2].idxTI].idtype == IT::V)
								{
									strcpy((char*)et.id, (char*)lex.idtable.table[lex.lextable.table[n + 2].idxTI].id);
									continue;
								}
							}
						}
						if (!equ && lex.idtable.table[lex.lextable.table[i + 1].idxTI].value.vint == 0) {
							isError = true;
							Log::WriteError(log, Error::geterrorin(323, lex.lextable.table[i].line, -1));
						}
					}
					if (lex.lextable.table[i + 1].lexema == LEX_LITERAL)
					{
						if (lex.idtable.table[e.idxTI].value.vint == 0) {
							isError = true;
							Log::WriteError(log, Error::geterrorin(323, lex.lextable.table[i].line, -1));
						}
					}
				}
				break;
			}

			case LEX_СYCLE:
			case LEX_WHERE:
			{
				if (lex.lextable.table[i + 1].lexema == LEX_LEFTTHESIS && lex.lextable.table[i + 3].lexema == LEX_RIGHTTHESIS)
					if ((lex.lextable.table[i + 2].lexema == LEX_ID || lex.lextable.table[i + 2].lexema == LEX_LITERAL) && lex.idtable.table[lex.lextable.table[i + 2].idxTI].iddatatype != IT::BOOL) {
						isError = true;
						Log::WriteError(log, Error::geterrorin(301, lex.lextable.table[i].line, -1));
					}
			}
			}
		}
		return !isError;
	}
}