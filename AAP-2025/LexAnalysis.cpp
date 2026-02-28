#include "stdafx.h"
#include "LexAnalysis.h"
#include <string>
using namespace std;
#define WORD_MAXSIZE 255

extern Log::LOG logfile;

namespace Lexer
{
	int BinToDec(char* str) {
		int zn = 1;
		if (str[0] == '1') { zn = -1; }
		for (int i = 2; i <= strlen(str) - 1; i++) {
			str[i - 2] = str[i];
		}
		str[strlen(str) - 2] = '\0';
		int j = 1;
		int Chislo = 0;
		for (int i = 0; i < strlen(str); i++) { // перевод из двоичной системы
			Chislo += ((char)str[i] - '0') * pow(2, strlen(str) - j); //перевод числа
			j++;
		}
		Chislo = Chislo * zn;
		return Chislo;
	}

	int OctToDec(char* str) {
		int zn = 1;
		if (str[0] == '1') { zn = -1; }
		for (int i = 2; i <= strlen(str) - 1; i++) {
			str[i - 2] = str[i];
		}
		str[strlen(str) - 2] = '\0';
		int j = 1;
		int Chislo = 0;
		for (int i = 0; i < strlen(str); ++i) { // перевод из восьмеричной системы
			Chislo *= 8;
			Chislo += ((char)str[i] - '0'); //перевод числа
			j++;
		}
		Chislo = Chislo * zn;
		return Chislo;
	}

	int HexToDec(char* str) {
		int value = 0;
		for (int i = 2; i < strlen(str); i++) {
			char c = str[i];
			int digit = 0;
			if (c >= '0' && c <= '9')
				digit = c - '0';
			else if (c >= 'A' && c <= 'F')
				digit = 10 + (c - 'A');
			else if (c >= 'a' && c <= 'f')
				digit = 10 + (c - 'a');
			value = value * 16 + digit;
		}
		return value;
	}

	int IntToMinus(char* str) {
		int zn = 1;
		if (str[0] == '-') { zn = -1; }
		return zn;
	}

	void Cleanup(unsigned char source[], int size)												// очистка текста от лишних пробелов и замена операторов														
	{
		char SP[] = { " ,;()[]{}=+-*/|~#<>?$" };
		bool findLit = false;
		int count = NULL;
		int length = NULL;
		int line = 1;
		int position = 0;

		for (int i = 0; i < size; i++)
		{
			if (source[i] == IN_CODE_ENDL)
				source[i] = IN_CODE_END;
		}

		for (int i = 0; i < size - 1; i++)
		{
			if (source[i] == '\"')
				findLit = !findLit;
			if (!findLit)
			{
				if (source[i] == '=' && source[i + 1] == '=')
				{
					source[i] = '~';
					source[i + 1] = ' ';
				}
				if (source[i] == '!' && source[i + 1] == '=')
				{
					source[i] = '#';
					source[i + 1] = ' ';
				}
				if (source[i] == '>' && source[i + 1] == '=')
				{
					source[i] = '?';
					source[i + 1] = ' ';
				}
				if (source[i] == '<' && source[i + 1] == '=')
				{
					source[i] = '$';
					source[i + 1] = ' ';
				}
			}
		}
		findLit = false;

		for (int i = 0; i < size; i++)
		{
			position++;
			if (source[i] == '\"')
			{
				findLit = !findLit;
				if (length + 1 > 255)
					Log::WriteError(logfile, Error::geterror(304));
				length = NULL;
				count++;
			}

			if (findLit) {
				if (source[i] == '|') {
					Log::WriteError(logfile, Error::geterrorin(300, line, position));
					findLit = false;
				}
				length++;
			}

			if ((source[i] == ' ' || source[i] == '\t') && !findLit)
			{
				for (int j = 0; j < sizeof(SP) - 1; j++)
				{
					if (source[i + 1] == SP[j] || source[i - 1] == SP[j] || i == 0)
					{
						for (int k = i; k < size; k++)
						{
							source[k] = source[k + 1];
						}
						i--;
						if (source[i] == '|')
							line--;
						break;
					}
				}
			}
			if (source[i] == '|') {
				position = 0;
				line++;
			}
		}
	}

