#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>

#define BUF_SIZE 8

TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
HANDLE ghMutex;
HANDLE ghWriteEvent;
HANDLE ghReadEvent;

int GenerateRandomNumber(void) {
    return rand();
}

int _tmain() {
    HANDLE hMapFile;
    LPCTSTR pBuf;

    hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,    // use paging file
            NULL,                    // default security
            PAGE_READWRITE,          // read/write access
            0,                       // maximum object size (high-order DWORD)
            BUF_SIZE,                // maximum object size (low-order DWORD)
            szName);                 // name of mapping object

    if (hMapFile == NULL) {
        _tprintf(TEXT("Could not create file mapping object (%lu).\n"),
                 GetLastError());
        return 1;
    }
    pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
                                  FILE_MAP_ALL_ACCESS, // read/write permission
                                  0,
                                  0,
                                  BUF_SIZE);

    if (pBuf == NULL) {
        _tprintf(TEXT("Could not map view of file (%lu).\n"),
                 GetLastError());

        CloseHandle(hMapFile);

        return 1;
    }
    int a, b;
    int count = 200;
    int i = 0;

    TCHAR *str = (TCHAR *) malloc(8 * sizeof(TCHAR));
    memset(str, 0, _tcslen(str) * sizeof(TCHAR));

    srand((time(0)));

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process.
    if (!CreateProcess(TEXT("p2.exe"),   // No module name (use command line)
                       NULL,        // Command line
                       NULL,           // Process handle not inheritable
                       NULL,           // Thread handle not inheritable
                       FALSE,          // Set handle inheritance to FALSE
                       0,              // No creation flags
                       NULL,           // Use parent's environment block
                       NULL,           // Use parent's starting directory
                       &si,            // Pointer to STARTUPINFO structure
                       &pi)           // Pointer to PROCESS_INFORMATION structure
            ) {
        printf("CreateProcess failed (%lu).\n", GetLastError());
    }

    // Solutia cu Mutex

    ghMutex = CreateMutex(
            NULL,              // default security attributes
            TRUE,             // initially not owned
            TEXT("mutex")   // mutex name
    );

    if (ghMutex == NULL) {
        printf("CreateMutex error: %lu\n", GetLastError());
        return 1;
    }

    while (i < count) {
        a = GenerateRandomNumber();
        b = 2 * a;
        CopyMemory((PVOID) pBuf, &a, sizeof(int));
        pBuf += sizeof(int);
        CopyMemory((PVOID) pBuf, &b, sizeof(int));
        pBuf += sizeof(int);
        printf("%d %d\n",a,b);
        ReleaseMutex(ghMutex);
        i++;
    }
    CloseHandle(ghMutex);

    //Solutia cu Events

    ghWriteEvent = CreateEvent(
            NULL,               // default security attributes
            TRUE,               // manual-reset event
            FALSE,              // initial state is nonsignaled
            TEXT("WriteEvent")  // object name
    );

    if (ghWriteEvent == NULL)
    {
        printf("CreateEvent failed (%lu)\n", GetLastError());
        return 1;
    }

    ghReadEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, TEXT("ReadEvent"));

    i = 0;


    while (i < count) {
        a = GenerateRandomNumber();
        b = 2 * a;
        WaitForSingleObject(ghReadEvent, INFINITE);
        CopyMemory((PVOID) pBuf, &a, sizeof(int));
        pBuf += sizeof(int);
        CopyMemory((PVOID) pBuf, &b, sizeof(int));
        pBuf += sizeof(int);
        SetEvent(ghWriteEvent);
        i++;
    }
    CloseHandle(ghReadEvent);
    CloseHandle(ghWriteEvent);

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    UnmapViewOfFile(pBuf);

    CloseHandle(hMapFile);

    return 0;
}
