/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/CServer.cpp
 *  PURPOSE:     Server interface handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CServer.h"
#include "CGame.h"
#include "CMainConfig.h"
#define ALLOC_STATS_MODULE_NAME "deathmatch"
#include "SharedUtil.hpp"
#include "SharedUtil.Thread.h"
#include "SharedUtil.IntervalCounter.h"
#include "SharedUtil.IntervalCounter.hpp"
#if defined(MTA_DEBUG)
    #include "SharedUtil.Tests.hpp"
#endif

#include <wasmtime/core/iwasm/include/wasm_export.h>

CServerInterface* g_pServerInterface = NULL;
CNetServer*       g_pNetServer = NULL;
CNetServer*       g_pRealNetServer = NULL;

CServer::CServer()
{
    // Init
    m_pServerInterface = NULL;
    m_pGame = NULL;
}

CServer::~CServer()
{
}


//extern bool wasm_runtime_call_indirect(wasm_exec_env_t exec_env, uint32_t element_indices, uint32_t argc, uint32_t argv[]);

// The first parameter is not exec_env because it is invoked by native funtions
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// The first parameter exec_env must be defined using type wasm_exec_env_t
// which is the calling convention for exporting native API by WAMR.
//
// Converts a given integer x to string str[].
// digit is the number of digits required in the output.
// If digit is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(wasm_exec_env_t exec_env, int x, char* str, int str_len, int digit)
{
    int i = 0;

    printf("calling into native function: %s\n", __FUNCTION__);

    while (x)
    {
        // native is responsible for checking the str_len overflow
        if (i >= str_len)
        {
            return -1;
        }
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < digit)
    {
        if (i >= str_len)
        {
            return -1;
        }
        str[i++] = '0';
    }

    reverse(str, i);

    if (i >= str_len)
        return -1;
    str[i] = '\0';
    return i;
}

int get_pow(wasm_exec_env_t exec_env, int x, int y)
{
    printf("calling into native function: %s\n", __FUNCTION__);
    return (int)pow(x, y);
}

int32_t calculate_native(wasm_exec_env_t exec_env, int32_t n, int32_t func1, int32_t func2)
{
    printf("calling into native function: %s, n=%d, func1=%d, func2=%d\n", __FUNCTION__, n, func1, func2);

    uint32_t argv[] = {n};
    //if (!wasm_runtime_call_indirect(exec_env, func1, 1, argv))
    //{
    //    printf("call func1 failed\n");
    //    return 0xDEAD;
    //}

    uint32_t n1 = argv[0];
    printf("call func1 and return n1=%d\n", n1);

    //if (!wasm_runtime_call_indirect(exec_env, func2, 1, argv))
    //{
    //    printf("call func2 failed\n");
    //    return 0xDEAD;
    //}

    uint32_t n2 = argv[0];
    printf("call func2 and return n2=%d\n", n2);
    return n1 + n2;
}


