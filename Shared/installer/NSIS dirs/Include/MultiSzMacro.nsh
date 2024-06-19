!define HKEY_CLASSES_ROOT        0x80000000
!define HKEY_CURRENT_USER        0x80000001
!define HKEY_LOCAL_MACHINE       0x80000002
!define HKEY_USERS               0x80000003
!define HKEY_PERFORMANCE_DATA    0x80000004
!define HKEY_PERFORMANCE_TEXT    0x80000050
!define HKEY_PERFORMANCE_NLSTEXT 0x80000060
!define HKEY_CURRENT_CONFIG      0x80000005
!define HKEY_DYN_DATA            0x80000006

!define KEY_QUERY_VALUE          0x0001
!define KEY_ENUMERATE_SUB_KEYS   0x0008

!define REG_NONE                 0
!define REG_SZ                   1
!define REG_EXPAND_SZ            2
!define REG_BINARY               3
!define REG_DWORD                4
!define REG_DWORD_LITTLE_ENDIAN  4
!define REG_DWORD_BIG_ENDIAN     5
!define REG_LINK                 6
!define REG_MULTI_SZ             7

!define RegOpenKeyEx     "Advapi32::RegOpenKeyEx(i, t, i, i, *i) i"
!define RegQueryValueEx  "Advapi32::RegQueryValueEx(i, t, i, *i, i, *i) i"
!define RegCloseKey      "Advapi32::RegCloseKey(i) i"

!macro _ReadRegStrMultiSz ROOT_KEY SUB_KEY VALUE TARGET_PATH RESULT
    !define UniqueID ${__LINE__}

    StrCpy $0 ""
    StrCpy $1 ""
    StrCpy $2 ""
    StrCpy $3 ""

    SetPluginUnload alwaysoff

    System::Call "${RegOpenKeyEx}(${ROOT_KEY}, '${SUB_KEY}', 0, ${KEY_QUERY_VALUE}|${KEY_ENUMERATE_SUB_KEYS}, .r0) .r3"

    StrCmp $3 0 multiszgoon_${UniqueID}
        ;MessageBox MB_OK|MB_ICONSTOP "Can't open registry key! ($3)"
        Goto multiszdone_${UniqueID}

multiszgoon_${UniqueID}:
    System::Call "${RegQueryValueEx}(r0, '${VALUE}', 0, .r1, 0, .r2) .r3"
    
    StrCmp $3 0 multiszread_${UniqueID}
        ;MessageBox MB_OK|MB_ICONSTOP "Can't query registry value size! ($3)"
        Goto multiszdone_${UniqueID}

multiszread_${UniqueID}:
    StrCmp $1 ${REG_MULTI_SZ} multisz_${UniqueID}
        ;MessageBox MB_OK|MB_ICONSTOP "Registry value not REG_MULTI_SZ! ($3)"
        Goto multiszdone_${UniqueID}

multisz_${UniqueID}:
    StrCmp $2 0 0 multiszalloc_${UniqueID}
        ;MessageBox MB_OK|MB_ICONSTOP "Registry value empty! ($3)"
        Goto multiszdone_${UniqueID}

multiszalloc_${UniqueID}:
    System::Alloc $2
    Pop $1

    StrCmp $1 0 0 multiszget_${UniqueID}
        ;MessageBox MB_OK|MB_ICONSTOP "Can't allocate enough memory! ($3)"
        Goto multiszdone_${UniqueID}

multiszget_${UniqueID}:
    System::Call "${RegQueryValueEx}(r0, '${VALUE}', 0, n, r1, r2) .r3"

    StrCmp $3 0 multiszprocess_${UniqueID}
        ;MessageBox MB_OK|MB_ICONSTOP "Can't query registry value data! ($3)"
        Goto multiszdone_${UniqueID}

multiszprocess_${UniqueID}:
    StrCpy $4 $1
    IntOp $6 $4 + $2
 
    ;REG_MULTI_SZ is double null terminated, change the size so the loop does not have to worry about it
!ifdef NSIS_UNICODE 
    IntOp $6 $6 - 2
!else
    IntOp $6 $6 - 1
!endif

multiszloop_${UniqueID}:
    System::Call "*$4(&t${NSIS_MAX_STRLEN} .r3)"
    StrLen $5 $3
    IntOp $5 $5 + 1 ;arnold
!ifdef NSIS_UNICODE    
    IntOp $5 $5 * 2 ;WCHAR
!endif
    IntOp $4 $4 + $5
    ${If} "$3" != "${TARGET_PATH}"
        IntCmp $4 $6 0 multiszloop_${UniqueID}
    ${EndIf}

multiszdone_${UniqueID}:
    System::Free $1

    StrCmp $0 0 multisznoClose_${UniqueID}
        System::Call "${RegCloseKey}(r0)"

multisznoClose_${UniqueID}:
    ${If} "$3" == "${TARGET_PATH}"
        StrCpy ${RESULT} $3
    ${Else}
        StrCpy ${RESULT} ""
    ${EndIf}

    SetPluginUnload manual

    !undef UniqueID
!macroend

!define ReadRegStrMultiSz `!insertmacro _ReadRegStrMultiSz`
