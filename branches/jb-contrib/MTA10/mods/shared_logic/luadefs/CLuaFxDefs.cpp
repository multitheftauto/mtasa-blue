/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaFxDefs.cpp
*  PURPOSE:     Lua fx definitions class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#include <StdInc.h>

void CLuaFxDefs::LoadFunctions ( void )
{
    CLuaCFunctions::AddFunction ( "fxAddBlood", CLuaFxDefs::fxAddBlood );
    CLuaCFunctions::AddFunction ( "fxAddWood", CLuaFxDefs::fxAddWood );
    CLuaCFunctions::AddFunction ( "fxAddSparks", CLuaFxDefs::fxAddSparks );
    CLuaCFunctions::AddFunction ( "fxAddTyreBurst", CLuaFxDefs::fxAddTyreBurst );
    CLuaCFunctions::AddFunction ( "fxAddBulletImpact", CLuaFxDefs::fxAddBulletImpact );
    CLuaCFunctions::AddFunction ( "fxAddPunchImpact", CLuaFxDefs::fxAddPunchImpact );
    CLuaCFunctions::AddFunction ( "fxAddDebris", CLuaFxDefs::fxAddDebris );
    CLuaCFunctions::AddFunction ( "fxAddGlass", CLuaFxDefs::fxAddGlass );
    CLuaCFunctions::AddFunction ( "fxAddWaterHydrant", CLuaFxDefs::fxAddWaterHydrant );
    CLuaCFunctions::AddFunction ( "fxAddGunshot", CLuaFxDefs::fxAddGunshot );
    CLuaCFunctions::AddFunction ( "fxAddTankFire", CLuaFxDefs::fxAddTankFire );
    CLuaCFunctions::AddFunction ( "fxAddWaterSplash", CLuaFxDefs::fxAddWaterSplash );
    CLuaCFunctions::AddFunction ( "fxAddBulletSplash", CLuaFxDefs::fxAddBulletSplash );
    CLuaCFunctions::AddFunction ( "fxAddFootSplash", CLuaFxDefs::fxAddFootSplash );
    CLuaCFunctions::AddFunction ( "fxAddParticle", CLuaFunctionDefs::FxAddParticle );
}

#include "../../../game_sa/CParticleInfoSA.h"
// Particle System Temporary Residence
//      System Layout:
//          fxAddParticle
//          fxSetParticleInfo
//          fxDisableDefaultParticle - look @ CParticleSystemMgr - 1st member, though they will be treated as any other
//          fxRemoveParticle
//      particle info for default particles are set within functions they are used in
//          so will need to do context-based nulling out(i.e. if someone wants to have red jetpack thrusters, disable setting it in CPed::RenderJetPack and add function to reenable it)
int CLuaFunctionDefs::FxAddParticle(lua_State* luaVM)
{
//  element fxAddParticle ( string name, float posX, float posY, float posZ, float velX, float velY, float velZ, [ float blur = 0.0, float brightness = 1.0 ] )
    SString strParticleName; CVector vecPos; CVector vecDir;
    float fBlur; float fBrightness;
    // it returns bool for DEBUGGING, it will be <element> later..
    // also add name later, will be testing with some random for now
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strParticleName );
    argStream.ReadNumber ( vecPos.fX );
    argStream.ReadNumber ( vecPos.fY );
    argStream.ReadNumber ( vecPos.fZ );
    argStream.ReadNumber ( vecDir.fX );
    argStream.ReadNumber ( vecDir.fY );
    argStream.ReadNumber ( vecDir.fZ );
    argStream.ReadNumber ( fBlur, 0.0f );
    argStream.ReadNumber ( fBrightness, 1.0f );

    if ( !argStream.HasErrors ( ) )
    {
        // move this to its own CClientParticle.. do this way cuz it's faster to debug
	    CCamera
		    *pCam = g_pGame->GetCamera();
	    CMatrix matrix;
	  //  if(0)
        CVector *pVecPos = &vecPos;
        CVector *pVecDir = &vecDir;
//        if(pCam->GetMatrix(&matrix))
	    {
            CParticleInfoSAInterface
                particleInfo;
            particleInfo.colour.r = 1.0f;
            particleInfo.colour.g = 1.0f;
            particleInfo.colour.b = 1.0f;
            particleInfo.colour.a = 1.0f;
            particleInfo.fSize = 30.0f;
            particleInfo.fDurationFactor = 10000.0f;
            particleInfo.pad1 = 100.0f; // nudge lil_Toady to see what's this shit
            CParticleInfoSAInterface
                *pParticleInfo = &particleInfo;
		    uint32 dwRet = 0;
		    {   // __stdcall CParticleData::createParticle
			    uint32 CParticleData__createParticle = (uint32)0x4A9BE0;
                uint32 *szParticleName = (uint32*)strParticleName.data();
                uint32 pParticleData = (uint32)0xA9AE80;
                CVector vecNull = CVector(0.0f, 0.0f, 0.0f);
                CVector *pVecNull = &vecNull;
			    __asm
			    {
				    push 0
				    push 0
				    push pVecNull
                   // lea eax, szParticleName
				  //  push eax
                    push szParticleName
                    mov ecx, pParticleData
				    call CParticleData__createParticle
				    mov dwRet, eax
			    }
		    }
		    if(dwRet)
		    {
                /*
			    {   // CParticleFx::StartParticle
				    uint32 CParticleFx__StartParticle = (uint32)0x4AA3D0;
				    __asm
				    {
					    mov ecx, dwRet
					    call CParticleFx__StartParticle
				    }
			    }
                */
                {   // CParticleFx::Initialise
                    uint32 CParticleFx__Initialise = (uint32)0x4AA2F0;
                    __asm
                    {
                       // mov ecx, dwRet
                       // call CParticleFx__Initialise
                    }
                }
                {   // CParticleFx::AddParticle
                    uint32 CParticleFx__AddParticle = (uint32)0x4AA440;
                    uint32 a9 = 0;
                    float fBrightness = 1.0f;
                    float fa7 = 1.2f; // 1.2f
                    float fa6 = 0.0f; // -1.0f
                    __asm
                    {
                        push a9
                        push fBrightness
                        push fa7
                        push fa6
                        push pParticleInfo
                        push fBlur
                        push pVecDir
                        push pVecPos
                        mov ecx, dwRet
                        call CParticleFx__AddParticle
                    }
                }
		    }
	    }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddParticle", *argStream.GetErrorMessage () ) );
    return 1;
}

