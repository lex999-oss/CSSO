// QueryKey - Enumerates the subkeys of key and its associated values.
//     hKey - Key whose subkeys and values are to be enumerated.

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
WCHAR path[1024] = L"C:\\Users\\ALEX\\Desktop\\FII\\CSSO\\tema1-2\\Tema";

void QueryKey(HKEY hKey)
{
    WCHAR achKey[MAX_KEY_LENGTH];        // buffer for subkey name
    DWORD cbName;                        // size of name string
    TCHAR achClass[MAX_PATH] = TEXT(""); // buffer for class name
    DWORD cchClassName = MAX_PATH;       // size of class string
    DWORD cSubKeys = 0;                  // number of subkeys
    DWORD cbMaxSubKey;                   // longest subkey size
    DWORD cchMaxClass;                   // longest class string
    DWORD cValues;                       // number of values for key
    DWORD cchMaxValue;                   // longest value name
    DWORD cbMaxValueData;                // longest value data
    DWORD cbSecurityDescriptor;          // size of security descriptor
    FILETIME ftLastWriteTime;            // last write time

    DWORD i, retCode;

    WCHAR achValue[MAX_VALUE_NAME];
    DWORD cchValue = MAX_VALUE_NAME;
    WCHAR xpath[1024];
    WCHAR fpath[1024];
    // Get the class name and the value count.
    retCode = RegQueryInfoKey(
        hKey,                  // key handle
        achClass,              // buffer for class name
        &cchClassName,         // size of class string
        NULL,                  // reserved
        &cSubKeys,             // number of subkeys
        &cbMaxSubKey,          // longest subkey size
        &cchMaxClass,          // longest class string
        &cValues,              // number of values for this key
        &cchMaxValue,          // longest value name
        &cbMaxValueData,       // longest value data
        &cbSecurityDescriptor, // security descriptor
        &ftLastWriteTime);     // last write time

    // Enumerate the key values.

    if (cValues)
    {
        printf("\nNumber of values: %d\n", cValues);
        retCode = ERROR_SUCCESS;
        for (i = 0; i < cValues; i++)
        {
            retCode = RegEnumValueW(hKey, i,
                                    achValue,
                                    &cchValue,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL);

            if (retCode == ERROR_SUCCESS)
            {
                wcscpy(fpath, path);
                wcscat(fpath, L"\\");
                wcscat(fpath, achValue);
                HANDLE hFile;
                hFile = CreateFileW(fpath,
                                    GENERIC_ALL,
                                    FILE_SHARE_READ,
                                    NULL,
                                    CREATE_ALWAYS,
                                    FILE_ATTRIBUTE_ARCHIVE | SECURITY_IMPERSONATION,
                                    NULL);
                if (hFile == INVALID_HANDLE_VALUE)

                    printf("\nCould not open %S file, error %d)\n", achKey, GetLastError());

                else

                {

                    printf("\n%S file HANDLE is OK!\n", achKey);

                    printf("\n%S opened successfully!\n", achKey);
                }
            }
        }
    }

    // Enumerate the subkeys, until RegEnumKeyEx fails.

    if (cSubKeys)
    {
        printf("\nNumber of subkeys: %d\n", cSubKeys);

        for (i = 0; i < cSubKeys; i++)
        {
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyExW(hKey, i,
                                    achKey,
                                    &cbName,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &ftLastWriteTime);
            if (retCode == ERROR_SUCCESS)
            {
                HKEY subKey;
                retCode = RegOpenKeyExW(hKey,
                                        achKey,
                                        0, KEY_READ,
                                        &subKey);
                //error checking skipped for illustrative purposes
                wcscpy(xpath, path);
                wcscat(xpath, L"\\");
                wcscat(xpath, achKey);
                CreateDirectoryW(xpath, NULL);
                wcscpy(path, xpath);
                QueryKey(subKey);
            }
        }
    }
}

int __cdecl _tmain()
{
    HKEY hTestKey;

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     TEXT("SOFTWARE\\Tema"),
                     0,
                     KEY_READ,
                     &hTestKey) == ERROR_SUCCESS)
    {
        QueryKey(hTestKey);
    }

    RegCloseKey(hTestKey);
}