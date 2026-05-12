/*--------------------------------------------------------------------------------
-- HEIG-VD, institute REDS, 1400 Yverdon-les-Bains
-- Project : Exemple description décodeur etat futur MSS
-- File    : Mouv.c
-- Author   : Alexandre Moore & Tenkeu Franklin 
-- Date    : 27.04.2026
--------------------------------------------------------------------------------*/
#include "Mouv.h"
#include  "Client.h"

Message msg = {0};
Message response = {0};

compute_checksum(Message* msg){
    unsigned char checksum = 0;
    checksum += msg->type;
    checksum += msg->number;
    checksum += msg->length;
    for (int i = 0; i < msg->length; i++) {
        checksum += msg->data[i];
    }
    return checksum;
}

convoyeur_on(SOCKET sock, struct sockaddr_in* addr) {
    msg.type = COM_CONVEYOR;
    msg.number = 0;
    msg.data[0] = 1; 
    msg.length = 1;
    msg.checksum = compute_checksum(&msg);
    if (SendAndRecieveMessage(&msg, &response, SERVER_IP) == 0){
        if (response.type == 0x10) { // REP_ERROR
            handle_other_responses(&response);
            return -2; // Erreur spécifique pour une réponse d'erreur du serveur
        }
        rep_convoyeur(&response);
        return 0; // Succès
    }
    return -1; // Erreur
}

convoyeur_off(SOCKET sock, struct sockaddr_in* addr){
    msg.type = COM_CONVEYOR;
    msg.number = 0;
    msg.data[0] = 0;
    msg.length = 1; // length est 1 car on envoie 1 octet
    msg.checksum = compute_checksum(&msg);
    
    if (SendAndRecieveMessage(&msg, &response, SERVER_IP) == 0){
        if (response.type == 0x10) { // REP_ERROR
            handle_other_responses(&response);
            return -2; // Erreur spécifique pour une réponse d'erreur du serveur
        }
        rep_convoyeur(&response);
        return 0; // Succès
    }
    return -1; // Erreur
}

rep_convoyeur(Message* Response){
    unsigned char checksum = compute_checksum((unsigned char*)Response);
    if(checksum != Response->checksum){
        printf("Checksum invalide pour la réponse du convoyeur.\n");
    }
    else{
        if (Response->type == REP_CONVEYOR && Response->length == 1){
            if(Response->data[0] == 1){
                printf("Convoyeur is currently ON.\n");
            } else{
                printf("Convoyeur is currently OFF.\n");
            }
        } else{
            printf("Type de message inattendu pour la réponse du convoyeur.\n");
        }
        return; // Checksum valide
    }
        
}

rep_vacumm(Message* Response){
    unsigned char checksum = compute_checksum((unsigned char*)Response);
    if(checksum != Response->checksum){
        printf("Checksum invalide pour la réponse du vacuum.\n");
        return; // Checksum invalide
    }
    else{
        if(Response->type == REP_SET_VACUUM && Response->length == 1){
            if(Response->data[0] == 1){
                printf("Vacuum is currently ON.\n");
            }
            else{
                printf("Vacuum is currently OFF.\n");
            }
        }
        else{
            printf("Type de message inattendu pour la réponse du vacuum.\n");
        }
        return; // Checksum valide
    }
}

set_vacuum(SOCKET sock, struct sockaddr_in* addr, int state){
    msg.type = COM_SET_VACUUM;
    msg.number = 0;
    msg.data[0] = state; // 1 pour allumer, 0 pour éteindre
    msg.length = 1; // length est 1 car on envoie 1 octet
    msg.checksum = compute_checksum(&msg);

    if (SendAndRecieveMessage(&msg, &response, SERVER_IP) == 0){
        rep_vacumm(&response);
        return 0; // Succès
    }
    return -1; // Erreur
}


vacuum_on(SOCKET sock, struct sockaddr_in* addr){
    if(set_vacuum(sock, addr, 1) == 0){
        return 0; // Succès
    }
    return -1; // Erreur
}

vacuum_off(SOCKET sock, struct sockaddr_in* addr){
    if(set_vacuum(sock, addr, 0) == 0){
        return 0; // Succès
    }
    return -1; // Erreur    
}

manual_menu(){
    printf("Commandes manuelles ---------------------------\n");
    printf("1. Allumer le convoyeur\n");
    printf("2. Arreter le convoyeur\n");
    printf("3. Afficher l'etat du capteur de palette\n");
    printf("4. Activer le vacuum\n");
    printf("5. Desactiver le vacuum\n");
    printf("6. Afficher l'etat presence piece\n");
    printf("7. Deplacer le robot en position Home\n");
    printf("8. Deplacer le robot en position Pick\n");
    printf("9. Deplacer le robot en position Place\n");
    printf("10. Afficher si le robot est en mouvement\n");
    printf("99. Quitter les fonctions manuelles\n");
    printf("Choix>");
    int choix;
    // Saisie du choix de l'utilisateur
    if (scanf("%d", &choix) != 1){
        return -1; // Erreur de saisie
        printf("Choix invalide. Veuillez entrer un nombre.\n");
    }
    return choix;
}

