/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//
// Bridge between the game and sim system
//
class CSimControl
{
public:
    static void Startup                     ( void );
    static void Shutdown                    ( void );
    static void DoPulse                     ( void );
    static void EnableSimSystem             ( bool bEnable, bool bApplyNow = true );
    static bool IsSimSystemEnabled          ( void );
    static void AddSimPlayer                ( CPlayer* pPlayer );
    static void RemoveSimPlayer             ( CPlayer* pPlayer );
    static void UpdateSimPlayer             ( CPlayer* pPlayer );
};
