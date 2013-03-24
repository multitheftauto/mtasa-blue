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

    CLuaCFunctions::AddFunction ( "fxCreateSystem", CLuaFunctionDefs::FxCreateSystem );
    CLuaCFunctions::AddFunction ( "fxCreateEntityEffect", CLuaFxDefs::fxCreateEntityEffect );
    CLuaCFunctions::AddFunction ( "fxRemoveEntityEffect", CLuaFxDefs::fxRemoveEntityEffect );
    CLuaCFunctions::AddFunction ( "fxSetDrawDistance", CLuaFxDefs::fxSetDrawDistance );
    CLuaCFunctions::AddFunction ( "fxGetDrawDistance", CLuaFxDefs::fxGetDrawDistance );
    //    CLuaCFunctions::AddFunction ( "fxSetParticleInfo", CLuaFunctionDefs::FxSetParticleInfo );
}

/**
 *  effectname is from effects.fxp(or ones we create when that will be possible)
 */
int CLuaFxDefs::fxCreateEntityEffect(lua_State* luaVM)
{
//  bool fxCreateEntityEffect ( element entity, string effectname, float x, float y, float z )
    CClientEntity* pEntity = NULL; SString strEffectName;
    CVector pos;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadString ( strEffectName );
    argStream.ReadNumber ( pos.fX );
    argStream.ReadNumber ( pos.fY );
    argStream.ReadNumber ( pos.fZ );

    if ( !argStream.HasErrors ( ) )
    {
#if 0
        DWORD dwClass = 0xA9AE00;    
        DWORD dwFunc = 0x4A11E0;
        CVector* pVecPos = &pos;
        const char* szEffectName = *strEffectName;
        CEntitySAInterface* pEntityInterface = pEntity->GetGameEntity()->GetInterface();
        RwMatrix* pMatrix = NULL;
        if(pEntityInterface->m_pRwObject)
        {
            RwFrame
                *pParent = reinterpret_cast<RwFrame*>(pEntityInterface->m_pRwObject->object.parent);
            if(pParent)
            {
                pMatrix = &pParent->modelling;
            }
        }
        __asm
        {
            push pMatrix
            push pVecPos
            push szEffectName
            push pEntityInterface
            mov ecx, dwClass
            call dwFunc
        }
#endif
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxCreateEntityFx", *argStream.GetErrorMessage () ) );
    
    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFxDefs::fxRemoveEntityEffect(lua_State* luaVM)
{
//  bool fxRemoveEntityEffect(element entity)
    CClientEntity* pEntity = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    if ( !argStream.HasErrors ( ) )
    {
        CEntitySAInterface* pInterface = pEntity->GetGameEntity()->GetInterface();
        DWORD dwClass = 0xA9AE00;
        DWORD dwFunc = 0x4A1280;
        __asm
        {
            push pInterface
            mov ecx, dwClass
            call dwFunc
        }
        lua_pushboolean ( luaVM, true );  
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxRemoveEntityEffect", *argStream.GetErrorMessage () ) );
    
    lua_pushboolean ( luaVM, false );  
    return 1;
}

int CLuaFxDefs::fxSetDrawDistance(lua_State* luaVM)
{
//  bool fxSetDrawDistance(float distance)
    float fDrawDistance;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( fDrawDistance );

    if ( !argStream.HasErrors ( ) )
    {
        // TODO
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxSetDrawDistance", *argStream.GetErrorMessage () ) );
    
    lua_pushboolean ( luaVM, false );  
    return 1;
}

int CLuaFxDefs::fxGetDrawDistance(lua_State* luaVM)
{
//  float fxGetDrawDistance(float distance)
    float fDrawDistance;
    // TODO IMPLEMENT
    lua_pushnumber ( luaVM, 0 );  
    return 1;
}

int CLuaFunctionDefs::FxSetParticleInfo(lua_State* luaVM)
{
//  bool fxSetParticleInfo ( float colorR, float colorG, float colorB, [ float size = 1.0, float durationFactor = 1.0, float unk = 1.0 ] )
    RwColorFloat colour; float fSize;
    float fDurationFactor; float fUnk;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( colour.r );
    argStream.ReadNumber ( colour.g );
    argStream.ReadNumber ( colour.b );
    argStream.ReadNumber ( fSize, 1.0f );
    argStream.ReadNumber ( fDurationFactor, 1.0f );
    argStream.ReadNumber ( fUnk, 1.0f );
    
    if ( !argStream.HasErrors ( ) )
    {
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxSetParticleInfo", *argStream.GetErrorMessage () ) );
    return 1;
}

// Particle System Temporary Residence
//      System Layout:
//          fxAddParticle
//          fxSetParticleInfo
//          fxDisableCommonParticle - look @ CFx - 1st member, though they will be treated as any other
//          fxRemoveParticle
//      particle info for default particles are set within functions they are used in
//          so will need to do context-based nulling out(i.e. if someone wants to have red jetpack thrusters, disable setting it in CPed::RenderJetPack and add function to reenable it)
int CLuaFunctionDefs::FxCreateSystem(lua_State* luaVM)
{
//  element fxCreateSystem ( string name, float pointX, float pointY, float pointZ, bool unknown  )
#if 0
    SString strParticleName; CVector vecPoint; 
    bool bUnk;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strParticleName );
    argStream.ReadNumber ( vecPoint.fX );
    argStream.ReadNumber ( vecPoint.fY );
    argStream.ReadNumber ( vecPoint.fZ );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxCreateSystem ( strParticleName, vecPoint, bUnk ) )
        {
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", "fxAddParticle", *argStream.GetErrorMessage () ) );
#endif
    return 0;
}
        /*
        // move this to its own CClientParticle.. do this way cuz it's faster to debug
        CVector *pVecPos = &vecPos;
        CVector *pVecDir = &vecDir;
	    {
            CParticleInfoSAInterface
                particleInfo;
            particleInfo.colour.r = 1.0f;
            particleInfo.colour.g = 1.0f;
            particleInfo.colour.b = 1.0f;
            particleInfo.colour.a = 1.0f;
            particleInfo.fSize = 30.0f;
            particleInfo.fDurationFactor = 1.0f;
            particleInfo.pad1 = 100.0f;
            CParticleInfoSAInterface
                *pParticleInfo = &particleInfo;
		    uint32 dwRet = 0;
		    {   // __stdcall CFxManager::CreateFxSystem
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
                
			    //{   // CParticleFx::StartParticle
				 //   uint32 CParticleFx__StartParticle = (uint32)0x4AA3D0;
				////    __asm
				// //   {
				//	    mov ecx, dwRet
				//	    call CParticleFx__StartParticle
				//    }
			    //}
               
                {   // CFxSystem::Initialise
                    uint32 CParticleFx__Initialise = (uint32)0x4AA2F0;
                    __asm
                    {
                       // mov ecx, dwRet
                       // call CParticleFx__Initialise
                    }
                }
                {   // CFxSystem::SetRateMult
                    uint32 CFxSystem__SetRateMult = (uint32)0x4AA6F0;
                    float fMult = 10.0f;
                    __asm
                    {
                        push fMult
                        mov ecx, dwRet
                        call CFxSystem__SetRateMult
                    }
                }
                {   // CFxSystem::AddParticle
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

}
*/
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
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

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
    argStream.ReadBool ( bBlur, false );
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
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

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
    argStream.ReadBool ( bIncludeSparks, true );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FxAddGunshot ( vecPosition, vecDirection, bIncludeSparks ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

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
        m_pScriptDebugging->LogBadType ( luaVM );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}