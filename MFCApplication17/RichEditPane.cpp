#include "pch.h"
#include "RichEditPane.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CRichEditPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
    ON_BN_CLICKED(1001, &CRichEditPane::OnBnClickedBold)
    ON_BN_CLICKED(1002, &CRichEditPane::OnBnClickedItalic)
    ON_BN_CLICKED(1003, &CRichEditPane::OnBnClickedUnderline)
    ON_BN_CLICKED(1004, &CRichEditPane::OnBnClickedClear)
    ON_UPDATE_COMMAND_UI(1001, &CRichEditPane::OnUpdateBold)
    ON_UPDATE_COMMAND_UI(1002, &CRichEditPane::OnUpdateItalic)
    ON_UPDATE_COMMAND_UI(1003, &CRichEditPane::OnUpdateUnderline)
    ON_UPDATE_COMMAND_UI(1004, &CRichEditPane::OnUpdateClear)
    ON_COMMAND(ID_EDIT_COPY, &CRichEditPane::OnEditCopy)
    ON_COMMAND(ID_EDIT_CUT, &CRichEditPane::OnEditCut)
    ON_COMMAND(ID_EDIT_PASTE, &CRichEditPane::OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CRichEditPane::OnUpdateEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CRichEditPane::OnUpdateEditCut)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CRichEditPane::OnUpdateEditPaste)
    ON_BN_CLICKED(ID_EDIT_COPY, &CRichEditPane::OnEditCopy)
    ON_BN_CLICKED(ID_EDIT_CUT, &CRichEditPane::OnEditCut)
    ON_BN_CLICKED(ID_EDIT_PASTE, &CRichEditPane::OnEditPaste)
    ON_WM_PAINT()
END_MESSAGE_MAP()

CRichEditPane::CRichEditPane() noexcept
{
}

CRichEditPane::~CRichEditPane()
{
}

int CRichEditPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    // reduce flicker by preventing parent from drawing over child controls
    ModifyStyle(0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // create rich edit control (add clip styles so it doesn't draw over child buttons)
    if (!m_wndRichEdit.Create(WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rectDummy, this, 2001))
    {
        TRACE0("Failed to create rich edit control\n");
        return -1;
    }

    // create toolbar buttons with larger size
    LOGFONT lf = {0};
    lf.lfHeight = 18; // font size for buttons
    lf.lfWeight = FW_BOLD;
    _tcscpy_s(lf.lfFaceName, _T("Segoe UI"));
    m_btnFont.CreateFontIndirect(&lf);

    // set rich edit font to Consolas
    LOGFONT lfEdit = {0};
    lfEdit.lfHeight = -MulDiv(10, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72); // 10pt
    lfEdit.lfWeight = FW_NORMAL;
    _tcscpy_s(lfEdit.lfFaceName, _T("Consolas"));
    m_editFont.CreateFontIndirect(&lfEdit);
    m_wndRichEdit.SetFont(&m_editFont);

    m_btnBold.Create(L"B", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, CRect(0,0,40,32), this, 1001);
    m_btnBold.SetFont(&m_btnFont);
    m_btnItalic.Create(L"I", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, CRect(44,0,84,32), this, 1002);
    m_btnItalic.SetFont(&m_btnFont);
    m_btnUnderline.Create(L"U", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, CRect(88,0,128,32), this, 1003);
    m_btnUnderline.SetFont(&m_btnFont);
    m_btnClear.Create(L"Clear", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, CRect(132,0,220,32), this, 1004);
    m_btnClear.SetFont(&m_btnFont);
    m_btnCopy.Create(L"Copy", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, CRect(220,0,280,32), this, ID_EDIT_COPY);
    m_btnCopy.SetFont(&m_btnFont);
    m_btnCut.Create(L"Cut", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, CRect(284,0,340,32), this, ID_EDIT_CUT);
    m_btnCut.SetFont(&m_btnFont);
    m_btnPaste.Create(L"Paste", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, CRect(348,0,420,32), this, ID_EDIT_PASTE);
    m_btnPaste.SetFont(&m_btnFont);

    // ensure controls are enabled and rich edit is behind buttons
    EnableWindow(TRUE);
    m_btnBold.EnableWindow(TRUE); m_btnBold.ShowWindow(SW_SHOW);
    m_btnItalic.EnableWindow(TRUE); m_btnItalic.ShowWindow(SW_SHOW);
    m_btnUnderline.EnableWindow(TRUE); m_btnUnderline.ShowWindow(SW_SHOW);
    m_btnClear.EnableWindow(TRUE); m_btnClear.ShowWindow(SW_SHOW);
    m_btnCopy.EnableWindow(TRUE); m_btnCopy.ShowWindow(SW_SHOW);
    m_btnCut.EnableWindow(TRUE); m_btnCut.ShowWindow(SW_SHOW);
    m_btnPaste.EnableWindow(TRUE); m_btnPaste.ShowWindow(SW_SHOW);
    if (m_wndRichEdit.GetSafeHwnd())
        ::SetWindowPos(m_wndRichEdit.GetSafeHwnd(), HWND_BOTTOM, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);

    // create line numbers control (read-only). use CreateEx and check result so it's not NULL
    // create line numbers without a vertical scroll bar (we'll sync to the main edit)
    BOOL bLn = m_wndLineNumbers.CreateEx(WS_EX_CLIENTEDGE, _T("EDIT"), nullptr,
        WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE,
        CRect(0,0,40,40), this, 5001);
    if (!bLn)
    {
        TRACE("Failed to create line numbers control, error=%u\n", ::GetLastError());
    }
    else
    {
        m_wndLineNumbers.SetFont(&m_editFont);
        m_wndLineNumbers.ShowWindow(SW_SHOW);
        // hide its own vertical scrollbar; line numbers should not scroll independently
        m_wndLineNumbers.ShowScrollBar(SB_VERT, FALSE);
    }
    m_nTimerId = ::SetTimer(this->GetSafeHwnd(), 5001, 200, NULL);
    AdjustLayout();

    // try to load default src.txt in the application folder
    CString strDefault = _T("src.txt");
    LoadFile(strDefault);
    return 0;
}

