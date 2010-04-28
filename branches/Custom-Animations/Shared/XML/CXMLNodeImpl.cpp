/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLNodeImpl.cpp
*  PURPOSE:     XML node class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CXMLNodeImpl::CXMLNodeImpl ( CXMLFileImpl* pFile, CXMLNodeImpl* pParent, TiXmlElement& Node ) :
    m_pNode ( &Node ),
    m_Attributes ( Node )
{
    // Init
    m_pFile = pFile;
    m_pParent = pParent;
    m_bCanRemoveFromList = true;

    if ( m_pFile )
        m_pDocument = pFile->GetDocument ();
    else
        m_pDocument = NULL;

    // Add to parent list
    if ( m_pParent )
    {
        m_pParent->AddToList ( this );
    }

    // Add to array over XML stuff
    m_ulID = CXMLArray::PopUniqueID ( this );
}


CXMLNodeImpl::~CXMLNodeImpl ( void )
{
    // Remove from array over XML stuff
    CXMLArray::PushUniqueID ( this );

    // Delete our children
    DeleteAllSubNodes ();

    // We need a parent to delete the node
    if ( m_pParent )
    {
        // Remove from parent list
        m_pParent->RemoveFromList ( this );
    }
    else
    {
        // NULL it in the file if any
        if ( m_pFile )
        {
            m_pFile->m_pRootNode = NULL;
        }
    }

    // Need to delete the node?
    if ( m_pNode )
    {
        // Grab the parent of our node and delete it using that if any to prevent crashing.
        // Otherwize delete it directly.
        TiXmlNode* pParent = m_pNode->Parent ();
        if ( pParent )
            pParent->RemoveChild ( m_pNode );
        else
            delete m_pNode;
    }
}


CXMLNode* CXMLNodeImpl::CreateSubNode ( const char* szTagName )
{
    // Create a new subnode after the last child
    TiXmlElement* pNewNode = new TiXmlElement ( szTagName );
    m_pNode->LinkEndChild ( pNewNode );

    // Create and return the wrapper element
    CXMLNode* xmlNode = new CXMLNodeImpl ( m_pFile, this, *pNewNode );
    if ( xmlNode->IsValid( ) )
        return xmlNode;
    else
    {
        delete xmlNode;
        return NULL;
    }
}


