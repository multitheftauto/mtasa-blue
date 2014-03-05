/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/RenderWare/CRwFrame.h
*  PURPOSE:     RenderWare RwFrame modification export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RW_FRAME_
#define _RW_FRAME_

class CRwObject;

class CRwFrame abstract : public virtual CRwObject
{
public:
    virtual                         ~CRwFrame       ( void ) {}

    virtual void                    SetName         ( const char *name ) = 0;
    virtual const char*             GetName         ( void ) const = 0;
    virtual unsigned int            GetHash         ( void ) const = 0;

    virtual void                    Link            ( CRwFrame *child ) = 0;
    virtual void                    Unlink          ( void ) = 0;

    typedef std::list <CRwObject*> objectList_t;
    typedef std::list <CRwFrame*> childList_t;

    virtual objectList_t&           GetObjects      ( void ) = 0;
    virtual childList_t&            GetChildren     ( void ) = 0;

    virtual void                    SetLTM          ( const RwMatrix& mat ) = 0;
    virtual const RwMatrix&         GetLTM          ( void ) const = 0;

    virtual void                    SetModelling    ( const RwMatrix& mat ) = 0;
    virtual const RwMatrix&         GetModelling    ( void ) const = 0;

    virtual void                    SetPosition     ( const CVector& pos ) = 0;
    virtual const CVector&          GetPosition     ( void ) const = 0;
};

#endif //_RW_FRAME_