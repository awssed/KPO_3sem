#include<iostream>
#include"LA.h"
#include"stdafx.h"
#include"In.h"
#include"Error.h"
#include <stdio.h>
#include <string.h>
#include <iomanip>



namespace LA
{
	LT::LexTable __LexTable = LT::Create(LT_MAXSIZE - 1);
	IT::IdTable __IdTable = IT::Create(TI_MAXSIZE - 1);
	bool stringFlag = false;
	bool parmFlag = false;
	bool functionFlag = false;
	bool mainFlag = false;
	bool callFunc;
	char* str = new char[MAX_LEX_SIZE];
	bool declareFlag = false;
	
	void Add_LT(int line, char token)
	{
		LT::Entry entry;
		entry.sn = line;
		memset(entry.lexema, '\0', strlen(entry.lexema) - 1);
		entry.lexema[0] = token;
		LT::Add(__LexTable, entry);
	}
	char FST()
	{
		FST_INT
		FST_STRING
		FST_FUNC
		FST_DECLARE
		FST_RETURN
		FST_MAIN
		FST_PRINT
		FST_LITERAL
		FST_IDENF
		if (FST::Execute(_int))
			return LEX_INTEGER;
		if (FST::Execute(_string))
		{
			stringFlag = true;
			return LEX_STRING;
		}
		if (FST::Execute(_function))
			return LEX_FUNCTION;
		if (FST::Execute(_declare))
			return LEX_DECLARE;
		if (FST::Execute(_return))
			return LEX_RETURN;
		if (FST::Execute(_main))
		{
			mainFlag = true;
			return LEX_MAIN;
		}
		if (FST::Execute(_print))
			return LEX_PRINT;
		if (FST::Execute(literal_int))
			return LEX_LITERAL;
		if (FST::Execute(idenf))
			return LEX_ID;
		return NULL;
	}
	void LA(Parm::PARM parm,In::IN in)
	{
		int indexIT;
		setlocale(LC_ALL, "rus");
		LT::Entry current_entry_LT;
		int bufferIndex = 0;
		current_entry_LT.sn = 0;
		current_entry_LT.idxTI = 0;
		current_entry_LT.lexema[0] = NULL;
		IT::Entry* currentScope=NULL;
		int number_literal = 0;
		IT::Entry current_entry_IT;
		__LexTable.size = 0;
		int currentLine = 1;
		std::ofstream LT_file;
		std::ofstream IT_file;
		LT_file.open("LT.txt");
		IT_file.open("IT.txt");
		for (int i = 0; i < in.size; i++)
		{
			if (((in.text[i] >= 'A' && in.text[i] <= 'Z') || (in.text[i] >= 'a' && in.text[i] <= 'z') || (in.text[i] >= '0' && in.text[i] <= '9') || (in.text[i] >= 0xC0 && in.text[i] <= 0xFF) || (in.text[i] >= 0xE0 && in.text[i] <= 0xFF)|| in.text[i]=='\'') && in.text[i] != ' ')
			{
					str[bufferIndex] = in.text[i];
					bufferIndex++;
					if (bufferIndex >= MAX_LEX_SIZE)
					{
						throw ERROR_THROW(119);
					}
			}else
			{
				str[bufferIndex ] = '\0';
				current_entry_LT.lexema[0] = FST();
				if (current_entry_LT.lexema[0] == LEX_MAIN)
				{
					mainFlag = true;
					current_entry_LT.idxTI = __IdTable.size;
					memcpy(current_entry_IT.id, str, 5);
					current_entry_IT.id[5] = '\0';
					current_entry_IT.iddatatype = IT::INT;
					current_entry_IT.value.vint = NULL;
					current_entry_IT.idxfirstLE = currentLine;
					current_entry_IT.scope = NULL;
					indexIT = IT::search(__IdTable, current_entry_IT);
					if (indexIT > 0)
					{
						throw ERROR_THROW(105);
					}
					if (indexIT == -1)
					{
						IT::Add(__IdTable, current_entry_IT);
					}
				}
				if (current_entry_LT.lexema[0] == LEX_LITERAL)
				{
					current_entry_LT.idxTI = __IdTable.size;
					sprintf_s(current_entry_IT.id, "L%d", number_literal);
					current_entry_IT.iddatatype = IT::INT;
					current_entry_IT.idtype = IT::L;
					current_entry_IT.idxfirstLE = currentLine;
					current_entry_IT.value.vint = atoi(str);
					current_entry_IT.scope = NULL;
					IT::Add(__IdTable, current_entry_IT);
				}
				if (current_entry_LT.lexema[0] == LEX_ID)
				{
					current_entry_IT.scope = currentScope;
					current_entry_LT.idxTI = __IdTable.size;
					memcpy(current_entry_IT.id, str, 5);
					current_entry_IT.id[5] = '\0';
					current_entry_IT.iddatatype = IT::INT;
					current_entry_IT.value.vint = NULL;
					current_entry_IT.idxfirstLE = currentLine;
					if (stringFlag)
					{
						stringFlag = false;
						current_entry_IT.iddatatype = IT::STR;
						strcpy_s(current_entry_IT.value.vstr->str, "");

					}
					current_entry_IT.idtype = IT::V;
					if (parmFlag)
					{
						current_entry_IT.idtype = IT::P;
						parmFlag = true;
					}
					if (parmFlag)
					{
						current_entry_IT.idtype = IT::P;
						if (functionFlag)
						{
							current_entry_IT.scope = currentScope;
						}
					}
					if (__LexTable.table[__LexTable.size - 1].lexema[0] == LEX_FUNCTION)
					{
						current_entry_IT.idtype = IT::F;
						functionFlag = true;
					}
					if (__LexTable.table[__LexTable.size - 2].lexema[0] == LEX_DECLARE|| current_entry_IT.idtype == IT::F || current_entry_IT.idtype == IT::P)
					{
						indexIT = IT::search(__IdTable, current_entry_IT);
						if ( indexIT> 0)
						{
							throw ERROR_THROW(105);
						}
						if (indexIT == -1)
						{
							IT::Add(__IdTable, current_entry_IT);
						}
					}
					if (__LexTable.table[__LexTable.size - 2].lexema[0] != LEX_DECLARE && current_entry_IT.idtype == IT::V)
					{
						indexIT = IT::search(__IdTable, current_entry_IT);
						if (indexIT >= 0)
						{
							current_entry_LT.idxTI = indexIT;
						}
						else
						{
							throw ERROR_THROW(106);
						}
					}
					std::memset(current_entry_IT.id, NULL, 5);
					current_entry_IT.iddatatype = IT::INT;
					current_entry_IT.value.vint = NULL;
				}
				
				bufferIndex = 0;
				std::memset(str, NULL, bufferIndex + 1);
			}
			if (current_entry_LT.lexema[0]!=NULL)
			{
				current_entry_LT.sn = currentLine;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;

			}
			switch (in.text[i])
			{
			case MARK:
				if (str[0] == '\'' && bufferIndex!=1)
				{
					current_entry_LT.idxTI = __IdTable.size;
					str[bufferIndex] = '\0';
					current_entry_LT.lexema[0] = LEX_LITERAL;
					number_literal++;
					sprintf_s(current_entry_IT.id, "L%d", number_literal);
					current_entry_IT.iddatatype = IT::STR;
					current_entry_IT.idtype = IT::L;
					current_entry_IT.idxfirstLE = currentLine;
					for (int i = 0; i < strlen(str); i++)
					{
						current_entry_IT.value.vstr->str[i] = str[i];
					}
					current_entry_IT.value.vstr->str[strlen(str)] = '\0';
					current_entry_IT.value.vstr->len = strlen(current_entry_IT.value.vstr->str);
					current_entry_LT.sn = currentLine;
					LT::Add(__LexTable, current_entry_LT);
					IT::Add(__IdTable, current_entry_IT);
					current_entry_LT.lexema[0] = NULL;
					break;
				}
				break;
			case NEW_LINE:
				current_entry_LT.lexema[0] = '\n';
				current_entry_LT.sn = currentLine;
				currentLine++;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;	
				break;
			case SEMICOLON:
				current_entry_LT.lexema[0] = LEX_SEMICOLON;
				current_entry_LT.sn = currentLine;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;
				std::cout << __LexTable.table[__LexTable.size - 2].lexema[0];
				if ((functionFlag||mainFlag) && currentScope != NULL && __LexTable.table[__LexTable.size -1].lexema[0] == LEX_BRACELET)
				{
					currentScope = NULL;
					functionFlag = false;
				}
				break;
			case COMMA:
				current_entry_LT.lexema[0] = LEX_COMMA;
				current_entry_LT.sn = currentLine;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;
				break;
			case LEFT_BRACE:
				if (mainFlag)
				{
					currentScope = &__IdTable.table[__IdTable.size - 1];
				}
				current_entry_LT.lexema[0] = LEX_LEFTBRACE;
				current_entry_LT.sn = currentLine;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;
				break;
			case RIGHT_BRACE:
				current_entry_LT.lexema[0] = LEX_BRACELET;
				current_entry_LT.sn = currentLine;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;
				break;
			case LEFTTHESIS:
				current_entry_LT.lexema[0] = LEX_LEFTTHESIS;
				current_entry_LT.sn = currentLine;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;
				if (__LexTable.table[__LexTable.size-3].lexema[0] == LEX_FUNCTION)
				{
					parmFlag = true;
					currentScope = &__IdTable.table[__IdTable.size - 1];
				}
				/*if (__IdTable.table[__LexTable.table[__LexTable.size - 1].idxTI].idtype == IT::F)
				{
					parmFlag = true;
				}*/
				break;
			case RIGHTTHESIS:
				current_entry_LT.lexema[0] = LEX_RIGHTTHESIS;
				current_entry_LT.sn = currentLine;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;
				parmFlag = false;
				break;
			case PLUS:
				current_entry_LT.lexema[0] = LEX_PLUS;
				current_entry_LT.sn = currentLine;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;
				break;
			case MINUS:
				current_entry_LT.lexema[0] = LEX_MINUS;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;
				break;
			case STAR:
				current_entry_LT.lexema[0] = LEX_STAR;
				current_entry_LT.sn = currentLine;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;
				break;
			case DIRSLASH:
				current_entry_LT.lexema[0] = LEX_DIRSLASH;
				current_entry_LT.sn = currentLine;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;
				break;
			case EQUAL:
				current_entry_LT.lexema[0] = LEX_EQUAL;
				current_entry_LT.sn = currentLine;
				LT::Add(__LexTable, current_entry_LT);
				current_entry_LT.lexema[0] = NULL;
				break;
			}
		}
		currentLine = 1;
		LT_file << currentLine;
		LT_file << '\t';
		for (int i = 0; i < __LexTable.size; i++)
		{
			current_entry_LT = LT::GetEntry(__LexTable, i);
			if (currentLine != current_entry_LT.sn)
			{
				currentLine = current_entry_LT.sn;
				LT_file << currentLine;
				LT_file << '\t';
			}
			LT_file << current_entry_LT.lexema[0];
		}
		LT_file.close();
		IT_file << std::setw(5) << "id"
			<< std::setw(10) << "datatype"
			<< std::setw(10) << "idtype"
			<< std::setw(10) << "Line"
			<< std::setw(10) << "value" 
			<<std::setw(10)  << "Scope" << std::endl;

		for (int i = 0; i < __IdTable.size; i++) {
			current_entry_IT = IT::GetEntry(__IdTable, i);
			IT_file << std::setw(5) << current_entry_IT.id;
			if (current_entry_IT.iddatatype == 1)
				IT_file << std::setw(10) << "INT";
			if (current_entry_IT.iddatatype == 2)
				IT_file << std::setw(10) << "STR";
			if (current_entry_IT.idtype == IT::V)
				IT_file << std::setw(10) << "V";
			if (current_entry_IT.idtype == IT::L)
				IT_file << std::setw(10) << "L";
			if (current_entry_IT.idtype == IT::F)
				IT_file << std::setw(10) << "F";
			if (current_entry_IT.idtype == IT::P)
				IT_file << std::setw(10) << "P";
			IT_file << std::setw(10) << current_entry_IT.idxfirstLE;

			if (current_entry_IT.iddatatype == IT::INT) {
				IT_file << std::setw(10) << current_entry_IT.value.vint;
			}
			if (current_entry_IT.iddatatype == IT::STR) {
				IT_file << std::setw(7);
				for (int j = 0; j < strlen(current_entry_IT.value.vstr->str); j++) {
					IT_file << current_entry_IT.value.vstr->str[j];
				}
			}
			IT_file << std::setw(10);
			if (current_entry_IT.scope != NULL) {
				for (int j = 0; j < strlen(current_entry_IT.scope->id); j++)
				{
					IT_file << current_entry_IT.scope->id[j];
				}
			}
			IT_file << std::endl;
		}
		IT_file.close();
	}
}