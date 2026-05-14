#pragma once
#include "Mouv_test.h"
#include "GestionMenu.h"
#include "Client.h"

int main(){
    int choix;
    if (OpenConnection(SERVER_IP) !=0){
        printf("Erreur d'ouverture de connexion.\n");
        return 1;
    }
    
    while (1){
        choix = menu_principal();
        switch (choix){
        case 1:
            int choice = manual_menu();
            manual_pilotage(choice);
            break;
        case 2:
            printf("Combien de pieces a assembler ? ");
            int n_pieces;
            if (scanf("%d", &n_pieces) != 1 || n_pieces <= 0){
                printf("Nombre invalide. Veuillez entrer un nombre entier positif.\n");
                break;
            }
            automatic_pilotage(n_pieces);
            break;
        case 3:
            handle_other_responses(&response);
            break;
        case 4:
            printf("Saisie de l'adresse de la machine non implémentée.\n");
            char new_ip[16];
            printf("Entrez la nouvelle adresse IP du serveur : ");
            scanf("%15s", new_ip);
            SetServerIP(new_ip);
            break;
        case 5:
            printf("Detection par des machines par diffusion.\n");
            char diff_ip;
            printf("Entrez l'adresse de difusion: ");
            scanf("%15s", diff_ip);
            localisation_machine(diff_ip);
            int choice1 = -1;
            scanf("%d", &choice1);  
            if(count == 0){
                while (choice1 != 0){
                    printf("Entrée Incorrecte, reessayez encore: ");
                    scanf("%d", &choice1); 
                }
                choix = menu_principal();
            }
            else{
                if (choice1 > count){
                    while (choice1 > count || choice1 < 0){
                        printf("Entrée Incorrecte, reessayez encore: ");
                        scanf("%d", &choice1);
                    }
                    if(count == 0){
                        choix = menu_principal();
                    }
                    else{
                        strncpy(SERVER_IP, machines_trouvees[choice1-1], 15);
                        SERVER_IP[15] = '\0'; // Sécurité
                        printf("Adresse IP mise à jour.\n");
                    }
                }
            }
            break;
        case 9:
            printf("Quitter le programme.\n");
            CloseConnection();
            return 0;
        default:
            printf("Choix invalide. Veuillez entrer un nombre valide.\n");
            break;
        }
    }
}