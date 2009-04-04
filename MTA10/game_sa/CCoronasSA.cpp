/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CCoronasSA.cpp
*  PURPOSE:		Corona entity manager
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CCoronasSA::CCoronasSA()
{
	DEBUG_TRACE("CCoronasSA::CCoronasSA()");
	for(int i =0;i < MAX_CORONAS;i++)
	{
		Coronas[i] = new CRegisteredCoronaSA((CRegisteredCoronaSAInterface *)(ARRAY_CORONAS + i * sizeof(CRegisteredCoronaSAInterface)));
	}
}

CCoronasSA::~CCoronasSA ()
{
    for ( int i = 0; i < MAX_CORONAS; i++ )
    {
        delete Coronas [i];
    }
}


CRegisteredCorona * CCoronasSA::GetCorona(DWORD ID)
{
	DEBUG_TRACE("CRegisteredCorona * CCoronasSA::GetCorona(DWORD ID)");
	return (CRegisteredCorona *)this->Coronas[ID];
}

CRegisteredCorona * CCoronasSA::CreateCorona(DWORD Identifier, CVector * position)
{
	DEBUG_TRACE("CRegisteredCorona * CCoronasSA::CreateCorona(DWORD Identifier, CVector * position)");
	CRegisteredCoronaSA * corona;
	corona = (CRegisteredCoronaSA *)this->FindCorona(Identifier);

	if(!corona)
		corona = (CRegisteredCoronaSA *)this->FindFreeCorona();

    if ( corona )
    {

        RwTexture * texture = this->GetTexture((eCoronaType)CORONATYPE_SHINYSTAR);
	    if(texture)
	    {
		    corona->Init(Identifier);
		    corona->SetPosition(position);
		    corona->SetTexture (texture);
		    return (CRegisteredCorona *)corona;
	    }
    }

	return (CRegisteredCorona *)NULL;
}

CRegisteredCorona * CCoronasSA::FindFreeCorona()
{
	DEBUG_TRACE("CRegisteredCorona * CCoronasSA::FindFreeCorona()");
	for(int i=2;i< MAX_CORONAS;i++)
	{
		if(Coronas[i]->GetIdentifier() == 0)
		{
			return Coronas[i];
		}
	}
	return (CRegisteredCorona *)NULL;
}

CRegisteredCorona * CCoronasSA::FindCorona(DWORD Identifier)
{
	DEBUG_TRACE("CRegisteredCorona * CCoronasSA::FindCorona(DWORD Identifier)");
	
	for(int i=0;i< MAX_CORONAS;i++)
	{
		if(Coronas[i]->GetIdentifier() == Identifier)
		{
			return Coronas[i];
		}
	}
	return (CRegisteredCorona *)NULL;
}

RwTexture * CCoronasSA::GetTexture(eCoronaType type)
{
	DEBUG_TRACE("RwTexture * CCoronasSA::GetTexture(eCoronaType type)");
	if(type < MAX_CORONA_TEXTURES)
		return (RwTexture *)(*(DWORD *)(ARRAY_CORONA_TEXTURES + type * sizeof(DWORD)));
	else
		return NULL;
}

void CCoronasSA::DisableSunAndMoon ( bool bDisabled )
{
    static BYTE byteOriginal = 0;
    if ( bDisabled && !byteOriginal )
    {
        byteOriginal = *(BYTE *)FUNC_DoSunAndMoon;
	    *(BYTE *)FUNC_DoSunAndMoon = 0xC3;
    }
    else if ( !bDisabled && byteOriginal )
    {
	    *(BYTE *)FUNC_DoSunAndMoon = byteOriginal;
        byteOriginal = 0;
    }
}