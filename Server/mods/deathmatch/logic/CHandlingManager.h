/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CHandlingManager.h
 *  PURPOSE:     Header file for vehicle handling manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CHandlingManager;

#pragma once

#include "CHandlingEntry.h"
#include "CCommon.h"

class CHandlingManager
{
public:
    CHandlingManager();
    ~CHandlingManager();

    // Helper functions
    eHandlingProperty GetPropertyEnumFromName(std::string strName);

    std::map<std::string, eHandlingProperty> m_HandlingNames;
};
