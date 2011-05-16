/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.1
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        animation/CEasingCurve.cpp
*  PURPOSE:     Easing curves for non-linear time interpolation
*  DEVELOPERS:  Kayl
*
*****************************************************************************/

#include "StdInc.h"

#include "CEasingCurve.h"

#include "EasingEquations.cpp" //Included from cpp to separate just visually maths equations from framework

//For easing functions that require more parameters than time (like amplitude, period, overshot)
class CComplexEasingFunction
{
public: //All public since it's only an internal class
    enum eType
    {
        In = 0, 
        Out, 
        InOut, 
        OutIn
    };

    CComplexEasingFunction ( CComplexEasingFunction::eType a_eType = In, double period = 0.3, double amplitude = 1.0, double overshoot = 1.70158):
        m_eType ( a_eType ),
        m_fAmplitude ( amplitude ),
        m_fPeriod ( period ),
        m_fOvershoot ( overshoot)
    { }
    
    virtual double valueForProgress ( double a_fProgress ) const = 0;
    
    eType   m_eType;
    double  m_fAmplitude;
    double  m_fPeriod;
    double  m_fOvershoot;
};

class CElasticEase : public CComplexEasingFunction
{
public:
    CElasticEase( eType a_eType ):
        CComplexEasingFunction( a_eType, 0.3f, 1.0f )
    { }

    double valueForProgress ( double a_fProgress ) const
    {
        double fPeriod = (m_fPeriod < 0) ? 0.3 : m_fPeriod;
        double fAmplitude = (m_fAmplitude < 0) ? 1.0 : m_fAmplitude;
        switch ( m_eType )
        {
        case In:
            return easeInElastic ( a_fProgress, fAmplitude, fPeriod );
        case Out:
            return easeOutElastic ( a_fProgress, fAmplitude, fPeriod );
        case InOut:
            return easeInOutElastic ( a_fProgress, fAmplitude, fPeriod );
        case OutIn:
            return easeOutInElastic ( a_fProgress, fAmplitude, fPeriod );
        default:
            return a_fProgress;
        }
    }
};

class CBackEase : public CComplexEasingFunction
{
public:
    CBackEase( eType a_eType ):
        CComplexEasingFunction( a_eType, 0.3, 1.0, 1.70158 )
    { }

    double valueForProgress ( double a_fProgress ) const
    {
        double fOvershoot = (m_fOvershoot < 0) ? 1.70158 : m_fOvershoot;
        switch ( m_eType ) 
        {
        case In:
            return easeInBack(a_fProgress, fOvershoot);
        case Out:
            return easeOutBack(a_fProgress, fOvershoot);
        case InOut:
            return easeInOutBack(a_fProgress, fOvershoot);
        case OutIn:
            return easeOutInBack(a_fProgress, fOvershoot);
        default:
            return a_fProgress;
        }
    }
};

class CBounceEase : public CComplexEasingFunction
{
public:
    CBounceEase ( eType a_eType ): 
        CComplexEasingFunction( a_eType, 0.3, 1.0 )
    { }
      
    double valueForProgress ( double a_fProgress ) const
    {
        double fAmplitude = (m_fAmplitude < 0.0) ? 1.0 : m_fAmplitude;
        switch ( m_eType ) 
        {
        case In:
            return easeInBounce ( a_fProgress, fAmplitude );
        case Out:
            return easeOutBounce ( a_fProgress, fAmplitude );
        case InOut:
            return easeInOutBounce ( a_fProgress, fAmplitude );
        case OutIn:
            return easeOutInBounce ( a_fProgress, fAmplitude );
        default:
            return a_fProgress;
        }
    }
};

class CEasingCurve_Impl
{
public: //All public since it's only an internal class
    explicit CEasingCurve_Impl ( CEasingCurve::eType a_eType );
    ~CEasingCurve_Impl ();

    void setType ( CEasingCurve::eType a_eType );
    void setParams ( double a_fPeriod, double a_fAmplitude, double a_fOvershoot );
    void getParams ( double& a_rfPeriod, double& a_rfAmplitude, double& a_rfOvershoot ) const;
    
