#define _CRT_SECURE_NO_WARNINGS
#include "Client.h"
#include "Mouv_test.h"
#include <stdio.h>
#include <string.h>

// Pragma pour lier la librairie Winsock automatiquement
#pragma comment(lib, "ws2_32.lib")

char SERVER_IP[16] = "127.0.0.1";
SOCKET socket_client = INVALID_SOCKET;
struct sockaddr_in server_Addr;
char machines_trouvees[10][16];
int count = 0;
Message msg = {0};
Message response = {0};

/**
 * @brief Set the Server IP object
 * 
 * @param new_ip 
 */
void SetServerIP(const char* new_ip) {
    if (!is_valid_ip(new_ip)) {
        printf("Adresse IP invalide : %s\n", new_ip);
        return;
    }
    strncpy(SERVER_IP, new_ip, 15);
    SERVER_IP[15] = '\0'; // Sécurité
    
    // On met à jour la structure d'adresse immédiatement
    memset(&server_Addr, 0, sizeof(server_Addr));
    server_Addr.sin_family = AF_INET;
    server_Addr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_Addr.sin_addr);
    
    printf("Serveur configure sur : %s\n", SERVER_IP);
}


/**
 * @brief     Tente de convertir l'IP en binaire et 
 *            Retourne 1 si succès, 0 si éche
 * 
 * @param ip 
 * @return int 
 */
int is_valid_ip(const char* ip) {
    struct sockaddr_in sa;
    // Tente de convertir l'IP en format binaire
    // Retourne 1 si succès, 0 si échec
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1;
}
/**
 * @brief Ouvre la connection
 * 
 * @param ip 
 * @return int 
 */
int OpenConnection(const char* ip) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return -1;

    socket_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_client == INVALID_SOCKET) return -1;

    // Configuration du timeout
    DWORD timeout = TIMEOUT;
    setsockopt(socket_client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    // Configuration de l'adresse
    memset(&server_Addr, 0, sizeof(server_Addr));
    server_Addr.sin_family = AF_INET;
    server_Addr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, ip, &server_Addr.sin_addr);
    return 0;
}
/**
 * @brief Gere l'envoi et la réponse de message venant/cherchant du serveur
 * 
 * @param msg 
 * @param reponse 
 * @return int 
 */
int SendAndRecieveMessage(Message* msg, Message* reponse) {
    if (socket_client == INVALID_SOCKET) return -1;

    unsigned char send_buffer[104];


    send_buffer[0] = msg->type;
    send_buffer[1] = msg->number;
    send_buffer[2] = msg->length;
    memcpy(&send_buffer[3], msg->data, msg->length);
    send_buffer[3 + msg->length] = msg->checksum;

    int sendResult = sendto(
        socket_client,
        (const char*)send_buffer,
        4 + msg->length,
        0,
        (struct sockaddr*)&server_Addr,
        sizeof(server_Addr)
    );

    if (sendResult == SOCKET_ERROR) {
        return WSAGetLastError();
    }

    unsigned char recv_buffer[104];
    int fromSize = sizeof(server_Addr);

    int recvResult = recvfrom(
        socket_client,
        (char*)recv_buffer,
        sizeof(recv_buffer),
        0,
        (struct sockaddr*)&server_Addr,
        &fromSize
    );

    if (recvResult == SOCKET_ERROR) {
        return WSAGetLastError();
    }

    if (recvResult < 4) {
        return -2;
    }

    reponse->type = recv_buffer[0];
    reponse->number = recv_buffer[1];
    reponse->length = recv_buffer[2];

    if (reponse->length > 100) {
        return -3;
    }

    if (recvResult != 4 + reponse->length) {
        return -4;
    }

    memcpy(reponse->data, &recv_buffer[3], reponse->length);
    reponse->checksum = recv_buffer[3 + reponse->length];

    if (compute_checksum(reponse) != reponse->checksum) {
        return -5;
    }

    return 0;
}
/**
 * @brief Ferme la connection en cas de socket invalide
 * 
 */
void CloseConnection() {
    if (socket_client != INVALID_SOCKET) {
        closesocket(socket_client);
        WSACleanup();
    }
}