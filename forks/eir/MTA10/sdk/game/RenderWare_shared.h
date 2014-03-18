/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/RenderWare_shared.h
*  PURPOSE:     Shared renderware definitions
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RenderWare_Shared_H_
#define _RenderWare_Shared_H_

#define _USE_MATH_DEFINES
#include <math.h>
#include <rwlist.hpp>

// TODO: Remove the internal RW definitions, DIF
#include <CMatrix.h>
#include <CVector.h>

// RenderWare primitive types
struct RwV2d
{
    float x,y;
};
struct RwV3d
{
    float x,y,z;
};
typedef float RwV4d[4];
struct RwPlane
{
    RwV3d normal;
    float length;
};
struct RwBBox
{
    RwV3d max;
    RwV3d min;
};
struct RwSphere
{
    CVector pos;
    float radius;

    bool operator ==( const RwSphere& right ) const
    {
        return pos == right.pos && radius == right.radius;
    }
};
struct RwColorFloat
{
    RwColorFloat()  {}

    RwColorFloat( float red, float green, float blue, float alpha )
    {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }

    float r,g,b,a;
};
struct RwColor
{
    RwColor()
    {
        r = g = b = a = 0;
    }
    
    RwColor( unsigned int color )
    {
        *(unsigned int*)this = color;
    }

    // In little endian: 0xAABBGGRR (ABGR)
    unsigned char r, g, b, a;

    operator unsigned int ( void ) const
    {
        return *(unsigned int*)this;
    }

    operator unsigned int& ( void )
    {
        return *(unsigned int*)this;
    }

    DWORD ToD3DColor( void )
    {
        // Transform this color struct into something that the
        // Direct3D 9 device understands.
        return ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | ( b );
    }
};
enum eRwType : unsigned char
{
    RW_NULL,
    RW_ATOMIC,
    RW_CLUMP,
    RW_LIGHT,
    RW_CAMERA,
    RW_TXD = 6,
    RW_SCENE,
    RW_GEOMETRY
};
enum RpLightType : unsigned int
{
    LIGHT_TYPE_NULL = 0,
    
    LIGHT_TYPE_DIRECTIONAL = 1,
    LIGHT_TYPE_AMBIENT = 2,
    
    LIGHT_TYPE_POINT = 0x80,
    LIGHT_TYPE_SPOT_1 = 0x81,
    LIGHT_TYPE_SPOT_2 = 0x82
};

static const float negOne = -1.0f;

class RwMatrix
{   // 16-byte padded
public:
    inline void IdentityRotation( void )
    {
        vRight.fX = 1; vRight.fY = 0; vRight.fZ = 0;
        vFront.fX = 0; vFront.fY = 1; vFront.fZ = 0;
        vUp.fX = 0; vUp.fY = 0; vUp.fZ = 1;
    }

    inline void Identity( void )
    {
        IdentityRotation();
    }

    RwMatrix( void )
    {
        Identity();

        // Making a clean matrix.
        a = 0;
        b = 0;
        c = 0;
        w = 1.0f;
    }

    RwMatrix( const CMatrix& mat )
    {
        vRight = mat.vRight;
        vFront = mat.vFront;
        vUp = mat.vUp;
        vPos = mat.vPos;
    }

    operator CMatrix( void ) const
    {
        CMatrix mat;
        mat.vRight = vRight;
        mat.vFront = vFront;
        mat.vUp = vUp;
        mat.vPos = vPos;
        return mat;
    }

    inline void assign( const RwMatrix& mat )
    {
        vRight = mat.vRight;
        vUp = mat.vUp;
        vFront = mat.vFront;
        vPos = mat.vPos;
    }

    RwMatrix( const RwMatrix& mat )
    {
        assign( mat );

        // Making a clean matrix.
        a = 0;
        b = 0;
        c = 0;
        w = 1.0f;
    }

