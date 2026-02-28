#pragma once

#include "Log.h"
#include "LexAnalysis.h"

namespace Semantics
{
	bool Analyze(Lexer::LEX lex, Log::LOG log);
}