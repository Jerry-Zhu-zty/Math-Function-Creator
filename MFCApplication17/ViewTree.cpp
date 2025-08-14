
#include "pch.h"
#include "framework.h"
#include "ViewTree.h"
#include "Resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree() noexcept
{
	
}

CViewTree::~CViewTree()
{
	if (m_pSelectStr != NULL)
		delete m_pSelectStr;
	if (m_pSelectParentStr != NULL)
		delete m_pSelectParentStr;
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, &CViewTree::OnNMClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree message handlers

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != nullptr);

#pragma warning(suppress : 26454)
	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != nullptr)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}
 
void CViewTree::OnNMClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here

	CWnd* pWnd = GetParentFrame();
	HWND hParentWnd = pWnd->GetSafeHwnd();
	CString strPropertiesWnd;
	BOOL bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	HWND hChildWnd = FindWindowEx(hParentWnd, NULL, NULL,strPropertiesWnd )->GetSafeHwnd();
	//HWND hPropertiesWnd = ::FindWindow(L"Properties", NULL);
	if (hChildWnd != NULL)
	{
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		HTREEITEM hItem = HitTest(pt);
		CString strText = GetItemText(hItem);
		Select(hItem,NULL);
		if (m_pSelectStr != NULL)
			delete m_pSelectStr;
		HTREEITEM hParentItem = GetParentItem(hItem);
		m_pSelectStr = new CString(strText);
		m_pSelectParentStr = new CString(GetItemText(hParentItem));
		//SetItemState(GetSelectedItem(),0,TVIS_SELECTED);
		if (*m_pSelectParentStr == "Function")
		{
			for (auto& a : g_vMathExpression)
			{
				if (CA2W(a.get_expression().c_str()) == (*m_pSelectStr))
				{
					::PostMessageW(hChildWnd, WM_USER_SELECT, (WPARAM)&a, (LPARAM)m_pSelectParentStr);
				}
			}
		}
		else if (*m_pSelectParentStr == "Variable")
		{
			for (auto& a : g_vVariable)
			{
				if (CA2W(a.get_name().c_str()) == (*m_pSelectStr))
				{
					::PostMessageW(hChildWnd, WM_USER_SELECT, (WPARAM)&a, (LPARAM)m_pSelectParentStr);
				}
			}
		}
		
		//if (hPropertiesWnd != NULL)
		//{
			//::AfxMessageBox(L"Select!");
		//	//::SendMessage(hPropertiesWnd, WM_USER_SELECT, (WPARAM)&GetItemText(GetSelectedItem()), NULL);
		//}
	}
	*pResult = 0;
}
