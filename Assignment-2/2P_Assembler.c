#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#define LINE_LIM 5000
#define INSTRCTN_LIM 256
#define OPRND_LIM 3

typedef struct opt_field
{
    char* operation;
    char* operand;
    char* opcode;
    char* format;
}opt;

typedef struct input_field
{
	char* name;
	char* operator;
	char* operand[OPRND_LIM];
  char extend;
	char* comment;
}token;

typedef struct symbol_field
{
	char symbol[20];
	int addr;
}symt;

typedef struct literal_field
{
	char literal[20];
	int addr;
}lit;

typedef struct object_field
{
    char type;
    char record;
    char* mod[OPRND_LIM];
    int instruction;
    int locctr;
    int format;
    char opt[OPRND_LIM];
    char use_ref;
}obj;

opt* op_table[INSTRCTN_LIM];
token* token_table[LINE_LIM];
obj* objstruct[LINE_LIM];
symt sym_table[LINE_LIM];
lit literal_table[LINE_LIM];

static int input_idx;
static int token_idx;
static int obj_idx;
static int sym_idx;
static int lit_idx;
static int locctr;
static int opt_idx;
static int sect_cnt;
static int lit_cnt;
char* input_data[LINE_LIM];
char* ref_list[OPRND_LIM];

void opt_file(char *file)
{
  printf("------< Loading Op-Table File >------\n");

	FILE* optable;
  optable = fopen(file,"r");
  if(optable == NULL)
   {
      printf("Error Opening Op-Table!");
      exit(1);
   }

	char line[100] = {'\0'};
	char* word;

	for(int i=0; i<INSTRCTN_LIM; i++)
  {
		fgets(line,sizeof(line),optable);
		if(feof(optable)) { break; }
		op_table[i] = (opt*)malloc(sizeof(opt));
		word = strtok(line,"\t");

		op_table[i]->operation = (char*)malloc(strlen(word)+1);
		strcpy(op_table[i]->operation,word);
		word = strtok(NULL,"\t");
		op_table[i]->operand = (char*)malloc(strlen(word)+1);
		strcpy(op_table[i]->operand,word);
		word = strtok(NULL,"\t");
		op_table[i]->format = (char*)malloc(strlen(word)+1);
		strcpy(op_table[i]->format,word);
		word = strtok(NULL,"\t");
		word[strlen(word)-1] = '\0';
		op_table[i]->opcode = (char*)malloc(strlen(word)+1);
		strcpy(op_table[i]->opcode,word);
		opt_idx++;
	}

  printf("---------< Op-Table Loaded >---------\n\n");

  for(int i=0; i<opt_idx; i++)
  {
    printf("%s\t%s\t%s\t%s\n",op_table[i]->operation,op_table[i]->opcode,op_table[i]->operand,op_table[i]->format);
  }
  printf("\n");
}

void input_file(char *file)
{
  printf("------< Loading Input File >------\n");

	FILE* input;
  input = fopen(file,"r");
  if(input == NULL)
   {
      printf("Error Opening Input-File!");
      exit(1);
   }

	char line[100];
	for(int i=0; i < LINE_LIM; i++)
  {
		fgets(line,sizeof(line),input);
		if(feof(input))
    {
			input_data[i] = (char*)malloc(strlen(line) + 1);
			strcpy(input_data[i],line);
			input_idx++;
			break;
		}
		input_data[i] = (char*)malloc(strlen(line) + 1);
		strcpy(input_data[i],line);
		input_idx++;
	}
  printf("------< Input-File Loaded >-------\n\n");

  /*for(int i=0; i<input_idx+1; i++)
  {
    printf("%s",input_data[i]);
  }*/
  printf("\n");
}

