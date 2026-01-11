#pragma once
#include <windows.h>
#include <string>

class AutoStartup
{
public:
    static bool SetAutoStartup(bool enable, const std::wstring& appName, const std::wstring& appPath)
    {
        HKEY hKey;
        LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, 
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
            0, KEY_SET_VALUE, &hKey);
        
        if (result != ERROR_SUCCESS)
            return false;
        
        if (enable)
        {
            std::wstring quotedPath = L"\"" + appPath + L"\"";
            result = RegSetValueExW(hKey, appName.c_str(), 0, REG_SZ, 
                reinterpret_cast<const BYTE*>(quotedPath.c_str()), 
                static_cast<DWORD>((quotedPath.length() + 1) * sizeof(WCHAR)));
        }
        else
        {
            result = RegDeleteValueW(hKey, appName.c_str());
            if (result == ERROR_FILE_NOT_FOUND)
                result = ERROR_SUCCESS;
        }
        
        RegCloseKey(hKey);
        return result == ERROR_SUCCESS;
    }

    static bool IsAutoStartupEnabled(const std::wstring& appName)
    {
        HKEY hKey;
        LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, 
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
            0, KEY_QUERY_VALUE, &hKey);
        
        if (result != ERROR_SUCCESS)
            return false;
        
        WCHAR buffer[MAX_PATH];
        DWORD bufferSize = sizeof(buffer);
        result = RegQueryValueExW(hKey, appName.c_str(), NULL, NULL, 
            reinterpret_cast<BYTE*>(buffer), &bufferSize);
        
        RegCloseKey(hKey);
        return result == ERROR_SUCCESS;
    }

    static std::wstring GetExecutablePath()
    {
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);
        return path;
    }
};
