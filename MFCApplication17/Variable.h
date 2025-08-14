#pragma once
#include "pch.h"
using namespace std;
class CMathExpression;
class CVariable;
extern vector<CVariable> g_vVariable;
class CVariable
{
public:
    CVariable()
    {
        m_vpRegisteredFunc.clear();
    }
    ~CVariable()
    {
        //for (CMathExpression* a : m_vpRegisteredFunc)
        //{
        //    //a->unregister_var(this);
        //}
        m_vpRegisteredFunc.clear();
    }
    void set_name(string var)
    {
        m_strName = var;
        //for (CMathExpression *a : m_vpRegisteredFunc)
        //{
        //    //a->unregister_var(this);
        //}
    }

    string get_name()
    {
        return m_strName;
    }
    void set_step(float step)
    {
        m_fStep = step;
    }
    float get_step()
    {
        return m_fStep;
    }
    void set_section(float min, float max)
    {
        m_fSection[0] = min;
        m_fSection[1] = max;
    }
    void set_section_min(float min)
    {
        m_fSection[0] = min;
    }
    void set_section_max(float max)
    {
        m_fSection[1] = max;
    }
    float* get_section()
    {
        return m_fSection;
    }
    void register_func(CMathExpression* pMath)
    {
        //if (pMath != NULL&& find(m_vpRegisteredFunc.begin(), m_vpRegisteredFunc.end(), pMath)== m_vpRegisteredFunc.end())
          //  m_vpRegisteredFunc.push_back(pMath);
    }
    void unregister_func(CMathExpression* pMath)
    {
        //auto it = find(m_vpRegisteredFunc.begin(), m_vpRegisteredFunc.end(), pMath);
        //if(it!= m_vpRegisteredFunc.end())
          //  m_vpRegisteredFunc.erase(it);
    }
    void set_value(float fValue)
    {
        m_fValue = fValue;
    }
    float get_value()
    {
        return m_fValue;
    }
    void set_change(bool isChange)
    {
        m_isChange = isChange;
    }
    bool is_change()
    {
        return m_isChange;
    }
protected:
    string m_strName;
    float m_fValue = 0;
    vector <CMathExpression*> m_vpRegisteredFunc;
    float m_fStep = 0.1;
    bool m_isChange = false;
    float m_fSection[2] = { 0,0 };
};

