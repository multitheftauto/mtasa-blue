/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.1
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        animation/CEasingCurve.h
*  PURPOSE:     Easing curves for non-linear time interpolation
*  DEVELOPERS:  Kayl
*
*****************************************************************************/

/****************************************************************************
** Derived from LGPL code from Qt Framework (QEasingCurve)
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
****************************************************************************/

class CEasingCurve;

#ifndef __CEASINGCURVE_H
#define __CEASINGCURVE_H

//Real implementation class defined in .cpp
class CEasingCurve_Impl;

//User friendly interface
class CEasingCurve
{
public:

    enum eType
    {
        Linear,

        InQuad,
        OutQuad,
        InOutQuad,
        OutInQuad,

        InElastic,
        OutElastic,
        InOutElastic,
        OutInElastic,

        InBack,
        OutBack,
        InOutBack,
        OutInBack,

        InBounce,
        OutBounce,

        InOutBounce,
        OutInBounce,

        SineCurve,
        CosineCurve,

        EASING_INVALID = 0xFF
    };

    typedef double (*SimpleEasingFunction)(double progress);
    
                        CEasingCurve ( eType a_eType = Linear );
    
                        CEasingCurve ( const CEasingCurve& a_rfOther );
    CEasingCurve&       operator=( const CEasingCurve& a_rfOther );

                        ~CEasingCurve ();

    static eType        GetEasingTypeFromString ( const std::string& a_rstrType );
    static std::string  GetStringFromEasingType ( eType a_eType );

    void                SetType ( eType a_eType );
    eType               GetType ( ) const;

    void                SetParams ( double a_fPeriod, double a_fAmplitude, double a_fOvershoot );
    void                GetParams ( double& a_rfPeriod, double& a_rfAmplitude, double& a_rfOvershoot ) const;
    
    float               ValueForProgress ( float progress ) const;
    bool                IsTargetValueFinalValue () const; //at t=1 can we use target as the final value (false for sin & cos)
protected:
    CEasingCurve_Impl*  m_pImplementation;
};

#endif
