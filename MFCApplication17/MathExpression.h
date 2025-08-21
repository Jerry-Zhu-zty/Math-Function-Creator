// Inverse Polish notation
#pragma once
#include "pch.h"
#include "Variable.h"
using namespace std;
#define INVALIDEXP NAN
class CMathExpression;
class CVariable;
extern vector<CMathExpression> g_vMathExpression;
class CMathExpression 
{
protected:
	string m_func;
	vector<string> m_vExpression, m_vSolve;
public:
	CMathExpression() {
		// Operator/function type mapping for fast lookup
		m_map["+"] = 1; m_map["-"] = 1;
		m_map["*"] = 2; m_map["/"] = 2;
		m_map["^"] = 3;
		m_map["("] = 4;
		m_map[")"] = 5;
		m_map["sin"] = 6; m_map["cos"] = 6; m_map["tan"] = 6;
		m_map["pow"] = 6; m_map["log"] = 6;
		m_vpVariable.reserve(100);
	}
	~CMathExpression()
	{
		for (auto& a : m_vpVariable)
		{
			a->unregister_func(this);
		}
		m_vpVariable.clear();
	}
	void set_expression(string input)
	{
		m_func = input;
		for (auto& a : m_vpVariable)
		{
			a->unregister_func(this);
		}
		m_vpVariable.clear();
		m_vExpression = slice(input);
		m_vSolve = solve(m_vExpression);
	}
	void set_section(float min, float max)
	{
		m_fSection[0] = min;
		m_fSection[1] = max;
	}
	float* get_section()
	{
		return m_fSection;
	}
	string get_expression()
	{
		return m_func;
	}
	float result(float x = 0)
	{
		float fResult = 0;
		if (!m_func.empty())
		{
			vector<string> vCopy = m_vSolve;
			if (m_fSection[0] == m_fSection[1])
			{
				substitute_x(vCopy, x);
				
			}
			else if (x <= m_fSection[1] && x >= m_fSection[0])
			{
				substitute_x(vCopy, x);
			}
			else
			{
				return NAN;
			}
			
			//for (auto a = m_vpVariable.begin(); a != m_vpVariable.end(); a++)
			//{
			//	if ((*a) != NULL)
			//	{
			//		if (!substitute(vCopy, (*a)->get_value(), (*a)->get_name()))
			//		{
			//			m_vpVariable.erase(a);
			//		}
			//	}
			//	else
			//	{
			//		m_vpVariable.erase(a);
			//	}
			//}
			for (int i = 0; ; i++)
			{
				if (i >= m_vpVariable.size())
				{
					break;
				}
				if (m_vpVariable[i] != NULL)
				{
					if (!substitute(vCopy, m_vpVariable[i]->get_value(), m_vpVariable[i]->get_name()))
					{
						m_vpVariable.erase(m_vpVariable.begin()+i);
					}
				}
				else
				{
					m_vpVariable.erase(m_vpVariable.begin() + i);
				}
			}
			//try {
				fResult = calculate(vCopy);
			//}
			//catch (const exception& e) {
				//TRACE((CString)(e.what()));
				//return NAN;
			//}
			return fResult;
		}
		else return NAN;

	}
	void refresh()
	{
		for (auto& a : m_vpVariable)
		{
			a->unregister_func(this);
		}
		m_vpVariable.clear();
		m_vExpression = slice(m_func);
		m_vSolve = solve(m_vExpression);
	}
	//float result(string input, float x = 0)//not recommended
	//{
	//	float fResult = 0;
	//	if (m_vSolve.empty() || m_func != input)
	//	{
	//		m_func = input;
	//		m_vExpression = slice(input);
	//		m_vSolve = solve(m_vExpression);
	//	}
	//	vector<string> vCopy = m_vSolve;
	//	substitute_x(vCopy, x);
	//	for (auto a : m_vpVariable)
	//	{
	//		substitute(vCopy, a->get_value(), a->get_name());
	//	}
	//	try {
	//		fResult = calculate(vCopy);
	//	}
	//	catch (const exception& e) {
	//		//TRACE((CString)(e.what()));
	//		return NAN;
	//	}
	//	return fResult;
	//}
	void draw_function(CDC* pDC, CCoordinate coordinate)
	{
		POINT pt;
		float fResult = 0, fPreviousResult = 0;
		int nOffsetX = coordinate.GetZeroPt().x, nOffsetY = coordinate.GetZeroPt().y;
		float fZoom = coordinate.GetScale();

		for (int i = coordinate.GetBorder().left - nOffsetX; i < coordinate.GetBorder().right - nOffsetX; i++)
		{
			//stmp = func;
			/*if (pos=stmp.find('x'))
			{*/
			//stmp.replace(pos, 1, "("+to_string(i * fZoom)+")");
			fPreviousResult = result((i - 1) * fZoom);
			fResult = result(i * fZoom);
			/*fPreviousResult = m_math.result(stmp, (i - 1) * fZoom);
			fResult = m_math.result(stmp, i * fZoom);*/
			//rect.left = pt.x - 5;
			//rect.right = pt.x + 5;
			//rect.top = pt.y - 5;
			//rect.bottom = pt.y + 5;

			if (!isnan((double)fResult) && !isinf((double)fResult))
			{
				pt.x = nOffsetX + i;
				pt.y = nOffsetY + (-1) * fResult / fZoom;

				if (i == coordinate.GetBorder().left - nOffsetX || isnan((double)fPreviousResult) || isinf((double)fPreviousResult))
					pDC->MoveTo(pt);
				else
					pDC->LineTo(pt);
				pDC->SetPixel(pt.x, pt.y, RGB(0, 0, 0));

			}
			//pDC->Ellipse(&rect);
		//}
		}

	}
	void set_section_min(float min)
	{
		m_fSection[0] = min;
	}
	void set_section_max(float max)
	{
		m_fSection[1] = max;
	}


