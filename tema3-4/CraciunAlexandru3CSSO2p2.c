#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#pragma comment(lib, "user32.lib")

#define BUF_SIZE 6000
#define MAX_NR_OF_PROCESSES 210
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");

typedef struct Process
{
    DWORD PID;
    DWORD PPID;
    TCHAR processName[80];
} Process;

Process GetProcessByPID(int _PID, Process *array)
{
    for (int i = 0; i < MAX_NR_OF_PROCESSES; i++)
    {
        if (array[i].PID == _PID)
            return array[i];
    }
}

int _tmain()
{
    HANDLE hMapFile;
    LPCSTR pBuf;

    hMapFile = OpenFileMappingA(
        FILE_MAP_ALL_ACCESS, // read/write access
        FALSE,               // do not inherit the name
        szName);             // name of mapping object

    if (hMapFile == NULL)
    {
        _tprintf(TEXT("Could not open file mapping object (%d).\n"),
                 GetLastError());
        return 1;
    }

    pBuf = (LPCSTR)MapViewOfFile(hMapFile,            // handle to map object
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
    TCHAR *BUFF = (TCHAR *)malloc(_tcslen(pBuf) * sizeof(TCHAR));
    memcpy(BUFF, pBuf, _tcslen(pBuf) * sizeof(TCHAR));

    //_tprintf(TEXT("%s"), BUFF);

    UnmapViewOfFile(pBuf);

    CloseHandle(hMapFile);

    DWORD processArrayLen = 0;
    DWORD processesWithoutParentsCount = 0;
    Process processArray[MAX_NR_OF_PROCESSES];
    TCHAR *token = _tcstok(BUFF, TEXT(":"));
    int i = 0;
    while (token != NULL)
    {
        if (token != NULL)
            processArray[i].PID = _ttoi(token);
        token = _tcstok(NULL, TEXT(":"));
        if (token != NULL)
            processArray[i].PPID = _ttoi(token);
        token = _tcstok(NULL, TEXT(":"));
        if (token != NULL)
            _tcscpy(processArray[i].processName, token);
        i++;
        token = _tcstok(NULL, TEXT(":"));
    }

    for (i = 0; i < MAX_NR_OF_PROCESSES; i++)
    {
        int IndentLevel = 0;
        if (processArray[i].PPID == 0)
        {
            _tprintf(TEXT("PID:%d NAME:%s\n"), processArray[i].PID, processArray[i].processName);
        }
        else
        {
            if (processArray[i].PPID != 0)
            {
                for (int x = 0; x < MAX_NR_OF_PROCESSES; x++)
                    if (processArray[i].PPID == processArray[x].PID)
                        IndentLevel++;
            }
            while (IndentLevel >= 0)
            {
                _tprintf(TEXT("\t"));
                IndentLevel--;
            }
            _tprintf(TEXT("PID:%d NAME:%s\n"), processArray[i].PID, processArray[i].processName);
        }
    }

    return 0;
}