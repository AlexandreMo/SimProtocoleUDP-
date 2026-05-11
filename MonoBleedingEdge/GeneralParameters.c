#pragma once
#include "GeneralParameters.h"
unsigned char calculateChecksum(Message* msg) {
    unsigned char checksum = 0;
    checksum += msg->type;
    checksum += msg->number;
    checksum += msg->length;
    for (int i = 0; i < msg->length; i++) {
        checksum += msg->data[i];
    }
    return checksum;
}

unsigned char responsetypeServeur(Message* receivedMessage) {
    switch (receivedMessage->type) {
        case COM_CONVEYOR:
            return REP_CONVEYOR;
        case COM_PALLET_SENSOR:
            return REP_PALLET_SENSOR;
        case COM_SET_VACUUM:
            return REP_SET_VACUUM;
        case COM_GET_HAS_PIECE:
            return REP_GET_HAS_PIECE;
        case COM_ROBOT_MOVE:
            return REP_ROBOT_MOVE;
        case COM_ROBOT_IS_MOVING:
            return REP_ROBOT_IS_MOVING;
        case COM_PRESENCE:
            return REP_PRESENCE;
        default:
            return REP_ERROR; // Type de message inconnu
    }
}
