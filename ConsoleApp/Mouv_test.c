/*--------------------------------------------------------------------------------
-- HEIG-VD, institute REDS, 1400 Yverdon-les-Bains
-- Project : Exemple description décodeur etat futur MSS
-- File    : Mouv.c
-- Author   : Alexandre Moore & Tenkeu Franklin
-- Date    : 27.04.2026
--------------------------------------------------------------------------------*/
#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "Mouv_test.h"
#include  "Client.h"
#include "protocole.h"

unsigned char compute_checksum(Message* msg) {
    // Création d'un buffer temporaire pour la sérialisation
    // Taille : type(1) + number(1) + length(1) + data(msg->length)
    unsigned char buffer[103];
    int index = 0;
    unsigned char checksum = 0;

    //Sérialisation des champs dans le buffer
    buffer[index++] = msg->type;
    buffer[index++] = msg->number;
    buffer[index++] = msg->length;

    //Copie uniquement des données réellement utilisées
    for (int i = 0; i < msg->length; i++) {
        buffer[index++] = msg->data[i];
    }

    //Calcul du checksum sur le contenu du buffer
    // On boucle jusqu'à 'index' qui représente la taille réelle du message sans le checksum
    for (int i = 0; i < index; i++) {
        checksum += buffer[i];
    }

    // Note : Si ton protocole demande le complément à deux (fréquent en industrie), 
    // utilise : return (unsigned char)(256 - checksum);
    return checksum;
}

int convoyeur_on() {
    msg.type = COM_CONVEYOR;
    msg.number = 0;
    msg.data[0] = 1;
    msg.length = 1;
    msg.checksum = compute_checksum(&msg);
    if (SendAndRecieveMessage(&msg, &response) == 0) {
        if (response.type == 0x10) { // REP_ERROR
            handle_other_responses(&response);
            return -2; // Erreur spécifique pour une réponse d'erreur du serveur
        }
        rep_convoyeur(&response);
        return 0; // Succès
    }
    return -1; // Erreur
}

int convoyeur_off() {
    msg.type = COM_CONVEYOR;
    msg.number = 0;
    msg.data[0] = 0;
    msg.length = 1; // length est 1 car on envoie 1 octet
    msg.checksum = compute_checksum(&msg);

    if (SendAndRecieveMessage(&msg, &response) == 0) {
        if (response.type == 0x10) { // REP_ERROR
            handle_other_responses(&response);
            return -2; // Erreur spécifique pour une réponse d'erreur du serveur
        }
        rep_convoyeur(&response);
        return 0; // Succès
    }
    return -1; // Erreur
}

void rep_convoyeur(Message* Response) {
    unsigned char checksum = compute_checksum(Response);
    if (checksum != Response->checksum) {
        printf("Checksum invalide pour la réponse du convoyeur.\n");
    }
    else {
        if (Response->type == REP_CONVEYOR && Response->length == 1) {
            if (Response->data[0] == 1) {
                printf("Convoyeur is currently ON.\n");
            }
            else {
                printf("Convoyeur is currently OFF.\n");
            }
        }
        else {
            printf("Type de message inattendu pour la réponse du convoyeur.\n");
        }
        return; // Checksum valide
    }

}

void rep_vacumm(Message* Response) {
    unsigned char checksum = compute_checksum(Response);
    if (checksum != Response->checksum) {
        printf("Checksum invalide pour la réponse du vacuum.\n");
        return; // Checksum invalide
    }
    else {
        if (Response->type == REP_SET_VACUUM && Response->length == 1) {
            if (Response->data[0] == 1) {
                printf("Vacuum is currently ON.\n");
            }
            else {
                printf("Vacuum is currently OFF.\n");
            }
        }
        else {
            printf("Type de message inattendu pour la réponse du vacuum.\n");
        }
        return; // Checksum valide
    }
}

int set_vacuum(int state) {
    msg.type = COM_SET_VACUUM;
    msg.number = 0;
    msg.data[0] = state; // 1 pour allumer, 0 pour éteindre
    msg.length = 1; // length est 1 car on envoie 1 octet
    msg.checksum = compute_checksum(&msg);

    if (SendAndRecieveMessage(&msg, &response) == 0) {
        rep_vacumm(&response);
        return 0; // Succès
    }
    return -1; // Erreur
}