extern "C"
{

int initWasm()
{
    static char global_heap_buf[512 * 1024];
    char *      buffer, error_buf[128];
    int         opt;
    char*       wasm_path = NULL;

    wasm_module_t        module = NULL;
    wasm_module_inst_t   module_inst = NULL;
    wasm_exec_env_t      exec_env = NULL;
    uint32               buf_size, stack_size = 8092, heap_size = 8092;
    wasm_function_inst_t func = NULL;
    wasm_function_inst_t func2 = NULL;
    char*                native_buffer = NULL;
    uint32_t             wasm_buffer = 0;

    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    
    static NativeSymbol native_symbols[] = {{
                                                "intToStr",            // the name of WASM function name
                                                intToStr,              // the native function pointer
                                                "(i*~i)i",             // the function prototype signature, avoid to use i32
                                                NULL                   // attachment is NULL
                                            },
                                            {
                                                "get_pow",            // the name of WASM function name
                                                get_pow,              // the native function pointer
                                                "(ii)i",              // the function prototype signature, avoid to use i32
                                                NULL                  // attachment is NULL
                                            },
                                            {"calculate_native", calculate_native, "(iii)i", NULL}};

    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

    // Native symbols need below registration phase
    init_args.n_native_symbols = sizeof(native_symbols) / sizeof(NativeSymbol);
    init_args.native_module_name = "env";
    init_args.native_symbols = native_symbols;

    if (!wasm_runtime_full_init(&init_args))
    {
        printf("Init runtime environment failed.\n");
        return -1;
    }

    //buffer = bh_read_file_to_buffer(wasm_path, &buf_size);

    if (!buffer)
    {
        printf("Open wasm app file [%s] failed.\n", wasm_path);
        goto fail;
    }

    module = wasm_runtime_load((uint8_t*)buffer, buf_size, error_buf, sizeof(error_buf));
    if (!module)
    {
        printf("Load wasm module failed. error: %s\n", error_buf);
        goto fail;
    }

    module_inst = wasm_runtime_instantiate(module, stack_size, heap_size, error_buf, sizeof(error_buf));

    if (!module_inst)
    {
        printf("Instantiate wasm module failed. error: %s\n", error_buf);
        goto fail;
    }

    exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);
    if (!exec_env)
    {
        printf("Create wasm execution environment failed.\n");
        goto fail;
    }

    if (!(func = wasm_runtime_lookup_function(module_inst, "generate_float", NULL)))
    {
        printf("The generate_float wasm function is not found.\n");
        goto fail;
    }

    wasm_val_t results[1];
    results[0].kind = WASM_F32;
    results[0].of.f32 = 0;

    wasm_val_t arguments[3];
    arguments[0].kind = WASM_I32;
    arguments[0].of.i32 = 10;
    arguments[1].kind = WASM_F64;
    arguments[0].of.f64 = 0.000101;
    arguments[2].kind = WASM_F32;
    arguments[2].of.f32 = 300.002;
    // pass 4 elements for function arguments
    if (!wasm_runtime_call_wasm_a(exec_env, func, 1, results, 3, arguments))
    {
        printf("call wasm function generate_float failed. %s\n", wasm_runtime_get_exception(module_inst));
        goto fail;
    }

    float ret_val;
    ret_val = results[0].of.f32;
    printf(
        "Native finished calling wasm function generate_float(), returned a "
        "float value: %ff\n",
        ret_val);

    // Next we will pass a buffer to the WASM function
    uint32 argv2[4];

    // must allocate buffer from wasm instance memory space (never use pointer
    // from host runtime)
    wasm_buffer = wasm_runtime_module_malloc(module_inst, 100, (void**)&native_buffer);

    memcpy(argv2, &ret_val, sizeof(float));            // the first argument
    argv2[1] = wasm_buffer;                            // the second argument is the wasm buffer address
    argv2[2] = 100;                                    //  the third argument is the wasm buffer size
    argv2[3] = 3;                                      //  the last argument is the digits after decimal point for
                                                       //  converting float to string

    if (!(func2 = wasm_runtime_lookup_function(module_inst, "float_to_string", NULL)))
    {
        printf("The wasm function float_to_string wasm function is not found.\n");
        goto fail;
    }

    if (wasm_runtime_call_wasm(exec_env, func2, 4, argv2))
    {
        printf(
            "Native finished calling wasm function: float_to_string, "
            "returned a formatted string: %s\n",
            native_buffer);
    }
    else
    {
        printf("call wasm function float_to_string failed. error: %s\n", wasm_runtime_get_exception(module_inst));
        goto fail;
    }

    wasm_function_inst_t func3 = wasm_runtime_lookup_function(module_inst, "calculate", NULL);
    if (!func3)
    {
        printf("The wasm function calculate is not found.\n");
        goto fail;
    }

    uint32_t argv3[1] = {3};
    if (wasm_runtime_call_wasm(exec_env, func3, 1, argv3))
    {
        uint32_t result = *(uint32_t*)argv3;
        printf("Native finished calling wasm function: calculate, return: %d\n", result);
    }
    else
    {
        printf("call wasm function calculate failed. error: %s\n", wasm_runtime_get_exception(module_inst));
        goto fail;
    }

fail:
    if (exec_env)
        wasm_runtime_destroy_exec_env(exec_env);
    if (module_inst)
    {
        if (wasm_buffer)
            wasm_runtime_module_free(module_inst, wasm_buffer);
        wasm_runtime_deinstantiate(module_inst);
    }
    if (module)
        wasm_runtime_unload(module);
    if (buffer)
        wasm_runtime_free(buffer);
    wasm_runtime_destroy();
    return 0;
}
}
void CServer::ServerInitialize(CServerInterface* pServerInterface)
{
    initWasm();
    m_pServerInterface = pServerInterface;
    g_pServerInterface = pServerInterface;
    g_pNetServer = pServerInterface->GetNetwork();
    g_pRealNetServer = g_pNetServer;
    #if defined(MTA_DEBUG)
    SharedUtil_Tests();
    #endif
}

bool CServer::ServerStartup(int iArgumentCount, char* szArguments[])
{
    if (!m_pGame)
    {
        m_pGame = new CGame;
        return m_pGame->Start(iArgumentCount, szArguments);
    }

    return false;
}

void CServer::ServerShutdown()
{
    if (m_pGame)
    {
        delete m_pGame;
        m_pGame = NULL;
    }
}

void CServer::GetTag(char* szInfoTag, int iInfoTag)
{
    if (m_pGame)
    {
        m_pGame->GetTag(szInfoTag, iInfoTag);
    }
}

void CServer::HandleInput(char* szCommand)
{
    if (m_pGame)
    {
        m_pGame->HandleInput(szCommand);
    }
}

void CServer::DoPulse()
{
    UNCLOCK(" Top", " Idle");
    if (m_pGame)
    {
        CLOCK(" Top", "Game->DoPulse");
        m_pGame->DoPulse();
        UNCLOCK(" Top", "Game->DoPulse");
    }
    CLOCK(" Top", " Idle");
}

bool CServer::IsFinished()
{
    if (m_pGame)
    {
        return m_pGame->IsFinished();
    }

    return false;
}

bool CServer::PendingWorkToDo()
{
    if (m_pGame && g_pNetServer)
    {
        if (g_pNetServer->GetPendingPacketCount() > 0)
        {
            return true;
        }
    }
    return false;
}

bool CServer::GetSleepIntervals(int& iSleepBusyMs, int& iSleepIdleMs, int& iLogicFpsLimit)
{
    if (m_pGame && g_pNetServer)
    {
        iSleepBusyMs = m_pGame->GetConfig()->GetPendingWorkToDoSleepTime();
        iSleepIdleMs = m_pGame->GetConfig()->GetNoWorkToDoSleepTime();
        iLogicFpsLimit = m_pGame->GetConfig()->GetServerLogicFpsLimit();
        return true;
    }
    return false;
}
