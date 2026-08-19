#pragma once
#include "pch.h"
using namespace std;
typedef enum e_var
{
    NUM = 0,
    STR = 1,
    ANY = 2
} VARBASICTYPE;
typedef struct tagVarSaveType
{
    string sName;
    VARBASICTYPE eVarType;
    string sValue;
    int nScoop = 0;
} VARSAVETYPE;

typedef struct tagArraySaveType
{
    string sName;
    vector<string> values;   // Stored element values
    VARBASICTYPE eElemType = ANY; // Element type for the array (NUM/STR/ANY)
    int nScoop = 0;
    bool bModified = false;
} ARRAYSAVETYPE;

typedef enum e_char
{
    DIGIT = 1,
    ALPHA = 2,
    OPER = 3,
    SPACE = 4,
    ENTER = 5,
    OTHER =6,
    SEPARATE = 7
} CHARTYPE;
typedef enum e_ctrl
{
    LET = 1,
    EXP = 2,
    FOR = 3,
    WHILE = 4,
    IF = 5,
    WRITE = 6,
    PROGRAM = 7,
    FUNCTION = 8,
    UNKNOWN = 9,
    ARRAYSET = 10,
    ARRAYGET = 11,
    ARRAYNEW = 12,
    ARRAYDEL = 13,
    CONVERT = 14,
    UPDATE = 15,
    SLEEP =16
} CTRLTYPE;

#define OPERATIONNUM 11
const string g_sOperationList[] = {
    "+",
    "-",
    "*",
    "/",
    "=",
    ")",
    "(",
    "<",
    ">",
    "!",
    "==",
    ">=",
    "<=",
    "!="};
typedef struct tagNode
{
    string sValue;
    VARSAVETYPE *pData = NULL;
    // VARBASICTYPE eVarType = ANY;
    vector<tagNode *> vpChild;
} NODE;
typedef struct tagFuncSaveType
{
    string sName;
    vector<string> params;
    NODE *pBody = NULL;
    int nScoop = 0;
} FUNCSAVETYPE;

class CScriptEngine
{
private:
    bool IsOper(char c);
    bool IsNumber(const string &s);
    VARSAVETYPE *CreateVarData(const string &value);
    bool CompareValues(const string &op, const string &left, const string &right);
    vector<string> Slice(string str);
    int BuildTree(NODE *pNode, vector<string> vsCodeData, int nPos = 0);
    CHARTYPE CheckCharType(char c);
    CTRLTYPE CheckCtrlType(string str);
    void BeginScoop();
    void EndScoop();
    void Visit(NODE *pNode);
    void StatementIf(NODE *pNode);
    void StatementExp(NODE *pNode);
    void StatementUnknown(NODE *pNode);
    void StatementProgram(NODE *pNode);
    void StatementFor(NODE *pNode) {} // TODO:Implement for loop
    void StatementFunction(NODE *pNode);
    void StatementLet(NODE *pNode);
    void StatementWhile(NODE *pNode);
    void StatementWrite(NODE *pNode);
    void StatementConvert(NODE *pNode);
    void StatementArraySet(NODE *pNode);
    void StatementArrayGet(NODE *pNode);
    void StatementArrayNew(NODE *pNode);
    void StatementArrayDel(NODE *pNode);
	void StatementUpdate(NODE* pNode);
    void StatementSleep(NODE* pNode);
    void PrintTree(NODE *pNode, int nDepth = 0);
    void ReadFile(string filename);
    void LoadInfo();
    void UpdateInfo();
private:
    vector<VARSAVETYPE> m_vVar;
    vector<ARRAYSAVETYPE> m_vArray;
    vector<FUNCSAVETYPE> m_vFunc;
    string m_sCode;
    int m_nScoopCount = 0;
    NODE* m_pHead = NULL;

public:
    void Run(const string &filename);
};