/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.1
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        animation/CPositionRotationAnimation.h
*  PURPOSE:     Shared (server/client) way of performing position & rotation interpolation. 
*  Could be put in a more general animation framework in the future.
*  Could also be used in a more general way to use slerp on quaternions
*  Could also be used in a more general way to use non-delta but target rotation
*  DEVELOPERS:  Kayl
*
*****************************************************************************/

#ifndef __CPOSITIONROTATIONANIMATION_H
#define __CPOSITIONROTATIONANIMATION_H

struct SPositionRotation
{    
    SPositionRotation () {}

    SPositionRotation ( const CVector& a_rvecPosition, const CVector&  a_rvecRotation ):
        m_vecPosition( a_rvecPosition ), 
        m_vecRotation ( a_rvecRotation )
    {
    }

    SPositionRotation operator+ ( const SPositionRotation& a_rOther ) const
    {
        SPositionRotation result;
        result.m_vecPosition = m_vecPosition + a_rOther.m_vecPosition;
        result.m_vecRotation = m_vecRotation + a_rOther.m_vecRotation;
        return result;
    }

    SPositionRotation operator- ( const SPositionRotation& a_rOther ) const
    {
        SPositionRotation result;
        result.m_vecPosition = m_vecPosition - a_rOther.m_vecPosition;
        result.m_vecRotation = m_vecRotation - a_rOther.m_vecRotation;
        return result;
    }

     SPositionRotation& operator*= ( float a_fScalar )
    {
        m_vecPosition *= a_fScalar;
        m_vecRotation *= a_fScalar;
        return *this;
    }

    CVector m_vecPosition;
    CVector m_vecRotation;
};

class CPositionRotationAnimation : public TInterpolation < SPositionRotation >
{
public:
                CPositionRotationAnimation();
    void        SetSourceValue ( const SPositionRotation& a_rValue );
    
    // a_bDeltaRotationMode defines if the rotation provided is a relative rotation
    void        SetTargetValue ( const SPositionRotation& a_rValue, bool a_bDeltaRotationMode );

    // bResumeMode = considering the already elapsed time, full animation from start otherwise
    void        ToBitStream( NetBitStreamInterface& a_rBitStream, bool a_bResumeMode ) const;
    // Returned pointer must be deleted by caller
    static CPositionRotationAnimation* FromBitStream ( NetBitStreamInterface& a_rBitStream );   
protected:
    bool        m_bDeltaRotationMode; //defines if the rotation provided is a relative rotation
};

#endif
