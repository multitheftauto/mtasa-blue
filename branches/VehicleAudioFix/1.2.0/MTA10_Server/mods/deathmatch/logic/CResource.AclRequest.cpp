/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResource.AclRequest.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


///////////////////////////////////////////////////////////////
//
// CResource::GetAutoGroupName
//
// Get group name for aclrequest acl
//
///////////////////////////////////////////////////////////////
SString CResource::GetAutoGroupName ( void )
{
    return SString ( "autoGroup_%s", GetName ().c_str () );
}


///////////////////////////////////////////////////////////////
//
// CResource::GetAutoAclName
//
// Get acl name for aclrequest items
//
///////////////////////////////////////////////////////////////
SString CResource::GetAutoAclName ( void )
{
    return SString ( "autoACL_%s", GetName ().c_str () );
}


///////////////////////////////////////////////////////////////
//
// CResource::GetAutoAcl
//
// Get acl for aclrequest items
// Can't fail
//
///////////////////////////////////////////////////////////////
CAccessControlList* CResource::GetAutoAcl ( void )
{
    return g_pGame->GetACLManager()->AddACL ( GetAutoAclName () );
}


///////////////////////////////////////////////////////////////
//
// CResource::FindAutoAcl
//
// Get acl for aclrequest items
// Will return NULL if it does not exist
//
///////////////////////////////////////////////////////////////
CAccessControlList* CResource::FindAutoAcl ( void )
{
    return g_pGame->GetACLManager()->GetACL ( GetAutoAclName () );
}



///////////////////////////////////////////////////////////////
//
// CResource::RemoveAutoPermissions
//
// Remove group and acl used for aclrequest items
//
///////////////////////////////////////////////////////////////
void CResource::RemoveAutoPermissions ( void )
{
    CAccessControlListManager * pACLManager = g_pGame->GetACLManager();

    // Remove auto group by name
    if ( CAccessControlListGroup* pGroup = pACLManager->GetGroup ( GetAutoGroupName () ) )
        pACLManager->DeleteGroup ( pGroup );

    // Remove auto acl by name
    if ( CAccessControlList* pAcl = pACLManager->GetACL ( GetAutoAclName () ) )
        pACLManager->DeleteACL ( pAcl );
}


///////////////////////////////////////////////////////////////
//
// CResource::RefreshAutoPermissions
//
// Update group and acl used aclrequest items
//
///////////////////////////////////////////////////////////////
bool CResource::RefreshAutoPermissions ( CXMLNode* pNodeAclRequest )
{
    // Ensure group and acl exist
    CAccessControlListGroup* pAutoGroup = g_pGame->GetACLManager()->AddGroup ( GetAutoGroupName () );
    pAutoGroup->AddACL ( GetAutoAcl () );
    pAutoGroup->AddObject ( GetName ().c_str (), CAccessControlListGroupObject::OBJECT_TYPE_RESOURCE );

    // Track unused right names
    std::vector < CAclRightName > unusedRightNameMap;
    std::vector < SAclRequest > unusedRequestList;
    GetAclRequests ( unusedRequestList );
    for ( uint i = 0 ; i < unusedRequestList.size () ; i++ )
       unusedRightNameMap.push_back ( unusedRequestList[i].rightName );

    // Track any pending requests
    bool bHasPending = false;

    for ( uint uiIndex = 0 ; true ; uiIndex++ )
    {
        CXMLNode* pNodeRight = pNodeAclRequest->FindSubNode ( "right", uiIndex );
        if ( !pNodeRight )
            break;

        // Find existing
        SAclRequest request ( CAclRightName ( pNodeRight->GetAttributeValue ( "name" ) ) );
        if ( !FindAclRequest ( request ) )
        {
            // Add new request
            request.bAccess = false;
            request.bPending = true;
            request.strWho = "";
            request.strDate = "";

            // Validate request
            if ( !request.rightName.IsValid () || !StringToBool ( pNodeRight->GetAttributeValue ( "access" ) ) )
            {
                CLogger::ErrorPrintf ( "Invalid aclrequest line in %s (%s)\n", GetName ().c_str (), *request.rightName.GetFullName () );
                return false;
            }

            CommitAclRequest ( request );
        }

        // This right is used
        ListRemove ( unusedRightNameMap, request.rightName );

        // Update flag
        bHasPending |= request.bPending;
    }

    // Remove rights not requested
    for ( std::vector < CAclRightName >::iterator iter = unusedRightNameMap.begin () ; iter != unusedRightNameMap.end () ; ++iter )
        GetAutoAcl ()->RemoveRight ( iter->GetName (), iter->GetType () );

    // If any rights are pending, print message
    if ( bHasPending )
    {
        CLogger::LogPrintf ( "Resource '%s' requests some acl rights. Use the command 'aclrequest review %s'\n", GetName ().c_str (), GetName ().c_str () );
    }

    return bHasPending;
}


///////////////////////////////////////////////////////////////
//
// CResource::HandleAclRequestListCommand
//
//
//
///////////////////////////////////////////////////////////////
bool CResource::HandleAclRequestListCommand ( bool bDetail )
{
    uint uiNumPending = 0;
    uint uiNumTotal = 0;

    // Get each request
    std::vector < SAclRequest > currentRequestList;
    GetAclRequests ( currentRequestList );
    for ( std::vector < SAclRequest >::iterator iter = currentRequestList.begin () ; iter != currentRequestList.end () ; ++iter )
    {
        const SAclRequest& request = *iter;

        if ( request.bPending )
            uiNumPending++;
        uiNumTotal++;

        if ( bDetail )
        {
            SString strStatus = request.bPending ? "pending" : request.bAccess ? "allow" : "deny";
            SString strOutput;
            strOutput += SString( "aclrequest: %s", GetName ().c_str () );
            strOutput += SString( " [%s] for %s", *strStatus, *request.rightName.GetFullName () );
            if ( !request.bPending )
                strOutput += SString ( " (by %s on %s)", *request.strWho , *request.strDate );
            CLogger::LogPrintf ( strOutput + "\n" );
        }
    }

    if ( !bDetail && uiNumTotal > 0 )
    {
        SString strOutput;
        strOutput += SString( "aclrequest: %s", GetName ().c_str () );
        strOutput += SString( " has %d aclrequest(s) of which %d are pending", uiNumTotal, uiNumPending );
        CLogger::LogPrintf ( strOutput + "\n" );
    }

    return uiNumTotal > 0;
}


