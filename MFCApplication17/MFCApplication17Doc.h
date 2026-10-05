
// MFCApplication17Doc.h : interface of the CMFCApplication17Doc class
//


#pragma once

#include "MathExpression.h"
#include "Variable.h"

class CMFCApplication17Doc : public CDocument
{
protected: // create from serialization only
	CMFCApplication17Doc() noexcept;
	DECLARE_DYNCREATE(CMFCApplication17Doc)

// Attributes
public:
	std::vector<CMathExpression>& GetMathExpressions() { return m_vMathExpression; }
	std::vector<CVariable>& GetVariables() { return m_vVariable; }

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CMFCApplication17Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// Per-document storage for math expressions and variables
	std::vector<CMathExpression> m_vMathExpression;
	std::vector<CVariable> m_vVariable;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
