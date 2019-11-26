#ifndef PROTOBRIDGE_H
#define PROTOBRIDGE_H

#include <stdint.h>

typedef struct ProtoBridge_t* ProtoBridge;

extern "C"
{

uint32_t CreateProtoBridge(ProtoBridge* phProtoBridge);
void DestroyProtoBridge(ProtoBridge hProtoBridge);

uint64_t UpdateProtoBridge(ProtoBridge hProtoBridge, uint64_t data);

}

#endif
