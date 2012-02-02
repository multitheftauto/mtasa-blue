/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerStatsPacket.cpp
*  PURPOSE:     Player statistics packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlayerStatsPacket::~CPlayerStatsPacket ( void )
{
    Clear ( );
}


bool CPlayerStatsPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Write the source player.
    if ( m_pSourceElement )
    {
        ElementID ID = m_pSourceElement->GetID ();
        BitStream.Write ( ID );

        // Write the stats
        unsigned short usNumStats = static_cast < unsigned short  >( m_List.size () );
        BitStream.WriteCompressed ( usNumStats );

        map < unsigned short, sPlayerStat* > ::const_iterator iter = m_List.begin ();
        sPlayerStat* pPlayerStat = NULL;
        for ( ; iter != m_List.end () ; iter++ )
        {
            pPlayerStat = (*iter).second;
            BitStream.Write ( pPlayerStat->id );
            BitStream.Write ( pPlayerStat->value );
        }

        return true;
    }

    return false;
}


void CPlayerStatsPacket::Add ( unsigned short usID, float fValue )
{
    map < unsigned short, sPlayerStat* > ::iterator iter = m_List.find ( usID );
    if ( iter != m_List.end ( ) )
    {
        if ( fValue == 0.0f )
        {
            sPlayerStat* pStat = (*iter).second;
            delete pStat;
            m_List.erase ( iter );
        }
        else
        {
            sPlayerStat* pStat = (*iter).second;
            pStat->value = fValue;
        }
    }
    else
    {
        sPlayerStat* pStat = new sPlayerStat;
        pStat->id = usID;
        pStat->value = fValue;
        m_List[ usID ] = pStat;
    }
}


void CPlayerStatsPacket::Remove ( unsigned short usID, float fValue )
{
    map < unsigned short, sPlayerStat* > ::const_iterator iter = m_List.find ( usID );
    sPlayerStat* pStat = NULL;
    if ( iter != m_List.end ( ) )
    {
        sPlayerStat* pStat = iter->second;
        delete pStat;
        m_List.erase ( iter );
    }
}
void CPlayerStatsPacket::Clear ( void )
{
    map < unsigned short, sPlayerStat* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
    {
        delete (*iter).second;
    }
    m_List.clear ();
}