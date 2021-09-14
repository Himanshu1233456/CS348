#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ESYM_LIM 100

static char mem[5000][32];
static int row_cnt;
static char obj_code[5000]="";

typedef struct esym_field
{
    char* name;
    char* sym;
    int addr;
    int lth;
}esym;

esym* esym_table[ESYM_LIM];
char* input_data[1000];
char* p2_data[1000];
static int input_idx;
static int p2_idx;
static int esym_idx;
static int indx;
static int store1;
static int store2;

int hex_dec(char* str)
{
    str=strrev(str);
    int idx=0,dec=0;
    while(str[idx]!='\0')
    {
      if(str[idx]=='A' || str[idx]=='B' || str[idx]=='C' || str[idx]=='D' || str[idx]=='E' || str[idx]=='F') { dec+=(int)(str[idx]-55)*(pow(16,idx)); }
      else if(str[idx]=='a' || str[idx]=='b' || str[idx]=='c' || str[idx]=='d' || str[idx]=='e' || str[idx]=='f') { dec+=(int)(str[idx]-87)*(pow(16,idx)); }
      else { dec+=(int)(str[idx]-48)*(pow(16,idx)); }
      idx++;
    }
    str=strrev(str);
    return dec;
}

int search_sym(char* symbol)
{
  for(int i=0; i<esym_idx; i++)
  {
    if(strcmp(esym_table[i]->name,symbol)==0 || strcmp(esym_table[i]->sym,symbol)==0)
    {
      return esym_table[i]->addr;
    }
  }
}

