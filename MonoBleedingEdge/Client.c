#pragma once
#include "Client.h"
#include <iostream>
#include "Mouv.h"

// Pragma pour lier la librairie Winsock automatiquement
#pragma comment(lib, "ws2_32.lib")

int SendAndRecieveMessage(Message* msg, Message* reponse, const char* server_ip) {
    // 1. Initialisation de Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return WSAGetLastError();
    }

    // 2. Création du socket UDP
    SOCKET socket_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_client == INVALID_SOCKET) {
        int err = WSAGetLastError();
        WSACleanup();
        return err;
    }

    // 3. Configuration du Timeout (APRES la création du socket)
    DWORD timeout = TIMEOUT; 
    if (setsockopt(socket_client, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        closesocket(socket_client);
        WSACleanup();
        return err;
    }

    // 5. Configuration de l'adresse du serveur
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, server_ip, &serverAddr.sin_addr);

    // 6. Envoi du message
    // Utilisation de sizeof(Message) car ta structure a une taille fixe
    int sendResult = sendto(socket_client, (const char*)msg, sizeof(Message), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (sendResult == SOCKET_ERROR) {
        int err = WSAGetLastError();
        closesocket(socket_client);
        WSACleanup();
        return err;
    }

    // 7. Réception de la réponse
    struct sockaddr_in fromAddr;
    int fromAddrSize = sizeof(fromAddr);
    // On reçoit directement dans l'adresse pointée par 'reponse'
    int recvResult = recvfrom(socket_client, (char*)reponse, sizeof(Message), 0, (struct sockaddr*)&fromAddr, &fromAddrSize);
    
    if (recvResult == SOCKET_ERROR) {
        int errorCode = WSAGetLastError();
        if (errorCode == WSAETIMEDOUT) {
            printf("Timeout : Aucune reponse du serveur %s\n", server_ip);
        } else {
            printf("Erreur de reception : %d\n", errorCode);
        }
        closesocket(socket_client);
        WSACleanup();
        return errorCode;
    }

    // 8. Nettoyage final
    closesocket(socket_client);
    WSACleanup();
    return 0; // Succès, 'reponse' est maintenant rempli
}