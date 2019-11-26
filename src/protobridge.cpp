#include <protobridge.h>

#include <Vtop.h>

struct ProtoBridgeContext
{
    Vtop top;
};

uint32_t CreateProtoBridge(ProtoBridge* phProtoBridge)
{
    uint32_t result = 0xFFFFFFFF;

    ProtoBridgeContext* pContext = new ProtoBridgeContext();
    (*phProtoBridge) = reinterpret_cast<ProtoBridge>(pContext);

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

uint64_t UpdateProtoBridge(ProtoBridge hProtoBridge, uint64_t data)
{
    ProtoBridgeContext* pContext = reinterpret_cast<ProtoBridgeContext*>(hProtoBridge);

    pContext->top.i_data = data;

    pContext->top.i_clk = 1;

    pContext->top.eval();

    pContext->top.i_clk = 0;

    pContext->top.eval();

    return pContext->top.o_data;
}
