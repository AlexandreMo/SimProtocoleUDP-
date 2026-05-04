/*--------------------------------------------------------------------------------
-- HEIG-VD, institute REDS, 1400 Yverdon-les-Bains
-- Project : Digital Twin CPU Assembly
-- File    : DigitalTwinCpuAssembly.c
-- Author  : Alexandre Moore & Tenkeu Franklin 
-- Date    : 27.04.2026
--------------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <WS2tcpip.h>

// librairie pour pilotage
#include "Mouv.h"
// Definir le port
#define PORT_NUMBER 27598
// Menu pour la console - renvoie le choix de l'utilisateur ou -1 en cas d'erreur de saisie
char menu_principal()
{
    printf("Assemblage de processeurs =====================\n");
    printf("1. Commandes manuelles\n");
    printf("2. Pilotage automatise\n");
    printf("3. Messages d'erreur de communication\n");
    printf("4. Saisie de l'adresse de la machine\n");
    printf("5. Detection par diffusion\n");
    printf("9. Quitter le programme\n");
    printf("Choix>");
    char choix;
    // Saisie du choix de l'utilisateur
    if (scanf("%d", &choix) != 1)
    {
        return -1; // Erreur de saisie
        printf("Choix invalide. Veuillez entrer un nombre.\n");
    }
    return choix;
}
char nb_pieces()
{
    printf("Nombre de pièces à assembler (1-10)>");
    char nb_pieces;
    // Saisie du nombre de pièces à assembler
    if (scanf("%d", &nb_pieces) != 1 || nb_pieces < 1 || nb_pieces > 10)
    {
        return -1; // Erreur de saisie
        printf("Nombre de pièces invalide. Veuillez entrer un nombre entre 1 et 10.\n");
    }
    return nb_pieces;
}
int main()
{
    // Initialiser Winsock 2
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    // Créer un socket UDP
    SOCKET socket_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_client == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
        int choix = 0;
        choix = menu_principal();
        switch (choix)
        {
            case 1:
                printf("Commandes manuelles sélectionnées.\n");
                manual_pilotage();
                break;
            case 2:
                printf("Pilotage automatisé sélectionné.\n");
                char n_pieces = nb_pieces();
                if (n_pieces != -1) {
                    automatic_pilotage(n_pieces);
                }   
                break;
            case 3:
                printf("Messages d'erreur de communication selectionnes.\n");
                error_message();
                break;
            case 4:
                printf("Saisie de l'adresse de la machine selectionnee.\n");
                // Implémenter la saisie de l'adresse de la machine ici
                break;
            case 5:
                printf("Detection par diffusion sélectionnee.\n");
                // Implémenter la détection par diffusion ici
                break;
            case 9:
                printf("Quitter le programme sélectionne.\n");
                break;
            default:
                printf("Choix invalide. Veuillez entrer un nombre entre 1 et 5 ou 9 pour quitter.\n");
                break;
        }

    // Fermer le socket (libérer les ressources)
    closesocket(socket_client);
    // Finaliser Winsock 2
    WSACleanup();
}