int vacuum_on() {
    if (set_vacuum(1) == 0) {
        return 0; // Succès
    }
    return -1; // Erreur
}

int vacuum_off() {
    if (set_vacuum(0) == 0) {
        return 0; // Succès
    }
    return -1; // Erreur    
}

void manual_pilotage(int choice) {
    // Simple manual control: move to predefined positions
    switch (choice) {
    case 1:
        convoyeur_on();
        rep_convoyeur(&response);
        break;
    case 2:
        convoyeur_off();
        rep_convoyeur(&response);
        break;
    case 3:
    {
        int status = -1;
        status = get_has_piece();
        switch (status) {
        case -1:
            printf("Contenu message corrompu.\n");
            break;
        case 0:
            printf("Palette abscente.\n");
            break;
        case 1:
            printf("Palette presente.\n");
            break;
        case 2:
            printf("Erreur du serveur lors de la vérification du capteur de palette.\n");
            break;
        default:
            printf("Erreur à signaler.\n");
        }
    }
    break;
    case 4:
        vacuum_on();
        rep_vacumm(&response);
        break;
    case 5:
        vacuum_off();
        rep_vacumm(&response);
        break;
    case 6:
        printf("Presence piece: %d\n", get_has_piece());
        break;
    case 7:
        robot_move(250000, -300000, 150000, 180000000, 0, 0);
        break;
    case 8:
        robot_move(426500, -191600, 39300, 195000000, 18000000, -10000000);
        break;
    case 9:
        robot_move(155000, -497000, -50000, 180000000, 0, 0);
        break;
    case 10:
        printf("Robot en mouvement: %d\n", robot_is_moving());
        break;
    case 99:
        printf("Retour au menu principal.\n");
        break;
    default:
        break;
    }
    return;
}

void automatic_pilotage(int n_pieces) {
    for (int i = 0; i < n_pieces; i++) {
        PilotState state = PS_CONVEYOR_ON;
        int attempts = 0;
        const int MAX_ATTEMPTS_SHORT = 200; // ~20s
        const int MAX_ATTEMPTS_LONG = 1000; // ~100s

        while (state != PS_DONE) {
            switch (state) {
            case PS_CONVEYOR_ON:
                convoyeur_on();
                attempts = 0;
                state = PS_WAIT_FOR_PALLET;
                break;

            case PS_WAIT_FOR_PALLET:
                if (get_pallet_sensor() == 1) {
                    // pallet in position => stop conveyor and continue
                    convoyeur_off();
                    state = PS_MOVE_TO_PICK;
                    attempts = 0;
                }
                else {
                    Sleep(100);
                    if (++attempts > MAX_ATTEMPTS_SHORT) {
                        printf("Timeout waiting for pallet. Aborting piece %d.\n", i + 1);
                        state = PS_DONE;
                    }
                }
                break;

            case PS_MOVE_TO_PICK:
                // coordonnées Pick (identiques au menu manuel)
                robot_move(426500, -191600, 39300, 195000000, 18000000, -10000000);
                attempts = 0;
                state = PS_WAIT_MOVE_TO_PICK;
                break;

            case PS_WAIT_MOVE_TO_PICK:
                // attendre que le robot termine son mouvement
                if (robot_is_moving() == 0) {
                    state = PS_VACUUM_ON;
                    attempts = 0;
                }
                else {
                    Sleep(100);
                    if (++attempts > MAX_ATTEMPTS_LONG) {
                        printf("Timeout waiting robot move to pick. Aborting piece %d.\n", i + 1);
                        state = PS_DONE;
                    }
                }
                break;

            case PS_VACUUM_ON:
                vacuum_on();
                attempts = 0;
                state = PS_WAIT_VACUUM_ON;
                break;

            case PS_WAIT_VACUUM_ON:
                // condition de réussite donnée par le serveur : has_piece == 1
                if (get_has_piece() == 1) {
                    state = PS_MOVE_TO_PLACE;
                    attempts = 0;
                }
                else {
                    Sleep(100);
                    if (++attempts > MAX_ATTEMPTS_SHORT) {
                        printf("Timeout waiting piece pickup. Aborting piece %d.\n", i + 1);
                        state = PS_DONE;
                    }
                }
                break;

            case PS_MOVE_TO_PLACE:
                // coordonnées Place (identiques au menu manuel)
                robot_move(155000, -497000, -50000, 180000000, 0, 0);
                attempts = 0;
                state = PS_WAIT_MOVE_TO_PLACE;
                break;

            case PS_WAIT_MOVE_TO_PLACE:
                if (robot_is_moving() == 0) {
                    state = PS_VACUUM_OFF;
                    attempts = 0;
                }
                else {
                    Sleep(100);
                    if (++attempts > MAX_ATTEMPTS_LONG) {
                        printf("Timeout waiting robot move to place. Aborting piece %d.\n", i + 1);
                        state = PS_DONE;
                    }
                }
                break;

            case PS_VACUUM_OFF:
                vacuum_off();
                attempts = 0;
                state = PS_WAIT_VACUUM_OFF;
                break;

            case PS_WAIT_VACUUM_OFF:
                // condition de réussite donnée par le serveur : has_piece == 0
                if (get_has_piece() == 0) {
                    printf("Piece %d deposee.\n", i + 1);
                    state = PS_DONE;
                }
                else {
                    Sleep(100);
                    if (++attempts > MAX_ATTEMPTS_SHORT) {
                        printf("Timeout waiting piece release. Aborting piece %d.\n", i + 1);
                        state = PS_DONE;
                    }
                }
                break;

            default:
                state = PS_DONE;
                break;
            }
        } // end while state machine    
    }
}

