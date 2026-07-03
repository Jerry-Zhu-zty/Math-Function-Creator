
#include "pch.h"
#include "framework.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "MFCApplication17.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd() noexcept
{
	m_nComboHeight = 0;
}

CPropertiesWnd::~CPropertiesWnd()
{
	delete m_pSendStr1;
	delete m_pSendStr2;
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_MESSAGE(WM_USER_SELECT, &CPropertiesWnd::OnUserSelect)
	ON_WM_TIMER()
	ON_MESSAGE(WM_USER_NOTIFY, &CPropertiesWnd::OnUserNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(), rectClient.Height() -(m_nComboHeight+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create Properties Combo \n");
		return -1;      // fail to create
	}

	m_wndObjectCombo.AddString(_T("Application"));
	m_wndObjectCombo.AddString(_T("Properties Window"));
	m_wndObjectCombo.SetCurSel(0);

	CRect rectCombo;
	m_wndObjectCombo.GetClientRect (&rectCombo);

	m_nComboHeight = rectCombo.Height();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	::SetTimer(this->GetSafeHwnd(), 1, 50, NULL);
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	std::lock_guard<std::mutex> guard(g_mtx);
	CString value = m_wndPropList.GetProperty(0)->GetSubItem(0)->GetValue();
	int index = 0;
	if (m_nCurrentType == 0)
	{
		for (auto& a : g_vMathExpression)
		{
			if (CA2W(a.get_expression().c_str()) == value)
			{
				g_vMathExpression.erase(g_vMathExpression.begin() + index);
				::PostMessage(m_classViewWnd, WM_USER_NOTIFY, NULL, NULL);
				break;
			}
			index++;

		}
	}
	else if (m_nCurrentType == 1)
	{
		for (auto& a : g_vVariable)
		{
			if (CA2W(a.get_name().c_str()) == value)
			{
				g_vVariable.erase(g_vVariable.begin() + index);
				for (auto a : g_vMathExpression)
				{
					a.refresh();
				}
				::PostMessage(m_classViewWnd, WM_USER_NOTIFY, NULL, NULL);
				break;
			}
			index++;
		}
	}
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{

	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::OnProperties2()
{
	std::lock_guard<std::mutex> guard(g_mtx);
	CString value = m_wndPropList.GetProperty(0)->GetSubItem(0)->GetValue();
	wstring wstr = value.GetString();
	string str(wstr.begin(), wstr.end());
	CMathExpression math;
	math.set_expression(str);
	if (isnan(math.result(0))) {
		AfxMessageBox(L"type error");
	}
	else {
		AfxMessageBox(L"ok");
	}
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Appearance"));

	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t)/*_T("About")*/m_currentStr, _T("Write a math function here")));
	CMFCPropertyGridProperty* pProp = NULL;//new CMFCPropertyGridProperty(_T("Action"), _T("Run"), _T("One of: None, Thin, Resizable, or Dialog Frame"));

	if (m_nCurrentType==1)
	{
		pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Value"), (_variant_t)((CVariable*)m_pCurrentItem)->get_value(), _T("Specifies the window's height")));
		pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Step"), (_variant_t)((CVariable*)m_pCurrentItem)->get_step(), _T("Specifies the window's height")));
		pProp = new CMFCPropertyGridProperty(_T("Action"), ((CVariable*)m_pCurrentItem)->is_change()?_T("Run"):_T("Pause"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
		pProp->AddOption(_T("Run"));
		pProp->AddOption(_T("Pause"));
		//pProp->AddOption(_T("Resizable"));
		//pProp->AddOption(_T("Dialog Frame"));
		pProp->AllowEdit(FALSE);

		pGroup1->AddSubItem(pProp);
	}
	
	
	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t)_T("About"), _T("Specifies the text that will be displayed in the window's title bar")));

	m_wndPropList.AddProperty(pGroup1);

	CMFCPropertyGridProperty* pSec = new CMFCPropertyGridProperty(_T("Section"), 0, TRUE);
	if (m_pCurrentItem != NULL)
	{
		if (m_nCurrentType == 0)
		{
			pProp = new CMFCPropertyGridProperty(_T("Left"), (_variant_t)((CMathExpression*)m_pCurrentItem)->get_section()[0], _T("Specifies the window's height"));
			//pProp->EnableSpinControl(TRUE);
			pSec->AddSubItem(pProp);

			pProp = new CMFCPropertyGridProperty(_T("Right"), (_variant_t)((CMathExpression*)m_pCurrentItem)->get_section()[1], _T("Specifies the window's width"));
			//pProp->EnableSpinControl(TRUE);
			pSec->AddSubItem(pProp);
			m_wndPropList.AddProperty(pSec);
		}
		else if (m_nCurrentType == 1)
		{
			pProp = new CMFCPropertyGridProperty(_T("Min"), (_variant_t)((CVariable*)m_pCurrentItem)->get_section()[0], _T("Specifies the window's height"));
			//pProp->EnableSpinControl(TRUE);
			pSec->AddSubItem(pProp);

			pProp = new CMFCPropertyGridProperty(_T("Max"), (_variant_t)((CVariable*)m_pCurrentItem)->get_section()[1], _T("Specifies the window's width"));
			//pProp->EnableSpinControl(TRUE);
			pSec->AddSubItem(pProp);
			m_wndPropList.AddProperty(pSec);
		}
	}
	pSec->Expand();

	/*CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	_tcscpy_s(lf.lfFaceName, _T("Arial"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t)true, _T("Specifies that the window uses MS Shell Dlg font")));

	m_wndPropList.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
	pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Application"));
	pProp->Enable(FALSE);
	pGroup3->AddSubItem(pProp);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"), RGB(210, 192, 254), nullptr, _T("Specifies the default window color"));
	pColorProp->EnableOtherButton(_T("Other..."));
	pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static const TCHAR szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));

	m_wndPropList.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (_variant_t)_T("Value 1"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (_variant_t)_T("Value 2"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (_variant_t)_T("Value 3"), _T("This is a description")));

	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);*/
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}