int read_line(char *line)
{
	char* tok;
	char* oprtr;
	token_table[token_idx] = (token*)malloc(sizeof(token));
	token_table[token_idx]->name = NULL;
	token_table[token_idx]->operator= NULL;
 	token_table[token_idx]->operand[0] = NULL;
	token_table[token_idx]->operand[1] = NULL;
	token_table[token_idx]->operand[2] = NULL;
	token_table[token_idx]->comment = NULL;

	if(line[0] == '.') { return 1;}

	if(line[0] == '\t')
  {
		token_table[token_idx]->name = NULL;
		tok = strtok(line,"\t\n");
	}
	else
  {
		tok = strtok(line,"\t\n");
		token_table[token_idx]->name = (char*)malloc(strlen(tok) + 1);
		strcpy(token_table[token_idx]->name,tok);
		tok = strtok(NULL,"\t\n");
	}

	if(tok == NULL) { return 1; }

	token_table[token_idx]->operator = (char*)malloc(strlen(tok) + 1);
	strcpy(token_table[token_idx]->operator, tok);

	if(strcmp(tok,"RSUB") == 0)
  {
		tok = strtok(NULL,"\t");
		if(tok != NULL)
    {
			token_table[token_idx]->comment = (char*)malloc(strlen(tok) + 1);
			strcpy(token_table[token_idx]->comment,tok);
		}
		token_idx++;
		return 1;
	}

	tok = strtok(NULL,"\t");
	oprtr = tok;
	tok = strtok(NULL,"\t\n");

	if(tok != NULL)
  {
		token_table[token_idx]->comment = (char*)malloc(strlen(tok) + 1);
		strcpy(token_table[token_idx]->comment,tok);
	}

	char* subtok = strtok(oprtr,",\n");
	for (int i=0; i < OPRND_LIM; i++) { token_table[token_idx]->operand[i] = NULL; }

	for(int i=0; i < OPRND_LIM; i++)
  {
		if(subtok == NULL) { break; }
		token_table[token_idx]->operand[i] = (char*)malloc(strlen(subtok) + 1);
		strcpy(token_table[token_idx]->operand[i],subtok);
		subtok = strtok(NULL,",\n");
	}
	token_idx++;
  return 1;
}

void pass1()
{
  int r;
	for(int i=0; i<input_idx; i++)
  {
    r = read_line(input_data[i]);
	}
}

int search_opcode(char *str)
{
	char* pstr;
	if('A' <= str[0] && str[0] <= 'Z') { pstr = str; }
	else { pstr = str + 1; }

	for(int i=0; i<opt_idx; i++)
  {
		if(strcmp(pstr,op_table[i]->operation) == 0) { return i; }
	}
	return -1;
}

int search_symtable(char* name)
{
	for(int i=0; i<sym_idx; i++)
  {
		if(strcmp(sym_table[i].symbol,name) == 0) { return sym_table[i].addr; }
  }
	return -1;
}

int search_symtable_sect(char* name,int sect)
{
	int cnt=0;
	for(int i=0; i<sym_idx; i++)
  {
		if(cnt == sect)
    {
			if(strcmp(sym_table[i].symbol,name) == 0) { return sym_table[i].addr; }
		}
		if(sym_table[i].symbol[0] == '\0') { cnt++; }
	}
	return -1;
}

int search_ltable(char* literal)
{
	for(int i=0; i<lit_idx; i++)
  {
		if(strcmp(literal_table[i].literal,literal) == 0) { return literal_table[i].addr; }
	}
	return -1;
}

int search_reftable(char* ref)
{
	for(int i=0; i<OPRND_LIM; i++)
  {
		if(ref_list[i] == NULL) { break; }
		if(strcmp(ref_list[i],ref) == 0) { return i; }
	}
	return -1;
}

