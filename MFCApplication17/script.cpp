#pragma once
#include "pch.h"
bool CScriptEngine::IsOper(char c)
{
    string str;
    str = c;
    for (auto a : g_sOperationList)
    {
        if (a == str)
            return true;
    }
    return false;
}
bool CScriptEngine::IsNumber(const string &s)
{
    bool bHasDot = false;
    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == '.')
        {
            if (bHasDot)
                return false;
            bHasDot = true;
        }
        else if (!isdigit(s[i]))
        {
            return false;
        }
    }
    return true;
}
#define LOGVAR(var) afxDump << var->sName << ": " << var->sValue << " (Type: " << var->eVarType << ")" << endl
VARSAVETYPE *CScriptEngine::CreateVarData(const string &value)
{
    // TODO:throw error if value is an undefined variable
    VARSAVETYPE *p = new VARSAVETYPE;
    // if (!IsNumber(value))
    //{
    if (value.size() >= 2 &&
        value[0] == '"' && value.back() == '"')
    {
        // remove quota
        string strValue = value.substr(1, value.size() - 2);
        p->sName = "STR";
        p->eVarType = STR;
        p->sValue = strValue;
    }
    //}
    else
    {
        p->sName = "VAR";
        p->eVarType = NUM;
        p->sValue = value;
    }
    p->nScoop = m_nScoopCount;
    // LOGVAR(p);
    return p;
}
bool CScriptEngine::CompareValues(const string &op, const string &left, const string &right)
{
    if (IsNumber(left) && IsNumber(right))
    {
        double l = stod(left);
        double r = stod(right);
        if (op == "==")
            return fabs(l - r) < 1e-9;
        if (op == "!=")
            return fabs(l - r) >= 1e-9;
        if (op == "<")
            return l < r;
        if (op == ">")
            return l > r;
        if (op == "<=")
            return l <= r;
        if (op == ">=")
            return l >= r;
    }
    else
    {
        if (op == "==")
            return left == right;
        if (op == "!=")
            return left != right;
    }
    return false;
}
CHARTYPE CScriptEngine::CheckCharType(char c)
{
    if (isdigit(c) || c == '.')
        return DIGIT;
    if (isalpha(c) || c == '_' || c == '\"')
        return ALPHA;
    if (c == ';')
        return SEPARATE;
    if (c == '\n')
    {
        return ENTER;
    }
    if (isspace(c))
    {
        return SPACE;
    }
    if (IsOper(c))
        return OPER;
    // if (c == '"')
    //    return OTHER;
    return OTHER;
}

vector<string> CScriptEngine::Slice(string str)
{
    string sCode = str;
    string sCurrent = "";
    CHARTYPE eCurCharType;
    vector<string> vsCodeData;
    bool bInStr = false;

    for (size_t i = 0; i < sCode.length(); i++)
    {
        if (sCode[i] == '"')
        {
            if (!bInStr)
            {
                if (!sCurrent.empty())
                {
                    vsCodeData.push_back(sCurrent);
                    sCurrent.clear();
                }
                bInStr = true;
                sCurrent = "\"";
            }
            else
            {
                sCurrent += "\"";
                vsCodeData.push_back(sCurrent);
                sCurrent.clear();
                bInStr = false;
            }
            continue;
        }

        if (bInStr)
        {
            sCurrent += sCode[i];
            continue;
        }

        sCurrent = sCurrent + sCode[i];
        eCurCharType = CheckCharType(sCode[i]);

        if (CheckCharType(sCurrent[0]) == SPACE)
        {
            sCurrent.clear();
            continue;
        }

        if (i < sCode.length())
        {
            if (CheckCharType(sCode[i + 1]) != eCurCharType)
            {
                vsCodeData.push_back(sCurrent);
                sCurrent.clear();
            }
        }
        else
        {
            vsCodeData.push_back(sCurrent);
            sCurrent.clear();
        }
    }

    if (!sCurrent.empty())
    {
        vsCodeData.push_back(sCurrent);
    }

    return vsCodeData;
}

