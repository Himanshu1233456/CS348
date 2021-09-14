#include <bits/stdc++.h>
using namespace std;

//OPTABLE DEFINED FOR CONVERSIONS

struct opcode_table
{
	char name[10];
	int opcode;
};

static struct opcode_table optable[23]={
    {"LDA",0x00},
    {"LDX",0x04},
    {"LDL",0x08},
    {"STA",0x0C},
    {"STX",0x10},
    {"STL",0x14},
    {"LDCH",0x50},
    {"STCH",0x54},
    {"ADD",0x18},
    {"SUB",0x1C},
    {"MUL",0x20},
    {"DIV",0x24},
    {"COMP",0x28},
    {"J",0x3C},
    {"JLT",0x38},
    {"JEQ",0x30},
    {"JGT",0x34},
    {"JSUB",0x48},
    {"RSUB",0x4C},
    {"TIX",0x2C},
    {"TD",0xE0},
    {"RD",0xD8},
    {"WD",0xDC},
};

//CLASS STORES THE PARAMETERS FOR EACH STATEMENT

class store
{
	public:
  	int line;
  	int loc;
  	string label;
  	string operation;
  	string operand;
  	unsigned long objcode;
};

store pass1[200];

class symbols
{
	public:
		string name;
		int addr;
};

symbols sym_table[200];

int l_ptr=0,w_ptr=0,p1_ptr=0,sym_ptr=0,loc_cnt[200],lc_ptr=0,op_flag=0,sym_flag=0,search_idx;

//CONVERTS STRING INTO DECIMAL
int Dec(string a)
{
	int num = 0;
	for (int i=a.length()-1, j=1; i>=0; i--)
	{
		num = num+int(a.at(i)-'0')*j;
		j=j*10;
	}
	return num;
}

//CONVERTS STRING INTO HEX INTEGER
int Hex(string a)
{
	int hex = 0;
	for(int i=a.length()-1,j=1;i>=0;i--)
	{
		if(a.at(i)>='0' && a.at(i)<='9')
			hex += int(a.at(i)-'0')*j;
		else if(a.at(i)>='A' && a.at(i)<='F')
            hex += int(a.at(i)-'A'+10)*j;
		else if(a.at(i)>='a' && a.at(i)>='f')
            hex += int(a.at(i)-'a'+10)*j;
		else ;
		j=j*16;
	}
	return hex;
}

//CALCULATES BUFFER LENGTH
int Length(string a)
{
	int b;
	if(a.at(0)=='C' || a.at(0)=='c' && a.at(1)=='\'')
  {
		for(b=2;b<=a.length();b++)
    {
			if(a.at(b)=='\'')
      {
				b -=2;
				break;
			}
		}
	}
	if (a.at(0)=='X' || a.at(0)=='x' && a.at(1)=='\'')
		b = 1;
	return b;
}

//READS A WORD IN A LINE
string scan(string a)
{
  string b="";
	while(a.at(l_ptr)!=' ' && a.at(l_ptr)!='\t' && a.at(l_ptr)!='\n')
  {
    b.push_back(a.at(l_ptr));
    l_ptr++;
  }
	return b;
}

//MOVES POINTER TO THE NEXT WORD IN A LINE
void next(string a)
{
	while(a.at(l_ptr)==' ' || a.at(l_ptr)=='\t')
    l_ptr++;
}

//CHECKS IF OPERATION IS PRESENT IN OPTABLE AND STORES INDEX IN COUNTER
int find_op(string a)
{
	int size = sizeof(optable)/sizeof(struct opcode_table);
	op_flag = 0;
	for(int i=0;i<size;i++)
  {
		if(a==optable[i].name)
    {
			search_idx = i;
			op_flag = 1;
			break;
		}
	}
	return op_flag;
}

