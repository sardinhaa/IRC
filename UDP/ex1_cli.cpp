/*




#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

#define SERV_HOST_ADDR "127.0.0.1"
#define SERV_UDP_PORT 50000
#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    WSADATA wsa_data;
    SOCKET socket_cliente;
    struct sockaddr_in info_servidor;
    struct sockaddr_in info_cliente;
    int res;

    // Verifica se foram passados os argumentos corretos: executável, ip, porto, mensagem
    if (argc != 4) {
        fprintf(stdout, "\nSintaxe:\n%s   \n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (res != 0) {
        fprintf(stderr, "\n Erro: Falha ao iniciar o winsock (%d)\n", res);
        exit(EXIT_FAILURE);
    }

    // Cria socket IPv4 do tipo datagrama
    socket_cliente = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_cliente == INVALID_SOCKET) {
        fprintf(stderr, "\n Erro: Falha ao iniciar o socket (%d)\n", WSAGetLastError());
        closesocket(socket_cliente);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Preenchimento da estrutura com os dados do servidor
    memset(&info_servidor, 0, sizeof(info_servidor));
    info_servidor.sin_family = AF_INET;
    info_servidor.sin_port = htons(atoi(argv[2])); // argv[2] é o porto

    // CORREÇÃO: Usa argv[1] para o IP em vez da macro fixa
    res = inet_pton(AF_INET, argv[1], &info_servidor.sin_addr);
    if (res != 1) {
        fprintf(stderr, "\n Erro: Endereco IP invalido (%d)\n", WSAGetLastError());
        closesocket(socket_cliente);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // CORREÇÃO: Envia argv[3] (a mensagem) e mede o strlen de argv[3]
    res = sendto(socket_cliente, argv[3], (int)strlen(argv[3]), 0, (struct sockaddr*)&info_servidor, sizeof(info_servidor));
    if (res == SOCKET_ERROR) {
        fprintf(stderr, "\n Erro na transmissao (%d)\n", WSAGetLastError());
        closesocket(socket_cliente);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    int tam_info_cliente = sizeof(info_cliente);
    getsockname(socket_cliente, (struct sockaddr*)&info_cliente, &tam_info_cliente);

    fprintf(stdout, "Porto Local = %d\n", ntohs(info_cliente.sin_port));

    // CORREÇÃO: Mostra os dados dinâmicos corretos passados nos argumentos
    fprintf(stdout, "Mensagem enviada para %s:%s -> %s\n", argv[1], argv[2], argv[3]);

    // --- AGUARDAR RESPOSTA DO SERVIDOR E APRESENTAR NA SAÍDA STANDARD ---
    char buffer_resposta[BUFFER_SIZE];
    struct sockaddr_in info_servidor_resposta;
    int tamanho_servidor_resp = sizeof(info_servidor_resposta);

    int bytes_resp = recvfrom(socket_cliente, buffer_resposta, BUFFER_SIZE - 1, 0, (struct sockaddr*)&info_servidor_resposta, &tamanho_servidor_resp);

    if (bytes_resp == SOCKET_ERROR) {
        fprintf(stderr, "\n Erro na rececao da resposta (%d)\n", WSAGetLastError());
    }
    else {
        buffer_resposta[bytes_resp] = '\0';
        fprintf(stdout, "Resposta do Servidor (Echo) -> %s\n", buffer_resposta);
    }

    closesocket(socket_cliente);
    WSACleanup();
    return(EXIT_SUCCESS);
}


    /*
    cd C:\Users\gp\source\repos\ex1_cli\x64\Debug
    ex1_cli.exe "Mensagem"
    
    */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

#define BUFFER_SIZE 1024

    /*
    argv[0] -> nome do executavel
    argv[1] -> username
    argv[2] -> ip do servidor
    argv[3] -> porto do servidor
    argv[4] -> mensagem a enviar
    */

void closeall(SOCKET sock) {
    closesocket(sock);
    WSACleanup();
}

int main(int argc, char* argv[]) {
    WSADATA wsa_data;
    SOCKET socket_cliente;
    struct sockaddr_in info_servidor;
    struct sockaddr_in info_cliente;
    int res;

    // Validação: executável + username + IP + porto
    if (argc != 4) {
        fprintf(stdout, "\nSintaxe:\n%s <username> <ip_servidor> <porto_servidor>\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (res != 0) {
        fprintf(stderr, "\nErro: Falha ao iniciar o winsock (%d)\n", res);
        exit(EXIT_FAILURE);
    }

    socket_cliente = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_cliente == INVALID_SOCKET) {
        fprintf(stderr, "\nErro: Falha ao iniciar o socket (%d)\n", WSAGetLastError());
        closeall(socket_cliente);
        exit(EXIT_FAILURE);
    }

    // Configuração do endereço do servidor
    memset(&info_servidor, 0, sizeof(info_servidor));
    info_servidor.sin_family = AF_INET;
    info_servidor.sin_port = htons((unsigned short)atoi(argv[3]));

    res = inet_pton(AF_INET, argv[2], &info_servidor.sin_addr);
    if (res != 1) {
        fprintf(stderr, "\nErro: Endereco IP invalido (%d)\n", WSAGetLastError());
        closeall(socket_cliente);
        exit(EXIT_FAILURE);
    }

    // Mostra o porto local atribuído pelo sistema operativo
    int tam_info_cliente = sizeof(info_cliente);
    getsockname(socket_cliente, (struct sockaddr*)&info_cliente, &tam_info_cliente);
    fprintf(stdout, "Porto Local atribuido = %d\n", ntohs(info_cliente.sin_port));
    fprintf(stdout, "Conectado como [%s]. Digite ':q' para encerrar.\n\n", argv[1]);

    char input_mensagem[BUFFER_SIZE];
    char pacote[BUFFER_SIZE];
    char buffer_resposta[BUFFER_SIZE];

    while (1) {
        fprintf(stdout, "> ");
        if (fgets(input_mensagem, sizeof(input_mensagem), stdin) == NULL) {
            break;
        }

        // Remove quebra de linha (\r ou \n) do fgets
        input_mensagem[strcspn(input_mensagem, "\r\n")] = '\0';

        // Ignora linhas vazias
        if (strlen(input_mensagem) == 0) {
            continue;
        }

        // Condição de paragem
        if (strcmp(input_mensagem, ":q") == 0) {
            fprintf(stdout, "A terminar sessao...\n");
            break;
        }

        // Formata pacote: "Username: Mensagem"
        snprintf(pacote, sizeof(pacote), "%s: %s", argv[1], input_mensagem);

        res = sendto(socket_cliente, pacote, (int)strlen(pacote), 0, (struct sockaddr*)&info_servidor, sizeof(info_servidor));
        if (res == SOCKET_ERROR) {
            fprintf(stderr, "Erro na transmissao (%d)\n", WSAGetLastError());
            continue;
        }

        fprintf(stdout, "Mensagem enviada com sucesso!\n");
    }

    closeall(socket_cliente);
    return EXIT_SUCCESS;
}

/*
cd C:\Users\gp\source\repos\ex1_cli\x64\Debug
ex1_cli.exe "Mensagem"

*/