#ifndef PROTOBRIDGE_H
#define PROTOBRIDGE_H

#include <stdint.h>
#include <stddef.h>

typedef struct ProtoBridge_t* ProtoBridge;

extern "C"
{

uint32_t CreateProtoBridge(ProtoBridge* phProtoBridge);
void DestroyProtoBridge(ProtoBridge hProtoBridge);

void ClockProtoBridge(ProtoBridge hProtoBridge);
void WriteProtoBridgeMemory(ProtoBridge hProtoBridge, const void* pSource, size_t size, size_t destination);
void ReadProtoBridgeMemory(ProtoBridge hProtoBridge, size_t source, size_t size, void* pDestination);
uint64_t QueryProtoBridgeCycleCount(ProtoBridge hProtoBridge);

}

#endif
