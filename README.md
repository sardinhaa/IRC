# Introdução às Redes de Comunicação (IRC) - Guia de Winsock (C)

Este repositório reúne a documentação, exemplos práticos e exercícios de programação de sockets (TCP e UDP) utilizando a biblioteca **Winsock** em ambiente Windows, estruturados por tópicos e por *bullet points* para a unidade curricular de **Introdução às Redes de Comunicação (IRC)**.

---

## 📋 Índice
1. [Introdução Geral ao Winsock](#1-introdução-geral-ao-winsock)
2. [Comunicação Orientada a Conexão (TCP)](#2-comunicação-orientada-a-conexão-tcp)
   - [Arquitetura Básica (Cliente / Servidor Simples)](#21-arquitetura-básica-cliente--servidor-simples)
   - [Servidores Avançados com Multiplexagem (`select()`)](#22-servidores-avançados-com-multiplexagem-select)
3. [Comunicação Não Orientada a Conexão (UDP)](#3-comunicação-não-orientada-a-conexão-udp)
   - [Comunicação Básica Ponto a Ponto (Echo)](#31-comunicação-básica-ponto-a-ponto-ex1_clicpp--ex1_srvcpp)
   - [Chat Distribuído e Gestão de Estado (FIFO)](#32-chat-distribuído-e-gestão-de-estado-2021-11-20-irc-testetdpp1-v06pdf)

---

## 1. Introdução Geral ao Winsock
* **Inicialização da Biblioteca:** Obrigatoriamente realizada através da função `WSAStartup(MAKEWORD(2,2), &wsaData)` antes de qualquer operação com sockets[cite: 1, 4].
* **Tratamento de Erros:** Capturados através de `WSAGetLastError()` para diagnóstico de falhas no subsistema de rede do sistema operativo[cite: 2, 5].
* **Encerramento:** Os sockets abertos devem ser fechados com `closesocket()` e a biblioteca limpa com `WSACleanup()` no final da execução[cite: 5, 8].

---

## 2. Comunicação Orientada a Conexão (TCP)

### 2.1. Arquitetura Básica (Cliente / Servidor Simples)
* **Lado do Servidor (`servidorTCP_Ex1.c`):**
  * Criação do socket TCP com `socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)`[cite: 2].
  * Configuração do endereço de escuta para qualquer interface (`INADDR_ANY`) num porto fixo (`SERV_TCP_PORT`)[cite: 2].
  * Registo do socket com `bind()` e colocação em modo de escuta com `listen()`[cite: 2].
  * Aceitação iterativa de conexões de clientes através de `accept()`[cite: 2].
  * Receção e envio de dados/confirmações utilizando as funções `recv()` e `send()`[cite: 2].
* **Lado do Cliente (`clienteTCP_Ex1.c`):**
  * Validação dos argumentos da linha de comando (mensagem a enviar, IP de destino e porto de destino)[cite: 1].
  * Resolução do endereço do servidor e estabelecimento da ligação com `connect()`[cite: 1].
  * Transmissão da string via `send()` e espera da confirmação do servidor via `recv()`[cite: 1].

### 2.2. Servidores Avançados com Multiplexagem (`select()`)
* **Gestão de Múltiplos Clientes:** Utilização da estrutura `fd_set` em conjunto com a função `select()` para monitorizar múltiplos sockets em simultâneo sem bloqueio síncrono[cite: 4, 6, 7].
* **Temporizações (Timeouts):** Configuração de limites de tempo de espera através da estrutura `timeval` (ex: 10 segundos de timeout para leituras ou aceitações)[cite: 4, 6, 7].
* **Tabelas de Clientes (`partners` / `socksTab`):**
  * Registo dinâmico de novas conexões aceites até um limite máximo (ex: `10` sockets)[cite: 3, 6, 7].
  * Remoção automática e fecho de sockets em caso de erro de leitura, fim de conexão ou timeout[cite: 6, 8, 9].
  * Difusão de mensagens (`sendMsgToClients`) a todos os parceiros registados, exceto ao remetente original, quando aplicável[cite: 8, 9].

---

## 3. Comunicação Não Orientada a Conexão (UDP)

### 3.1. Comunicação Básica Ponto a Ponto (`ex1_cli.cpp` / `ex1_srv.cpp`)
* **Criação do Socket:** Utilização do tipo de socket `SOCK_DGRAM`[cite: 16, 17].
* **Envio e Receção sem Conexão Prévia:**
  * Envio de pacotes através de `sendto()`, especificando diretamente a estrutura de endereço de destino (`struct sockaddr_in`)[cite: 16].
  * Receção de pacotes e identificação automática da origem (IP e porto do cliente) através de `recvfrom()`[cite: 17].
* **Atribuição Dinâmica de Portos:** O sistema operativo atribui automaticamente um porto local ao cliente através de `bind` implícito ou explícito, consultável via `getsockname()`[cite: 16].

### 3.2. Chat Distribuído e Gestão de Estado (`2021-11-20-IRC-TesteUDP...`)
* **Estrutura de Dados FIFO:** 
  * Armazenamento local das últimas mensagens recebidas (ex: limite de 10 mensagens) num array gerido em formato *First-In-First-Out*[cite: 10, 13, 14].
  * Deslocação (*shift*) das mensagens anteriores no array sempre que uma nova mensagem é rececionada[cite: 14].
* **Formatação de Mensagens Aumentadas:**
  * Enriquecimento do conteúdo recebido com as coordenadas de origem (endereço IP e porto do cliente remetente, ex: `"127.0.0.1:62199# Mensagem"`) antes de proceder ao armazenamento e reenvio[cite: 10, 13, 14].
* **Gestão de Identidades e Validação (`VdR.c`):**
  * Leitura e validação rigorosa dos parâmetros recebidos na linha de comando (identidade da viatura, portos e IPs do Gestor de Recolhas - GdR)[cite: 19, 20].
  * Verificação estrita da origem dos pacotes UDP recebidos (`strcmp` para o IP e comparação de portas) para garantir que os comandos provêm exclusivamente da entidade autorizada[cite: 20, 22].
  * 
