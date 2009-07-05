/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CInterpolator.cpp
*  PURPOSE:     Interpolation calculations
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CInterpolator::CInterpolator()
{
    return;
}

CInterpolator::~CInterpolator()
{
    while ( !m_vecVecList.empty () )
    {
        VecMap* map = m_vecVecList.back ();
        m_vecVecList.pop_back ();
        delete map;
    }
}


void CInterpolator::Push(float fX, float fY, float fZ, unsigned long ulTimeStamp)
{
    VecMap* map = new VecMap;
    map->m_fX = fX;
    map->m_fY = fY;
    map->m_fZ = fZ;
    map->m_ulTimeStamp = ulTimeStamp;
    m_vecVecList.insert(m_vecVecList.begin(), map);
}


unsigned long CInterpolator::GetOldestEntry(CVector &Vec)
{
    Vec.fX = m_vecVecList.back()->m_fX;
    Vec.fY = m_vecVecList.back()->m_fY;
    Vec.fZ = m_vecVecList.back()->m_fZ;
    return m_vecVecList.back()->m_ulTimeStamp;
}

bool CInterpolator::Evaluate ( unsigned long ulTime, CVector& vecResult )
{
    if ( m_vecVecList.empty () )
        return false;

	// Time later than newest point, so use the newest point
	if ( ulTime > m_vecVecList[0]->m_ulTimeStamp )
    {
        vecResult.fX = m_vecVecList[0]->m_fX;
        vecResult.fY = m_vecVecList[0]->m_fY;
        vecResult.fZ = m_vecVecList[0]->m_fZ;
        return true;
    }

	// Find the two points either side and interpolate
    for ( unsigned int idx = 1 ; idx < m_vecVecList.size () ; idx++ )
    {
	    if ( ulTime > m_vecVecList[idx]->m_ulTimeStamp )
        {
            return Eval ( *m_vecVecList[idx-1], *m_vecVecList[idx], ulTime, vecResult );
        }
    }

	// Time earlier than oldest point, so use the oldest point
    vecResult.fX = m_vecVecList.back ()->m_fX;
    vecResult.fY = m_vecVecList.back ()->m_fY;
    vecResult.fZ = m_vecVecList.back ()->m_fZ;
    return true;
}

// Left should be later than Right
bool CInterpolator::Eval ( const VecMap& Left, const VecMap& Right, unsigned long ulTimeEval, CVector& vecResult )
{
    // Check for being the same or maybe wrap around
    if ( Right.m_ulTimeStamp >= Left.m_ulTimeStamp )
    {
        vecResult.fX = Left.m_fX;
        vecResult.fY = Left.m_fY;
        vecResult.fZ = Left.m_fZ;
        return true;
    }

    // Find the relative position of ulTimeEval between R.m_ulTimeStamp and L.m_ulTimeStamp
    float fAlpha = Unlerp ( Right.m_ulTimeStamp, ulTimeEval, Left.m_ulTimeStamp );

    // Lerp between Right.pos and Left.pos
    vecResult.fX = Lerp ( Right.m_fX, fAlpha, Left.m_fX );
    vecResult.fY = Lerp ( Right.m_fY, fAlpha, Left.m_fY );
    vecResult.fZ = Lerp ( Right.m_fZ, fAlpha, Left.m_fZ );
    return true;
}


void CInterpolator::Pop()
{
    if ( !m_vecVecList.empty() )
    {
        VecMap* map = m_vecVecList.back ();
        m_vecVecList.pop_back();
        delete map;
    }
}