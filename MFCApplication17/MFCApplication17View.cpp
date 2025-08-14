
// MFCApplication17View.cpp : implementation of the CMFCApplication17View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MFCApplication17.h"
#endif

#include "MFCApplication17Doc.h"
#include "MFCApplication17View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication17View

IMPLEMENT_DYNCREATE(CMFCApplication17View, CView)

BEGIN_MESSAGE_MAP(CMFCApplication17View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFCApplication17View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_MESSAGE(WM_USER_NOTIFY, &CMFCApplication17View::OnUserNotify)
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_HOME, &CMFCApplication17View::OnHome)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_SHOWPT, &CMFCApplication17View::OnShowpt)
	ON_COMMAND(ID_BUTTONNEWVARIABLE, &CMFCApplication17View::OnButtonnewvariable)
END_MESSAGE_MAP()

// CMFCApplication17View construction/destruction

CMFCApplication17View::CMFCApplication17View() noexcept
{
	// TODO: add construction code here
	m_pMemoryDC = NULL;

}

CMFCApplication17View::~CMFCApplication17View()
{

	if (m_pMemoryDC != NULL)
		delete m_pMemoryDC;
}

BOOL CMFCApplication17View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMFCApplication17View drawing

void CMFCApplication17View::OnDraw(CDC* pDC)
{
	g_viewHwnd = this->GetSafeHwnd();

	CRect rc;
	GetClientRect(&rc);
	//pDC->SetWindowOrg(500, 500);
	//pDC->Ellipse(-50, -50, 50, 50);
	m_coordinate.SetZeroPt(m_nOffsetX,m_nOffsetY);
	m_coordinate.SetBorder(rc);
	if (m_pMemoryDC == NULL)
	{
		m_pMemoryDC = new CDC;
		m_pMemoryDC->CreateCompatibleDC(pDC);
		CBitmap MemBitmap;
		MemBitmap.CreateCompatibleBitmap(pDC, 3000, 3000);
		CBitmap* pOldBit = m_pMemoryDC->SelectObject(&MemBitmap);
		m_pMemoryDC->FillSolidRect(0, 0, 3000, 3000, RGB(255, 255, 255));

	}
	else
	{
		m_pMemoryDC->FillSolidRect(0, 0, 3000, 3000, RGB(255, 255, 255));
	}
	
	m_coordinate.DrawCoordinate(m_pMemoryDC);
	if (m_bShowPt)
	{
		CPoint point, resPt;
		CString str;
		::GetCursorPos(&point);
		ScreenToClient(&point);
		for (auto a : g_vMathExpression)
		{
			resPt.x = point.x;
			if (!isnan(a.result((point.x - m_nOffsetX) * m_dZoom)))
			{
				resPt.y = m_nOffsetY + (-1) * (a.result((point.x - m_nOffsetX) * m_dZoom)) / m_dZoom;
				if (!isnan((double)resPt.y) && !isinf((double)resPt.y))
				{
					str.Format(L"%f,%f", (point.x - m_nOffsetX) * m_dZoom, (-1) * (resPt.y - m_nOffsetY) * m_dZoom);
					m_pMemoryDC->TextOut(resPt.x + 20, resPt.y, str);
					m_pMemoryDC->Ellipse(resPt.x - 5, resPt.y - 5, resPt.x + 5, resPt.y + 5);
				}
			}
		}
	}
	CPen oldPen;
	oldPen.CreatePen(0, 1, RGB(0, 0, 0));
	m_pMemoryDC->SelectObject(&oldPen);
	//DrawFunc(m_pMemoryDC);
	for_each(g_vMathExpression.begin(), g_vMathExpression.end(), 
		[=](auto exp) {
			exp.draw_function(m_pMemoryDC,m_coordinate); 
		}
	);
	pDC->BitBlt(0, 0, rc.Width(), rc.Height(), m_pMemoryDC, 0, 0, SRCCOPY);
	CMFCApplication17Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CMFCApplication17View printing


void CMFCApplication17View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMFCApplication17View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMFCApplication17View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMFCApplication17View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CMFCApplication17View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCApplication17View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMFCApplication17View diagnostics

#ifdef _DEBUG
void CMFCApplication17View::AssertValid() const
{
	CView::AssertValid();
}

void CMFCApplication17View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCApplication17Doc* CMFCApplication17View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCApplication17Doc)));
	return (CMFCApplication17Doc*)m_pDocument;
}
#endif //_DEBUG


// CMFCApplication17View message handlers


afx_msg LRESULT CMFCApplication17View::OnUserNotify(WPARAM wParam, LPARAM lParam)
{
	//AfxMessageBox(*(CString*)wParam+(CString)(*(COleVariant*)lParam));
	Invalidate(0);
	return 0;
}


void CMFCApplication17View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default	
	switch (nChar)
	{
	case VK_RIGHT:
		//AfxMessageBox(L"");
		m_nOffsetX-=50;
		break;
	case VK_LEFT:
		m_nOffsetX+=50;
		break;
	case VK_DOWN:
		m_nOffsetY-=50;
		break;
	case VK_UP:
		m_nOffsetY+=50;
		break;
	default:
		break;
	}
	Invalidate(0);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CMFCApplication17View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	if (zDelta < 0)
		m_dZoom *= 2;
	else
		m_dZoom /= 2;
	m_coordinate.SetScale(m_dZoom);

	Invalidate(0);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMFCApplication17View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		if (m_lastMousePt != CPoint(0, 0) && m_lastMousePt != point)
		{
			m_nOffsetX += point.x - m_lastMousePt.x;
			m_nOffsetY += point.y - m_lastMousePt.y;
		}
		m_lastMousePt = point;

	}
	else {
		m_lastMousePt.SetPoint(0, 0);
	}
	Invalidate(0);
	CView::OnMouseMove(nFlags, point);
}


void CMFCApplication17View::OnHome()
{
	m_nOffsetX = 500;
	m_nOffsetY = 500;
	m_dZoom = 10;
	Invalidate(0);
	// TODO: Add your command handler code here
}


void CMFCApplication17View::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnLButtonDown(nFlags, point);
}


void CMFCApplication17View::OnShowpt()
{
	m_bShowPt = !m_bShowPt;
	Invalidate(0);
	// TODO: Add your command handler code here
}


void CMFCApplication17View::OnButtonnewvariable()
{
	// TODO: Add your command handler code here
}