void CXMLNodeImpl::DeleteAllSubNodes ( void )
{
    // Don't let the nodes remove themselves from this list
    m_bCanRemoveFromList = false;

    // Delete each item in the list
    list < CXMLNode* > ::iterator iter;
    for ( iter = m_Children.begin (); iter != m_Children.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_bCanRemoveFromList = true;
    m_Children.clear ();
}


unsigned int CXMLNodeImpl::GetSubNodeCount ( void )
{
    return static_cast < unsigned int > ( m_Children.size () );
}


CXMLNode* CXMLNodeImpl::GetSubNode ( unsigned int uiIndex )
{
    // Find the given index in the list
    unsigned int uiTemp = 0;
    list < CXMLNode* > ::iterator iter;
    for ( iter = m_Children.begin (); iter != m_Children.end (); iter++ )
    {
        if ( uiIndex == uiTemp )
        {
            return *iter;
        }

        ++uiTemp;
    }
    // Couldn't find it
    return NULL;
}


CXMLNode* CXMLNodeImpl::FindSubNode ( const char* szTagName, unsigned int uiIndex )
{
    std::string TagName ( szTagName );

    // Find the item with the given name
    unsigned int uiTemp = 0;
    list < CXMLNode* > ::iterator iter;
    for ( iter = m_Children.begin (); iter != m_Children.end (); iter++ )
    {
        if ( dynamic_cast < CXMLNodeImpl* > ( (*iter) )->GetNode ()->ValueStr () == szTagName )
        {
            if ( uiTemp == uiIndex )
            {
                return *iter;
            }
            
            ++uiTemp;
        }
    }

    // Couldn't find it
    return NULL;
}


CXMLAttributes& CXMLNodeImpl::GetAttributes ( void )
{
    return m_Attributes;
}


CXMLNode* CXMLNodeImpl::GetParent ( void )
{
    return m_pParent;
}


int CXMLNodeImpl::GetLine ( void )
{
    return m_pNode->Row ();
}


const std::string& CXMLNodeImpl::GetTagName ( void )
{
    return m_pNode->ValueStr ();
}


void CXMLNodeImpl::SetTagName ( const std::string& strString )
{
    m_pNode->SetValue ( strString );
}


const std::string CXMLNodeImpl::GetTagContent ( void )
{
    // FIXME: This can be sort of misleading as sub-tags are not included
    const char* szSubText = m_pNode->GetText ();
    return szSubText ? std::string ( szSubText ) : std::string ( "" );
}


bool CXMLNodeImpl::GetTagContent ( bool& bContent )
{
    // Get boolean value
    const char* szText = m_pNode->GetText ();
    if ( !szText )
        return false;

    if ( m_pNode->GetText () [0] == '1' )
    {
        bContent = true;
        return true;
    }
    else if ( m_pNode->GetText () [0] == '0' )
    {
        bContent = false;
        return true;
    }

    return false;   // Invalid
}


bool CXMLNodeImpl::GetTagContent ( int& iContent )
{
    long lValue;

    const char* szText = m_pNode->GetText ();
    if ( !szText )
        return false;

    if ( !StringToLong ( szText, lValue ) )
        return false;

    // Check the range
    if ( lValue < INT_MIN || lValue > INT_MAX )
        return false;
    else
    {
        iContent = static_cast < int > ( lValue );
        return true;
    }
}


bool CXMLNodeImpl::GetTagContent ( unsigned int& uiContent )
{
    long lValue;

    const char* szText = m_pNode->GetText ();
    if ( !szText )
        return false;

    if ( !StringToLong ( szText, lValue ) )
        return false;

    // Check the range
    if ( lValue < 0 || lValue > UINT_MAX )
        return false;
    else
    {
        uiContent = static_cast < unsigned int > ( lValue );
        return true;
    }
}


bool CXMLNodeImpl::GetTagContent ( float& fContent )
{
    const char* szText = m_pNode->GetText ();
    if ( !szText )
        return false;

    fContent = static_cast < float > ( atof ( szText ) );
    return true;
}


void CXMLNodeImpl::SetTagContent ( const char* szText )
{
    m_pNode->Clear();
    TiXmlText* pNewNode = new TiXmlText ( szText );
    m_pNode->LinkEndChild ( pNewNode );
}


void CXMLNodeImpl::SetTagContent ( bool bContent )
{
    // Convert to string and set it
    char szBuffer[2];
    szBuffer[1] = 0;
    if ( bContent ) szBuffer[0] = '1';
    else szBuffer[0] = '0';

    SetTagContent ( szBuffer );
}


void CXMLNodeImpl::SetTagContent ( int iContent )
{
    // Convert to string and set it
    char szBuffer[40];
    sprintf ( szBuffer, "%i", iContent );
    SetTagContent ( szBuffer );
}


void CXMLNodeImpl::SetTagContent ( unsigned int uiContent )
{
    // Convert to string and set it
    char szBuffer [40];
    sprintf ( szBuffer, "%u", uiContent );
    SetTagContent ( szBuffer );
}


void CXMLNodeImpl::SetTagContent ( float fContent )
{
    // Convert to string and set it
    char szBuffer [40];
    sprintf ( szBuffer, "%f", fContent );
    SetTagContent ( szBuffer );
}


void CXMLNodeImpl::SetTagContentf ( const char* szFormat, ... )
{
    // Convert the formatted string to a string (MAX 1024 BYTES) and set it
    char szBuffer [1024];
    va_list va;
    va_start ( va, szFormat );
    _VSNPRINTF ( szBuffer, 1024, szFormat, va );
    va_end ( va );
    SetTagContent ( szBuffer );
}


TiXmlElement* CXMLNodeImpl::GetNode ( void )
{
    return m_pNode;
}


CXMLNode* CXMLNodeImpl::CopyNode ( CXMLNode *pParent )
{
    CXMLNodeImpl *pNew = new CXMLNodeImpl ( NULL, reinterpret_cast < CXMLNodeImpl* > ( pParent ), *m_pNode->Clone ()->ToElement () );

    // Copy the list, so we don't end up in an endless loop
    std::list < CXMLNode* > ChildrenCopy ( m_Children );

    // Recursively copy each child
    list < CXMLNode* > ::iterator iter;
    for ( iter = ChildrenCopy.begin (); iter != ChildrenCopy.end (); iter++ )
    {
        pNew->AddToList ( (*iter)->CopyNode ( pNew ) );
    }

    return dynamic_cast < CXMLNode* > ( pNew );
}


bool CXMLNodeImpl::CopyChildrenInto ( CXMLNode* pDestination, bool bRecursive )
{
    // Delete all the children of the target
    pDestination->DeleteAllSubNodes ();

    // Iterate through our children if we have. Otherwize just copy the content
    if ( m_Children.size () > 0 )
    {
        std::list < CXMLNode* > ::iterator iter = m_Children.begin ();
        CXMLNode* pMyChildNode;
        CXMLNode* pNewChildNode;
        for ( ; iter != m_Children.end (); iter++ )
        {
            // Grab its tag name
            pMyChildNode = *iter;
            const std::string& strTagName = pMyChildNode->GetTagName ();

            // Create at the destination
            pNewChildNode = pDestination->CreateSubNode ( strTagName.c_str () );
            if ( pNewChildNode )
            {
                // Copy our child's attributes into it
                int iAttributeCount = pMyChildNode->GetAttributes ().Count ();
                int i = 0;
                CXMLAttribute* pAttribute;
                for ( ; i < iAttributeCount; i++ )
                {
                    // Create a copy of every attribute into our destination
                    pAttribute = pMyChildNode->GetAttributes ().Get ( i );
                    if ( pAttribute )
                    {
                        pNewChildNode->GetAttributes ().Create ( *pAttribute );
                    }
                }

                // Run it recursively if asked to. Copy child child nodes etc..
                if ( bRecursive )
                {
                    if ( !pMyChildNode->CopyChildrenInto ( pNewChildNode, true ) )
                    {
                        pDestination->DeleteAllSubNodes ();
                        return false;
                    }
                }
            }
            else
            {
                pDestination->DeleteAllSubNodes ();
                return false;
            }
        }
    }
    else
    {
        const char* szText = m_pNode->GetText ();
        if ( szText )
            pDestination->SetTagContent ( szText );
    }

    // Success
    return true;
}


void CXMLNodeImpl::DeleteWrapper ( void )
{
    // Don't let the nodes remove themselves from this list
    m_bCanRemoveFromList = false;

    // Delete each wrapper item in the list
    list < CXMLNode* > ::iterator iter;
    for ( iter = m_Children.begin (); iter != m_Children.end (); iter++ )
    {
        reinterpret_cast < CXMLNodeImpl* > (*iter)->DeleteWrapper ();
    }

    // Clear the list
    m_bCanRemoveFromList = true;
    m_Children.clear ();

    // Prevent our destructor from deleting the node
    m_pNode = NULL;

    // Remove from parent list
    if ( m_pParent )
    {
        m_pParent->RemoveFromList ( this );
        m_pParent = NULL;
    }

    // Delete us
    delete this;
}


void CXMLNodeImpl::AddToList ( CXMLNode* pNode )
{
    m_Children.push_back ( pNode );
}


void CXMLNodeImpl::RemoveFromList ( CXMLNode* pNode )
{
    if ( m_bCanRemoveFromList )
    {
        if ( !m_Children.empty() ) m_Children.remove ( pNode );
    }
}


void CXMLNodeImpl::RemoveAllFromList ( void )
{
    m_Children.clear ();
}


bool CXMLNodeImpl::StringToLong ( const char* szString, long& lValue )
{
    // Convert to string
    char szBuffer [33];
    assert ( strlen ( szString ) < 32 );
    char* pEnd;
    strcpy ( szBuffer, szString );
    lValue = strtol ( szBuffer, &pEnd, 0 );

#ifndef WIN32
    if ( ERANGE == errno )
    {
        return false;
    }
    else
    if ( pEnd == szBuffer )
    {
        return false;
    }
    else
#endif
    {
        return true;
    }
}

