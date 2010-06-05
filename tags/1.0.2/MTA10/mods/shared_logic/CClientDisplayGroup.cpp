/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDisplayGroup.cpp
*  PURPOSE:     Text display grouping class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#include <StdInc.h>

CClientDisplayGroup::CClientDisplayGroup ( void )
{
    m_bVisible = false;
}


CClientDisplayGroup::~CClientDisplayGroup ( void )
{
    RemoveAll ();
}


void CClientDisplayGroup::RemoveAll ( void )
{

}