void make_symtab()
{
	int temp_const=0,temp_lit=0;
	for(int i=0; i<token_idx; i++)
  {
		objstruct[obj_idx] = (obj*)malloc(sizeof(obj));

    objstruct[obj_idx]->type = 0;
    objstruct[obj_idx]->use_ref = 0;
    objstruct[obj_idx]->mod[0]=NULL;
		objstruct[obj_idx]->mod[1]=NULL;
		objstruct[obj_idx]->mod[2]=NULL;

		objstruct[obj_idx]->record = 'E';
		objstruct[obj_idx]->instruction = -1;
		objstruct[obj_idx]->locctr = -1;
		objstruct[obj_idx]->format = 0;

		if(token_table[i]->name != NULL)
    {
			if(strcmp(token_table[i]->operator,"CSECT") == 0)
      {
				locctr = 0;
				temp_const = i;
				memset(sym_table[sym_idx].symbol,'\0',sizeof(sym_table[sym_idx].symbol));
				sym_idx++;
			}

			if (strcmp(token_table[i]->operator,"EQU") == 0) {
				if (strcmp(token_table[i]->operand[0], "*")==0)
        {
					strcpy(sym_table[sym_idx].symbol, token_table[i]->name);
					sym_table[sym_idx].addr = locctr;
					sym_idx++;
				}
				else
        {
					char tmp[20];
					strcpy(tmp, token_table[i]->operand[0]);
					char* tok = strtok(tmp, "+-");
					int idx1 = search_symtable(tok);
					tok = strtok(NULL, "+-");
					int idx2 = search_symtable(tok);
					strcpy(sym_table[sym_idx].symbol,token_table[i]->name);
					sym_table[sym_idx].addr = idx1 - idx2;
					sym_idx++;
				}
			}
			else
      {
				strcpy(sym_table[sym_idx].symbol,token_table[i]->name);
				sym_table[sym_idx].addr = locctr;
				sym_idx++;
			}
		}

		if(strcmp(token_table[i]->operator,"END") == 0)
    {
			char* plit = NULL;
			char* cont;
			for(int j = temp_lit; j <= i; j++)
      {
				char* opr = token_table[j]->operand[0];
				if(opr != NULL && opr[0] == '=')
        {
					plit = token_table[j]->operand[0];
					char tmp[20];
					strcpy(tmp, plit);
					cont = strtok(tmp, "'");
					cont = strtok(NULL, "'");
					int adr = search_ltable(cont);
					if(adr < 0)
          {
						strcpy(literal_table[lit_idx].literal, cont);
						literal_table[lit_idx].addr = locctr;
						lit_idx++;
						if(plit != NULL && plit[1] == 'C')
            {
							int len = strlen(cont);
							locctr += len;
							objstruct[obj_idx]->type = 'C';
						}
						else if (plit != NULL && plit[1] == 'X')
            {
							int len = strlen(cont);
							locctr += len / 2;
							objstruct[obj_idx]->type = 'X';
						}
					}
				}
			}
		}
		else if(strcmp(token_table[i]->operator,"START") == 0) { objstruct[obj_idx]->record = 'H'; }

		char* opt = token_table[i]->operator;
    int idx = search_opcode(opt);
		if(idx > 0)
    {
			int format = atoi(op_table[idx]->format);
			if (format == 1 || format == 2)
      {
				locctr += format;
			}
			else
      {
				if(opt[0] == '+') { locctr += 4; }
				else { locctr += 3; }
			}
		}
		else
    {
			if(strcmp(opt,"BYTE") == 0)
      {
				char tmp[20];
				strcpy(tmp, token_table[i]->operand[0]);
				char * tok = strtok(tmp, "'");
				int len = 0;
				if(strcmp(tok,"C") == 0)
        {
					tok = strtok(NULL,"'");
					int len = strlen(tok);
					locctr += len;
				}
				else if(strcmp(tok,"X") == 0)
        {
					tok = strtok(NULL, "'");
					len = strlen(tok);
					locctr += len / 2;
				}
			}

			else if(strcmp(opt,"WORD") == 0) { locctr += 3; }
			else if(strcmp(opt,"RESB") == 0)
      {
				int num = atoi(token_table[i]->operand[0]);
				locctr += 1 * num;
			}
			else if(strcmp(opt,"RESW") == 0)
      {
				int num = atoi(token_table[i]->operand[0]);
				locctr += 3 * num;
			}
			else if(strcmp(token_table[i]->operator,"LTORG") == 0)
      {
				char* plit = NULL;
				char* cont = NULL;
				int cur_locctr = locctr;
				for (int j = temp_const; j <= i; j++)
        {
					char* tmp = token_table[j]->operand[0];
					if(tmp != NULL && tmp[0] == '=')
          {
						plit = token_table[j]->operand[0];
						break;
					}
				}

				if(plit != NULL && plit[1] == 'C')
        {
					char tmp[10];
					strcpy(tmp, plit);
					cont = strtok(tmp, "'");
					cont = strtok(NULL, "'");
					int len = strlen(cont);
					locctr += len;
					objstruct[obj_idx]->type = 'C';
				}
				else if(plit != NULL && plit[1] == 'X')
        {
					char tmp[10];
					strcpy(tmp, plit);
					cont = strtok(tmp, "'");
					cont = strtok(NULL, "'");
					int len = strlen(cont);
					locctr += len / 2;
					objstruct[obj_idx]->type = 'X';
				}

				int adr = search_ltable(cont);

				if(adr < 0 && plit!=NULL)
        {
					strcpy(literal_table[lit_idx].literal,cont);
					literal_table[lit_idx].addr = cur_locctr;
					lit_idx++;
				}
				temp_lit = i+1;
			}
		}
		objstruct[obj_idx]->locctr = locctr;
		objstruct[obj_idx]->format = 0;
		obj_idx++;
	}
}

