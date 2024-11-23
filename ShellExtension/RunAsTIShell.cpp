// RunAsTIShell.cpp : Implementation of CRunAsTIShell

#include "pch.h"
#include "RunAsTIShell.h"

#include <strsafe.h>

// CRunAsTIShell

CRunAsTIShell::CRunAsTIShell()
{
    m_pIDFolder = NULL;
    ZeroMemory(m_szFile, sizeof(m_szFile));
    m_pDataObj = NULL;
    ZeroMemory(m_pszVerb, sizeof(m_pszVerb));
    ZeroMemory(m_pwszVerb, sizeof(m_pwszVerb));

    m_hShieldBitmap = NULL;
    CreateIconBitmaps();

    ZeroMemory(m_runAsTIPath, sizeof(m_runAsTIPath));
    CreateRunAsTIExecutablePath();
}

CRunAsTIShell::~CRunAsTIShell()
{
    DeleteObject(m_hShieldBitmap);
}

void CRunAsTIShell::CreateIconBitmaps()
{
    SHSTOCKICONINFO ssii = {};
    ssii.cbSize = sizeof(ssii);

    HRESULT hres = SHGetStockIconInfo(SIID_SHIELD, SHGSI_ICON | SHGSI_SMALLICON, &ssii);

    if (hres == S_OK && ssii.hIcon != NULL)
    {
        ICONINFOEX iconInfo = {};
        iconInfo.cbSize = sizeof(iconInfo);
        if (GetIconInfoEx(ssii.hIcon, &iconInfo) && iconInfo.hbmColor != NULL)
        {
            m_hShieldBitmap = (HBITMAP)CopyImage(iconInfo.hbmColor, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
        }
    }

    if (ssii.hIcon != NULL)
    {
        DestroyIcon(ssii.hIcon);
    }
}

void CRunAsTIShell::CreateRunAsTIExecutablePath()
{
    HKEY hKey;
    DWORD dwBufferSize = sizeof(m_runAsTIPath);
    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, L"CLSID\\{3357a0e4-15a5-42fa-b591-10039a6c02a6}\\InprocServer32", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        if (RegQueryValueEx(hKey, L"", 0, NULL, (LPBYTE)m_runAsTIPath, &dwBufferSize) == ERROR_SUCCESS)
        {
            PathRemoveFileSpec(m_runAsTIPath);
            PathAddBackslash(m_runAsTIPath);
            PathAppend(m_runAsTIPath, L"RunAsTI.exe");
        }

        RegCloseKey(hKey);
    }
}

HRESULT CRunAsTIShell::Initialize(LPCITEMIDLIST pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID)
{
    // If Initialize has already been called, release the old PIDL
    ILFree(m_pIDFolder);
    m_pIDFolder = nullptr;

    // Store the new PIDL.
    if (pidlFolder)
    {
        m_pIDFolder = ILClone(pidlFolder);
    }

    // If Initialize has already been called, release the old
    // IDataObject pointer.
    if (m_pDataObj)
    {
        m_pDataObj->Release();
    }

    // If a data object pointer was passed in, save it and
    // extract the file name. 
    if (pdtobj)
    {
        m_pDataObj = pdtobj;
        pdtobj->AddRef();

        STGMEDIUM   medium;
        FORMATETC   fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        UINT        uCount;

        if (SUCCEEDED(m_pDataObj->GetData(&fe, &medium)))
        {
            // Get the count of files dropped.
            uCount = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, NULL, 0);

            // Get the first file name from the CF_HDROP.
            if (uCount)
            {
                DragQueryFile((HDROP)medium.hGlobal, 0, m_szFile, sizeof(m_szFile) / sizeof(TCHAR));
            }

            ReleaseStgMedium(&medium);
        }
    }

    // Duplicate the registry handle. 
    if (hkeyProgID)
    {
        RegOpenKeyEx(hkeyProgID, nullptr, 0L, MAXIMUM_ALLOWED, &m_hRegKey);
    }

    return S_OK;
}

#define IDM_RUNASTI 0

HRESULT CRunAsTIShell::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
    HRESULT hr;

    if (!(CMF_DEFAULTONLY & uFlags))
    {
        InsertMenu(hmenu, indexMenu, MF_STRING | MF_BYPOSITION, idCmdFirst + IDM_RUNASTI, _T("&Run as TrustedInstaller"));

        if (m_hShieldBitmap != nullptr)
        {
            SetMenuItemBitmaps(hmenu, indexMenu, MF_BYPOSITION, m_hShieldBitmap, m_hShieldBitmap);
        }

        hr = StringCbCopyA(m_pszVerb, sizeof(m_pszVerb), "RunAsTI");
        hr = StringCbCopyW(m_pwszVerb, sizeof(m_pwszVerb), L"RunAsTI");

        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(IDM_RUNASTI + 1));
    }

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
}

HRESULT CRunAsTIShell::InvokeCommand(CMINVOKECOMMANDINFO* pici)
{
    BOOL fEx = FALSE;
    BOOL fUnicode = FALSE;

    if (pici->cbSize == sizeof(CMINVOKECOMMANDINFOEX))
    {
        fEx = TRUE;
        if ((pici->fMask & CMIC_MASK_UNICODE))
        {
            fUnicode = TRUE;
        }
    }

    if (!fUnicode && HIWORD(pici->lpVerb))
    {
        if (StrCmpIA(pici->lpVerb, m_pszVerb))
        {
            return E_FAIL;
        }
    }
    else if (fUnicode && HIWORD(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW))
    {
        if (StrCmpIW(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW, m_pwszVerb))
        {
            return E_FAIL;
        }
    }
    else if (LOWORD(pici->lpVerb) != IDM_RUNASTI)
    {
        return E_FAIL;
    }
    else
    {
        if (PathFileExists(m_runAsTIPath))
        {
            PathQuoteSpaces(m_szFile);
            ShellExecute(pici->hwnd, NULL, m_runAsTIPath, m_szFile, NULL, SW_SHOWNORMAL);
        }
        else
        {
            MessageBox(pici->hwnd, L"File not found.", L"RunAsTI", MB_OK | MB_ICONERROR);
        }
    }

    return S_OK;
}

HRESULT CRunAsTIShell::GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax)
{
    HRESULT hr = E_INVALIDARG;

    if (idCmd == IDM_RUNASTI)
    {
        switch (uType)
        {
        case GCS_HELPTEXTW:
            // Only useful for pre-Vista versions of Windows that 
            // have a Status bar.
            hr = StringCchCopyW(reinterpret_cast<PWSTR>(pszName), cchMax, L"Run as TrustedInstaller");
            break;

        case GCS_VERBW:
            // GCS_VERBW is an optional feature that enables a caller
            // to discover the canonical name for the verb that is passed in
            // through idCommand.
            hr = StringCchCopyW(reinterpret_cast<PWSTR>(pszName), cchMax, L"RunAsTI");
            break;
        }
    }
    return hr;
}
