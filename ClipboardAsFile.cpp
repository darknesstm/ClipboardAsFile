// ClipboardAsFile.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ClipboardAsFile.h"
#include "Config.h"
#include "ClipboardManager.h"
#include "AutoStartup.h"
#include "ExplorerDropHelper.h"
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
#define HOTKEY_ID_1 1
#define HOTKEY_ID_2 2

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
void                ShowBalloonNotification(const WCHAR* title, const WCHAR* message, DWORD iconType);
void                ConvertToClipboard();
void                DropToActiveWindow();
void                RegisterGlobalHotkeys(HWND hWnd);
void                UnregisterGlobalHotkeys(HWND hWnd);

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
   RegisterGlobalHotkeys(hWnd);

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
                ConvertToClipboard();
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
        if (wParam == HOTKEY_ID_1)
        {
            ConvertToClipboard();
        }
        else if (wParam == HOTKEY_ID_2)
        {
            DropToActiveWindow();
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
        UnregisterGlobalHotkeys(hWnd);
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
            
            // Hotkey 1
            CheckDlgButton(hDlg, IDC_HOTKEY1_ENABLED, 
                g_config.GetHotkey1Enabled() ? BST_CHECKED : BST_UNCHECKED);
            
            UINT mod1 = g_config.GetHotkey1Mod();
            CheckDlgButton(hDlg, IDC_HOTKEY1_CTRL, (mod1 & MOD_CONTROL) ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_HOTKEY1_ALT, (mod1 & MOD_ALT) ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_HOTKEY1_SHIFT, (mod1 & MOD_SHIFT) ? BST_CHECKED : BST_UNCHECKED);
            
            HWND hCombo1 = GetDlgItem(hDlg, IDC_HOTKEY1_KEY);
            const WCHAR* keys[] = {L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", 
                                   L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", 
                                   L"U", L"V", L"W", L"X", L"Y", L"Z"};
            for (int i = 0; i < 26; i++)
            {
                SendMessageW(hCombo1, CB_ADDSTRING, 0, (LPARAM)keys[i]);
            }
            SendMessageW(hCombo1, CB_SETCURSEL, g_config.GetHotkey1Vk() - 'A', 0);
            
            // Hotkey 2
            CheckDlgButton(hDlg, IDC_HOTKEY2_ENABLED, 
                g_config.GetHotkey2Enabled() ? BST_CHECKED : BST_UNCHECKED);
            
            UINT mod2 = g_config.GetHotkey2Mod();
            CheckDlgButton(hDlg, IDC_HOTKEY2_CTRL, (mod2 & MOD_CONTROL) ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_HOTKEY2_ALT, (mod2 & MOD_ALT) ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_HOTKEY2_SHIFT, (mod2 & MOD_SHIFT) ? BST_CHECKED : BST_UNCHECKED);
            
            HWND hCombo2 = GetDlgItem(hDlg, IDC_HOTKEY2_KEY);
            for (int i = 0; i < 26; i++)
            {
                SendMessageW(hCombo2, CB_ADDSTRING, 0, (LPARAM)keys[i]);
            }
            SendMessageW(hCombo2, CB_SETCURSEL, g_config.GetHotkey2Vk() - 'A', 0);
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
                    
                    // Hotkey 1
                    bool hotkey1Enabled = IsDlgButtonChecked(hDlg, IDC_HOTKEY1_ENABLED) == BST_CHECKED;
                    g_config.SetHotkey1Enabled(hotkey1Enabled);
                    
                    UINT mod1 = 0;
                    if (IsDlgButtonChecked(hDlg, IDC_HOTKEY1_CTRL) == BST_CHECKED)
                        mod1 |= MOD_CONTROL;
                    if (IsDlgButtonChecked(hDlg, IDC_HOTKEY1_ALT) == BST_CHECKED)
                        mod1 |= MOD_ALT;
                    if (IsDlgButtonChecked(hDlg, IDC_HOTKEY1_SHIFT) == BST_CHECKED)
                        mod1 |= MOD_SHIFT;
                    g_config.SetHotkey1Mod(mod1);
                    
                    HWND hCombo1 = GetDlgItem(hDlg, IDC_HOTKEY1_KEY);
                    int sel1 = (int)SendMessageW(hCombo1, CB_GETCURSEL, 0, 0);
                    if (sel1 >= 0)
                    {
                        g_config.SetHotkey1Vk('A' + sel1);
                    }
                    
                    // Hotkey 2
                    bool hotkey2Enabled = IsDlgButtonChecked(hDlg, IDC_HOTKEY2_ENABLED) == BST_CHECKED;
                    g_config.SetHotkey2Enabled(hotkey2Enabled);
                    
                    UINT mod2 = 0;
                    if (IsDlgButtonChecked(hDlg, IDC_HOTKEY2_CTRL) == BST_CHECKED)
                        mod2 |= MOD_CONTROL;
                    if (IsDlgButtonChecked(hDlg, IDC_HOTKEY2_ALT) == BST_CHECKED)
                        mod2 |= MOD_ALT;
                    if (IsDlgButtonChecked(hDlg, IDC_HOTKEY2_SHIFT) == BST_CHECKED)
                        mod2 |= MOD_SHIFT;
                    g_config.SetHotkey2Mod(mod2);
                    
                    HWND hCombo2 = GetDlgItem(hDlg, IDC_HOTKEY2_KEY);
                    int sel2 = (int)SendMessageW(hCombo2, CB_GETCURSEL, 0, 0);
                    if (sel2 >= 0)
                    {
                        g_config.SetHotkey2Vk('A' + sel2);
                    }
                    
                    g_config.Save();
                    
                    UnregisterGlobalHotkeys(g_hWnd);
                    RegisterGlobalHotkeys(g_hWnd);
                    
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
    
    // Enable balloon notifications by updating flags
    nid.uFlags |= NIF_INFO;
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

void ShowBalloonNotification(const WCHAR* title, const WCHAR* message, DWORD iconType)
{
    nid.uFlags = NIF_INFO;
    nid.dwInfoFlags = iconType;
    wcscpy_s(nid.szInfoTitle, title);
    wcscpy_s(nid.szInfo, message);
    
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

void ConvertToClipboard()
{
    if (ClipboardManager::ConvertTextToFile(g_config.GetTempPath()))
    {
        ShowBalloonNotification(L"转换成功", L"剪贴板文本已转换为文件", NIIF_INFO);
    }
    else
    {
        ShowBalloonNotification(L"转换失败", L"请确保剪贴板包含文本内容", NIIF_ERROR);
    }
}

void DropToActiveWindow()
{
    // Create file from clipboard text without modifying clipboard, then drop to active window
    std::wstring filePath = ClipboardManager::CreateFileFromClipboardTextToPath(g_config.GetTempPath());
    if (filePath.empty())
    {
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
        bool dropped = false;

        // Strategy 1: Try Explorer-specific drop using IShellView
        if (ExplorerDropHelper::DropToExplorer(hwndTarget, pDataObj))
        {
            dropped = true;
        }
        else
        {
            // Strategy 2: Try generic drop using SHGetIDropTarget
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
                dropped = true;
            }
        }

        pDataObj->Release();
    }

    if (comInited)
    {
        CoUninitialize();
    }
}

void RegisterGlobalHotkeys(HWND hWnd)
{
    if (g_config.GetHotkey1Enabled())
    {
        RegisterHotKey(hWnd, HOTKEY_ID_1, g_config.GetHotkey1Mod(), g_config.GetHotkey1Vk());
    }
    if (g_config.GetHotkey2Enabled())
    {
        RegisterHotKey(hWnd, HOTKEY_ID_2, g_config.GetHotkey2Mod(), g_config.GetHotkey2Vk());
    }
}

void UnregisterGlobalHotkeys(HWND hWnd)
{
    UnregisterHotKey(hWnd, HOTKEY_ID_1);
    UnregisterHotKey(hWnd, HOTKEY_ID_2);
}
