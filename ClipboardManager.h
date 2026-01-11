#pragma once
#include <windows.h>
#include <string>
#include <shlwapi.h>
#include <fstream>
#include <vector>
#include <oleidl.h>

#pragma comment(lib, "shlwapi.lib")

class ClipboardManager
{
public:
    // Creates a file from current clipboard Unicode text and returns the file path (empty on failure).
    static std::wstring CreateFileFromClipboardTextToPath(const std::wstring& tempPath)
    {
        if (!OpenClipboard(NULL))
            return std::wstring();

        std::wstring result;
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        
        if (hData != NULL)
        {
            WCHAR* pszText = static_cast<WCHAR*>(GlobalLock(hData));
            if (pszText != NULL)
            {
                std::wstring text(pszText);
                GlobalUnlock(hData);
                
                if (!text.empty())
                {
                    result = CreateFileFromTextInternal(text, tempPath);
                }
            }
        }
        
        CloseClipboard();
        return result;
    }

    // Original behavior: create file and put CF_HDROP into clipboard
    static bool ConvertTextToFile(const std::wstring& tempPath)
    {
        std::wstring filePath = CreateFileFromClipboardTextToPath(tempPath);
        if (filePath.empty())
            return false;

        return SetClipboardHdropFromFile(filePath);
    }

private:
    static std::string WideStringToUtf8(const std::wstring& wstr)
    {
        if (wstr.empty()) return std::string();
        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
        if (sizeNeeded <= 0) return std::string();
        std::string result(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &result[0], sizeNeeded, NULL, NULL);
        return result;
    }

    static std::wstring CreateFileFromTextInternal(const std::wstring& text, const std::wstring& tempPath)
    {
        CreateDirectoryW(tempPath.c_str(), NULL);
        
        SYSTEMTIME st;
        GetLocalTime(&st);
        
        WCHAR fileName[MAX_PATH];
        swprintf_s(fileName, MAX_PATH, L"%s\\clipboard_%04d%02d%02d_%02d%02d%02d.txt",
            tempPath.c_str(), st.wYear, st.wMonth, st.wDay, 
            st.wHour, st.wMinute, st.wSecond);
        
        std::string utf8 = WideStringToUtf8(text);
        std::ofstream file(fileName, std::ios::binary);
        if (!file.is_open())
            return std::wstring();
        
        if (!utf8.empty())
            file.write(utf8.data(), utf8.size());
        file.close();
        
        return std::wstring(fileName);
    }

    static bool SetClipboardHdropFromFile(const std::wstring& filePath)
    {
        if (!OpenClipboard(NULL))
            return false;

        EmptyClipboard();

        size_t pathLen = filePath.length();
        size_t dataSize = sizeof(DROPFILES) + (pathLen + 2) * sizeof(WCHAR);
        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dataSize);
        if (!hGlobal)
        {
            CloseClipboard();
            return false;
        }

        DROPFILES* pDrop = (DROPFILES*)GlobalLock(hGlobal);
        if (!pDrop)
        {
            GlobalFree(hGlobal);
            CloseClipboard();
            return false;
        }

        pDrop->pFiles = sizeof(DROPFILES);
        pDrop->pt.x = 0;
        pDrop->pt.y = 0;
        pDrop->fNC = FALSE;
        pDrop->fWide = TRUE;

        WCHAR* psz = (WCHAR*)((BYTE*)pDrop + sizeof(DROPFILES));
        wcscpy_s(psz, pathLen + 2, filePath.c_str());
        psz[pathLen + 1] = 0;

        GlobalUnlock(hGlobal);

        if (SetClipboardData(CF_HDROP, hGlobal) == NULL)
        {
            GlobalFree(hGlobal);
            CloseClipboard();
            return false;
        }

        CloseClipboard();
        return true;
    }

