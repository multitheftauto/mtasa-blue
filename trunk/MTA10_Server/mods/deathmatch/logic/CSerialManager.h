/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CSerialManager.h
*  PURPOSE:     Serial verification manager class
*  DEVELOPERS:  lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CSERIALMANAGER_H
#define __CSERIALMANAGER_H

#include "json/json.h"
#include <list>
#include "CPlayer.h"

typedef void (*SERIALVERIFICATIONCALLBACK) ( CPlayer*, bool, char* );

#define SERIAL_VERIFICATION_TIMEOUT 3000
#define SERIAL_VERIFICATION_SERVER  "dp3.community.mtasa.com"
#define SERIAL_VERIFICATION_URL     "http://dp3.community.mtasa.com/core/1.0/server.php"

enum eSerialVerificationResult
{
	SERIAL_ERROR_UNEXPECTED,
	SERIAL_ERROR_SUCCESS,
    SERIAL_ERROR_VERIFICATION_FAILED,
	SERIAL_ERROR_COMPUTER_USED,
	SERIAL_ERROR_SERIALS_MAX,
	SERIAL_ERROR_INVALID_ACCOUNT,
	SERIAL_ERROR_ACCOUNT_USED,
	SERIAL_ERROR_INVALID,
	SERIAL_ERROR_ACCOUNT_BANNED,
    SERIAL_ERROR_SERIAL_BANNED,
    
    SERIAL_ERROR_LAST
};

class CSerialVerification
{
public:

                                CSerialVerification         ( CPlayer* pPlayer, SERIALVERIFICATIONCALLBACK pCallBack );
                                ~CSerialVerification        ( void );

	SERIALVERIFICATIONCALLBACK  GetCallBack                 ( void )    { return m_pCallBack; };
    CPlayer*                    GetPlayer                   ( void )    { return m_pPlayer; };

    void                        DoPulse                     ( void );

    static void                 ProgressCallback            ( double nJustDownloaded, double nTotalDownloaded, char * szData, size_t nDataLength, void * pObject, bool bComplete, int iError );
    bool                        IsFinished					( void )    { return m_bFinished; };

private:
    unsigned long               m_ulStartTime;
    SERIALVERIFICATIONCALLBACK  m_pCallBack;
    CPlayer*                    m_pPlayer;
    bool                        m_bFinished;
};


class CSerialManager 
{
public:
                                CSerialManager              ( void );
                                ~CSerialManager             ( void );

    void                        Verify                      ( CPlayer* pPlayer, SERIALVERIFICATIONCALLBACK pCallBack );

	void                        Remove                      ( CPlayer* pPlayer );
    void                        Remove                      ( CSerialVerification * call );
	bool                        IsVerified                  ( CSerialVerification * call );

	void                        DoPulse                     ( void );

private:
    std::list < CSerialVerification* > m_calls;
};
#endif