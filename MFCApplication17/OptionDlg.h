#pragma once
#include "afxdialogex.h"
#include "pch.h"

// COptionDlg dialog

class COptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~COptionDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	bool m_bOldImplict = false;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedNormalradio();
	afx_msg void OnBnClickedImplicitradio();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
