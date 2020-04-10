/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CColLineSA.h
 *  PURPOSE:     Header file for collision line class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

typedef struct
{
    CVector vecStart;
    DWORD   pad0;
    CVector vecEnd;
    DWORD   pad1
} CColLineSA;
