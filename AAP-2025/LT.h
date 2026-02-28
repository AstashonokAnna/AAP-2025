#pragma once

#include "Error.h"

#define LEXEMA_FIXSIZE		1																// фиксированная длина лексемы
#define LT_MAXSIZE			4096															// максимальное количество строк в таблице лексем
#define LT_TI_NULLIDX		0xfffffff														// код отсутствия идентификатора в таблице идентификаторов

#define LEX_TYPE			't'						// лексема для объявления типа (int_ str_ bool_)
#define LEX_CREATE			'c'						// лексема для создания переменной (declare)
#define LEX_ID				'i'						// лексема для идентификатора
#define LEX_LITERAL			'l'						// лексема для литерала
#define LEX_MAIN			'm'						// лексема для main
#define LEX_FUNCTION		'f'						// лексема для function
#define LEX_PROCEDURE		'p'						// лексема для procedure
#define LEX_GIVE			'r'						// лексема для возвращения значения (give)
#define LEX_SPEAK			'o'						// лексема для вывода в консоль
//#define LEX_OPERATOR		'v'						// лексема для операции (+ - * / %)
#define LEX_EQUAL			'='						// лексема для присваивания (=)
#define LEX_LOGOPERATOR		'g'						// лексема для логического оператора (> < ~ #)
#define LEX_СYCLE			'u'						// лексема для cycle (цикл)
#define LEX_WHERE			'w'						
#define LEX_OTHERWISE		'!'						// лексема для or (иначе условие)
#define LEX_SWITCH          'q'                     // лексема для switch
#define LEX_CASE            'h'                     // лексема для case
#define LEX_DEFAULT         'y'                     // лексема для default
#define LEX_ABS             'a'                     // лексема для abs
#define LEX_SQRT            'x'                     // лексема для sqrt
#define LEX_SEMICOLON		';'						// лексема для ;
#define LEX_COMMA			','						// лексема для ,
#define LEX_LEFTBRACE		'{'						// лексема для {
#define LEX_BRACELET		'}'						// лексема для }
#define LEX_LEFTTHESIS		'['						// лексема для [
#define LEX_RIGHTTHESIS		']'						// лексема для ]
#define LEX_LEFTBRACKET		'('						// лексема для (
#define LEX_RIGHTBRACKET	')'						// лексема для )
#define LEX_PLUS            '+'
#define LEX_MINUS           '-'
#define LEX_TIMES           '*'
#define LEX_DIVIDE          '/'
#define LEX_OSTATOC         '%'



#define LEX_COMBINE			'k'						// лексема для комбинации строк (combine)
#define LEX_LEXCOMP			's'					
#define LEX_MEASURE			'd'						// лексема для измерения длины строки (measure)
#define LEX_EXPONENT		'e'						// лексема для возведения в степень (exponent)
#define LEX_BREAKL			'b'						// лексема для выхода из цикла (breakl)
#define LEX_RANDOMIZE		'z'						// лексема для генерации случайного числа (randomize)
#define LEX_READ            '_'

namespace LT																				// таблица лексем
{
	enum OPER { NOT = -1, PLUS = 1, MINUS, MUL, DIV, REST, MORE, LESS, EQU, NOEQU, MOREEQU, LESSEQU };

	struct Entry								// строка таблицы лексем
	{
		unsigned char lexema;					// лексема
		int line;								// номер строки в исходном тексте
		int idxTI;								// индекс в таблице идентификаторов или LT_TI_NULLIDX
		int priority;							// приоритет
		OPER operation;
	};

	struct LexTable								// описание таблицы лексем
	{
		int maxsize;							// размер таблицы лексем < LT_MAXSIZE
		int size;								// текущий размер таблицы лексем < maxsize
		Entry* table;							// массив строк таблицы лексем
	};

	LexTable Create(																		// создание таблицы лексем
		int size																			// размер таблицы лексем < LT_MAXSIZE
	);

	void Add(																				// добавление строки в таблицу лексем
		LexTable& lextable,																	// описание таблицы лексем
		Entry entry																			// строка таблицы лексем
	);

	Entry GetEntry(																			// получение строки таблицы лексем
		LexTable& lextable,																	// описание таблицы лексем
		int n																				// номер нужной строки
	);

	void Delete(LexTable& lextable);														// удаление таблицы лексем (очистка памяти)

	Entry writeEntry(																		// заполнение строки таблицы лексем
		Entry& entry,
		unsigned char lexema,
		int indx,
		int line,
		int priority = -1,
		OPER operation = NOT
	);

	void WriteLexTable(LexTable& lextable);

	void WriteTokens(LexTable lextable);
};