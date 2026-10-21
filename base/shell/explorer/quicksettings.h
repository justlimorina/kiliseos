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
    HFONT m_hFontBold;
    HFONT m_hFontNormal;
    HFONT m_hFontSmall;

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
        m_hFontBold(NULL),
        m_hFontNormal(NULL),
        m_hFontSmall(NULL)
    {
    }

    ~CQuickSettingsWnd()
    {
        if (m_hFontBold) DeleteObject(m_hFontBold);
        if (m_hFontNormal) DeleteObject(m_hFontNormal);
        if (m_hFontSmall) DeleteObject(m_hFontSmall);
    }

    BEGIN_MSG_MAP(CQuickSettingsWnd)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
        MESSAGE_HANDLER(WM_COMMAND, OnCommand)
        MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
        MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
    END_MSG_MAP()

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
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
                                         WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_ENABLESELRANGE,
                                         50, 68, 185, 24,
                                         m_hWnd, (HMENU)IDC_QS_TRACKBAR_VOL,
                                         GetModuleHandleW(NULL), NULL);
        if (m_hwndVolTrack)
        {
            SendMessageW(m_hwndVolTrack, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
            SendMessageW(m_hwndVolTrack, TBM_SETPOS, TRUE, m_CurrentVolPercent);
        }

        /* 2x2 Quick Toggle Buttons */
        INT btnW = 135;
        INT btnH = 28;

        m_hwndBtnNetwork = CreateWindowExW(0, L"BUTTON", L"Network",
                                           WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                           12, 105, btnW, btnH,
                                           m_hWnd, (HMENU)IDC_QS_BTN_NETWORK,
                                           GetModuleHandleW(NULL), NULL);
        SendMessageW(m_hwndBtnNetwork, WM_SETFONT, (WPARAM)m_hFontNormal, TRUE);

        m_hwndBtnSettings = CreateWindowExW(0, L"BUTTON", L"Settings",
                                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                            153, 105, btnW, btnH,
                                            m_hWnd, (HMENU)IDC_QS_BTN_SETTINGS,
                                            GetModuleHandleW(NULL), NULL);
        SendMessageW(m_hwndBtnSettings, WM_SETFONT, (WPARAM)m_hFontNormal, TRUE);

        m_hwndBtnTheme = CreateWindowExW(0, L"BUTTON", L"Display / Themes",
                                         WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                         12, 140, btnW, btnH,
                                         m_hWnd, (HMENU)IDC_QS_BTN_THEME,
                                         GetModuleHandleW(NULL), NULL);
        SendMessageW(m_hwndBtnTheme, WM_SETFONT, (WPARAM)m_hFontNormal, TRUE);

        m_hwndBtnLock = CreateWindowExW(0, L"BUTTON", L"Lock Screen",
                                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                        153, 140, btnW, btnH,
                                        m_hWnd, (HMENU)IDC_QS_BTN_LOCK,
                                        GetModuleHandleW(NULL), NULL);
        SendMessageW(m_hwndBtnLock, WM_SETFONT, (WPARAM)m_hFontNormal, TRUE);

        /* Date & Time button */
        m_hwndBtnDate = CreateWindowExW(0, L"BUTTON", L"Date & Time Settings",
                                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                        12, 178, 175, 26,
                                        m_hWnd, (HMENU)IDC_QS_BTN_DATE,
                                        GetModuleHandleW(NULL), NULL);
        SendMessageW(m_hwndBtnDate, WM_SETFONT, (WPARAM)m_hFontSmall, TRUE);

        /* Shut Down button */
        m_hwndBtnShutdown = CreateWindowExW(0, L"BUTTON", L"Shut Down",
                                            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                                            195, 178, 93, 26,
                                            m_hWnd, (HMENU)IDC_QS_BTN_SHUTDOWN,
                                            GetModuleHandleW(NULL), NULL);
        SendMessageW(m_hwndBtnShutdown, WM_SETFONT, (WPARAM)m_hFontNormal, TRUE);

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

        HFONT oldFont = (HFONT)SelectObject(memDC, m_hFontBold);
        SetTextColor(memDC, RGB(240, 244, 248));
        TextOutW(memDC, 46, 15, L"Administrator", 13);

        SelectObject(memDC, m_hFontSmall);
        SetTextColor(memDC, RGB(154, 160, 166)); // #9AA0A6
        TextOutW(memDC, 46, 30, L"kiliseOS Quick Settings", 23);

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
            ShowWindow(SW_HIDE);
        }
        return 0;
    }

    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        ShowWindow(SW_HIDE);
        return 0;
    }

    VOID Toggle(HWND hwndAnchor)
    {
        if (m_hWnd == NULL)
        {
            RECT rc = { 0, 0, 300, 245 };
            Create(NULL, rc, L"kiliseOS Quick Settings", WS_POPUP | WS_BORDER, WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
        }

        if (IsWindowVisible())
        {
            ShowWindow(SW_HIDE);
        }
        else
        {
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
