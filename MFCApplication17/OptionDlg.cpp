// OptionDlg.cpp : implementation file
//

#include "pch.h"
#include "MFCApplication17.h"
#include "afxdialogex.h"
#include "OptionDlg.h"


// COptionDlg dialog

IMPLEMENT_DYNAMIC(COptionDlg, CDialogEx)

COptionDlg::COptionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPTION, pParent)
{
	m_bOldImplict = g_bImplicit;
}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialogEx)
	ON_BN_CLICKED(IDC_NORMALRADIO, &COptionDlg::OnBnClickedNormalradio)
	ON_BN_CLICKED(IDC_IMPLICITRADIO, &COptionDlg::OnBnClickedImplicitradio)
	ON_BN_CLICKED(IDCANCEL, &COptionDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// COptionDlg message handlers

void COptionDlg::OnBnClickedNormalradio()
{
	g_bImplicit = false;
	// TODO: Add your control notification handler code here
}

void COptionDlg::OnBnClickedImplicitradio()
{
	g_bImplicit = true;
	// TODO: Add your control notification handler code here
}

void COptionDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	g_bImplicit = m_bOldImplict;
	CDialogEx::OnCancel();
}

BOOL COptionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (g_bImplicit == false)
	{
		CheckRadioButton(IDC_NORMALRADIO, IDC_IMPLICITRADIO, IDC_NORMALRADIO);

	}
	else
	{
		CheckRadioButton(IDC_NORMALRADIO, IDC_IMPLICITRADIO, IDC_IMPLICITRADIO);
	}
	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
