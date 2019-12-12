#include <protobridge.h>
#include <cassert>

#include <Vtop.h>

struct ProtoBridgeContext
{
    Vtop top;
};

void ExecuteClock(ProtoBridgeContext* pContext, bool enableLogic)
{
    pContext->top.i_logic_en = enableLogic;

    pContext->top.i_clk = 1;

    pContext->top.eval();

    pContext->top.i_clk = 0;

    pContext->top.eval();

    pContext->top.i_logic_en = 0;
}

uint32_t CreateProtoBridge(ProtoBridge* phProtoBridge)
{
    uint32_t result = 0xFFFFFFFF;

    ProtoBridgeContext* pContext = new ProtoBridgeContext();
    (*phProtoBridge) = reinterpret_cast<ProtoBridge>(pContext);

    pContext->top.i_clk = 0;

    pContext->top.eval();

    pContext->top.i_logic_en = 1;
    pContext->top.i_mem_op = 0;
    pContext->top.i_clk = 1;
    pContext->top.i_rst = 1;

    pContext->top.eval();

    pContext->top.i_clk = 0;
    pContext->top.i_rst = 0;

    pContext->top.eval();

    pContext->top.i_logic_en = 0;

    return result;
}

void DestroyProtoBridge(ProtoBridge hProtoBridge)
{
    ProtoBridgeContext* pContext = reinterpret_cast<ProtoBridgeContext*>(hProtoBridge);
    delete pContext;
}

void ClockProtoBridge(ProtoBridge hProtoBridge)
{
    ProtoBridgeContext* pContext = reinterpret_cast<ProtoBridgeContext*>(hProtoBridge);

    ExecuteClock(pContext, true);
}

void WriteProtoBridgeMemory(ProtoBridge hProtoBridge, const void* pSource, size_t size, size_t destination)
{
    ProtoBridgeContext* pContext = reinterpret_cast<ProtoBridgeContext*>(hProtoBridge);

    // Write operation
    pContext->top.i_mem_op = 2;

    const size_t numQwords = (size / sizeof(uint64_t));
    assert(size % sizeof(uint64_t) == 0); // @TODO: Handle non-qword memory operations

    for (size_t qwordIndex = 0; qwordIndex < numQwords; ++qwordIndex)
    {
        pContext->top.i_mem_addr = destination + qwordIndex;
        pContext->top.i_mem_data = *(reinterpret_cast<const uint64_t*>(pSource) + qwordIndex);

        do
        {
            ExecuteClock(pContext, false);
        } while (pContext->top.o_mem_op_pending);
    }

    pContext->top.i_mem_op = 0;
}

void ReadProtoBridgeMemory(ProtoBridge hProtoBridge, size_t source, size_t size, void* pDestination)
{
    ProtoBridgeContext* pContext = reinterpret_cast<ProtoBridgeContext*>(hProtoBridge);

    // Read operation
    pContext->top.i_mem_op = 1;

    const size_t numQwords = (size / sizeof(uint64_t));
    assert(size % sizeof(uint64_t) == 0); // @TODO: Handle non-qword memory operations

    for (size_t qwordIndex = 0; qwordIndex < numQwords; ++qwordIndex)
    {
        pContext->top.i_mem_addr = source + qwordIndex;

        do
        {
            ExecuteClock(pContext, false);
        } while (pContext->top.o_mem_op_pending);

        *(reinterpret_cast<uint64_t*>(pDestination) + qwordIndex) = pContext->top.o_mem_data;
    }

    pContext->top.i_mem_op = 0;
}
