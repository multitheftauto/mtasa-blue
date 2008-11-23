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
	vector < sPlayerStat* > ::iterator iter = m_List.begin ();
	for ( ; iter != m_List.end () ; iter++ )
	{
		delete (*iter);
	}
	m_List.clear ();
}


bool CPlayerStatsPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    // Write the source player.
    if ( m_pSourceElement )
    {
        ElementID ID = m_pSourceElement->GetID ();
        BitStream.Write ( ID );

		// Write the stats
		unsigned short usNumStats = static_cast < unsigned short  >( m_List.size () );
		BitStream.Write ( usNumStats );

		vector < sPlayerStat* > ::const_iterator iter = m_List.begin ();
		for ( ; iter != m_List.end () ; iter++ )
		{
			BitStream.Write ( (*iter)->id );
			BitStream.Write ( (*iter)->value );
		}

		return true;
	}

	return false;
}


void CPlayerStatsPacket::Add ( unsigned short usID, float fValue )
{
	sPlayerStat* Stat = new sPlayerStat;
	Stat->id = usID;
	Stat->value = fValue;
	m_List.push_back ( Stat );
}
