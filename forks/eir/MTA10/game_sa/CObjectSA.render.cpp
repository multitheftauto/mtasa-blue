/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CObjectSA.render.cpp
*  PURPOSE:     Object entity rendering fixes
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "gamesa_renderware.h"

static const float objLightingMod = ( 1.0f / 30.0f );

void __thiscall CObjectSAInterface::_PreRender( void )
{
    // This code is called to set up the rendering logic of objects and buildings.
    if ( bFixedLighting )
        SetupFixedLighting();

    // Do some logic that is very random.
    if ( objEffectSysTime > pGame->GetSystemTime() )
    {
        __asm
        {
            mov ecx,this
            mov eax,0x0059FB50
            call eax
        }
    }

    if ( !m_pAttachedEntity )
    {
        float dayNightBalance = *(float*)0x008D12C0;

        CColLighting& lighting = this->objLighting;

        float realDayLighting = (float)lighting.day * objLightingMod;
        float realNightLighting = (float)lighting.night * objLightingMod;

        float reverseDayNightBalance = 1.0f - dayNightBalance;

        float balancedDayLighting = realDayLighting * reverseDayNightBalance;
        float balancedNightLighting = realNightLighting * dayNightBalance;

        m_fLighting = balancedDayLighting + balancedNightLighting;
    }

    // Fade the object out.
    if ( RwObject *rwobj = GetRwObject() )
    {
        if ( rwobj->type == RW_CLUMP && bFadingOutClump )
        {
            RpClump *theClump = (RpClump*)rwobj;

            int alphaVal = RpClumpGetAlpha( theClump );

            alphaVal = std::max( 0, alphaVal - 16 );

            RpClumpSetAlpha( theClump, alphaVal );
        }
    }

    // Call CEntitySAInterface::PreRender
    __asm
    {
        mov ecx,this
        mov eax,0x00535FA0
        call eax
    }

    float objScale = this->fScale;

    if ( objScale != 1.0f || bUpdateScale )
    {
        RwObject *rwobj = GetRwObject();

        // MTA fix: postpone the update of the scale until we have a valid RenderWare object.
        if ( rwobj )
        {
            CVector scaleVec( objScale, objScale, objScale );

            // CRASH HERE.
            RwFrame *objFrame = rwobj->parent;

            Placeable.GetMatrix( objFrame->modelling );

            // MTA fix: use custom object scale if provided.
            const CVector *useScale = NULL;

            if ( CObjectSA *mtaObj = Pools::GetObject( this ) )
            {
                useScale = mtaObj->GetScale();
            }

            if ( !useScale )
                useScale = &scaleVec;

            RwMatrixScale( &objFrame->modelling, (const RwV3d*)useScale, TRANSFORM_BEFORE );

            // Make sure we recalculate this frame's hierarchy.
            objFrame->Update();

            // The_GTA: removed some friggin weird logic.
            bUpdateScale = false;
        }
    }

    if ( RwObject *rwobj = GetRwObject() )
    {
        if ( rwobj->type == RW_CLUMP )
        {
            // Update anim skeleton.
            __asm
            {
                mov ecx,this
                mov eax,0x00532B20
                call eax
            }
        }
    }
    
    // Yay, we set stuff up.
}

void CObjectSAInterface::SetupFixedLighting( void )
{
    const CVector& objPos = Placeable.GetPosition();

    CColPointSAInterface colPoint;
    CEntitySAInterface *hitEntity = NULL;

    bool hit = pGame->GetWorld()->ProcessVerticalLine( objPos, -1000.0f, colPoint, &hitEntity, true, false, false, false, true, false, false );

    if ( hit )
    {
        objLighting = colPoint.lightingB;
    }
}

void ObjectRender_Init( void )
{
    // Install crash fixes.
    HookInstall( 0x0059FD50, h_memFunc( &CObjectSAInterface::_PreRender ), 5 );
}

void ObjectRender_Shutdown( void )
{
}