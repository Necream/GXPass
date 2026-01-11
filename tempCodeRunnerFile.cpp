#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#endif

std::string getDeviceUniqueID() {
    std::string id;

#ifdef _WIN32
    // Windows: 尝试获取主板序列号
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (SUCCEEDED(hres)) {
        hres = CoInitializeSecurity(NULL, -1, NULL, NULL,
                                    RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
                                    NULL, EOAC_NONE, NULL);
        if (SUCCEEDED(hres)) {
            IWbemLocator *pLoc = nullptr;
            if (SUCCEEDED(CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                                          IID_IWbemLocator, (LPVOID *)&pLoc))) {
                IWbemServices *pSvc = nullptr;
                if (SUCCEEDED(pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"),
                                                  NULL, NULL, 0, NULL, 0, 0, &pSvc))) {
                    CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                                      RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
                                      NULL, EOAC_NONE);

                    IEnumWbemClassObject* pEnumerator = nullptr;
                    if (SUCCEEDED(pSvc->ExecQuery(
                        bstr_t("WQL"),
                        bstr_t("SELECT SerialNumber FROM Win32_BaseBoard"),
                        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                        NULL, &pEnumerator))) {

                        IWbemClassObject *pclsObj = nullptr;
                        ULONG uReturn = 0;
                        if (pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn) == S_OK && uReturn != 0) {
                            VARIANT vtProp;
                            pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
                            id = _bstr_t(vtProp.bstrVal);
                            VariantClear(&vtProp);
                            pclsObj->Release();
                        }
                        pEnumerator->Release();
                    }
                    pSvc->Release();
                }
                pLoc->Release();
            }
        }
        CoUninitialize();
    }

    // 如果没有获取到，则使用C盘卷序列号
    if (id.empty()) {
        DWORD serial = 0;
        if (GetVolumeInformationA("C:\\", NULL, 0, &serial, NULL, NULL, NULL, 0)) {
            id = std::to_string(serial);
        }
    }

#else
    // Linux/macOS: 使用机器 UUID
    std::ifstream file("/etc/machine-id");
    if (file.is_open()) {
        std::getline(file, id);
    }
    if (id.empty()) {
        // 兼容老旧Linux
        file.close();
        file.open("/var/lib/dbus/machine-id");
        if (file.is_open()) std::getline(file, id);
    }
#endif

    if (id.empty()) id = "UNKNOWN_DEVICE";

    return id;
}

int main() {
    std::string deviceID = getDeviceUniqueID();
    std::cout << "Device Unique ID: " << deviceID << std::endl;
}
