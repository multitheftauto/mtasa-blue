/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEventGroupSA.h
*  PURPOSE:     Header file for base event group class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_EVENTGROUP
#define __CGAMESA_EVENTGROUP

#include <game/CEventGroup.h>

#define FUNC_CEventGroup_Add          0x4ab420

class CEventGroupSAInterface
{
public:
};

class CEventGroupSA : public CEventGroup
{
public:
                                CEventGroupSA       ( CEventGroupSAInterface * pInterface )  { m_pInterface = pInterface; }

    CEvent *                    Add                 ( CEvent * pEvent, bool b_1 );

protected:
    CEventGroupSAInterface *    m_pInterface;
};

#endif
