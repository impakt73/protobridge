#ifndef PROTOBRIDGE_H
#define PROTOBRIDGE_H

#include <stdint.h>
#include <stddef.h>

typedef struct ProtoBridge_t* ProtoBridge;

struct DataStatus
{
    uint8_t isInputFull;
    uint8_t isOutputEmpty;
};

extern "C"
{

uint32_t CreateProtoBridge(ProtoBridge* phProtoBridge);
void DestroyProtoBridge(ProtoBridge hProtoBridge);

DataStatus QueryProtoBridgeDataStatus(ProtoBridge hProtoBridge);
void ClockProtoBridge(ProtoBridge hProtoBridge, const uint8_t* pInputData, uint8_t* pOutputData);

}

#endif
