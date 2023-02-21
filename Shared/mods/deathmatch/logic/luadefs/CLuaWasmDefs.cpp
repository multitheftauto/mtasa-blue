/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Shared/mods/logic/luadefs/CLuaWasmDefs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaWasmDefs.h"
#include "CScriptArgReader.h"
#include "Utils.h"
#include <lua/CLuaFunctionParser.h>
#include <SharedUtil.Memory.h>

#ifndef MTA_CLIENT
    #include <wasmtime/core/iwasm/include/wasm_export.h>
#endif

#ifndef MTA_CLIENT

static wasm_module_t      module = nullptr;
static wasm_module_inst_t module_inst = nullptr;
static uint32             stack_size = 80000092;
static uint32             heap_size = 80000092;
static char               error_buf[128] = {};
static wasm_exec_env_t    exec_env = NULL;
static wasm_function_inst_t      func = NULL;

static auto WasmLoadString(std::string code) -> int
{
    module = wasm_runtime_load((uint8_t*)code.c_str(), code.length(), error_buf, sizeof(error_buf));
    if (!module)
    {
        printf("Load wasm module failed. error: %s\n", error_buf);
        return 1;
    }

    module_inst = wasm_runtime_instantiate(module, stack_size, heap_size, error_buf, sizeof(error_buf));

    if (!module_inst)
    {
        printf("Instantiate wasm module failed. error: %s\n", error_buf);
        return 2;
    }

    exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);
    if (!exec_env)
    {
        printf("Create wasm execution environment failed.\n");
        return 3;
    }
    return 0;
}

static auto WasmCall(std::string funcName, std::vector<double> args) -> double
{
    if (module_inst == nullptr)
        return -2;

    if (!(func = wasm_runtime_lookup_function(module_inst, funcName.c_str(), NULL)))
    {
        printf("The %s wasm function is not found.\n", funcName.c_str());
        return -1;
    }

    wasm_val_t results[1];
    results[0].kind = WASM_I32;
    results[0].of.i32 = 0;

    std::vector<wasm_val_t> arguments;
    for (const auto& val : args)
    {
        arguments.push_back({});
        arguments.back().kind = WASM_I32;
        arguments.back().of.i32 = val;
    }

    // pass 4 elements for function arguments
    if (!wasm_runtime_call_wasm_a(exec_env, func, 1, results, 2, arguments.data()))
    {
        printf("call wasm function add failed. %s\n", wasm_runtime_get_exception(module_inst));
        return -3;
    }

    return results[0].of.i32;
}
#endif

void CLuaWasmDefs::LoadFunctions()
{
#ifndef MTA_CLIENT
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"wasmLoadString", ArgumentParser<WasmLoadString>},
        {"wasmCall", ArgumentParser<WasmCall>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
#endif
}