int robot_is_moving() {
    msg.type = COM_ROBOT_IS_MOVING;
    msg.number = 0;
    msg.length = 0;
    msg.checksum = compute_checksum(&msg);
    if (SendAndRecieveMessage(&msg, &response) == 0) {
        unsigned char checksum = compute_checksum(&response);
        if (checksum != response.checksum) {
            printf("Checksum invalide pour la réponse du robot_is_moving.\n");
            return -1; // Checksum invalide
        }
        if (response.type == REP_ROBOT_IS_MOVING && response.length == 1) {
            if (response.data[0] == 1) {
                printf("Robot is currently moving.\n");
            }
            else {
                printf("Robot is currently stationary.\n");
            }
            return response.data[0]; // 1 if moving, 0 if not
        }
        if (response.type == REP_ERROR) {
            printf("Erreur du serveur lors de la vérification du mouvement du robot.\n");
            return -2; // Erreur spécifique pour une réponse d'erreur du serveur
        }
        return -3; // Error pour type de message inattendu
    }
    return -1; // Erreur de communication
}

int get_pallet_sensor() {
    msg.type = COM_PALLET_SENSOR;
    msg.number = 0;
    msg.length = 0;
    msg.checksum = compute_checksum(&msg);
    msg.data[0] = 0; // Pas de données à envoyer, mais on peut mettre 0 pour la cohérence
    SendAndRecieveMessage(&msg, &response);
    unsigned char checksum = compute_checksum(&response);
    if (checksum != response.checksum) {
        return -1; // Checksum invalide
    }

    if (response.type == REP_PALLET_SENSOR && response.length == 1) {
        return response.data[0]; // 1 if pallet detected, 0 if not
    }

    if (response.type == REP_ERROR) {
        return -2; // Erreur spécifique pour une réponse d'erreur du serveur
    }

    return -3; // Error
}

