/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		Shared/sdk/net/bitstream.h
*  PURPOSE:		Network bitstream interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "Common.h"

struct ISyncStructure;

class NetBitStreamInterface
{
public:
    virtual             ~NetBitStreamInterface      ( void ) {};

    virtual int         GetReadOffsetAsBits         ( void ) = 0;

    virtual void        Reset                       ( void ) = 0;
    virtual void        ResetReadPointer            ( void ) = 0;

    // Don't use this, it screws up randomly in certain conditions causing packet misalign
    virtual void        Write                       ( const unsigned char& input ) = 0;
    virtual void        Write                       ( const char& input ) = 0;
    virtual void        Write                       ( const unsigned short& input ) = 0;
    virtual void        Write                       ( const short& input ) = 0;
    virtual void        Write                       ( const unsigned int& input ) = 0;
    virtual void        Write                       ( const int& input ) = 0;
    virtual void        Write                       ( const unsigned long& input ) = 0;
    virtual void        Write                       ( const long& input ) = 0;
    virtual void        Write                       ( const float& input ) = 0;
    virtual void        Write                       ( const double& input ) = 0;
    virtual void        Write                       ( const char* input, int numberOfBytes ) = 0;
    virtual void        Write                       ( const ISyncStructure* syncStruct ) = 0;

    virtual void        WriteCompressed             ( const unsigned char& input ) = 0;
    virtual void        WriteCompressed             ( const char& input ) = 0;
    virtual void        WriteCompressed             ( const unsigned short& input ) = 0;
    virtual void        WriteCompressed             ( const short& input ) = 0;
    virtual void        WriteCompressed             ( const unsigned int& input ) = 0;
    virtual void        WriteCompressed             ( const int& input ) = 0;
    virtual void        WriteCompressed             ( const unsigned long& input ) = 0;
    virtual void        WriteCompressed             ( const long& input ) = 0;
    virtual void        WriteCompressed             ( const float& input ) = 0;
    virtual void        WriteCompressed             ( const double& input ) = 0;

    virtual void        WriteBits                   ( const char* input, unsigned int numbits ) = 0;
    virtual void        WriteBit                    ( bool input ) = 0;

    // Write a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12 bytes.  Will further compress y or z axis aligned vectors. Accurate to 1/32767.5.
    virtual void        WriteNormVector             ( float x, float y, float z ) = 0;

    // Write a vector, using 10 bytes instead of 12. Loses accuracy to about 3/10ths and only saves 2 bytes, so only use if accuracy is not important.
    virtual void        WriteVector                 ( float x, float y, float z ) = 0;

    // Write a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.  Slightly lossy.
    virtual void        WriteNormQuat               ( float w, float x, float y, float z) = 0;

    // Write an orthogonal matrix by creating a quaternion, and writing 3 components of the quaternion in 2 bytes each for 6 bytes instead of 36
    virtual void        WriteOrthMatrix             (
                                                      float m00, float m01, float m02,
                                                      float m10, float m11, float m12,
                                                      float m20, float m21, float m22
                                                    ) = 0;

    virtual bool        Read                        ( unsigned char& output ) = 0;
    virtual bool        Read                        ( char& output ) = 0;
    virtual bool        Read                        ( unsigned short& output ) = 0;
    virtual bool        Read                        ( short& output ) = 0;
    virtual bool        Read                        ( unsigned int& output ) = 0;
    virtual bool        Read                        ( int& output ) = 0;
    virtual bool        Read                        ( unsigned long& output ) = 0;
    virtual bool        Read                        ( long& output ) = 0;
    virtual bool        Read                        ( float& output ) = 0;
    virtual bool        Read                        ( double& output ) = 0;
    virtual bool        Read                        ( char* output, int numberOfBytes ) = 0;
    virtual bool        Read                        ( ISyncStructure* syncStruct ) = 0;

    virtual bool        ReadCompressed              ( unsigned char& output ) = 0;
    virtual bool        ReadCompressed              ( char& output ) = 0;
    virtual bool        ReadCompressed              ( unsigned short& output ) = 0;
    virtual bool        ReadCompressed              ( short& output ) = 0;
    virtual bool        ReadCompressed              ( unsigned int& output ) = 0;
    virtual bool        ReadCompressed              ( int& output ) = 0;
    virtual bool        ReadCompressed              ( unsigned long& output ) = 0;
    virtual bool        ReadCompressed              ( long& output ) = 0;
    virtual bool        ReadCompressed              ( float& output ) = 0;
    virtual bool        ReadCompressed              ( double& output ) = 0;

    virtual bool        ReadBits                    ( char* output, unsigned int numbits ) = 0;
    virtual bool        ReadBit                     ( ) = 0;

    virtual bool        ReadNormVector              ( float &x, float &y, float &z ) = 0;
    virtual bool        ReadVector                  ( float &x, float &y, float &z ) = 0;
    virtual bool        ReadNormQuat                ( float &w, float &x, float &y, float &z) = 0;
    virtual bool        ReadOrthMatrix              (
                                                      float &m00, float &m01, float &m02,
                                                      float &m10, float &m11, float &m12,
                                                      float &m20, float &m21, float &m22
                                                    ) = 0;

    virtual int         GetNumberOfBitsUsed         ( void ) const = 0;
    virtual int         GetNumberOfBytesUsed        ( void ) const = 0;
    virtual int         GetNumberOfUnreadBits       ( void ) const = 0;

    // Helper template methods that are not actually part
    // of the interface but get inline compiled.

    template < typename T >
    inline void         WriteBits                   ( T * input, unsigned int numbits )
    {
        WriteBits ( reinterpret_cast < const char * > ( input ), numbits );
    }

    template < typename T >
    inline bool         ReadBits                    ( T * output, unsigned int numbits )
    {
        return ReadBits ( reinterpret_cast < char * > ( output ), numbits );
    }

    bool                ReadBit                     ( bool& output )
    {
        unsigned char ucTemp = 0;
        if ( ReadBits ( &ucTemp, 1 ) )
        {
            output = ucTemp & 1;
            return true;
        }
        return false;
    }

};

// Interface for all sync structures
struct ISyncStructure
{
    virtual         ~ISyncStructure () {}
    virtual bool    Read            ( NetBitStreamInterface& bitStream ) = 0;
    virtual void    Write           ( NetBitStreamInterface& bitStream ) const = 0;
};