	unsigned char** GetWord(unsigned char source[], int size)										// получение массива слов		
	{
		unsigned char** word = new unsigned char* [LT_MAXSIZE];
		for (int i = 0; i < LT_MAXSIZE; i++)
			word[i] = new unsigned char[256] { NULL };

		bool findSP, findLit = false;
		int j = 0;
		char SP[] = { " ,;()[]{}=+-*/|<>~#$?" };
		for (int i = 0, k = 0; i < size - 1; i++, k++)
		{
			findSP = false;
			if (source[i] == '\"')
				findLit = !findLit;
			for (int t = 0; t < sizeof(SP) - 1; t++)
			{
				if (source[i] == SP[t] && !findLit)
				{
					findSP = true;
					if (word[j][0] != NULL) {
						word[j++][k] = '\0';
						k = 0;
					}
					if (SP[t] == ' ') {
						k = -1;
						break;
					}
					word[j][k++] = SP[t];
					word[j++][k] = '\0';
					k = -1;
					break;
				}
			}
			if (!findSP)
				word[j][k] = source[i];
		}
		word[j] = NULL;
		for (int i = 0; i < j; i++)
			if (!strcmp((char*)word[i], ""))
				return NULL;
		return word;
	}

	LEX Analyze(In::IN in, Log::LOG log)
	{
		LEX lex;
		LT::LexTable lextable = LT::Create(LT_MAXSIZE);
		IT::IdTable idtable = IT::Create(TI_MAXSIZE);
		Cleanup(in.text, in.size);
		unsigned char** word = new unsigned char* [LT_MAXSIZE];
		for (int i = 0; i < LT_MAXSIZE; i++)
			word[i] = new unsigned char[WORD_MAXSIZE] {NULL};

		do {
			word = GetWord(in.text, in.size);
		} while (word == NULL);

		int indxLex = 0;																			// индекс лексемы
		int indexID = 0;																			// индекс идентификатора
		int clit = 1;																				// счетчик литералов
		int line = 1;
		int position = 0;

		unsigned char emptystr[] = "";
		unsigned char* prefix = new unsigned char[ID_MAXSIZE] { "" };								// область видимости
		unsigned char* bufprefix = new unsigned char[ID_MAXSIZE] { "" };								// буфер для склейки
		unsigned char* oldprefix = new unsigned char[ID_MAXSIZE] { "" };								// предыдущая область
		unsigned char* L = new unsigned char[2] { "L" };
		unsigned char* bufL = new unsigned char[TI_STR_MAXSIZE];
		unsigned char* nameLiteral = new unsigned char[10] { "" };
		char* charclit = new char[10] { "" };														// преобразование счетчика в строку

		bool findFunc = false;
		bool findStnFunc = false;
		bool findParm = false;
		bool findProc = false;
		int main = NULL;
		int idFunc = -1;
		int idParam = -1;

		int open = 0, close = 0;

		bool idkhtnm = false;

		IT::Entry entryIT;																			// запись для таблицы идентификаторов

		for (int i = 0; word[i] != NULL; i++, indxLex++)											// проход по словам
		{

			bool findSameID = false;																// флаг совпадения идентификатора
			position += strlen((char*)word[i]) + 1;

			if (word[i][0] == '|') {
				line++;
				position = 0;
				indxLex--;
				continue;
			}

			FST::FST fstNew(word[i], GRAPH_CREATE);
			if (FST::execute(fstNew))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_CREATE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstTypeUInteger(word[i], GRAPH_UINT);
			if (FST::execute(fstTypeUInteger))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_TYPE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::UINT;
				continue;
			}

			FST::FST fstTypeString(word[i], GRAPH_STR);
			if (FST::execute(fstTypeString))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_TYPE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::STR;
				_mbscpy(entryIT.value.vstr.str, emptystr);
				continue;
			}
			FST::FST fstTypeCHR(word[i], GRAPH_CHR);
			if (FST::execute(fstTypeCHR))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_TYPE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::CHR;
				_mbscpy(entryIT.value.vstr.str, emptystr);
				continue;
			}

			FST::FST fstTypeBool(word[i], GRAPH_BOOL);
			if (FST::execute(fstTypeBool))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_TYPE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.iddatatype = IT::BOOL;
				continue;
			}

			FST::FST fstFunction(word[i], GRAPH_FUNCTION);
			if (FST::execute(fstFunction))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_FUNCTION, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.idtype = IT::F;
				entryIT.value.params.count = 0;
				entryIT.value.params.types = new IT::IDDATATYPE[MAX_PARAMS_COUNT];
				idFunc = indexID;
				findFunc = true;
				continue;
			}

			FST::FST fstProcedure(word[i], GRAPH_PROCEDURE);
			if (FST::execute(fstProcedure))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_PROCEDURE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				entryIT.idtype = IT::F;
				entryIT.iddatatype = IT::PROC;
				entryIT.value.params.count = 0;
				entryIT.value.params.types = new IT::IDDATATYPE[MAX_PARAMS_COUNT];
				idFunc = indexID;
				findProc = true;
				findFunc = true;
				continue;
			}

			FST::FST fstLink(word[i], GRAPH_COMBINE);
			if (FST::execute(fstLink))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_COMBINE, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT = {};
				_mbscpy(entryIT.id, word[i]);
				entryIT.idxfirstLE = indxLex;
				entryIT.idtype = IT::S;
				_mbscpy(entryIT.visibility, emptystr);
				entryIT.iddatatype = COMBINE_TYPE;
				entryIT.value.params.count = COMBINE_PARAMS_CNT;
				entryIT.value.params.types = new IT::IDDATATYPE[COMBINE_PARAMS_CNT];
				for (int k = 0; k < COMBINE_PARAMS_CNT; k++)
					entryIT.value.params.types[k] = IT::LINK_PARAMS[k];
				IT::Add(idtable, entryIT);
				entryIT = {};
				findStnFunc = true;
				continue;
			}
			FST::FST fstLexComp(word[i], GRAPH_LEXCOMP);
			if (FST::execute(fstLexComp))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_LEXCOMP, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT = {};
				_mbscpy(entryIT.id, word[i]);
				entryIT.idxfirstLE = indxLex;
				entryIT.idtype = IT::S;
				_mbscpy(entryIT.visibility, emptystr);
				entryIT.iddatatype = LEXCOMP_TYPE;
				entryIT.value.params.count = LEXCOMP_PARAMS_CNT;
				entryIT.value.params.types = new IT::IDDATATYPE[LEXCOMP_PARAMS_CNT];
				for (int k = 0; k < COMBINE_PARAMS_CNT; k++)
					entryIT.value.params.types[k] = IT::LEXCOMP_PARAMS[k];
				IT::Add(idtable, entryIT);
				entryIT = {};
				findStnFunc = true;
				continue;
			}

			FST::FST fstMeasure(word[i], GRAPH_MEASURE);
			if (FST::execute(fstMeasure))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_MEASURE, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT = {};
				_mbscpy(entryIT.id, word[i]);
				entryIT.idxfirstLE = indxLex;
				entryIT.idtype = IT::S;
				_mbscpy(entryIT.visibility, emptystr);
				entryIT.iddatatype = MEASURE_TYPE;
				entryIT.value.params.count = MEASURE_PARAMS_CNT;
				entryIT.value.params.types = new IT::IDDATATYPE[MEASURE_PARAMS_CNT];
				for (int k = 0; k < MEASURE_PARAMS_CNT; k++)
					entryIT.value.params.types[k] = IT::MEASURE_PARAMS[k];
				IT::Add(idtable, entryIT);
				entryIT = {};
				findStnFunc = true;
				continue;
			}

			FST::FST fstRandomize(word[i], GRAPH_RANDOMIZE);
			if (FST::execute(fstRandomize))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_RANDOMIZE, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT = {};
				_mbscpy(entryIT.id, word[i]);
				entryIT.idxfirstLE = indxLex;
				entryIT.idtype = IT::S;
				_mbscpy(entryIT.visibility, emptystr);
				entryIT.iddatatype = RANDOMIZE_TYPE;
				entryIT.value.params.count = RANDOMIZE_PARAMS_CNT;
				entryIT.value.params.types = new IT::IDDATATYPE[RANDOMIZE_PARAMS_CNT];
				for (int k = 0; k < RANDOMIZE_PARAMS_CNT; k++)
					entryIT.value.params.types[k] = IT::RANDOMIZE_PARAMS[k];
				IT::Add(idtable, entryIT);
				entryIT = {};
				findStnFunc = true;
				continue;
			}

			FST::FST fstExponent(word[i], GRAPH_EXPONENT);
			if (FST::execute(fstExponent))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_EXPONENT, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT = {};
				_mbscpy(entryIT.id, word[i]);
				entryIT.idxfirstLE = indxLex;
				entryIT.idtype = IT::S;
				_mbscpy(entryIT.visibility, emptystr);
				entryIT.iddatatype = EXPONENT_TYPE;
				entryIT.value.params.count = EXPONENT_PARAMS_CNT;
				entryIT.value.params.types = new IT::IDDATATYPE[EXPONENT_PARAMS_CNT];
				for (int k = 0; k < EXPONENT_PARAMS_CNT; k++)
					entryIT.value.params.types[k] = IT::EXPONENT_PARAMS[k];
				IT::Add(idtable, entryIT);
				entryIT = {};
				findStnFunc = true;
				continue;
			}

			FST::FST fstSqrt(word[i], GRAPH_SQRT);
			if (FST::execute(fstSqrt))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_SQRT, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT = {};
				_mbscpy(entryIT.id, word[i]);
				entryIT.idxfirstLE = indxLex;
				entryIT.idtype = IT::S;
				_mbscpy(entryIT.visibility, emptystr);
				entryIT.iddatatype = SQRT_TYPE;
				entryIT.value.params.count = SQRT_PARAMS_CNT;
				entryIT.value.params.types = new IT::IDDATATYPE[SQRT_PARAMS_CNT];
				for (int k = 0; k < SQRT_PARAMS_CNT; k++)
					entryIT.value.params.types[k] = IT::SQRT_PARAMS[k];
				IT::Add(idtable, entryIT);
				entryIT = {};
				findStnFunc = true;
				continue;
			}

			FST::FST fstBreakl(word[i], GRAPH_BREAKL);
			if (FST::execute(fstBreakl))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_BREAKL, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstGive(word[i], GRAPH_GIVE);
			if (FST::execute(fstGive))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_GIVE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstSpeak(word[i], GRAPH_SPEAK);
			if (FST::execute(fstSpeak))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_SPEAK, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstCycle(word[i], GRAPH_CYCLE);
			if (FST::execute(fstCycle))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_СYCLE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstSwitch(word[i], GRAPH_SWITCH);
			if (FST::execute(fstSwitch))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_SWITCH, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstWhere(word[i], GRAPH_WHERE);
			if (FST::execute(fstWhere))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_WHERE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstOtherwise(word[i], GRAPH_OTHERWISE);
			if (FST::execute(fstOtherwise))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_OTHERWISE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstCase(word[i], GRAPH_CASE);
			if (FST::execute(fstCase))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_CASE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstDefault(word[i], GRAPH_DEFAULT);
			if (FST::execute(fstDefault))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_DEFAULT, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstMain(word[i], GRAPH_MAIN);
			if (FST::execute(fstMain))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_MAIN, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				_mbscpy(oldprefix, prefix);
				_mbscpy(prefix, word[i]);
				_mbscpy(entryIT.visibility, emptystr);
				main++;
				if (main > 1) {
					Log::WriteError(log, Error::geterrorin(303, line, position));
				}
				continue;
			}

			FST::FST fstTrue(word[i], GRAPH_TRUE);
			FST::FST fstFalse(word[i], GRAPH_FALSE);
			if (FST::execute(fstTrue) || FST::execute(fstFalse))
			{
				int value;

				if (!strcmp((char*)word[i], "true"))
					value = 1;															// преобразование значения булевого литерала из строки в число
				else
					value = 0;

				for (int k = 0; k < idtable.size; k++)									// поиск литерала в ТИ
				{
					if (idtable.table[k].value.vint == value && idtable.table[k].idtype == IT::L && idtable.table[k].iddatatype == IT::BOOL)
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						findSameID = true;
						break;
					}
				}

				if (findSameID)															// если уже есть такой литерал в ТИ
					continue;

				LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT.idtype = IT::L;
				entryIT.iddatatype = IT::BOOL;
				entryIT.value.vint = value;
				entryIT.idxfirstLE = indxLex;
				// генерация имени литерала
				_itoa_s(clit++, charclit, sizeof(char) * 10, 10);						// преобразование счетчика литералов в строку(charclit)
				_mbscpy(bufL, L);														// копируем в буфер "L"
				word[i] = _mbscat(bufL, (unsigned char*)charclit);						// склеиваем эти две строки
				_mbscpy(entryIT.id, word[i]);
				IT::Add(idtable, entryIT);
				entryIT = {};
				continue;
			}

			FST::FST fstIdentif(word[i], GRAPH_ID);
			if (FST::execute(fstIdentif))
			{
				if (strlen((const char*)word[i]) > ID_MAXSIZE)
					Log::WriteError(log, Error::geterrorin(324, line, -1));
				if (findFunc)															// если функция
				{
					if (findProc && entryIT.iddatatype != IT::PROC)
						Log::WriteError(log, Error::geterrorin(312, line, -1));
					int idx = IT::IsId(idtable, word[i]);								// есть в таблице идентификаторов такой же(для функций)
					if (strlen((const char*)word[i]) > ID_MAXSIZE)
						Log::WriteError(log, Error::geterrorin(312, line, -1));
					if (idx != TI_NULLIDX)
						Log::WriteError(log, Error::geterrorin(308, line, -1));
				}
				else
				{
					int idx = IT::IsId(idtable, word[i]);								// ищем идентификатор
					if (idx != TI_NULLIDX)												// если нашли идентификатор то это вызов
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_ID, idx, line);
						LT::Add(lextable, entryLT);
						findFunc = false;
						continue;
					}
					_mbscpy(bufprefix, prefix);
					word[i] = _mbscat(bufprefix, word[i]);
					idx = IT::IsId(idtable, word[i]);									// ищем в области видимости
					if (idx != TI_NULLIDX)												// если нашли идентификатор то это вызов
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_ID, idx, line);
						LT::Add(lextable, entryLT);
						if (lextable.table[lextable.size - 3].lexema == LEX_CREATE)
						{
							if (idtable.table[idx].idtype == IT::P)
								Log::WriteError(log, Error::geterrorin(309, line, -1));
							else
								Log::WriteError(log, Error::geterrorin(310, line, -1));
						}
						continue;
					}
				}

				LT::Entry entryLT = writeEntry(entryLT, LEX_ID, indexID++, line);
				LT::Add(lextable, entryLT);
				/*if (!findParm) {
					Log::WriteError(log, Error::geterrorin(324, line, -1));
				}*/
				if (findParm) {															// если это параметр функции или процедуры
					entryIT.idtype = IT::P;
					_mbscpy(entryIT.visibility, prefix);
					idParam++;
					idtable.table[idFunc].value.params.count++;

					if (idtable.table[idFunc].value.params.count > 3)
						Log::WriteError(log, Error::geterrorin(314, line, -1));
					if (idtable.table[idFunc].value.params.count < 1)
						Log::WriteError(log, Error::geterrorin(324, line, -1));

					idtable.table[idFunc].value.params.types[idParam] = entryIT.iddatatype;
				}

				else if (!findFunc) {													// если переменная
					entryIT.idtype = IT::V;
					_mbscpy(entryIT.visibility, prefix);
					if (entryIT.iddatatype == IT::INT || entryIT.iddatatype == IT::BOOL || entryIT.iddatatype == IT::UINT)
						entryIT.value.vint = TI_INT_DEFAULT;							// значение по умолчанию для целочисленных переменных - 0
					if (entryIT.iddatatype == IT::STR || entryIT.iddatatype == IT::CHR) {
						entryIT.value.vstr.len = 0;
						memset(entryIT.value.vstr.str, TI_STR_DEFAULT, sizeof(char));	// значение по умолчанию для строковых - пустая строка
					}

					if (lextable.table[lextable.size - 2].lexema == LEX_TYPE && lextable.table[lextable.size - 3].lexema != LEX_CREATE)
						Log::WriteError(log, Error::geterrorin(305, line, position));
					if (lextable.table[lextable.size - 2].lexema == LEX_CREATE && lextable.table[lextable.size - 2].lexema != LEX_TYPE)
						Log::WriteError(log, Error::geterrorin(307, line, position));
				}
				else {																	// если объявление функции
					_mbscpy(oldprefix, prefix);
					_mbscpy(prefix, word[i]);
					_mbscpy(entryIT.visibility, emptystr);
				}

				entryIT.idxfirstLE = indxLex;
				_mbscpy(entryIT.id, word[i]);

				if (entryIT.idtype == IT::F && entryIT.iddatatype == IT::UNKNOWN)
					Log::WriteError(log, Error::geterrorin(311, line, -1));
				if (entryIT.idtype == IT::V && entryIT.iddatatype == IT::UNKNOWN)
					Log::WriteError(log, Error::geterrorin(306, line, -1));

				IT::Add(idtable, entryIT);												// добавляем в таблицу идентификаторов
				entryIT = {};
				findFunc = false;
				findProc = false;
				continue;
			}

			FST::FST fstLiteralInt(word[i], GRAPH_INT_LITERAL);
			//if (IntToMinus((char*)word[i]) == -1)
			//	Log::WriteError(log, Error::geterrorin(325, line, -1));
			if (FST::execute(fstLiteralInt))
			{

				int value = atoi((char*)word[i]);										// преобразование значения целочисленного литерала из строки в число

				if (value > 2147483647 || value < -2147483648)
					Log::WriteError(log, Error::geterrorin(319, line, -1));

				for (int k = 0; k < idtable.size; k++)									// поиск литерала в ТИ
				{
					if (idtable.table[k].value.vint == value && idtable.table[k].idtype == IT::L && idtable.table[k].iddatatype == IT::INT)
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						findSameID = true;
						break;
					}
				}

				if (findSameID)															// если уже есть такой литерал в ТИ
					continue;

				LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT.idtype = IT::L;
				entryIT.iddatatype = IT::UINT;
				entryIT.value.vint = value;
				entryIT.idxfirstLE = indxLex;
				// генерация имени литерала
				_itoa_s(clit++, charclit, sizeof(char) * 10, 10);						// преобразование счетчика литералов в строку(charclit)
				_mbscpy(bufL, L);														// копируем в буфер "L"
				word[i] = _mbscat(bufL, (unsigned char*)charclit);						// склеиваем эти две строки
				_mbscpy(entryIT.id, word[i]);
				IT::Add(idtable, entryIT);
				entryIT = {};
				continue;
			}
			//BIN_INT
			FST::FST fstLiteralBinInt(word[i], GRAPH_BIN_INT_LITERAL);
			if (FST::execute(fstLiteralBinInt))
			{
				int value = BinToDec((char*)word[i]);										// преобразование значения двоичного литерала из строки в число

				if (value > 2147483647 || value < -2147483648)
					Log::WriteError(log, Error::geterrorin(319, line, -1));

				for (int k = 0; k < idtable.size; k++)									// поиск литерала в ТИ
				{
					if (idtable.table[k].value.vint == value && idtable.table[k].idtype == IT::L && idtable.table[k].iddatatype == IT::INT)
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						findSameID = true;
						break;
					}
				}

				if (findSameID)															// если уже есть такой литерал в ТИ
					continue;

				LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT.idtype = IT::L;
				entryIT.iddatatype = IT::UINT;
				entryIT.value.vint = value;
				entryIT.idxfirstLE = indxLex;
				// генерация имени литерала
				_itoa_s(clit++, charclit, sizeof(char) * 10, 10);						// преобразование счетчика литералов в строку(charclit)
				_mbscpy(bufL, L);														// копируем в буфер "L"
				word[i] = _mbscat(bufL, (unsigned char*)charclit);						// склеиваем эти две строки
				_mbscpy(entryIT.id, word[i]);
				IT::Add(idtable, entryIT);
				entryIT = {};
				continue;
			}
			//OCT_INT
			FST::FST fstLiteralOctInt(word[i], GRAPH_OCT_INT_LITERAL);
			if (FST::execute(fstLiteralOctInt))
			{
				int value = OctToDec((char*)word[i]);										// преобразование значения восьмеричного литерала из строки в число

				if (value > 2147483647 || value < -2147483648)
					Log::WriteError(log, Error::geterrorin(319, line, -1));

				for (int k = 0; k < idtable.size; k++)									// поиск литерала в ТИ
				{
					if (idtable.table[k].value.vint == value && idtable.table[k].idtype == IT::L && idtable.table[k].iddatatype == IT::INT)
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						findSameID = true;
						break;
					}
				}

				if (findSameID)															// если уже есть такой литерал в ТИ
					continue;

				LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT.idtype = IT::L;
				entryIT.iddatatype = IT::UINT;
				entryIT.value.vint = value;
				entryIT.idxfirstLE = indxLex;
				// генерация имени литерала
				_itoa_s(clit++, charclit, sizeof(char) * 10, 10);						// преобразование счетчика литералов в строку(charclit)
				_mbscpy(bufL, L);														// копируем в буфер "L"
				word[i] = _mbscat(bufL, (unsigned char*)charclit);						// склеиваем эти две строки
				_mbscpy(entryIT.id, word[i]);
				IT::Add(idtable, entryIT);
				entryIT = {};
				continue;
			}

			FST::FST fstLiteralHexInt(word[i], GRAPH_HEX_INT_LITERAL);
			if (FST::execute(fstLiteralHexInt))
			{
				int value = HexToDec((char*)word[i]);

				for (int k = 0; k < idtable.size; k++)
				{
					if (idtable.table[k].value.vint == value && idtable.table[k].idtype == IT::L && idtable.table[k].iddatatype == IT::INT)
					{
						LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						findSameID = true;
						break;
					}
				}

				if (findSameID)
					continue;

				LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, indexID++, line);
				LT::Add(lextable, entryLT);
				entryIT.idtype = IT::L;
				entryIT.iddatatype = IT::UINT;
				entryIT.value.vint = value;
				entryIT.idxfirstLE = indxLex;

				_itoa_s(clit++, charclit, sizeof(char) * 10, 10);
				_mbscpy(bufL, L);
				word[i] = _mbscat(bufL, (unsigned char*)charclit);
				_mbscpy(entryIT.id, word[i]);
				IT::Add(idtable, entryIT);
				entryIT = {};
				continue;
			}

			FST::FST fstLiteralCHR(word[i], GRAPH_CHR_LITERAL);
			if (FST::execute(fstLiteralCHR))
			{
				int length = _mbslen(word[i]);											// длина литерала
				for (int k = 0; k < length; k++)										// удаление кавычек с обеих сторон
					word[i][k] = word[i][k + 1];
				word[i][length - 2] = 0;

				if (_mbslen(word[i]) == 0)
					Log::WriteError(log, Error::geterrorin(318, line, position));

				for (int k = 0; k < idtable.size; k++)									// поиск литерала в ТИ
				{
					if (!(_mbscmp(idtable.table[k].value.vstr.str, word[i])))
					{
						findSameID = true;
						LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						break;
					}
				}

				if (findSameID)															// если уже есть такой литерал в ТИ
					continue;

				LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, indexID++, line);
				LT::Add(lextable, entryLT);
				_mbscpy(entryIT.value.vstr.str, word[i]);								// копирование значения строкового литерала в таблицу идентификаторов
				entryIT.value.vstr.len = length - 2;									// длина самого строкового литерала
				entryIT.idtype = IT::L;
				entryIT.iddatatype = IT::CHR;
				entryIT.idxfirstLE = indxLex;
				// генерация имени литерала
				_itoa_s(clit++, charclit, sizeof(char) * 10, 10);						// преобразование счетчика литералов в строку(charclit)
				_mbscpy(bufL, L);														// копируем в буфер "L"
				nameLiteral = _mbscat(bufL, (unsigned char*)charclit);					// склеиваем эти две строки (L + charclit)
				_mbscpy(entryIT.id, nameLiteral);
				IT::Add(idtable, entryIT);
				entryIT = {};
				continue;
			}

			FST::FST fstLiteralString(word[i], GRAPH_STRING_LITERAL);
			if (FST::execute(fstLiteralString))
			{
				int length = _mbslen(word[i]);											// длина литерала
				for (int k = 0; k < length; k++)										// удаление кавычек с обеих сторон
					word[i][k] = word[i][k + 1];
				word[i][length - 2] = 0;

				if (_mbslen(word[i]) == 0)
					Log::WriteError(log, Error::geterrorin(318, line, position));

				for (int k = 0; k < idtable.size; k++)									// поиск литерала в ТИ
				{
					if (!(_mbscmp(idtable.table[k].value.vstr.str, word[i])))
					{
						findSameID = true;
						LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, k, line);
						LT::Add(lextable, entryLT);
						break;
					}
				}

				if (findSameID)															// если уже есть такой литерал в ТИ
					continue;

				LT::Entry entryLT = writeEntry(entryLT, LEX_LITERAL, indexID++, line);
				LT::Add(lextable, entryLT);
				_mbscpy(entryIT.value.vstr.str, word[i]);								// копирование значения строкового литерала в таблицу идентификаторов
				entryIT.value.vstr.len = length - 2;									// длина самого строкового литерала
				entryIT.idtype = IT::L;
				entryIT.iddatatype = IT::STR;
				entryIT.idxfirstLE = indxLex;
				// генерация имени литерала
				_itoa_s(clit++, charclit, sizeof(char) * 10, 10);						// преобразование счетчика литералов в строку(charclit)
				_mbscpy(bufL, L);														// копируем в буфер "L"
				nameLiteral = _mbscat(bufL, (unsigned char*)charclit);					// склеиваем эти две строки (L + charclit)
				_mbscpy(entryIT.id, nameLiteral);
				IT::Add(idtable, entryIT);
				entryIT = {};
				continue;
			}

			FST::FST fstOperator(word[i], GRAPH_OPERATOR);
			if (FST::execute(fstOperator))
			{
				LT::Entry entryLT;
				switch (word[i][0])														// определение оператора для установки приоритета операции
				{
				case '+':
					entryLT = writeEntry(entryLT, LEX_PLUS, LT_TI_NULLIDX, line, 2, LT::PLUS);
					break;
				case '-':
					entryLT = writeEntry(entryLT, LEX_MINUS, LT_TI_NULLIDX, line, 2, LT::MINUS);
					break;
				case '/':
					entryLT = writeEntry(entryLT, LEX_DIVIDE, LT_TI_NULLIDX, line, 3, LT::DIV);
					break;
				case '*':
					entryLT = writeEntry(entryLT, LEX_TIMES, LT_TI_NULLIDX, line, 3, LT::MUL);
					break;
				case '%':
					entryLT = writeEntry(entryLT, LEX_OSTATOC, LT_TI_NULLIDX, line, 3, LT::REST);
					break;
				}
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstLogOperator(word[i], GRAPH_LOG_OPERATOR);
			if (FST::execute(fstLogOperator))
			{
				LT::Entry entryLT;
				switch (word[i][0])
				{
				case '>':
					entryLT = writeEntry(entryLT, LEX_LOGOPERATOR, LT_TI_NULLIDX, line, -1, LT::MORE);
					break;
				case '<':
					entryLT = writeEntry(entryLT, LEX_LOGOPERATOR, LT_TI_NULLIDX, line, -1, LT::LESS);
					break;
				case '?':
					entryLT = writeEntry(entryLT, LEX_LOGOPERATOR, LT_TI_NULLIDX, line, -1, LT::MOREEQU);
					break;
				case '$':
					entryLT = writeEntry(entryLT, LEX_LOGOPERATOR, LT_TI_NULLIDX, line, -1, LT::LESSEQU);
					break;
				case '~':
					entryLT = writeEntry(entryLT, LEX_LOGOPERATOR, LT_TI_NULLIDX, line, -1, LT::EQU);
					break;
				case '#':
					entryLT = writeEntry(entryLT, LEX_LOGOPERATOR, LT_TI_NULLIDX, line, -1, LT::NOEQU);
					break;
				}
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstSemicolon(word[i], GRAPH_SEMICOLON);
			if (FST::execute(fstSemicolon))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_SEMICOLON, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstComma(word[i], GRAPH_COMMA);
			if (FST::execute(fstComma))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_COMMA, LT_TI_NULLIDX, line, 1);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstLeftBrace(word[i], GRAPH_LEFTBRACE);
			if (FST::execute(fstLeftBrace))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_LEFTBRACE, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				open++;
				continue;
			}

			FST::FST fstRightBrace(word[i], GRAPH_BRACELET);
			if (FST::execute(fstRightBrace))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_BRACELET, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				close++;
				continue;
			}

			FST::FST fstLeftThesis(word[i], GRAPH_LEFTTHESIS);
			if (FST::execute(fstLeftThesis))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_LEFTTHESIS, LT_TI_NULLIDX, line, 0);
				LT::Add(lextable, entryLT);
				if (indexID > 0 && idtable.table[indexID - 1].idtype == IT::F)
					findParm = true;
				continue;
			}

			FST::FST fstRightThesis(word[i], GRAPH_RIGHTTHESIS);
			if (FST::execute(fstRightThesis))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_RIGHTTHESIS, LT_TI_NULLIDX, line, 0);

				if (findParm && word[i + 1][0] != LEX_LEFTBRACE && word[i + 2][0] != LEX_LEFTBRACE)		// если это не заголовок функции
					_mbscpy(prefix, oldprefix);															// восстанавливаем предыдущую область видимости
				idParam = -1;
				findParm = false;
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstLeftBracket(word[i], GRAPH_LEFTBRACKET);
			if (FST::execute(fstLeftBracket))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_LEFTBRACKET, LT_TI_NULLIDX, line, 0);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstRightBracket(word[i], GRAPH_RIGHTBRACKET);
			if (FST::execute(fstRightBracket))
			{

				LT::Entry entryLT = writeEntry(entryLT, LEX_RIGHTBRACKET, LT_TI_NULLIDX, line, 0);
				LT::Add(lextable, entryLT);
				continue;
			}

			FST::FST fstEqual(word[i], GRAPH_EQUAL);
			if (FST::execute(fstEqual))
			{
				LT::Entry entryLT = writeEntry(entryLT, LEX_EQUAL, LT_TI_NULLIDX, line);
				LT::Add(lextable, entryLT);
				continue;
			}


			idkhtnm = true;
			throw ERROR_THROW_IN(205, line, position);
		}

		if (main == NULL)
			Log::WriteError(log, Error::geterror(302));

		if (open > close)
			Log::WriteError(log, Error::geterror(612));
		if (close > open)
			Log::WriteError(log, Error::geterror(613));

		lex.idtable = idtable;
		lex.lextable = lextable;
		return lex;
	}
}