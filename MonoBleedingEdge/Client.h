#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define DEFAULT_PORT 27598
#define TIMEOUT 5000 // Timeout en millisecondes

const char *SERVER_IP = "127.0.0.1"; // Adresse IP du serveur (localhost)


int SendAndRecieveMessage(Message* msg, Message* reponse, const char* server_ip);