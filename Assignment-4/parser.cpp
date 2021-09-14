#include<bits/stdc++.h>
using namespace std;

char* tmp_error;
vector<string> tokens;
vector<string> errors;
vector<string> keyw;

extern int yyparse(void);
extern FILE *yyin;

int main(int argc, char** argv)
{
	if(argc == 1)
	{
		cout<<endl<<"No File-Name Argument Detected! Exiting......."<<endl;
		exit(1);
	}
	else
	{ 
		FILE *input = fopen(argv[1],"r");
		if(input == NULL)
		{
			printf("\nError Opening Input File!\n");
			exit(1);
		}
		else { yyin = input; }
	}

	tmp_error = (char* )malloc(sizeof(char)*512);
	tmp_error[0] = '\0';

	for(int j=0; j<13; j++)
	{
		if(j==0) { keyw.push_back("PROGRAM"); }
		if(j==1) { keyw.push_back("VAR"); }
		if(j==2) { keyw.push_back("BEGIN"); }
		if(j==3) { keyw.push_back("END"); }
		if(j==4) { keyw.push_back("END."); }
		if(j==5) { keyw.push_back("INTEGER"); }
		if(j==6) { keyw.push_back("REAL"); }
		if(j==7) { keyw.push_back("FOR"); }
		if(j==8) { keyw.push_back("READ"); }
		if(j==9) { keyw.push_back("WRITE"); }
		if(j==10) { keyw.push_back("TO"); }
		if(j==11) { keyw.push_back("DO"); }
		if(j==12) { keyw.push_back("DIV"); }
	}

	if(yyparse()==1) { printf("\n----ERROR IN PARSING INPUT FILE!----\n"); }
	else { printf("\n----[ PARSE SUCCESSFUL ]----\n"); }

	if(errors.size()==0) { printf("\n----[ NO ERRORS FOUND ]----\n"); }
	else
	{
		for(int e=0; e<errors.size(); e++)
		{
			cout<<errors[e];
			cout<<endl; 
		}
	}

	return 0;
}