/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CDoorSA.cpp
 *  PURPOSE:     Vehicle door entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

/**
 * \todo Find out what GetAngleOpenRatio actually does
 * @return FLOAT Not sure...
 */
FLOAT CDoorSA::GetAngleOpenRatio()
{
    DEBUG_TRACE("FLOAT CDoorSA::GetAngleOpenRatio ( )");

    if (GetInterface())
    {
        // CDoor::GetAngleOpenRatio
        return ((float(__thiscall*)(CDoorSAInterface*))(FUNC_GetAngleOpenRatio))(GetInterface());
    }

    return 0.0f;
}

/**
 * Checks if the door is closed
 * @return BOOL TRUE if the door is closed, FALSE otherwise
 */
BOOL CDoorSA::IsClosed()
{
    DEBUG_TRACE("BOOL CDoorSA::IsClosed (  )");

    if (GetInterface())
    {
        // CDoor::IsClosed
        return ((bool(__thiscall*)(CDoorSAInterface*))(FUNC_IsClosed))(GetInterface());
    }

    return FALSE;
}

/**
 * Checks if the door is completely open
 * @return BOOL TRUE if the door is fully opened, FALSE if it is opening, is closed or is missing.
 * \todo Check what this returns if the door has been ripped off
 */
BOOL CDoorSA::IsFullyOpen()
{
    DEBUG_TRACE("BOOL CDoorSA::IsFullyOpen (  )");

    if (GetInterface())
    {
        // CDoor::IsFullyOpen
        return ((bool(__thiscall*)(CDoorSAInterface*))(FUNC_IsFullyOpen))(GetInterface());
    }

    return FALSE;
}

/**
 * Opens the door
 * @param fUnknown Not sure...
 * \todo Check what the parameter for Open does
 */
VOID CDoorSA::Open(float fOpenRatio)
{
    DEBUG_TRACE("VOID CDoorSA::Open ( float fOpenRatio )");

    if (GetInterface())
    {
        // CDoor::Open
        ((void(__thiscall*)(CDoorSAInterface*, float))(FUNC_Open))(GetInterface(), fOpenRatio);
    }
}
