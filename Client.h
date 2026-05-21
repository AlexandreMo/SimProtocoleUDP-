/*--------------------------------------------------------------------------------
-- HEIG-VD, bureau iai, 1400 Yverdon-les-Bains
-- Project : Header file for robot control functions
-- File    : Mouv.c
-- Author   : Alexandre Moore & Tenkeu Franklin 
-- Date    : 27.04.2026
--------------------------------------------------------------------------------*/
#pragma once
#include "protocole.h"

#pragma comment(lib, "ws2_32.lib")

void SetServerIP(const char* new_ip);

int is_valid_ip(const char* ip);

int OpenConnection(const char* ip);

int SendAndRecieveMessage(Message* msg, Message* reponse);

void CloseConnection();