int main()
{
  FILE *in,*p2,*out;
  in = fopen("Object_Code.txt","r");
  out = fopen("Esym_Table.txt","w");

  printf("\n");
  if(in==NULL) { printf("Error Opening Input-File!\n"); exit(1); }
  else { printf("Reading Input-File!\n\n"); }

  char line[100];
  int inp=0;
  while(fgets(line,sizeof(line),in)!=NULL)
  {
    if(line[0]=='\n') { continue; }
    else
    {
      input_data[inp] = (char*)malloc(strlen(line)+1);
      strcpy(input_data[inp],line);
      inp++;
      input_idx++;
    }
  }
//  printf("%d\n",input_idx);
  fclose(in);

  int progaddr=0; //Change the Starting Address Here
  int csaddr;
  int cslth;
  int execaddr;

  //PASS-1 (Creates External-Symbols Table)
  csaddr = progaddr;
  for(int i=0; i<input_idx; i++)
  {
    char* tok;
    tok = strtok(input_data[i]," ");

    if(*tok=='H')
    {
      char* field[4];
      int idx=0;
      while(tok!=NULL)
      {
        field[idx] = (char*)malloc(strlen(tok)+1);
        strcpy(field[idx],tok);
        tok = strtok(NULL," ");
        idx++;
      }

      (field[3])[strcspn(field[3],"\n")]=0;
      cslth = hex_dec(field[3]);
      fprintf(out,"%s\t%c\t%04X\t%02X\n",field[1],'-',csaddr,cslth);

      esym_table[esym_idx] = (esym*)malloc(sizeof(esym));

      esym_table[esym_idx]->name = (char*)malloc(strlen(field[1]) + 1);
      strcpy(esym_table[esym_idx]->name,field[1]);
      esym_table[esym_idx]->sym = (char*)malloc(strlen("-") + 1);
      strcpy(esym_table[esym_idx]->sym,"-");
      esym_table[esym_idx]->addr = (int)malloc(sizeof(csaddr));
      esym_table[esym_idx]->addr=csaddr;
      esym_table[esym_idx]->lth = (int)malloc(sizeof(cslth));
      esym_table[esym_idx]->lth=cslth;

      esym_idx++;
    }

    else if(*tok=='D')
    {
      char* field[20];
      int idx=0;
      while(tok!=NULL)
      {
        field[idx] = (char*)malloc(strlen(tok)+1);
        strcpy(field[idx],tok);
        tok = strtok(NULL," ");
        idx++;
      }

      (field[idx-1])[strcspn(field[idx-1],"\n")]=0;

      for(int b=1; b<idx; b=b+2)
      {
        int val = csaddr + hex_dec(field[b+1]);
        fprintf(out,"%c\t%s\t%04X\t%c\n",'-',field[b],val,'-');

        esym_table[esym_idx] = (esym*)malloc(sizeof(esym));

        esym_table[esym_idx]->name = (char*)malloc(strlen("-") + 1);
        strcpy(esym_table[esym_idx]->name,"-");
        esym_table[esym_idx]->sym = (char*)malloc(strlen(field[b]) + 1);
        strcpy(esym_table[esym_idx]->sym,field[b]);
        esym_table[esym_idx]->addr = (int)malloc(sizeof(val));
        esym_table[esym_idx]->addr=val;
        esym_table[esym_idx]->lth = (int)malloc(sizeof(0));
        esym_table[esym_idx]->lth=0;

        esym_idx++;
      }
    }
    else if(*tok=='E') { csaddr += cslth; }
    else { continue; }
  }

  printf("-x-x-x-x-x- External-Symbols-Table -x-x-x-x-x-\n\n");
  printf("CSNAME\tSYMBOL\tLOC\tLENGTH\n\n");
  for(int a=0; a<esym_idx; a++)
  {
    printf("%s\t%s\t%04X\t%02X\n",esym_table[a]->name,esym_table[a]->sym,esym_table[a]->addr,esym_table[a]->lth);
  }
  printf("\n");
  printf("-x-x-x-x-x- Memory Mappings -x-x-x-x-x-\n\n");

  p2 = fopen("Object_Code.txt","r");

  char buffer[100];
  inp=0;
  while(fgets(buffer,sizeof(buffer),p2)!=NULL)
  {
    if(buffer[0]=='\n') { continue; }
    else
    {
      p2_data[inp] = (char*)malloc(strlen(buffer)+1);
      strcpy(p2_data[inp],buffer);
      inp++;
      p2_idx++;
    }
  }
  fclose(p2);

  //PASS-2 (Mapping)
  csaddr = progaddr;
  execaddr = progaddr;
  for(int i=0; i<p2_idx; i++)
  {
    char* tok;
    tok = strtok(p2_data[i]," ");

    if(*tok=='H')
    {
      char* field[4];
      indx=0;
      while(tok!=NULL)
      {
        field[indx] = (char*)malloc(strlen(tok)+1);
        strcpy(field[indx],tok);
        tok = strtok(NULL," ");
        indx++;
      }

      (field[3])[strcspn(field[3],"\n")]=0;
      cslth = hex_dec(field[3]);
    }

    else if(*tok=='T')
    {
      char* field[100];
      indx=0;
      while(tok!=NULL)
      {
        field[indx] = (char*)malloc(strlen(tok)+1);
        strcpy(field[indx],tok);
        tok = strtok(NULL," ");
        indx++;
      }
      (field[indx-1])[strcspn(field[indx-1],"\n")]=0;

      if(hex_dec(field[1]) - store1 != store2)
      {
        for(int c=0; c<(hex_dec(field[1])-store1)-7; c++) { strcat(obj_code,"x"); }
      }

      store1 = hex_dec(field[1]);
      store2 = hex_dec(field[2]);
      char oc[200]="";
      for(int b=3; b<indx; b++)
      {
        strcat(obj_code,field[b]);
        strcat(oc,field[b]);
      }
//      printf("%s\n",oc);

      int l=0;
      while(oc[l]!='\0')
      {
        l++;
      }

      int val = csaddr + hex_dec(field[1]);
//      printf("%06X <------ %s\n",val,oc);

      int loc_addr=0;
      for(int c=3; c<indx; c++)
      {
        printf("\t%s -------> %06X\n",field[c],csaddr+loc_addr+hex_dec(field[1]));
        if(strlen(field[c])==2) { loc_addr+=1; }
        else if(strlen(field[c])==4) { loc_addr+=2; }
        else if(strlen(field[c])==6) { loc_addr+=3; }
        else if(strlen(field[c])==8) { loc_addr+=4; }
      }
    }

    else if(*tok=='M')
    {
      char* field[4];
      indx=0;
      while(tok!=NULL)
      {
        field[indx] = (char*)malloc(strlen(tok)+1);
        strcpy(field[indx],tok);
        tok = strtok(NULL," ");
        indx++;
      }
      (field[3])[strcspn(field[3],"\n")]=0;

      int s=0;
      char symbol[10] = "";
      while((field[3])[s]!='\0')
      {
        symbol[s] = (field[3])[s+1];
        s++;
      }

      if((field[3])[0]=='+')
      {
        int loc = search_sym(symbol);
        int l = hex_dec(field[2]);
        int am = csaddr+hex_dec(field[1]);
        printf("\nModification: %s ------> %06X\n\n",field[1],am+loc);
      }
      else if((field[3])[0]=='-')
      {
        int loc = search_sym(symbol);
        int l = hex_dec(field[2]);
        int am = csaddr+hex_dec(field[1]);
        printf("\nModification: %s ------> %06X\n\n",field[1],am-loc);
      }
    }

    else if(*tok=='E')
    {
      char* field[2];
      int idx=0;
      while(tok!=NULL)
      {
        field[idx] = (char*)malloc(strlen(tok)+1);
        strcpy(field[idx],tok);
        tok = strtok(NULL," ");
        idx++;
      }
      if(idx==2)
      {
        (field[1])[strcspn(field[1],"\n")]=0;
        execaddr = csaddr + hex_dec(field[1]);
      }
      cslth += csaddr;
    }
    else { continue; }
  }

  for(int i=0; i<5000; i++)
  {
    for(int j=0; j<32; j++)
    {
      mem[i][j] = 'x';
    }
  }

  int cnt=0;
  for(int i=0; i<5000; i++)
  {
    if(obj_code[cnt]=='\0') { break; }
    for(int j=0; j<32 && obj_code[cnt]!='\0'; j++)
    {
      mem[i][j] = obj_code[cnt];
      cnt++;
    }
  }

//OUTPUT (PARTIAL) [Some Mappings Are At Higher Memory Addresses]
  printf("\n\n");
  printf("\tADDR\t\t    0  1  2  3    4  5  6  7    8  9  A  B    C  D  E  F");
  printf("\n\n");
  for(int i=0; i<5; i++)
  {
    printf("\t%04d\t\t",csaddr);
    for(int j=0; j<32; j++)
    {
      if(j%2==0) { printf(" "); }
      if(j%8==0) { printf("  "); }
      printf("%c",mem[i][j]);
    }
    printf("\n");
    csaddr+=10;
  }

  return 0;
}