int CScriptEngine::BuildTree(NODE *pNode, vector<string> vsCodeData, int nPos)
{
    NODE *pAdd = NULL;
    bool bTemp = true;
    for (size_t i = nPos; i < vsCodeData.size(); i++)
    {
        if (vsCodeData[i] == "(")
        {
            pAdd = new NODE;
            pNode->vpChild.push_back(pAdd);
            i = BuildTree(pAdd, vsCodeData, i + 1);
        }
        else if (vsCodeData[i] == ")")
        {
            return i;
        }
        else if (CheckCharType(vsCodeData[i][0]) == ALPHA || CheckCharType(vsCodeData[i][0]) == DIGIT || CheckCharType(vsCodeData[i][0]) == OPER)
        {
            // assert(vsCodeData[i][0]=='(');
            if (bTemp == true)
            {
                pNode->sValue = vsCodeData[i];
                bTemp = false;
            }
            else
            {
                pAdd = new NODE;
                pAdd->sValue = vsCodeData[i];
                pNode->vpChild.push_back(pAdd);
                // assert(pNode->sValue == "array_new");
            }
        }
    }
    return vsCodeData.size();
}
CTRLTYPE CScriptEngine::CheckCtrlType(string str)
{
    if (str == "for")
        return FOR;
    if (str == "while")
        return WHILE;
    else if (str == "if")
        return IF;
    else if (str == "let")
        return LET;
    else if (str == "exp")
        return EXP;
    else if (str == "write")
        return WRITE;
    else if (str == "program")
        return PROGRAM;
    else if (str == "function")
        return FUNCTION;
    else if (str == "array_set")
        return ARRAYSET;
    else if (str == "array_get")
        return ARRAYGET;
    else if (str == "array_new")
        return ARRAYNEW;
    else if (str == "convert")
        return CONVERT;
	else if (str == "update")
        return UPDATE;
    else
        return UNKNOWN;
}

void CScriptEngine::BeginScoop()
{
    m_nScoopCount++;
}
void CScriptEngine::EndScoop()
{
    auto newEnd = remove_if(m_vVar.begin(), m_vVar.end(),
                            [&](const VARSAVETYPE &var)
                            { return var.nScoop == m_nScoopCount; });
    m_vVar.erase(newEnd, m_vVar.end());
    auto newEndArr = remove_if(m_vArray.begin(), m_vArray.end(),
                               [&](const ARRAYSAVETYPE &arr)
                               { return arr.nScoop == m_nScoopCount; });
    m_vArray.erase(newEndArr, m_vArray.end());
    m_nScoopCount--;
}
void CScriptEngine::StatementProgram(NODE *pNode)
{
    BeginScoop();
    // for_each(pNode->vpChild.begin(), pNode->vpChild.end(), [](NODE *pNode)
    //          { Visit(pNode); });
    for (auto a : pNode->vpChild)
    {
        Visit(a);
    }
    EndScoop();
}

void CScriptEngine::StatementFunction(NODE *pNode)
{
    if (pNode->vpChild.size() < 2)
        return;

    string fname = pNode->vpChild[0]->sValue;
    vector<string> params;
    NODE *pParamList = NULL;
    if (pNode->vpChild.size() >= 2)
        pParamList = pNode->vpChild[1];
    if (pParamList)
    {
        params.push_back(pParamList->sValue);
        for (auto c : pParamList->vpChild)
            params.push_back(c->sValue);
    }
    NODE *pBody = NULL;
    if (pNode->vpChild.size() >= 3)
        pBody = pNode->vpChild[2];

    auto it = find_if(m_vFunc.begin(), m_vFunc.end(), [&](const FUNCSAVETYPE &f)
                      { return f.sName == fname; });
    if (it == m_vFunc.end())
    {
        FUNCSAVETYPE f;
        f.sName = fname;
        f.params = params;
        f.pBody = pBody;
        f.nScoop = m_nScoopCount;
        m_vFunc.push_back(f);
    }
    else
    {
        it->params = params;
        it->pBody = pBody;
    }
}
void CScriptEngine::StatementWrite(NODE *pNode)
{
    for (auto a : pNode->vpChild)
    {
        Visit(a);
        //afxDump << a->pData->sValue << endl;
        CStringA str;
        str.Format("%s\n", a->pData->sValue.c_str());
        //MessageBoxA(NULL, str, "Info", MB_OK);
        //afxDump << str<<"\n";
    }
}
void CScriptEngine::StatementWhile(NODE *pNode)
{
    BeginScoop();
    Visit(pNode->vpChild[0]);
    while (stoi(pNode->vpChild[0]->pData->sValue))
    {
        Visit(pNode->vpChild[0]);
        Visit(pNode->vpChild[1]);
    }
    EndScoop();
}