public:
    // Create an IDataObject representing the file (CF_HDROP) without touching the clipboard
    static HRESULT CreateDataObjectFromFile(const std::wstring& filePath, IDataObject** ppData)
    {
        if (ppData == NULL) return E_POINTER;
        *ppData = NULL;

        // Prepare DROPFILES structure with wide chars
        SIZE_T pathLen = filePath.length();
        SIZE_T bytes = sizeof(DROPFILES) + (pathLen + 2) * sizeof(WCHAR);
        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (!hGlobal) return E_OUTOFMEMORY;

        DROPFILES* pDrop = (DROPFILES*)GlobalLock(hGlobal);
        if (!pDrop)
        {
            GlobalFree(hGlobal);
            return E_OUTOFMEMORY;
        }

        pDrop->pFiles = sizeof(DROPFILES);
        pDrop->pt.x = 0;
        pDrop->pt.y = 0;
        pDrop->fNC = FALSE;
        pDrop->fWide = TRUE;

        WCHAR* psz = (WCHAR*)((BYTE*)pDrop + sizeof(DROPFILES));
        wcscpy_s(psz, pathLen + 2, filePath.c_str());
        psz[pathLen + 1] = 0;

        GlobalUnlock(hGlobal);

        // Implement a simple IDataObject that returns this HGLOBAL for CF_HDROP
        class FileDataObject : public IDataObject
        {
        public:
            FileDataObject(HGLOBAL h) : m_ref(1), m_hGlobal(h) {}
            ~FileDataObject() { if (m_hGlobal) GlobalFree(m_hGlobal); }

            // IUnknown
            STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
            {
                if (!ppvObject) return E_POINTER;
                if (riid == IID_IUnknown || riid == IID_IDataObject)
                {
                    *ppvObject = static_cast<IDataObject*>(this);
                    AddRef();
                    return S_OK;
                }
                *ppvObject = NULL;
                return E_NOINTERFACE;
            }
            STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_ref); }
            STDMETHODIMP_(ULONG) Release() { ULONG c = InterlockedDecrement(&m_ref); if (c == 0) delete this; return c; }

            // IDataObject
            STDMETHODIMP GetData(FORMATETC* pFmtEtc, STGMEDIUM* pMedium)
            {
                if (!pFmtEtc || !pMedium) return E_POINTER;
                if (pFmtEtc->cfFormat == CF_HDROP && (pFmtEtc->tymed & TYMED_HGLOBAL))
                {
                    // Provide a copy of the global memory
                    SIZE_T size = GlobalSize(m_hGlobal);
                    HGLOBAL hCopy = GlobalAlloc(GMEM_MOVEABLE, size);
                    if (!hCopy) return E_OUTOFMEMORY;
                    void* src = GlobalLock(m_hGlobal);
                    void* dst = GlobalLock(hCopy);
                    memcpy(dst, src, size);
                    GlobalUnlock(hCopy);
                    GlobalUnlock(m_hGlobal);

                    pMedium->tymed = TYMED_HGLOBAL;
                    pMedium->hGlobal = hCopy;
                    pMedium->pUnkForRelease = NULL;
                    return S_OK;
                }
                return DV_E_FORMATETC;
            }
            STDMETHODIMP GetDataHere(FORMATETC* /*pFmtEtc*/, STGMEDIUM* /*pMedium*/) { return E_NOTIMPL; }
            STDMETHODIMP QueryGetData(FORMATETC* pFmtEtc)
            {
                if (!pFmtEtc) return E_POINTER;
                if (pFmtEtc->cfFormat == CF_HDROP && (pFmtEtc->tymed & TYMED_HGLOBAL))
                    return S_OK;
                return DV_E_FORMATETC;
            }
            STDMETHODIMP GetCanonicalFormatEtc(FORMATETC* /*pIn*/, FORMATETC* pOut) { if (pOut) pOut->ptd = NULL; return E_NOTIMPL; }
            STDMETHODIMP SetData(FORMATETC* /*pFmtEtc*/, STGMEDIUM* /*pMedium*/, BOOL /*fRelease*/) { return E_NOTIMPL; }
            STDMETHODIMP EnumFormatEtc(DWORD /*dwDirection*/, IEnumFORMATETC** /*ppEnumFormatEtc*/) { return E_NOTIMPL; }
            STDMETHODIMP DAdvise(FORMATETC* /*pFormatetc*/, DWORD /*advf*/, IAdviseSink* /*pAdvSink*/, DWORD* /*pdwConnection*/) { return OLE_E_ADVISENOTSUPPORTED; }
            STDMETHODIMP DUnadvise(DWORD /*dwConnection*/) { return E_NOTIMPL; }
            STDMETHODIMP EnumDAdvise(IEnumSTATDATA** /*ppEnumAdvise*/) { return E_NOTIMPL; }

        private:
            volatile LONG m_ref;
            HGLOBAL m_hGlobal; // owned
        };

        // Instantiate and return
        FileDataObject* pObj = new FileDataObject(hGlobal);
        if (!pObj)
        {
            GlobalFree(hGlobal);
            return E_OUTOFMEMORY;
        }

        *ppData = pObj;
        return S_OK;
    }

    // existing cleanup function unchanged
    static void CleanupOldFiles(const std::wstring& tempPath, int hoursOld)
    {
        if (hoursOld <= 0) return;
        
        WIN32_FIND_DATAW findData;
        std::wstring searchPath = tempPath + L"\\*.*";
        HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);
        
        if (hFind == INVALID_HANDLE_VALUE)
            return;
        
        FILETIME currentTime;
        GetSystemTimeAsFileTime(&currentTime);
        ULARGE_INTEGER currentTime64;
        currentTime64.LowPart = currentTime.dwLowDateTime;
        currentTime64.HighPart = currentTime.dwHighDateTime;
        
        ULONGLONG deleteThreshold = hoursOld * 36000000000ULL;
        
        do
        {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                ULARGE_INTEGER fileTime64;
                fileTime64.LowPart = findData.ftLastWriteTime.dwLowDateTime;
                fileTime64.HighPart = findData.ftLastWriteTime.dwHighDateTime;
                
                if (currentTime64.QuadPart - fileTime64.QuadPart > deleteThreshold)
                {
                    std::wstring filePath = tempPath + L"\\" + findData.cFileName;
                    DeleteFileW(filePath.c_str());
                }
            }
        } while (FindNextFileW(hFind, &findData));
        
        FindClose(hFind);
    }
};
