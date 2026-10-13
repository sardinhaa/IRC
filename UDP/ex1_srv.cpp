/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

#define SERV_UDP_PORT 50000
#define BUFFER_SIZE 1024

int main(void) {
    WSADATA wsa_data;
    SOCKET socket_servidor;
    struct sockaddr_in info_servidor;
    struct sockaddr_in info_cliente;
    int tamanho_cliente;
    int res, bytes_recebidos;
    char buffer[BUFFER_SIZE];

    // Inicializar o Winsock
    res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (res != 0) {
        fprintf(stderr, "Falha no Winsock (%d)\n", res);
        system("pause");
        return EXIT_FAILURE;
    }

    // Criar o socket UDP
    socket_servidor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_servidor == INVALID_SOCKET) {
        fprintf(stderr, "Falha no socket (%d)\n", WSAGetLastError());
        WSACleanup();
        system("pause");
        return EXIT_FAILURE;
    }

    // Configurar os dados do servidor
    memset(&info_servidor, 0, sizeof(info_servidor));
    info_servidor.sin_family = AF_INET;
    info_servidor.sin_addr.s_addr = htonl(INADDR_ANY);
    info_servidor.sin_port = htons(SERV_UDP_PORT);

    // Fazer o bind ao porto
    res = bind(socket_servidor, (struct sockaddr*)&info_servidor, sizeof(info_servidor));
    if (res == SOCKET_ERROR) {
        fprintf(stderr, "Falha no bind (%d)\n", WSAGetLastError());
        closesocket(socket_servidor);
        WSACleanup();
        system("pause");
        return EXIT_FAILURE;
    }

    printf("Servidor UDP ativo no porto %d...\n", SERV_UDP_PORT);

    // Ciclo infinito para processar os pedidos dos clientes
    while (1) {
        tamanho_cliente = sizeof(info_cliente);

        // Recebe a mensagem e guarda a origem em info_cliente
        bytes_recebidos = recvfrom(socket_servidor, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&info_cliente, &tamanho_cliente);

        if (bytes_recebidos == SOCKET_ERROR || bytes_recebidos < 0) {
            fprintf(stderr, "Erro ao receber pacote (%d)\n", WSAGetLastError());
            continue;
        }

        buffer[bytes_recebidos] = '\0';

        // Obter a localização de origem (IP e Porto)
        char* ip_cliente = inet_ntoa(info_cliente.sin_addr);
        unsigned short porto_cliente = ntohs(info_cliente.sin_port);

        // Mostra na consola a localização de origem e os conteúdos da mensagem
        fprintf(stdout, "Mensagem recebida de %s:%d -> %s\n", ip_cliente, porto_cliente, buffer);
        fflush(stdout);

        // Reenviar a mensagem de volta ao cliente (Echo)
        int res_envio = sendto(socket_servidor, buffer, bytes_recebidos, 0, (struct sockaddr*)&info_cliente, tamanho_cliente);

        if (res_envio == SOCKET_ERROR) {
            fprintf(stderr, "Erro ao reenviar a mensagem (%d)\n", WSAGetLastError());
        }
    }

    closesocket(socket_servidor);
    WSACleanup();
    return EXIT_SUCCESS;
}
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

#define SERV_UDP_PORT 50000
#define BUFFER_SIZE 1024

int main(void) {
    WSADATA wsa_data;
    SOCKET socket_servidor;
    struct sockaddr_in info_servidor;
    struct sockaddr_in info_cliente;
    int tamanho_cliente;
    int res, bytes_recebidos;
    char buffer[BUFFER_SIZE];

    res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (res != 0) {
        fprintf(stderr, "Falha no Winsock (%d)\n", res);
        system("pause");
        return EXIT_FAILURE;
    }

    socket_servidor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_servidor == INVALID_SOCKET) {
        fprintf(stderr, "Falha no socket (%d)\n", WSAGetLastError());
        WSACleanup();
        system("pause");
        return EXIT_FAILURE;
    }

    memset(&info_servidor, 0, sizeof(info_servidor));
    info_servidor.sin_family = AF_INET;
    info_servidor.sin_addr.s_addr = htonl(INADDR_ANY);
    info_servidor.sin_port = htons(SERV_UDP_PORT);

    res = bind(socket_servidor, (struct sockaddr*)&info_servidor, sizeof(info_servidor));
    if (res == SOCKET_ERROR) {
        fprintf(stderr, "Falha no bind (%d)\n", WSAGetLastError());
        closesocket(socket_servidor);
        WSACleanup();
        system("pause");
        return EXIT_FAILURE;
    }

    printf("Servidor de Mensagens UDP ativo no porto %d...\n", SERV_UDP_PORT);

    while (1) {
        tamanho_cliente = sizeof(info_cliente);

        bytes_recebidos = recvfrom(socket_servidor, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&info_cliente, &tamanho_cliente);

        if (bytes_recebidos == SOCKET_ERROR || bytes_recebidos < 0) {
            fprintf(stderr, "Erro ao receber pacote (%d)\n", WSAGetLastError());
            continue;
        }

        buffer[bytes_recebidos] = '\0';

        // Obter Timestamp atual (Data e Hora) de forma segura no Windows
        time_t t = time(NULL);
        struct tm tm_info;
        localtime_s(&tm_info, &t);
        char time_str[32];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tm_info);

        // Extrair IP e Porto de origem
        char* ip_cliente = inet_ntoa(info_cliente.sin_addr);
        unsigned short porto_cliente = ntohs(info_cliente.sin_port);

        // REGISTO DETALHADO NO SERVIDOR (Timestamp, IP, Porto e Conteúdo)
        fprintf(stdout, "[%s] Mensagem recebida de IP: %s | Porto: %d -> Conteudo: %s\n",
            time_str, ip_cliente, porto_cliente, buffer);
        fflush(stdout);

        // Reenviar a mensagem de volta ao cliente (Echo com o formato de nome e mensagem)
        int res_envio = sendto(socket_servidor, buffer, bytes_recebidos, 0, (struct sockaddr*)&info_cliente, tamanho_cliente);

        if (res_envio == SOCKET_ERROR) {
            fprintf(stderr, "Erro ao reenviar a mensagem (%d)\n", WSAGetLastError());
        }
    }

    closesocket(socket_servidor);
    WSACleanup();
    return EXIT_SUCCESS;
}