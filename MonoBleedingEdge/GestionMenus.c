#pragma once
#include <stdio.h>
#include <string.h>
#include "Client.h"

int MainMenu(){
    printf("=========================Assemblage de processeurs ==========================\n");
    printf("Veuillez choisir une option :\n");
    printf("1. Commandes manuelles\n");
    printf("2. Pilotage automatique\n");
    printf("3. Messages d'erreur\n  ");
    printf("4. Saisie adresse de la machine cible\n");
    printf("5. Détection par diffusion\n");
    printf("9. Quitter\n");
    int choice;
    if (scanf("%d", &choice) != 1){
        printf("Erreur de saisie. Veuillez entrer un nombre.\n");
        while (getchar() != '\n')continue; // Vider le buffer d'entrée
        return -1; // Indiquer une erreur de saisie
    }
    return choice;
}

int manual_menu(){
    printf("=========================Commandes manuelles ==========================\n");
    printf("Veuillez choisir une option :\n");
    printf("1. Allumer le convoyeur\n");
    printf("2. Arreter le convoyeur\n");
    printf("3. Afficher l'etat du capteur de palette\n");
    printf("4. Activer le vacuum\n");
    printf("5. Desactiver le vacuum\n");
    printf("6. Afficher l'etat presence piece\n");
    printf("7. Deplacer le robot a la position Home\n");
    printf("8. Deplacer le robot a la position Pick\n");
    printf("9. Deplacer le robot a la position Place\n");
    printf("10. Afficher l'etat de mouvement du robot\n");
    printf("99. Retour au menu principal\n");
    int choice;
    if (scanf("%d", &choice) != 1){
        printf("Erreur de saisie. Veuillez entrer un nombre.\n");
        while (getchar() != '\n')continue; // Vider le buffer d'entrée
        return -1; // Indiquer une erreur de saisie
    }
    return choice;
}

int automatic_menu(){
    printf("=========================Pilotage automatique ==========================\n");
    printf("Veuillez choisir une option :\n");
    printf("1. Lancer le pilotage automatique pour 5 pieces\n");
    printf("2. Lancer le pilotage automatique pour 10 pieces\n");
    printf("3. Lancer le pilotage automatique pour 20 pieces\n");
    printf("99. Retour au menu principal\n");
    int choice;
    if (scanf("%d", &choice) != 1){
        printf("Erreur de saisie. Veuillez entrer un nombre.\n");
        while (getchar() != '\n')continue; // Vider le buffer d'entrée
        return -1; // Indiquer une erreur de saisie
    }
    return choice;
}

int choixMachineCible(){
    printf("Adresse de diffusion :%s", ADDRESSE_DIFFUSION);
    printf("=========================Selection d'une machine ==========================\n");
    printf("Veuillez choisir la machine à utiliser\n");
    printf("1 - %s - franklin\n", MACHINE1);
    printf("2 - %s - Alexendre\n", MACHINE2);
    printf("0 - Retour au menu principal\n");
    int choice;
    if (scanf("%d", &choice) != 1){
        printf("Erreur de saisie. Veuillez entrer un nombre.\n");
        while (getchar() != '\n')continue; // Vider le buffer d'entrée
        return -1; // Indiquer une erreur de saisie
    }
    return choice;
}