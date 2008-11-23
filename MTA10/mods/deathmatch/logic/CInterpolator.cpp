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
    return;
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
bool CInterpolator::IsTimeInFuture ( unsigned long ulTime )
{
    if ( m_vecVecList.empty () )
        return false;

    return (ulTime>m_vecVecList[0]->m_ulTimeStamp);
}
inline bool CInterpolator::IsTimeTooFarPast( unsigned long ulTime )
{
    if ( m_vecVecList.empty () )
        return false;

    return (ulTime<m_vecVecList.back()->m_ulTimeStamp);
}


unsigned long CInterpolator::GetOldestEntry(CVector &Vec)
{
    Vec.fX = m_vecVecList.back()->m_fX;
    Vec.fY = m_vecVecList.back()->m_fY;
    Vec.fZ = m_vecVecList.back()->m_fZ;
    return m_vecVecList.back()->m_ulTimeStamp;
}

bool CInterpolator::Evaluate(unsigned long ulTime, CVector &Vec )
{
    if ( m_vecVecList.empty () )
        return false;

    if (IsTimeTooFarPast(ulTime))
    {
        Vec.fX = m_vecVecList.back()->m_fX;
        Vec.fY = m_vecVecList.back()->m_fY;
        Vec.fZ = m_vecVecList.back()->m_fZ;
        return true;
    }
    m_vecVecList[m_vecVecList.size()/2];
    
    unsigned int j = (unsigned int)m_vecVecList.size()-1;
    int iLeft= 0;
    int iRight = j;
    bool bRight = false;
    bool bLeft = false;
    for (unsigned int i = 0; i < m_vecVecList.size(); i ++ )
    {
        if ( !bLeft && m_vecVecList[i]->m_ulTimeStamp <= ulTime )
        {
            bLeft = true;
            iLeft = i;            
        }
        if ( !bRight && m_vecVecList[j]->m_ulTimeStamp >= ulTime )
        {
            bRight = true;
            iRight = j;
        }
        j--;

    }

    return Eval ( *m_vecVecList[iLeft], *m_vecVecList[iRight],ulTime, Vec);
}


bool CInterpolator::Eval(VecMap L, VecMap R, unsigned long ulTimeEval, CVector &Vec )
{
    float fA = float ( R.m_ulTimeStamp - ulTimeEval );
    float fB = float ( R.m_ulTimeStamp - L.m_ulTimeStamp );
    // Jax: not sure why these are both 0 sometimes, but 0/0 = corrupt time
    if ( fA == 0.0f && fB == 0.0f )
    {
        Vec.fX = L.m_fX;
        Vec.fY = L.m_fY;
        Vec.fZ = L.m_fZ;
        return true;
    }

    float fTime = fA / fB;    
    if ( fTime != 0.0f )
    {        
        Vec.fX = ( ( R.m_fX - L.m_fX ) * fTime ) + L.m_fX;
        Vec.fY = ( ( R.m_fY - L.m_fY ) * fTime ) + L.m_fY;
        Vec.fZ = ( ( R.m_fZ - L.m_fZ ) * fTime ) + L.m_fZ;
    }
    else
    {
        Vec.fX = L.m_fX;
        Vec.fY = L.m_fY;
        Vec.fZ = L.m_fZ;
    }

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