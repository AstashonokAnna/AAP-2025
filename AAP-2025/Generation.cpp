#include "stdafx.h"
#include "Generation.h"

using namespace std;
using uint = unsigned int;

namespace Generator
{
	Gener::Gener(LT::LexTable plexT, IT::IdTable pidT, wchar_t outfile[])
	{
		wchar_t path[PARM_MAX_SIZE];
		wcscpy_s(path, L"./");
		wcscat_s(path, outfile);

		idT = pidT;
		lexT = plexT;
		out = std::ofstream(path, ios_base::out);	
		Head();
		Constants();
		Data();
		Code();
	}

	void Gener::Head()
	{
		out << ".586\n";
		out << ".model flat, stdcall\n";

		out << "includelib libucrt.lib\n";
		out << "includelib kernel32.lib\n";
		out << "includelib D:\\AAP-2025Compiler-main\\AAP-2025\\AAP-2025\\Debug\\Lib.lib\n";
		out << "ExitProcess PROTO :DWORD\n\n";

		out << "EXTRN COMBINE: proc\n";
		out << "EXTRN LEXCOMP: proc\n";
		out << "EXTRN MEASURE: proc\n";
		out << "EXTRN BREAKL: proc\n";
		out << "EXTRN RANDOMIZE: proc\n";
		out << "EXTRN EXPONENT: proc\n";
		out << "EXTRN read: proc\n";
		out << "EXTRN OutputInt: proc\n";
		out << "EXTRN OutputStr: proc\n";
		out << "\n.stack 4096\n\n";
	}

	void Gener::Constants()
	{
		out << ".const\n";
	const uint idCount = static_cast<uint>(idT.size);
	for (uint i = 0; i < idCount; i++)
		{
			if (idT.table[i].idtype == IT::L)
			{
				out << "\t" << idT.table[i].id;
				if (idT.table[i].iddatatype == IT::STR || idT.table[i].iddatatype == IT::CHR)
					out << " BYTE \"" << idT.table[i].value.vstr.str << "\", 0";
			if (idT.table[i].iddatatype == IT::UINT || idT.table[i].iddatatype == IT::INT || idT.table[i].iddatatype == IT::BOOL)
				out << " DWORD " << static_cast<uint>(idT.table[i].value.vint);
				out << '\n';
			}
		}
	}

	void Gener::Data()
	{
		out << "\n.data\n";
		out << "\tbuffer BYTE 256 dup(0)\n";
		out << "\ttmpsqrt SDWORD 0\n";
	const uint lexCount = static_cast<uint>(lexT.size);
	for (uint i = 0; i < lexCount; i++)
		{
			if (lexT.table[i].lexema == LEX_CREATE)
			{
				if (idT.table[lexT.table[i + 2].idxTI].idtype == IT::V)
				{
					out << "\t" << idT.table[lexT.table[i + 2].idxTI].id;
					if (idT.table[lexT.table[i + 2].idxTI].iddatatype == IT::STR || idT.table[lexT.table[i + 2].idxTI].iddatatype == IT::CHR)
					{
						out << " DWORD ?\n";
					}
				if (idT.table[lexT.table[i + 2].idxTI].iddatatype == IT::UINT || idT.table[lexT.table[i + 2].idxTI].iddatatype == IT::INT || idT.table[lexT.table[i + 2].idxTI].iddatatype == IT::BOOL)
				{
					out << " DWORD 0\n";
				}
				}
			}
		}
	}

