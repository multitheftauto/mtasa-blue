/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayerStats.h
*  PURPOSE:     Player statistics class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPlayerStats;

#ifndef __CPLAYERSTATS_H
#define __CPLAYERSTATS_H

#include <vector>
using namespace std;

struct sStat
{
	unsigned short id;
	float value;
};

class CPlayerStats
{
public:
										~CPlayerStats			( void );

	bool								GetStat					( unsigned short usID, float& fValue );
	void								SetStat					( unsigned short usID, float fValue );
	vector < sStat* > ::const_iterator	IterBegin				( void )		{ return m_List.begin (); }
	vector < sStat* > ::const_iterator	IterEnd					( void )		{ return m_List.end (); }
	unsigned short						GetSize					( void )		{ return static_cast < unsigned short > ( m_List.size () ); }

private:
	vector < sStat* >					m_List;
};

#endif
