/*=========================== Cliente basico TCP ===============================*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFFERSIZE     4096

void Abort(const char* msg, SOCKET s);
int writeN(SOCKET sock, char* buffer, int nbytes);
int readLine(SOCKET sock, char* buffer, int tamMax);

/*________________________________ main ________________________________________*/
int main(int argc, char* argv[]) {

	SOCKET sock = INVALID_SOCKET;
	int msg_len, nbytes, iResult;
	struct addrinfo hints, * res = NULL;
	char buffer[BUFFERSIZE];
	WSADATA wsaData;

	if (argc != 4) { /* Testa sintaxe */
		fprintf(stderr, "<CLI> Sintaxe: %s \"frase_a_enviar\" ip_ou_nome_destino porto_destino\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/*=============== INICIA OS WINSOCKS ==============*/
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		(void)getchar();
		exit(1);
	}

	/*=============== PREENCHE HINTS PARA O GETADDRINFO ==============*/
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       /* IPv4 */
	hints.ai_socktype = SOCK_STREAM; /* TCP */
	hints.ai_protocol = IPPROTO_TCP;

	/*=============== RESOLVE O NOME DO SERVIDOR E PORTO ====================*/
	iResult = getaddrinfo(argv[2], argv[3], &hints, &res);
	if (iResult != 0) {
		fprintf(stderr, "<CLI> getaddrinfo failed: %d\n", iResult);
		Abort("Impossibilidade de resolver o nome do servidor", sock);
	}

	/*=============== ABRE SOCKET COM OS DADOS RESOLVIDOS ==================*/
	if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == INVALID_SOCKET) {
		freeaddrinfo(res);
		Abort("Impossibilidade de abrir socket", sock);
	}

	/*========================== ESTABELECE LIGACAO ======================*/
	if (connect(sock, res->ai_addr, (int)res->ai_addrlen) == SOCKET_ERROR) {
		freeaddrinfo(res);
		Abort("Impossibilidade de estabelecer ligacao", sock);
	}

	/* Já não precisamos da estrutura de endereços */
	freeaddrinfo(res);

	while (1) {
		/*====================== ENVIA MENSAGEM AO SERVIDOR ==================*/
		msg_len = (int)strlen(argv[1]);

		// Verifica se o argumento enviado é "SAIR"
		int is_sair = (strcmp(argv[1], "SAIR") == 0);
		if (is_sair) {
			printf("<CLI> A enviar pedido de encerramento (SAIR) ao servidor...\n");
		}

		// Prepara a mensagem adicionando \n para o servidor ler com readLine
		char* mensagem_com_nl = (char*)malloc(msg_len + 2);
		if (mensagem_com_nl == NULL) {
			Abort("Erro de alocacao de memoria", sock);
		}
		strcpy_s(mensagem_com_nl, msg_len + 2, argv[1]);
		strcat_s(mensagem_com_nl, msg_len + 2, "\n");

		if ((nbytes = writeN(sock, mensagem_com_nl, (int)strlen(mensagem_com_nl))) == SOCKET_ERROR) {
			free(mensagem_com_nl);
			Abort("Impossibilidade de transmitir mensagem...", sock);
		}

		free(mensagem_com_nl);
		fprintf(stderr, "<CLI> Mensagem \"%s\" enviada\n", argv[1]);

		// Se enviou SAIR, sai do ciclo antes de tentar ler resposta (ou trata o fecho)
		if (is_sair) {
			printf("<CLI> Pedido de saída enviado. A fechar...\n");
			break;
		}

		/*========================== ESPERA CONFIRMACAO =======================*/
		nbytes = readLine(sock, buffer, BUFFERSIZE);

		if (nbytes == SOCKET_ERROR) {
			Abort("Impossibilidade de receber confirmacao", sock);
		}
		if (nbytes == 0) {
			printf("<CLI> O servidor encerrou a ligacao.\n");
			break;
		}
		else {
			printf("<CLI> Confirmacao recebida {%s}.\n", buffer);
		}

		// Como o enunciado de um cliente básico tipicamente envia apenas 1 mensagem e termina, 
		// se pretender apenas 1 envio por execução, pode remover o while(1) ou colocar um break aqui.
		break;
	}

	/*=========================== FECHA SOCKET ============================*/
	closesocket(sock);
	WSACleanup();

	exit(EXIT_SUCCESS);
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
	fprintf(stderr, "<CLI> Erro fatal: %s <%d>\n", msg, WSAGetLastError());

	if (s != INVALID_SOCKET)
		closesocket(s);

	WSACleanup();
	exit(EXIT_FAILURE);
}
