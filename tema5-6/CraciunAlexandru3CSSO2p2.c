#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#pragma comment(lib, "user32.lib")

#define BUF_SIZE 8

TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
HANDLE ghMutex;
HANDLE ghWriteEvent;
HANDLE ghReadEvent;

int _tmain()
{
    HANDLE hMapFile;
    LPCTSTR pBuf;

    printf("\t\t\t\t\t\tAl doilea proces: \n\n");


    hMapFile = OpenFileMapping(
            FILE_MAP_ALL_ACCESS,   // read/write access
            FALSE,                 // do not inherit the name
            szName);               // name of mapping object

    if (hMapFile == NULL)
    {
        _tprintf(TEXT("Could not open file mapping object (%lu).\n"),
                 GetLastError());
        return 1;
    }

    pBuf = (LPTSTR) MapViewOfFile(hMapFile, // handle to map object
                                  FILE_MAP_ALL_ACCESS,  // read/write permission
                                  0,
                                  0,
                                  BUF_SIZE);

    if (pBuf == NULL)
    {
        _tprintf(TEXT("Could not map view of file (%lu).\n"),
                 GetLastError());

        CloseHandle(hMapFile);

        return 1;
    }

    int count = 200;
    int i = 0;
    int a,b;
    printf("\t\tSolutia cu mutex: \n\n");
    ghMutex = OpenMutex(SYNCHRONIZE, TRUE, TEXT("mutex"));
    while(i < count)
    {
        WaitForSingleObject(ghMutex, INFINITE);
        CopyMemory((LPVOID)&a, pBuf, sizeof(int));
        pBuf += sizeof(int);
        CopyMemory((LPVOID)&b, pBuf, sizeof(int));
        pBuf += sizeof(int);
        ReleaseMutex(ghMutex);
        if (b == 2*a)
            printf("(%d:%d) valid!\n",a,b);
        else
            printf("(%d:%d) invalid!\n",a,b);
        i++;
    }
    CloseHandle(ghMutex);

    printf("\n");
    printf("\t\tSolutia cu events: \n\n");

    ghReadEvent = CreateEvent(
            NULL,               // default security attributes
            TRUE,               // manual-reset event
            FALSE,              // initial state is nonsignaled
            TEXT("ReadEvent")  // object name
    );

    if (ghReadEvent == NULL)
    {
        printf("CreateEvent failed (%lu)\n", GetLastError());
        return 1;
    }

    ghWriteEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, TEXT("WriteEvent"));

    i = 0;

    while(i < count)
    {
        WaitForSingleObject(ghWriteEvent, INFINITE);
        CopyMemory((LPVOID)&a, pBuf, sizeof(int));
        pBuf += sizeof(int);
        CopyMemory((LPVOID)&b, pBuf, sizeof(int));
        pBuf += sizeof(int);
        SetEvent(ghReadEvent);
        if (b == 2*a)
            printf("(%d:%d) valid!\n",a,b);
        else
            printf("(%d:%d) invalid!\n",a,b);
        i++;
    }
    CloseHandle(ghReadEvent);
    CloseHandle(ghWriteEvent);

    UnmapViewOfFile(pBuf);

    CloseHandle(hMapFile);


    return 0;
}