#include <protobridge.h>

#include <Vtop.h>

struct ProtoBridgeContext
{
    Vtop top;
    void* pMemory;
    size_t memorySize;
};

uint32_t CreateProtoBridge(ProtoBridge* phProtoBridge, void* pMemory, size_t memorySize)
{
    uint32_t result = 0xFFFFFFFF;

    ProtoBridgeContext* pContext = new ProtoBridgeContext();
    (*phProtoBridge) = reinterpret_cast<ProtoBridge>(pContext);

    pContext->pMemory = pMemory;
    pContext->memorySize = memorySize;

    pContext->top.i_clk = 0;
    pContext->top.i_rst = 1;

    pContext->top.eval();

    pContext->top.i_rst = 0;

    pContext->top.eval();

    return result;
}

void DestroyProtoBridge(ProtoBridge hProtoBridge)
{
    ProtoBridgeContext* pContext = reinterpret_cast<ProtoBridgeContext*>(hProtoBridge);
    delete pContext;
}

void UpdateProtoBridge(ProtoBridge hProtoBridge)
{
    ProtoBridgeContext* pContext = reinterpret_cast<ProtoBridgeContext*>(hProtoBridge);

    // Handle memory reads
    if (pContext->top.o_read_en && (pContext->pMemory != nullptr))
    {
        const uint32_t readAddr = pContext->top.o_read_addr;
        if ((readAddr + sizeof(uint32_t)) <= pContext->memorySize)
        {
            uint32_t data;
            memcpy(&data, reinterpret_cast<uint8_t*>(pContext->pMemory) + readAddr, sizeof(uint32_t));
            pContext->top.i_data = data;
            printf("Read %u From %u\n", static_cast<uint32_t>(data), static_cast<uint32_t>(readAddr));
        }
    }

    // Handle memory writes
    if (pContext->top.o_write_en && (pContext->pMemory != nullptr))
    {
        const uint32_t writeAddr = pContext->top.o_write_addr;
        if ((writeAddr + sizeof(uint32_t)) <= pContext->memorySize)
        {
            const uint32_t data = pContext->top.o_data;
            memcpy(reinterpret_cast<uint8_t*>(pContext->pMemory) + writeAddr, &data, sizeof(uint32_t));
            printf("Write %u To %u\n", static_cast<uint32_t>(data), static_cast<uint32_t>(writeAddr));
        }
    }

    pContext->top.i_clk = 1;

    pContext->top.eval();

    pContext->top.i_clk = 0;

    pContext->top.eval();
}
