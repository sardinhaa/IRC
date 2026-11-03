#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// Função que será executada por cada thread para atender um cliente
DWORD WINAPI AtendeCliente(LPVOID lpParam) {
    SOCKET newSock = (SOCKET)lpParam;
    char buffer[1024];
    int res;

    printf("<SERV> Atendendo novo cliente no socket %d\n", (int)newSock);

    // Exemplo: Receber dados do cliente
    res = recv(newSock, buffer, sizeof(buffer) - 1, 0);
    if (res > 0) {
        buffer[res] = '\0';
        printf("<SERV> Recebido: %s\n", buffer);
    }

    printf("<SERV> Ligação ao cliente terminada.\n");
    closesocket(newSock);
    return 0;
}

int main(int argc, char *argv[]) {
    WSADATA wsa_data;
    SOCKET socket_proxy, newSock;
    struct sockaddr_in info_proxy, cli_addr;
    int cliaddr_len = sizeof(cli_addr);
    HANDLE hThread;
    int res;

    // Verificar parâmetros
    if (argc < 2) {
        printf("Erro de sintaxe: use proxy.exe <porta>\n");
        return 1;
    }

    // Iniciar Winsock
    res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (res != 0) {
        fprintf(stderr, "\nErro: Falha ao iniciar o winsock (%d)\n", res);
        return 1;
    }

    // Criar socket TCP (SOCK_STREAM)
    socket_proxy = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_proxy == INVALID_SOCKET) {
        fprintf(stderr, "\nErro: Falha ao criar o socket (%d)\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configurar informações do servidor
    memset(&info_proxy, 0, sizeof(info_proxy));
    info_proxy.sin_family = AF_INET;
    info_proxy.sin_addr.s_addr = htonl(INADDR_ANY);
    info_proxy.sin_port = htons(atoi(argv[1]));

    // Registrar o servidor (Bind)
    if (bind(socket_proxy, (struct sockaddr*)&info_proxy, sizeof(info_proxy)) == SOCKET_ERROR) {
        fprintf(stderr, "Erro: Impossibilidade de registar-se para escuta (%d)\n", WSAGetLastError());
        closesocket(socket_proxy);
        WSACleanup();
        return 1;
    }

    // Colocar em modo de escuta
    if (listen(socket_proxy, SOMAXCONN) == SOCKET_ERROR) {
        fprintf(stderr, "Erro no listen (%d)\n", WSAGetLastError());
        closesocket(socket_proxy);
        WSACleanup();
        return 1;
    }

    printf("Servidor proxy a escuta no porto %s\n", argv[1]);

    while (1) {
        // Aceitar nova conexão
        newSock = accept(socket_proxy, (struct sockaddr*)&cli_addr, &cliaddr_len);
        if (newSock == INVALID_SOCKET) {
            fprintf(stderr, "<SERV> Impossibilidade de aceitar cliente... (%d)\n", WSAGetLastError());
            continue;
        }

        printf("<SERV> Novo cliente conectado.\n");

        // Criar Thread para atender o cliente
        hThread = CreateThread(NULL, 0, AtendeCliente, (LPVOID)newSock, 0, NULL);
        if (hThread == NULL) {
            fprintf(stderr, "Erro ao criar thread (%d)\n", GetLastError());
            closesocket(newSock);
        } else {
            // Fechar o handle da thread (não o socket) para evitar leak de recursos
            CloseHandle(hThread);
        }
    }

    closesocket(socket_proxy);
    WSACleanup();
    return 0;
}