// Helper: load file (assume UTF-8) into the rich edit control
BOOL CRichEditPane::LoadFile(const CString& strPath)
{
    if (strPath.IsEmpty())
        return FALSE;

    std::ifstream ifs(CT2A(strPath, CP_UTF8), std::ios::binary);
    if (!ifs)
        return FALSE;
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    // detect UTF-8 BOM
    const unsigned char* data = (const unsigned char*)content.data();
    int offset = 0;
    if (content.size() >= 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
        offset = 3;

    if (content.size() <= (size_t)offset)
    {
        m_wndRichEdit.SetWindowText(L"");
        return TRUE;
    }

    int utf8Len = (int)content.size() - offset;
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, content.data() + offset, utf8Len, NULL, 0);
    if (wideLen <= 0)
        return FALSE;
    CStringW wstr;
    MultiByteToWideChar(CP_UTF8, 0, content.data() + offset, utf8Len, wstr.GetBuffer(wideLen), wideLen);
    wstr.ReleaseBuffer(wideLen);
    m_wndRichEdit.SetWindowText(wstr);
    return TRUE;
}

// Helper: save content of rich edit control to file as UTF-8 (with BOM)
BOOL CRichEditPane::SaveFile(const CString& strPath)
{
    if (strPath.IsEmpty())
        return FALSE;
    CStringW wtext;
    m_wndRichEdit.GetWindowTextW(wtext);
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wtext, -1, NULL, 0, NULL, NULL);
    if (utf8Len <= 0)
        return FALSE;
    std::string utf8;
    utf8.resize(utf8Len - 1);
    WideCharToMultiByte(CP_UTF8, 0, wtext, -1, &utf8[0], utf8Len, NULL, NULL);

    std::ofstream ofs(CT2A(strPath, CP_UTF8), std::ios::binary | std::ios::trunc);
    if (!ofs)
        return FALSE;
    // write BOM
    unsigned char bom[3] = {0xEF,0xBB,0xBF};
    ofs.write((const char*)bom, 3);
    if (!utf8.empty())
        ofs.write(utf8.data(), (std::streamsize)utf8.size());
    return ofs.good();
}

