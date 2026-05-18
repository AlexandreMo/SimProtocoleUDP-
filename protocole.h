#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_PORT 27598
#define TIMEOUT 5000 // Timeout en millisecondes

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

// ===============================
// Constantes protocole
// ===============================
typedef enum{
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

typedef enum {
    PS_START = 0,
    PS_CONVEYOR_ON,
    PS_WAIT_FOR_PALLET,
    PS_MOVE_TO_PICK,
    PS_WAIT_MOVE_TO_PICK,
    PS_VACUUM_ON,
    PS_WAIT_VACUUM_ON,
    PS_MOVE_TO_PLACE,
    PS_WAIT_MOVE_TO_PLACE,
    PS_VACUUM_OFF,
    PS_WAIT_VACUUM_OFF,
    PS_DONE
} PilotState;


extern char SERVER_IP[16];
extern SOCKET socket_client;
extern struct sockaddr_in server_Addr;
extern char machines_trouvees[10][16];
extern int count;
extern Message msg;
extern Message response;


unsigned char compute_checksum(Message* msg);