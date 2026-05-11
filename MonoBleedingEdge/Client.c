#pragma once
#include "Client.h"
#include <iostream>
#include "GeneralParameters.h"

// Pragma pour lier la librairie Winsock automatiquement
#pragma comment(lib, "ws2_32.lib")

unsigned char calculateChecksum(Message* msg) {
    unsigned char checksum = 0;
    checksum += msg->type;
    checksum += msg->number;
    checksum += msg->length;
    for (int i = 0; i < msg->length; i++) {
        checksum += msg->data[i];
    }
    return checksum;
}

int SendAndRecieveMessage(Message* msg, Message* reponse, const char* server_ip) {
    // Initialisation de Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Erreur d'initialisation de Winsock: %d\n", WSAGetLastError());
        return WSAGetLastError();
    }

    // Création du socket UDP
    SOCKET socket_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (socket_client == INVALID_SOCKET) {
        printf("Erreur de création du socket: %d\n", WSAGetLastError());
        WSACleanup();
        return WSAGetLastError();
    }

    DWORD timeout = TIMEOUT; // 1 seconde
    setsockopt(socket_client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    
    msg->checksum = calculateChecksum(msg);

    // Configuration de l'adresse du serveur
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, server_ip, &serverAddr.sin_addr);

    // Envoi du message
    // On envoie l'adresse de la structure (msg) et la taille totale de la structure
    int sendResult = sendto(socket_client, (const char*)msg, sizeof(Message), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (sendResult == SOCKET_ERROR) {
        printf("Erreur d'envoi du message: %d\n", WSAGetLastError());
        closesocket(socket_client);
        WSACleanup();
        return WSAGetLastError();
    }

    // Réception de la réponse
    struct sockaddr_in fromAddr;
    int fromAddrSize = sizeof(fromAddr);
    int recvResult = recvfrom(socket_client, (char*)reponse, sizeof(Message), 0, (struct sockaddr*)&fromAddr, &fromAddrSize);
    if (recvResult == SOCKET_ERROR) {
        int errorCode = WSAGetLastError();
        if (errorCode == WSAETIMEDOUT) {
            printf("Timeout lors de la réception du message.\n");
        } else {
            printf("Erreur de réception du message: %d\n", errorCode);
        }
        closesocket(socket_client);
        WSACleanup();
        return errorCode;
    }

    // Fermeture du socket et nettoyage de Winsock
    closesocket(socket_client);
    WSACleanup();
    return 0; // Succès
}
