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

class CPlaceableSAInterface;

class CMatrixLinkSAInterface : public CMatrixSAInterface
{
public:
    CMatrixLinkSAInterface() : CMatrixSAInterface{}, m_pOwner{}, m_pPrev{}, m_pNext{} {}

public:
    CPlaceableSAInterface*  m_pOwner;
    CMatrixLinkSAInterface* m_pPrev;
    CMatrixLinkSAInterface* m_pNext;
};
