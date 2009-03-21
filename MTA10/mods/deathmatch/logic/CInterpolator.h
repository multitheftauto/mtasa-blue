/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CInterpolator.h
*  PURPOSE:     Header for interpolator class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __INTERPOLATOR__
#define __INTERPOLATOR__

#include <vector>
#include "Utils.h"

struct VecMap
{
    unsigned long m_ulTimeStamp;
    float m_fX,m_fY,m_fZ;
};

// the logic may seem backwards in some places, because i'm inserting at the front, so the list is 
// reversed to what you'd think, like a stack
class CInterpolator
{
public:

                                CInterpolator( void );
                                ~CInterpolator( void );

    bool                        IsTimeInFuture ( unsigned long ulTime );
    inline bool                 IsTimeTooFarPast( unsigned long ulTime );
    void                        Push( float fX, float fY, float fZ, unsigned long ulTimeStamp );
    bool                        Evaluate( unsigned long ulTime, CVector &Vec );
    void                        Pop();
    unsigned long               GetOldestEntry(CVector &Vec);

    inline void                 Clear           ( void )            { m_vecVecList.clear (); };
    inline unsigned int         Count           ( void )            { return static_cast < unsigned int > ( m_vecVecList.size () ); };

    inline std::vector <VecMap*>::const_iterator   IterBegin   ( void )    { return m_vecVecList.begin (); };
    inline std::vector <VecMap*>::const_iterator   IterEnd     ( void )    { return m_vecVecList.end (); };

private:


    std::vector <VecMap *>      m_vecVecList;
    bool                        Eval( VecMap L, VecMap R, unsigned long ulTime, CVector &Vec );
};

#endif