afx_msg LRESULT CPropertiesWnd::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	std::lock_guard<std::mutex> guard(g_mtx);
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)lParam;
	if (pProp == NULL) { return NULL; }
	CString name = pProp->GetName();
	CString value = (CString)pProp->GetValue();
	wstring wstr = value.GetString();
	string str(wstr.begin(),wstr.end());
	HWND hwnd = NULL;
	CMathExpression* pMath=NULL;
	CVariable* pVar=NULL;
	switch (m_nCurrentType)
	{
	case 0:
		for (auto& a : g_vMathExpression)
		{
			if (CA2W(a.get_expression().c_str()) == m_currentStr)
			{
				pMath = &a;
				break;
			}
		}
		break;
	case 1:
		for (auto& a : g_vVariable)
		{
			if (CA2W(a.get_name().c_str()) == m_currentStr)
			{
				pVar = &a;
				break;
			}
		}
		break;
	default:
		break;
	}
	if (m_nCurrentType == 0)
	{
		if (pMath != NULL)
		{
			if (name == L"Caption")
			{
				pMath->set_expression(str);
				m_currentStr = CA2W(str.c_str());
				::PostMessage(m_classViewWnd, WM_USER_NOTIFY, NULL, NULL);
			}
			else if (name == L"Left")
			{
				pMath->set_section_min(stof(str));
			}
			else if (name == L"Right")
			{
				pMath->set_section_max(stof(str));
			}
		}
	}
	else if (m_nCurrentType == 1)
	{
		if (pVar != NULL)
		{
			if (name == L"Caption")
			{
				pVar->set_name(str);
				for (auto& a : g_vMathExpression)
				{
					a.refresh();
				}
				m_currentStr = value;
				::PostMessage(m_classViewWnd, WM_USER_NOTIFY, NULL, NULL);
			}
			else if (name == L"Value")
			{
				pVar->set_value(stof(str));
			}
			else if (name == L"Max")
			{
				pVar->set_section_max(stof(str));
			}
			else if (name == L"Min")
			{
				pVar->set_section_min(stof(str));
			}
			else if (name == L"Step")
			{
				pVar->set_step(stof(str));
			}
			else if (name == L"Action")
			{
				if (str == "Run")
				{
					pVar->set_change(true);
				}
				else if(str == "Pause")
				{
					pVar->set_change(false);
				}
			}
		}
	}
	//if (name == L"Caption")
