/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/CGarageSA.h
*  PURPOSE:		Header file for garage handling class
*  DEVELOPERS:	Ed Lyons <eai@opencoding.net>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_GARAGE
#define __CGAMESA_GARAGE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <game/CGarage.h>
#include "CEntitySA.h"
#include "CAutomobileSA.h"

class CGarageSAInterface : public CGarageInterface
{
public:
    CVector vecPosition;                //  0
    FLOAT DirectionA_X, DirectionA_Y;   // 12
    FLOAT DirectionB_X, DirectionB_Y;   // 20
    FLOAT Height, Width, Depth;         // 28
    FLOAT Left, Right, Front, Back;     // 40
    FLOAT DoorPosition;                 // 56
    DWORD pad;                          // 60
    DWORD pad2;                         // 64
    char Name[ 8 ];                     // 68
    BYTE Type;                          // 76
    BYTE DoorState;                     // 77
    BYTE DoorFlags;                     // 78
    BYTE OriginalType;                  // 79
    BYTE pad3 [ 132 ];                  // 80
};

class CGarageSA : public CGarage
{
public:
                                CGarageSA       ( CGarageSAInterface * pInterface );

    void                        SetType         ( BYTE type );
    inline BYTE                 GetType         ( ) const   { return this->internalInterface->Type; }

    void                        GetPosition         ( CVector& vecPosition ) const;
    void                        GetDirection        ( float& x1, float& y1, float& x2, float& y2 ) const;
    void                        GetSize             ( float& height, float& width, float& depth ) const;
    void                        GetBoundingBox      ( float& left, float& right, float& front, float& back ) const;

    void                        SetOpen             ( bool bOpen );
    bool                        IsOpen              ( ) const;

    inline CGarageInterface*    GetInterface    ( )         { return this->internalInterface; }

private:
    CGarageSAInterface *        internalInterface;
};

#endif