#pragma once
#include <windows.h>
#include <string>
#include <shlobj.h>

class Config
{
private:
    std::wstring m_tempPath;
    int m_deleteAfterHours;
    bool m_autoStartup;
    bool m_hotkeyEnabled;
    UINT m_hotkeyMod;
    UINT m_hotkeyVk;
    
    std::wstring GetDefaultTempPath()
    {
        WCHAR path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path)))
        {
            std::wstring result = path;
            result += L"\\ClipboardAsFile\\Temp";
            return result;
        }
        return L"C:\\Temp\\ClipboardAsFile";
    }

    std::wstring GetConfigPath()
    {
        WCHAR path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path)))
        {
            std::wstring result = path;
            result += L"\\ClipboardAsFile";
            CreateDirectoryW(result.c_str(), NULL);
            result += L"\\config.ini";
            return result;
        }
        return L"";
    }

public:
    Config()
    {
        m_tempPath = GetDefaultTempPath();
        m_deleteAfterHours = 24;
        m_autoStartup = false;
        m_hotkeyEnabled = true;
        m_hotkeyMod = MOD_CONTROL | MOD_ALT;
        m_hotkeyVk = 'V';
        Load();
    }

    void Load()
    {
        std::wstring configPath = GetConfigPath();
        if (configPath.empty()) return;

        WCHAR buffer[MAX_PATH];
        
        GetPrivateProfileStringW(L"Settings", L"TempPath", m_tempPath.c_str(), 
            buffer, MAX_PATH, configPath.c_str());
        m_tempPath = buffer;
        
        m_deleteAfterHours = GetPrivateProfileIntW(L"Settings", L"DeleteAfterHours", 
            24, configPath.c_str());
        
        m_autoStartup = GetPrivateProfileIntW(L"Settings", L"AutoStartup", 
            0, configPath.c_str()) != 0;
        
        m_hotkeyEnabled = GetPrivateProfileIntW(L"Settings", L"HotkeyEnabled", 
            1, configPath.c_str()) != 0;
        
        m_hotkeyMod = GetPrivateProfileIntW(L"Settings", L"HotkeyMod", 
            MOD_CONTROL | MOD_ALT, configPath.c_str());
        
        m_hotkeyVk = GetPrivateProfileIntW(L"Settings", L"HotkeyVk", 
            'V', configPath.c_str());
    }

    void Save()
    {
        std::wstring configPath = GetConfigPath();
        if (configPath.empty()) return;

        WritePrivateProfileStringW(L"Settings", L"TempPath", 
            m_tempPath.c_str(), configPath.c_str());
        
        WritePrivateProfileStringW(L"Settings", L"DeleteAfterHours", 
            std::to_wstring(m_deleteAfterHours).c_str(), configPath.c_str());
        
        WritePrivateProfileStringW(L"Settings", L"AutoStartup", 
            m_autoStartup ? L"1" : L"0", configPath.c_str());
        
        WritePrivateProfileStringW(L"Settings", L"HotkeyEnabled", 
            m_hotkeyEnabled ? L"1" : L"0", configPath.c_str());
        
        WritePrivateProfileStringW(L"Settings", L"HotkeyMod", 
            std::to_wstring(m_hotkeyMod).c_str(), configPath.c_str());
        
        WritePrivateProfileStringW(L"Settings", L"HotkeyVk", 
            std::to_wstring(m_hotkeyVk).c_str(), configPath.c_str());
    }

    const std::wstring& GetTempPath() const { return m_tempPath; }
    void SetTempPath(const std::wstring& path) { m_tempPath = path; }
    
    int GetDeleteAfterHours() const { return m_deleteAfterHours; }
    void SetDeleteAfterHours(int hours) { m_deleteAfterHours = hours; }
    
    bool GetAutoStartup() const { return m_autoStartup; }
    void SetAutoStartup(bool enabled) { m_autoStartup = enabled; }
    
    bool GetHotkeyEnabled() const { return m_hotkeyEnabled; }
    void SetHotkeyEnabled(bool enabled) { m_hotkeyEnabled = enabled; }
    
    UINT GetHotkeyMod() const { return m_hotkeyMod; }
    void SetHotkeyMod(UINT mod) { m_hotkeyMod = mod; }
    
    UINT GetHotkeyVk() const { return m_hotkeyVk; }
    void SetHotkeyVk(UINT vk) { m_hotkeyVk = vk; }
};
