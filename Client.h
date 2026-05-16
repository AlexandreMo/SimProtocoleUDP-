#pragma once
#include "protocole.h"

#pragma comment(lib, "ws2_32.lib")

void SetServerIP(const char* new_ip);

int is_valid_ip(const char* ip);

int OpenConnection(const char* ip);

int SendAndRecieveMessage(Message* msg, Message* reponse);

void CloseConnection();