int get_has_piece() {
    msg.type = COM_GET_HAS_PIECE;
    msg.number = 0;
    msg.length = 0;
    msg.data[0] = 0; // Pas de données à envoyer, mais on peut mettre 0 pour la cohérence
    msg.checksum = compute_checksum(&msg);

    if (SendAndRecieveMessage(&msg, &response) != 0) {
        return -4; // Erreur de communication
    }

    unsigned char checksum = compute_checksum(&response);

    if (checksum != response.checksum) {
        return -1; // Checksum invalide
    }

    if (response.type == REP_GET_HAS_PIECE && response.length == 1) {
        if (response.data[0] == 1) {
            printf("Has piece: Yes\n");
        }
        else {
            printf("Has piece: No\n");
        }
        return response.data[0]; // 1 if has piece, 0 if not
    }

    if (response.type == REP_ERROR) {
        printf("Erreur du serveur lors de la vérification de la présence de pièce.\n");
        return -2; // Erreur spécifique pour une réponse d'erreur du serveur
    }

    return -3; // Error
}

int robot_move(int x, int y, int z, int rx, int ry, int rz) {
    msg.type = COM_ROBOT_MOVE;
    msg.number = 0;
    msg.length = 24; // 6 coordonnées * 4 octets chacune
    int coords[6] = { x, y, z, rx, ry, rz };
    for (int i = 0; i < 6; i++) {
        // Conversion en Big Endian avant l'envoi
        int bigEndianCoord = htonl(coords[i]);
        memcpy(msg.data + (i * 4), &bigEndianCoord, 4);
    }
    msg.checksum = compute_checksum(&msg);

    if (SendAndRecieveMessage(&msg, &response) == 0) {
        rep_robot_move(&response); // Analyse de la réponse
        return 0; // Succès
    }
    return -1; // Erreur
}

void rep_robot_move(Message* Response) {
    unsigned char checksum = compute_checksum(Response);
    if (checksum != Response->checksum) {
        printf("Checksum invalide pour la reponse du mouvement du robot.\n");
        return; // Checksum invalide
    }

    if (Response->type == REP_ROBOT_MOVE && Response->length == 1) {
        if (Response->data[0] == 1) {
            printf("Robot move command executed successfully.\n");
        }
        else {
            printf("Robot move command failed.\n");
        }
    }
    else {
        printf("Type de message inattendu pour la réponse du mouvement du robot.\n");
    }
}

