#ifndef CLIENT_H
#define CLIENT_H

#include "GeneralParameters.h"

// On ne met que le prototype de la fonction
// Elle prend le message à envoyer et l'IP de la cible
int sendMessage(Message* msg, const char* server_ip);

#endif