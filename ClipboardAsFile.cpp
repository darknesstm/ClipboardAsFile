// ClipboardAsFile.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ClipboardAsFile.h"
#include "Config.h"
#include "ClipboardManager.h"
#include "AutoStartup.h"
#include <shellapi.h>
#include <shlobj.h>
#include <string>
#include <commctrl.h>
#include <objbase.h>
#include <oleidl.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")

#define MAX_LOADSTRING 100
#define WM_TRAYICON (WM_USER + 1)
#define TIMER_CLEANUP 1
#define HOTKEY_ID 1

// SHGetIDropTarget may not be available in some SDK headers; declare it here
extern "C" HRESULT WINAPI SHGetIDropTarget(HWND hwnd, IDropTarget **ppdt);

// 全局变量:
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
NOTIFYICONDATAW nid = {};
Config g_config;
HWND g_hWnd = NULL;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Settings(HWND, UINT, WPARAM, LPARAM);
void                AddTrayIcon(HWND hWnd);
void                RemoveTrayIcon();
void                ShowTrayMenu(HWND hWnd);
void                ConvertClipboard(bool autoPaste = false);
void                RegisterGlobalHotkey(HWND hWnd);
void                UnregisterGlobalHotkey(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIPBOARDASFILE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, SW_HIDE))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIPBOARDASFILE));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIPBOARDASFILE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLIPBOARDASFILE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;

   // 不显示主窗口，只显示托盘图标
   // ShowWindow(hWnd, nCmdShow);
   // UpdateWindow(hWnd);

   // 添加托盘图标
   AddTrayIcon(hWnd);

   // 注册全局热键
   RegisterGlobalHotkey(hWnd);

   // 启动清理定时器（每小时检查一次）
   SetTimer(hWnd, TIMER_CLEANUP, 3600000, NULL);

   // 立即执行一次清理
   ClipboardManager::CleanupOldFiles(g_config.GetTempPath(), g_config.GetDeleteAfterHours());

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONUP)
        {
            ShowTrayMenu(hWnd);
        }
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case ID_TRAY_CONVERT:
            case IDM_CONVERT:
                ConvertClipboard(false);
                break;
            case ID_TRAY_SETTINGS:
            case IDM_SETTINGS:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGS), hWnd, Settings);
                break;
            case ID_TRAY_ABOUT:
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case ID_TRAY_EXIT:
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_HOTKEY:
        if (wParam == HOTKEY_ID)
        {
            ConvertClipboard(true);
        }
        break;

    case WM_TIMER:
        if (wParam == TIMER_CLEANUP)
        {
            ClipboardManager::CleanupOldFiles(g_config.GetTempPath(), g_config.GetDeleteAfterHours());
        }
        break;

    case WM_DESTROY:
        KillTimer(hWnd, TIMER_CLEANUP);
        UnregisterGlobalHotkey(hWnd);
        RemoveTrayIcon();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// "关于"框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// "设置"对话框的消息处理程序
INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        {
            SetDlgItemTextW(hDlg, IDC_TEMP_PATH, g_config.GetTempPath().c_str());
            SetDlgItemInt(hDlg, IDC_DELETE_AFTER, g_config.GetDeleteAfterHours(), FALSE);
            
            CheckDlgButton(hDlg, IDC_AUTO_STARTUP, 
                AutoStartup::IsAutoStartupEnabled(L"ClipboardAsFile") ? BST_CHECKED : BST_UNCHECKED);
            
            CheckDlgButton(hDlg, IDC_HOTKEY_ENABLED, 
                g_config.GetHotkeyEnabled() ? BST_CHECKED : BST_UNCHECKED);
            
            UINT mod = g_config.GetHotkeyMod();
            CheckDlgButton(hDlg, IDC_HOTKEY_CTRL, (mod & MOD_CONTROL) ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_HOTKEY_ALT, (mod & MOD_ALT) ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_HOTKEY_SHIFT, (mod & MOD_SHIFT) ? BST_CHECKED : BST_UNCHECKED);
            
            HWND hCombo = GetDlgItem(hDlg, IDC_HOTKEY_KEY);
            const WCHAR* keys[] = {L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", 
                                   L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", 
                                   L"U", L"V", L"W", L"X", L"Y", L"Z"};
            for (int i = 0; i < 26; i++)
            {
                SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)keys[i]);
            }
            SendMessageW(hCombo, CB_SETCURSEL, g_config.GetHotkeyVk() - 'A', 0);
        }
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDC_BROWSE:
                {
                    BROWSEINFOW bi = {};
                    bi.hwndOwner = hDlg;
                    bi.lpszTitle = L"选择临时文件目录";
                    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
                    
                    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
                    if (pidl != NULL)
                    {
                        WCHAR path[MAX_PATH];
                        if (SHGetPathFromIDListW(pidl, path))
                        {
                            SetDlgItemTextW(hDlg, IDC_TEMP_PATH, path);
                        }
                        CoTaskMemFree(pidl);
                    }
                }
                break;

            case IDOK:
                {
                    WCHAR path[MAX_PATH];
                    GetDlgItemTextW(hDlg, IDC_TEMP_PATH, path, MAX_PATH);
                    g_config.SetTempPath(path);
                    
                    BOOL translated;
                    UINT hours = GetDlgItemInt(hDlg, IDC_DELETE_AFTER, &translated, FALSE);
                    if (translated)
                    {
                        g_config.SetDeleteAfterHours(hours);
                    }
                    
                    bool autoStartup = IsDlgButtonChecked(hDlg, IDC_AUTO_STARTUP) == BST_CHECKED;
                    g_config.SetAutoStartup(autoStartup);
                    AutoStartup::SetAutoStartup(autoStartup, L"ClipboardAsFile", 
                        AutoStartup::GetExecutablePath());
                    
                    bool hotkeyEnabled = IsDlgButtonChecked(hDlg, IDC_HOTKEY_ENABLED) == BST_CHECKED;
                    g_config.SetHotkeyEnabled(hotkeyEnabled);
                    
                    UINT mod = 0;
                    if (IsDlgButtonChecked(hDlg, IDC_HOTKEY_CTRL) == BST_CHECKED)
                        mod |= MOD_CONTROL;
                    if (IsDlgButtonChecked(hDlg, IDC_HOTKEY_ALT) == BST_CHECKED)
                        mod |= MOD_ALT;
                    if (IsDlgButtonChecked(hDlg, IDC_HOTKEY_SHIFT) == BST_CHECKED)
                        mod |= MOD_SHIFT;
                    g_config.SetHotkeyMod(mod);
                    
                    HWND hCombo = GetDlgItem(hDlg, IDC_HOTKEY_KEY);
                    int sel = (int)SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
                    if (sel >= 0)
                    {
                        g_config.SetHotkeyVk('A' + sel);
                    }
                    
                    g_config.Save();
                    
                    UnregisterGlobalHotkey(g_hWnd);
                    RegisterGlobalHotkey(g_hWnd);
                    
                    EndDialog(hDlg, LOWORD(wParam));
                    return (INT_PTR)TRUE;
                }
                break;

            case IDCANCEL:
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void AddTrayIcon(HWND hWnd)
{
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_CLIPBOARDASFILE));
    wcscpy_s(nid.szTip, L"ClipboardAsFile - 剪贴板转文件");
    
    Shell_NotifyIconW(NIM_ADD, &nid);
}