void CRichEditPane::AdjustLayout()
{
    if (GetSafeHwnd() == nullptr)
        return;

    CRect rectClient;
    GetClientRect(rectClient);

    CRect rectToolbar = rectClient;
    rectToolbar.top = rectClient.top;
    rectToolbar.bottom = rectClient.top + m_nBtnHeight;

    CRect rcLine = rectClient;
    rcLine.left = rectClient.left;
    rcLine.right = rectClient.left + 40; // line number area width

    CRect rectEdit = rectClient;
    rectEdit.left = rcLine.right + 2;
    rectEdit.top = rectToolbar.bottom + 2;

    m_btnBold.SetWindowPos(nullptr, rectToolbar.left + 6, rectToolbar.top + 4, 40, 30, SWP_NOACTIVATE | SWP_NOZORDER);
    m_btnItalic.SetWindowPos(nullptr, rectToolbar.left + 52, rectToolbar.top + 4, 40, 30, SWP_NOACTIVATE | SWP_NOZORDER);
    m_btnUnderline.SetWindowPos(nullptr, rectToolbar.left + 98, rectToolbar.top + 4, 40, 30, SWP_NOACTIVATE | SWP_NOZORDER);
    m_btnClear.SetWindowPos(nullptr, rectToolbar.left + 144, rectToolbar.top + 4, 76, 30, SWP_NOACTIVATE | SWP_NOZORDER);
    m_btnCopy.SetWindowPos(nullptr, rectToolbar.left + 220, rectToolbar.top + 4, 60, 30, SWP_NOACTIVATE | SWP_NOZORDER);
    m_btnCut.SetWindowPos(nullptr, rectToolbar.left + 284, rectToolbar.top + 4, 56, 30, SWP_NOACTIVATE | SWP_NOZORDER);
    m_btnPaste.SetWindowPos(nullptr, rectToolbar.left + 348, rectToolbar.top + 4, 72, 30, SWP_NOACTIVATE | SWP_NOZORDER);

    m_wndRichEdit.SetWindowPos(nullptr, rectEdit.left+22, rectEdit.top, rectEdit.Width()-24, rectEdit.Height()-2, SWP_NOACTIVATE | SWP_NOZORDER);
    m_wndLineNumbers.SetWindowPos(nullptr, rcLine.left+2, rectEdit.top, rcLine.Width()+20, rectEdit.Height()-2, SWP_NOACTIVATE | SWP_NOZORDER);

    // Force rich edit to the bottom of the z-order within the pane and buttons to top so buttons receive mouse clicks
    if (m_wndRichEdit.GetSafeHwnd() != NULL)
    {
        ::SetWindowPos(m_wndRichEdit.GetSafeHwnd(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    if (m_btnBold.GetSafeHwnd() != NULL)
        ::SetWindowPos(m_btnBold.GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (m_btnItalic.GetSafeHwnd() != NULL)
        ::SetWindowPos(m_btnItalic.GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (m_btnUnderline.GetSafeHwnd() != NULL)
        ::SetWindowPos(m_btnUnderline.GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (m_btnClear.GetSafeHwnd() != NULL)
        ::SetWindowPos(m_btnClear.GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (m_btnCopy.GetSafeHwnd() != NULL)
        ::SetWindowPos(m_btnCopy.GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (m_btnCut.GetSafeHwnd() != NULL)
        ::SetWindowPos(m_btnCut.GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (m_btnPaste.GetSafeHwnd() != NULL)
        ::SetWindowPos(m_btnPaste.GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void CRichEditPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);
    AdjustLayout();
    // force immediate redraw of pane and children to avoid residual artifacts
    RedrawWindow(nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
}

void CRichEditPane::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == m_nTimerId)
    {
        // get total lines and first visible line in the rich edit
        int nLines = m_wndRichEdit.GetLineCount();
        int nFirstVisible = (int)::SendMessage(m_wndRichEdit.GetSafeHwnd(), EM_GETFIRSTVISIBLELINE, 0, 0);

        // if number of lines changed, rebuild the line numbers text and scroll to the same first visible line
        if (nLines != m_nTotalLines)
        {
            m_nTotalLines = nLines;
            CString str;
            str.Empty();
            for (int i = 1; i <= nLines; ++i)
            {
                str.AppendFormat(L"%d\r\n", i);
            }
            m_wndLineNumbers.SetWindowText(str);
            // ensure the line numbers control is scrolled to the same first visible line
            if (m_wndLineNumbers.GetSafeHwnd())
            {
                ::SendMessage(m_wndLineNumbers.GetSafeHwnd(), EM_LINESCROLL, 0, nFirstVisible);
            }
            m_nPrevFirst = nFirstVisible;
        }
        else
        {
            // scroll the line numbers control by the difference in first visible line
            if (nFirstVisible != m_nPrevFirst)
            {
                if (m_wndLineNumbers.GetSafeHwnd())
                {
                    int nDelta = 0;
                    if (m_nPrevFirst == -1)
                        nDelta = nFirstVisible; // initial alignment
                    else
                        nDelta = nFirstVisible - m_nPrevFirst;
                    ::SendMessage(m_wndLineNumbers.GetSafeHwnd(), EM_LINESCROLL, 0, nDelta);
                }
                m_nPrevFirst = nFirstVisible;
            }
        }
    }
    CDockablePane::OnTimer(nIDEvent);
}

void CRichEditPane::OnSetFocus(CWnd* pOldWnd)
{
    CDockablePane::OnSetFocus(pOldWnd);
    m_wndRichEdit.SetFocus();
}

BOOL CRichEditPane::OnEraseBkgnd(CDC* pDC)
{
    // prevent flicker by erasing background and forcing toolbar area redraw
    CRect rc;
    GetClientRect(&rc);
    pDC->FillSolidRect(&rc, ::GetSysColor(COLOR_3DFACE));
    // redraw toolbar area immediately to avoid ghosting when resizing
    CRect rectToolbar = rc;
    rectToolbar.bottom = rectToolbar.top + m_nBtnHeight;
    pDC->FillSolidRect(&rectToolbar, ::GetSysColor(COLOR_BTNFACE));
    return TRUE; // we've erased background
}

void CRichEditPane::OnBnClickedBold()
{
    // toggle bold on selection
    CHARFORMAT cf = {0};
    cf.cbSize = sizeof(cf);
    m_wndRichEdit.GetSelectionCharFormat(cf);
    if (cf.dwEffects & CFE_BOLD)
        cf.dwEffects &= ~CFE_BOLD;
    else
        cf.dwEffects |= CFE_BOLD;
    cf.dwMask = CFM_BOLD;
    m_wndRichEdit.SetSelectionCharFormat(cf);
}

void CRichEditPane::OnBnClickedItalic()
{
    CHARFORMAT cf = {0};
    cf.cbSize = sizeof(cf);
    m_wndRichEdit.GetSelectionCharFormat(cf);
    if (cf.dwEffects & CFE_ITALIC)
        cf.dwEffects &= ~CFE_ITALIC;
    else
        cf.dwEffects |= CFE_ITALIC;
    cf.dwMask = CFM_ITALIC;
    m_wndRichEdit.SetSelectionCharFormat(cf);
}

void CRichEditPane::OnBnClickedUnderline()
{
    CHARFORMAT cf = {0};
    cf.cbSize = sizeof(cf);
    m_wndRichEdit.GetSelectionCharFormat(cf);
    if (cf.dwEffects & CFE_UNDERLINE)
        cf.dwEffects &= ~CFE_UNDERLINE;
    else
        cf.dwEffects |= CFE_UNDERLINE;
    cf.dwMask = CFM_UNDERLINE;
    m_wndRichEdit.SetSelectionCharFormat(cf);
}

void CRichEditPane::OnBnClickedClear()
{
    m_wndRichEdit.SetWindowText(L"");
}

void CRichEditPane::OnUpdateBold(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

void CRichEditPane::OnUpdateItalic(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

void CRichEditPane::OnUpdateUnderline(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

void CRichEditPane::OnUpdateClear(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

void CRichEditPane::OnEditCopy()
{
    m_wndRichEdit.Copy();
}

void CRichEditPane::OnEditCut()
{
    m_wndRichEdit.Cut();
}

void CRichEditPane::OnEditPaste()
{
    m_wndRichEdit.Paste();
}

void CRichEditPane::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_wndRichEdit.GetSelectionType() != 0);
}

void CRichEditPane::OnUpdateEditCut(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_wndRichEdit.GetSelectionType() != 0);
}

void CRichEditPane::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
    // enable paste if clipboard has text
    BOOL bHasText = ::IsClipboardFormatAvailable(CF_UNICODETEXT) || ::IsClipboardFormatAvailable(CF_TEXT);
    pCmdUI->Enable(bHasText);
}

BOOL CRichEditPane::PreTranslateMessage(MSG* pMsg)
{
    // Support keyboard shortcuts Ctrl+C/X/V when rich edit has focus
    if (pMsg->message == WM_KEYDOWN && (GetFocus()->GetSafeHwnd() == m_wndRichEdit.GetSafeHwnd()))
    {
        bool ctrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
        if (ctrl)
        {
            switch (pMsg->wParam)
            {
            case 'C':
                OnEditCopy(); return TRUE;
            case 'X':
                OnEditCut(); return TRUE;
            case 'V':
                OnEditPaste(); return TRUE;
            default:
                break;
            }
        }
    }
    return CDockablePane::PreTranslateMessage(pMsg);
}

void CRichEditPane::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: Add your message handler code here
    // Do not call CDockablePane::OnPaint() for painting messages
    dc.TextOutW(500, 10, L"src.txt must be included in the same folder Unfinished ReadOnly!!!");
}