//{
//	if (m_nCurrentType == 0)
//	{
//		//for (auto& a : g_vMathExpression)
//		//{
//		//	if (CA2W(a.get_expression().c_str()) == m_currentStr)
//		//	{
//		//		math.set_expression(str);
//		//		/*if (isnan(math.result(0))) {
//		//			AfxMessageBox(L"type error");
//		//		}
//		//		else {*/
//		//		a.set_expression(str);
//		//		m_currentStr = CA2W(str.c_str());
//		//		//}
//		//		break;
//		//	}
//		//}
//		if(pMath!=NULL)pMath->set_expression(str);
//		m_currentStr = CA2W(str.c_str());
//	}
//	else if (m_nCurrentType == 1)
//	{
//		/*for (auto& a : g_vVariable)
//		{
//			if (CA2W(a.get_name().c_str()) == m_currentStr)
//			{
//				a.set_name(str);
//				break;
//			}
//		}*/
//		if (pVar != NULL)
//		{
//			pVar->set_name(str);
//		}
//	}
//	
//	if (m_pSendStr1 != NULL)
//		delete m_pSendStr1;
//	if (m_pSendStr2 != NULL)
//		delete m_pSendStr2;
//	m_pSendStr1 = new CString(name);
//	m_pSendStr2 = new CString(value);
//	//CWnd* pWnd = GetParentFrame();
//	/*HWND hParentWnd = pWnd->GetSafeHwnd();
//	CString strClassViewWnd;
//	BOOL bNameValid = strClassViewWnd.LoadString(IDB_CLASS_VIEW);
//	ASSERT(bNameValid);
//	HWND hChildWnndPropList.RemoveAll();d = FindWindowEx(hParentWnd, NULL, NULL, strClassViewWnd)->GetSafeHwnd();
//	::PostMessage(hChildWnd, WM_USER_SELECT,(WPARAM)m_pSendStr1, (LPARAM)m_pSendStr2);*/
//	::PostMessage(m_classViewWnd, WM_USER_NOTIFY, (WPARAM)m_pSendStr1, (LPARAM)m_pSendStr2);
//}
//else if (name == L"Value")
//{
//		/*for (auto& a : g_vVariable)
//		{
//			if (CA2W(a.get_name().c_str()) == m_currentStr)
//			{
//				a.set_value(stof(str));
//				break;
//			}
//		}*/
//	if (pVar != NULL)
//	{
//		pVar->set_value(stof(str));
//	}
//		//hwnd = ((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView()->GetSafeHwnd();
//		//if (hwnd != NULL)
//		//{
//		//}
//}
//
	::PostMessage(g_viewHwnd, WM_USER_NOTIFY, (WPARAM)m_pSendStr1, (LPARAM)m_pSendStr2);
	return 0;
}