	bool are_all_digits(const string& str) const {
		if (str == "x") return true; // treat "x" as a number
		for (const char& c : str) {
			if (!isdigit(static_cast<unsigned char>(c)) && c != '.')
				return false;
		}
		return true;
	}
	bool are_all_alphas(const string& str) const {
		bool isAlpha = true;
		for_each(str.begin(), str.end(), 
			[&isAlpha](char c) 
			{
				if (!isalpha(c))
					isAlpha = false; 
			}
		);
		return isAlpha;
	}
	int identify(const string& str) const {
		if (are_all_digits(str))
			return 0;
		auto it = m_map.find(str);
		if (it != m_map.end())
			return it->second;
		if (are_all_alphas(str))
			return 7;
		return -1;
	}

	vector<string> solve(const vector<string>& vExpression) {
		stack<string> stk;
		vector<string> vRes;
		vRes.reserve(vExpression.size());
		for (auto& token : vExpression) {
			int id = identify(token);
			if (id == 0) {
				vRes.push_back(token);
			}
			else if (id == 7)
			{
				vRes.push_back(token);
				/*if (!register_variable(token))
					throw runtime_error("Invalid expression");*/
				register_variable(token);
			}
			else if (id >= 1 && id <= 4) {
				if (stk.empty() || identify(stk.top()) == 4 || id == 4)
					stk.push(token);
				else {
					while (!stk.empty() && identify(stk.top()) != 4 && id <= identify(stk.top())) {
						vRes.push_back(stk.top());
						stk.pop();
					}
					stk.push(token);
				}
			}
			else if (id == 5) {
				while (!stk.empty() && identify(stk.top()) != 4) {
					vRes.push_back(stk.top());
					stk.pop();
				}
				if (!stk.empty()) stk.pop(); // pop '('
				if (!stk.empty() && identify(stk.top()) == 6) {
					vRes.push_back(stk.top());
					stk.pop();
				}
			}
			else if (id == 6) { // function
				stk.push(token);
			}
		}
		while (!stk.empty()) {
			vRes.push_back(stk.top());
			stk.pop();
		}
		return vRes;
	}

