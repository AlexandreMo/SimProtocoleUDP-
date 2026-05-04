/*--------------------------------------------------------------------------------
-- HEIG-VD, institute REDS, 1400 Yverdon-les-Bains
-- Project : Header file for robot control functions
-- File    : Mouv.c
-- Author   : Alexandre Moore & Tenkeu Franklin 
-- Date    : 27.04.2026
--------------------------------------------------------------------------------*/
// Constantes pour les commandes et les messages 
#define REP_ERROR            0x10
#define REP_INFO             0x20

#define COM_CONVEYOR         0x30
#define REP_CONVEYOR         0x31

#define COM_PALLET_SENSOR    0x32
#define REP_PALLET_SENSOR    0x33

#define COM_SET_VACUUM       0x34
#define REP_SET_VACUUM       0x35
#define REP_ERROR            0x10
#define REP_INFO             0x20

#define COM_CONVEYOR         0x30
#define REP_CONVEYOR         0x31

#define COM_PALLET_SENSOR    0x32
#define REP_PALLET_SENSOR    0x33

#define COM_SET_VACUUM       0x34
#define REP_SET_VACUUM       0x35

#define COM_GET_HAS_PIECE    0x36
#define REP_GET_HAS_PIECE    0x37

#define COM_ROBOT_MOVE       0x40
#define REP_ROBOT_MOVE       0x41

#define COM_ROBOT_IS_MOVING  0x42
#define REP_ROBOT_IS_MOVING  0x43

#define COM_PRESENCE         0x60
#define REP_PRESENCE         0x61
// Fonctions de pilotage du robot
void info_robot();
void send_message();
int receive_message();
int compute_checksum();
void convoyeur_on();
void vacuum_on();
void robot_move();
void automatic_pilotage(char n_pieces);
void error_message();
void manual_pilotage();
char machine_detection();
