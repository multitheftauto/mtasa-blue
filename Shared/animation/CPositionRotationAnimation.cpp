/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.1
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        animation/CPositionRotationAnimation.cpp
*  PURPOSE:     Shared (server/client) way of performing position & rotation interpolation. 
*  Could be put in a more general animation framework in the future.
*  Could also be used in a more general way to use slerp on quaternions
*  Could also be used in a more general way to use non-delta but target rotation
*  DEVELOPERS:  Kayl
*
*****************************************************************************/

#include "StdInc.h"

#include "net/SyncStructures.h"

CPositionRotationAnimation::CPositionRotationAnimation():
    m_bDeltaRotationMode ( false )
{
}

void CPositionRotationAnimation::SetSourceValue ( const SPositionRotation& a_rValue )
{
    SPositionRotation realValue = a_rValue;
    CVector& rvecRotation = realValue.m_vecRotation;

     // Get the start rotation between 0 and 2*pi
    if ( rvecRotation.fX < 0 )
        rvecRotation.fX += 2.0f * PI;
    else if ( rvecRotation.fX >= 2*PI )
        rvecRotation.fX -= 2.0f * PI;

    if ( rvecRotation.fY < 0 )
        rvecRotation.fY += 2.0f * PI;
    else if ( rvecRotation.fY >= 2*PI )
        rvecRotation.fY -= 2.0f * PI;

    if ( rvecRotation.fZ < 0 )
        rvecRotation.fZ += 2.0f * PI;
    else if ( rvecRotation.fZ >= 2*PI )
        rvecRotation.fZ -= 2.0f * PI;

    TInterpolation::SetSourceValue ( realValue );
}

void CPositionRotationAnimation::SetTargetValue ( const SPositionRotation& a_rValue, bool a_bDeltaRotationMode )
{
    TInterpolation::SetTargetValue ( a_rValue );
   
    m_bDeltaRotationMode = a_bDeltaRotationMode;
    
    if ( m_bDeltaRotationMode )
    {
        m_DeltaValue.m_vecRotation = a_rValue.m_vecRotation;
        m_TargetValue.m_vecRotation = m_SourceValue.m_vecRotation + m_DeltaValue.m_vecRotation;
    }
}

 void CPositionRotationAnimation::ToBitStream( NetBitStreamInterface& a_rBitStream, bool a_bResumeMode ) const
 {
     a_rBitStream.WriteBit ( a_bResumeMode );
     if ( a_bResumeMode )
     {
        unsigned long ulNow = _GetTime ();
        unsigned long ulElaspedTime = ulNow - m_ulStartTime;
        unsigned long ulTimeLeft = 0;
        if ( m_ulEndTime > ulNow )
        {
            ulTimeLeft = m_ulEndTime - ulNow;
        }
        a_rBitStream.WriteCompressed ( ulElaspedTime );
        a_rBitStream.WriteCompressed ( ulTimeLeft );
     }
     else
     {
         a_rBitStream.WriteCompressed ( m_ulDuration );
     }

     SPositionSync positionSync;
     positionSync.data.vecPosition = m_SourceValue.m_vecPosition;
     a_rBitStream.Write ( &positionSync );

     SRotationRadiansSync rotationSync ( true ); //RPC function used floats when join time packet didn't, let's go for float
     rotationSync.data.vecRotation = m_SourceValue.m_vecRotation;
     a_rBitStream.Write ( &rotationSync );
    
     positionSync.data.vecPosition = m_TargetValue.m_vecPosition;
     a_rBitStream.Write ( &positionSync );

     a_rBitStream.WriteBit ( m_bDeltaRotationMode );
     if ( m_bDeltaRotationMode )
     {
        rotationSync.data.vecRotation = m_DeltaValue.m_vecRotation; //We serialize DELTA
     }
     else
     {
         rotationSync.data.vecRotation = m_TargetValue.m_vecRotation;
     }
     a_rBitStream.Write ( &rotationSync );

     ////We write the string directly to allow new types without changing netcode (since integer values of enum might change)
     a_rBitStream.WriteString ( CEasingCurve::GetStringFromEasingType( m_easingCurve.GetType() ) );
     double fEasingPeriod, fEasingAmplitude, fEasingOvershoot;
     m_easingCurve.GetParams ( fEasingPeriod, fEasingAmplitude, fEasingOvershoot );
     a_rBitStream.Write ( fEasingPeriod );
     a_rBitStream.Write ( fEasingAmplitude );
     a_rBitStream.Write ( fEasingOvershoot );
 }
 
 CPositionRotationAnimation* CPositionRotationAnimation::FromBitStream ( NetBitStreamInterface& a_rBitStream )
 {
     bool bResumeMode;
     if ( !a_rBitStream.ReadBit ( bResumeMode ) )
     {
        return NULL;
     }

     CPositionRotationAnimation* pAnimation = new CPositionRotationAnimation (); //It's up to the caller do delete it if we return it

     if ( bResumeMode )
     {
          unsigned long ulElaspedTime, ulTimeLeft;
          if (  !a_rBitStream.ReadCompressed ( ulElaspedTime ) || !a_rBitStream.ReadCompressed ( ulTimeLeft ) )
          {
              delete pAnimation;
              return NULL;
          }          
          pAnimation->SetDuration ( ulElaspedTime, ulTimeLeft );
     }
     else
     {
         unsigned long ulDuration;
         if ( !a_rBitStream.ReadCompressed ( ulDuration) )
         {
             delete pAnimation;
             return NULL;
         }
         pAnimation->SetDuration ( ulDuration );
     }

      SPositionSync positionSync;
      SRotationRadiansSync rotationSync ( true );

      if ( !a_rBitStream.Read ( &positionSync ) || !a_rBitStream.Read ( &rotationSync ) )
      {
          delete pAnimation;
          return NULL;
      }
      pAnimation->SetSourceValue ( SPositionRotation ( positionSync.data.vecPosition, rotationSync.data.vecRotation ) );

      bool bDeltaRotationMode;
      if (  !a_rBitStream.Read ( &positionSync )        || 
            !a_rBitStream.ReadBit( bDeltaRotationMode ) ||
            !a_rBitStream.Read ( &rotationSync ) )
      {
          delete pAnimation;
          return NULL;
      }
      pAnimation->SetTargetValue ( SPositionRotation ( positionSync.data.vecPosition, rotationSync.data.vecRotation ), bDeltaRotationMode );

      std::string strEasingType;
      double fEasingPeriod, fEasingAmplitude, fEasingOvershoot;
      if (  !a_rBitStream.ReadString ( strEasingType)   ||
            !a_rBitStream.Read ( fEasingPeriod)         ||
            !a_rBitStream.Read ( fEasingAmplitude)      ||
            !a_rBitStream.Read ( fEasingOvershoot) )
      {
          delete pAnimation;
          return NULL;
      }

      CEasingCurve::eType easingType = CEasingCurve::GetEasingTypeFromString ( strEasingType );
      if ( easingType == CEasingCurve::EASING_INVALID )
      {
          // Server asks for a type I don't know, let's use oldschool linear then
          easingType = CEasingCurve::Linear;
      }
      pAnimation->SetEasing ( easingType, fEasingPeriod, fEasingAmplitude, fEasingOvershoot );
      
      return pAnimation;
 }
