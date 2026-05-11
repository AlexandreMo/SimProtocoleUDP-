#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#define PROTOCOL_MAX_DATA 100
#define DEFAULT_PORT 8005
#define MACHINE1 "198.168.10.20" // IP de la machine1 
#define MACHINE2 "198.168.10.20" // IP de la machine2
#define ADDRESSE_DIFFUSION "198.168.10.255" // Adresse de diffusion du réseau local
#define DATA_SIZE 100
#define ADDRESS_TEXT_SIZE 128
#define TIMEOUT 1000

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

#pragma pack(push, 1) // Alignement sur 1 octet pour éviter les problèmes de padding
typedef struct
{
unsigned char type;
unsigned char number;
unsigned char length;
unsigned char data[DATA_SIZE];
unsigned char checksum;

} Message;
#pragma pack(pop)

// Dans ton serveur ou GeneralParameters.h
const int32_t POS_HOME[6] = {250000, -300000, 150000, 180000000, 0, 0};
const int32_t POS_PICK[6] = {426500, -191600, 39300, 195000000, 18000000, -10000000};
const int32_t POS_PLACE[6] = {155000, -497000, -50000, 180000000, 0, 0};

unsigned char calculateChecksum(Message* msg);
unsigned char responsetype(Message* receivedMessage);

