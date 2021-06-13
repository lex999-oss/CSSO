#include <windows.h>
#include <stdio.h>

int main()
{
    
    HKEY hKey;
    RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        TEXT("SOFTWARE\\Tema"),
        0,
        KEY_ALL_ACCESS,
        &hKey
    );
    DWORD nr;
    RegQueryInfoKey(
        hKey,
        NULL,
        NULL,
        NULL,
        &nr,
        NULL,
        NULL,
        NULL,NULL,NULL,NULL,NULL
    );
    printf("%d",nr);
    return 0;
}