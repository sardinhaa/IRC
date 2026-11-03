/*________________________________servidorTCPv1.c___________________________________*/
/*======================= Servidor interactivo TCP ============================
  Este servidor destina-se a mostrar mensagens recebidas via TCP de forma contínua,
  mantendo a ligação aberta até o cliente enviar "SAIR" ou fechar a sessão.
===============================================================================*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define SERV_TCP_PORT  6000
#define BUFFERSIZE     4096

void Abort(const char* msg, SOCKET s);
void AtendeCliente(SOCKET sock);
int writeN(SOCKET sock, char* buffer, int nbytes);
int readLine(SOCKET sock, char* buffer, int tamMax);

/*________________________________ main ________________________________________*/
int main(int argc, char* argv[]) {

	SOCKET sock = INVALID_SOCKET, newSock = INVALID_SOCKET;
	int iResult;
	int cliaddr_len;
	struct sockaddr_in cli_addr, serv_addr;
	WSADATA wsaData;

	/*=============== INICIA OS WINSOCKS ==============*/
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		getchar();
		exit(1);
	}

	/*================== ABRE SOCKET PARA ESCUTA DE CLIENTES ================*/
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		Abort("Impossibilidade de abrir socket", sock);

	/*=================== PREENCHE ENDERECO DE ESCUTA =======================*/
	memset((char*)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  /* Recebe de qq interface */
	serv_addr.sin_port = htons(SERV_TCP_PORT);      /* Escuta no porto Well-Known */

	/*====================== REGISTA-SE PARA ESCUTA =========================*/
	if (bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
		Abort("Impossibilidade de registar-se para escuta", sock);

	/*============ AVISA QUE ESTA PRONTO A ACEITAR PEDIDOS ==================*/
	if (listen(sock, 5) == SOCKET_ERROR)
		Abort("Impossibilidade de escutar pedidos", sock);

	printf("<SERV> Servidor TCP a escutar no porto %d...\n", SERV_TCP_PORT);

	/*================ PASSA A ATENDER CLIENTES INTERACTIVAMENTE =============*/
	cliaddr_len = sizeof(cli_addr);
	while (1) {
		if ((newSock = accept(sock, (struct sockaddr*)&cli_addr, &cliaddr_len)) == SOCKET_ERROR) {
			fprintf(stderr, "<SERV> Impossibilidade de aceitar cliente...\n");
		}
		else {
			printf("<SERV> Novo cliente conectado.\n");
			AtendeCliente(newSock);
			closesocket(newSock);
			printf("<SERV> Ligacao ao cliente terminada. À espera de novos clientes...\n");
		}
	}
}

/*___________________________ AtendeCliente ____________________________________*/
void AtendeCliente(SOCKET sock) {
	char buffer[BUFFERSIZE];
	static unsigned int cont = 0;
	int nbytes;

	/* Ciclo para manter a conversação aberta com o mesmo cliente (estilo Telnet) */
	while (1) {
		nbytes = readLine(sock, buffer, BUFFERSIZE);

		if (nbytes == SOCKET_ERROR) {
			fprintf(stderr, "\n<SER> Erro na recepcao de dados...\n");
			break;
		}
		if (nbytes == 0) {
			fprintf(stderr, "\n<SER> O cliente fechou a ligacao.\n");
			break;
		}

		/*================== VERIFICA SE O COMANDO É "SAIR" ==================*/
		if (strcmp(buffer, "SAIR") == 0) {
			printf("\n<SER> Comando SAIR recebido. A fechar ligacao com este cliente...\n");
			// Se quiser que o comando SAIR feche o servidor inteiro em vez de apenas o cliente, 
			// pode descomentar a linha abaixo:
			// exit(EXIT_SUCCESS);
			break; // Sai do ciclo e fecha este socket, mantendo o servidor pronto para outros
		}

		printf("\n<SER> Mensagem n. %d recebida {%s}\n", ++cont, buffer);

		/*============ ENVIA CONFIRMACAO USANDO writeN =============*/
		printf("<SER> Confirma recepcao de mensagem.\n");

		char resposta[32];
		sprintf_s(resposta, sizeof(resposta), "%d\n", nbytes);

		if (writeN(sock, resposta, (int)strlen(resposta)) == SOCKET_ERROR) {
			fprintf(stderr, "<SER> Impossibilidade de Confirmar.\n");
			break;
		}
		else {
			printf("<SER> Mensagem confirmada.\n");
		}
	}
}

/*________________________________ writeN ______________________________________*/
int writeN(SOCKET sock, char* buffer, int nbytes) {
	int nleft = nbytes;
	int nwritten;
	char* ptr = buffer;

	while (nleft > 0) {
		nwritten = send(sock, ptr, nleft, 0);
		if (nwritten == SOCKET_ERROR)
			return SOCKET_ERROR;
		if (nwritten == 0)
			break;
		nleft -= nwritten;
		ptr += nwritten;
	}
	return (nbytes - nleft);
}

/*________________________________ readLine ____________________________________*/
int readLine(SOCKET sock, char* buffer, int tamMax) {
	int bytes_recebidos = 0, res;
	char c;

	while (bytes_recebidos < tamMax - 1) {
		res = recv(sock, &c, 1, 0);
		if (res == SOCKET_ERROR)
			return SOCKET_ERROR;
		if (res == 0)
			break; // Conexão fechada

		if (c == '\r')
			continue; // Ignora o \r
		if (c == '\n')
			break;    // Fim da linha

		buffer[bytes_recebidos++] = c;
	}
	buffer[bytes_recebidos] = '\0';
	return bytes_recebidos;
}

/*________________________________ Abort _______________________________________*/
void Abort(const char* msg, SOCKET s) {
	fprintf(stderr, "\a<SER> Erro fatal: %s <%d>\n", msg, WSAGetLastError());

	if (s != INVALID_SOCKET)
		closesocket(s);

	WSACleanup();
	exit(EXIT_FAILURE);
}	