int presencesim(const char* new_ip) {
    msg.type = COM_PRESENCE;
    msg.number = 0;
    msg.length = 0;
    msg.data[0] = 0; // Pas de données à envoyer, mais on peut mettre 0 pour la cohérence
    msg.checksum = compute_checksum(&msg);

    struct sockaddr_in broadcastAddr;
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, new_ip, &broadcastAddr.sin_addr);

    // On envoie à l'adresse de diffusion, pas à l'IP habituelle
    int sendResult = sendto(socket_client, (const char*)&msg, 4, 0,
        (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

    // Ensuite, tu attends la réponse normalement
    if (sendResult != SOCKET_ERROR) {
        return SendAndRecieveMessage(&msg, &response);
    }
    return -1;
}



void print_presence(Message* Response, const char* new_ip) {
    printf("Adresse de diffusion: %s\n", new_ip);
    printf("****************************************************************\n");
    printf("******************Selection d'une machine***********************\n");
    printf("****************************************************************\n");

}

int rep_presence(Message* Response) {
    unsigned char checksum = compute_checksum(Response);
    if (checksum != Response->checksum) {
        printf("Checksum invalide pour la réponse de presence.\n");
        return -1; // Checksum invalide
    }
    else {
        if (Response->type == REP_PRESENCE && Response->length == 1) {
            if (Response->data[0] == 1) {
                printf("Presence detected.\n");
            }
            else {
                printf("No presence detected.\n");
            }
            return Response->data[0]; // 1 if presence detected, 0 if not
        }
        else {
            printf("Type de message inattendu pour la réponse de presence.\n");
        }
        return -1; // Checksum valide mais type de message inattendu
    }
}

void handle_other_responses(Message* response) {
    switch (response->type) {
    case REP_ERROR: // 0x10
    {
        char messageStr[101] = { 0 };
        memcpy(messageStr, response->data, response->length);
        for (int i = 0; messageStr[i]; i++) {
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
    }
    case REP_INFO:
        handle_rep_info(response);
        break;
    case NULL:
        printf("Aucune réponse du serveur.\n");
    default:
        break;
    }
}

// Variable globale pour suivre le nombre de pièces traitées
int total_palettes = 0;

void handle_rep_info(Message* info) {
    if (info->type != REP_INFO) return;

    // Vérification du checksum pour être sûr que la donnée est fiable
    unsigned char cks = compute_checksum(info);
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

void localisation_machine(const char* diff_ip) {
    count = 0;
    if (!is_valid_ip(diff_ip)) {
        printf("Adresse IP invalide : %s\n", diff_ip);
        return;
    }

    struct sockaddr_in destAddr;
    memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, diff_ip, &destAddr.sin_addr);

    Message presence_msg = { 0 };
    presence_msg.type = COM_PRESENCE;
    presence_msg.number = 0;
    presence_msg.length = 0;
    presence_msg.checksum = compute_checksum(&presence_msg);

    unsigned char send_buffer[4] = {
        presence_msg.type,
        presence_msg.number,
        presence_msg.length,
        presence_msg.checksum
    };

    int broadcastEnabled = 1;
    setsockopt(socket_client, SOL_SOCKET, SO_BROADCAST,
        (const char*)&broadcastEnabled, sizeof(broadcastEnabled));

    int sendResult = sendto(socket_client, (const char*)send_buffer, sizeof(send_buffer), 0,
        (struct sockaddr*)&destAddr, sizeof(destAddr));
    if (sendResult == SOCKET_ERROR) {
        printf("Erreur lors de l'envoi de la diffusion : %d\n", WSAGetLastError());
        return;
    }

    printf("\nAdresse de diffusion : %s\n", diff_ip);
    printf("**********************************************\n");
    printf("****** Selection d'une machine ***************\n");
    printf("**********************************************\n");

    struct sockaddr_in fromAddr;
    unsigned char recv_buffer[104];

    // On écoute pendant un court laps de temps (le timeout du socket gère l'arrêt)
    while (1) {
        int fromSize = sizeof(fromAddr);
        int recvResult = recvfrom(socket_client, (char*)recv_buffer, sizeof(recv_buffer), 0,
            (struct sockaddr*)&fromAddr, &fromSize);

        if (recvResult == SOCKET_ERROR) {
            break;
        }
        if (recvResult < 4) {
            continue;
        }

        Message presence_response = { 0 };
        presence_response.type = recv_buffer[0];
        presence_response.number = recv_buffer[1];
        presence_response.length = recv_buffer[2];

        if (presence_response.length > 100 || recvResult != 4 + presence_response.length) {
            continue;
        }

        memcpy(presence_response.data, &recv_buffer[3], presence_response.length);
        presence_response.checksum = recv_buffer[3 + presence_response.length];

        if (compute_checksum(&presence_response) != presence_response.checksum) {
            continue;
        }

        response = presence_response;
        if (response.type == REP_PRESENCE) {
            if (count >= 10) {
                continue;
            }
            count++;
            char ip_str[16];
            inet_ntop(AF_INET, &(fromAddr.sin_addr), ip_str, sizeof(ip_str));

            // On mémorise l'IP pour l'option de sélection
            strncpy(machines_trouvees[count - 1], ip_str, 15);
            machines_trouvees[count - 1][15] = '\0';

            // On récupère le nom dans data (ex: "francois")
            char nom[21] = { 0 };
            if (response.length > 1 && (response.data[0] == 0 || response.data[0] == 1)) {
                int nom_length = response.length - 1;
                if (nom_length > 20) nom_length = 20;
                memcpy(nom, &response.data[1], nom_length);
            }
            else if (response.length > 1 || (response.length == 1 && response.data[0] > 1)) {
                int nom_length = response.length;
                if (nom_length > 20) nom_length = 20;
                memcpy(nom, response.data, nom_length);
            }

            if (nom[0] != '\0') {
                printf("  %d - %s - %s\n", count, ip_str, nom);
            }
            else {
                printf("  %d - %s\n", count, ip_str);
            }
        }
    }
    printf("  0 - Revenir au menu principal\n");
    printf("\n");
    printf("Choix > ");
}
