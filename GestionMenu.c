#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "GestionMenu.h"


int menu_principal(){
    printf("Assemblage de processeurs =====================\n");
    printf("1. Commandes manuelles\n");
    printf("2. Pilotage automatise\n");
    printf("3. Messages d'erreur de communication\n");
    printf("4. Saisie de l'adresse de la machine\n");
    printf("5. Detection par diffusion\n");
    printf("9. Quitter le programme\n");
    printf("Choix>");
    int choi1;
    // Saisie du choix de l'utilisateur
    if (scanf("%d", &choi1) != 1){
        printf("Choix invalide. Veuillez entrer un nombre.\n");
        while (getchar() != '\n');
        return -1; // Erreur de saisie
    }
    return choi1;
}

int menu_pilotage_manuel(){
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
    int choi;
    // Saisie du choix de l'utilisateur
    if (scanf("%d", &choi) != 1){
        printf("Choix invalide. Veuillez entrer un nombre.\n");
        while (getchar() != '\n');
        return -1; // Erreur de saisie        
    }
    return choi;
}