void CScriptEngine::StatementLet(NODE *pNode)
{
    if (pNode->vpChild.size() < 2)
        return;

    string varName = pNode->vpChild[0]->sValue;

    Visit(pNode->vpChild[1]);

    if (pNode->vpChild[1]->pData == NULL)
    {
        //afxDump << "Error: Expression has no result for variable "<<"\n";
            //afxDump << varName << endl;
        return;
    }

    string value = pNode->vpChild[1]->pData->sValue;

    auto it = find_if(m_vVar.begin(), m_vVar.end(),
                      [&](const VARSAVETYPE &var)
                      { return var.sName == varName; });

    if (it == m_vVar.end())
    {
        VARSAVETYPE var;
        var.sName = varName;
        var.sValue = value;
        var.eVarType = pNode->vpChild[1]->pData->eVarType; // TODO:Correct type detection(Finished)
        var.nScoop = m_nScoopCount;
        m_vVar.push_back(var);
    }
    else
    {
        it->sValue = value;
    }
}

void CScriptEngine::StatementIf(NODE *pNode)
{
    BeginScoop();
    if (pNode->vpChild.size() < 2)
    {
        return;
    }
    Visit(pNode->vpChild[0]);
    if (pNode->vpChild[0]->pData != NULL)
    {
        VARSAVETYPE *pTemp = pNode->vpChild[0]->pData;
        if (stoi(pTemp->sValue) != 0)
        {
            Visit(pNode->vpChild[1]);
        }
    }
    EndScoop();
}

void CScriptEngine::StatementExp(NODE *pNode)
{
    if (pNode->vpChild.empty())
    {
        return;
    }
    for (auto a : pNode->vpChild)
    {
        Visit(a);
    }
    if (pNode->vpChild.size() == 3)
    {
        NODE *pLeft = pNode->vpChild[0];
        NODE *pOpNode = pNode->vpChild[2];
        NODE *pRight = pNode->vpChild[1];
        string sOp = pOpNode->sValue;
        string sLeftVal = pLeft->pData->sValue;
        string sRightVal = pRight->pData->sValue;
        string sResult;

        if (pLeft->pData && pRight->pData && pOpNode->vpChild.empty())
        {
            // bool bLeftIsNum = IsNumber(sLeftVal);
            // bool bRightIsNum = IsNumber(sRightVal);
            bool bLeftIsNum = pLeft->pData->eVarType == NUM;
            bool bRightIsNum = pRight->pData->eVarType == NUM;
            // num
            if (bLeftIsNum && bRightIsNum &&
                (sOp == "+" || sOp == "-" || sOp == "*" || sOp == "/"))
            {
                double l = stod(sLeftVal);
                double r = stod(sRightVal);

                if (sOp == "+")
                    sResult = to_string(l + r);
                else if (sOp == "-")
                    sResult = to_string(l - r);
                else if (sOp == "*")
                    sResult = to_string(l * r);
                else if (sOp == "/")
                {
                    if (fabs(r) < 1e-9)
                        sResult = "Error: Division by zero";
                    else
                        sResult = to_string(l / r);
                }
            }
            // str
            else if (sOp == "+")
            {
                string sLeftActual = sLeftVal;
                string sRightActual = sRightVal;
                sResult = sLeftActual + sRightActual;
            }
            else if (sOp == "==" || sOp == "!=" || sOp == "<" || sOp == ">" || sOp == "<=" || sOp == ">=")
            {
                string sLeftCompare = sLeftVal;
                string sRightCompare = sRightVal;

                if (!bLeftIsNum)
                {
                    for (auto a : m_vVar)
                    {
                        if (a.sName == sLeftVal)
                        {
                            sLeftCompare = a.sValue;
                            break;
                        }
                    }
                }
                if (!bRightIsNum)
                {
                    for (auto a : m_vVar)
                    {
                        if (a.sName == sRightVal)
                        {
                            sRightCompare = a.sValue;
                            break;
                        }
                    }
                }

                sResult = CompareValues(sOp, sLeftCompare, sRightCompare) ? "1" : "0";
            }
        }

        if (!sResult.empty())
        {
            pNode->pData = CreateVarData(sResult);
            // pNode->eVarType = pNode->pData->eVarType;
        }
    }
    return;
}