int CLuaFunctionDefs::FxSetParticleInfo(lua_State* luaVM)
{

    return 1;
}

int CLuaFxDefs::fxAddBlood ( lua_State* luaVM )
{
    // bool fxAddBlood ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [int count=1, float fBrightness=1.0] )
    CVector vecPosition; CVector vecDirection; 
    int32 iCount; float fBrightness;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( vecDirection.fX );
    argStream.ReadNumber ( vecDirection.fY );
    argStream.ReadNumber ( vecDirection.fZ );
    argStream.ReadNumber ( iCount, 1 );
    argStream.ReadNumber ( fBrightness, 1.0f );
    
    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddBlood ( vecPosition, vecDirection, iCount, fBrightness ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddBlood", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddWood ( lua_State* luaVM )
{
    // bool fxAddWood ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [int count=1, float fBrightness=1.0] )
    CVector vecPosition; CVector vecDirection; 
    int32 iCount; float fBrightness;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( vecDirection.fX );
    argStream.ReadNumber ( vecDirection.fY );
    argStream.ReadNumber ( vecDirection.fZ );
    argStream.ReadNumber ( iCount, 1 );
    argStream.ReadNumber ( fBrightness, 1.0f );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddWood ( vecPosition, vecDirection, iCount, fBrightness ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddWood", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddSparks ( lua_State* luaVM )
{
    // bool fxAddSparks ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [float force=1, int count=1, float acrossLineX=0, float acrossLineY=0, float acrossLineZ=0, bool blur=false, float spread=1, float life=1] )
    CVector vecPosition; CVector vecDirection;
    float fForce; int32 iCount; CVector vecAcrossLine;
    bool bBlur; float fSpread; float fLife;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( vecDirection.fX );
    argStream.ReadNumber ( vecDirection.fY );
    argStream.ReadNumber ( vecDirection.fZ );
    argStream.ReadNumber ( fForce, 1.0f );
    argStream.ReadNumber ( iCount, 1 );
    argStream.ReadNumber ( vecAcrossLine.fX );
    argStream.ReadNumber ( vecAcrossLine.fY );
    argStream.ReadNumber ( vecAcrossLine.fZ );
    argStream.ReadNumber ( bBlur, false );
    argStream.ReadNumber ( fSpread, 1.0f );
    argStream.ReadNumber ( fLife, 1.0f );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddSparks ( vecPosition, vecDirection, fForce, iCount, vecAcrossLine, bBlur, fSpread, fLife ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddSparks", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddTyreBurst ( lua_State* luaVM )
{
    // bool fxAddTyreBurst ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ )
    CVector vecPosition; CVector vecDirection;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( vecDirection.fX );
    argStream.ReadNumber ( vecDirection.fY );
    argStream.ReadNumber ( vecDirection.fZ );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddTyreBurst ( vecPosition, vecDirection ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddTyreBurst", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddBulletImpact ( lua_State* luaVM )
{
    // bool fxAddBulletImpact ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [int smokeSize=1, int sparkCount=1, float fSmokeIntensity=1.0] )
    CVector vecPosition; CVector vecDirection;
    int32 iSmokeSize = 1; int32 iSparkCount = 1; float fSmokeIntensity = 1.0f;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( vecDirection.fX );
    argStream.ReadNumber ( vecDirection.fY );
    argStream.ReadNumber ( vecDirection.fZ );
    argStream.ReadNumber ( iSmokeSize, 1 );
    argStream.ReadNumber ( iSparkCount, 1 );
    argStream.ReadNumber ( fSmokeIntensity, 1.0f );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddBulletImpact ( vecPosition, vecDirection, iSmokeSize, iSparkCount, fSmokeIntensity ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddBulletImpact", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddPunchImpact ( lua_State* luaVM )
{
    // bool fxAddPunchImpact ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ )
    CVector vecPosition; CVector vecDirection;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( vecDirection.fX );
    argStream.ReadNumber ( vecDirection.fY );
    argStream.ReadNumber ( vecDirection.fZ );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddPunchImpact ( vecPosition, vecDirection ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddPunchImpact", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddDebris ( lua_State* luaVM )
{
    // bool fxAddDebris ( float posX, float posY, float posZ, [int colorR=255, int colorG=0, int colorB=0, int colorA=255, float scale=1.0, int count=1] )
    CVector vecPosition; RwColor colour;
    float fScale; int32 iCount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( colour.r, 255 );
    argStream.ReadNumber ( colour.g, 0 );
    argStream.ReadNumber ( colour.b, 0 );
    argStream.ReadNumber ( colour.a, 255 );
    argStream.ReadNumber ( fScale, 1.0f );
    argStream.ReadNumber ( iCount, 1 );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddDebris ( vecPosition, colour, fScale, iCount ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddDerbis", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddGlass ( lua_State* luaVM )
{
    // bool fxAddGlass ( float posX, float posY, float posZ, [int colorR=255, int colorG=0, int colorB=0, int colorA=255, float scale=1.0, int count=1] )
    CVector vecPosition; RwColor colour;
    float fScale; int32 iCount;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( colour.r, 255 );
    argStream.ReadNumber ( colour.g, 0 );
    argStream.ReadNumber ( colour.g, 0 );
    argStream.ReadNumber ( colour.a, 255 );
    argStream.ReadNumber ( fScale, 1.0f );
    argStream.ReadNumber ( iCount, 1 );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddGlass ( vecPosition, colour, fScale, iCount ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddGlass", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddWaterHydrant ( lua_State* luaVM )
{
    // bool fxAddWaterHydrant ( float posX, float posY, float posZ )
    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddWaterHydrant ( vecPosition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddWaterHydrant", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddGunshot ( lua_State* luaVM )
{
    // bool fxAddGunshot ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ, [bool includeSparks=true] )
    CVector vecPosition; CVector vecDirection;
    bool bIncludeSparks;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( vecDirection.fX );
    argStream.ReadNumber ( vecDirection.fY );
    argStream.ReadNumber ( vecDirection.fZ );
    argStream.ReadNumber ( bIncludeSparks, true );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddGunshot ( vecPosition, vecDirection, bIncludeSparks ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddGunshot", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddTankFire ( lua_State* luaVM )
{
    // bool fxAddTankFire ( float posX, float posY, float posZ, float dirX, float dirY, float dirZ )
    CVector vecPosition; CVector vecDirection;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( vecDirection.fX );
    argStream.ReadNumber ( vecDirection.fY );
    argStream.ReadNumber ( vecDirection.fZ );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddTankFire ( vecPosition, vecDirection ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddTankFire", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddWaterSplash ( lua_State* luaVM )
{
    // bool fxAddWaterSplash ( float posX, float posY, float posZ )
    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddWaterSplash ( vecPosition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddWaterSplash", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddBulletSplash ( lua_State* luaVM )
{
    // bool fxAddBulletSplash ( float posX, float posY, float posZ )
    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddBulletSplash ( vecPosition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddBulletSplash", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFxDefs::fxAddFootSplash ( lua_State* luaVM )
{
    // bool fxAddFootSplash ( float posX, float posY, float posZ )
    CVector vecPosition;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddFootSplash ( vecPosition ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddFootSplash", *argStream.GetErrorMessage () ) );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}