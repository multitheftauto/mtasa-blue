# user interface
 
!define GameExplorer_GenerateGUID '!insertmacro GameExplorer_GenerateGUID'
!define GameExplorer_AddGame      '!insertmacro GameExplorer_AddGame'
!define GameExplorer_UpdateGame   '!insertmacro GameExplorer_UpdateGame'
!define GameExplorer_RemoveGame   '!insertmacro GameExplorer_RemoveGame'
 
# internal stuff
 
!define CLSCTX_INPROC_SERVER 1
!define IID_IGameExplorer  {E7B2FB72-D728-49B3-A5F2-18EBF5F1349E}
!define CLSID_GameExplorer {9A5EA990-3034-4D6F-9128-01F3C61022BC}
 
!define GIS_CURRENT_USER 2
!define GIS_ALL_USERS    3
 
!define IGameExplorer_QueryInterface 0
!define IGameExplorer_AddRef         1
!define IGameExplorer_Release        2
!define IGameExplorer_AddGame        3
!define IGameExplorer_RemoveGame     4
!define IGameExplorer_UpdateGame     5
!define IGameExplorer_VerifyAccess   6
 
# includes
 
!include LogicLib.nsh
 
# the actual code
 
!macro GameExplorer_GenerateGUID
 
  System::Call 'ole32::CoCreateGuid(g .s)'
 
!macroend
 
!macro GameExplorer_AddGame CONTEXT GDF INSTDIR EXE GUID
 
  !define __GAME_EXPLORER_UNIQUE "${__LINE__}${__FILE__}"
 
  Push $0
  Push $1
 
  Push $R0
  Push $R1
  Push $R2
  Push $R3
 
  Push "${EXE}"
  Push "${GUID}"
  Push "${INSTDIR}"
  Push "${GDF}"
 
  Pop $R0 # == ${GDF}
  Pop $R1 # == ${INSTDIR}
  Pop $R2 # == ${GUID}
  Pop $R3 # == ${EXE}
 
  ClearErrors
 
  System::Call "ole32::CoCreateInstance( \
    g '${CLSID_GameExplorer}', i 0, \
    i ${CLSCTX_INPROC_SERVER}, \
    g '${IID_IGameExplorer}', *i .r1) i .r0"
 
  ${If} $0 != 0 # S_OK
 
    SetErrors
    Goto "done_${__GAME_EXPLORER_UNIQUE}"
 
  ${EndIf}
 
  !if ${CONTEXT} == all
 
    System::Call "$1->${IGameExplorer_AddGame}(w R0, w R1, i ${GIS_ALL_USERS}, g R2) i .r0"
 
  !else if ${CONTEXT} == user
 
    System::Call "$1->${IGameExplorer_AddGame}(w R0, w R1, i ${GIS_CURRENT_USER}, g R2) i .r0"
 
  !else
 
    !error "Invalid CONTEXT passed to GameExplorer_AddGame! Must be `user` or `all`."
 
  !endif
 
  ${If} $0 != 0 # S_OK
 
    SetErrors
 
  ${Else}
 
    # Create play task
 
    !if ${CONTEXT} == all
 
      SetShellVarContext all
 
    !else if ${CONTEXT} == user
 
      SetShellVarContext current
 
    !endif
 
    CreateDirectory $APPDATA\Microsoft\Windows\GameExplorer\$R2\PlayTasks\0
    CreateShortcut $APPDATA\Microsoft\Windows\GameExplorer\$R2\PlayTasks\0\Play.lnk $R3
 
  ${EndIf}
 
  System::Call "$1->${IGameExplorer_Release}()"
 
  "done_${__GAME_EXPLORER_UNIQUE}:"
 
    Pop $R3
    Pop $R2
    Pop $R1
    Pop $R0
 
    Pop $1
    Pop $0
 
  !undef __GAME_EXPLORER_UNIQUE
 
!macroend
 
!macro _GameExplorer_GUID_Function Function GUID
 
  !define __GAME_EXPLORER_UNIQUE "${__LINE__}${__FILE__}"
 
  Push $0
  Push $1
  Push $2
  Push $3
  Push $4
  Push $5
 
  Push $R0
 
  Push "${GUID}"
 
  Pop $R0 # == ${GUID}
 
  System::Alloc 16
  Exch $R0
  System::Call "ole32::CLSIDFromString(w s, i R0)"
  System::Call "*$R0(i .r2, i .r3, i .r4, i .r5)"
  System::Free $R0
 
  ClearErrors
 
  System::Call "ole32::CoCreateInstance( \
    g '${CLSID_GameExplorer}', i 0, \
    i ${CLSCTX_INPROC_SERVER}, \
    g '${IID_IGameExplorer}', *i .r1) i .r0"
 
  ${If} $0 != 0 # S_OK
 
    SetErrors
    Goto "done_${__GAME_EXPLORER_UNIQUE}"
 
  ${EndIf}
 
  System::Call "$1->${Function}(i r2, i r3, i r4, i r5) i .r0"
 
  ${If} $0 != 0 # S_OK
 
    SetErrors
 
  ${EndIf}
 
  System::Call "$1->${IGameExplorer_Release}()"
 
  "done_${__GAME_EXPLORER_UNIQUE}:"
 
    Pop $R0
 
    Pop $5
    Pop $4
    Pop $3
    Pop $2
    Pop $1
    Pop $0
 
  !undef __GAME_EXPLORER_UNIQUE
 
!macroend
 
!macro GameExplorer_UpdateGame GUID
 
  !insertmacro _GameExplorer_GUID_Function ${IGameExplorer_UpdateGame} "${GUID}"
 
!macroend
 
!macro GameExplorer_RemoveGame GUID
 
  !insertmacro _GameExplorer_GUID_Function ${IGameExplorer_RemoveGame} "${GUID}"
 
!macroend