#pragma once

#include "Greibach.h"

#define GRB_ERROR_SERIES 600

#define NS(n)	GRB::Rule::Chain::N(n)
#define TS(n)	GRB::Rule::Chain::T(n)
#define ISNS(n)	GRB::Rule::Chain::isN(n)

namespace GRB
{
	Greibach greibach(
		NS('S'), TS('$'),                     // стартовый символ, конец цепочки
		15,									  // количество правил
		Rule(
			NS('S'), GRB_ERROR_SERIES + 0,    // правило для стартового символа
			5,
			Rule::Chain(6, TS('f'), TS('t'), TS('i'), NS('F'), NS('B'), NS('S')),
			Rule::Chain(5, TS('p'), TS('i'), NS('F'), NS('U'), NS('S')),
			Rule::Chain(4, TS('m'), TS('{'), NS('N'), TS('}')),
			Rule::Chain(5, TS('f'), TS('t'), TS('i'), NS('F'), NS('B')),
			Rule::Chain(4, TS('p'), TS('i'), NS('F'), NS('U'))
		),
		Rule(
			NS('F'), GRB_ERROR_SERIES + 1,
			2,
			Rule::Chain(3, TS('['), NS('P'), TS(']')),
			Rule::Chain(2, TS('['), TS(']'))
		),
		Rule(
			NS('P'), GRB_ERROR_SERIES + 2,    // правило с параметрами функции или в процедуре
			2,
			Rule::Chain(2, TS('t'), TS('i')),
			Rule::Chain(4, TS('t'), TS('i'), TS(','), NS('P'))
		),
		Rule(
			NS('B'), GRB_ERROR_SERIES + 3,    // тело объявления функции
			2,
			Rule::Chain(8, TS('{'), NS('N'), TS('r'), TS('['), NS('I'), TS(']'), TS(';'), TS('}')),
			Rule::Chain(7, TS('{'), TS('r'), TS('['), NS('I'), TS(']'), TS(';'), TS('}'))
		),
		Rule(
			NS('I'), GRB_ERROR_SERIES + 4,    // инициализатор. множество символов из литералов и идентификаторов
			4,
			Rule::Chain(1, TS('l')),
			Rule::Chain(2, TS('l'), NS('I')),
			Rule::Chain(1, TS('i')),
			Rule::Chain(2, TS('i'), NS('I'))
		),
		Rule(
			NS('U'), GRB_ERROR_SERIES + 5,    // тело у процедуры
			1,
			Rule::Chain(3, TS('{'), NS('N'), TS('}'))
		),
		Rule(
			NS('N'), GRB_ERROR_SERIES + 6,    // набор операторов в теле функции
			34,
			Rule::Chain(5, TS('c'), TS('t'), TS('i'), TS(';'), NS('N')),
			Rule::Chain(7, TS('c'), TS('t'), TS('i'), TS('='), NS('E'), TS(';'), NS('N')),
			Rule::Chain(5, TS('i'), TS('='), NS('E'), TS(';'), NS('N')),
			Rule::Chain(8, TS('u'), TS('['), NS('R'), TS(']'), TS('{'), NS('X'), TS('}'), NS('N')),
			Rule::Chain(8, TS('w'), TS('['), NS('R'), TS(']'), TS('{'), NS('X'), TS('}'), NS('N')),
			Rule::Chain(12, TS('w'), TS('['), NS('R'), TS(']'), TS('{'), NS('X'), TS('}'), TS('!'), TS('{'), NS('X'), TS('}'), NS('N')),
			Rule::Chain(6, TS('d'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(6, TS('s'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(6, TS('k'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(6, TS('e'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(6, TS('z'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(6, TS('a'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(6, TS('x'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(6, TS('o'), TS('['), NS('I'), TS(']'), TS(';'), NS('N')),
			Rule::Chain(3, TS('b'), TS(';'), NS('N')),
			Rule::Chain(4, TS('i'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(9, TS('q'), TS('['), NS('E'), TS(']'), TS('{'), NS('V'), TS('}'), TS(';'), NS('N')),

			Rule::Chain(4, TS('c'), TS('t'), TS('i'), TS(';')),
			Rule::Chain(6, TS('c'), TS('t'), TS('i'), TS('='), NS('E'), TS(';')),
			Rule::Chain(4, TS('i'), TS('='), NS('E'), TS(';')),
			Rule::Chain(7, TS('u'), TS('['), NS('R'), TS(']'), TS('{'), NS('X'), TS('}')),
			Rule::Chain(7, TS('w'), TS('['), NS('R'), TS(']'), TS('{'), NS('X'), TS('}')),
			Rule::Chain(11, TS('w'), TS('['), NS('R'), TS(']'), TS('{'), NS('X'), TS('}'), TS('!'), TS('{'), NS('X'), TS('}')),
			Rule::Chain(3, TS('k'), NS('K'), TS(';')),
			Rule::Chain(3, TS('d'), NS('K'), TS(';')),
			Rule::Chain(3, TS('s'), NS('K'), TS(';')),
			Rule::Chain(3, TS('e'), NS('K'), TS(';')),
			Rule::Chain(3, TS('z'), NS('K'), TS(';')),
			Rule::Chain(3, TS('a'), NS('K'), TS(';')),
			Rule::Chain(3, TS('x'), NS('K'), TS(';')),
			Rule::Chain(3, TS('o'), NS('K'), TS(';')),
			Rule::Chain(2, TS('b'), TS(';')),
			Rule::Chain(3, TS('i'), NS('K'), TS(';')),
			Rule::Chain(8, TS('q'), TS('['), NS('E'), TS(']'), TS('{'), NS('V'), TS('}'), TS(';'))
		),
		Rule(
			NS('R'), GRB_ERROR_SERIES + 7,    // правило с условием цикла
			4,
			Rule::Chain(1, TS('i')),
			Rule::Chain(3, TS('i'), TS('g'), TS('i')),
			Rule::Chain(3, TS('i'), TS('g'), TS('l')),
			Rule::Chain(3, TS('l'), TS('g'), TS('i'))
		),
		Rule(
			NS('K'), GRB_ERROR_SERIES + 8,    // правило с вызовом функции
			2,
			Rule::Chain(3, TS('['), NS('W'), TS(']')),
			Rule::Chain(2, TS('['), TS(']'))
		),
		Rule(
			NS('E'), GRB_ERROR_SERIES + 9,    // правило с арифметическим выражением
			22,
			Rule::Chain(1, TS('i')),
			Rule::Chain(1, TS('l')),
			Rule::Chain(3, TS('('), NS('E'), TS(')')),
			Rule::Chain(2, TS('i'), NS('K')),
			Rule::Chain(2, TS('d'), NS('K')),
			Rule::Chain(2, TS('s'), NS('K')),
			Rule::Chain(2, TS('k'), NS('K')),
			Rule::Chain(2, TS('e'), NS('K')),
			Rule::Chain(2, TS('z'), NS('K')),
			Rule::Chain(2, TS('a'), NS('K')),
			Rule::Chain(2, TS('x'), NS('K')),

			Rule::Chain(2, TS('i'), NS('M')),
			Rule::Chain(2, TS('l'), NS('M')),
			Rule::Chain(4, TS('('), NS('E'), TS(')'), NS('M')),
			Rule::Chain(3, TS('i'), NS('K'), NS('M')),
			Rule::Chain(3, TS('d'), NS('K'), NS('M')),
			Rule::Chain(3, TS('s'), NS('K'), NS('M')),
			Rule::Chain(3, TS('k'), NS('K'), NS('M')),
			Rule::Chain(3, TS('e'), NS('K'), NS('M')),
			Rule::Chain(3, TS('z'), NS('K'), NS('M')),
			Rule::Chain(3, TS('a'), NS('K'), NS('M')),
			Rule::Chain(3, TS('x'), NS('K'), NS('M'))

		),
		Rule(
			NS('W'), GRB_ERROR_SERIES + 10,    // правило с параметрами вызова функции 
			4,
			Rule::Chain(1, TS('i')),
			Rule::Chain(1, TS('l')),
			Rule::Chain(3, TS('i'), TS(','), NS('W')),
			Rule::Chain(3, TS('l'), TS(','), NS('W'))
		),
		Rule(
			NS('M'), GRB_ERROR_SERIES + 9,    // правило с арифметическим выражением
			10,
			Rule::Chain(2, TS('+'), NS('E')),
			Rule::Chain(2, TS('-'), NS('E')),
			Rule::Chain(2, TS('/'), NS('E')),
			Rule::Chain(2, TS('*'), NS('E')),
			Rule::Chain(2, TS('%'), NS('E')),
			Rule::Chain(3, TS('+'), NS('E'), NS('M')),
			Rule::Chain(3, TS('-'), NS('E'), NS('M')),
			Rule::Chain(3, TS('/'), NS('E'), NS('M')),
			Rule::Chain(3, TS('*'), NS('E'), NS('M')),
			Rule::Chain(3, TS('%'), NS('E'), NS('M'))
		),
		Rule(
			NS('X'), GRB_ERROR_SERIES + 11,    // набор операторов в теле цикла/условного оператора
			14,
			Rule::Chain(5, TS('i'), TS('='), NS('E'), TS(';'), NS('N')),
			Rule::Chain(6, TS('d'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(6, TS('s'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(6, TS('k'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(6, TS('e'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(6, TS('z'), NS('K'), TS(';'), NS('N')),
			Rule::Chain(6, TS('o'), TS('['), NS('I'), TS(']'), TS(';'), NS('N')),
			Rule::Chain(3, TS('b'), TS(';'), NS('N')),
			Rule::Chain(4, TS('i'), NS('K'), TS(';'), NS('N')),

			Rule::Chain(4, TS('i'), TS('='), NS('E'), TS(';')),
			Rule::Chain(3, TS('k'), NS('K'), TS(';')),
			Rule::Chain(3, TS('e'), NS('K'), TS(';')),
			Rule::Chain(3, TS('z'), NS('K'), TS(';')),
			Rule::Chain(3, TS('d'), NS('K'), TS(';')),
			Rule::Chain(3, TS('s'), NS('K'), TS(';')),
			Rule::Chain(3, TS('o'), NS('K'), TS(';')),
			Rule::Chain(2, TS('b'), TS(';')),
			Rule::Chain(3, TS('i'), NS('K'), TS(';'))
		),
		Rule(
			NS('V'), GRB_ERROR_SERIES + 12,   // правило switch/case/default
			3,
			Rule::Chain(8, TS('h'), TS('['), NS('I'), TS(']'), TS('{'), NS('X'), TS('}'), NS('V')),
			Rule::Chain(7, TS('h'), TS('['), NS('I'), TS(']'), TS('{'), NS('X'), TS('}')),
			Rule::Chain(4, TS('y'), TS('{'), NS('X'), TS('}'))
		)

	);
}