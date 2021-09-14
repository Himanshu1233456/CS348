%{
	#include <bits/stdc++.h> 
	using namespace std;

	int variable=0;
	bool var_stat;
	bool dec_stat;
	char tok;

	typedef struct sym_tab
	{
		string name;
		int type;
		int line_num;
	}tab;

	unordered_map<string,tab> SYMTAB;

	extern vector<string> tokens;
	extern vector<string> errors;

	extern int line_num;
	extern char* tmp_error;

	vector<string> ids;
	
	extern int yylex(void);

	void type_error(int type_1, int type_2);
	void store_error(char* error);
	extern void yyerror(const char* error);
%}

//CUSTOM yyval DECLARATIONS
%union
{
	char *id;
	int int_val;
	double real_val;
}

%token PROGRAM_TOK 1
%token VAR_TOK 2
%token BEGIN_TOK 3
%token FOR_TOK 8
%token READ_TOK 9
%token WRITE_TOK 10
%token END_TOK 4
%token ENDDOT_TOK 5
%token TO_TOK 11
%token DO_TOK 12
%token SEMICOL_TOK 13
%token COL_TOK 14
%token OBRACK_TOK 21
%token CBRACK_TOK 22
%token COMMA_TOK 15
%token ASSGN_TOK 16
%token MULTPLY_TOK 19
%token DIV_TOK 20
%token PLUS_TOK 17
%token MINUS_TOK 18

%token <id> ID_TOK 23
%token <real_val> REALTYPE_TOK 7
%token <int_val> INT_TOK 24
%token <real_val> REAL_TOK 25
%token <int_val> INTTYPE_TOK 6

//TYPE SPECIFICATION
%type <id> ID
%type <int_val> type exp term factor

%%

prog:
		  PROGRAM_TOK prog_name VAR_TOK { variable = 1; var_stat=true; } dec_list { variable = 0; var_stat=true; } BEGIN_TOK stmt_list ENDDOT_TOK		  
		;

prog_name: 
		  ID
		;

dec_list: 
	  	  dec dec_list 
		| dec 				{ dec_stat=true; }
		;

dec: 
		  id_list COL_TOK type {	
								for(int i=0;i<ids.size();i++)
								{
									var_stat=true;
									auto itr = SYMTAB.find(ids[i]);
									if(itr == SYMTAB.end())
									{
										tab str_tmp;
										str_tmp.line_num = line_num;
										str_tmp.name = ids[i];
										str_tmp.type = $3;
										var_stat=true;
										SYMTAB.insert(make_pair(ids[i],str_tmp));
									}
									else
									{
										var_stat=true;
										tmp_error[0]='\0';
										tok='1';
										sprintf(tmp_error,"ERROR --> [ Duplicate Variable %s Definition ] Line: %d",ids[i].c_str(),line_num);
										tok='0';
										store_error(tmp_error);
									}
								}
								ids.clear();
							}
		;

type: 
		  INTTYPE_TOK 	{ $$ = INTTYPE_TOK;  var_stat=true; tok='1'; }
		| REALTYPE_TOK 	{ $$ = REALTYPE_TOK; var_stat=true; tok='0'; }
		;

id_list: 
		  ID 					{	
		  							if(variable)
		  							{ 
		  								var_stat=true;
										string temp($1);
										tok='1';
										ids.push_back(temp);
									}
								}

		| id_list COMMA_TOK ID 	{	
									if(variable)
									{
										var_stat=true;
										string temp($3);
										tok='0';
										ids.push_back(temp);
									}
								} 
		;

stmt_list: 
		  stmt
		| stmt_list SEMICOL_TOK stmt
		;

stmt: 
		  assign
		| read 		{ var_stat=true; }
		| write 	{ var_stat=true; }
		| for
		;

assign: 
		  ID ASSGN_TOK exp 	{	
		  						string temp($1);
								auto itr = SYMTAB.find(temp);
								if(itr == SYMTAB.end())
								{
									var_stat=true;
									tmp_error[0]='\0';
									tok='1';
									sprintf(tmp_error,"ERROR --> [ Variable %s Not Declared ] Line: %d",$1,line_num);
									tok='0';
									store_error(tmp_error);
								}
								else if(itr->second.type != $3) { type_error(itr->second.type,$3); var_stat=true; }
								dec_stat=false;
							}
		;

