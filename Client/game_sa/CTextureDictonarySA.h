/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CTextureDictonarySA.h
 *  PURPOSE:     Header file for game texture dictonary class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <game/RenderWare.h>

class CTextureDictonarySAInterface
{
public:
    RwTexDictionary* rwTexDictonary;
    unsigned short   usUsagesCount;
    unsigned short   usParentIndex;
    unsigned int     hash;
};
