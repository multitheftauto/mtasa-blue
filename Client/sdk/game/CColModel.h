/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CColModel.h
 *  PURPOSE:     Collision model entity interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CColModelSAInterface;

class CColModel
{
public:
    virtual CColModelSAInterface* GetInterface() const = 0;
    virtual void                  Destroy() = 0;
};
