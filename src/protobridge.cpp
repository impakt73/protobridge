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

    pContext->top.i_rst_n = 0;

    pContext->top.eval();

    ExecuteClock(pContext);

    pContext->top.i_rst_n = 1;

    pContext->top.eval();

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

DataStatus QueryProtoBridgeDataStatus(ProtoBridge hProtoBridge)
{
    ProtoBridgeContext* pContext = reinterpret_cast<ProtoBridgeContext*>(hProtoBridge);

    DataStatus status = {};

    status.isInputFull = pContext->top.o_input_full;
    status.isOutputEmpty = pContext->top.o_output_empty;

    return status;
}

void ClockProtoBridge(ProtoBridge hProtoBridge, const uint8_t* pInputData, uint8_t* pOutputData)
{
    ProtoBridgeContext* pContext = reinterpret_cast<ProtoBridgeContext*>(hProtoBridge);

    if ((pInputData != nullptr) && (pContext->top.o_input_full == false))
    {
        pContext->top.i_data_valid = 1;
        pContext->top.i_data = *pInputData;
    }
    else
    {
        pContext->top.i_data_valid = 0;
    }

    if ((pOutputData != nullptr) && (pContext->top.o_output_empty == false))
    {
        pContext->top.i_data_read = 1;
        *pOutputData = pContext->top.o_data;
    }
    else
    {
        pContext->top.i_data_read = 0;
    }

    ExecuteClock(pContext);
}