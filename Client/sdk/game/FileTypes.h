/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        FileTypes.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once
#include <stdint.h>

static inline int32_t GetBaseIDforDFF()
{
    return 0;
}

static inline int32_t GetBaseIDforTXD()
{
    return *(int32_t*)(0x407104 + 2);
}

static inline int32_t GetBaseIDforCOL()
{
    return *(int32_t*)(0x410344 + 2);
}

static inline int32_t GetBaseIDforIPL()
{
    return *(int32_t*)(0x4044F4 + 2);
}

static inline int32_t GetBaseIDforDAT()
{
    return *(int32_t*)(0x44D064 + 2);
}

static inline int32_t GetBaseIDforIFP()
{
    return *(int32_t*)(0x404969 + 2);
}

static inline int32_t GetBaseIDforRRR()
{
    return *(int32_t*)(0x4594A1 + 2);
}

static inline int32_t GetBaseIDforSCM()
{
    return *(int32_t*)(0x46A574 + 2);
}

static inline int32_t GetCountOfAllFileIDs()
{
    return (*(char**)(0x5B8AFA + 2) - *(char**)(0x5B8B08 + 6)) / 0x14 /* sizeof(CStreamingInfo) */;
}
