#pragma once

#include "stdafx.h"
#include "Error.h"
#include "LT.h"

#define ID_MAXSIZE		10																	// максимальная длина имени идентификатора													// максимальная длина имени идентификатора + нулевой символ
#define TI_MAXSIZE		4096																// максимальное количество строк в таблице идентификаторов
#define TI_INT_DEFAULT	0x00000000															// значение по умолчанию для типа integer
#define TI_UINT_DEFAULT	0x00000000															// значение по умолчанию для типа unsigned integer
#define TI_STR_DEFAULT	0x00																// значение по умолчанию для типа string
#define TI_NULLIDX		0xffffffff															// код отсутствия идентификатора в таблице
#define TI_STR_MAXSIZE	255																// максимальная длина строки													
#define MAX_PARAMS_COUNT 3
#define COMBINE_PARAMS_CNT 2
#define LEXCOMP_PARAMS_CNT 2
#define MEASURE_PARAMS_CNT 1
#define RANDOMIZE_PARAMS_CNT 2
#define EXPONENT_PARAMS_CNT 2
#define READ_PARAMS_CNT 0
#define ABS_PARAMS_CNT 1
#define SQRT_PARAMS_CNT 1

#define COMBINE_TYPE IT::IDDATATYPE::STR
#define LEXCOMP_TYPE IT::IDDATATYPE::INT
#define MEASURE_TYPE IT::IDDATATYPE::INT
#define RANDOMIZE_TYPE IT::IDDATATYPE::UINT
#define EXPONENT_TYPE IT::IDDATATYPE::UINT
#define READ_TYPE IT::IDDATATYPE::STR
#define ABS_TYPE IT::IDDATATYPE::UINT
#define SQRT_TYPE IT::IDDATATYPE::UINT

namespace IT																				// таблица идентификаторов
{
	enum IDDATATYPE { INT = 1, STR = 2, PROC = 3, BOOL = 4, CHR = 5, UINT = 6, UNKNOWN };		// тип данных идентификатора: integer, string, тип void(процедура), bool, тип неизвестен
	enum IDTYPE {
		V = 1, F = 2, P = 3, L = 4, S = 5						// тип идентификатора: переменная, функция, параметр, литерал, стан. функция
	};

	static const IDDATATYPE LINK_PARAMS[] = { IT::IDDATATYPE::STR, IT::IDDATATYPE::STR };
	static const IDDATATYPE LEXCOMP_PARAMS[] = { IT::IDDATATYPE::STR, IT::IDDATATYPE::STR };
	static const IDDATATYPE MEASURE_PARAMS[] = { IT::IDDATATYPE::STR };
	static const IDDATATYPE READ_PARAMS[] = { IT::IDDATATYPE::UNKNOWN };
	static const IDDATATYPE RANDOMIZE_PARAMS[] = { IT::IDDATATYPE::UINT, IT::IDDATATYPE::UINT };
	static const IDDATATYPE EXPONENT_PARAMS[] = { IT::IDDATATYPE::UINT, IT::IDDATATYPE::UINT };
	static const IDDATATYPE ABS_PARAMS[] = { IT::IDDATATYPE::UINT };
	static const IDDATATYPE SQRT_PARAMS[] = { IT::IDDATATYPE::UINT };

	struct Entry												// строка таблицы идентификаторов
	{
		int			idxfirstLE;									// индекс первой лексемы в таблице лексем
		unsigned char id[ID_MAXSIZE];							// идентификатор
		IDDATATYPE	iddatatype = UNKNOWN;						// тип данных
		IDTYPE		idtype;										// тип идентификатора
		unsigned char visibility[ID_MAXSIZE];					// область видимости

		struct
		{
			int vint;											// значение integer
			struct
			{
				int len;										// количество символов в string
				unsigned char str[TI_STR_MAXSIZE - 1];			// строка string
			} vstr;												// значение string
			struct
			{
				int count;										// количество параметров функции
				IDDATATYPE* types;								// типы параметров функции
			} params;
		} value = { LT_TI_NULLIDX };							// значение идентификатора
	};

	struct IdTable												// описание таблицы идентификаторов
	{
		int maxsize;											// размер таблицы идентификаторов < TI_MAXSIZE
		int size;												// текущий размер таблицы идентификаторов < maxsize
		Entry* table;											// массив строк таблицы идентификаторов
	};

	IdTable Create(																			// создание таблицы идентификаторов
		int size																			// размер таблицы идентификаторов < TI_MAXSIZE
	);

	void Add(																				// добавление строки в таблицу идентификаторов
		IdTable& idtable,																	// описание таблицы идентификаторов
		Entry entry																			// строка таблицы идентификаторов
	);

	Entry GetEntry(																			// получение строки таблицы идентификаторов
		IdTable& idtable,																	// описание таблицы идентификаторов
		int n																				// номер нужной строки
	);

	int IsId(																				// функция: номер строки (если есть), TI_NULLIDX (если нет)
		IdTable& idtable,																	// описание таблицы идентификаторов
		unsigned char id[ID_MAXSIZE]														// идентификатор
	);

	void Delete(IdTable& idtable);															// удаление таблицы (очистка памяти)

	void WriteTable(IdTable& idtable);
};