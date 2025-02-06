/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CMatrixLinkSA.h
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CMatrixSA.h"

class CMatrixLinkSAInterface : public CMatrixSAInterface
{
public:
    CBuildingSAInterface*   m_pOwner;
    CMatrixLinkSAInterface* m_pPrev;
    CMatrixLinkSAInterface* m_pNext;
};