void CScriptEngine::StatementUnknown(NODE *pNode)
{
    // function call: node has children and name matches a defined function
    if (!pNode->vpChild.empty())
    {
        auto it = find_if(m_vFunc.begin(), m_vFunc.end(), [&](const FUNCSAVETYPE &f)
                          { return f.sName == pNode->sValue; });
        if (it != m_vFunc.end())
        {
            // evaluate arguments
            vector<string> argVals;
            for (auto arg : pNode->vpChild)
            {
                Visit(arg);
                if (arg->pData != NULL)
                    argVals.push_back(arg->pData->sValue);
                else
                    argVals.push_back(string(""));
            }

            // bind parameters
            BeginScoop();
            for (size_t i = 0; i < it->params.size(); ++i)
            {
                VARSAVETYPE var;
                var.sName = it->params[i];
                var.sValue = i < argVals.size() ? argVals[i] : string("");
                var.nScoop = m_nScoopCount;
                m_vVar.push_back(var);
            }

            // execute body
            if (it->pBody)
            {
                Visit(it->pBody);
                if (it->pBody->pData)
                    pNode->pData = CreateVarData(it->pBody->pData->sValue);
            }
            EndScoop();
            return;
        }
    }

    if (pNode->vpChild.empty()) // single variable or literal
    {
        // if (!IsNumber(pNode->sValue))
        // {
        //     if (pNode->sValue.size() >= 2 &&
        //         pNode->sValue[0] == '"' && pNode->sValue.back() == '"')
        //     {
        //         // remove quota
        //         string strValue = pNode->sValue.substr(1, pNode->sValue.size() - 2);
        //         pNode->pData = CreateVarData(strValue);
        //         return;
        //     }

        for (auto a : m_vVar)
        {
            if (a.sName == pNode->sValue)
            {
                // VARSAVETYPE aCopy = a; // Create a copy to avoid dangling pointer issues
                // pNode->pData = &aCopy; // CreateVarData(a.sValue);
                pNode->pData = CreateVarData(a.sValue);
                return;
            }
        }
        // undefined
        pNode->pData = CreateVarData(pNode->sValue);
        //}
        // else
        // {
        //     pNode->pData = CreateVarData(pNode->sValue);
        // }
    }
}
void CScriptEngine::StatementArraySet(NODE *pNode)
{
    auto it = find_if(m_vArray.begin(), m_vArray.end(), [&](const ARRAYSAVETYPE &arr)
                      { return arr.sName == pNode->vpChild[0]->sValue; });
    if (it != m_vArray.end())
    {
        Visit(pNode->vpChild[1]);
        Visit(pNode->vpChild[2]);
        if (pNode->vpChild[1]->pData && pNode->vpChild[2]->pData)
        {
            size_t index = stoi(pNode->vpChild[1]->pData->sValue);
            if (index >= 0)
            {
                VARBASICTYPE v = pNode->vpChild[2]->pData->eVarType;
                // If array element type is not yet set, set it on first insertion
                if (it->eElemType == ANY && it->values.empty())
                {
                    it->eElemType = v;
                    it->bModified = true;
                }
                // Enforce type check
                if (v != it->eElemType)
                {
                    //afxDump << "Error: Type mismatch assigning to array "<<"\n";
                    it->eElemType = v;
                    //afxDump << it->sName << endl;
                    //return;  TODO: Fix Type Detection
                }

                if (index < it->values.size())
                {
                    it->values[index] = pNode->vpChild[2]->pData->sValue;
                }
                else
                {
                    it->values.push_back(pNode->vpChild[2]->pData->sValue);
                }
            }
        }
    }
}
void CScriptEngine::StatementArrayGet(NODE *pNode)
{
    auto it = find_if(m_vArray.begin(), m_vArray.end(), [&](const ARRAYSAVETYPE &arr)
                      { return arr.sName == pNode->vpChild[0]->sValue; });
    Visit(pNode->vpChild[1]);
    if (it != m_vArray.end())
    {
        size_t index = stoi(pNode->vpChild[1]->pData->sValue);
        if (index >= 0 && index < it->values.size())
        {
            pNode->pData = CreateVarData(it->values[index]);
        }
        else
        {
            pNode->pData = CreateVarData("Error: Array index out of bounds");
        }
    }
    else
    {
        pNode->pData = CreateVarData("Error: Array not found");
    }
}
void CScriptEngine::StatementArrayNew(NODE *pNode)
{
    if (pNode->vpChild.empty())
        return;
    string arrayName = pNode->vpChild[0]->sValue;
    m_vArray.push_back({arrayName, {}, ANY, m_nScoopCount});
}
void CScriptEngine::StatementArrayDel(NODE *pNode)
{
    if (pNode->vpChild.empty())
        return;
    string arrayName = pNode->vpChild[0]->sValue;
    auto it = find_if(m_vArray.begin(), m_vArray.end(), [&](const ARRAYSAVETYPE &arr)
                      { return arr.sName == arrayName; });
    Visit(pNode->vpChild[1]);
    if (it != m_vArray.end())
    {
        size_t index = stoi(pNode->vpChild[1]->pData->sValue);
        if (index >= 0 && index < it->values.size())
        {
            it->values.erase(it->values.begin() + index, it->values.begin() + index + 1);
            it->bModified = true;
        }
    }
}
void CScriptEngine::StatementConvert(NODE *pNode)
{
    if (pNode->vpChild.empty())
        return;
    if (pNode->vpChild.size() < 2)
        return;
    Visit(pNode->vpChild[1]);
    string targetType = pNode->vpChild[0]->sValue; // expected "NUM" or "STR"
    VARSAVETYPE *v = new VARSAVETYPE;
    v = pNode->vpChild[1]->pData;
    if (targetType == "STR")
    {
        v->eVarType = STR;
    }
    else if (targetType == "NUM")
    {
        v->eVarType = NUM;
    }
    else
    {
        //afxDump << "Error: Invalid target type for convert: "<<"\n";
            //afxDump << targetType << endl;
        return;
    }
    pNode->pData = v;
}
void CScriptEngine::StatementUpdate(NODE* pNode)
{
    UpdateInfo();
}
void CScriptEngine::StatementSleep(NODE* pNode)
{
    if (pNode->vpChild.empty())
        return;
    Visit(pNode->vpChild[0]);
    Sleep(stoi(pNode->vpChild[0]->pData->sValue));
}
void CScriptEngine::Visit(NODE *pNode)
{
    if (!pNode)
    {
        return;
    }
    switch (CheckCtrlType(pNode->sValue))
    {
    case IF:
        StatementIf(pNode);
        break;
    case FOR:
        StatementFor(pNode);
        break;
    case FUNCTION:
        StatementFunction(pNode);
        break;
    case EXP:
        StatementExp(pNode);
        break;
    case WRITE:
        StatementWrite(pNode);
        break;
    case PROGRAM:
        StatementProgram(pNode);
        break;
    case LET:
        StatementLet(pNode);
        break;
    case WHILE:
        StatementWhile(pNode);
        break;
    case ARRAYSET:
        StatementArraySet(pNode);
        break;
    case ARRAYGET:
        StatementArrayGet(pNode);
        break;
    case ARRAYNEW:
        StatementArrayNew(pNode);
        break;
    case ARRAYDEL:
        StatementArrayDel(pNode);
        break;
    case CONVERT:
        StatementConvert(pNode);
        break;
    case UPDATE:
        StatementUpdate(pNode);
		break;
	case SLEEP:
		StatementSleep(pNode);
		break;
    default:
        StatementUnknown(pNode);
        break;
    }
}
void CScriptEngine::PrintTree(NODE *pNode, int nDepth)
{
    //if (!pNode)
    //    return;

    //for (int i = 0; i < nDepth; i++)
        //afxDump << "  " << "\n";;

    //afxDump << pNode->sValue << endl;

    //for (auto pChild : pNode->vpChild)
    //{
     //   PrintTree(pChild, nDepth + 1);
    //}
}
void CScriptEngine::ReadFile(string filename)
{
    ifstream file(filename, ios::ate);
    if (file.is_open())
    {
        streamsize size = file.tellg();
        file.seekg(0, ios::beg);
        string content(static_cast<string::size_type>(size), ' ');
        file.read(&content[0], size);
        //afxDump << content << endl;
        m_sCode = content;
        file.close();
    }
}
void CScriptEngine::Run(const string &filename)
{
    LoadInfo();
    ReadFile(filename);
    vector<string> vsCodeData = Slice(m_sCode);
    m_pHead = new NODE;
    m_pHead->sValue = "program";
    auto begin = chrono::high_resolution_clock::now();
    BuildTree(m_pHead, vsCodeData);
    PrintTree(m_pHead);
    Visit(m_pHead);
    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::nanoseconds>(end - begin);
    TRACE("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);
    UpdateInfo();
}
void CScriptEngine::LoadInfo()
{
	std::lock_guard<std::mutex> guard(g_mtx);
	// Add math expressions
    m_vArray.clear();
	vector<string> vMathExpStr;
	for (auto &a : g_vMathExpression)
	{
		vMathExpStr.push_back(a.get_expression());
	}
	ARRAYSAVETYPE arrMath;
	arrMath.nScoop = 0;
	arrMath.eElemType = STR;
	arrMath.sName = "math_exp";
	arrMath.values = vMathExpStr;
	m_vArray.push_back(arrMath);

	// Add math expression sections
	vector<string> vMathSectionMin, vMathSectionMax;
	for (auto &a : g_vMathExpression)
	{
		float* sec = a.get_section();
		if (sec)
		{
			vMathSectionMin.push_back(to_string(sec[0]));
			vMathSectionMax.push_back(to_string(sec[1]));
		}
		else
		{
			vMathSectionMin.push_back(to_string(0.0f));
			vMathSectionMax.push_back(to_string(0.0f));
		}
	}
	ARRAYSAVETYPE arrMathMin; 
    arrMathMin.nScoop = 0; 
    arrMathMin.eElemType = NUM; 
    arrMathMin.sName = "math_section_min"; 
    arrMathMin.values = vMathSectionMin; 
    m_vArray.push_back(arrMathMin);
	
    ARRAYSAVETYPE arrMathMax; 
    arrMathMax.nScoop = 0; 
    arrMathMax.eElemType = NUM; 
    arrMathMax.sName = "math_section_max"; 
    arrMathMax.values = vMathSectionMax; 
    m_vArray.push_back(arrMathMax);

	// Add variables members
	vector<string> vVarName, vVarValue, vVarStep, vVarChange, vVarSectionMin, vVarSectionMax;
	for (auto &v : g_vVariable)
	{
		vVarName.push_back(v.get_name());
		vVarValue.push_back(to_string(v.get_value()));
		vVarStep.push_back(to_string(v.get_step()));
		vVarChange.push_back(v.is_change() ? string("1") : string("0"));
		float* vsec = v.get_section();
		if (vsec)
		{
			vVarSectionMin.push_back(to_string(vsec[0]));
			vVarSectionMax.push_back(to_string(vsec[1]));
		}
		else
		{
			vVarSectionMin.push_back(to_string(0.0f));
			vVarSectionMax.push_back(to_string(0.0f));
		}
	}
	ARRAYSAVETYPE arrVarNames; 
    arrVarNames.nScoop = 0; 
    arrVarNames.eElemType = STR; 
    arrVarNames.sName = "variable_name"; 
    arrVarNames.values = vVarName; 
    m_vArray.push_back(arrVarNames);

	ARRAYSAVETYPE arrVarValues; 
    arrVarValues.nScoop = 0; 
    arrVarValues.eElemType = NUM; 
    arrVarValues.sName = "variable_value"; 
    arrVarValues.values = vVarValue; 
    m_vArray.push_back(arrVarValues);

	ARRAYSAVETYPE arrVarStep; 
    arrVarStep.nScoop = 0; 
    arrVarStep.eElemType = NUM; 
    arrVarStep.sName = "variable_step"; 
    arrVarStep.values = vVarStep; 
    m_vArray.push_back(arrVarStep);

	ARRAYSAVETYPE arrVarChange; 
    arrVarChange.nScoop = 0; 
    arrVarChange.eElemType = NUM; 
    arrVarChange.sName = "variable_change"; 
    arrVarChange.values = vVarChange; 
    m_vArray.push_back(arrVarChange);

	ARRAYSAVETYPE arrVarSecMin; 
    arrVarSecMin.nScoop = 0; 
    arrVarSecMin.eElemType = NUM; 
    arrVarSecMin.sName = "variable_section_min"; 
    arrVarSecMin.values = vVarSectionMin; 
    m_vArray.push_back(arrVarSecMin);

	ARRAYSAVETYPE arrVarSecMax; 
    arrVarSecMax.nScoop = 0; 
    arrVarSecMax.eElemType = NUM; 
    arrVarSecMax.sName = "variable_section_max"; 
    arrVarSecMax.values = vVarSectionMax; 
    m_vArray.push_back(arrVarSecMax);
}
void CScriptEngine::UpdateInfo()
{
	std::lock_guard<std::mutex> guard(g_mtx);
	// Update global math expressions and variables from m_vArray
	// Update math expressions
	auto itMath = find_if(m_vArray.begin(), m_vArray.end(), [](const ARRAYSAVETYPE &a) { return a.sName == "math_exp"; });
    if (itMath != m_vArray.end() && itMath->bModified == true)
	{
		size_t count = itMath->values.size();
		g_vMathExpression.clear();
		g_vMathExpression.reserve(count);
		for (const auto &s : itMath->values)
		{
			CMathExpression m;
			m.set_expression(s);
			g_vMathExpression.push_back(m);
		}
		// update sections
		auto itMin = find_if(m_vArray.begin(), m_vArray.end(), [](const ARRAYSAVETYPE &a) { return a.sName == "math_section_min"; });
		auto itMax = find_if(m_vArray.begin(), m_vArray.end(), [](const ARRAYSAVETYPE &a) { return a.sName == "math_section_max"; });
		if (itMin != m_vArray.end() && itMax != m_vArray.end())
		{
			size_t n = g_vMathExpression.size();
			if (itMin->values.size() < n) n = itMin->values.size();
			if (itMax->values.size() < n) n = itMax->values.size();
			for (size_t i = 0; i < n; ++i)
			{
				float fmin = static_cast<float>(stof(itMin->values[i]));
				float fmax = static_cast<float>(stof(itMax->values[i]));
				g_vMathExpression[i].set_section(fmin, fmax);
			}
		}
	}

	// Update variables
	auto itVarName = find_if(m_vArray.begin(), m_vArray.end(), [](const ARRAYSAVETYPE &a) { return a.sName == "variable_name"; });
	auto itVarValue = find_if(m_vArray.begin(), m_vArray.end(), [](const ARRAYSAVETYPE &a) { return a.sName == "variable_value"; });
	auto itVarStep = find_if(m_vArray.begin(), m_vArray.end(), [](const ARRAYSAVETYPE &a) { return a.sName == "variable_step"; });
	auto itVarChange = find_if(m_vArray.begin(), m_vArray.end(), [](const ARRAYSAVETYPE &a) { return a.sName == "variable_change"; });
	auto itVarSecMin = find_if(m_vArray.begin(), m_vArray.end(), [](const ARRAYSAVETYPE &a) { return a.sName == "variable_section_min"; });
	auto itVarSecMax = find_if(m_vArray.begin(), m_vArray.end(), [](const ARRAYSAVETYPE &a) { return a.sName == "variable_section_max"; });

	if (itVarName != m_vArray.end())
	{
		size_t n = itVarName->values.size();
		g_vVariable.clear();
		g_vVariable.reserve(n);
		for (size_t i = 0; i < n; ++i)
		{
			CVariable var;
			var.set_name(itVarName->values[i]);
			if (itVarValue != m_vArray.end() && i < itVarValue->values.size())
				var.set_value(stof(itVarValue->values[i]));
			if (itVarStep != m_vArray.end() && i < itVarStep->values.size())
				var.set_step(stof(itVarStep->values[i]));
			if (itVarChange != m_vArray.end() && i < itVarChange->values.size())
				var.set_change(itVarChange->values[i] == "1");
			if (itVarSecMin != m_vArray.end() && itVarSecMax != m_vArray.end() && i < itVarSecMin->values.size() && i < itVarSecMax->values.size())
				var.set_section(stof(itVarSecMin->values[i]), stof(itVarSecMax->values[i]));
            g_vVariable.push_back(var);
        }
    }

    ::PostMessage(g_classViewWnd, WM_USER_NOTIFY, NULL, NULL);
    ::PostMessage(g_propertiesViewWnd, WM_USER_NOTIFY, NULL, NULL);
}

