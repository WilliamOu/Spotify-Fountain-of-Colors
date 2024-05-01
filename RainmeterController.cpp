#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>

// Base message for Rainmeter specific operations
const int WM_RAINMETER = WM_APP + 1000;
// Sub-message to execute a bang
const int RAINMETER_EXECUTE_BANG = 1;
// Tray icon ID
const UINT TRAY_ICON_ID = 1;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/// /// 
/// Utility Functions
/// /// 

bool IsSpotifyRunning() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (std::wstring(pe32.szExeFile) == L"Spotify.exe") {
                CloseHandle(hSnapshot);
                return true;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return false;
}

void SendBang(const WCHAR* bang) {
    static HWND rmWnd = NULL;
    if (!rmWnd) {
        rmWnd = FindWindow(L"DummyRainWClass", NULL);
        if (!rmWnd) {
            return;
        }
    }

    COPYDATASTRUCT cds;
    cds.dwData = 1;
    cds.cbData = (DWORD)(wcslen(bang) + 1) * sizeof(WCHAR);
    cds.lpData = (void*)bang;

    SendMessage(rmWnd, WM_COPYDATA, 0, (LPARAM)&cds);
}

bool IsSpotifyTopmost() {
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL) {
        return false;
    }

    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (pe32.th32ProcessID == processId) {
                CloseHandle(hSnapshot);
                return (std::wstring(pe32.szExeFile) == L"Spotify.exe");
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return false;
}

/// /// 
/// Main Processes
/// /// 

#define WM_TRAYICON (WM_APP + 1)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"TrayIconClass";

    if (!RegisterClassEx(&wc)) {
        return 1;
    }

    HWND hwnd = CreateWindowEx(0, L"TrayIconClass", L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
    if (!hwnd) {
        return 1;
    }

    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"Spotify Rainmeter Control");

    if (!Shell_NotifyIcon(NIM_ADD, &nid)) {
        return 1;
    }

    bool wasSpotifyRunning = false;
    bool wasTopmost = false;
    std::cout << "Program loaded" << std::endl;

    MSG msg;
    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        bool isSpotifyRunning = IsSpotifyRunning();
        bool isTopmost = IsSpotifyTopmost();

        if (isSpotifyRunning && !wasSpotifyRunning) {
            SendBang(L"!ActivateConfig \"Fountain of Colors\\Initialize\" \"Initialize.ini\"");
            SendBang(L"!Update");
            wasSpotifyRunning = true;
        }
        else if (!isSpotifyRunning && wasSpotifyRunning) {
            SendBang(L"!DeactivateConfig \"Fountain of Colors\\Initialize\"");
            wasSpotifyRunning = false;
            wasTopmost = false;
        }

        if (isSpotifyRunning) {
            if (isTopmost && !wasTopmost) {
                SendBang(L"!SetTransparency \"255\" \"Fountain of Colors\"");
                SendBang(L"!Update");
                wasTopmost = true;
            }
            else if (!isTopmost && wasTopmost) {
                SendBang(L"!SetTransparency \"0\" \"Fountain of Colors\"");
                SendBang(L"!Update");
                wasTopmost = false;
            }
        }

        Sleep(100);
    }

    Shell_NotifyIcon(NIM_DELETE, &nid);

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            SendBang(L"!DeactivateConfig \"Fountain of Colors\\Initialize\"");
            PostQuitMessage(0);
        }
        else if (lParam == WM_LBUTTONDBLCLK) {
            // Currently no actions taken
        }
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}