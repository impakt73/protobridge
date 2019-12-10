#ifndef PROTOBRIDGE_H
#define PROTOBRIDGE_H

#include <stdint.h>
#include <stddef.h>

typedef struct ProtoBridge_t* ProtoBridge;

extern "C"
{

uint32_t CreateProtoBridge(ProtoBridge* phProtoBridge, void* pMemory, size_t memorySize);
void DestroyProtoBridge(ProtoBridge hProtoBridge);

void UpdateProtoBridge(ProtoBridge hProtoBridge);
void WriteProtoBridgeRegister(ProtoBridge hProtoBridge, uint32_t offset, uint32_t data);
uint32_t ReadProtoBridgeRegister(ProtoBridge hProtoBridge, uint32_t offset);

}

#endif