void symtab_out(char *file)
{
	FILE* out;
  out = fopen(file,"w");
	make_symtab();

	for(int i=0; i<sym_idx; i++)
  {
		if(sym_table[i].symbol[0] != '\0')
    {
			fprintf(out,"%s\t\t%X\n",sym_table[i].symbol,sym_table[i].addr);
		}
		else { fputs("\n",out); }
	}
}

void literaltab_out(char* file)
{
	FILE* out;
  out = fopen(file,"w");

	for(int i=0; i<lit_idx; i++)
  {
		if(* sym_table[i].symbol != '\0')
    {
			fprintf(out,"%s\t%X\n",literal_table[i].literal,literal_table[i].addr);
		}
	}
}

int set_nixbpe(token* tok)
{
	tok->extend = 0;
	char* op = tok->operator;
	char* Xbit = tok->operand[1];
	char* operand = tok->operand[0];
	int idx = -1;
	if((idx = search_opcode(op))> 0)
  {
		if(strcmp(op,"RSUB") == 0)
    {
			tok->extend = 48;
			return idx;
		}

		if(operand[0] == '#')
    {
			tok->extend += 16;
			operand = operand + 1;
		}
		else if(operand[0] == '@')
    {
			tok->extend += 32;
			operand = operand + 1;
		}
		else { tok->extend += 48; }

		if(op[0] == '+')
    {
			tok->extend += 1;
			tok->extend -= 2;
		}

		if (Xbit !=NULL && strcmp(Xbit, "X") == 0) { tok->extend += 8; }
		tok->extend += 2;
		if(isdigit(operand[0]) != 0) { tok->extend -= 2; }

		return idx;
	}
	return idx;
}

