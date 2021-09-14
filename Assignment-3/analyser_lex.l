%{
	#include<stdio.h>
	#include<string.h>
	#include <stdbool.h>

	#define TABLE_SIZE 2000 //HASHTABLE SIZE (HIGHER THE SIZE, LESSER NUMBER OF COLLISIONS)

	//HASHTABLE STRUCTURE (Type = 1 for Identifiers & Type = 2 for Numbers)
	typedef struct hash_table
	{
		char *name;
		int type;
		bool present;
	}tab;

	tab SYM_TAB[TABLE_SIZE];

	int hash_code(char *token);
	int install_id(char *token);
	int install_num(char *token);

	int line_num = 1;

	FILE *symbols;
%}
  
%%

"PROGRAM"	{ return 1; }
"VAR"		{ return 2; }
"BEGIN"		{ return 3; }
"END"		{ return 4; }
"END."		{ return 5; }
"INTEGER"	{ return 6; }
"FOR"		{ return 7; }	
"READ"		{ return 8; }
"WRITE"		{ return 9; }
"TO"		{ return 10; }
"DO"		{ return 11; }
";"			{ return 12; }
":"			{ return 13; }
","			{ return 14; }
":="		{ return 15; }
"+"			{ return 16; }
"-"			{ return 17; }
"*"			{ return 18; }
"DIV"		{ return 19; }
"("			{ return 20; }
")"			{ return 21; }

[A-Z]([A-Z0-9])* 	{ install_id(yytext);
					  return 22;
					}

[0-9]+				{ install_num(yytext);
					  return 23; }

" "					{}

"\n"		{ ++line_num; }

%%
  
int yywrap(void)
{
	return 1;
}

//FUNCTION TO GENERATE HASH CODE FROM STRING (djb2 Algorithm)
int hash_code(char *token)
{
    unsigned long code = 5381;
    int i;

    char* temp = (char *)malloc(strlen(token)+1);
    strcpy(temp,token);

    while(i = *temp++) { code = ((code << 5) + code) + i; }
    code = code % ((unsigned long)TABLE_SIZE);

    return (int)code;
}

//ADDS IDENTIFIER TO HASH-TABLE
int install_id(char *token)
{
	int index = hash_code(token);
	printf("%d\n",index);
	if(SYM_TAB[index].present)
	{
		printf("TOKEN ALREADY IN SYMBOLS TABLE!\n");
	}
	else
	{
		SYM_TAB[index].name = (char *)malloc(strlen(token)+1);
		strcpy(SYM_TAB[index].name,token);

		SYM_TAB[index].type = 1;
		SYM_TAB[index].present = true;

		fprintf(symbols,"%d \t\t %s \t\t %d \n",index,SYM_TAB[index].name,SYM_TAB[index].type);
		printf("TOKEN ADDED IN SYMBOLS TABLE!\n");
		return index;
	}
}

//ADDS NUMBER TO HASH-TABLE
int install_num(char *token)
{
	int index = hash_code(token);
	printf("%d\n",index);
	if(SYM_TAB[index].present)
	{
		printf("TOKEN ALREADY IN SYMBOLS TABLE!\n");
	}
	else
	{
		SYM_TAB[index].name = (char *)malloc(strlen(token)+1);
		strcpy(SYM_TAB[index].name,token);

		SYM_TAB[index].type = 2;
		SYM_TAB[index].present = true;

		fprintf(symbols,"%d \t\t %s \t\t %d \n",index,SYM_TAB[index].name,SYM_TAB[index].type);
		printf("TOKEN ADDED IN SYMBOLS TABLE!\n");
		return index;
	}
}

int main()
{   
	FILE *input = fopen("input.txt","r");
	FILE *output = fopen("output.txt","w");
	symbols = fopen("symbols.txt","w");

	if(input == NULL)
	{
		printf("Error Opening Input File!");
		exit(1);
	}
	else { yyin = input; }

	printf("\n");
	printf(" Line\tToken\tToken Specifier\n");
	fprintf(output," Line\tToken\tToken Specifier\n\n");
	fprintf(symbols,"Hash Index\tToken\t\tType\n\n");

	int ret_val = 0;
	while(ret_val = yylex())
	{
		if(ret_val == 22)
		{
			printf("  %d \t %d \t ^%s \n",line_num,ret_val,yytext);
			fprintf(output,"  %d \t %d \t ^%s \n",line_num,ret_val,yytext);
		}
		else if(ret_val == 23)
		{
			printf("  %d \t %d \t #%s \n",line_num,ret_val,yytext);
			fprintf(output,"  %d \t %d \t #%s \n",line_num,ret_val,yytext);
		}
		else
		{
			printf("  %d \t %d \t \n",line_num,ret_val);
			fprintf(output,"  %d \t %d \t \n",line_num,ret_val);
		}
	}

    return 0;
}