	float calculate(const vector<string>& vExpression){
		stack<float> stk;
		for (const auto& token : vExpression) {
			int id = identify(token);
			if (id == 0 || (token[0] == '-' && token.length() > 1)) { // number
				stk.push(stof(token));
			}
			else if (id >= 1 && id <= 3) { // operators
				if (stk.size() < 2) return INVALIDEXP; //throw runtime_error("Invalid expression");
				float b = stk.top(); stk.pop();
				float a = stk.top(); stk.pop();
				if (token == "+") stk.push(a + b);
				else if (token == "-") stk.push(a - b);
				else if (token == "*") stk.push(a * b);
				else if (token == "/") stk.push(a / b);
				else if (token == "^") stk.push(pow(a, b));
			}
			else if (id == 6) { // function
				if (token == "sin" || token == "cos" || token == "tan") {
					if (stk.empty()) return INVALIDEXP;//throw runtime_error("Invalid expression");
					float a = stk.top(); stk.pop();
					if (token == "sin") stk.push(sin(a));
					else if (token == "cos") stk.push(cos(a));
					else if (token == "tan") stk.push(tan(a));
				}
				else if (token == "pow" || token == "log") {
					if (stk.size() < 2) return INVALIDEXP;//throw runtime_error("Invalid expression");
					float b = stk.top(); stk.pop();
					float a = stk.top(); stk.pop();
					if (token == "pow") stk.push(pow(a, b));
					else if (token == "log") stk.push(log(b) / log(a));
				}
			}
		}
		if (stk.empty()) return INVALIDEXP;//throw runtime_error("Invalid expression");
		return stk.top();
		/*if (m_fSection[0] == m_fSection[1])
		{
			return stk.top();
		}
		else if (stk.top() <= m_fSection[1] && stk.top() >= m_fSection[0]&&stk.top()!=INFINITY)
		{
			return stk.top();
		}
		else
		{
			return NAN;
		}*/
	}

	vector<string> slice(const string& str) const {
		vector<string> vRes;
		vRes.reserve(str.size());
		string stmp;
		int index = 0;
		char c;
		string s = str;
		for (auto it = s.begin(); it != s.end(); ++it) {
			c = *it;
			if (isalpha(c) && c != 'x') {
				string func;
				while (it != s.end() && isalpha(*it)) {
					func += *it;
					++it;
					++index;
				}
				--it;
				--index;
				vRes.push_back(func);
			}
			if ((vRes.empty() && c == '-') || (!vRes.empty() && c == '-' && !isdigit(s[index - 1]))) {
				s.replace(index, 1, "(0-1)*");
				it = s.begin() + index;
				c = '(';
			}
			if (!vRes.empty() && ((isdigit(c) && (isdigit(vRes.back()[0]) || vRes.back().back() == '.')) || c == '.')) {
				vRes.back() += c;
			}
			else if (
				/*the front of the number*/(isdigit(c) && (vRes.empty() || !isdigit(vRes.back()[0])))
				||
				/*operator*/(!isdigit(c) && c != '.' && !isalpha(c))
				) {
				stmp = c;
				vRes.push_back(stmp);
			}
			index++;
		}
		return vRes;
	}
	bool isdigit(char c) const {
		return ::isdigit(c) || c == 'x';
	}

	void substitute_x(vector<string>& vExpression, float x_value) const {
		for (auto& token : vExpression) {
			if (token == "x") token = to_string(x_value);
		}
	}
	bool substitute(vector<string>& vExpression, float value, string ch = "x") const
	{
		bool isChanged = false;
		for (auto& token : vExpression)
		{
			if (token == ch)
			{
				token = to_string(value);
				isChanged = true;
			}
		}
		return isChanged;
	}
	bool register_variable(string strVarName)
	{
		for (auto& a : g_vVariable)
		{
			if (a.get_name() == strVarName)
			{
				a.register_func(this);
				if (find(m_vpVariable.begin(), m_vpVariable.end(), &a) == m_vpVariable.end())
				{
					m_vpVariable.push_back(&a);
				}
				return true;
			}
		}
		return false;
	}
	void unregister_var(CVariable* pVar)
	{
		auto it = find(m_vpVariable.begin(), m_vpVariable.end(), pVar);
		if (it != m_vpVariable.end())
			m_vpVariable.erase(it);
	}
protected:
	unordered_map<string, int> m_map;
	vector<CVariable*> m_vpVariable;
	float m_fSection[2] = { 0,0 };
};
