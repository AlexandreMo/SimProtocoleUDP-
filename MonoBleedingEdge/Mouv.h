/*--------------------------------------------------------------------------------
-- HEIG-VD, bureau iai, 1400 Yverdon-les-Bains
-- Project : Header file for robot control functions
-- File    : Mouv.c
-- Author   : Alexandre Moore & Tenkeu Franklin 
-- Date    : 27.04.2026
--------------------------------------------------------------------------------*/
#ifndef MOUV_H
#define MOUV_H

#include <winsock2.h>
#include <stdio.h>
#include <string.h>
// ===============================
// Constantes protocole
// ===============================
typedef enum
{
REP_ERROR = 0x10,
REP_INFO = 0x20,

COM_CONVEYOR = 0x30,
REP_CONVEYOR = 0x31,

COM_PALLET_SENSOR = 0x32,
REP_PALLET_SENSOR = 0x33,

COM_SET_VACUUM = 0x34,
REP_SET_VACUUM = 0x35,

COM_GET_HAS_PIECE = 0x36,
REP_GET_HAS_PIECE = 0x37,

COM_ROBOT_MOVE = 0x40,
REP_ROBOT_MOVE = 0x41,

COM_ROBOT_IS_MOVING = 0x42,
REP_ROBOT_IS_MOVING = 0x43,

COM_PRESENCE = 0x60,
REP_PRESENCE = 0x61

} MessageType;

// ===============================
// Structure d’un message
// ===============================
#pragma pack(push, 1) // Force l'alignement à 1 octet (pas de trous)
typedef struct{
unsigned char type;
unsigned char number;
unsigned char length;
unsigned char data[100];
unsigned char checksum;
} Message;
#pragma pack(pop) // Fin de l'alignement forcé

// Constantes pilotage manuel
// Position Coordonnées (X, Y, Z, Rx, Ry, Rz)
// Home
// 250000, -300000, 150000, 180000000, 0, 0
// Pick
// 426500, -191600, 39300, 195000000, 18000000, -10000000
// Place
// 155000, -497000, -50000, 180000000, 0, 0
// ===============================
// Fonctions bas niveau (UDP)
// ===============================
unsigned char compute_checksum(unsigned char* buffer, int length);

int send_message(SOCKET sock, struct sockaddr_in* addr, Message* msg);

int receive_message(SOCKET sock, Message* msg);

// ===============================
// Fonctions haut niveau (robot)
// ===============================
void convoyeur_on(SOCKET sock, struct sockaddr_in* addr);
void convoyeur_off(SOCKET sock, struct sockaddr_in* addr);
void rep_convoyeur(Message* Response);

int set_vacuum(SOCKET sock, struct sockaddr_in* addr, int state);
void rep_vacumm(Message* Response);
int vacuum_on(SOCKET sock, struct sockaddr_in* addr);
int vacuum_off(SOCKET sock, struct sockaddr_in* addr);

int robot_move(SOCKET sock, struct sockaddr_in* addr,
int x, int y, int z,
int rx, int ry, int rz);
void rep_robot_move(Message* Response);

int robot_is_moving(SOCKET sock, struct sockaddr_in* addr);

int get_pallet_sensor(SOCKET sock, struct sockaddr_in* addr);
int get_has_piece(SOCKET sock, struct sockaddr_in* addr);

int presencesim(SOCKET sock, struct sockaddr_in* addr);
int rep_presence(Message* Response);

// ===============================
// Fonctions applicatives
// ===============================
// Menu pour la console - renvoie le choix de l'utilisateur ou -1 en cas d'erreur de saisie
int manual_menu(int choice);
void manual_pilotage(SOCKET sock, struct sockaddr_in* addr);

void automatic_pilotage(SOCKET sock, struct sockaddr_in* addr, int n_pieces);

void handle_other_responses(Message* response);

void handle_rep_info(Message* info);

// ===============================
// Détection machine
// ===============================
int machine_detection(SOCKET sock);

int manual_menu();
void manual_pilotage(SOCKET sock, struct sockaddr_in* addr);
void automatic_pilotage(SOCKET sock, struct sockaddr_in* addr, int n_pieces);

#endif
