#pragma once
#include "Serveur.h"

StartServer(Message* receivedMessage, Message* responseMessage, const char* server_ip) {
    // Initialisation de Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Erreur d'initialisation de Winsock: %d\n", WSAGetLastError());
        return WSAGetLastError();
    }

    // Création du socket UDP
    SOCKET socket_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_server == INVALID_SOCKET) {
        printf("Erreur de création du socket: %d\n", WSAGetLastError());
        WSACleanup();
        return WSAGetLastError();
    }

    // Configuration de l'adresse du serveur
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Écoute sur toutes les interfaces

    // Liaison du socket à l'adresse
    if (bind(socket_server, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Erreur de bind: %d\n", WSAGetLastError());
        closesocket(socket_server);
        WSACleanup();
        return WSAGetLastError();
    }
    printf("Serveur en écoute sur le port %d...\n", DEFAULT_PORT);
    while (1) {
        struct sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        int recvResult = recvfrom(socket_server, (char*)receivedMessage, sizeof(Message), 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (recvResult == SOCKET_ERROR) {
            printf("Erreur de réception du message: %d\n", WSAGetLastError());
            continue; // Continuer à écouter malgré l'erreur
        }

        // Vérification du checksum
        unsigned char expectedChecksum = calculateChecksum(receivedMessage);
        if (expectedChecksum != receivedMessage->checksum) {
            printf("Checksum invalide pour le message reçu.\n");
            continue; // Ignorer le message avec un checksum invalide
        }

        printf("Message reçu de %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        
        // Envoyer la réponse au client
        int sendResult = sendto(socket_server, (const char*)responseMessage, sizeof(Message), 0, (struct sockaddr*)&clientAddr, clientAddrSize);
        if (sendResult == SOCKET_ERROR) {
            printf("Erreur d'envoi du message: %d\n", WSAGetLastError());
            continue; // Continuer à écouter malgré l'erreur d'envoi
        }
    }
}