void RemoveTrayIcon()
{
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

void ShowTrayMenu(HWND hWnd)
{
    POINT pt;
    GetCursorPos(&pt);
    
    HMENU hMenu = LoadMenuW(hInst, MAKEINTRESOURCE(200));
    if (hMenu)
    {
        HMENU hSubMenu = GetSubMenu(hMenu, 0);
        if (hSubMenu)
        {
            SetForegroundWindow(hWnd);
            
            TrackPopupMenu(hSubMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
            
            PostMessage(hWnd, WM_NULL, 0, 0);
        }
        DestroyMenu(hMenu);
    }
}

void ConvertClipboard(bool autoPaste)
{
    if (!autoPaste)
    {
        if (ClipboardManager::ConvertTextToFile(g_config.GetTempPath()))
        {
            MessageBoxW(NULL, L"剪贴板文本已转换为文件！", L"成功", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            MessageBoxW(NULL, L"转换失败！请确保剪贴板包含文本内容。", L"错误", MB_OK | MB_ICONERROR);
        }
        return;
    }

    // autoPaste == true: create file from clipboard text without modifying the clipboard, then drop it
    std::wstring filePath = ClipboardManager::CreateFileFromClipboardTextToPath(g_config.GetTempPath());
    if (filePath.empty())
    {
        // nothing to do
        return;
    }

    HWND hwndTarget = GetForegroundWindow();
    if (!hwndTarget)
        return;

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    bool comInited = SUCCEEDED(hr);

    IDataObject* pDataObj = NULL;
    hr = ClipboardManager::CreateDataObjectFromFile(filePath, &pDataObj);
    if (SUCCEEDED(hr) && pDataObj)
    {
        IDropTarget* pDropTarget = NULL;

        typedef HRESULT (WINAPI *PFNSHGetIDropTarget)(HWND, IDropTarget**);
        HMODULE hShell = GetModuleHandleW(L"shell32.dll");
        PFNSHGetIDropTarget pfn = NULL;
        if (hShell)
        {
            pfn = (PFNSHGetIDropTarget)GetProcAddress(hShell, "SHGetIDropTarget");
        }

        if (pfn && SUCCEEDED(pfn(hwndTarget, &pDropTarget)) && pDropTarget)
        {
            POINT ptScreen;
            GetCursorPos(&ptScreen);
            POINT ptClient = ptScreen;
            ScreenToClient(hwndTarget, &ptClient);

            POINTL ptl = { ptClient.x, ptClient.y };
            DWORD dwEffect = DROPEFFECT_COPY;

            pDropTarget->DragEnter(pDataObj, MK_LBUTTON, ptl, &dwEffect);
            pDropTarget->Drop(pDataObj, MK_LBUTTON, ptl, &dwEffect);
            pDropTarget->DragLeave();

            pDropTarget->Release();
            pDropTarget = NULL;
        }
        // If no drop target found, do nothing to avoid changing clipboard

        pDataObj->Release();
        pDataObj = NULL;
    }

    if (comInited)
    {
        CoUninitialize();
    }
}

void RegisterGlobalHotkey(HWND hWnd)
{
    if (g_config.GetHotkeyEnabled())
    {
        RegisterHotKey(hWnd, HOTKEY_ID, g_config.GetHotkeyMod(), g_config.GetHotkeyVk());
    }
}

void UnregisterGlobalHotkey(HWND hWnd)
{
    UnregisterHotKey(hWnd, HOTKEY_ID);
}