void make_instruction(int i)
{
	int idx;
	idx = set_nixbpe(token_table[i]);
	if(idx > 0)
  {
		opt* pinst = op_table[idx];
		char* format = pinst->format;
		char* stop;
		int opcode = (int)strtol(pinst->opcode, &stop, 16);

		if(strcmp(format,"1") == 0)
    {
			objstruct[i]->record = 'T';
			objstruct[i]->instruction = opcode;
			objstruct[i]->format = 1;
		}
 		else if(strcmp(format,"2") == 0)
    {
			char r[2];
			memset(r,0,sizeof(r));

			for(int j=0; j<atoi(pinst->operand); j++) { r[j] = token_table[i]->operand[j][0]; }

			for(int j=0; j<2; j++)
      {
				if (r[j] != 0)
        {
					switch (r[j])
					{
					case 'A':
						r[j] = 0;
						break;
					case 'X':
						r[j] = 1;
						break;
					case 'L':
						r[j] = 2;
						break;
					case 'B':
						r[j] = 3;
						break;
					case 'S':
						r[j] = 4;
						break;
					case 'T':
						r[j] = 5;
						break;
					case 'F':
						r[j] = 6;
						break;
					}
				}
			}
			objstruct[i]->record = 'T';
			objstruct[i]->instruction = opcode << 8 | r[0] << 4 | r[1];
			objstruct[i]->format = 2;
		}

		else if(strcmp(format,"3/4") == 0)
    {
			char* operator = token_table[i]->operator;
			char* operand = token_table[i]->operand[0];
			if(operator[0] == '+')
      {
				int nix = token_table[i]->extend;
				int adr=0;
				if(operand[0] == '#' || operand[0] == '@') { operand = operand + 1; }
				int idx = search_reftable(operand);
				if(idx >= 0)
        {
					adr = 0;
					objstruct[i]->use_ref = 1;
					objstruct[i]->mod[0] = operand;
					objstruct[i]->opt[0] = '+';
				}
				else if(search_symtable_sect(operand,sect_cnt) > 0) { adr = search_symtable_sect(operand, sect_cnt); }
				objstruct[i]->record = 'T';
				objstruct[i]->instruction = (opcode << 24) | (nix<<20) | (adr);
				objstruct[i]->format = 4;
			}
			else
      {
				int nix = token_table[i]->extend;
				int adr = 0;
				int symtab = 0;
				int pc = 0;
				if(objstruct[i]->locctr != -1) { pc = objstruct[i]->locctr; }
				if(operand!=NULL)
        {
					if(operand[0] == '#' || operand[0] == '@') { operand = operand + 1; }
					if((nix & 2) == 0) { adr = atoi(operand); }
					else if(operand[0] == '=')
          {
						char tmp[10];
						strcpy(tmp,operand);
						char* tok = strtok(tmp,"'");
						tok = strtok(NULL,"'");
						symtab = search_ltable(tok);
						adr = symtab-pc;
					}
					else
          {
						symtab = search_symtable_sect(operand,sect_cnt);
						adr = symtab-pc;
					}
				}
				objstruct[i]->record = 'T';
				objstruct[i]->instruction = (opcode << 16) | (nix << 12) | (0xFFF&adr);
				objstruct[i]->format = 3;
			}
		}
	}
	else
  {
		char* op = token_table[i]->operator;
		char* operand = token_table[i]->operand[0];
		if(strcmp(op,"EXTDEF") == 0) { objstruct[i]->record = 'D'; }
		else if(strcmp(op,"EXTREF") == 0)
    {
			for(int j=0; j<OPRND_LIM; j++)
      {
				ref_list[j] = token_table[i]->operand[j];
				objstruct[i]->record = 'R';
			}
		}
		else if(strcmp(op, "CSECT") == 0)
    {
			objstruct[i]->record = 'H';
			sect_cnt++;
		}
		else if(strcmp(op, "WORD") == 0 || strcmp(op, "BYTE") == 0)
    {
			char* opr = operand;
			char tmp[20];
			strcpy(tmp, opr);
			char* stop;

			if(opr[0] == 'C' || opr[0] == 'X')
      {
				char* tok = strtok(tmp,"'");
				tok = strtok(NULL,"'");
				if (opr[0] == 'C')
				{
					int len = strlen(tok);
					for(int j=0; j<len; j++)
          {
						objstruct[i]->instruction = (objstruct[i]->instruction << 8) | tmp[j];
						objstruct[i]->format += 1;
						objstruct[i]->record = 'T';
					}
				}
				else
        {
					objstruct[i]->instruction = (int)strtol(tok,&stop,16);
					objstruct[i]->format += 1;
					objstruct[i]->record = 'T';
				}
			}
			else
      {
				if(strchr(opr,'+') != NULL || strchr(opr, '-') != NULL)
        {
					if(strchr(opr,'+') != NULL)
          {
						char* tok = strtok(tmp,"+");
						char* op1 = tok;
						tok = strtok(NULL,"+");
						char* op2 = tok;
						int idx1 = search_reftable(op1);
						objstruct[i]->mod[0] = op1;
						int idx2 = search_reftable(op2);
						objstruct[i]->mod[1] = op2;
						int TA1 = 0;
						int TA2 = 0;

						if(idx1 < 0)
            {
							int sym_adr = search_symtable(op1);
							TA1 = sym_adr;
						}
						else
            {
							objstruct[i]->opt[0] = '+';
							objstruct[i]->use_ref = 1;
						}

						if(idx2 < 0)
            {
							int sym_adr = search_symtable(op2);
							TA2 = sym_adr;
						}
						else
            {
							objstruct[i]->opt[1] = '+';
							objstruct[i]->use_ref = 1;
						}
						objstruct[i]->record = 'T';
						objstruct[i]->instruction = TA1 + TA2;
						objstruct[i]->format = 3;
					}
					else if(strchr(opr,'-') != NULL)
          {
						char* tok = strtok(tmp,"-");
						char* op1 = tok;
						tok = strtok(NULL, "-");
						char* op2 = tok;
						int idx1 = search_reftable(op1);
						objstruct[i]->mod[0] = (char*)malloc(sizeof(strlen(op1)));
						strcpy(objstruct[i]->mod[0], op1);
						int idx2 = search_reftable(op2);
						objstruct[i]->mod[1] = (char*)malloc(sizeof(strlen(op2)));
						strcpy(objstruct[i]->mod[1], op2);
						int TA1 = 0;
						int TA2 = 0;

						if(idx1 < 0)
            {
							int sym_adr = search_symtable(op1);
							TA1 = sym_adr;
						}
						else
            {
							objstruct[i]->opt[0] = '+';
							objstruct[i]->use_ref = 1;
						}

						if(idx2 < 0)
            {
							int sym_adr = search_symtable(op2);
							TA2 = sym_adr;
						}
						else
            {
							objstruct[i]->opt[1] = '-';
							objstruct[i]->use_ref = 1;
						}
						objstruct[i]->record = 'T';
						objstruct[i]->instruction = TA1-TA2;
						objstruct[i]->format = 3;
					}
				}
				else { objstruct[i]->instruction = (int)strtol(opr, &stop, 16); }
			}
		}
		else if(strcmp(op,"LTORG") == 0 || strcmp(op,"END") == 0)
    {
			objstruct[i]->instruction = 0;
			char* ptr = literal_table[lit_cnt].literal;
			char tmp[10];
			strcpy(tmp, ptr);
			int len = strlen(tmp);
			char* stop;

			if(objstruct[i]->type == 'C')
      {
				objstruct[i]->record = 'T';
				for (int j=0; j<len; j++)
        {
					objstruct[i]->instruction = (objstruct[i]->instruction << 8) | tmp[j];
					objstruct[i]->format += 1;
				}
			}
			else
      {
				objstruct[i]->record = 'T';
				objstruct[i]->instruction = (int)strtol(tmp,&stop,16);
				objstruct[i]->format += 1;
			}
			lit_cnt++;
		}
	}
}

