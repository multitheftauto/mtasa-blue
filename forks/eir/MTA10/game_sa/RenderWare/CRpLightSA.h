/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/CRpLightSA.h
*  PURPOSE:     RenderWare RpLight export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RW_LIGHT_SA_
#define _RW_LIGHT_SA_

class CModelSA;

class CRpLightSA : public virtual CRpLight, public CRwObjectSA
{
    friend class CModelSA;
public:
                                    CRpLightSA          ( RpLight *light );
                                    ~CRpLightSA         ( void );

    inline RpLight*                 GetObject           ( void )                            { return (RpLight*)m_object; }
    inline const RpLight*           GetObject           ( void ) const                      { return (const RpLight*)m_object; }

    eRwType                         GetType             ( void ) const                      { return RW_LIGHT; }
    bool                            IsFrameExtension    ( void ) const                      { return true; }

    CRpLightSA*                     Clone               ( void ) const;

    void                            SetLightIndex       ( unsigned int idx );
    unsigned int                    GetLightIndex       ( void ) const;

    inline RpLightType              GetLightType        ( void ) const                      { return (RpLightType)GetObject()->subtype; }

    void                            SetAttenuation      ( const CVector& atten );
    const CVector&                  GetAttenuation      ( void ) const;
    
    void                            SetFalloff          ( float falloff );
    float                           GetFalloff          ( void ) const;

    void                            AddToModel          ( CModel *model );
    CModel*                         GetModel            ( void );
    void                            RemoveFromModel     ( void );

    void                            SetColor            ( const RwColorFloat& color )       { GetObject()->SetColor( color ); }
    const RwColorFloat&             GetColor            ( void ) const                      { return GetObject()->color; }

    void                            SetRadius           ( float radius );
    float                           GetRadius           ( void ) const                      { return GetObject()->radius; }

    void                            SetConeAngle        ( float radians );
    float                           GetConeAngle        ( void ) const;

    void                            SetDirection        ( const CVector& dirVec );
    void                            GetDirection        ( CVector& vecOut ) const;

    void                            AddToScene          ( void );
    bool                            IsAddedToScene      ( void ) const;
    void                            RemoveFromScene     ( void );

private:
    CModelSA*   m_model;
};

#endif //_RW_LIGHT_SA_