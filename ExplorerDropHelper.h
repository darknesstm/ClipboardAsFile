#pragma once
#include <windows.h>
#include <shlobj.h>
#include <oleidl.h>
#include <exdisp.h>
#include <string>

#pragma comment(lib, "ole32.lib")

class ExplorerDropHelper
{
public:
    // Try to drop file to active Explorer window using IShellView
    static bool DropToExplorer(HWND hwndTarget, IDataObject* pDataObj)
    {
        if (!hwndTarget || !pDataObj)
            return false;

        // Try to get IShellBrowser from the window
        IShellBrowser* pBrowser = NULL;
        if (SUCCEEDED(GetShellBrowserFromWindow(hwndTarget, &pBrowser)) && pBrowser)
        {
            IShellView* pView = NULL;
            if (SUCCEEDED(pBrowser->QueryActiveShellView(&pView)) && pView)
            {
                IDropTarget* pDropTarget = NULL;
                if (SUCCEEDED(pView->QueryInterface(IID_IDropTarget, (void**)&pDropTarget)) && pDropTarget)
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
                    pView->Release();
                    pBrowser->Release();
                    return true;
                }
                pView->Release();
            }
            pBrowser->Release();
        }

        return false;
    }

private:
    static HRESULT GetShellBrowserFromWindow(HWND hwnd, IShellBrowser** ppBrowser)
    {
        if (!ppBrowser)
            return E_POINTER;
        *ppBrowser = NULL;

        // Try IShellWindows to enumerate shell windows
        IShellWindows* pShellWindows = NULL;
        HRESULT hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL,
            IID_IShellWindows, (void**)&pShellWindows);
        if (FAILED(hr) || !pShellWindows)
            return hr;

        long count = 0;
        pShellWindows->get_Count(&count);

        for (long i = 0; i < count; i++)
        {
            VARIANT vi;
            VariantInit(&vi);
            vi.vt = VT_I4;
            vi.lVal = i;

            IDispatch* pDisp = NULL;
            if (SUCCEEDED(pShellWindows->Item(vi, &pDisp)) && pDisp)
            {
                IWebBrowserApp* pApp = NULL;
                if (SUCCEEDED(pDisp->QueryInterface(IID_IWebBrowserApp, (void**)&pApp)) && pApp)
                {
                    HWND hwndShell = NULL;
                    if (SUCCEEDED(pApp->get_HWND((SHANDLE_PTR*)&hwndShell)) && hwndShell == hwnd)
                    {
                        IServiceProvider* pSP = NULL;
                        if (SUCCEEDED(pApp->QueryInterface(IID_IServiceProvider, (void**)&pSP)) && pSP)
                        {
                            hr = pSP->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)ppBrowser);
                            pSP->Release();
                            pApp->Release();
                            pDisp->Release();
                            pShellWindows->Release();
                            return hr;
                        }
                    }
                    pApp->Release();
                }
                pDisp->Release();
            }
        }

        pShellWindows->Release();
        return E_FAIL;
    }
};
