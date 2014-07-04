/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/MainFunctions.h
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

void    InitLocalization                ( bool bNoFail );
void    HandleSpecialLaunchOptions      ( void );
void    HandleDuplicateLaunching        ( void );
void    HandleCustomStartMessage        ( void );
void    PreLaunchWatchDogs              ( void );
void    PostRunWatchDogs                ( int iReturnCode );
void    HandleIfGTAIsAlreadyRunning     ( void );
void    ValidateGTAPath                 ( void );
void    CheckAntiVirusStatus            ( void );
void    CheckDataFiles                  ( void );
int     LaunchGame                      ( SString strCmdLine );
void    HandleOnQuitCommand             ( void );

#define EXIT_OK     (0)
#define EXIT_ERROR  (1)
