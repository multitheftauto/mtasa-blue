/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CAclRightName.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


//
// Encapsulate a right name as a name and type
//
class CAclRightName
{
public:
    // Construct from the full name "type.name"
    explicit CAclRightName ( const SString& strRightFullName )
    {
        m_strFullName = strRightFullName;
        SString strTypePart;
        if ( !m_strFullName.Split ( ".", &strTypePart, &m_strNamePart ) )
            m_bValid = false;
        else
            m_bValid = StringToEnum( strTypePart, m_TypePart );
    }

    // Construct from the type and name
    CAclRightName ( CAccessControlListRight::ERightType typePart, const SString& strNamePart )
    {
        m_strNamePart = strNamePart;
        m_TypePart = typePart;
        m_strFullName = EnumToString( m_TypePart ) + "." + m_strNamePart;
        m_bValid = ( m_TypePart >= CAccessControlListRight::RIGHT_TYPE_COMMAND && m_TypePart <= CAccessControlListRight::RIGHT_TYPE_GENERAL );
    }

    bool operator==( const CAclRightName& other ) const
    {
        return m_strFullName == other.m_strFullName;
    }

    bool                                IsValid     ( void ) const      { return m_bValid; }
    const SString&                      GetName     ( void ) const      { return m_strNamePart; }
    CAccessControlListRight::ERightType GetType     ( void ) const      { return m_TypePart; }
    const SString&                      GetFullName ( void ) const      { return m_strFullName; }

protected:
    SString                             m_strNamePart;
    CAccessControlListRight::ERightType m_TypePart;
    SString                             m_strFullName;
    bool                                m_bValid;
};


//
// A single acl request for a resource
//
struct SAclRequest
{
    SAclRequest ( const CAclRightName& rightName ) : rightName ( rightName ), bAccess ( false ), bPending ( false ) {}

    CAclRightName   rightName;
    bool            bAccess;
    bool            bPending;
    SString         strWho;
    SString         strDate;
};