    void __thiscall Add( const RwMatrix& mat, RwMatrix& outmt )
    {
        __asm
        {
            mov eax,mat
            mov edx,outmt

            movups xmm0,[eax]RwMatrix.vRight
            movups xmm1,[eax]RwMatrix.vFront
            movups xmm2,[eax]RwMatrix.vUp
            movups xmm3,[eax]RwMatrix.vPos

            movups xmm4,[ecx]RwMatrix.vRight
            movups xmm5,[ecx]RwMatrix.vFront
            movups xmm6,[ecx]RwMatrix.vUp
            movups xmm7,[ecx]RwMatrix.vPos

            addps xmm0,xmm4
            addps xmm1,xmm5
            addps xmm2,xmm6
            addps xmm3,xmm7

            movups [edx]RwMatrix.vRight,xmm0
            movups [edx]RwMatrix.vFront,xmm1
            movups [edx]RwMatrix.vUp,xmm2
            movups [edx]RwMatrix.vPos,xmm3
        }
    }

    RwMatrix __thiscall operator + ( const RwMatrix& mat )
    {
        float outmt[16];

        Add( mat, *(RwMatrix*)outmt );
        return *(RwMatrix*)outmt;
    }

    void __thiscall Sub( const RwMatrix& mat, RwMatrix& outmt )
    {
        __asm
        {
            mov eax,mat
            mov edx,outmt

            movups xmm0,[eax]RwMatrix.vRight
            movups xmm1,[eax]RwMatrix.vFront
            movups xmm2,[eax]RwMatrix.vUp
            movups xmm3,[eax]RwMatrix.vPos

            movups xmm4,[ecx]RwMatrix.vRight
            movups xmm5,[ecx]RwMatrix.vFront
            movups xmm6,[ecx]RwMatrix.vUp
            movups xmm7,[ecx]RwMatrix.vPos

            subps xmm0,xmm4
            subps xmm1,xmm5
            subps xmm2,xmm6
            subps xmm3,xmm7

            movups [edx]RwMatrix.vRight,xmm0
            movups [edx]RwMatrix.vFront,xmm1
            movups [edx]RwMatrix.vUp,xmm2
            movups [edx]RwMatrix.vPos,xmm3
        }
    }

    RwMatrix __thiscall operator - ( const RwMatrix& mat )
    {
        float outmt[16];

        Sub( mat, *(RwMatrix*)outmt );
        return *(RwMatrix*)outmt;
    }

    RwMatrix operator * ( const RwMatrix& mat )
    {
        float outmt[16];
        Multiply( mat, (RwMatrix&)outmt );
        return (RwMatrix&)outmt;
    }

    RwMatrix operator / ( RwMatrix other )
    {
        other.Invert();

        return *this * other;
    }

    CVector operator * ( const CVector& vec )
    {
        return CVector(
            vRight.fX * vec.fX + vFront.fX * vec.fY + vUp.fX * vec.fZ,
            vRight.fY * vec.fX + vFront.fY * vec.fY + vUp.fY * vec.fZ,
            vRight.fZ * vec.fX + vFront.fZ * vec.fY + vUp.fZ * vec.fZ
        );
    }

#define DEG2RAD(x)  ( M_PI * x / 180 )
#define RAD2DEG(x)  ( x / M_PI * 180 )

    inline void FromHeading( float heading )
    {
        float ch = cos( heading );
        float sh = sin( heading );

        vRight[0] = ch;  vRight[1] = sh;  vRight[2] = 0;
        vFront[0] = -sh;    vFront[1] = ch;     vFront[2] = 0;
        vUp[0] = 0;      vUp[1] = 0;      vUp[2] = 1.0f;
    }

    inline float ToHeading( void ) const
    {
        return atan2( -vFront.fX, vFront.fY );
    }

    // I have done the homework for MTA
    inline void rotXY( float x, float y )
    {
        double ch = cos( x );
        double sh = sin( x );
        double cb = cos( y );
        double sb = sin( y );

        vRight[0] = (float)( cb );
        vRight[1] = 0;
        vRight[2] = (float)( sb );

        vFront[0] = (float)( sb * sh );
        vFront[1] = (float)( ch );
        vFront[2] = (float)( -sh * cb );

        vUp[0] = (float)( -sb * ch );
        vUp[1] = (float)( sh );
        vUp[2] = (float)( cb * ch );
    }

