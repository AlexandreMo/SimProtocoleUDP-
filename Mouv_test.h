/*--------------------------------------------------------------------------------
-- HEIG-VD, bureau iai, 1400 Yverdon-les-Bains
-- Project : Header file for robot control functions
-- File    : Mouv.c
-- Author   : Alexandre Moore & Tenkeu Franklin 
-- Date    : 27.04.2026
--------------------------------------------------------------------------------*/
#pragma once
#ifndef MOUV_H
#define MOUV_H

#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include "Client.h"
#include <ctype.h>
// Constantes pilotage manuel
// Position Coordonnées (X, Y, Z, Rx, Ry, Rz)
// Home
// 250000, -300000, 150000, 180000000, 0, 0
// Pick
// 426500, -191600, 39300, 195000000, 18000000, -10000000
// Place
// 155000, -497000, -50000, 180000000, 0, 0

// ===============================
// Fonctions haut niveau (robot)
// ===============================
int convoyeur_on();
int convoyeur_off();
void rep_convoyeur(Message* Response);

int set_vacuum(int state);
void rep_vacumm(Message* Response);
int vacuum_on();
int vacuum_off();

int robot_move(int x, int y, int z,
int rx, int ry, int rz);
void rep_robot_move(Message* Response);

int robot_is_moving();

int get_pallet_sensor();
int get_has_piece();

void localisation_machine(const char* diff_ip);

// ===============================
// Fonctions applicatives
// ===============================
// Menu pour la console - renvoie le choix de l'utilisateur ou -1 en cas d'erreur de saisie
void manual_pilotage(int choice);

void automatic_pilotage(int n_pieces);

void handle_other_responses(Message* response);

void handle_rep_info(Message* info);

// ===============================
// Détection machine
// ===============================

#endif