void pass2()
{
	int sect_cnt = 0;
	for(int i=0; i<token_idx; i++) { make_instruction(i); }
}

int addr_len(int sect)
{
	int s_cnt = 0;
	int start_ad = 0;
	int end_ad = 0;

	for(int i=0; i<obj_idx; i++)
  {
		if(s_cnt == sect)
    {
			if(objstruct[i]->record == 'H') { start_ad = objstruct[i]->locctr; }

			if(i<obj_idx-1 && objstruct[i + 1]->record == 'H')
      {
				end_ad = objstruct[i]->locctr;
				break;
			}
			else if(i == obj_idx - 1) { end_ad = objstruct[i]->locctr; }
		}
		else
    {
			if(i<obj_idx && objstruct[i + 1]->record == 'H') { s_cnt++; }
		}
	}
	return end_ad - start_ad;
}

void objcode_out(char *file)
{
  FILE* obj_out;
  obj_out = fopen(file,"w");

	char record;
	int loc, inst, format;
  int sector = 0;
	int end = objstruct[0]->locctr;


	sect_cnt = 0;
	lit_cnt = 0;

	for(int i=0; i<obj_idx; i++)
  {
		record = objstruct[i]->record;
		loc = objstruct[i]->locctr;
		inst = objstruct[i]->instruction;
		format = objstruct[i]->format;

		if(record == 'H')
    {
			if(i != 0) { fputs("\n\n",obj_out); }
			fprintf(obj_out,"%c %s",'H',token_table[i]->name);
      fprintf(stdout,"%c%c%-6s",'H',' ',token_table[i]->name);
//      fprintf(obj_out,"%c",' ');
      fprintf(stdout,"%c",' ');
			fprintf(obj_out," %06X",objstruct[i]->locctr);
      fprintf(stdout,"%06X",objstruct[i]->locctr);
//      fprintf(obj_out,"%c",' ');
      fprintf(stdout,"%c",' ');
			int len=addr_len(sect_cnt++);
			fprintf(obj_out," %06X",len);
      fprintf(stdout,"%06X",len);
			sector = i;
		}

		else if(record=='D')
    {
			fputs("\n",obj_out);
      printf("\n");
			fputs("D",obj_out);
      printf("D ");
			for(int j=0; j < OPRND_LIM; j++)
      {
				if(token_table[i]->operand[j] != NULL)
        {
					int sym_loc = search_symtable(token_table[i]->operand[j]);
					fprintf(obj_out," %s %06X",token_table[i]->operand[j],sym_loc);
          fprintf(stdout,"%s %06X ",token_table[i]->operand[j],sym_loc);
				}
			}
		}

		else if(record=='R')
    {
			fputs("\n",obj_out);
      printf("\n");
			fputs("R",obj_out);
      printf("R ");
			for(int j=0; j<OPRND_LIM;j++)
      {
				if(token_table[i]->operand[j] != NULL)
        {
          fprintf(obj_out," %s",token_table[i]->operand[j]);
          fprintf(stdout,"%-6s ",token_table[i]->operand[j]);
        }
			}
		}

		else if(record == 'T')
    {
			fputs("\n",obj_out);
      printf("\n");
			fputs("T",obj_out);
      printf("T ");
			int num=0, sum=0;

			while((i < obj_idx) && (objstruct[i]->record == 'T'))
      {
				if (format == 0) { sum += objstruct[i]->format; }
				else { sum += objstruct[i]->format; }

				if(sum > 30)
        {
					sum -= objstruct[i]->format;
					break;
				}
				num++;
				i++;
			}

			i -= num;

			fprintf(obj_out," %06X %02X",objstruct[i-1]->locctr,sum);
      fprintf(stdout,"%06X %02X ",objstruct[i-1]->locctr,sum);

			for(int j=0; j<num; j++)
      {
				format = objstruct[i]->format;
				if(format==1) { fprintf(obj_out," %02X",objstruct[i]->instruction); fprintf(stdout,"%02X ",objstruct[i]->instruction); }
				if(format==2) { fprintf(obj_out," %04X",objstruct[i]->instruction); fprintf(stdout,"%04X ",objstruct[i]->instruction); }
				if(format==3) { fprintf(obj_out," %06X",objstruct[i]->instruction); fprintf(stdout,"%06X ",objstruct[i]->instruction); }
				if(format==4) { fprintf(obj_out," %08X",objstruct[i]->instruction); fprintf(stdout,"%08X ",objstruct[i]->instruction); }
				i++;
			}
			i--;
		}

		if((i != obj_idx-1 && objstruct[i + 1]->record == 'H')||i== obj_idx-1)
    {
			for (int j = sector; j <= i; j++)
      {
				if (objstruct[j]->use_ref == 1 || objstruct[j]->format == 4)
        {
					int startidx = 0;
					int target_bit = 0;
					if(objstruct[j]->format==4)
          {
						startidx = (int)(objstruct[j]->locctr - 2.5);
						target_bit = 5;
					}
					else
          {
						startidx = objstruct[j]->locctr - 3;
						target_bit = 6;
					}
					char* ref;
					char opt;
					for(int k = 0; k < OPRND_LIM; k++)
          {
						if(objstruct[j]->mod[k] == NULL)
            {
							if(k == 0)
              {
								fprintf(obj_out,"\nM %06X %02d",startidx,target_bit);
                fprintf(stdout,"\nM %06X %02d",startidx,target_bit);
							}
							break;
						}
						ref = objstruct[j]->mod[k];
						opt = objstruct[j]->opt[k];
						if(ref != NULL)
            {
							fprintf(obj_out,"\nM %06X %02d %c%s",startidx,target_bit,opt,ref);
              fprintf(stdout,"\nM %06X %02d%c%s",startidx,target_bit,opt,ref);
						}
						else
            {
							fprintf(obj_out,"\nM %06X%02d%-6s",startidx,target_bit,ref);
              fprintf(stdout,"\nM %06X%02d%-6s",startidx,target_bit,ref);
						}
					}
				}
			}

			if(end == 0)
      {
				fprintf(obj_out,"\nE %06X",end);
        fprintf(stdout,"\nE %06X",end);
				end = -1;
			}
			else { fprintf(obj_out,"\nE"); fprintf(stdout,"\nE"); }
      fprintf(stdout,"\n\n");
		}
	}
}

int main()
{
  printf("\n");
	opt_file("OpTable.txt");
  input_file("input.txt");
  printf("------< Starting PASS-1 >------\n");
  pass1();
  printf("--------< PASS-1 DONE! >-------\n\n");
  symtab_out("Symbols_Table.txt");
  printf("------< Symbols-Table Generated! ========> [Symbols_Table.txt]\n\n");
  literaltab_out("Literals.txt");
  printf("------< Literals-Table Generated! ========> [Literals.txt]\n\n");
  printf("------< Starting PASS-2 >------\n");
  pass2();
  printf("--------< PASS-2 DONE! >-------\n\n");
  printf("-x-x-x-x-x-x-x-x-x-x- OBJECT-CODE -x-x-x-x-x-x-x-x-x-x-\n\n");
  objcode_out("Object_Code.txt");
  printf("------< Object-Code Generated! ========> [Object_Code.txt]\n");
	return 0;
}
