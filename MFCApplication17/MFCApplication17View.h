
// MFCApplication17View.h : interface of the CMFCApplication17View class
//

#pragma once

class CMFCApplication17View : public CView
{
protected: // create from serialization only
	CMFCApplication17View() noexcept;
	DECLARE_DYNCREATE(CMFCApplication17View)

// Attributes
public:
	CMFCApplication17Doc* GetDocument() const;

// Operations
public:
	CMathExpression m_math;
	CCoordinate m_coordinate;
	CDC* m_pMemoryDC;
	int m_nOffsetX = 500, m_nOffsetY = 500;
	double m_dZoom=10;
	CPoint m_lastMousePt;
	BOOL m_bShowPt = true;
//public:
	//void DrawFunc(/*string func, */CDC* pDC)//disabled
	//{
	//	string stmp;
	//	POINT pt;
	//	//RECT rect;
	//	//int pos;
	//	float fResult, fPreviousResult;
	//	for (auto &a : g_vMathExpression)
	//	{
	//		for (int i = m_coordinate.GetBorder().left - m_nOffsetX; i < m_coordinate.GetBorder().right - m_nOffsetX; i++)
	//		{
	//			//stmp = func;
	//			/*if (pos=stmp.find('x'))
	//			{*/
	//			//stmp.replace(pos, 1, "("+to_string(i * m_dZoom)+")");
	//			fPreviousResult = a.result((i - 1) * m_dZoom);
	//			fResult = a.result(i * m_dZoom);
	//			/*fPreviousResult = m_math.result(stmp, (i - 1) * m_dZoom);
	//			fResult = m_math.result(stmp, i * m_dZoom);*/
	//			//rect.left = pt.x - 5;
	//			//rect.right = pt.x + 5;
	//			//rect.top = pt.y - 5;
	//			//rect.bottom = pt.y + 5;

	//			if (!isnan((double)fResult) && !isinf((double)fResult))
	//			{
	//				pt.x = m_nOffsetX + i;
	//				pt.y = m_nOffsetY + (-1)*fResult / m_dZoom;

	//				if (i == m_coordinate.GetBorder().left - m_nOffsetX || isnan((double)fPreviousResult) || isinf((double)fPreviousResult))
	//					pDC->MoveTo(pt);
	//				else
	//					pDC->LineTo(pt);
	//				pDC->SetPixel(pt.x, pt.y, RGB(0, 0, 0));

	//			}
	//			//pDC->Ellipse(&rect);
	//		//}
	//		}
	//	}
	//}
// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CMFCApplication17View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnUserNotify(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHome();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowpt();
	afx_msg void OnButtonrun();
	afx_msg void OnButtonpause();
	afx_msg void OnFileSave();
	afx_msg void OnButtonrunscript();
};

#ifndef _DEBUG  // debug version in MFCApplication17View.cpp
inline CMFCApplication17Doc* CMFCApplication17View::GetDocument() const
   { return reinterpret_cast<CMFCApplication17Doc*>(m_pDocument); }
#endif

