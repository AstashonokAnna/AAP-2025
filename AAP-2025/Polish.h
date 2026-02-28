#pragma once
#include "stdafx.h"
#include "LexAnalysis.h"
#include "Error.h"

namespace Poliz
{
	bool StartPoliz(Lexer::LEX& lex);
	bool Poliz(int i, Lexer::LEX& lex);
}
