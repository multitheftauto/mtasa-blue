/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CAESoundManagerSA.h
 *  PURPOSE:     Header file for audio engine sound manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <game/CAESoundManager.h>

#define CLASS_CAESoundManager                           0xB62CB0

class CAESoundManagerSAInterface
{
};

class CAESoundManagerSA : public CAESoundManager
{
public:
    CAESoundManagerSA(CAESoundManagerSAInterface* pInterface);

public:
    virtual void CancelSoundsInBankSlot(uint uiGroup, uint uiIndex) override;

private:
    CAESoundManagerSAInterface* m_pInterface;
};
