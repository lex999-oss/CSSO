#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <conio.h>
#include <stdio.h>

#define BUF_SIZE 6000
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");

//  Forward declarations:
void printError(TCHAR *msg);

int main(void)
{
    TCHAR *BUFF = ( TCHAR * ) malloc ( 210 * 1024 * sizeof ( TCHAR ) ) ;
    memset(BUFF, 0, 210 * 1024 * sizeof ( TCHAR ) );
    HANDLE hProcessSnap;
    HANDLE hProcess;
    TCHAR *szMsg = (TCHAR *)malloc(1024 * sizeof(TCHAR));
    memset(szMsg, 0, 1024 * sizeof ( TCHAR ) );
    PROCESSENTRY32 pe32;
    //SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        printError(TEXT("CreateToolhelp32Snapshot (of processes)"));
        return 1;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32))
    {
        printError(TEXT("Process32First")); // show cause of failure
        CloseHandle(hProcessSnap);          // clean the snapshot object
        return 1;
    }

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    
    _stprintf(szMsg, TEXT("%d:%d:%s:"), pe32.th32ProcessID, pe32.th32ParentProcessID, pe32.szExeFile);
    _tcscat(BUFF, szMsg);
    Process32Next(hProcessSnap, &pe32);
    do
    {
        //_tprintf(TEXT("%d:%d:%s\n"), pe32.th32ProcessID, pe32.th32ParentProcessID, pe32.szExeFile);
        _stprintf(szMsg, TEXT("%d:%d:%s:"), pe32.th32ProcessID, pe32.th32ParentProcessID, pe32.szExeFile);
        _tcscat(BUFF, szMsg);
    } while (Process32Next(hProcessSnap, &pe32));

    TCHAR * REDUCED_BUFF = ( TCHAR * ) malloc ( ( _tcslen ( BUFF ) + 1 ) * sizeof (TCHAR) );
    CopyMemory ( REDUCED_BUFF, BUFF, ( _tcslen ( BUFF ) + 1 ) * sizeof (TCHAR) );
    free ( BUFF );

    BUFF = REDUCED_BUFF;
    _tprintf(TEXT("%s"), BUFF);

    CloseHandle(hProcessSnap);
    HANDLE hMapFile;
    LPCTSTR pBuf;

    hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE, // use paging file
        NULL,      //security flag for big page sizes 
        PAGE_READWRITE,       // read/write access
        0,                    // maximum object size (high-order DWORD)
        BUF_SIZE,             // maximum object size (low-order DWORD)
        szName);              // name of mapping object

    if (hMapFile == NULL)
    {
        _tprintf(TEXT("Could not create file mapping object (%d).\n"),
                 GetLastError());
        return 1;
    }
    pBuf = (LPTSTR)MapViewOfFile(hMapFile,            // handle to map object
                                 FILE_MAP_ALL_ACCESS, // read/write permission
                                 0,
                                 0,
                                 BUF_SIZE);

    if (pBuf == NULL)
    {
        _tprintf(TEXT("Could not map view of file (%d).\n"),
                 GetLastError());

        CloseHandle(hMapFile);

        return 1;
    }

    CopyMemory((PVOID)pBuf, BUFF, (_tcslen(BUFF) * sizeof(TCHAR)));

    getchar();

    UnmapViewOfFile(pBuf);

    CloseHandle(hMapFile);

    free(REDUCED_BUFF);
    free(szMsg);
    REDUCED_BUFF = NULL;
    szMsg = NULL;
    
    return 0;
}

void printError(TCHAR *msg)
{
    DWORD eNum;
    TCHAR sysMsg[256];
    TCHAR *p;

    eNum = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL, eNum,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                  sysMsg, 256, NULL);

    // Trim the end of the line and terminate it with a null
    p = sysMsg;
    while ((*p > 31) || (*p == 9))
        ++p;
    do
    {
        *p-- = 0;
    } while ((p >= sysMsg) &&
             ((*p == '.') || (*p < 33)));

    // Display the message
    _tprintf(TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg);
}