/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        CExePatchedStatus.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

struct SExePatchedStatus
{
    bool bTimestamp;
    bool bLargeMem;
    bool bDep;
    bool bNvightmare;
    bool bAltModules;

    bool operator!= ( const SExePatchedStatus& other ) const    { return !operator==( other ); }
    bool operator== ( const SExePatchedStatus& other ) const
    {
        return bTimestamp == other.bTimestamp
               && bLargeMem == other.bLargeMem
               && bDep == other.bDep
               && bNvightmare == other.bNvightmare
               && bAltModules == other.bAltModules;
    }
};

enum EPatchMode
{
    PATCH_SET_OFF = 1,
    PATCH_SET_ON,
    PATCH_CHECK,
};

enum EPatchResult
{
    PATCH_SET_RESULT_REQ_ADMIN = 1,
    PATCH_SET_RESULT_OK,
    PATCH_CHECK_RESULT_OFF,
    PATCH_CHECK_RESULT_ON,
};

SExePatchedStatus   GetExePatchedStatus             ( bool bUseExeCopy );
SExePatchedStatus   GetExePatchRequirements         ( void );
bool                SetExePatchedStatus             ( bool bUseExeCopy, const SExePatchedStatus& reqStatus );
bool                ShouldUseExeCopy                ( void );
SString             GetPatchExeAdminReason          ( bool bUseExeCopy, const SExePatchedStatus& reqStatus );
uint                GetExeFileSize                  ( bool bUseExeCopy );
bool                CopyExe                         ( void );
SString             GetExePathFilename              ( bool bUseExeCopy );
SString             GetUsingExePathFilename         ( void );
bool                GetPatchRequirementAltModules   ( void );
EPatchResult        UpdatePatchStatusNvightmare     ( const SString& strGTAEXEPath, EPatchMode mode );
EPatchResult        UpdatePatchStatusTimestamp      ( const SString& strGTAEXEPath, EPatchMode mode );
EPatchResult        UpdatePatchStatusLargeMem       ( const SString& strGTAEXEPath, EPatchMode mode );
EPatchResult        UpdatePatchStatusDep            ( const SString& strGTAEXEPath, EPatchMode mode );
bool                GetPatchRequirementAltModules   ( void );
EPatchResult        UpdatePatchStatusAltModules     ( const SString& strGTAEXEPath, EPatchMode mode );