    inline void SetRotationRad( float x, float y, float z )
    {
        double ch = cos( x );
        double sh = sin( x );
        double cb = cos( y );
        double sb = sin( y );
        double ca = cos( z );
        double sa = sin( z );

        vRight[0] = (float)( ca * cb );
        vRight[1] = (float)( -sa * cb );
        vRight[2] = (float)( sb );

        vFront[0] = (float)( ca * sb * sh + sa * ch );
        vFront[1] = (float)( ca * ch - sa * sb * sh );
        vFront[2] = (float)( -sh * cb );

        vUp[0] = (float)( sa * sh - ca * sb * ch );
        vUp[1] = (float)( sa * sb * ch + ca * sh );
        vUp[2] = (float)( ch * cb );
    }

    inline void SetRotation( float x, float y, float z )
    {
        SetRotationRad( (float)DEG2RAD( x ), (float)DEG2RAD( y ), (float)DEG2RAD( z ) );
    }

    inline void GetRotationRad( float& x, float& y, float& z ) const
    {
        if ( vRight[2] == 1 )
        {
            y = (float)( M_PI / 2 );

            x = 0;
            z = (float)atan2( vRight[0], vRight[1] );
        }
        else if ( vRight[2] == -1 )
        {
            y = -(float)( M_PI / 2 );

            x = -0;
            z = (float)atan2( vRight[0], vRight[1] );
        }
        else
        {
            y = asin( vRight[2] );

            x = (float)atan2( -vFront[2], vUp[2] );
            z = (float)atan2( -vRight[1], vRight[0] );
        }
    }

    inline void GetRotation( float& x, float& y, float& z ) const
    {
        GetRotationRad( x, y, z );

        x = (float)RAD2DEG( x );
        y = (float)RAD2DEG( y );
        z = (float)RAD2DEG( z );
    }

    inline void __thiscall Multiply( const RwMatrix& mat, RwMatrix& dst ) const
    {
	    __asm
	    {
		    mov edx,[mat]
		    mov esi,[dst]

		    movups xmm4,[edx]RwMatrix.vRight
		    movups xmm5,[edx]RwMatrix.vFront
		    movups xmm6,[edx]RwMatrix.vUp
    		
		    movss xmm3,[ecx]RwMatrix.vFront
		    movss xmm7,[ecx]RwMatrix.vUp

		    // X
		    movss xmm0,[ecx]
		    movss xmm1,[ecx+4]
		    movss xmm2,[ecx+8]

		    shufps xmm0,xmm0,0x40
		    shufps xmm1,xmm1,0x40
		    shufps xmm2,xmm2,0x40
    		
		    // Do cache vars
		    shufps xmm3,xmm3,0x40
		    shufps xmm7,xmm7,0x40

		    mulps xmm0,xmm4
		    mulps xmm1,xmm5
		    mulps xmm2,xmm6

		    addps xmm0,xmm1
		    addps xmm0,xmm2

		    movups [esi]RwMatrix.vRight,xmm0

		    // Y
		    movss xmm1,[ecx+0x14]
		    movss xmm2,[ecx+0x18]

		    shufps xmm1,xmm1,0x40
		    shufps xmm2,xmm2,0x40

		    mulps xmm3,xmm4
		    mulps xmm1,xmm5
		    mulps xmm2,xmm6

		    addps xmm3,xmm1
		    addps xmm3,xmm2

		    movups [esi]RwMatrix.vFront,xmm3

		    // Z
		    movss xmm1,[ecx+0x24]
		    movss xmm2,[ecx+0x28]

		    shufps xmm1,xmm1,0x40
		    shufps xmm2,xmm2,0x40

		    mulps xmm7,xmm4
		    mulps xmm1,xmm5
		    mulps xmm2,xmm6

		    addps xmm7,xmm1
		    addps xmm7,xmm2

		    movups [esi]RwMatrix.vUp,xmm7
	    }
    }

