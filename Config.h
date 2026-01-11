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
    bool m_hotkey1Enabled;
    UINT m_hotkey1Mod;
    UINT m_hotkey1Vk;
    bool m_hotkey2Enabled;
    UINT m_hotkey2Mod;
    UINT m_hotkey2Vk;
    
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
        m_hotkey1Enabled = true;
        m_hotkey1Mod = MOD_CONTROL | MOD_ALT;
        m_hotkey1Vk = 'V';
        m_hotkey2Enabled = true;
        m_hotkey2Mod = MOD_CONTROL | MOD_SHIFT;
        m_hotkey2Vk = 'V';
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
        
        m_hotkey1Enabled = GetPrivateProfileIntW(L"Settings", L"Hotkey1Enabled", 
            1, configPath.c_str()) != 0;
        
        m_hotkey1Mod = GetPrivateProfileIntW(L"Settings", L"Hotkey1Mod", 
            MOD_CONTROL | MOD_ALT, configPath.c_str());
        
        m_hotkey1Vk = GetPrivateProfileIntW(L"Settings", L"Hotkey1Vk", 
            'V', configPath.c_str());
        
        m_hotkey2Enabled = GetPrivateProfileIntW(L"Settings", L"Hotkey2Enabled", 
            1, configPath.c_str()) != 0;
        
        m_hotkey2Mod = GetPrivateProfileIntW(L"Settings", L"Hotkey2Mod", 
            MOD_CONTROL | MOD_SHIFT, configPath.c_str());
        
        m_hotkey2Vk = GetPrivateProfileIntW(L"Settings", L"Hotkey2Vk", 
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
        
        WritePrivateProfileStringW(L"Settings", L"Hotkey1Enabled", 
            m_hotkey1Enabled ? L"1" : L"0", configPath.c_str());
        
        WritePrivateProfileStringW(L"Settings", L"Hotkey1Mod", 
            std::to_wstring(m_hotkey1Mod).c_str(), configPath.c_str());
        
        WritePrivateProfileStringW(L"Settings", L"Hotkey1Vk", 
            std::to_wstring(m_hotkey1Vk).c_str(), configPath.c_str());
        
        WritePrivateProfileStringW(L"Settings", L"Hotkey2Enabled", 
            m_hotkey2Enabled ? L"1" : L"0", configPath.c_str());
        
        WritePrivateProfileStringW(L"Settings", L"Hotkey2Mod", 
            std::to_wstring(m_hotkey2Mod).c_str(), configPath.c_str());
        
        WritePrivateProfileStringW(L"Settings", L"Hotkey2Vk", 
            std::to_wstring(m_hotkey2Vk).c_str(), configPath.c_str());
    }

    const std::wstring& GetTempPath() const { return m_tempPath; }
    void SetTempPath(const std::wstring& path) { m_tempPath = path; }
    
    int GetDeleteAfterHours() const { return m_deleteAfterHours; }
    void SetDeleteAfterHours(int hours) { m_deleteAfterHours = hours; }
    
    bool GetAutoStartup() const { return m_autoStartup; }
    void SetAutoStartup(bool enabled) { m_autoStartup = enabled; }
    
    bool GetHotkey1Enabled() const { return m_hotkey1Enabled; }
    void SetHotkey1Enabled(bool enabled) { m_hotkey1Enabled = enabled; }
    
    UINT GetHotkey1Mod() const { return m_hotkey1Mod; }
    void SetHotkey1Mod(UINT mod) { m_hotkey1Mod = mod; }
    
    UINT GetHotkey1Vk() const { return m_hotkey1Vk; }
    void SetHotkey1Vk(UINT vk) { m_hotkey1Vk = vk; }
    
    bool GetHotkey2Enabled() const { return m_hotkey2Enabled; }
    void SetHotkey2Enabled(bool enabled) { m_hotkey2Enabled = enabled; }
    
    UINT GetHotkey2Mod() const { return m_hotkey2Mod; }
    void SetHotkey2Mod(UINT mod) { m_hotkey2Mod = mod; }
    
    UINT GetHotkey2Vk() const { return m_hotkey2Vk; }
    void SetHotkey2Vk(UINT vk) { m_hotkey2Vk = vk; }
};
