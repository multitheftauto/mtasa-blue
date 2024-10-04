/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/gamesa_init.h
 *  PURPOSE:     Global game module header
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

//
// Use MemSet/Cpy/Put for non Mem*Fast memory regions
//

void MemSet(void* dwDest, int cValue, uint uiAmount);
void MemCpy(void* dwDest, const void* dwSrc, uint uiAmount);

template <class T, class U>
void MemPut(U ptr, const T value)
{
    if (*(T*)ptr != value)
        MemCpy((void*)ptr, &value, sizeof(T));
}

template <class T, class U>
void MemAdd(U ptr, const T value)
{
    T current;
    memcpy(&current, (void*)ptr, sizeof(T));
    current += value;
    MemCpy((void*)ptr, &current, sizeof(T));
}

//
// Use Mem*Fast for the following memory regions:
//
// 0x4C0300 - 0x4C03FF
// 0x4EB900 - 0x4EB9FF
// 0x502200 - 0x5023FF
// 0x50AB00 - 0x50ABFF
// 0x50BF00 - 0x50BFFF
// 0x533200 - 0x5332FF
// 0x609C00 - 0x609CFF
// 0x60D800 - 0x60D8FF
// 0x60F200 - 0x60F2FF
// 0x642000 - 0x6420FF
// 0x648A00 - 0x648AFF
// 0x64CA00 - 0x64CAFF
// 0x687000 - 0x6870FF
// 0x6A0700 - 0x6A07FF
// 0x6AEA00 - 0x6AEAFF
// 0x729B00 - 0x729BFF
// 0x742B00 - 0x742BFF
//
inline void MemCpyFast(void* dwDest, const void* dwSrc, uint uiAmount)
{
    DEBUG_CHECK_IS_FAST_MEM(dwDest, uiAmount);
    memcpy(dwDest, dwSrc, uiAmount);
}

inline void MemSetFast(void* dwDest, int cValue, uint uiAmount)
{
    DEBUG_CHECK_IS_FAST_MEM(dwDest, uiAmount);
    memset(dwDest, cValue, uiAmount);
}

template <class T, class U>
void MemPutFast(U ptr, const T value)
{
    DEBUG_CHECK_IS_FAST_MEM(ptr, sizeof(T));
    *(T*)ptr = value;
}

template <class T, class U>
void MemAddFast(U ptr, const T value)
{
    DEBUG_CHECK_IS_FAST_MEM(ptr, sizeof(T));
    *(T*)ptr += value;
}

template <class T, class U>
void MemSubFast(U ptr, const T value)
{
    DEBUG_CHECK_IS_FAST_MEM(ptr, sizeof(T));
    *(T*)ptr -= value;
}

template <class T, class U>
void MemAndFast(U ptr, const T value)
{
    DEBUG_CHECK_IS_FAST_MEM(ptr, sizeof(T));
    *(T*)ptr &= value;
}

template <class T, class U>
void MemOrFast(U ptr, const T value)
{
    DEBUG_CHECK_IS_FAST_MEM(ptr, sizeof(T));
    *(T*)ptr |= value;
}

bool GetDebugIdEnabled(uint uiDebugId);
void LogEvent(uint uiDebugId, const char* szType, const char* szContext, const char* szBody, uint uiAddReportLogId = 0);

struct __THISCALL{};
struct __CDECL{};
struct __STDCALL{};
struct __FASTCALL{};
struct __VECTORCALL{};

template <typename... Args>
struct GTAFuncSignature
{
    std::tuple<Args...> args;
    GTAFuncSignature(Args... a) : args(std::forward<Args>(a)...) {}
};

template <typename... Args>
GTAFuncSignature<Args...> PrepareSignature(Args... args)
{
    return GTAFuncSignature<Args...>(std::forward<Args>(args)...);
}

template <typename ReturnType, typename Func, typename Tuple, std::size_t... I>
ReturnType apply_helper(Func&& f, Tuple&& t, std::index_sequence<I...>)
{
    return f(std::get<I>(std::forward<Tuple>(t))...);
}

template <typename ReturnType, typename CallingConvention, typename Func, typename... Args>
ReturnType CallGTAFunction(Func function, GTAFuncSignature<Args...>& sig)
{
    if constexpr (std::is_same_v<CallingConvention, __THISCALL>)
        return apply_helper<ReturnType>(reinterpret_cast<ReturnType(__thiscall*)(Args...)>(function), sig.args, std::index_sequence_for<Args...>{});
    else if constexpr (std::is_same_v<CallingConvention, __CDECL>)
        return apply_helper<ReturnType>(reinterpret_cast<ReturnType(__cdecl*)(Args...)>(function), sig.args, std::index_sequence_for<Args...>{});
    else if constexpr (std::is_same_v<CallingConvention, __STDCALL>)
        return apply_helper<ReturnType>(reinterpret_cast<ReturnType(__stdcall*)(Args...)>(function), sig.args, std::index_sequence_for<Args...>{});
    else if constexpr (std::is_same_v<CallingConvention, __FASTCALL>)
        return apply_helper<ReturnType>(reinterpret_cast<ReturnType(__fastcall*)(Args...)>(function), sig.args, std::index_sequence_for<Args...>{});
    else if constexpr (std::is_same_v<CallingConvention, __VECTORCALL>)
        return apply_helper<ReturnType>(reinterpret_cast<ReturnType(__vectorcall*)(Args...)>(function), sig.args, std::index_sequence_for<Args...>{});
    else
        static_assert(sizeof(char) == 0, "Invalid calling convention specified in CallGTAFunction");
}
