#pragma once
#ifndef SERVEUR_H
#define SERVEUR_H

#include "GeneralParameters.h"

int StartServer(Message* receivedMessage, Message* responseMessage, const char* server_ip);

#endif