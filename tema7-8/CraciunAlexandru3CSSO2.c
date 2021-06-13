#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#include <tchar.h>

#define BUFF_SIZE 256
#define MAX_NO_OF_LINES 10 //4 for config data and 6 for actual commands
#define MAX_NO_OF_CMDS 6

int main() {
    HINTERNET InternetHandler;
    if (NULL == (InternetHandler = InternetOpen(
            "tema7_8-app",
            INTERNET_OPEN_TYPE_PRECONFIG,
            NULL,
            NULL,
            0
    ))) {
        printf("Error initializing Internet handler!\n");
        return 0;
    }

    HINTERNET ConnectionHandler;
    if (NULL == (ConnectionHandler = InternetOpenUrl(
            InternetHandler,
            "http://students.info.uaic.ro/~alexandru.craciun/csso/file.txt",
            NULL,
            0,
            INTERNET_FLAG_DONT_CACHE,
            0
    ))) {
        printf("Error reaching URL!\n");
        return 0;
    }

    char holdBuff[BUFF_SIZE];
    char *temp = holdBuff;
    DWORD dNoOfBytesRead = 0;
    while (InternetReadFile(ConnectionHandler, temp, 1, &dNoOfBytesRead) == TRUE && dNoOfBytesRead > 0) {
        temp += dNoOfBytesRead;
    }
    *temp = '\0';

    char *lines[MAX_NO_OF_LINES];
    char *cmds[MAX_NO_OF_CMDS];
    char *ptr = strtok(holdBuff, "\n");
    int i = 0;
    while (ptr != NULL) {
        lines[i] = ptr;
        ptr = strtok(NULL, "\n");
        i++;
    }
    int nr_of_cmds = atoi(lines[0]);
    char *adress = lines[1];
    char *user = lines[2];
    char *pass = lines[3];
    i = 0;
    while (i <= nr_of_cmds) {
        cmds[i] = lines[i + 4];
        i++;
    }

    HINTERNET hFtpConnection;

    if (NULL == (hFtpConnection = InternetConnect(
            InternetHandler,
            adress,
            INTERNET_DEFAULT_FTP_PORT,
            user,
            pass,
            INTERNET_SERVICE_FTP,
            INTERNET_FLAG_PASSIVE,
            0
    ))) {
        printf("Error connecting to FTP server!\n");
        return 0;
    }
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char file_adress[128];
    char file_adress_copy[128];
    char filename[16];
    char exec_path[144];

    for (i = 0; i <= nr_of_cmds; i++) {
        strcpy(exec_path, "C:\\Users\\ALEX\\Desktop\\FII\\CSSO\\");
        ptr = strtok(cmds[i], " ");
        while (NULL != ptr) {
            if (strcmp(ptr, "RUN") == 0) {
                ptr = strtok(NULL, " ");
                strcat(exec_path, ptr);
                // Start the child process.
                if (!CreateProcess(exec_path,   // No module name (use command line)
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
                    ptr = NULL;
                }
                ptr = NULL;
                strcpy(exec_path, "C:\\Users\\ALEX\\Desktop\\FII\\CSSO\\");
            } else {
                if (strcmp(ptr, "PUT") == 0) {
                    ptr = strtok(NULL, " ");
                    strcpy(file_adress, ptr);
                    strcpy(file_adress_copy, file_adress);
                    strrev(file_adress_copy);
                    char *ptr2 = strtok(file_adress_copy, "\\");
                    strcpy(filename, ptr2);
                    strrev(filename);
                    if (!FtpPutFile(
                            hFtpConnection,
                            file_adress,
                            filename,
                            FTP_TRANSFER_TYPE_BINARY,
                            0
                    )) {
                        printf("Error uploading file to FTP server!\n");
                        ptr = NULL;
                    }
                    ptr = NULL;
                }
            }
        }
    }
    CloseHandle(hFtpConnection);
    CloseHandle(ConnectionHandler);
    CloseHandle(InternetHandler);
    return 0;
}
