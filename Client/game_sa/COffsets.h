/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/COffsets.h
 *  PURPOSE:     Header file for game variable offsets class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class COffsets
{
public:
    static void Initialize10EU();
    static void Initialize10US();
    static void Initialize11();

private:
    static void InitializeCommon10();
};