///////////////////////////////////////////////////////////////
//
// CResource::HandleAclRequestChangeCommand
//
// Returns true if any acl request was changed
//
///////////////////////////////////////////////////////////////
bool CResource::HandleAclRequestChangeCommand ( const SString& strRightFullName, bool bAccess, const SString& strWho )
{
    if ( strRightFullName != "all" )
    {
        return HandleAclRequestChange ( CAclRightName ( strRightFullName ), bAccess, strWho );
    }

    bool bChanged = false;

    // Try to change every acl request
    std::vector < SAclRequest > currentRequestList;
    GetAclRequests ( currentRequestList );
    for ( std::vector < SAclRequest >::iterator iter = currentRequestList.begin () ; iter != currentRequestList.end () ; ++iter )
    {
        bChanged |= HandleAclRequestChange ( iter->rightName, bAccess, strWho );
    }

    return bChanged;
}


///////////////////////////////////////////////////////////////
//
// CResource::HandleAclRequestChange
//
// Update an existing valid acl request
// Returns true if the acl request was changed
//
///////////////////////////////////////////////////////////////
bool CResource::HandleAclRequestChange ( const CAclRightName& rightName, bool bAccess, const SString& strWho )
{
    // Get current request
    SAclRequest currentRequest ( rightName );
    if ( !FindAclRequest ( currentRequest ) )
        return false;

    // Check access is changing
    if ( !currentRequest.bPending && currentRequest.bAccess == bAccess )
        return false;

    // Update
    currentRequest.bAccess = bAccess;
    currentRequest.bPending = false;
    currentRequest.strWho = strWho;
    currentRequest.strDate = GetLocalTimeString ( true );
    CommitAclRequest ( currentRequest );

    SString strOutput;
    strOutput += SString( "aclrequest: %s", GetName ().c_str () );
    strOutput += SString( " %s changed to %s (%s)", *rightName.GetFullName (), bAccess ? "allow" : "deny", *strWho );
    CLogger::LogPrintf ( strOutput + "\n" );
    return true;
}


///////////////////////////////////////////////////////////////
//
// CResource::GetAclRequests
//
// Get all acl requests for this resource
//
///////////////////////////////////////////////////////////////
void CResource::GetAclRequests ( std::vector < SAclRequest >& outResultList )
{
    outResultList.clear ();

    CAccessControlList* pAutoAcl = FindAutoAcl ();
    if ( !pAutoAcl )
        return;

    // Get each right
    for ( std::list < CAccessControlListRight* >::const_iterator iter = pAutoAcl->IterBegin () ; iter != pAutoAcl->IterEnd () ; ++iter )
    {
        CAccessControlListRight* pAclRight = *iter;

        // Create SAclRequest from ACL
        SAclRequest request ( CAclRightName ( pAclRight->GetRightType (), pAclRight->GetRightName () ) );
        request.bAccess = StringToBool ( pAclRight->GetAttributeValue ( "access" ) );
        request.bPending = StringToBool ( pAclRight->GetAttributeValue ( "pending" ) );
        request.strWho = pAclRight->GetAttributeValue ( "who" );
        request.strDate = pAclRight->GetAttributeValue ( "date" );

        outResultList.push_back ( request );
    }
}


///////////////////////////////////////////////////////////////
//
// CResource::CommitAclRequest
//
// Right will be created if not exists
// Can't fail
//
///////////////////////////////////////////////////////////////
void CResource::CommitAclRequest ( const SAclRequest& request )
{
    CAccessControlListRight* pAclRight = GetAutoAcl ()->AddRight ( request.rightName.GetName (), request.rightName.GetType (), request.bAccess );

    pAclRight->SetRightAccess ( request.bAccess );
    pAclRight->SetAttributeValue ( "pending", request.bPending ? "true" : "false" );
    pAclRight->SetAttributeValue ( "who", request.strWho );
    pAclRight->SetAttributeValue ( "date", request.strDate );
}


///////////////////////////////////////////////////////////////
//
// CResource::FindAclRequest
//
// Will fail if right does not have a pending attribute
//
///////////////////////////////////////////////////////////////
bool CResource::FindAclRequest ( SAclRequest& result )
{
    if ( !FindAutoAcl () )
        return false;

    CAccessControlListRight* pAclRight = GetAutoAcl ()->GetRight ( result.rightName.GetName (), result.rightName.GetType () );
    if ( !pAclRight )
        return false;

    // Fill SAclRequest
    result.bAccess = StringToBool ( pAclRight->GetAttributeValue ( "access" ) );
    result.bPending = StringToBool ( pAclRight->GetAttributeValue ( "pending" ) );
    result.strWho = pAclRight->GetAttributeValue ( "who" );
    result.strDate = pAclRight->GetAttributeValue ( "date" );

    // Ensure not pending and allow
    if ( result.bPending && result.bAccess )
    {
        result.bAccess = false;
        CommitAclRequest ( result );
    }

    return pAclRight->GetAttributeValue ( "pending" ) != "";
}
