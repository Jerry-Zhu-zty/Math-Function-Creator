#pragma once

#include "pch.h"
#include <afxrich.h>

class CRichEditPane : public CDockablePane
{
public:
    CRichEditPane() noexcept;
    virtual ~CRichEditPane();

    // load and save text file into the rich edit control
    BOOL LoadFile(const CString& strPath);
    BOOL SaveFile(const CString& strPath);

    void AdjustLayout();
protected:
    CRichEditCtrl m_wndRichEdit;
    CButton m_btnBold;
    CButton m_btnItalic;
    CButton m_btnUnderline;
    CButton m_btnClear;
    CButton m_btnCopy;
    CButton m_btnCut;
    CButton m_btnPaste;
    int m_nBtnHeight = 32;
    CFont m_btnFont;
    CFont m_editFont;
    CFont m_lineFont;
    CEdit m_wndLineNumbers;
    int m_nPrevFirst = -1;
    int m_nTotalLines = 0;
    UINT_PTR m_nTimerId = 0;
    int m_nLineHeight = 0;
    int m_nLastDigitWidth = 0;
    int m_nLastLineFontHeight = 0;

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnBnClickedBold();
    afx_msg void OnBnClickedItalic();
    afx_msg void OnBnClickedUnderline();
    afx_msg void OnBnClickedClear();
    afx_msg void OnUpdateBold(CCmdUI* pCmdUI);
    afx_msg void OnUpdateItalic(CCmdUI* pCmdUI);
    afx_msg void OnUpdateUnderline(CCmdUI* pCmdUI);
    afx_msg void OnUpdateClear(CCmdUI* pCmdUI);
    afx_msg void OnEditCopy();
    afx_msg void OnEditCut();
    afx_msg void OnEditPaste();
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();
};
