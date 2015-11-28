/*
 2015.11.26
*/
#include "analysis.h"
#include <stdarg.h>

//----------------------语法树的节点类型------------------------------------------------
typedef double (*FuncPtr)(double);
typedef struct ExprNode
{
	enum Token_type OpCode;
	union
	{
		struct{struct ExprNode *left,*right}CaseOperator;  //二元运算 有两个孩子节点
        struct{struct ExprNode *Child;
               FuncPtr MathFuncPtr;}CaseFunc;      //函数调用 一个孩子（一个参数）
        double CaseConst;                         //常数
        double *CaseParmPtr;                      //参数T，因为是变量 所以绑定左值，即是一个指针
	}Content;
}ExprNode;
 Token token;                                     //全局变量token
 double Parmeter;                                //参数
//---------------------函数的声明----------------------------------------------
 void FetchToken();                        //调用词法分析器的GetToken，得到一个记号
 void MatchToken(enum Token_type AToken);  //匹配当前记号，成功则获取下一个，否则返回语法错误
 void SyntaxError(int case_of);            //处理语法错误
 void ErrMsg(unsigned LineNo,char *descrip,char *string);  //打印错误信息
 void PrintSyntaxTree(ExprNode *root,int indent);  //打印语法树
 ExprNode* MakeExprNode(enum Token_type opcode, ...);
                                                   //建立语法树
 void Parser(char * SrcFilePtr);           //语法分析器对外的接口
 void Program();
 void Statement();
 void OriginStatement();
 void RotStatement();
 void ScaleStatement();
 void ForStatement();

 ExprNode* Expression();
 ExprNode* Term();
 ExprNode* Factor();
 ExprNode* Component();
 ExprNode* Atom();                       //以上全为 产生式的递归子程序

 void Enter(char* );                     //进入一个递归子程序
 void Exit(char* );                      //退出一个递归子程序