manual_pilotage(SOCKET sock, struct sockaddr_in* addr){
    // Simple manual control: move to predefined positions
    int choice;
    choice = manual_menu();
    switch (choice){
        case 1:
            convoyeur_on(sock, addr);
            break;
        case 2:
            convoyeur_off(sock, addr);
            break;
        case 3:
            printf("Capteur de palette: %d\n", get_pallet_sensor(sock, addr));
            break;
        case 4:
            vacuum_on(sock, addr);
            break;
        case 5:
            vacuum_off(sock, addr);
            break;
        case 6:
            printf("Presence piece: %d\n", get_has_piece(sock, addr));
            break;
        case 7:
            robot_move(sock, addr, 250000, -300000, 150000, 180000000, 0, 0);
            break;
        case 8:
            robot_move(sock, addr, 426500, -191600, 39300, 195000000, 18000000, -10000000);
            break;
        case 9:
            robot_move(sock, addr, 155000, -497000, -50000, 180000000, 0, 0);
            break;
        case 10:
            printf("Robot en mouvement: %d\n", robot_is_moving(sock, addr));
            break;
        case 99:
            printf("Retour au menu principal.\n");
            break;
        default:
            printf("Choix invalide. Veuillez entrer un nombre valide.\n");
            break;
    }
    return;
}

automatic_pilotage(SOCKET sock, struct sockaddr_in* addr, int n_pieces){
    for (int i = 0; i < n_pieces; i++)
    {
        // Move to pick
        robot_move(sock, addr, 426500, -191600, 39300, 195000000, 18000000, -10000000);
        while (robot_is_moving(sock, addr)) {} // Wait
        vacuum_on(sock, addr);
        // Move to place
        robot_move(sock, addr, 155000, -497000, -50000, 180000000, 0, 0);
        while (robot_is_moving(sock, addr)) {} // Wait
        vacuum_off(sock, addr);
    }
}

robot_is_moving(SOCKET sock, struct sockaddr_in* addr){
    msg.type = COM_ROBOT_IS_MOVING;
    msg.number = 0;
    msg.length = 0;
    msg.checksum = compute_checksum(&msg);
    if (SendAndRecieveMessage(&msg, &response, SERVER_IP) == 0){
        unsigned char checksum = compute_checksum((unsigned char*)&response);
        if (checksum != response.checksum){
            printf("Checksum invalide pour la réponse du robot_is_moving.\n");
            return -1; // Checksum invalide
        }
        if (response.type == REP_ROBOT_IS_MOVING && response.length == 1){
            if(response.data[0] == 1){
                printf("Robot is currently moving.\n");
            } else{
                printf("Robot is currently stationary.\n");
            }
            return response.data[0]; // 1 if moving, 0 if not
        }
        if (response.type == REP_ERROR){
            printf("Erreur du serveur lors de la vérification du mouvement du robot.\n");
            return -2; // Erreur spécifique pour une réponse d'erreur du serveur
        }
        return -3; // Error pour type de message inattendu
    }
    return -1; // Erreur de communication
}

get_pallet_sensor(SOCKET sock, struct sockaddr_in* addr){
    msg.type = COM_PALLET_SENSOR;
    msg.number = 0;
    msg.length = 0;
    msg.checksum = compute_checksum(&msg);
    msg.data[0] = 0; // Pas de données à envoyer, mais on peut mettre 0 pour la cohérence
    SendAndRecieveMessage(&msg, &response, SERVER_IP);
    unsigned char checksum = compute_checksum((unsigned char*)&response);
    if (checksum != response.checksum){
        return -1; // Checksum invalide
    }

    if (response.type == REP_PALLET_SENSOR && response.length == 1){
        printf("Pallet sensor state: %d\n", response.data[0]);
        return response.data[0]; // 1 if pallet detected, 0 if not
    }

    if(response.type == REP_ERROR){
        printf("Erreur du serveur lors de la vérification du capteur de palette.\n");
        return -2; // Erreur spécifique pour une réponse d'erreur du serveur
    }

    return -3; // Error
}

get_has_piece(SOCKET sock, struct sockaddr_in* addr){
    msg.type = COM_GET_HAS_PIECE;
    msg.number = 0;
    msg.length = 0;
    msg.data[0] = 0; // Pas de données à envoyer, mais on peut mettre 0 pour la cohérence
    msg.checksum = compute_checksum(&msg);

    if(SendAndRecieveMessage(&msg, &response, SERVER_IP) != 0){
        return -4; // Erreur de communication
    }

    unsigned char checksum = compute_checksum((unsigned char*)&response);

    if (checksum != response.checksum){
        return -1; // Checksum invalide
    }

    if (response.type == REP_GET_HAS_PIECE && response.length == 1){
        if(response.data[0] == 1){
            printf("Has piece: Yes\n");
        } else{
            printf("Has piece: No\n");
        }
        return response.data[0]; // 1 if has piece, 0 if not
    }

    if(response.type == REP_ERROR){
        printf("Erreur du serveur lors de la vérification de la présence de pièce.\n");
        return -2; // Erreur spécifique pour une réponse d'erreur du serveur
    }

    return -3; // Error
}