    CEasingCurve::eType                     m_eType;
    CEasingCurve::SimpleEasingFunction      m_pSimpleFunction;
    CComplexEasingFunction*                 m_pComplexFunction;
    bool                                    m_bFinishesAtOne; //f(1) = 1?   
};

CEasingCurve_Impl::CEasingCurve_Impl ( CEasingCurve::eType a_eType ):
    m_pSimpleFunction ( NULL ),
    m_pComplexFunction ( NULL )
{
    setType ( a_eType );
}

CEasingCurve_Impl::~CEasingCurve_Impl ()
{
    if ( m_pComplexFunction )
    {
        delete m_pComplexFunction;
        m_pComplexFunction = NULL;
    }
}

void CEasingCurve_Impl::setType ( CEasingCurve::eType a_eType )
{
    if ( m_eType == a_eType )
    {
        return;
    }

    m_eType = a_eType;
    m_bFinishesAtOne = true;
    m_pSimpleFunction = NULL;
    if ( m_pComplexFunction )
    {
        delete m_pComplexFunction;
        m_pComplexFunction = NULL;
    }

    switch ( m_eType )
    {
    case CEasingCurve::Linear:
        m_pSimpleFunction = &easeNone;
        break;
    case CEasingCurve::InQuad:
        m_pSimpleFunction = &easeInQuad;
        break;
    case CEasingCurve::OutQuad:
        m_pSimpleFunction = &easeOutQuad;
        break;
    case CEasingCurve::InOutQuad:
        m_pSimpleFunction = &easeInOutQuad;
        break;
    case CEasingCurve::OutInQuad:
        m_pSimpleFunction = &easeOutInQuad;
        break;
    case CEasingCurve::InElastic:
        m_pComplexFunction = new CElasticEase ( CComplexEasingFunction::In );
        break;
    case CEasingCurve::OutElastic:
        m_pComplexFunction = new CElasticEase ( CComplexEasingFunction::Out );
        break;
    case CEasingCurve::InOutElastic:
        m_pComplexFunction = new CElasticEase ( CComplexEasingFunction::InOut );
        break;
    case CEasingCurve::OutInElastic:
        m_pComplexFunction = new CElasticEase ( CComplexEasingFunction::OutIn );
        break;
    case CEasingCurve::InBack:
        m_pComplexFunction = new CBackEase ( CComplexEasingFunction::In );
        break;
    case CEasingCurve::OutBack:
        m_pComplexFunction = new CBackEase ( CComplexEasingFunction::Out );
        break;
    case CEasingCurve::InOutBack:
        m_pComplexFunction = new CBackEase ( CComplexEasingFunction::InOut );
        break;
    case CEasingCurve::OutInBack:
        m_pComplexFunction = new CBackEase ( CComplexEasingFunction::OutIn );
        break;
    case CEasingCurve::InBounce:
        m_pComplexFunction = new CBounceEase ( CComplexEasingFunction::In );
        break;
    case CEasingCurve::OutBounce:
        m_pComplexFunction = new CBounceEase ( CComplexEasingFunction::Out );
        break;
    case CEasingCurve::InOutBounce:
        m_pComplexFunction = new CBounceEase ( CComplexEasingFunction::InOut );
        break;
    case CEasingCurve::OutInBounce:
        m_pComplexFunction = new CBounceEase ( CComplexEasingFunction::OutIn );
        break;
    case CEasingCurve::SineCurve:
        m_pSimpleFunction = &easeSineCurve;
        m_bFinishesAtOne = false;
        break;
    case CEasingCurve::CosineCurve:
        m_pSimpleFunction = &easeCosineCurve;
        m_bFinishesAtOne = false;
        break;
    default:
        break;
    }
}

void CEasingCurve_Impl::setParams ( double a_fPeriod, double a_fAmplitude, double a_fOvershoot )
{
    if ( m_pComplexFunction )
    {
        m_pComplexFunction->m_fPeriod = a_fPeriod;
        m_pComplexFunction->m_fAmplitude = a_fAmplitude;
        m_pComplexFunction->m_fOvershoot = a_fOvershoot;
    }
}

