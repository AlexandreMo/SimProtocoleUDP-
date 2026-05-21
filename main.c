/*--------------------------------------------------------------------------------
-- HEIG-VD, bureau iai, 1400 Yverdon-les-Bains
-- Project : Header file for robot control functions
-- File    : Mouv.c
-- Author   : Alexandre Moore & Tenkeu Franklin 
-- Date    : 27.04.2026
--------------------------------------------------------------------------------*/
#define _CRT_SECURE_NO_WARNINGS
#include "Mouv_test.h"
#include "GestionMenu.h"
#include "Client.h"

int main() {
    int choix = -1;
    int choice = -1;
    if (OpenConnection(SERVER_IP) != 0) {
        printf("Erreur d'ouverture de connexion.\n");
        return 1;
    }

    while (1) {
        choix = menu_principal();
        switch (choix) {
        case 1:
            while (choice != 99) {
                choice = menu_pilotage_manuel();
                manual_pilotage(choice);
            }
            break;
        case 2:
            printf("Combien de pieces a assembler ? ");
            int n_pieces;
            if (scanf("%d", &n_pieces) != 1 || n_pieces <= 0) {
                printf("Nombre invalide. Veuillez entrer un nombre entier positif.\n");
                break;
            }
            automatic_pilotage(n_pieces);
            break;
        case 3:
            Message invalid_msg;
            // Message 1 
            invalid_msg.type = 0x39; // type invalide intentionnellement
            invalid_msg.number = 0;
            invalid_msg.length = 2;
            invalid_msg.data[0] = 0xAA;
            invalid_msg.data[1] = 0x55;
            invalid_msg.checksum = compute_checksum(&invalid_msg);

            SendAndRecieveMessage(&invalid_msg, &response);
            handle_other_responses(&response);
            // Message 2 
            invalid_msg.type = COM_CONVEYOR; // type invalide intentionnellement
            invalid_msg.number = 1;
            invalid_msg.length = 0;
            invalid_msg.data[0] = 0x55;
            invalid_msg.data[1] = 0x78; // Donnée Invalides
            invalid_msg.checksum = compute_checksum(&invalid_msg);
            SendAndRecieveMessage(&invalid_msg, &response);
            handle_other_responses(&response);
            // Message 3 
            invalid_msg.type = COM_CONVEYOR; // type invalide intentionnellement
            invalid_msg.number = 0;
            invalid_msg.length = 1;
            invalid_msg.data[0] = 1;
            invalid_msg.checksum = 5;
            SendAndRecieveMessage(&invalid_msg, &response);
            
            handle_other_responses(&response);
            break;
        case 4:
        {
            printf("Saisie de l'adresse de la machine non implémentée.\n");
            char new_ip[16];
            int valide = 0;

            while (!valide) {
                printf("Entrez la nouvelle IP du serveur : ");

                // 1. On vérifie que scanf a lu une chaîne
                if (scanf("%15s", new_ip) == 1) {

                    // 2. On vérifie que l'IP est syntaxiquement correcte
                    if (is_valid_ip(new_ip)) {
                        valide = 1; // On sort de la boucle
                    }
                    else {
                        printf("Erreur : '%s' n'est pas une adresse IPv4 valide (ex: 127.0.0.1).\n", new_ip);
                    }
                }
                else {
                    printf("Erreur de lecture. Veuillez reessayer.\n");
                }

                // 3. IMPORTANT : Nettoyage du tampon (buffer) de saisie
                // Si l'utilisateur tape "127.0.0.1 n'importe quoi", le surplus reste dans le buffer
                while (getchar() != '\n');
            }

            SetServerIP(new_ip);
            printf("Configuration reussie !\n");
            break;
        }
        case 5:
        {
            printf("Detection par des machines par diffusion.\n");
            char diff_ip[16];
            int valide1 = 0;

            while (!valide1) {
                printf("Entrez l'adresse de diffusion: ");

                // 1. On vérifie que scanf a lu une chaîne
                if (scanf("%15s", diff_ip) == 1) {

                    // 2. On vérifie que l'IP est syntaxiquement correcte
                    if (is_valid_ip(diff_ip)) {
                        valide1 = 1; // On sort de la boucle
                    }
                    else {
                        printf("Erreur : '%s' n'est pas une adresse IPv4 valide (ex: 127.0.0.1).\n", diff_ip);
                    }
                }
                else {
                    printf("Erreur de lecture. Veuillez reessayer.\n");
                }

                // 3. IMPORTANT : Nettoyage du tampon (buffer) de saisie
                // Si l'utilisateur tape "127.0.0.1 n'importe quoi", le surplus reste dans le buffer
                while (getchar() != '\n');
            }

            localisation_machine(diff_ip);
            int choice1 = -1;
            int valide2 = 0;
            while (!valide2) {
                if (scanf("%d", &choice1) != 1) {
                    printf("Entrée invalide.\n");
                }
                else
                {
                    if (count == 0 && choice1 == 0)
                    {
                        printf("Retour au menu principal.\n");
                        valide2 = 1;
                    }

                    else if ((count > 0) && (choice1 >= 0) && (choice1 <= count)) {
                        SetServerIP(machines_trouvees[choice1 - 1]);
                        printf("Configuration reussie !\n");
                        valide2 = 1;
                    }
                    else 
                    {
                        printf("Choix invalide. \n");
                    }
                }
            }
            break;
        }
        case 9:
            printf("Quitter le programme.\n");
            CloseConnection();
            return 0;
        default:
            break;
        }
    }
}