exp: 
	  term 					{ $$ = $1; var_stat=true; }

	| exp PLUS_TOK term 	{
								if($1 == $3) { $$ = $1; var_stat=true; tok='1'; }
								else { type_error($1,$3); var_stat=true; tok='0'; }
							}

	| exp MINUS_TOK term 	{	
								if($1 == $3) { $$ = $1; var_stat=true; tok='1'; }
								else { type_error($1,$3); var_stat=true; tok='0'; }
							}

	| error 				{ yyerrok; }
	;

term: 
	  factor 					{ $$ = $1; }
	| term MULTPLY_TOK factor 	{
									if($1 == $3) { $$ = $1; var_stat=true; tok='1'; }
									else { type_error($1,$3); var_stat=true; tok='0'; }
								}

	| term DIV_TOK factor 		{
									if($1 == $3) { $$ = $1; var_stat=true; tok='1'; }
									else { type_error($1,$3); var_stat=true; tok='0'; }
								}
	;

factor:
	  ID 									{
												string temp($1);
												auto itr = SYMTAB.find(temp);
												if(itr == SYMTAB.end())
												{
													var_stat=true;
													tmp_error[0]='\0';
													tok='1';
													sprintf(tmp_error,"ERROR --> [ Variable %s Not Declared ] Line: %d",$1,line_num);
													tok='0';
													store_error(tmp_error);
												}
												else { $$ = itr->second.type; }
											}

	| INT_TOK 								{ $$ = INTTYPE_TOK; var_stat=true; tok='1'; } 
	| REAL_TOK 								{ $$ = REALTYPE_TOK; var_stat=true; tok='0'; }
	| OBRACK_TOK exp CBRACK_TOK 			{ $$ = $2; var_stat=true; }
	;

read:
	  READ_TOK OBRACK_TOK id_list CBRACK_TOK
	;

write:
	  WRITE_TOK OBRACK_TOK id_list CBRACK_TOK
	;

for:
	  FOR_TOK index_exp DO_TOK body
	;

index_exp:
	  ID ASSGN_TOK exp TO_TOK exp
	;
body: 
	  stmt
	| BEGIN_TOK stmt_list END_TOK
	;

ID:
	  ID_TOK 	{
	  				var_stat=false;
					string id_temp(yylval.id);
				  	for(int i=0;i<tokens.size();i++)
				  	{
				  		if(id_temp == tokens[i])
				  		{
				  			var_stat=true;
				  			tmp_error[0]='\0';
				  			tok='1';
				  			sprintf(tmp_error,"ERROR --> [ Variable %s Matches Keyword ] Line: %d",yylval.id,line_num);
				  			tok='0';
				  			store_error(tmp_error);
				  		}
				  	}
				  	var_stat=true;
				  	$$ = yylval.id;
				  	tok='1';
				}
	;

%%

void type_error(int type_1, int type_2)
{
	string s1;
	string s2;

	if(type_1 != (int)INTTYPE_TOK) { s1 = "REAL"; }
	else { s1 = "INTEGER"; }

	if(type_2 != (int)INTTYPE_TOK) { s2 = "REAL"; }
	else { s2 = "INTEGER"; }

	tmp_error[0] = '\0';

	sprintf(tmp_error,"ERROR --> [ Incompatible Types %s and %s ] Line: %d",s1.c_str(),s2.c_str(),line_num);
	string temp(tmp_error);
	errors.push_back(temp);
}

void store_error(char* error)
{
	var_stat=true;
	string temp(error);
	tok='0';
	errors.push_back(temp);
}

void yyerror(const char* error)
{ 
	tmp_error[0]='\0';
	tok='1';
	sprintf(tmp_error,"ERROR --> [ %s ] Line: %d",error,line_num);
	var_stat=true;
	string temp(tmp_error);
	tok='0';
	errors.push_back(temp);
}