void CEasingCurve_Impl::getParams ( double& a_rfPeriod, double& a_rfAmplitude, double& a_rfOvershoot ) const
{
    if ( m_pComplexFunction )
    {
        a_rfPeriod = m_pComplexFunction->m_fPeriod;
        a_rfAmplitude = m_pComplexFunction->m_fAmplitude;
        a_rfOvershoot = m_pComplexFunction->m_fOvershoot;
    }
    else
    {
        a_rfPeriod = 0.0;
        a_rfAmplitude = 0.0;
        a_rfOvershoot = 0.0;
    }
}

CEasingCurve::CEasingCurve ( eType a_eType )
{
    m_pImplementation = new CEasingCurve_Impl ( a_eType );
}

CEasingCurve::CEasingCurve ( const CEasingCurve& a_rfOther )
{
    m_pImplementation = new CEasingCurve_Impl ( a_rfOther.GetType () );
    double fPeriod, fAmplitude, fOvershoot;
    a_rfOther.GetParams ( fPeriod, fAmplitude, fOvershoot );
    SetParams ( fPeriod, fAmplitude, fOvershoot );
}

CEasingCurve&  CEasingCurve::operator=( const CEasingCurve& a_rfOther )
{
    SetType ( a_rfOther.GetType () );
    double fPeriod, fAmplitude, fOvershoot;
    a_rfOther.GetParams ( fPeriod, fAmplitude, fOvershoot );
    SetParams ( fPeriod, fAmplitude, fOvershoot );
    return *this;
}

CEasingCurve::~CEasingCurve ()
{
    delete m_pImplementation;
}

static std::map<std::string, CEasingCurve::eType> s_mStringToType;
static std::map<CEasingCurve::eType, std::string> s_mTypeToString;

void declareStringVersionsOfEasingTypes ()
{
    s_mStringToType.clear();
    s_mTypeToString.clear();
#define DECLARE_EASING(x)   s_mStringToType[""#x""] = CEasingCurve::x; s_mTypeToString[CEasingCurve::x] = ""#x"";
#include "EasingDeclarations.hpp"
#undef DECLARE_EASING
}

CEasingCurve::eType CEasingCurve::GetEasingTypeFromString ( const std::string& a_rstrType )
{
    if ( s_mStringToType.empty() )
    {
        declareStringVersionsOfEasingTypes ();
    }
    std::map<std::string, CEasingCurve::eType>::const_iterator foundType = s_mStringToType.find( a_rstrType );
    if ( foundType == s_mStringToType.end() )
    {
        return EASING_INVALID;
    }
    else
    {
        return foundType->second;
    }
}
 
std::string CEasingCurve::GetStringFromEasingType ( CEasingCurve::eType a_eType )
{
    if (s_mTypeToString.empty())
    {
        declareStringVersionsOfEasingTypes ();
    }
    std::map<CEasingCurve::eType, std::string>::const_iterator foundType = s_mTypeToString.find( a_eType );
    if ( foundType == s_mTypeToString.end() )
    {
        return "Invalid";
    }
    else
    {
        return foundType->second;
    }
}

void CEasingCurve::SetType ( eType a_eType )
{
    m_pImplementation->setType ( a_eType );
}

CEasingCurve::eType CEasingCurve::GetType ( ) const
{
    return m_pImplementation->m_eType;
}

void CEasingCurve::SetParams ( double a_fPeriod, double a_fAmplitude, double a_fOvershoot )
{
    m_pImplementation->setParams ( a_fPeriod, a_fAmplitude, a_fOvershoot );
}

void CEasingCurve::GetParams ( double& a_rfPeriod, double& a_rfAmplitude, double& a_rfOvershoot ) const
{
    m_pImplementation->getParams ( a_rfPeriod, a_rfAmplitude, a_rfOvershoot );
}

float CEasingCurve::ValueForProgress( float progress ) const
{
    if ( m_pImplementation->m_pComplexFunction )
    {
        return static_cast<float> ( m_pImplementation->m_pComplexFunction->valueForProgress ( progress ) );
    }
    else if ( m_pImplementation->m_pSimpleFunction )
    {
        return  static_cast<float> ( m_pImplementation->m_pSimpleFunction ( progress ) );
    }
    else
    {
        return progress;
    }
}

bool CEasingCurve::IsTargetValueFinalValue ( ) const
{
    return m_pImplementation->m_bFinishesAtOne;
}