/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMouseControl.h
*  PURPOSE:     Simulates the 'Fly with mouse' and 'Steer with mouse' controls
*  DEVELOPERS:  Talidan
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <game/CPad.h>

#ifndef __CMOUSECONTROL_H
#define __CMOUSECONTROL_H

enum MouseControlVehicleType
{
    TYPE_CAR = 0,
    TYPE_BIKE,
    TYPE_PLANE,
    TYPE_NONE,
};


class CMouseControl
{
public:
                        CMouseControl           ( void );
                        ~CMouseControl          ( void );

    void                Reset                   ( void );
    bool                ProcessMouseMove        ( UINT uMsg, WPARAM wParam, LPARAM lParam );
    void                ApplyAxes               ( CControllerState& cs );

protected:
    short                           m_usLeftStickX;
    short                           m_usLeftStickY;
};

#endif