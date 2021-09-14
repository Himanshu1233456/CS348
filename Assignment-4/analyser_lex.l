%{
	#include <bits/stdc++.h>
	#include "y.tab.h"
	using namespace std;

	int line_num = 1;
	bool match_cnt;
	char tokn;

	extern char* tmp_error;
	extern vector<string> errors;

	void no_match(void);
%}

%option noyywrap
  
%%

"PROGRAM"	{ return PROGRAM_TOK; }
"VAR"		{ return VAR_TOK; }
"BEGIN"		{ return BEGIN_TOK; }
"END"		{ return END_TOK; }
"END."		{ return ENDDOT_TOK; }
"INTEGER"	{ return INTTYPE_TOK; }
"REAL"		{ return REALTYPE_TOK; }
"FOR"		{ return FOR_TOK; }	
"READ"		{ return READ_TOK; }
"WRITE"		{ return WRITE_TOK; }
"TO"		{ return TO_TOK; }
"DO"		{ return DO_TOK; }
";"			{ return SEMICOL_TOK; }
":"			{ return COL_TOK; }
","			{ return COMMA_TOK; }
":="		{ return ASSGN_TOK; }
"+"			{ return PLUS_TOK; }
"-"			{ return MINUS_TOK; }
"*"			{ return MULTPLY_TOK; }
"DIV"		{ return DIV_TOK; }
"("			{ return OBRACK_TOK; }
")"			{ return CBRACK_TOK; }

[A-Z]([A-Z0-9])* 	{
						match_cnt=true;
						yylval.id = (char *)malloc(strlen(yytext) + 1);
						strcpy(yylval.id,yytext);
						return ID_TOK;
					}

[0-9]+				{
						match_cnt=true;
						yylval.int_val = atoi(yytext);
						return INT_TOK;
					}

[0-9]+"."[0-9]+		{
						match_cnt=true;
						yylval.real_val = atof(yytext);
						return REAL_TOK;
					}

" "					{}

"\n"		{ ++line_num; }

.			{ no_match(); }

%%
  
void no_match(void)
{
	tmp_error[0]='\0';
	tokn='1';
	sprintf(tmp_error,"ERROR --> [ Illegal Character ] Line: %d",line_num);
	string temp(tmp_error);
	match_cnt=false;
	errors.push_back(temp);
}