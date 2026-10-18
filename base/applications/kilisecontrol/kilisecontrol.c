/* kiliseOS Control Center: a lightweight shell front-end for existing CPLs. */
#define WIN32_NO_STATUS
#include <windef.h>
#include <winbase.h>
#include <wingdi.h>
#include <winuser.h>
#include <shellapi.h>

#define ID_WIFI       101
#define ID_SOUND      102
#define ID_DISPLAY    103
#define ID_POWER      104
#define ID_DATE       105
#define ID_APPS       106

static HINSTANCE g_hInstance;
static HFONT g_hTitle, g_hText;

static void OpenPanel(HWND hwnd, LPCWSTR target)
{
    ShellExecuteW(hwnd, L"open", target, NULL, NULL, SW_SHOWNORMAL);
}

static void DrawCard(HDC hdc, RECT r, LPCWSTR title, LPCWSTR subtitle, BOOL blue)
{
    HBRUSH bg = CreateSolidBrush(blue ? RGB(29, 113, 216) : RGB(255, 255, 255));
    HPEN pen = CreatePen(PS_SOLID, 1, blue ? RGB(29, 113, 216) : RGB(218, 222, 232));
    HGDIOBJ oldBrush = SelectObject(hdc, bg), oldPen = SelectObject(hdc, pen);
    RoundRect(hdc, r.left, r.top, r.right, r.bottom, 14, 14);
    SelectObject(hdc, oldBrush); SelectObject(hdc, oldPen);
    DeleteObject(bg); DeleteObject(pen);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, blue ? RGB(255,255,255) : RGB(30,34,42));
    SelectObject(hdc, g_hText);
    RECT text = { r.left + 16, r.top + 13, r.right - 12, r.top + 35 };
    DrawTextW(hdc, title, -1, &text, DT_LEFT | DT_SINGLELINE);
    SetTextColor(hdc, blue ? RGB(226,240,255) : RGB(103,110,124));
    text.top += 27; text.bottom += 32;
    DrawTextW(hdc, subtitle, -1, &text, DT_LEFT | DT_WORDBREAK);
}

static LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        g_hTitle = CreateFontW(-24,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,0,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Tahoma");
        g_hText = CreateFontW(-14,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,0,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_SWISS,L"Tahoma");
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_WIFI: OpenPanel(hwnd, L"ncpa.cpl"); break;
        case ID_SOUND: OpenPanel(hwnd, L"mmsys.cpl"); break;
        case ID_DISPLAY: OpenPanel(hwnd, L"desk.cpl"); break;
        case ID_POWER: OpenPanel(hwnd, L"powercfg.cpl"); break;
        case ID_DATE: OpenPanel(hwnd, L"timedate.cpl"); break;
        case ID_APPS: OpenPanel(hwnd, L"appwiz.cpl"); break;
        } return 0;
    case WM_LBUTTONUP:
    {
        int x = LOWORD(lParam), y = HIWORD(lParam);
        int col = (x - 24) / 180, row = (y - 110) / 100;
        if (x >= 24 && x < 744 && y >= 110 && y < 310 && col >= 0 && col < 4 && row >= 0 && row < 2)
        {
            static const int ids[2][4] = {{ ID_WIFI, ID_SOUND, ID_DISPLAY, ID_POWER }, { ID_DATE, ID_APPS, 0, 0 }};
            if (ids[row][col]) PostMessageW(hwnd, WM_COMMAND, ids[row][col], 0);
        }
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps); RECT rc; GetClientRect(hwnd, &rc);
        HBRUSH background = CreateSolidBrush(RGB(244,246,251)); FillRect(hdc, &rc, background); DeleteObject(background);
        SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(28,30,38)); SelectObject(hdc, g_hTitle);
        RECT title = { 24, 24, rc.right - 24, 58 }; DrawTextW(hdc, L"Control Center", -1, &title, DT_LEFT | DT_SINGLELINE);
        SelectObject(hdc, g_hText); SetTextColor(hdc, RGB(95,102,116)); title.top = 62; title.bottom = 88;
        DrawTextW(hdc, L"Quick access to your kiliseOS system settings", -1, &title, DT_LEFT | DT_SINGLELINE);
        RECT card;
        SetRect(&card, 24, 110, 196, 196); DrawCard(hdc, card, L"Network", L"Wi-Fi, Ethernet and connections", TRUE);
        SetRect(&card, 204, 110, 376, 196); DrawCard(hdc, card, L"Sound", L"Output, input and volume", FALSE);
        SetRect(&card, 384, 110, 556, 196); DrawCard(hdc, card, L"Display", L"Resolution and appearance", FALSE);
        SetRect(&card, 564, 110, 736, 196); DrawCard(hdc, card, L"Power", L"Battery and power plans", FALSE);
        SetRect(&card, 24, 210, 196, 296); DrawCard(hdc, card, L"Date & Time", L"Clock, region and timezone", FALSE);
        SetRect(&card, 204, 210, 376, 296); DrawCard(hdc, card, L"Applications", L"Install or remove programs", FALSE);
        EndPaint(hwnd, &ps); return 0;
    }
    case WM_DESTROY: DeleteObject(g_hTitle); DeleteObject(g_hText); PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE inst, HINSTANCE prev, PWSTR cmd, int show)
{
    WNDCLASSEXW wc = { sizeof(wc) }; g_hInstance = inst; wc.lpfnWndProc = MainProc; wc.hInstance = inst;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW); wc.lpszClassName = L"KiliseControlCenter"; RegisterClassExW(&wc);
    HWND hwnd = CreateWindowExW(WS_EX_APPWINDOW, wc.lpszClassName, L"Control Center", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 760, 370, NULL, NULL, inst, NULL);
    ShowWindow(hwnd, show); UpdateWindow(hwnd);
    MSG msg; while (GetMessageW(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessageW(&msg); } return (int)msg.wParam;
}