int main()
{
  int sno=0,begin,loc,pg_length,fa,la,fi,li,x,tmp_obj[30];
  string line,word,label,oprn,opnd,arr_opr[20],arr_opd[20];
	unsigned long objc,idx,addr,pre_oc;
  ifstream in;
  ofstream p1,out,sym;

  in.open("input.txt"); //CHANGE THE NAME OF INPUT FILE HERE
  p1.open("pass1.txt");
	sym.open("symbols.txt");
	out.open("output.txt");

//PASS-1 (INPUT IS THE FILE WITH GIVEN INSTRUCTIONS)

  while(getline(in,line))
  {
    line.push_back('\n');
    l_ptr=0;
    word = scan(line);
    if(word.at(0)!='.')
    {
      l_ptr=0;
      pass1[p1_ptr].line = p1_ptr;

      label=scan(line);
      pass1[p1_ptr].label = label;
      next(line);

      if(sno==0)
      {
        oprn = scan(line);
        pass1[p1_ptr].operation = oprn;
        if(oprn=="START")
        {
          next(line);
          opnd = scan(line);
          pass1[p1_ptr].operand = opnd;
          loc_cnt[lc_ptr] = Hex(opnd);
          begin = loc_cnt[lc_ptr];
        }
        else
        {
          loc_cnt[lc_ptr] = 0;
          begin = loc_cnt[lc_ptr];
        }
      }
      else
      {
        oprn = scan(line);
        pass1[p1_ptr].operation = oprn;
        next(line);
        opnd = scan(line);
        pass1[p1_ptr].operand = opnd;

        if(oprn!="END")
				{
					sym_table[sym_ptr].name = label;
					sym_table[sym_ptr].addr = loc_cnt[lc_ptr-1];
					sym_ptr++;

          if(find_op(oprn))
            loc_cnt[lc_ptr] = loc + 3;
          else if(oprn=="WORD")
            loc_cnt[lc_ptr] = loc + 3;
          else if(oprn=="RESW")
            loc_cnt[lc_ptr] = loc + 3*Dec(opnd);
          else if(oprn=="RESB")
            loc_cnt[lc_ptr] = loc + Dec(opnd);
          else if(oprn=="BYTE")
            loc_cnt[lc_ptr] = loc + Length(opnd);
          }
        }
        pass1[p1_ptr].loc = loc_cnt[lc_ptr-1];
        loc = loc_cnt[lc_ptr];
        lc_ptr++;
        p1_ptr++;
      }
      op_flag=0;
      sno++;
    }
		pg_length=loc_cnt[lc_ptr-2] - loc_cnt[0];

//PASS-2 (USING INTERMEDIATE ARRAY & SYMBOL TABLE FROM PASS-1)

		for(int p2=1;p2<p1_ptr;p2++)
		{
				idx=0;
				addr=0;
				objc=0;

				if(find_op(pass1[p2].operation))
				{
					objc = optable[search_idx].opcode;
					objc <<= 16;
					pass1[p2].objcode = objc;

					string opr_tmp = pass1[p2].operand;

					if(opr_tmp.length()>0 && opr_tmp.at((opr_tmp).length()-2)==',' && opr_tmp.at((opr_tmp).length()-1)=='X')
					{
						idx = 0x008000;
					}
					else
						idx = 0x000000;

					for(int s=0;s<sym_ptr;s++)
					{
						if(opr_tmp == sym_table[s].name)
							addr = (long)sym_table[s].addr;
					}
					pre_oc = idx+addr+objc;
					pass1[p2].objcode = pre_oc;
				}
				else if(pass1[p2].operation=="WORD")
				{
					pass1[p2].objcode = Dec(pass1[p2].operand);
				}
				else if(pass1[p2].operation=="BYTE")
				{
					string opr_tmp1 = pass1[p2].operand;
					pass1[p2].objcode = 0;

					if(opr_tmp1.length()>0 && (opr_tmp1.at(0)=='C' || opr_tmp1.at(0)=='c' && opr_tmp1.at(1)=='\''))
					{
						for(int t=2;t<=opr_tmp1.length()-2;t++)
						{
							pass1[p2].objcode = pass1[p2].objcode + (int)opr_tmp1[t];
							pass1[p2].objcode<<=8;
						}
					}

					if(opr_tmp1.length()>0 && (opr_tmp1.at(0)=='X' || opr_tmp1.at(0)=='x' && opr_tmp1.at(1)=='\''))
					{
						char *operand_ptr;
						operand_ptr = &opr_tmp1[2];
						*(operand_ptr+2)='\0';
						for(int t=2; t<=opr_tmp1.length();t++)
						{
							pass1[p2].objcode = Hex(operand_ptr);
							pass1[p2].objcode<<=8;
						}
					}
					pass1[p2].objcode>>=8;
				}
				else ;
		}

		for(int q=0;q<p1_ptr;q++)
		{
			p1<<hex<<pass1[q].loc<<"\t"<<pass1[q].label<<"\t"<<pass1[q].operation<<"\t"<<pass1[q].operand<<"\t"<<pass1[q].objcode<<endl;
		}

		for(int u=0;u<sym_ptr;u++)
		{
			sym<<sym_table[u].name<<"\t"<<hex<<sym_table[u].addr<<"\n";
		}

//PRINTING THE OUTPUT OBJECT CODE

		cout<<"-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x"<<endl<<endl;
		if(pass1[0].operation=="START")
		{
			cout<<"	H\t"<<pass1[0].label<<"\t";
			cout<<setfill('0')<<uppercase<<hex<<setw(6)<<begin<<"\t"<<setw(6)<<pg_length;
			cout<<endl;

			out<<"H^"<<pass1[0].label<<"^";
			out<<setfill('0')<<uppercase<<hex<<setw(6)<<begin<<"^"<<setw(6)<<pg_length;
			out<<endl;
		}

		int itr=1,addr_tmp;
		while(1)
		{
			fi = itr;
			fa = pass1[itr].loc;
			la = pass1[itr].loc+27;

			for(x=0,addr_tmp=fa;addr_tmp<=la;itr++)
			{
				if(pass1[itr].operation=="END")
					break;
				else if(pass1[itr].operation!="RESB" && pass1[itr].operation!="RESW")
				{
					arr_opr[x] = pass1[itr].operation;
					arr_opd[x] = pass1[itr].operand;
					tmp_obj[x] = pass1[itr].objcode;
					li=itr+1;
				  x++;
				}
				else ;
				addr_tmp = pass1[itr+1].loc;
			}

			cout<<"	T\t";
			cout<<setfill('0')<<uppercase<<hex<<setw(6)<<fa<<"\t";
			cout<<setfill('0')<<hex<<setw(2)<<(pass1[li].loc - pass1[fi].loc)<<"\t";

			out<<"T";
			out<<"^";
			out<<setfill('0')<<uppercase<<hex<<setw(6)<<fa;
			out<<"^";
			out<<setfill('0')<<hex<<setw(2)<<(pass1[li].loc - pass1[fi].loc);

			for(int c=0;c<x;c++)
			{
				if((arr_opr[c]=="BYTE") && (arr_opd[c].length()>0 && (arr_opd[c].at(0)=='X' || (arr_opd[c]).at(0)=='x')))
				{
					cout<<setfill('0')<<hex<<setw(2)<<tmp_obj[c]<<"\t";
					out<<"^";
					out<<setfill('0')<<hex<<setw(2)<<tmp_obj[c];
				}
				else
				{
					cout<<setfill('0')<<hex<<setw(6)<<tmp_obj[c]<<"\t";
					out<<"^";
					out<<setfill('0')<<hex<<setw(6)<<tmp_obj[c];
				}
			}
			cout<<"\n";
			out<<"\n";
			if(pass1[itr].operation=="END")
				break;
		}

		cout<<"	E\t";
		cout<<setfill('0')<<uppercase<<hex<<setw(6)<<begin;
		cout<<endl<<endl;

		out<<"E^";
		out<<setfill('0')<<uppercase<<hex<<setw(6)<<begin;
		out<<endl<<endl;
		cout<<"-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x"<<endl<<endl;
}
