#include <protobridge.h>
#include <cassert>

#include <Vtop.h>

#if VM_TRACE
    #include <verilated_vcd_c.h>
#endif

struct ProtoBridgeContext
{
    Vtop top;

#if VM_TRACE
    VerilatedVcdC trace;
    uint64_t      time;
#endif
};

void ExecuteClock(ProtoBridgeContext* pContext)
{
    pContext->top.i_clk = 1;

    pContext->top.eval();

#if VM_TRACE
    pContext->trace.dump(pContext->time++);
#endif

    pContext->top.i_clk = 0;

    pContext->top.eval();

#if VM_TRACE
    pContext->trace.dump(pContext->time++);
#endif
}

uint32_t CreateProtoBridge(ProtoBridge* phProtoBridge)
{
    uint32_t result = 0xFFFFFFFF;

    ProtoBridgeContext* pContext = new ProtoBridgeContext();
    (*phProtoBridge) = reinterpret_cast<ProtoBridge>(pContext);

#if VM_TRACE
    Verilated::traceEverOn(true);

    pContext->time = 0;
    pContext->top.trace(&pContext->trace, 99);

    pContext->trace.open("trace.vcd");
#endif

    pContext->top.i_rst = 1;

    ExecuteClock(pContext);

    pContext->top.i_rst = 0;

    return result;
}

void DestroyProtoBridge(ProtoBridge hProtoBridge)
{
    ProtoBridgeContext* pContext = reinterpret_cast<ProtoBridgeContext*>(hProtoBridge);

#if VM_TRACE
    pContext->trace.close();
#endif

    delete pContext;
}

void ClockProtoBridge(ProtoBridge hProtoBridge)
{
    ProtoBridgeContext* pContext = reinterpret_cast<ProtoBridgeContext*>(hProtoBridge);

    ExecuteClock(pContext);
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
            ExecuteClock(pContext);
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
            ExecuteClock(pContext);
        } while (pContext->top.o_mem_op_pending);

        *(reinterpret_cast<uint64_t*>(pDestination) + qwordIndex) = pContext->top.o_mem_data;
    }

    pContext->top.i_mem_op = 0;
}

uint64_t QueryProtoBridgeCycleCount(ProtoBridge hProtoBridge)
{
    ProtoBridgeContext* pContext = reinterpret_cast<ProtoBridgeContext*>(hProtoBridge);

    return (pContext->time / 2);
}
