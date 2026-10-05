
// MFCApplication17Doc.cpp : implementation of the CMFCApplication17Doc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MFCApplication17.h"
#endif

#include "MFCApplication17Doc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCApplication17Doc

IMPLEMENT_DYNCREATE(CMFCApplication17Doc, CDocument)

BEGIN_MESSAGE_MAP(CMFCApplication17Doc, CDocument)
END_MESSAGE_MAP()


// CMFCApplication17Doc construction/destruction

CMFCApplication17Doc::CMFCApplication17Doc() noexcept
{
	// TODO: add one-time construction code here

}

CMFCApplication17Doc::~CMFCApplication17Doc()
{
}

BOOL CMFCApplication17Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// Initialize per-document variables and expressions
	m_vVariable.clear();
	m_vVariable.reserve(100);
	CVariable var;
	var.set_name("a"); var.set_value(1);
	m_vVariable.push_back(var);
	var.set_name("b"); var.set_value(1);
	m_vVariable.push_back(var);

	m_vMathExpression.clear();
	m_vMathExpression.reserve(100);
	CMathExpression exp;
	std::string defaults[] = {"a*x+b"};
	for (auto &s : defaults)
	{
		if (s.length())
		{
			exp.set_expression(s);
			m_vMathExpression.push_back(exp);
			// register any variables from this document that the expression uses
			m_vMathExpression.back().register_variables(m_vVariable);
		}
	}

	return TRUE;
}




// CMFCApplication17Doc serialization

void CMFCApplication17Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CMFCApplication17Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CMFCApplication17Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CMFCApplication17Doc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMFCApplication17Doc diagnostics

#ifdef _DEBUG
void CMFCApplication17Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCApplication17Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMFCApplication17Doc commands