robot_move(SOCKET sock, struct sockaddr_in* addr, int x, int y, int z, int rx, int ry, int rz) {
    msg.type = COM_ROBOT_MOVE;
    msg.number = 0;
    msg.length = 24; // 6 coordonnées * 4 octets chacune
    int coords[6] = {x, y, z, rx, ry, rz};
    for(int i = 0; i < 6; i++){
        // Conversion en Big Endian avant l'envoi
        int bigEndianCoord = htonl(coords[i]);
        memcpy(msg.data + (i * 4), &bigEndianCoord, 4);
    }
    msg.checksum = compute_checksum(&msg);

    if (SendAndRecieveMessage(&msg, &response, SERVER_IP) == 0){
        rep_robot_move(&response); // Analyse de la réponse
        return 0; // Succès
    }
    return -1; // Erreur
}

rep_robot_move(Message* Response){
    unsigned char checksum = compute_checksum((unsigned char*)Response);
    if(checksum != Response->checksum){
        printf("Checksum invalide pour la réponse du mouvement du robot.\n");
        return; // Checksum invalide
    }

    if (Response->type == REP_ROBOT_MOVE && Response->length == 1){
        if(Response->data[0] == 1) {
            printf("Robot move command executed successfully.\n");
        } else {
            printf("Robot move command failed.\n");
        }
    } else{
        printf("Type de message inattendu pour la réponse du mouvement du robot.\n");
    }
}

presencesim(SOCKET sock, struct sockaddr_in* addr){
    msg.type = COM_PRESENCE;
    msg.number = 0;
    msg.length = 0;
    msg.data[0] = 0; // Pas de données à envoyer, mais on peut mettre 0 pour la cohérence
    msg.checksum = compute_checksum(&msg);

    if (SendAndRecieveMessage(&msg, &response, SERVER_IP) == 0){
        return rep_presence(&response);
    }
    return -1; // Error
}

rep_presence(Message* Response){
    unsigned char checksum = compute_checksum((unsigned char*)Response);
    if(checksum != Response->checksum){
        printf("Checksum invalide pour la réponse de presence.\n");
        return -1; // Checksum invalide
    }
    else{
        if(Response->type == REP_PRESENCE && Response->length == 1){
            if(Response->data[0] == 1){
                printf("Presence detected.\n");
            }
            else{
                printf("No presence detected.\n");
            }
            return Response->data[0]; // 1 if presence detected, 0 if not
        }
        else{
            printf("Type de message inattendu pour la réponse de presence.\n");
        }
        return -1; // Checksum valide mais type de message inattendu
    }
}

handle_other_responses(Message* response){
    switch (response->type)
    {
    case REP_ERROR: // 0x10
        char messageStr[101] = {0};
        memcpy(messageStr, response->data, response->length);
        for(int i = 0; messageStr[i]; i++) {
            messageStr[i] = tolower((unsigned char)messageStr[i]); // Convertir en minuscules pour faciliter la recherche de mots-clés
        }
        printf("\n[!] ECHEC DE LA COMMANDE\n");
        if (strstr(messageStr, "type") != NULL || strstr(messageStr, "command") != NULL) {
            printf("Cause : L'ID de la commande est inconnu du serveur.\n");
        } 
        else if (strstr(messageStr, "length") != NULL || strstr(messageStr, "Size") != NULL) {
            printf("Cause : La taille des donnees envoyees est incorrecte.\n");
        }
        else if (strstr(messageStr, "checksum") != NULL) {
            printf("Cause : Erreur de transmission (Checksum invalide).\n");
        }

        printf("Message brut du serveur : %s\n", messageStr);
        break;
    case REP_INFO:
        handle_rep_info(response);
        break;
    default:
        break;
    }
}

// Variable globale pour suivre le nombre de pièces traitées
int total_palettes = 0;

void handle_rep_info(Message* info) {
    if (info->type != REP_INFO) return;

    // Vérification du checksum pour être sûr que la donnée est fiable
    unsigned char cks = calculateChecksum(info);
    if (cks != info->checksum) {
        printf("[REP_INFO] Erreur de checksum, donnée ignorée.\n");
        return;
    }

    // Le simulateur envoie souvent le compteur sur 1 ou 4 octets
    if (info->length == 1) {
        total_palettes = (int)info->data[0];
    } 
    else if (info->length == 4) {
        // Si c'est un entier 32-bit (Big Endian)
        int count;
        memcpy(&count, info->data, 4);
        total_palettes = ntohl(count); // Convertir du réseau vers l'hôte
    }

    printf("\n>>> [MESSAGE INFO] Palette detectee ! Total traite : %d <<<\n", total_palettes);
}