/***************************************

Based on viewacls by iain patterson
https://iain.cx/articles/acls/

****************************************/

#include "StdInc.h"
#include <Sddl.h>

#define Log1(x) OutputDebugString ( x )
#define Log(x) OutputDebugString ( SString x )
#define fLog(a,x) OutputDebugString ( SString x )
#define fLog1(a,x) OutputDebugString ( x )

class Blah
{
public:

static SECURITY_DESCRIPTOR *get_security_descriptor(char *filename) {
  /* First find out the amount of memory needed for the security descriptor */
  SECURITY_DESCRIPTOR *sd;
  unsigned long size = 0;

  GetFileSecurity(filename, DACL_SECURITY_INFORMATION, 0, 0, &size);
    Log1(("\n"));

  /* We should have failed GetFileSecurity with an out of memory error */
  int ret = GetLastError();
  if (ret != ERROR_INSUFFICIENT_BUFFER) {
    OutputDebugString( SString ( "Error getting file security DACL: %d\n", ret));
    Log(( "Error getting file security DACL: %d\n", ret));
    return 0;
  }

  /* Allocate memory for security descriptor */
  sd = (SECURITY_DESCRIPTOR *) HeapAlloc(GetProcessHeap(), 0, size);
  if (! sd) {
    fLog1(stderr,( "Out of memory for security descriptor!\n"));
    return 0;
  }

  /* Now perform the actual GetFileSecurity() call */
  if (! GetFileSecurity(filename, DACL_SECURITY_INFORMATION, sd, size, &size)) {
    fLog(stderr,( "Error getting file security DACL: %d\n", GetLastError()));
    HeapFree(GetProcessHeap(), 0, sd);
    return 0;
  }

  return sd;
}

/* Security descriptor needs to have DACL_SECURITY_INFORMATION */
static ACL *get_dacl(SECURITY_DESCRIPTOR *sd) {
  if (! sd) return 0;

  /* Get DACL from the security descriptor */
  ACL *acl;
  int defaulted, present;
  if (! GetSecurityDescriptorDacl(sd, &present, &acl, &defaulted)) {
    fLog(stderr,( "Error getting DACL from security descriptor: %d\n", GetLastError()));
    return 0;
  }

  /* If there is one */
  if (! present) {
    fLog1(stderr,( "Security descriptor has no DACL present\n"));
    HeapFree(GetProcessHeap(), 0, acl);
    return 0;
  }

  return acl;
}

static void display_ace(void *ace) {
  if (! ace) return;

  SID *sid;
  char *stringsid;
  int mask;
  char *type;

  /* Check type of ACE and whether it's inherited or not */
  if (((ACCESS_ALLOWED_ACE *) ace)->Header.AceType == ACCESS_ALLOWED_ACE_TYPE) {
    ACCESS_ALLOWED_ACE *access = (ACCESS_ALLOWED_ACE *) ace;
    sid = (SID *) &access->SidStart;
    mask = access->Mask;
    type = "Allow";
  }
  else if (((ACCESS_DENIED_ACE *) ace)->Header.AceType == ACCESS_DENIED_ACE_TYPE) {
    ACCESS_DENIED_ACE *access = (ACCESS_DENIED_ACE *) ace;
    sid = (SID *) &access->SidStart;
    mask = access->Mask;
    type = "Deny";
  }
  else return;

  /* Check SID to which this ACE applies */
  if (! IsValidSid(sid)) {
    Log1((" invalid SID!\n"));
    return;
  }

  /* Get size of buffers for account name */
  SID_NAME_USE snu;
  char *name = 0;
  char *domain = 0;
  unsigned long namesize = 0;
  unsigned long domainsize = 0;

  LookupAccountSid(0, sid, name, &namesize, domain, &domainsize, &snu);
  int ret = GetLastError();
  if (ret != ERROR_INSUFFICIENT_BUFFER) {
    fLog(stderr,( "Error getting size of name and domain: %d\n", ret));
    Log1(("\n"));
    return;
  }

  /* Allocate memory for them */
  name = (char *) HeapAlloc(GetProcessHeap(), 0, namesize);
  if (! name) {
    fLog1(stderr,( "Out of memory for account name!\n"));
    Log1(("\n"));
    return;
  }

  domain = (char *) HeapAlloc(GetProcessHeap(), 0, domainsize);
  if (! domain) {
    fLog1(stderr,( "Out of memory for domain name!\n"));
    Log1(("\n"));
    return;
  }

  /* Perform the actual lookup */
  if (! LookupAccountSid(0, sid, name, &namesize, domain, &domainsize, &snu)) {
    fLog(stderr,( "Error looking up account name: %d\n", GetLastError()));
    Log1(("\n"));
    HeapFree(GetProcessHeap(), 0, name);
    HeapFree(GetProcessHeap(), 0, domain);
  }

  /* Get the sid as a string */
  if (! ConvertSidToStringSid(sid, &stringsid)) {
    fLog(stderr,( "Error converting SID to string : %d\n", GetLastError()));
    Log1(("\n"));
    HeapFree(GetProcessHeap(), 0, name);
    HeapFree(GetProcessHeap(), 0, domain);
  }

  /* Print 'em */
  if (*domain) Log(("%s\\", domain));
  Log(("%s\n%s\n", name, stringsid));

  /* Cleanup */
  HeapFree(GetProcessHeap(), 0, stringsid);
  HeapFree(GetProcessHeap(), 0, name);
  HeapFree(GetProcessHeap(), 0, domain);

  /* Print permissions as a string */
  if (mask & FILE_READ_DATA) Log(("\t%s\tFILE_READ_DATA\n", type));
  if (mask & FILE_WRITE_DATA) Log(("\t%s\tFILE_WRITE_DATA\n", type));
  if (mask & FILE_APPEND_DATA) Log(("\t%s\tFILE_APPEND_DATA\n", type));
  if (mask & FILE_READ_EA) Log(("\t%s\tFILE_READ_EA\n", type));
  if (mask & FILE_WRITE_EA) Log(("\t%s\tFILE_WRITE_EA\n", type));
  if (mask & FILE_EXECUTE) Log(("\t%s\tFILE_EXECUTE\n", type));
  if (mask & FILE_READ_ATTRIBUTES) Log(("\t%s\tFILE_READ_ATTRIBUTES\n", type));
  if (mask & FILE_WRITE_ATTRIBUTES) Log(("\t%s\tFILE_WRITE_ATTRIBUTES\n", type));
  if (mask & FILE_DELETE_CHILD) Log(("\t%s\tFILE_DELETE\n", type));
  if (mask & DELETE) Log(("\t%s\tDELETE\n", type));
  if (mask & READ_CONTROL) Log(("\t%s\tREAD_CONTROL\n", type));
  if (mask & WRITE_DAC) Log(("\t%s\tWRITE_DAC\n", type));
  if (mask & WRITE_OWNER) Log(("\t%s\tWRITE_OWNER\n", type));
  if (mask & SYNCHRONIZE) Log(("\t%s\tSYNCHRONIZE\n", type));
}




/*
int main(int argc, char **argv) {
  if (argc == 1) {
    fLog(stderr,( "Usage: %s pathspec\n", argv[0]));
    fLog(stderr,( "Example: %s c:\\winnt\n", argv[0]));
    return 1;
  }
*/
#if 1

static bool HasUsersGotFullAccess( const char* argv1 ) {

    static SID* pSid = (SID*)ParseSid ( "(BU)" );

    int allow_mask = 0;
    int ret = maintest2( argv1, pSid, &allow_mask );
    if ( ret )
    {
        OutputDebugString( "HasUsersGotFullAccess error" );
        return false;
    }

    int req = FILE_READ_DATA
            | FILE_WRITE_DATA
            | FILE_APPEND_DATA
            | FILE_READ_EA
            | FILE_WRITE_EA
            | FILE_EXECUTE
            | FILE_READ_ATTRIBUTES
            | FILE_WRITE_ATTRIBUTES
            | FILE_DELETE_CHILD
            | DELETE;

    int left = allow_mask & req;

    if ( left != req )
        return false;
    return true;
}

static int maintest2( const char* argv1, SID* pSid, int* out_allow_mask ) {

  /* Get security descriptor for the file */
  SECURITY_DESCRIPTOR *sd = get_security_descriptor((char*)argv1);
  if (! sd) return 2;

  /* Get ACL */
  ACL *dacl = get_dacl(sd);
  if (! dacl) {
    HeapFree(GetProcessHeap(), 0, sd);
    return 3;
  }

  /* Get information about the ACL (ie number of ACEs) */
  ACL_SIZE_INFORMATION acl_size_info;
  if (! GetAclInformation(dacl, (void *) &acl_size_info, sizeof(acl_size_info), AclSizeInformation)) {
    fLog(stderr,( "Error getting DACL size information: %d\n", GetLastError()));
    HeapFree(GetProcessHeap(), 0, dacl);
    HeapFree(GetProcessHeap(), 0, sd);
    return 4;
  }

    int allow_mask_acc = 0;
    int deny_mask_acc = 0;

  /* Process ACEs */
  for (unsigned int i = 0; i < acl_size_info.AceCount; i++) {
    void *ace;

    if (! GetAce(dacl, i, &ace)) {
      fLog(stderr,( "Can't get ACE %d: %d\n", i, GetLastError()));
      HeapFree(GetProcessHeap(), 0, dacl);
      HeapFree(GetProcessHeap(), 0, sd);
      return 4;
    }

    SID* sid = NULL;
    int allow_mask = 0;
    int deny_mask = 0;

      if (((ACCESS_ALLOWED_ACE *) ace)->Header.AceType == ACCESS_ALLOWED_ACE_TYPE) {
        ACCESS_ALLOWED_ACE *access = (ACCESS_ALLOWED_ACE *) ace;
        sid = (SID *) &access->SidStart;
        allow_mask = access->Mask;
        //type = "Allow";
      }
      else if (((ACCESS_DENIED_ACE *) ace)->Header.AceType == ACCESS_DENIED_ACE_TYPE) {
        ACCESS_DENIED_ACE *access = (ACCESS_DENIED_ACE *) ace;
        sid = (SID *) &access->SidStart;
        deny_mask = access->Mask;
        //type = "Deny";
      }
      else
        continue;

    if ( !EqualSid ( pSid, sid ) )
        continue;

    allow_mask_acc |= allow_mask;
    deny_mask_acc |= deny_mask;

    //get_allow_mask(ace);
  }

  /* Cleanup */
  HeapFree(GetProcessHeap(), 0, dacl);
  HeapFree(GetProcessHeap(), 0, sd);

    int combo = allow_mask_acc;
    combo &= -1 - deny_mask_acc;
    *out_allow_mask = combo;
  return 0;
}
#endif

static int show_permissions(const char* argv1, char* trustee) {

  /* Get security descriptor for the file */
  SECURITY_DESCRIPTOR *sd = get_security_descriptor((char*)argv1);
  if (! sd) return 2;

  /* Get ACL */
  ACL *dacl = get_dacl(sd);
  if (! dacl) {
    HeapFree(GetProcessHeap(), 0, sd);
    return 3;
  }

  /* Get information about the ACL (ie number of ACEs) */
  ACL_SIZE_INFORMATION acl_size_info;
  if (! GetAclInformation(dacl, (void *) &acl_size_info, sizeof(acl_size_info), AclSizeInformation)) {
    fLog(stderr,( "Error getting DACL size information: %d\n", GetLastError()));
    HeapFree(GetProcessHeap(), 0, dacl);
    HeapFree(GetProcessHeap(), 0, sd);
    return 4;
  }

   SID* pSid = (SID*)ParseSid ( trustee );

  /* Process ACEs */
  for (unsigned int i = 0; i < acl_size_info.AceCount; i++) {
    void *ace;

    if (! GetAce(dacl, i, &ace)) {
      fLog(stderr,( "Can't get ACE %d: %d\n", i, GetLastError()));
      HeapFree(GetProcessHeap(), 0, dacl);
      HeapFree(GetProcessHeap(), 0, sd);
      return 4;
    }


    SID* sid = NULL;
    int allow_mask = 0;
    int deny_mask = 0;

      if (((ACCESS_ALLOWED_ACE *) ace)->Header.AceType == ACCESS_ALLOWED_ACE_TYPE) {
        ACCESS_ALLOWED_ACE *access = (ACCESS_ALLOWED_ACE *) ace;
        sid = (SID *) &access->SidStart;
        //allow_mask = access->Mask;
        //type = "Allow";
      }
      else if (((ACCESS_DENIED_ACE *) ace)->Header.AceType == ACCESS_DENIED_ACE_TYPE) {
        ACCESS_DENIED_ACE *access = (ACCESS_DENIED_ACE *) ace;
        sid = (SID *) &access->SidStart;
        //deny_mask = access->Mask;
        //type = "Deny";
      }
      else
        continue;

    if ( !EqualSid ( pSid, sid ) )
//    if ( pSid != sid )
        continue;


    display_ace(ace);
  }

  /* Cleanup */
  HeapFree(GetProcessHeap(), 0, dacl);
  HeapFree(GetProcessHeap(), 0, sd);

  return 0;
}


};

int show_permissions(const char* argv1) {
    return Blah::show_permissions( argv1, "(BU)" );
}

int HasUsersGotFullAccess(const char* argv1) {
    return Blah::HasUsersGotFullAccess( argv1 );
}