afx_msg LRESULT CPropertiesWnd::OnUserSelect(WPARAM wParam, LPARAM lParam)
{
	std::lock_guard<std::mutex> guard(g_mtx);
	//TRACE(*(CString*)wParam);
	CMFCPropertyGridProperty* pGroup=m_wndPropList.GetProperty(0);
	CMFCPropertyGridProperty* pProp = NULL;
	CString str,captionStr;
	CMathExpression mathPara;
	CVariable varPara;
	if (pGroup != NULL) {
		m_pCurrentItem = (LPVOID)wParam;
		if (*(CString*)lParam == L"Function")
		{
			m_nCurrentType = 0;
			mathPara = *(CMathExpression*)wParam;
			captionStr = CA2W(mathPara.get_expression().c_str());
		}
		else if (*(CString*)lParam == L"Variable")
		{
			m_nCurrentType = 1;
			varPara = *(CVariable*)wParam;
			captionStr = CA2W(varPara.get_name().c_str());
		}
		else
		{
			m_wndPropList.RemoveAll();
			return 0;
		}
		m_wndPropList.RemoveAll();
		InitPropList();
		pGroup = m_wndPropList.GetProperty(0);
		if (pGroup == NULL) { return NULL; }
		for (int i = 0; i < pGroup->GetSubItemsCount(); i++)
		{
			pProp = pGroup->GetSubItem(i);
			if (pProp == NULL) { return NULL; }
			if ((str = pProp->GetName()) == L"Caption")
			{
				
				pProp->SetValue(captionStr);
				m_currentStr = captionStr;
				
			}
		}
		
	}
	return 0;

}




void CPropertiesWnd::OnTimer(UINT_PTR nIDEvent)
{
	std::lock_guard<std::mutex> guard(g_mtx);
	// TODO: Add your message handler code here and/or call default
	CMFCPropertyGridProperty* pGroup = m_wndPropList.GetProperty(0);
	CMFCPropertyGridProperty* pProp = NULL;
	if (nIDEvent == 1)
	{
		for (auto& a : g_vVariable)
		{
			if (a.is_change() == true)
			{
				if (a.get_step() > 0) {
					if (a.get_value() < a.get_section()[1]&& a.get_value()+a.get_step()<= a.get_section()[1])
					{
						a.set_value(a.get_value() + a.get_step());
					}
					else
					{
						a.set_step((-1) * a.get_step());
					}
				}
				else if (a.get_step() < 0)
				{
					if (a.get_value() > a.get_section()[0] && a.get_value() + a.get_step() >= a.get_section()[0])
					{
						a.set_value(a.get_value() + a.get_step());
					}
					else
					{
						a.set_step((-1) * a.get_step());
					}
				}
				if (m_currentStr == CA2W(a.get_name().c_str()))
				{
					if (pGroup == NULL) { return; }
					for (int i = 0; i < pGroup->GetSubItemsCount(); i++)
					{
						pProp = pGroup->GetSubItem(i);
						if (pProp == NULL) { return; }
						CString str;
						if ((str=pProp->GetName()) == L"Value")
						{
							pProp->SetValue(a.get_value());

						}
					}
				}
				::PostMessage(g_viewHwnd, WM_USER_NOTIFY,NULL,NULL);
			}
		}
	}
	CDockablePane::OnTimer(nIDEvent);
}

afx_msg LRESULT CPropertiesWnd::OnUserNotify(WPARAM wParam, LPARAM lParam)
{
	std::lock_guard<std::mutex> guard(g_mtx);
	CMFCPropertyGridProperty* pGroup = m_wndPropList.GetProperty(0);
	CMFCPropertyGridProperty* pProp = NULL;
	for (auto& a : g_vVariable)
	{
		if (m_currentStr == CA2W(a.get_name().c_str()))
		{
			if (pGroup == NULL) { return NULL; }
			for (int i = 0; i < pGroup->GetSubItemsCount(); i++)
			{
				pProp = pGroup->GetSubItem(i);
				if (pProp == NULL) { return NULL; }
				CString str;
				if ((str = pProp->GetName()) == L"Action")
				{
					if (a.is_change() == true)
						pProp->SetValue(L"Run");
					else if (a.is_change() == false)
						pProp->SetValue(L"Pause");
					//TODO:respond to the script message
				}
			}
		}
	}
	return 0;
}
