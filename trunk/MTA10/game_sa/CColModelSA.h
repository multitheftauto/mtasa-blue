/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CColModelSA.h
*  PURPOSE:		Header file for collision model entity class
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_COLMODELSA
#define __CGAMESA_COLMODELSA

#include <windows.h>
#include <game/CColModel.h>

#define FUNC_CColModel_operator_new		0x40FC30
#define FUNC_CColModel_constructor		0x40FB60

#define SIZEOF_CColModel				0x30

class CColModelSAInterface
{

};

class CColModelSA : public CColModel
{
private:
	CColModelSAInterface		* internalInterface;
public:
									CColModelSA		( CColModelSAInterface * objectInterface );
									CColModelSA		( void );
									~CColModelSA	( void );

	inline CColModelSAInterface *	GetColModel		( void ) { return this->internalInterface; };
};

#endif