//------------------通过词法分析器获得一个记号-----------------------------------------
void FetchToken()
{
	token=GetToken();
	if(token.type==ERRTOKEN)
		SyntaxError(1);
}
//------------------匹配记号-------------------------------------------------------------
void MatchToken(enum Token_type The_Token)
{
	if(token.type!=The_Token)
		SyntaxError(2);
	FetchToken();
}
//------------------语法错误处理----------------------------------------------------------
void SyntaxError(int case_of)
{
	switch(case_of)
	{
		case 1: ErrMsg(LineNo,"错误记号",token.lexeme);
		        break;
		case 2: ErrMsg(LineNo,"不是预期记号",token.lexeme);
                break;
	}
}
//-------------------打印错误信息----------------------------------------------------------
void ErrMsg(unsigned LineNo,char *descrip,char *string)
{
	printf("第%5d行的%s是一个%s!\n",LineNo,string,descrip);
}
//--------------------打印语法树，先序遍历--------------------------------------------------
void PrintSyntaxTree(ExprNode *root,int indent)  //参考大二数据结构上机代码
{
	int temp;
	for (temp=1;temp<indent; temp++)
		printf("\t");
	switch(root->OpCode)
	{
		case PLUS: printf("%s\n","+");break;
		case MINUS: printf("%s\n","-");break;
		case MUL: printf("%s\n","*");break;
		case DIV: printf("%s\n","/");break;
		case POWER: printf("%s\n","**" );break;
		case FUNC: printf("%x\n",root->Content.CaseFunc.MathFuncPtr );break;
		case CONST_ID: printf("%f\n",root->Content.CaseConst );break;
		case T:printf("%s\n","T");break;
		default: printf("Error Tree Node!\n");exit(0);
	}
	if(root->OpCode==CONST_ID||root->OpCode==T)  //到达叶子节点
		return ;
	if(root->OpCode==FUNC)      //函数  递归打印一个孩子的节点
		PrintSyntaxTree(root->Content.CaseFunc.Child,indent+1);
	else       //二元运算  递归打印两个孩子节点
	{
		PrintSyntaxTree(root->Content.CaseOperator.left,indent+1);
		PrintSyntaxTree(root->Content.CaseOperator.right,indent+1);
	}
}
//---------------------Parser的递归子程序--------------------------------------
void Parser(char *SrcFilePtr)
{
	if(!InitScanner(SrcFilePtr))            //初始化语法分析器
	{
		printf("打开文件错误!\n");
		return;
	}
	FetchToken();
	Program();
	CloseScanner();                        //关闭语法分析器
}
//---------------------Program的递归子程序-------------------------------------
void Program()
{
	Enter("Program");
    while(token.type!=NONTOKEN)
    {
    	Statement();
    	MatchToken(SEMICO);
    }
    Exit("Program");
}
//------------------Statement的递归子程序---------------------------------------
void Statement()
{
	Enter("Statement");
	switch(token.type)
	{
		case ORGIN : OriginStatement(); break;
		case SCALE : ScaleStatement(); break;
		case ROT : RotStatement();break;
		case FOR : ForStatement();break;
		default : SyntaxError(2);
	}
	Exit("Statement");
}
//-----------------OriginStatement的递归子程序-----------------------------------
void OriginStatement()
{   ExprNode *tmp1,*tmp2;
	Enter("OriginStatement");
	MatchToken(ORGIN);
	MatchToken(IS);
	MatchToken(L_BRACKET);
	tmp1=Expression();
	MatchToken(COMMA);
	tmp2=Expression();
	MatchToken(R_BRACKET);
	Exit("OriginStatement");
}
//----------------ScaleStatement的递归子程序------------------------------------
void ScaleStatement()
{
	ExprNode *tmp1,*tmp2;
	Enter("ScaleStatement");
	MatchToken(SCALE);
	MatchToken(IS);
	MatchToken(L_BRACKET);
	tmp1=Expression();
	MatchToken(COMMA);
	tmp2=Expression();
	MatchToken(R_BRACKET);
	Exit("ScaleStatement");
}
//----------------RotStatement的递归子程序------------------------------------
void RotStatement()
{
	ExprNode *tmp;
	Enter("ROT");
	FetchToken(ROT);
	FetchToken(IS);
	tmp=Expression();
	Exit("ROT");
}
//---------------ForStatement的递归子程序------------------------------------
void ForStatement()
{ 
	Enter("ForStatement");
	ExprNode *start_ptr,*end_ptr,*step_ptr,*x_ptr,*y_ptr;
	MatchToken(FOR);
	MatchToken(T);
	MatchToken(FROM);
	start_ptr=Expression();             //起点表达式
	MatchToken(TO);
	end_ptr=Expression();               //终点表达式
	MatchToken(STEP);                
	step_ptr=Expression();              //步长表达式
	MatchToken(DRAW);
	MatchToken(L_BRACKET);
	x_ptr=Expression();                 //横坐标表达式
	MatchToken(COMMA);
	y_ptr=Expression();                 //纵坐标表达式
	MatchToken(R_BRACKET);
	Exit("ForStatement");
}
//---------------Expression的递归子程序--------------------------------------
ExprNode* Expression()
{
	ExprNode *left,*right;
	enum Token_type token_tmp;
	Enter("Expression");
	left=Term();
	while(token.type==PLUS||token.type==MINUS)
	{
		token_tmp=token.type;
		MatchToken(token_tmp);
		right=Term();
		left=MakeExprNode(token_tmp,left,right);
	}
	PrintSyntaxTree(left,1);
	Exit("Expression");
    return left;
}
//-------------Termd的递归子程序------------------------------------------------
ExprNode* Term()
{
	ExprNode *left,*right;
	enum Token_type token_tmp;

	left=Factor();
	while(token.type==MUL||token.type==DIV)
	{
		token_tmp=token.type;
		MatchToken(token_tmp);
		right=Factor();
		left=MakeExprNode(token_tmp,left,right);
	}
    return left;
}
//-------------Factor的递归子程序-----------------------------------------------
ExprNode* Factor()
{
	ExprNode *left,*right;
	if(token.type==PLUS)
	{
		MatchToken(PLUS);
		right=Factor();
	}
	else if(token.type==MINUS)
	{
		MatchToken(MINUS);
		right=Factor();
		left=malloc(sizeof(ExprNode));
		left->OpCode=CONST_ID;
		left->Content.CaseConst=0.0;
		right=MakeExprNode(MINUS,left,right);
	}
	else
		right=Component();
	return right;
}
//----------------Component的递归子程序-------------------------------------------
ExprNode* Component()
{
	ExprNode *left,*right;
	left=Atom();
	if(token.type==POWER)
	{
		MatchToken(POWER);
		right=Component();
		left=MakeExprNode(POWER,left,right);
	}
	return left;
}
//-------------Atom的递归子程序---------------------------------------------------
ExprNode* Atom()
{
	Token t=token;
	ExprNode *address,*tmp;

	switch(token.type)
	{
		case CONST_ID:
             MatchToken(CONST_ID);
             address=MakeExprNode(CONST_ID,t.value);
             break;
        case T:
             MatchToken(T);
             address=MakeExprNode(T);
             break;
        case FUNC:
             MatchToken(FUNC);
             MatchToken(L_BRACKET);
             tmp=Expression();
             address=MakeExprNode(FUNC,t.FuncPtr,tmp);
             MatchToken(R_BRACKET);
             break;
        case L_BRACKET:
             MatchToken(L_BRACKET);
             address=Expression();
             MatchToken(R_BRACKET);
             break;
        default:
        SyntaxError(2);
	}
	return address;
}
//-------------MakeExprNode构造语法树--------------------------------------------
ExprNode* MakeExprNode(enum Token_type opcode, ...)
{
	ExprNode* node=malloc(sizeof(ExprNode));
    node->OpCode=opcode;

    va_list ArgPtr;                //va_list代表一种数据对象，用于存放参量列表中省略号代表的参量
    va_start(ArgPtr,opcode);      //把ArgPtr初始化为参数列表

    switch(opcode)
    {
    	case CONST_ID:
    	     node->Content.CaseConst=(double)va_arg(ArgPtr,double);break;
    	case T:
             node->Content.CaseParmPtr=&Parmeter;break;
        case FUNC:
             node->Content.CaseFunc.MathFuncPtr=(FuncPtr)va_arg(ArgPtr,FuncPtr);
             node->Content.CaseFunc.Child=(ExprNode*)va_arg(ArgPtr,ExprNode*);
             break;
        default:      //二元运算结点
             node->Content.CaseOperator.left=(ExprNode*)va_arg(ArgPtr,ExprNode*);
             node->Content.CaseOperator.right=(ExprNode*)va_arg(ArgPtr,ExprNode*);
                    break;
    }
    va_end(ArgPtr);           //完成清理工作
    return node;
}
//-----------------------Enter函数---------------------------------------------------
void Enter(char* T)
{
	printf("Enter in %s\n",T);
}
//-----------------------Exit函数----------------------------------------------------
void Exit(char* T)
{
	printf("Exit from %s\n",T);
}
