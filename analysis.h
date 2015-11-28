/*
完成词法分析器的过程中遇到以下问题：

1. 对问题的理解不到位，思路不清晰
2. 太久没有编程，C语言使用不熟练
3. 编程过程不够细心，有许多BUG

解决方法：
1.反复看书，上网查相关资料，询问我的大牛室友
2.翻出之前的C语言书，查询用到的语法
3.反复DEBUG，其中把“”写成了‘’让我找了好久，记忆深刻
                                                 2015.11.20
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define bool int
#define FALSE 0
#define TRUE 1

enum Token_type{
 	  ORGIN,SCALE,ROT,IS,TO,STEP,DRAW,FOR,FROM,       //保留字
    T,                                              //参数
    SEMICO,L_BRACKET,R_BRACKET,COMMA,               //分隔符
    PLUS,MINUS,MUL,DIV,POWER,                       //运算符
    FUNC,                                           //函数
    CONST_ID,                                       //常数
    NONTOKEN,                                       //空记号
    ERRTOKEN                                        //出错记号
 };
 //------------------------记号的数据结构---------------------------------
 typedef struct Token
 {
 	enum Token_type type;
 	char *lexeme;
 	double value;
 	double (*FuncPtr)(double);
 }Token;

 static Token TokenTab[]={                          //区分记号的符号表
 	{CONST_ID,"PI",3.1415926,NULL},
 	{CONST_ID,"E",2.71828,NULL},
 	{T,"T",0.0,NULL},
 	{FUNC,"SIN",0.0,sin},
 	{FUNC,"COS",0.0,cos},
 	{FUNC,"TAN",0.0,tan},
 	{FUNC,"LN",0.0,log},
 	{FUNC,"EXP",0.0,exp},
 	{FUNC,"SQRT",0.0,sqrt},
 	{ORGIN,"ORGIN",0.0,NULL},
 	{SCALE,"SCALE",0.0,NULL},
 	{ROT,"ROT",0.0,NULL},
 	{IS,"IS",0.0,NULL},
 	{FOR,"FOR",0.0,NULL},
 	{FROM,"FROM",0.0,NULL},
 	{TO,"TO",0.0,NULL},
 	{STEP,"STEP",0.0,NULL},
 	{DRAW,"DRAW",0.0,NULL}
 };

 unsigned int LineNo=1;                          //跟踪记号所在源文件的行号
 FILE* fp;                                       //文件指针
 long count=0;                                   //文件偏移量
 char TokenBuffer[8]={NULL};                     //存放单词自身的字符串
 char buffer[8]={NULL};
 int m=0;                                        //单词存放缓冲区的下标
 char Char;
 //---------------------------打开文件-----------------------------
bool InitScanner(const char* FileName){

	if(fp=fopen(FileName,"r"))
		return TRUE;
	else
		return FALSE;
}

//----------------------------关闭文件-----------------------------
void CloseScanner(void){
	if(fclose(fp)!=0)
		printf("Error in closing file\n");
}

//----------------------------读取一个字符-------------------------
char GetChar(void){
	char ch;
	fseek(fp,count,SEEK_SET);
    ch=getc(fp);
    if(ch!=EOF)
	{
		count+=1;
	    return ch;
	}
	else                 //所读范围超出文件范围
       return NULL;

}

//--------------------------回退一个字符----------------------------
void BackChar(void){
	count=count-1;
}
//-------------------------已识别的字符放进TokenBuffer---------------
void AddCharTokenString(void){
     TokenBuffer[m]=Char;
     TokenBuffer[++m]='\0';
}
//-------------------------清空TokenBuffer---------------------------
void EmptyTokenString(void){
	int i=0;
	for(;i<8;i++)
		TokenBuffer[i]=NULL;
	m=0;
}
//-------------------------查找识别出的字符串的相应记号---------------
Token JudgeKeyToken(char* IDString){
	int i;
	for (i = 0; i < 18; ++i)
	{
		if(strcmp(TokenTab[i].lexeme,strupr(IDString))==0)
			return TokenTab[i];

	}
	if(i>=17){
		Token eorro;                //错误记号

		eorro.type=ERRTOKEN;
		eorro.lexeme=IDString;
		eorro.value=0.0;
		eorro.FuncPtr=NULL;
		return eorro;
	}

}
//-----------------------从字符串得到常数的数值----------------------------
double GetValue(){
	double v=0;
	int i=0;
	while(TokenBuffer[i]!='.'&&TokenBuffer[i]!='\0'){
		v=v*10+TokenBuffer[i]-'0';
		i++;
	}
	return v;
}
//-----------------------获得一个记号--------------------------------------
Token GetToken(void){
	Token a_token;
	Char=GetChar();

	while(Char==' '||Char=='\t'){  //过滤掉空格和制表符
		Char=GetChar();
	}
 while(Char=='\n')       //过滤换行符 且行号加1
 {
  LineNo+=1;
  Char=GetChar();
 }
	if(Char!=NULL){
		if((Char>='a'&&Char<='z')||(Char>='A'&&Char<='Z')){
			AddCharTokenString();
			Char=GetChar();
			while((Char>='0'&&Char<='9')||(Char>='a'&&Char<='z')||(Char>='A'&&Char<='Z')){
				AddCharTokenString();
				Char=GetChar();
	        }
            BackChar();                            //回退字符 把该字符用于下一个记号的识别中
	        a_token=JudgeKeyToken(TokenBuffer);    //得到识别出字符串的相应记号
	        EmptyTokenString();                    //清空缓冲区 方便下一次的识别
		}
		else if(Char>='0'&&Char<='9'){

           AddCharTokenString();
           Char=GetChar();
           while(Char>='0'&&Char<='9'){
           	AddCharTokenString();
           	Char=GetChar();
           }
           if(Char=='.')
           {
           	AddCharTokenString();
           	Char=GetChar();
           	while(Char>='0'&&Char<='9'){
           		AddCharTokenString();
           		Char=GetChar();
           	} 
           	BackChar();
           	a_token.type=CONST_ID;
                                  //不能直接用 a_token.lexeme=TokenBuffer;
                                 //因为TokenBuffer是一个指针 赋给a_token.lexme后
                                //两者指向同一块内存区域  在EmptyTokenString()后
                                //这一块内存区域为空   所以此时打印出来的也为空
           	 int i=0;
             for (i = 0; i < 8; ++i)
           {
             buffer[i]=TokenBuffer[i];
           }
            a_token.lexeme=buffer;
           
           	a_token.value=GetValue();
           	a_token.FuncPtr=NULL;

           	EmptyTokenString();
           	}
           	else{
           		BackChar();
           		a_token.type=CONST_ID;
             int i=0;
             for (i = 0; i < 8; ++i)
           {
             buffer[i]=TokenBuffer[i];
           }
            a_token.lexeme=buffer;
           
           
           	  a_token.value=GetValue();
             	a_token.FuncPtr=NULL;

             	EmptyTokenString();

           	}

           }

         else {
           switch (Char){              //其他字符
                        case ';':
                                 a_token.type=SEMICO;
                                 a_token.lexeme=";";
                                 a_token.value=0.0;
                                 a_token.FuncPtr=NULL;break;
                        case '(':
                                 a_token.type=L_BRACKET;
                                 a_token.lexeme="(";
                                 a_token.value=0.0;
                                 a_token.FuncPtr=NULL;break;
                        case ')':
                                 a_token.type=R_BRACKET;
                                 a_token.lexeme=")";
                                 a_token.value=0.0;
                                 a_token.FuncPtr=NULL;break;
                        case ',':
                                 a_token.type=COMMA;
                                 a_token.lexeme=",";
                                 a_token.value=0.0;
                                 a_token.FuncPtr=NULL;break;
                        case '+':
                                 a_token.type=PLUS;
                                 a_token.lexeme="+";
                                 a_token.value=0.0;
                                 a_token.FuncPtr=NULL;break;
                        case '-':
                                 Char=GetChar();
                                 if(Char=='-'){             //遇到注释符
                                   Char=GetChar();
                                   while(Char!='\n')
                                     {
                                       Char=GetChar();   //把这一行剩下的字符全都读掉
                                     }
                                 }
                                 else{
                                   BackChar();
                                    a_token.type=MINUS;
                                    a_token.lexeme="-";
                                    a_token.value=0.0;
                                    a_token.FuncPtr=NULL;
                                 } break;

                        case '*':
                                Char=GetChar();
                                if (Char=='*')
                                {
                                 a_token.type=POWER;
                                 a_token.lexeme="**";
                                 a_token.value=0.0;
                                 a_token.FuncPtr=NULL;
                                }
                                else{
                                  BackChar();
                                    a_token.type=MUL;
                                    a_token.lexeme="*";
                                    a_token.value=0.0;
                                    a_token.FuncPtr=NULL;
                                }break;
                       case '/':
                               Char=GetChar();
                               if(Char=='/'){             //遇到注释符
                                   Char=GetChar();
                                   while(Char!='\n'){
                                     Char=GetChar();    //把这一行剩下的字符都读掉
                                   }
                               }
                               else{
                                 BackChar();
                                 a_token.type=DIV;
                                    a_token.lexeme="/";
                                    a_token.value=0.0;
                                    a_token.FuncPtr=NULL;
                               }break;
                      }
           }
	}


		else{            //读到文件结尾
			a_token.type=NONTOKEN;
			a_token.lexeme=NULL;
			a_token.value=0.0;
			a_token.FuncPtr=NULL;
		}
		return a_token;
}
