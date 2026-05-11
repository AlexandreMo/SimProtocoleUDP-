/*--------------------------------------------------------------------------------
-- HEIG-VD, institute REDS, 1400 Yverdon-les-Bains
-- Project : Exemple description décodeur etat futur MSS
-- File    : Mouv.c
-- Author   : Alexandre Moore & Tenkeu Franklin 
-- Date    : 27.04.2026
--------------------------------------------------------------------------------*/
#include "Mouv.h"
unsigned char compute_checksum(unsigned char* buffer, int length)
{
    unsigned char checksum = 0;
    for (int i = 0; i < length; i++)
    {
        checksum += buffer[i];
    }
    return checksum;
}
int send_message(SOCKET sock, struct sockaddr_in* addr, Message* msg)
{
    unsigned char buffer[104];
    buffer[0] = msg->type;
    buffer[1] = msg->number;
    buffer[2] = msg->length;
    memcpy(buffer + 3, msg->data, msg->length);
    buffer[3 + msg->length] = compute_checksum(buffer, 3 + msg->length);
    
    return sendto(sock, (const char*)buffer, 4 + msg->length, 0, (const struct sockaddr*)addr, sizeof(*addr));
}
int receive_message(SOCKET sock, Message* msg)
{
    unsigned char buffer[104];
    int addr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in addr;
    int recv_len = recvfrom(sock, (char*)buffer, 104, 0, (struct sockaddr*)&addr, &addr_len);
    if (recv_len < 4)
    {
        return -1; // Message trop court
    }
    unsigned char checksum = compute_checksum(buffer, recv_len - 1);
    if (checksum != buffer[recv_len - 1])
    {
        return -1; // Checksum invalide
    }
    msg->type = buffer[0];
    msg->number = buffer[1];
    msg->length = buffer[2];
    memcpy(msg->data, buffer + 3, msg->length);
    return recv_len;
}
void convoyeur_on(SOCKET sock, struct sockaddr_in* addr)
{
    Message msg = {COM_CONVEYOR, 0, 0, {0}, 0};
    send_message(sock, addr, &msg);
}
void convoyeur_off(SOCKET sock, struct sockaddr_in* addr)
{
    Message msg = {COM_CONVEYOR, 1, 0, {0}, 0};
    send_message(sock, addr, &msg);
}
void set_vacuum(SOCKET sock, struct sockaddr_in* addr, int on)
{
    Message msg = {COM_SET_VACUUM, on ? 1 : 0, 0, {0}, 0};
    send_message(sock, addr, &msg);
}   
void vacuum_on(SOCKET sock, struct sockaddr_in* addr)
{
    set_vacuum(sock, addr, 1);
}
void vacuum_off(SOCKET sock, struct sockaddr_in* addr)
{
    set_vacuum(sock, addr, 0);
}

int manual_menu()
{
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
    char choix;
    // Saisie du choix de l'utilisateur
    if (scanf("%d", &choix) != 1)
    {
        return -1; // Erreur de saisie
        printf("Choix invalide. Veuillez entrer un nombre.\n");
    }
    return choix;
}
void manual_pilotage(SOCKET sock, struct sockaddr_in* addr)
{
    // Simple manual control: move to predefined positions
    int choice;
    choice = manual_menu();
    switch (choice)
    {
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
}

void automatic_pilotage(SOCKET sock, struct sockaddr_in* addr, int n_pieces)
{
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
