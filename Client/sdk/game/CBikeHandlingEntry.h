/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CHandlingEntry.h
 *  PURPOSE:     Vehicle handling entry interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CBikeHandlingEntry
{
public:
    // Destructor
    virtual ~CBikeHandlingEntry(){};

    // Use this to copy data from an another handling class to this
    virtual void Assign(const CBikeHandlingEntry* pData) = 0;

    // Call this every time you're done changing something. This will recalculate
    // all transmission/handling values according to the new values.
    virtual void Recalculate() = 0;
};