	void Gener::Code()
	{
		stack<unsigned char*> stk;
	const uint INVALID_INDEX = static_cast<uint>(-1);
	uint num_of_points = 0,
		num_of_ret = 0,
		num_of_ends = 0,
		num_of_cycles = 0,
		num_of_switch = 0,
		num_of_cases = 0,
		switch_depth = 0,
		switch_expr_idx = INVALID_INDEX,
		current_switch = INVALID_INDEX,
		countParm = 0;
		string strret = string(),
			cycle_code = string(),
			func_name = string(),
			switch_end_label = string(),
			switch_skip_label = string();
	bool flag_func = 0,
		flag_ret = 0,
		flag_body = 0,
		flag_if = 0,
		flag_then = 0,
		flag_else = 0,
		flag_measure = 0,
		flag_rand = 0,
		flag_comp = 0,
		flag_exp = 0,
		flag_abs = 0,
		flag_sqrt = 0,
		flag_read = 0,
		flag_proc = 0,
		flag_link = 0,
		flag_callfunc = 0,
		flag_condition = 0,
		flag_noParm = 0,
		flag_break = 0,
		flag_speak = 0,
		flag_cycle = 0,
		flag_switch = 0,
		flag_case = 0,
		flag_default = 0;

		auto valueAsString = [&](uint idx) -> string
			{
				if (idx == LT_TI_NULLIDX || idx >= static_cast<uint>(idT.size))
					return string("0");
				IT::Entry& entry = idT.table[idx];
				if (entry.idtype == IT::L)
				{
					if (entry.iddatatype == IT::CHR && entry.value.vstr.len > 0)
						return to_string(static_cast<uint>(entry.value.vstr.str[0]));
					return to_string(static_cast<uint>(entry.value.vint));
				}
				return string((char*)entry.id);
			};

		auto funcAsmName = [&](const IT::Entry& entry) -> string
			{
				return string("fn_") + string((char*)entry.id);
			};

		auto evaluateExpression = [&](uint start, uint end)
			{
				for (uint j = start; j < end; j++)
				{
					switch (lexT.table[j].lexema)
					{
					case LEX_ID:
					case LEX_LITERAL:
					{
						out << "\tpush " << valueAsString(lexT.table[j].idxTI) << "\n";
						break;
					}
					case LEX_PLUS:
					case LEX_MINUS:
					case LEX_DIVIDE:
					case LEX_OSTATOC:
					case LEX_TIMES:
					{
						switch (lexT.table[j].operation)
						{
						case LT::MUL:
						{
							out << "\tpop eax\n\tpop ebx\n";
							out << "\tmul ebx\n\tpush eax\n";
							break;
						}
						case LT::PLUS:
						{
							out << "\tpop eax\n\tpop ebx\n";
							out << "\tadd eax, ebx\n\tpush eax\n";
							break;
						}
						case LT::MINUS:
						{
							out << "\tpop ebx\n\tpop eax\n";
							out << "\tsub eax, ebx\n\tpush eax\n";
							break;
						}
						case LT::DIV:
						{
							out << "\tpop ebx\n\tpop eax\n";
							out << "\tcdq\n\tidiv ebx\n\tpush eax\n";
							break;
						}
						case LT::REST:
						{
							out << "\tpop ebx\n\tpop eax\n";
							out << "\tcdq\n\tidiv ebx\n\tpush edx\n";
							break;
						}
						}
						break;
					}
					default:
						break;
					}
				}
			};

		out << "\n.code\n\n";
		out << "ABS PROC value : SDWORD\n";
		out << "\tmov eax, value\n";
		out << "\tcdq\n";
		out << "\txor eax, edx\n";
		out << "\tsub eax, edx\n";
		out << "\tret 4\n";
		out << "ABS ENDP\n\n";
		out << "SQRTI PROC value : SDWORD\n";
		out << "\tfinit\n";
		out << "\tfild value\n";
		out << "\tfsqrt\n";
		out << "\tfistp tmpsqrt\n";
		out << "\tmov eax, tmpsqrt\n";
		out << "\tret 4\n";
		out << "SQRTI ENDP\n\n";
		for (uint i = 0; i < static_cast<uint>(lexT.size); i++)
		{
			switch (lexT.table[i].lexema)
			{
			case LEX_PROCEDURE:
			case LEX_FUNCTION:
			{
				while (lexT.table[i].lexema != LEX_RIGHTTHESIS)
				{
					if (lexT.table[i].lexema == LEX_ID && idT.table[lexT.table[i].idxTI].idtype == IT::F)
					{
						if (idT.table[lexT.table[i].idxTI].iddatatype == IT::PROC)
							flag_proc = 1;
						out << (func_name = funcAsmName(idT.table[lexT.table[i].idxTI])) << " PROC ";
					}
					if (lexT.table[i].lexema == LEX_ID && idT.table[lexT.table[i].idxTI].idtype == IT::P)
					{
						out << idT.table[lexT.table[i].idxTI].id << " : ";
						if (idT.table[lexT.table[i].idxTI].iddatatype == IT::INT || idT.table[lexT.table[i].idxTI].iddatatype == IT::BOOL)
						{
							out << "DWORD";
						}
						else if (idT.table[lexT.table[i].idxTI].iddatatype == IT::UINT)
						{
							out << "DWORD";
						}
						else
						{
							out << "DWORD";
						}
					}
					if (lexT.table[i].lexema == LEX_COMMA)
					{
						out << ", ";
					}
					i++;
				}
				flag_func = 1;
				out << "\n";
				break;
			}
			case LEX_MAIN:
			{
				flag_body = 1;
				out << "main PROC\n";
				break;
			}
			case LEX_EQUAL:
			{
			uint result_position = (i == 0 ? 0u : i - 1);
				while (lexT.table[i].lexema != LEX_SEMICOLON)
				{
					switch (lexT.table[i].lexema)
					{
					case LEX_ID:
					case LEX_LITERAL:
					{
						if (idT.table[lexT.table[i].idxTI].idtype == IT::F)
						{
							flag_callfunc = 1;
							break;
						}
						if (idT.table[lexT.table[i].idxTI].iddatatype == IT::INT || idT.table[lexT.table[i].idxTI].iddatatype == IT::BOOL || idT.table[lexT.table[i].idxTI].iddatatype == IT::UINT)
						{
							out << "\tpush " << idT.table[lexT.table[i].idxTI].id << "\n";
							stk.push(idT.table[lexT.table[i].idxTI].id);
							break;
						}
						if (idT.table[lexT.table[i].idxTI].iddatatype == IT::STR || idT.table[lexT.table[i].idxTI].iddatatype == IT::CHR)
						{
							char* s;
							if (idT.table[lexT.table[i].idxTI].idtype == IT::L)
							{
								out << "\tpush offset " << idT.table[lexT.table[i].idxTI].id << "\n";
								s = new char[8]{ "offset " };
							}
							else
							{
								out << "\tpush " << idT.table[lexT.table[i].idxTI].id << "\n";
								s = new char[1]{ "" };
							}
							size_t len1 = strlen((char*)s);
							size_t len2 = strlen((char*)idT.table[lexT.table[i].idxTI].id);
							char* result = (char*)malloc(len1 + len2 + 1);
							memcpy(result, s, len1);
							memcpy(result + len1, (char*)idT.table[lexT.table[i].idxTI].id, len2 + 1);
							stk.push((unsigned char*)result);
							break;
						}
					}
					case LEX_PLUS:
					case LEX_MINUS:
					{

					}



					case LEX_DIVIDE:
					case LEX_OSTATOC:
					case LEX_TIMES:
					{
						switch (lexT.table[i].operation)
						{
						case LT::MUL:
						{
							out << "\tpop eax\n\tpop ebx\n";
							out << "\tmul ebx\n\tpush eax\n";
							break;
						}
						case LT::PLUS:
						{
							out << "\tpop eax\n\tpop ebx\n";
							out << "\tadd eax, ebx\n\tpush eax\n";
							break;
						}
						case LT::MINUS:
						{
							out << "\tpop ebx\n\tpop eax\n";
							out << "\tsub eax, ebx\n\tpush eax\n";
							break;
						}
						case LT::DIV:
						{
							out << "\tpop ebx\n\tpop eax\n";
							out << "\tcdq\n\tidiv ebx\n\tpush eax\n";
							break;
						}
						case LT::REST:
						{
							out << "\tpop ebx\n\tpop eax\n";
							out << "\tcdq\n\tidiv ebx\n\tpush edx\n";
							break;
						}
						}
						break;
					}
					case LEX_MEASURE:
					{
						flag_measure = 1;
						break;
					}
					case LEX_COMBINE:
					{
						flag_link = 1;
						break;
					}
					case LEX_RANDOMIZE:
					{
						flag_rand = 1;
						break;
					}
					case LEX_LEXCOMP:
					{
						flag_comp = 1;
						break;
					}
					case LEX_EXPONENT:
					{
						flag_exp = 1;
						break;
					}
					case LEX_ABS:
					{
						flag_abs = 1;
						break;
					}
					case LEX_SQRT:
					{
						flag_sqrt = 1;
						break;
					}
					case LEX_READ:
					{
						flag_read = 1;
						break;
					}
					case '@':
					{
						countParm = static_cast<uint>(lexT.table[i + 1].lexema - '0');

                        if (flag_link)
						{
							out << "\tpush offset buffer\n";
							out << "\tcall COMBINE\n" << "\tpush eax\n";
							flag_link = false;
						}
						if (flag_measure)
						{
							out << "\tcall MEASURE;" << "\n" << "\tpush eax\n";
							flag_measure = false;
						}
						
						if (flag_rand)
						{
							out << "\tcall RANDOMIZE;" << "\n" << "\tpush eax\n";
							flag_rand = false;
						}
						if (flag_comp)
						{
							out << "\tcall LEXCOMP;" << "\n" << "\tpush eax\n";
							flag_comp = false;
						}
						if (flag_exp)
						{
							out << "\tcall EXPONENT;" << "\n" << "\tpush eax\n";
							flag_exp = false;
						}
						if (flag_abs)
						{
							out << "\tcall ABS;" << "\n" << "\tpush eax\n";
							flag_abs = false;
						}
						if (flag_sqrt)
						{
							out << "\tcall SQRTI;" << "\n" << "\tpush eax\n";
							flag_sqrt = false;
						}
						if (flag_read)
						{
							out << "\tpush offset buffer\n";
							out << "\tcall read;" << "\n" << "\tpush eax\n";
							flag_read = false;
						}
						break;
					}
					}
					i++;
				}
				out << "\tpop " << idT.table[lexT.table[result_position].idxTI].id << "\n";
				break;
			}
			case '@':
			{
			countParm = static_cast<uint>(lexT.table[i + 1].lexema - '0');
			if (countParm == 1)
				flag_noParm = 1;
			for (uint j = 1; j <= countParm; j++)
				{
					if (idT.table[lexT.table[i - j].idxTI].iddatatype == IT::INT || idT.table[lexT.table[i - j].idxTI].iddatatype == IT::BOOL || idT.table[lexT.table[i - j].idxTI].iddatatype == IT::UINT)
						out << "\tpush " << idT.table[lexT.table[i - j].idxTI].id << "\n";
					else
					{
						if (idT.table[lexT.table[i - j].idxTI].idtype == IT::L)
							out << "\tpush offset" << idT.table[lexT.table[i - j].idxTI].id << "\n";
						else
							out << "\tpush " << idT.table[lexT.table[i - j].idxTI].id << "\n";
					}
				}
				out << "\tcall " << funcAsmName(idT.table[lexT.table[i - countParm - 1].idxTI]) << "\n";
				break;
			}
			case LEX_GIVE:
			{
				if (flag_speak || flag_break) {
					out << "\tpop eax\n";
					flag_speak = false;
					flag_break = false;
				}
				out << "\tpush ";
				i++;
				if (idT.table[lexT.table[i + 1].idxTI].idtype == IT::L)
				{
					out << idT.table[lexT.table[i + 1].idxTI].value.vint << "\n";
				}
				else
				{
					out << idT.table[lexT.table[i + 1].idxTI].id << "\n";
				}
				
				if (flag_func)
				{
					out << "\tjmp local" << num_of_ret << "\n";
					flag_ret = 1;
				}
				if (flag_body)
				{
					out << "\t\tjmp theend\n";
					flag_ret = 1;
				}
				break;
			}
			case LEX_SWITCH:
			{
				flag_switch = 1;
				flag_case = 0;
				flag_default = 0;
				switch_skip_label.clear();
				num_of_cases = 0;
				current_switch = num_of_switch++;
				switch_end_label = "swend" + to_string(current_switch);
				{
					uint expr_start = i + 2;
					uint expr_end = expr_start;
					while (expr_end < static_cast<uint>(lexT.size) && lexT.table[expr_end].lexema != LEX_RIGHTTHESIS)
						expr_end++;
					if (expr_end < static_cast<uint>(lexT.size))
					{
						evaluateExpression(expr_start, expr_end);
						out << "\tpop eax\n";
						switch_expr_idx = LT_TI_NULLIDX;
						i = expr_end;
					}
					else
					{
						switch_expr_idx = lexT.table[i + 2].idxTI;
						out << "\tmov eax, " << valueAsString(switch_expr_idx) << "\n";
					}
				}
				break;
			}
			case LEX_CASE:
			{
				if (flag_switch)
				{
					flag_case = 1;
					flag_default = 0;
					num_of_cases++;
					switch_skip_label = "swskip" + to_string(current_switch) + "_" + to_string(num_of_cases);
					uint case_idx = lexT.table[i + 2].idxTI;
					out << "\tcmp eax, " << valueAsString(case_idx) << "\n";
					out << "\tjne " << switch_skip_label << "\n";
					out << "swcase" << current_switch << "_" << num_of_cases << ":\n";
				}
				break;
			}
			case LEX_DEFAULT:
			{
				if (flag_switch)
				{
					flag_case = 1;
					flag_default = 1;
					switch_skip_label.clear();
					out << "swdefault" << current_switch << ":\n";
				}
				break;
			}
			case LEX_LEFTBRACE:
			{
				if (flag_switch)
					switch_depth++;
				break;
			}
			case LEX_BRACELET:
			{
				if (flag_switch && flag_case)
				{
					out << "\tjmp " << switch_end_label << "\n";
					if (!switch_skip_label.empty())
						out << switch_skip_label << ":\n";
					flag_case = false;
				}
				if (flag_body && !flag_then && !flag_else && !flag_func && !flag_cycle && switch_depth == 0 && !flag_switch)
				{
					if (flag_ret)
					{
						out << "theend:\n";
						flag_ret = false;
					}
					out << "\tpush 0\n\tcall ExitProcess\nmain ENDP\nend main";
				}
				if (flag_func && !flag_then && !flag_else && !flag_cycle)
				{
					if (flag_ret)
					{
						out << "local" << num_of_ret++ << ":\n";
						out << "\tpop eax\n\tret\n";
						flag_ret = false;
					}
					if (flag_proc)
					{
						out << "\tret\n";
						flag_proc = false;
					}
					out << func_name << " ENDP\n\n";
					flag_func = false;
				}
				if (flag_then)
				{
					flag_then = false;
					if (flag_else)
					{
						out << "\tjmp e" << num_of_ends << "\n";
						flag_else = false;
					}
					out << "m" << num_of_points++ << ":\n";
				}
				if (flag_else)
				{
					flag_else = false;
					out << "e" << num_of_ends++ << ":\n";
				}
				if (flag_cycle)
				{
					out << cycle_code << "cyclenext" << num_of_cycles << ":\n";
					cycle_code.clear();
					num_of_cycles++;
					flag_cycle = false;
				}
				if (flag_switch && switch_depth > 0)
				{
					switch_depth--;
					if (switch_depth == 0)
					{
						out << switch_end_label << ":\n";
						flag_switch = 0;
						flag_default = 0;
						switch_expr_idx = INVALID_INDEX;
						current_switch = INVALID_INDEX;
					}
				}
				break;
			}
			case LEX_WHERE:
			{
				flag_if = 1;
				break;
			}
			case LEX_СYCLE:
			{
				flag_cycle = 1;
				flag_condition = 1;
				break;
			}
			case LEX_OTHERWISE:
			{
				flag_else = 1;
				break;
			}
			case LEX_LEFTTHESIS:
			{
				if (flag_if)
				{
					if (idT.table[lexT.table[i + 1].idxTI].iddatatype == IT::BOOL && lexT.table[i + 2].lexema == LEX_RIGHTTHESIS)
					{
						out << "\tmov eax, " << idT.table[lexT.table[i + 1].idxTI].id << "\n";
						out << "\tcmp eax, 1\n";
						out << "\tjz m" << num_of_points << "\n";
						out << "\tjnz m" << num_of_points + 1 << "\n";
					}
					else
					{
						out << "\tmov eax, " << idT.table[lexT.table[i + 1].idxTI].id << "\n";
						out << "\tcmp eax, " << idT.table[lexT.table[i + 3].idxTI].id << "\n";
						if (lexT.table[i + 2].operation == LT::MORE)
						{
							out << "\tjg m" << num_of_points << "\n";
							out << "\tjl m" << num_of_points + 1 << "\n";
						}
						else if (lexT.table[i + 2].operation == LT::LESS)
						{
							out << "\tjl m" << num_of_points << "\n";
							out << "\tjg m" << num_of_points + 1 << "\n";
						}
						else if (lexT.table[i + 2].operation == LT::MOREEQU)
						{
							out << "\tjg m" << num_of_points << "\n";
							out << "\tjz m" << num_of_points << "\n";
							out << "\tjl m" << num_of_points + 1 << "\n";
						}
						else if (lexT.table[i + 2].operation == LT::LESSEQU)
						{
							out << "\tjl m" << num_of_points << "\n";
							out << "\tjz m" << num_of_points << "\n";
							out << "\tjg m" << num_of_points + 1 << "\n";
						}
						else if (lexT.table[i + 2].operation == LT::EQU)
						{
							out << "\tjz m" << num_of_points << "\n";
							out << "\tjnz m" << num_of_points + 1 << "\n";
						}
						else if (lexT.table[i + 2].operation == LT::NOEQU)
						{
							out << "\tjnz m" << num_of_points << "\n";
							out << "\tjz m" << num_of_points + 1 << "\n";
						}
					}
					out << "\tje m" << num_of_points + 1 << "\n";
					uint j = i;
					while (lexT.table[j++].lexema != LEX_BRACELET)
					{
						if (lexT.table[j + 1].lexema == LEX_OTHERWISE)
						{
							flag_else = true;
							break;
						}
					}
				}
				if (flag_condition)
				{
					if (idT.table[lexT.table[i + 1].idxTI].iddatatype == IT::BOOL && lexT.table[i + 2].lexema == LEX_RIGHTTHESIS)
					{
						out << "\tmov eax, " << idT.table[lexT.table[i + 1].idxTI].id << "\n";
						out << "\tcmp eax, 1\n";
						cycle_code = "\tmov eax, " + string((char*)idT.table[lexT.table[i + 1].idxTI].id) + "\n" +
							"\tcmp eax, " + string((char*)idT.table[lexT.table[i + 1].idxTI].value.vint) + "\n";
						cycle_code += "\tjz cycle" + to_string(num_of_cycles) + "\n";
						out << "\tjz cycle" << num_of_cycles << "\n";
					}
					else
					{
						cycle_code = "\tmov eax, " + string((char*)idT.table[lexT.table[i + 1].idxTI].id) + "\n" +
							"\tcmp eax, " + string((char*)idT.table[lexT.table[i + 3].idxTI].id) + "\n";
						out << "\tmov eax, " << idT.table[lexT.table[i + 1].idxTI].id << "\n";
						out << "\tcmp eax, " << idT.table[lexT.table[i + 3].idxTI].id << "\n";
						if (lexT.table[i + 2].operation == LT::MORE)
						{
							cycle_code += "\tjg cycle" + to_string(num_of_cycles) + "\n";
							out << "\tjg cycle" << num_of_cycles << "\n";
						}
						else if (lexT.table[i + 2].operation == LT::LESS)
						{
							cycle_code += "\tjl cycle" + to_string(num_of_cycles) + "\n";
							out << "\tjl cycle" << num_of_cycles << "\n";
						}
						else if (lexT.table[i + 2].operation == LT::MOREEQU)
						{
							cycle_code += "\tjg cycle" + to_string(num_of_cycles) + "\n";
							out << "\tjg cycle" << num_of_cycles << "\n";
							cycle_code += "\tjz cycle" + to_string(num_of_cycles) + "\n";
							out << "\tjz cycle" << num_of_cycles << "\n";
						}
						else if (lexT.table[i + 2].operation == LT::LESSEQU)
						{
							cycle_code += "\tjl cycle" + to_string(num_of_cycles) + "\n";
							out << "\tjl cycle" << num_of_cycles << "\n";
							cycle_code += "\tjz cycle" + to_string(num_of_cycles) + "\n";
							out << "\tjz cycle" << num_of_cycles << "\n";
						}
						else if (lexT.table[i + 2].operation == LT::EQU)
						{
							cycle_code += "\tjz cycle" + to_string(num_of_cycles) + "\n";
							out << "\tjz cycle" << num_of_cycles << "\n";
						}
						else if (lexT.table[i + 2].operation == LT::NOEQU)
						{
							cycle_code += "\tjnz cycle" + to_string(num_of_cycles) + "\n";
							out << "\tjnz cycle" << num_of_cycles << "\n";
						}
					}
					out << "\tjmp cyclenext" << num_of_cycles << "\n";
				}
				break;
			}
			case LEX_RIGHTTHESIS:
			{
				if (lexT.table[i + 1].lexema == LEX_LEFTBRACE && flag_if)
				{
					flag_then = true;
					out << "m" << num_of_points++ << ":\n";
					flag_if = false;
				}
				if (lexT.table[i + 1].lexema == LEX_LEFTBRACE && flag_condition)
				{
					out << "cycle" << num_of_cycles << ":\n";
					flag_condition = false;
				}
				break;
			}
			case LEX_SPEAK:
			{
				flag_speak = true;

				if (idT.table[lexT.table[i + 2].idxTI].iddatatype == IT::INT || idT.table[lexT.table[i + 2].idxTI].iddatatype == IT::BOOL || idT.table[lexT.table[i + 2].idxTI].iddatatype == IT::UINT)
				{
					out << "\tpush " << idT.table[lexT.table[i + 2].idxTI].id << "\n\tcall OutputInt\n";
				}
				else
				{
					if (idT.table[lexT.table[i + 2].idxTI].idtype == IT::L)
						out << "\tpush offset ";
					else
						out << "\tpush ";
					out << idT.table[lexT.table[i + 2].idxTI].id << "\n\tcall OutputStr\n";
				}
				break;
			}
			case LEX_BREAKL:
			{
				flag_break = true;
				out << "\tcall BREAKL\n";
				break;
			}
			}
		}
	}
};