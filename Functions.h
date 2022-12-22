#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <Wbemidl.h>
#include <tchar.h> 
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")  
#define _WIN32_DCOM
#define MenuUTF16 1
#define MenuUTF8 2
#define BT_SEARCH 3
#define BT_PASTE 4
#define BT_SAVE 5
#define BT_UNSAVE 6
#define CAP_NAME 7
#define CAP_SIZE 8
#define CAP_PATH 9
#define EDIT_PATH 10
#define EDIT_FILE 11
#define list_id 12
#define ClearMenu 13
using namespace std;
LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);
void SetCheck(HWND hWnd);
void MainWndAddWidgets(HWND hWnd);
void LoadData(string path);
void LoadDataUTF8(string path);
void SaveData(string path);
void SaveDataUTF8(string path);
struct InfFiles
{
    string Name;
    string ExecutablePath;
    int Size;
}; 
int SearchFiles(vector<InfFiles>& Files)
{

    HRESULT hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hRes))
    {
        return 1;
    }

    if ((FAILED(hRes = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, 0))))
    {
  
        return 1;
    }

    IWbemLocator* pLocator = NULL;
    if (FAILED(hRes = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pLocator))))
    {
      
        return 1;
    }

    IWbemServices* pService = NULL;
    if (FAILED(hRes = pLocator->ConnectServer(bstr_t("root\\CIMV2"), NULL, NULL, NULL, WBEM_FLAG_CONNECT_USE_MAX_WAIT, NULL, NULL, &pService)))
    {
        pLocator->Release();
        
        return 1;
    }

    IEnumWbemClassObject* pEnumerator = NULL;
    if (FAILED(hRes = pService->ExecQuery(bstr_t("WQL"), bstr_t("SELECT FileName, Name, FileSize FROM CIM_DataFile WHERE Extension = \"inf\" AND FileSize > \"100\""), WBEM_FLAG_FORWARD_ONLY, NULL, &pEnumerator)))
    {
        pLocator->Release();
        pService->Release();
        
        return 1;
    }

    IWbemClassObject* clsObj = NULL;
    int numElems;
    while ((hRes = pEnumerator->Next(WBEM_INFINITE, 1, &clsObj, (ULONG*)&numElems)) != WBEM_S_FALSE)
    {
        if (FAILED(hRes))
            break;
        InfFiles File;
        VARIANT vtProp1;
        VariantInit(&vtProp1);
        clsObj->Get(L"FileName", 0, &vtProp1, 0, 0);
        File.Name = bstr_t(vtProp1.bstrVal);
        VariantClear(&vtProp1);
        clsObj->Get(L"Name", 0, &vtProp1, 0, 0);
        File.ExecutablePath = bstr_t(vtProp1.bstrVal);
        VariantClear(&vtProp1);
        clsObj->Get(L"FileSize", 0, &vtProp1, 0, 0);
        File.Size = atoi(bstr_t(vtProp1.bstrVal));
        VariantClear(&vtProp1);
        Files.push_back(File);
        clsObj->Release();

    }
    pEnumerator->Release();
    pService->Release();
    pLocator->Release();
    return 0;
}
bool Mycompare(InfFiles& e1, InfFiles& e2)
{
    bool a;
    const char* p = e1.Name.c_str();
    const char* p2 = e2.Name.c_str();
    if (_strnicmp(p, p2, 10) < 0)
        a = true;
    else
        a = false;
    return a;
}
int SortFiles(vector<InfFiles>& Files) {
    sort(Files.begin(), Files.end(), Mycompare);
    return 0;
}