    inline void __thiscall Invert( void )
    {
        // Optimization to use SSE registers instead of stack space
        __asm
        {
            movups xmm0,[ecx]RwMatrix.vRight
            movups xmm1,[ecx]RwMatrix.vFront
            movups xmm2,[ecx]RwMatrix.vUp
            movups xmm3,[ecx]RwMatrix.vPos

            // Prepare for position invert
            movss xmm4,negOne

            movss [ecx],xmm0
            movss [ecx+0x04],xmm1
            movss [ecx+0x08],xmm2

            // Left-shift the vectors
            shufps xmm0,xmm0,0x49
            shufps xmm1,xmm1,0x49
            shufps xmm2,xmm2,0x49

            // Pos invert prep
            shufps xmm4,xmm4,0x40

            movss [ecx+0x10],xmm0
            movss [ecx+0x14],xmm1
            movss [ecx+0x18],xmm2

            // Left-shift the vectors
            shufps xmm0,xmm0,0x49
            shufps xmm1,xmm1,0x49
            shufps xmm2,xmm2,0x49

            movss [ecx+0x20],xmm0
            movss [ecx+0x24],xmm1
            movss [ecx+0x28],xmm2

            // Invert the position
            mulps xmm3,xmm4
            movups [ecx+0x30],xmm3
        }
    }

    inline void __thiscall Transform( const CVector& point, CVector& vout ) const
    {
	    __asm
	    {
		    mov eax,[point]
		    mov esi,[vout]

            // __thiscall makes sure that our matrix is in ecx
		    movups xmm4,[ecx]RwMatrix.vRight
		    movups xmm5,[ecx]RwMatrix.vFront
		    movups xmm6,[ecx]RwMatrix.vUp
		    movups xmm3,[ecx]RwMatrix.vPos

            // Read the offset parameters
		    movss xmm0,[eax]CVector.fX
		    movss xmm1,[eax]CVector.fY
		    movss xmm2,[eax]CVector.fZ

            // Expand to vectors
		    shufps xmm0,xmm0,0x40
		    shufps xmm1,xmm1,0x40
		    shufps xmm2,xmm2,0x40

		    mulps xmm0,xmm4
		    mulps xmm1,xmm5
		    mulps xmm2,xmm6

		    addps xmm0,xmm1
		    addps xmm0,xmm2
		    addps xmm0,xmm3

            // Write the output
		    movss [esi]CVector.fX,xmm0
		    shufps xmm0,xmm0,0x49
		    movss [esi]CVector.fY,xmm0
		    shufps xmm0,xmm0,0x49
		    movss [esi]CVector.fZ,xmm0
	    }
    }

    float& operator [] ( unsigned int i )
    {
        assert( i < 0x10 );

        return ((float*)(this))[i];
    }

    float operator [] ( unsigned int i ) const
    {
        assert( i < 0x10 );

        return ((float*)(this))[i];
    }

#if 0
    CVector& operator [] ( unsigned int i )
    {
        assert( i < 4 );

        return ((CVector*)(this))[i];
    }

    CVector operator [] ( unsigned int i ) const
    {
        assert( i < 4 );

        return ((CVector*)(this))[i];
    }
#endif

    union
    {
        struct
        {
            CVector         vRight;
            unsigned int    a;  // anyone wondering about the flags: they were just there for debug.
            CVector         vFront;
            unsigned int    b;
            CVector         vUp;
            unsigned int    c;
            CVector         vPos;
            float           w;
        };
        struct
        {
            RwV3d           right;
            unsigned int    flags;  // anyone wondering about the flags: they were just there for debug.
            RwV3d           at;
            unsigned int    b;
            RwV3d           up;
            unsigned int    c;
            RwV3d           pos;
            float           w;
        };
    };
};

#endif