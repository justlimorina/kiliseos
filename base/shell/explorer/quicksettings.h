/*
 * PROJECT:     kiliseOS Explorer
 * LICENSE:     LGPL-2.1-or-later (https://spdx.org/licenses/LGPL-2.1-or-later)
 * PURPOSE:     ChromeOS / MD3 Quick Settings Flyout Window
 * COPYRIGHT:   Copyright 2026 Michael Vu <justlimorina@gmail.com>
 */

#pragma once

#define IDC_QS_TRACKBAR_VOL   2001
#define IDC_QS_BTN_LOCK       2002
#define IDC_QS_BTN_SETTINGS   2003
#define IDC_QS_BTN_THEME      2004
#define IDC_QS_BTN_NETWORK    2005
#define IDC_QS_BTN_SHUTDOWN   2006
#define IDC_QS_BTN_DATE       2007

class CQuickSettingsWnd :
    public CWindowImpl<CQuickSettingsWnd, CWindow, CControlWinTraits>
{
    HWND m_hwndVolTrack;
    HWND m_hwndBtnLock;
    HWND m_hwndBtnSettings;
    HWND m_hwndBtnTheme;
    HWND m_hwndBtnNetwork;
    HWND m_hwndBtnShutdown;
    HWND m_hwndBtnDate;
    DWORD m_CurrentVolPercent;
    DWORD m_dwHideTick;
    HFONT m_hFontBold;
    HFONT m_hFontNormal;
    HFONT m_hFontSmall;
    HBRUSH m_hBgBrush;

public:
    DECLARE_WND_CLASS_EX(L"kiliseOS_QuickSettingsWnd", CS_HREDRAW | CS_VREDRAW, COLOR_WINDOW)

    CQuickSettingsWnd() :
        m_hwndVolTrack(NULL),
        m_hwndBtnLock(NULL),
        m_hwndBtnSettings(NULL),
        m_hwndBtnTheme(NULL),
        m_hwndBtnNetwork(NULL),
        m_hwndBtnShutdown(NULL),
        m_hwndBtnDate(NULL),
        m_CurrentVolPercent(75),
        m_dwHideTick(0),
        m_hFontBold(NULL),
        m_hFontNormal(NULL),
        m_hFontSmall(NULL),
        m_hBgBrush(NULL)
    {
    }

    ~CQuickSettingsWnd()
    {
        if (m_hFontBold) DeleteObject(m_hFontBold);
        if (m_hFontNormal) DeleteObject(m_hFontNormal);
        if (m_hFontSmall) DeleteObject(m_hFontSmall);
        if (m_hBgBrush) DeleteObject(m_hBgBrush);
    }

    BEGIN_MSG_MAP(CQuickSettingsWnd)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
        MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
        MESSAGE_HANDLER(WM_COMMAND, OnCommand)
        MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
    END_MSG_MAP()

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        SetWindowRgn(CreateRoundRectRgn(0, 0, 301, 246, 12, 12), TRUE);
        m_hBgBrush = CreateSolidBrush(RGB(25, 28, 30));

        NONCLIENTMETRICSW ncm;
        ncm.cbSize = sizeof(ncm);
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);

        LOGFONTW lf = ncm.lfMessageFont;
        lf.lfHeight = -12;
        m_hFontNormal = CreateFontIndirectW(&lf);

        lf.lfWeight = FW_BOLD;
        m_hFontBold = CreateFontIndirectW(&lf);

        lf.lfHeight = -10;
        lf.lfWeight = FW_NORMAL;
        m_hFontSmall = CreateFontIndirectW(&lf);

        /* Volume Slider */
        m_hwndVolTrack = CreateWindowExW(0, TRACKBAR_CLASSW, L"",
                                         WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_BOTH,
                                         50, 68, 185, 24,
                                         m_hWnd, (HMENU)IDC_QS_TRACKBAR_VOL,
                                         GetModuleHandleW(NULL), NULL);
        if (m_hwndVolTrack)
        {
            SendMessageW(m_hwndVolTrack, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
            SendMessageW(m_hwndVolTrack, TBM_SETPOS, TRUE, m_CurrentVolPercent);
        }

        /* 2x2 Quick Toggle Buttons (MD3 Owner-Drawn Pills) */
        INT btnW = 135;
        INT btnH = 28;
        DWORD dwBtnStyle = WS_CHILD | WS_VISIBLE | BS_OWNERDRAW;

        m_hwndBtnNetwork = CreateWindowExW(0, L"BUTTON", L"Network",
                                           dwBtnStyle, 12, 105, btnW, btnH,
                                           m_hWnd, (HMENU)IDC_QS_BTN_NETWORK,
                                           GetModuleHandleW(NULL), NULL);

        m_hwndBtnSettings = CreateWindowExW(0, L"BUTTON", L"Settings",
                                            dwBtnStyle, 153, 105, btnW, btnH,
                                            m_hWnd, (HMENU)IDC_QS_BTN_SETTINGS,
                                            GetModuleHandleW(NULL), NULL);

        m_hwndBtnTheme = CreateWindowExW(0, L"BUTTON", L"Display / Themes",
                                         dwBtnStyle, 12, 140, btnW, btnH,
                                         m_hWnd, (HMENU)IDC_QS_BTN_THEME,
                                         GetModuleHandleW(NULL), NULL);

        m_hwndBtnLock = CreateWindowExW(0, L"BUTTON", L"Lock Screen",
                                        dwBtnStyle, 153, 140, btnW, btnH,
                                        m_hWnd, (HMENU)IDC_QS_BTN_LOCK,
                                        GetModuleHandleW(NULL), NULL);

        /* Date & Time button */
        m_hwndBtnDate = CreateWindowExW(0, L"BUTTON", L"Date & Time Settings",
                                        dwBtnStyle, 12, 178, 175, 26,
                                        m_hWnd, (HMENU)IDC_QS_BTN_DATE,
                                        GetModuleHandleW(NULL), NULL);

        /* Shut Down button */
        m_hwndBtnShutdown = CreateWindowExW(0, L"BUTTON", L"Shut Down",
                                            dwBtnStyle, 195, 178, 93, 26,
                                            m_hWnd, (HMENU)IDC_QS_BTN_SHUTDOWN,
                                            GetModuleHandleW(NULL), NULL);

        return TRUE;
    }

    LRESULT OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        HDC hdc = (HDC)wParam;
        SetBkColor(hdc, RGB(25, 28, 30));
        SetTextColor(hdc, RGB(240, 244, 248));
        return (LRESULT)m_hBgBrush;
    }

    LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;
        if (!lpdis || lpdis->CtlType != ODT_BUTTON)
            return FALSE;

        HDC hdc = lpdis->hDC;
        RECT rc = lpdis->rcItem;
        BOOL bPressed = (lpdis->itemState & ODS_SELECTED) != 0;
        BOOL bIsShutdown = (lpdis->CtlID == IDC_QS_BTN_SHUTDOWN);

        /* Clear button corner background with card color */
        FillRect(hdc, &rc, m_hBgBrush);

        COLORREF clrFill = bIsShutdown
            ? (bPressed ? RGB(102, 157, 246) : RGB(138, 180, 248))
            : (bPressed ? RGB(45, 50, 56) : RGB(33, 36, 40));
        COLORREF clrBorder = bIsShutdown
            ? RGB(138, 180, 248)
            : RGB(58, 64, 72);

        HBRUSH hBrush = CreateSolidBrush(clrFill);
        HPEN hPen = CreatePen(PS_SOLID, 1, clrBorder);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

        RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 10, 10);

        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(hBrush);
        DeleteObject(hPen);

        WCHAR szText[64] = { 0 };
        ::GetWindowTextW(lpdis->hwndItem, szText, _countof(szText));

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, bIsShutdown ? RGB(19, 21, 24) : RGB(240, 244, 248));
        HFONT hFontUse = bIsShutdown ? m_hFontBold : (lpdis->CtlID == IDC_QS_BTN_DATE ? m_hFontSmall : m_hFontNormal);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFontUse);

        DrawTextW(hdc, szText, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, hOldFont);
        return TRUE;
    }

    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 1;
    }

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(&ps);
        if (!hdc) return 0;

        RECT rcClient;
        GetClientRect(&rcClient);

        /* Double buffer */
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBM = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
        HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);

        /* Background: Obsidian Card #191C1E */
        HBRUSH bgBrush = CreateSolidBrush(RGB(25, 28, 30));
        FillRect(memDC, &rcClient, bgBrush);
        DeleteObject(bgBrush);

        /* Card border #2D3238 */
        HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(45, 50, 56));
        HPEN oldPen = (HPEN)SelectObject(memDC, borderPen);
        SelectObject(memDC, GetStockObject(NULL_BRUSH));
        RoundRect(memDC, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 12, 12);
        SelectObject(memDC, oldPen);
        DeleteObject(borderPen);

        SetBkMode(memDC, TRANSPARENT);

        /* Header: User avatar & title */
        HBRUSH avatarBrush = CreateSolidBrush(RGB(138, 180, 248)); // #8AB4F8 Cyan accent
        RECT rcAvatar = { 14, 12, 38, 36 };
        SelectObject(memDC, avatarBrush);
        SelectObject(memDC, GetStockObject(NULL_PEN));
        Ellipse(memDC, rcAvatar.left, rcAvatar.top, rcAvatar.right, rcAvatar.bottom);
        DeleteObject(avatarBrush);

        WCHAR szUser[64] = L"User";
        DWORD cchUser = _countof(szUser);
        if (!GetUserNameW(szUser, &cchUser) || szUser[0] == L'\0')
            wcscpy(szUser, L"User");

        HFONT oldFont = (HFONT)SelectObject(memDC, m_hFontBold);
        SetTextColor(memDC, RGB(19, 21, 24));
        WCHAR szInitial[2] = { (WCHAR)towupper(szUser[0]), L'\0' };
        DrawTextW(memDC, szInitial, 1, &rcAvatar, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SetTextColor(memDC, RGB(240, 244, 248));
        TextOutW(memDC, 46, 12, szUser, lstrlenW(szUser));

        SelectObject(memDC, m_hFontSmall);
        SetTextColor(memDC, RGB(154, 160, 166)); // #9AA0A6
        TextOutW(memDC, 46, 27, L"kiliseOS Quick Settings", 23);

        /* Divider lines */
        HPEN divPen = CreatePen(PS_SOLID, 1, RGB(45, 50, 56));
        SelectObject(memDC, divPen);
        MoveToEx(memDC, 12, 50, NULL);
        LineTo(memDC, rcClient.right - 12, 50);

        /* Volume Label */
        SelectObject(memDC, m_hFontNormal);
        SetTextColor(memDC, RGB(232, 234, 237));
        TextOutW(memDC, 12, 70, L"Vol:", 4);

        WCHAR szVol[16];
        wsprintfW(szVol, L"%u%%", (unsigned int)m_CurrentVolPercent);
        TextOutW(memDC, 245, 70, szVol, lstrlenW(szVol));

        /* Divider before date */
        MoveToEx(memDC, 12, 212, NULL);
        LineTo(memDC, rcClient.right - 12, 212);

        /* Current Date string at bottom */
        SYSTEMTIME st;
        GetLocalTime(&st);
        WCHAR szDate[64];
        GetDateFormatW(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL, szDate, _countof(szDate));
        SelectObject(memDC, m_hFontSmall);
        SetTextColor(memDC, RGB(154, 160, 166));
        TextOutW(memDC, 14, 222, szDate, lstrlenW(szDate));

        SelectObject(memDC, oldFont);
        SelectObject(memDC, oldPen);
        DeleteObject(divPen);

        /* BitBlt to screen */
        BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBM);
        DeleteObject(memBM);
        DeleteDC(memDC);

        EndPaint(&ps);
        return 0;
    }

    LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (lParam != 0 && (HWND)lParam == m_hwndVolTrack)
        {
            m_CurrentVolPercent = (DWORD)SendMessageW(m_hwndVolTrack, TBM_GETPOS, 0, 0);
            DWORD dwVol = (m_CurrentVolPercent * 0xFFFF) / 100;
            waveOutSetVolume(0, MAKELONG(dwVol, dwVol));
            InvalidateRect(NULL, FALSE);
        }
        return 0;
    }

    LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        WORD wID = LOWORD(wParam);
        switch (wID)
        {
        case IDC_QS_BTN_LOCK:
            ShowWindow(SW_HIDE);
            LockWorkStation();
            break;

        case IDC_QS_BTN_SETTINGS:
            ShowWindow(SW_HIDE);
            ShellExecuteW(m_hWnd, L"open", L"control.exe", NULL, NULL, SW_SHOWNORMAL);
            break;

        case IDC_QS_BTN_THEME:
            ShowWindow(SW_HIDE);
            ShellExecuteW(m_hWnd, L"open", L"desk.cpl", NULL, NULL, SW_SHOWNORMAL);
            break;

        case IDC_QS_BTN_NETWORK:
            ShowWindow(SW_HIDE);
            ShellExecuteW(m_hWnd, L"open", L"ncpa.cpl", NULL, NULL, SW_SHOWNORMAL);
            break;

        case IDC_QS_BTN_DATE:
            ShowWindow(SW_HIDE);
            ShellExecuteW(m_hWnd, L"open", L"timedate.cpl", NULL, NULL, SW_SHOWNORMAL);
            break;

        case IDC_QS_BTN_SHUTDOWN:
        {
            ShowWindow(SW_HIDE);
            HWND hwndTray = FindWindowW(L"Shell_TrayWnd", NULL);
            if (hwndTray)
            {
                PostMessageW(hwndTray, TWM_DOEXITWINDOWS, 0, 0);
            }
            break;
        }
        }
        return 0;
    }

    LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            m_dwHideTick = GetTickCount();
            ShowWindow(SW_HIDE);
        }
        return 0;
    }

    VOID Toggle(HWND hwndAnchor)
    {
        if (m_hWnd == NULL)
        {
            RECT rc = { 0, 0, 300, 245 };
            Create(NULL, rc, L"kiliseOS Quick Settings", WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
        }

        if (IsWindowVisible())
        {
            m_dwHideTick = GetTickCount();
            ShowWindow(SW_HIDE);
        }
        else
        {
            if (GetTickCount() - m_dwHideTick < 250)
                return;

            RECT rcAnchor;
            ::GetWindowRect(hwndAnchor, &rcAnchor);

            INT w = 300;
            INT h = 245;
            INT x = rcAnchor.right - w;
            INT y = rcAnchor.top - h - 6;

            if (x < 0) x = 0;
            if (y < 0) y = 0;

            SetWindowPos(HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
            SetForegroundWindow(m_hWnd);
            SetFocus();

            /* Sync current audio volume */
            DWORD dwVol = 0;
            if (waveOutGetVolume(0, &dwVol) == MMSYSERR_NOERROR)
            {
                m_CurrentVolPercent = (LOWORD(dwVol) * 100) / 0xFFFF;
                if (m_hwndVolTrack)
                {
                    SendMessageW(m_hwndVolTrack, TBM_SETPOS, TRUE, m_CurrentVolPercent);
                }
            }
            InvalidateRect(NULL, FALSE);
        }
    }
};

extern CQuickSettingsWnd g_QuickSettingsWnd;
