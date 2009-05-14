/// \file
/// \brief This class allows you to write and read native types as a string of bits.  BitStream is used extensively throughout RakNet and is designed to be used by users as well.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#if defined(_MSC_VER) && _MSC_VER < 1299 // VC6 doesn't support template specialization

#ifndef __BITSTREAM_H
#define __BITSTREAM_H

#include "RakMemoryOverride.h"
#include "RakNetDefines.h"
#include "Export.h"
#include "RakNetTypes.h"
#include "RakAssert.h"
#if defined(_PS3) || defined(__PS3__) || defined(SN_TARGET_PS3)
#include <math.h>
#else
#include <cmath>
#endif
#include <float.h>

#ifdef _MSC_VER
#pragma warning( push )
#endif

/// Arbitrary size, just picking something likely to be larger than most packets
#define BITSTREAM_STACK_ALLOCATION_SIZE 256

/// The namespace RakNet is not consistently used.  It's only purpose is to avoid compiler errors for classes whose names are very common.
/// For the most part I've tried to avoid this simply by using names very likely to be unique for my classes.
namespace RakNet
{
	/// This class allows you to write and read native types as a string of bits.  BitStream is used extensively throughout RakNet and is designed to be used by users as well.
	/// \sa BitStreamSample.txt
	class RAK_DLL_EXPORT BitStream
	{

	public:
		/// Default Constructor
		BitStream();

		/// Create the bitstream, with some number of bytes to immediately allocate.
		/// There is no benefit to calling this, unless you know exactly how many bytes you need and it is greater than BITSTREAM_STACK_ALLOCATION_SIZE.
		/// In that case all it does is save you one or more realloc calls.
		/// \param[in] initialBytesToAllocate the number of bytes to pre-allocate.
		BitStream( int initialBytesToAllocate );

		/// Initialize the BitStream, immediately setting the data it contains to a predefined pointer.
		/// Set \a _copyData to true if you want to make an internal copy of the data you are passing. Set it to false to just save a pointer to the data.
		/// You shouldn't call Write functions with \a _copyData as false, as this will write to unallocated memory
		/// 99% of the time you will use this function to cast Packet::data to a bitstream for reading, in which case you should write something as follows:
		/// \code
		/// RakNet::BitStream bs(packet->data, packet->length, false);
		/// \endcode
		/// \param[in] _data An array of bytes.
		/// \param[in] lengthInBytes Size of the \a _data.
		/// \param[in] _copyData true or false to make a copy of \a _data or not.
		BitStream( unsigned char* _data, unsigned int lengthInBytes, bool _copyData );

		/// Destructor
		~BitStream();

		/// Resets the bitstream for reuse.
		void Reset( void );

		/// Bidirectional serialize/deserialize any integral type to/from a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] var The value to write
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool Serialize(bool writeToBitstream, bool &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, unsigned char &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, char &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, unsigned short &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, short &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, unsigned int &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, int &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, unsigned long &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, long &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, long long &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, unsigned long long &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, float &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, double &var){if (writeToBitstream)Write(var);else return Read(var); return true;}
		bool Serialize(bool writeToBitstream, long double &var){if (writeToBitstream)Write(var);else return Read(var); return true;}

		/// Bidirectional serialize/deserialize any integral type to/from a bitstream.  If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against.  Only used if \a writeToBitstream is true.
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool SerializeDelta(bool writeToBitstream, bool &currentValue, bool lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, unsigned char &currentValue, unsigned char lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, char &currentValue, char lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, unsigned short &currentValue, unsigned short lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, short &currentValue, short lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, unsigned int &currentValue, unsigned int lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, int &currentValue, int lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, unsigned long &currentValue, unsigned long lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, long long &currentValue, long long lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, unsigned long long &currentValue, unsigned long long lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, float &currentValue, float lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, double &currentValue, double lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, long double &currentValue, long double lastValue){if (writeToBitstream) WriteDelta(currentValue, lastValue); else return ReadDelta(currentValue);return true;}

		/// Bidirectional version of SerializeDelta when you don't know what the last value is, or there is no last value.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] currentValue The current value to write
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool SerializeDelta(bool writeToBitstream, bool &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, unsigned char &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, char &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, unsigned short &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, short &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, unsigned int &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, int &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, unsigned long &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, long long &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, unsigned long long &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, float &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, double &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}
		bool SerializeDelta(bool writeToBitstream, long double &currentValue){if (writeToBitstream) WriteDelta(currentValue); else return ReadDelta(currentValue);return true;}

		/// Bidirectional serialize/deserialize any integral type to/from a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] var The value to write
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool SerializeCompressed(bool writeToBitstream, bool &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, unsigned char &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, char &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, unsigned short &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, short &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, unsigned int &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, int &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, unsigned long &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, long &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, long long &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, unsigned long long &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, float &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, double &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}
		bool SerializeCompressed(bool writeToBitstream, long double &var){if (writeToBitstream)WriteCompressed(var);else return ReadCompressed(var); return true;}

		/// Bidirectional serialize/deserialize any integral type to/from a bitstream.  If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against.  Only used if \a writeToBitstream is true.
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool SerializeCompressedDelta(bool writeToBitstream, bool &currentValue, bool lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, unsigned char &currentValue, unsigned char lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, char &currentValue, char lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, unsigned short &currentValue, unsigned short lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, short &currentValue, short lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, unsigned int &currentValue, unsigned int lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, int &currentValue, int lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, unsigned long &currentValue, unsigned long lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, long long &currentValue, long long lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, unsigned long long &currentValue, unsigned long long lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, float &currentValue, float lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, double &currentValue, double lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, long double &currentValue, long double lastValue){if (writeToBitstream) WriteCompressedDelta(currentValue, lastValue); else return ReadCompressedDelta(currentValue);return true;}

		/// Save as SerializeCompressedDelta(templateType &currentValue, templateType lastValue) when we have an unknown second parameter
		bool SerializeCompressedDelta(bool writeToBitstream, bool &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, unsigned char &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, char &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, unsigned short &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, short &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, unsigned int &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, int &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, unsigned long &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, long &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, long long &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, unsigned long long &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, float &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, double &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}
		bool SerializeCompressedDelta(bool writeToBitstream, long double &var){if (writeToBitstream)WriteCompressedDelta(var);else return ReadCompressedDelta(var); return true;}

		/// Bidirectional serialize/deserialize an array or casted stream or raw data.  This does NOT do endian swapping.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] input a byte buffer
		/// \param[in] numberOfBytes the size of \a input in bytes
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool Serialize(bool writeToBitstream,  char* input, const int numberOfBytes );

		/// Bidirectional serialize/deserialize a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12-24 bytes.  Will further compress y or z axis aligned vectors.
		/// Accurate to 1/32767.5.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool SerializeNormVector(bool writeToBitstream,  float &x, float &y, float z ){if (writeToBitstream) WriteNormVector(x,y,z); else return ReadNormVector(x,y,z); return true;}
		bool SerializeNormVector(bool writeToBitstream,  double &x, double &y, double &z ){if (writeToBitstream) WriteNormVector(x,y,z); else return ReadNormVector(x,y,z); return true;}

		/// Bidirectional serialize/deserialize a vector, using 10 bytes instead of 12.
		/// Loses accuracy to about 3/10ths and only saves 2 bytes, so only use if accuracy is not important.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool SerializeVector(bool writeToBitstream,  float &x, float &y, float &z ){if (writeToBitstream) WriteVector(x,y,z); else	return ReadVector(x,y,z); return true;}
		bool SerializeVector(bool writeToBitstream,  double &x, double &y, double &z ){if (writeToBitstream) WriteVector(x,y,z); else	return ReadVector(x,y,z); return true;}

		/// Bidirectional serialize/deserialize a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.  Slightly lossy.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] w w
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool SerializeNormQuat(bool writeToBitstream,  float &w, float &x, float &y, float &z){if (writeToBitstream) WriteNormQuat(w,x,y,z); else return ReadNormQuat(w,x,y,z); return true;}
		bool SerializeNormQuat(bool writeToBitstream,  double &w, double &x, double &y, double &z){if (writeToBitstream) WriteNormQuat(w,x,y,z); else return ReadNormQuat(w,x,y,z); return true;}

		/// Bidirectional serialize/deserialize an orthogonal matrix by creating a quaternion, and writing 3 components of the quaternion in 2 bytes each
		/// for 6 bytes instead of 36
		/// Lossy, although the result is renormalized
		bool SerializeOrthMatrix(
		bool writeToBitstream,
		float &m00, float &m01, float &m02,
		float &m10, float &m11, float &m12,
		float &m20, float &m21, float &m22 ){if (writeToBitstream) WriteOrthMatrix(m00,m01,m02,m10,m11,m12,m20,m21,m22); else return ReadOrthMatrix(m00,m01,m02,m10,m11,m12,m20,m21,m22); return true;}
		bool SerializeOrthMatrix(
		bool writeToBitstream,
		double &m00, double &m01, double &m02,
		double &m10, double &m11, double &m12,
		double &m20, double &m21, double &m22 ){if (writeToBitstream) WriteOrthMatrix(m00,m01,m02,m10,m11,m12,m20,m21,m22); else return ReadOrthMatrix(m00,m01,m02,m10,m11,m12,m20,m21,m22); return true;}

		/// Bidirectional serialize/deserialize numberToSerialize bits to/from the input. Right aligned
		/// data means in the case of a partial byte, the bits are aligned
		/// from the right (bit 0) rather than the left (as in the normal
		/// internal representation) You would set this to true when
		/// writing user data, and false when copying bitstream data, such
		/// as writing one bitstream to another
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] input The data
		/// \param[in] numberOfBitsToSerialize The number of bits to write
		/// \param[in] rightAlignedBits if true data will be right aligned
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool SerializeBits(bool writeToBitstream, unsigned char* input, int numberOfBitsToSerialize, const bool rightAlignedBits = true );

		/// Write any integral type to a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// \param[in] var The value to write
		void Write(bool var){if ( var ) Write1(); else Write0();}
		void Write(unsigned char var){WriteBits( ( unsigned char* ) & var, sizeof( unsigned char ) * 8, true );}
		void Write(char var){WriteBits( ( unsigned char* ) & var, sizeof( char ) * 8, true );}
		void Write(unsigned short var) {if (DoEndianSwap()){unsigned char output[sizeof(unsigned short)]; ReverseBytes((unsigned char*)&var, output, sizeof(unsigned short)); WriteBits( ( unsigned char* ) output, sizeof(unsigned short) * 8, true );} WriteBits( ( unsigned char* ) & var, sizeof(unsigned short) * 8, true );}
		void Write(short var) {if (DoEndianSwap()){unsigned char output[sizeof(short)]; ReverseBytes((unsigned char*)&var, output, sizeof(short)); WriteBits( ( unsigned char* ) output, sizeof(short) * 8, true );} WriteBits( ( unsigned char* ) & var, sizeof(short) * 8, true );}
		void Write(unsigned int var) {if (DoEndianSwap()){unsigned char output[sizeof(unsigned int)]; ReverseBytes((unsigned char*)&var, output, sizeof(unsigned int)); WriteBits( ( unsigned char* ) output, sizeof(unsigned int) * 8, true );} WriteBits( ( unsigned char* ) & var, sizeof(unsigned int) * 8, true );}
		void Write(int var) {if (DoEndianSwap()){unsigned char output[sizeof(int)]; ReverseBytes((unsigned char*)&var, output, sizeof(int)); WriteBits( ( unsigned char* ) output, sizeof(int) * 8, true );} WriteBits( ( unsigned char* ) & var, sizeof(int) * 8, true );}
		void Write(unsigned long var) {if (DoEndianSwap()){unsigned char output[sizeof(unsigned long)]; ReverseBytes((unsigned char*)&var, output, sizeof(unsigned long)); WriteBits( ( unsigned char* ) output, sizeof(unsigned long) * 8, true );} WriteBits( ( unsigned char* ) & var, sizeof(unsigned long) * 8, true );}
		void Write(long var) {if (DoEndianSwap()){unsigned char output[sizeof(long)]; ReverseBytes((unsigned char*)&var, output, sizeof(long)); WriteBits( ( unsigned char* ) output, sizeof(long) * 8, true );} WriteBits( ( unsigned char* ) & var, sizeof(long) * 8, true );}
		void Write(long long var) {if (DoEndianSwap()){unsigned char output[sizeof(long long)]; ReverseBytes((unsigned char*)&var, output, sizeof(long long)); WriteBits( ( unsigned char* ) output, sizeof(long long) * 8, true );} WriteBits( ( unsigned char* ) & var, sizeof(long long) * 8, true );}
		void Write(unsigned long long var) {if (DoEndianSwap()){unsigned char output[sizeof(unsigned long long)]; ReverseBytes((unsigned char*)&var, output, sizeof(unsigned long long)); WriteBits( ( unsigned char* ) output, sizeof(unsigned long long) * 8, true );} WriteBits( ( unsigned char* ) & var, sizeof(unsigned long long) * 8, true );}
		void Write(float var) {if (DoEndianSwap()){unsigned char output[sizeof(float)]; ReverseBytes((unsigned char*)&var, output, sizeof(float)); WriteBits( ( unsigned char* ) output, sizeof(float) * 8, true );} WriteBits( ( unsigned char* ) & var, sizeof(float) * 8, true );}
		void Write(double var) {if (DoEndianSwap()){unsigned char output[sizeof(double)]; ReverseBytes((unsigned char*)&var, output, sizeof(double)); WriteBits( ( unsigned char* ) output, sizeof(double) * 8, true );} WriteBits( ( unsigned char* ) & var, sizeof(double) * 8, true );}
		void Write(long double var) {if (DoEndianSwap()){unsigned char output[sizeof(long double)]; ReverseBytes((unsigned char*)&var, output, sizeof(long double)); WriteBits( ( unsigned char* ) output, sizeof(long double) * 8, true );} WriteBits( ( unsigned char* ) & var, sizeof(long double) * 8, true );}
		void Write(void* var) {if (DoEndianSwap()){unsigned char output[sizeof(void*)]; ReverseBytes((unsigned char*)&var, output, sizeof(void*)); WriteBits( ( unsigned char* ) output, sizeof(void*) * 8, true );} WriteBits( ( unsigned char* ) & var, sizeof(void*) * 8, true );}
		void Write(SystemAddress var){WriteBits( ( unsigned char* ) & var.binaryAddress, sizeof(var.binaryAddress) * 8, true ); Write(var.port);}
		void Write(NetworkID var){if (NetworkID::IsPeerToPeerMode()) Write(var.systemAddress); Write(var.localSystemAddress);}

		/// Write any integral type to a bitstream.  If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against
		void WriteDelta(bool currentValue, bool lastValue){
			#pragma warning(disable:4100)   // warning C4100: 'peer' : unreferenced formal parameter
			Write(currentValue);
		}
		void WriteDelta(unsigned char currentValue, unsigned char lastValue){if (currentValue==lastValue) {Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(char currentValue, char lastValue){if (currentValue==lastValue)	{Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(unsigned short currentValue, unsigned short lastValue){if (currentValue==lastValue)	{Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(short currentValue, short lastValue){if (currentValue==lastValue) {Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(unsigned int currentValue, unsigned int lastValue){if (currentValue==lastValue)	{Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(int currentValue, int lastValue){if (currentValue==lastValue) {Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(unsigned long currentValue, unsigned long lastValue){if (currentValue==lastValue) {Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(long currentValue, long lastValue){if (currentValue==lastValue)	{Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(long long currentValue, long long lastValue){if (currentValue==lastValue) {Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(unsigned long long currentValue, unsigned long long lastValue){if (currentValue==lastValue)	{Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(float currentValue, float lastValue){if (currentValue==lastValue) {Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(double currentValue, double lastValue){if (currentValue==lastValue)	{Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(long double currentValue, long double lastValue){if (currentValue==lastValue) {Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(SystemAddress currentValue, SystemAddress lastValue){if (currentValue==lastValue) {Write(false);} else {Write(true); Write(currentValue);}}
		void WriteDelta(NetworkID currentValue, NetworkID lastValue){if (currentValue==lastValue) {Write(false);} else {Write(true); Write(currentValue);}}

		/// WriteDelta when you don't know what the last value is, or there is no last value.
		/// \param[in] currentValue The current value to write
		void WriteDelta(bool var){Write(var);}
		void WriteDelta(unsigned char var){Write(true); Write(var);}
		void WriteDelta(char var){Write(true); Write(var);}
		void WriteDelta(unsigned short var){Write(true); Write(var);}
		void WriteDelta(short var){Write(true); Write(var);}
		void WriteDelta(unsigned int var){Write(true); Write(var);}
		void WriteDelta(int var){Write(true); Write(var);}
		void WriteDelta(unsigned long var){Write(true); Write(var);}
		void WriteDelta(long var){Write(true); Write(var);}
		void WriteDelta(long long var){Write(true); Write(var);}
		void WriteDelta(unsigned long long var){Write(true); Write(var);}
		void WriteDelta(float var){Write(true); Write(var);}
		void WriteDelta(double var){Write(true); Write(var);}
		void WriteDelta(long double var){Write(true); Write(var);}
		void WriteDelta(SystemAddress var){Write(true); Write(var);}
		void WriteDelta(NetworkID var){Write(true); Write(var);}

		/// Write any integral type to a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
		/// \param[in] var The value to write
		void WriteCompressed(bool var) {Write(var);}
		void WriteCompressed(unsigned char var) {WriteCompressed( ( unsigned char* ) & var, sizeof( unsigned char ) * 8, true );}
		void WriteCompressed(char var) {WriteCompressed( (unsigned char* ) & var, sizeof( unsigned char ) * 8, true );}
		void WriteCompressed(unsigned short var) {if (DoEndianSwap()) {unsigned char output[sizeof(unsigned short)]; ReverseBytes((unsigned char*)&var, output, sizeof(unsigned short)); WriteCompressed( ( unsigned char* ) output, sizeof(unsigned short) * 8, true );} else WriteCompressed( ( unsigned char* ) & var, sizeof(unsigned short) * 8, true );}
		void WriteCompressed(short var) {if (DoEndianSwap()) {unsigned char output[sizeof(short)]; ReverseBytes((unsigned char*)&var, output, sizeof(short)); WriteCompressed( ( unsigned char* ) output, sizeof(short) * 8, true );} else WriteCompressed( ( unsigned char* ) & var, sizeof(short) * 8, true );}
		void WriteCompressed(unsigned int var) {if (DoEndianSwap()) {unsigned char output[sizeof(unsigned int)]; ReverseBytes((unsigned char*)&var, output, sizeof(unsigned int)); WriteCompressed( ( unsigned char* ) output, sizeof(unsigned int) * 8, true );} else WriteCompressed( ( unsigned char* ) & var, sizeof(unsigned int) * 8, true );}
		void WriteCompressed(int var) {if (DoEndianSwap()) { unsigned char output[sizeof(int)]; ReverseBytes((unsigned char*)&var, output, sizeof(int)); WriteCompressed( ( unsigned char* ) output, sizeof(int) * 8, true );} else WriteCompressed( ( unsigned char* ) & var, sizeof(int) * 8, true );}
		void WriteCompressed(unsigned long var) {if (DoEndianSwap()) {unsigned char output[sizeof(unsigned long)]; ReverseBytes((unsigned char*)&var, output, sizeof(unsigned long)); WriteCompressed( ( unsigned char* ) output, sizeof(unsigned long) * 8, true );} else WriteCompressed( ( unsigned char* ) & var, sizeof(unsigned long) * 8, true );}
		void WriteCompressed(long var) {if (DoEndianSwap()) {unsigned char output[sizeof(long)]; ReverseBytes((unsigned char*)&var, output, sizeof(long)); WriteCompressed( ( unsigned char* ) output, sizeof(long) * 8, true );} else WriteCompressed( ( unsigned char* ) & var, sizeof(long) * 8, true );}
		void WriteCompressed(long long var) {if (DoEndianSwap()) {unsigned char output[sizeof(long long)]; ReverseBytes((unsigned char*)&var, output, sizeof(long long)); WriteCompressed( ( unsigned char* ) output, sizeof(long long) * 8, true );} else WriteCompressed( ( unsigned char* ) & var, sizeof(long long) * 8, true );}
		void WriteCompressed(unsigned long long var) {if (DoEndianSwap()) {	unsigned char output[sizeof(unsigned long long)]; ReverseBytes((unsigned char*)&var, output, sizeof(unsigned long long)); WriteCompressed( ( unsigned char* ) output, sizeof(unsigned long long) * 8, true );} else WriteCompressed( ( unsigned char* ) & var, sizeof(unsigned long long) * 8, true );}
		void WriteCompressed(float var) {RakAssert(var > -1.01f && var < 1.01f); if (var < -1.0f) var=-1.0f; if (var > 1.0f) var=1.0f; Write((unsigned short)((var+1.0f)*32767.5f));}
		void WriteCompressed(double var) {RakAssert(var > -1.01 && var < 1.01); if (var < -1.0) var=-1.0; if (var > 1.0) var=1.0; Write((unsigned long)((var+1.0)*2147483648.0));}
		void WriteCompressed(long double var) {RakAssert(var > -1.01 && var < 1.01); if (var < -1.0) var=-1.0; if (var > 1.0) var=1.0; Write((unsigned long)((var+1.0)*2147483648.0));}

		/// Write any integral type to a bitstream.  If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against
		void WriteCompressedDelta(bool currentValue, bool lastValue)
		{
			#pragma warning(disable:4100)   // warning C4100: 'peer' : unreferenced formal parameter
			Write(currentValue);
		}
		void WriteCompressedDelta(unsigned char currentValue, unsigned char lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}
		void WriteCompressedDelta(char currentValue, char lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}
		void WriteCompressedDelta(unsigned short currentValue, unsigned short lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}
		void WriteCompressedDelta(short currentValue, short lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}
		void WriteCompressedDelta(unsigned int currentValue, unsigned int lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}
		void WriteCompressedDelta(int currentValue, int lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}
		void WriteCompressedDelta(unsigned long currentValue, unsigned long lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}
		void WriteCompressedDelta(long currentValue, long lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}
		void WriteCompressedDelta(long long currentValue, long long lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}
		void WriteCompressedDelta(unsigned long long currentValue, unsigned long long lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}
		void WriteCompressedDelta(float currentValue, float lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}
		void WriteCompressedDelta(double currentValue, double lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}
		void WriteCompressedDelta(long double currentValue, long double lastValue){if (currentValue==lastValue) {Write(false);} else { Write(true); WriteCompressed(currentValue);}}

		/// Save as WriteCompressedDelta(templateType currentValue, templateType lastValue) when we have an unknown second parameter
		void WriteCompressedDelta(bool var) {Write(var);}
		void WriteCompressedDelta(unsigned char var) { Write(true);	WriteCompressed(var); }
		void WriteCompressedDelta(char var) { Write(true);	WriteCompressed(var); }
		void WriteCompressedDelta(unsigned short var) { Write(true);	WriteCompressed(var); }
		void WriteCompressedDelta(short var) { Write(true);	WriteCompressed(var); }
		void WriteCompressedDelta(unsigned int var) { Write(true);	WriteCompressed(var); }
		void WriteCompressedDelta(int var) { Write(true);	WriteCompressed(var); }
		void WriteCompressedDelta(unsigned long var) { Write(true);	WriteCompressed(var); }
		void WriteCompressedDelta(long var) { Write(true);	WriteCompressed(var); }
		void WriteCompressedDelta(long long var) { Write(true);	WriteCompressed(var); }
		void WriteCompressedDelta(unsigned long long var) { Write(true);	WriteCompressed(var); }
		void WriteCompressedDelta(float var) { Write(true);	WriteCompressed(var); }
		void WriteCompressedDelta(double var) { Write(true);	WriteCompressed(var); }
		void WriteCompressedDelta(long double var) { Write(true);	WriteCompressed(var); }

		/// Read any integral type from a bitstream.  Define __BITSTREAM_NATIVE_END if you need endian swapping.
		/// \param[in] var The value to read
		bool Read(bool &var){if ( readOffset + 1 > numberOfBitsUsed ) return false;
		if ( data[ readOffset >> 3 ] & ( 0x80 >> ( readOffset & 7 ) ) )
				var = true;
			else
				var = false;
			// Has to be on a different line for Mac
			readOffset++;
			return true;
		}
		bool Read(unsigned char &var) {return ReadBits( ( unsigned char* ) &var, sizeof(unsigned char) * 8, true );}
		bool Read(char &var) {return ReadBits( ( unsigned char* ) &var, sizeof(char) * 8, true );}
		bool Read(unsigned short &var) {if (DoEndianSwap()){unsigned char output[sizeof(unsigned short)]; if (ReadBits( ( unsigned char* ) output, sizeof(unsigned short) * 8, true )) { ReverseBytes(output, (unsigned char*)&var, sizeof(unsigned short)); return true;} return false;} else return ReadBits( ( unsigned char* ) & var, sizeof(unsigned short) * 8, true );}
		bool Read(short &var) {if (DoEndianSwap()){unsigned char output[sizeof(short)]; if (ReadBits( ( unsigned char* ) output, sizeof(short) * 8, true )) { ReverseBytes(output, (unsigned char*)&var, sizeof(short)); return true;} return false;} else return ReadBits( ( unsigned char* ) & var, sizeof(short) * 8, true );}
		bool Read(unsigned int &var) {if (DoEndianSwap()){unsigned char output[sizeof(unsigned int)]; if (ReadBits( ( unsigned char* ) output, sizeof(unsigned int) * 8, true )) { ReverseBytes(output, (unsigned char*)&var, sizeof(unsigned int)); return true;} return false;} else return ReadBits( ( unsigned char* ) & var, sizeof(unsigned int) * 8, true );}
		bool Read(int &var) {if (DoEndianSwap()){unsigned char output[sizeof(int)]; if (ReadBits( ( unsigned char* ) output, sizeof(int) * 8, true )) { ReverseBytes(output, (unsigned char*)&var, sizeof(int)); return true;} return false;} else return ReadBits( ( unsigned char* ) & var, sizeof(int) * 8, true );}
		bool Read(unsigned long &var) {if (DoEndianSwap()){unsigned char output[sizeof(unsigned long)]; if (ReadBits( ( unsigned char* ) output, sizeof(unsigned long) * 8, true )) { ReverseBytes(output, (unsigned char*)&var, sizeof(unsigned long)); return true;} return false;} else return ReadBits( ( unsigned char* ) & var, sizeof(unsigned long) * 8, true );}
		bool Read(long &var) {if (DoEndianSwap()){unsigned char output[sizeof(long)]; if (ReadBits( ( unsigned char* ) output, sizeof(long) * 8, true )) { ReverseBytes(output, (unsigned char*)&var, sizeof(long)); return true;} return false;} else return ReadBits( ( unsigned char* ) & var, sizeof(long) * 8, true );}
		bool Read(long long &var) {if (DoEndianSwap()){unsigned char output[sizeof(long long)]; if (ReadBits( ( unsigned char* ) output, sizeof(long long) * 8, true )) { ReverseBytes(output, (unsigned char*)&var, sizeof(long long)); return true;} return false;} else return ReadBits( ( unsigned char* ) & var, sizeof(long long) * 8, true );}
		bool Read(unsigned long long &var) {if (DoEndianSwap()){unsigned char output[sizeof(unsigned long long)]; if (ReadBits( ( unsigned char* ) output, sizeof(unsigned long long) * 8, true )) { ReverseBytes(output, (unsigned char*)&var, sizeof(unsigned long long)); return true;} return false;} else return ReadBits( ( unsigned char* ) & var, sizeof(unsigned long long) * 8, true );}
		bool Read(float &var) {if (DoEndianSwap()){unsigned char output[sizeof(float)]; if (ReadBits( ( unsigned char* ) output, sizeof(float) * 8, true )) { ReverseBytes(output, (unsigned char*)&var, sizeof(float)); return true;} return false;} else return ReadBits( ( unsigned char* ) & var, sizeof(float) * 8, true );}
		bool Read(double &var) {if (DoEndianSwap()){unsigned char output[sizeof(double)]; if (ReadBits( ( unsigned char* ) output, sizeof(double) * 8, true )) { ReverseBytes(output, (unsigned char*)&var, sizeof(double)); return true;} return false;} else return ReadBits( ( unsigned char* ) & var, sizeof(double) * 8, true );}
		bool Read(long double &var) {if (DoEndianSwap()){unsigned char output[sizeof(long double)]; if (ReadBits( ( unsigned char* ) output, sizeof(long double) * 8, true )) { ReverseBytes(output, (unsigned char*)&var, sizeof(long double)); return true;} return false;} else return ReadBits( ( unsigned char* ) & var, sizeof(long double) * 8, true );}
		bool Read(void* &var) {if (DoEndianSwap()){unsigned char output[sizeof(void*)]; if (ReadBits( ( unsigned char* ) output, sizeof(void*) * 8, true )) { ReverseBytes(output, (unsigned char*)&var, sizeof(void*)); return true;} return false;} else return ReadBits( ( unsigned char* ) & var, sizeof(void*) * 8, true );}
		bool Read(SystemAddress &var){ReadBits( ( unsigned char* ) & var.binaryAddress, sizeof(var.binaryAddress) * 8, true ); return Read(var.port);}
		bool Read(NetworkID &var){if (NetworkID::IsPeerToPeerMode()) Read(var.systemAddress); return Read(var.localSystemAddress);}

		/// Read any integral type from a bitstream.  If the written value differed from the value compared against in the write function,
		/// var will be updated.  Otherwise it will retain the current value.
		/// ReadDelta is only valid from a previous call to WriteDelta
		/// \param[in] var The value to read
		bool ReadDelta(bool &var) {return Read(var);}
		bool ReadDelta(unsigned char &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(char &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(unsigned short &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(short &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(unsigned int &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(int &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(unsigned long &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(long &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(long long &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(unsigned long long &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(float &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(double &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(long double &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(SystemAddress &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}
		bool ReadDelta(NetworkID &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=Read(var); return success;}


		/// Read any integral type from a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// \param[in] var The value to read
		bool ReadCompressed(bool &var) {return Read(var);}
		bool ReadCompressed(unsigned char &var) {return ReadCompressed( ( unsigned char* ) &var, sizeof(unsigned char) * 8, true );}
		bool ReadCompressed(char &var) {return ReadCompressed( ( unsigned char* ) &var, sizeof(unsigned char) * 8, true );}
		bool ReadCompressed(unsigned short &var){if (DoEndianSwap()){unsigned char output[sizeof(unsigned short)]; if (ReadCompressed( ( unsigned char* ) output, sizeof(unsigned short) * 8, true )){ReverseBytes(output, (unsigned char*)&var, sizeof(unsigned short)); return true;} return false;}else return ReadCompressed( ( unsigned char* ) & var, sizeof(unsigned short) * 8, true );}
		bool ReadCompressed(short &var){if (DoEndianSwap()){unsigned char output[sizeof(short)]; if (ReadCompressed( ( unsigned char* ) output, sizeof(short) * 8, true )){ReverseBytes(output, (unsigned char*)&var, sizeof(short)); return true;} return false;}else return ReadCompressed( ( unsigned char* ) & var, sizeof(short) * 8, true );}
		bool ReadCompressed(unsigned int &var){if (DoEndianSwap()){unsigned char output[sizeof(unsigned int)]; if (ReadCompressed( ( unsigned char* ) output, sizeof(unsigned int) * 8, true )){ReverseBytes(output, (unsigned char*)&var, sizeof(unsigned int)); return true;} return false;}else return ReadCompressed( ( unsigned char* ) & var, sizeof(unsigned int) * 8, true );}
		bool ReadCompressed(int &var){if (DoEndianSwap()){unsigned char output[sizeof(int)]; if (ReadCompressed( ( unsigned char* ) output, sizeof(int) * 8, true )){ReverseBytes(output, (unsigned char*)&var, sizeof(int)); return true;} return false;}else return ReadCompressed( ( unsigned char* ) & var, sizeof(int) * 8, true );}
		bool ReadCompressed(unsigned long &var){if (DoEndianSwap()){unsigned char output[sizeof(unsigned long)]; if (ReadCompressed( ( unsigned char* ) output, sizeof(unsigned long) * 8, true )){ReverseBytes(output, (unsigned char*)&var, sizeof(unsigned long)); return true;} return false;}else return ReadCompressed( ( unsigned char* ) & var, sizeof(unsigned long) * 8, true );}
		bool ReadCompressed(long &var){if (DoEndianSwap()){unsigned char output[sizeof(long)]; if (ReadCompressed( ( unsigned char* ) output, sizeof(long) * 8, true )){ReverseBytes(output, (unsigned char*)&var, sizeof(long)); return true;} return false;}else return ReadCompressed( ( unsigned char* ) & var, sizeof(long) * 8, true );}
		bool ReadCompressed(long long &var){if (DoEndianSwap()){unsigned char output[sizeof(long long)]; if (ReadCompressed( ( unsigned char* ) output, sizeof(long long) * 8, true )){ReverseBytes(output, (unsigned char*)&var, sizeof(long long)); return true;} return false;}else return ReadCompressed( ( unsigned char* ) & var, sizeof(long long) * 8, true );}
		bool ReadCompressed(unsigned long long &var){if (DoEndianSwap()){unsigned char output[sizeof(unsigned long long)]; if (ReadCompressed( ( unsigned char* ) output, sizeof(unsigned long long) * 8, true )){ReverseBytes(output, (unsigned char*)&var, sizeof(unsigned long long)); return true;} return false;}else return ReadCompressed( ( unsigned char* ) & var, sizeof(unsigned long long) * 8, true );}
		bool ReadCompressed(float &var){unsigned short compressedFloat; if (Read(compressedFloat)) { var = ((float)compressedFloat / 32767.5f - 1.0f); return true;} return false;}
		bool ReadCompressed(double &var) {unsigned long compressedFloat; if (Read(compressedFloat)) { var = ((double)compressedFloat / 2147483648.0 - 1.0); return true; } return false;}
		bool ReadCompressed(long double &var) {unsigned long compressedFloat; if (Read(compressedFloat)) { var = ((long double)compressedFloat / 2147483648.0 - 1.0); return true; } return false;}
		bool ReadCompressed(SystemAddress &var) {return Read(var);}
		bool ReadCompressed(NetworkID &var) {return Read(var);}

		/// Read any integral type from a bitstream.  If the written value differed from the value compared against in the write function,
		/// var will be updated.  Otherwise it will retain the current value.
		/// the current value will be updated.
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// ReadCompressedDelta is only valid from a previous call to WriteDelta
		/// \param[in] var The value to read
		bool ReadCompressedDelta(bool &var) {return Read(var);}
		bool ReadCompressedDelta(unsigned char &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}
		bool ReadCompressedDelta(char &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}
		bool ReadCompressedDelta(unsigned short &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}
		bool ReadCompressedDelta(short &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}
		bool ReadCompressedDelta(unsigned int &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}
		bool ReadCompressedDelta(int &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}
		bool ReadCompressedDelta(unsigned long &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}
		bool ReadCompressedDelta(long &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}
		bool ReadCompressedDelta(long long &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}
		bool ReadCompressedDelta(unsigned long long &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}
		bool ReadCompressedDelta(float &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}
		bool ReadCompressedDelta(double &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}
		bool ReadCompressedDelta(long double &var){bool dataWritten; bool success; success=Read(dataWritten); if (dataWritten) success=ReadCompressed(var); return success;}

		/// Write an array or casted stream or raw data.  This does NOT do endian swapping.
		/// \param[in] input a byte buffer
		/// \param[in] numberOfBytes the size of \a input in bytes
		void Write( const char* input, const int numberOfBytes );

		/// Write one bitstream to another
		/// \param[in] numberOfBits bits to write
		/// \param bitStream the bitstream to copy from
		void Write( BitStream *bitStream, int numberOfBits );
		void Write( BitStream *bitStream );

		/// Read a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12-24 bytes.  Will further compress y or z axis aligned vectors.
		/// Accurate to 1/32767.5.
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		void WriteNormVector( float x, float y, float z );
		void WriteNormVector( double x, double y, double z ) {WriteNormVector((float)x,(float)y,(float)z);}

		/// Write a vector, using 10 bytes instead of 12.
		/// Loses accuracy to about 3/10ths and only saves 2 bytes, so only use if accuracy is not important.
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		void WriteVector( float x, float y, float z );
		void WriteVector( double x, double y, double z ) {WriteVector((float)x, (float)y, (float)z);}

		/// Write a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.  Slightly lossy.
		/// \param[in] w w
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		void WriteNormQuat( float w, float x, float y, float z);
		void WriteNormQuat( double w, double x, double y, double z) {WriteNormQuat((float)w, (float) x, (float) y, (float) z);}

		/// Write an orthogonal matrix by creating a quaternion, and writing 3 components of the quaternion in 2 bytes each
		/// for 6 bytes instead of 36
		/// Lossy, although the result is renormalized
		void WriteOrthMatrix(
			float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22 )
		{
			WriteOrthMatrix((double)m00,(double)m01,(double)m02,
			(double)m10,(double)m11,(double)m12,
			(double)m20,(double)m21,(double)m22);
		}

		void WriteOrthMatrix(
			double m00, double m01, double m02,
			double m10, double m11, double m12,
			double m20, double m21, double m22 );

		/// Read an array or casted stream of byte. The array
		/// is raw data. There is no automatic endian conversion with this function
		/// \param[in] output The result byte array. It should be larger than @em numberOfBytes.
		/// \param[in] numberOfBytes The number of byte to read
		/// \return true on success false if there is some missing bytes.
		bool Read( char* output, const int numberOfBytes );

		/// Read a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12-24 bytes.  Will further compress y or z axis aligned vectors.
		/// Accurate to 1/32767.5.
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		bool ReadNormVector( float &x, float &y, float &z );
		bool ReadNormVector( double &x, double &y, double &z ) {float fx, fy, fz; bool b = ReadNormVector(fx, fy, fz); x=fx; y=fy; z=fz; return b;}

		/// Read 3 floats or doubles, using 10 bytes, where those float or doubles comprise a vector
		/// Loses accuracy to about 3/10ths and only saves 2 bytes, so only use if accuracy is not important.
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		bool ReadVector( float x, float y, float z );
		bool ReadVector( double &x, double &y, double &z ) {return ReadVector((float)x, (float)y, (float)z);}

		/// Read a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.
		/// \param[in] w w
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		bool ReadNormQuat( float &w, float &x, float &y, float &z){double dw, dx, dy, dz; bool b=ReadNormQuat(dw, dx, dy, dz); w=(float)dw; x=(float)dx; y=(float)dy; z=(float)dz; return b;}
		bool ReadNormQuat( double &w, double &x, double &y, double &z);

		/// Read an orthogonal matrix from a quaternion, reading 3 components of the quaternion in 2 bytes each and extrapolating the 4th.
		/// for 6 bytes instead of 36
		/// Lossy, although the result is renormalized
		bool ReadOrthMatrix(
			float &m00, float &m01, float &m02,
			float &m10, float &m11, float &m12,
			float &m20, float &m21, float &m22 );
		bool ReadOrthMatrix(
			double &m00, double &m01, double &m02,
			double &m10, double &m11, double &m12,
			double &m20, double &m21, double &m22 );

		///Sets the read pointer back to the beginning of your data.
		void ResetReadPointer( void );

		/// Sets the write pointer back to the beginning of your data.
		void ResetWritePointer( void );

		///This is good to call when you are done with the stream to make
		/// sure you didn't leave any data left over void
		void AssertStreamEmpty( void );

		/// RAKNET_DEBUG_PRINTF the bits in the stream.  Great for debugging.
		void PrintBits( void ) const;

		/// Ignore data we don't intend to read
		/// \param[in] numberOfBits The number of bits to ignore
		void IgnoreBits( const int numberOfBits );

		/// Ignore data we don't intend to read
		/// \param[in] numberOfBits The number of bytes to ignore
		void IgnoreBytes( const int numberOfBytes );

		///Move the write pointer to a position on the array.
		/// \param[in] offset the offset from the start of the array.
		/// \attention
		/// Dangerous if you don't know what you are doing!
		/// For efficiency reasons you can only write mid-stream if your data is byte aligned.
		void SetWriteOffset( const int offset );

		/// Returns the length in bits of the stream
		inline int GetNumberOfBitsUsed( void ) const {return GetWriteOffset();}
		inline int GetWriteOffset( void ) const {return numberOfBitsUsed;}

		///Returns the length in bytes of the stream
		inline int GetNumberOfBytesUsed( void ) const {return BITS_TO_BYTES( numberOfBitsUsed );}

		///Returns the number of bits into the stream that we have read
		inline int GetReadOffset( void ) const {return readOffset;}

		// Sets the read bit index
		inline void SetReadOffset( int newReadOffset ) {readOffset=newReadOffset;}

		///Returns the number of bits left in the stream that haven't been read
		inline int GetNumberOfUnreadBits( void ) const {return numberOfBitsUsed - readOffset;}

		/// Makes a copy of the internal data for you \a _data will point to
		/// the stream. Returns the length in bits of the stream. Partial
		/// bytes are left aligned
		/// \param[out] _data The allocated copy of GetData()
		int CopyData( unsigned char** _data ) const;

		/// Set the stream to some initial data.
		/// \internal
		void SetData( unsigned char *input );

		/// Gets the data that BitStream is writing to / reading from
		/// Partial bytes are left aligned.
		/// \return A pointer to the internal state
		inline unsigned char* GetData( void ) const {return data;}

		/// Write numberToWrite bits from the input source Right aligned
		/// data means in the case of a partial byte, the bits are aligned
		/// from the right (bit 0) rather than the left (as in the normal
		/// internal representation) You would set this to true when
		/// writing user data, and false when copying bitstream data, such
		/// as writing one bitstream to another
		/// \param[in] input The data
		/// \param[in] numberOfBitsToWrite The number of bits to write
		/// \param[in] rightAlignedBits if true data will be right aligned
		void WriteBits( const unsigned char* input,	int numberOfBitsToWrite, const bool rightAlignedBits = true );

		/// Align the bitstream to the byte boundary and then write the
		/// specified number of bits.  This is faster than WriteBits but
		/// wastes the bits to do the alignment and requires you to call
		/// ReadAlignedBits at the corresponding read position.
		/// \param[in] input The data
		/// \param[in] numberOfBytesToWrite The size of input.
		void WriteAlignedBytes( void *input, const int numberOfBytesToWrite );

		/// Aligns the bitstream, writes inputLength, and writes input. Won't write beyond maxBytesToWrite
		/// \param[in] input The data
		/// \param[in] inputLength The size of input.
		/// \param[in] maxBytesToWrite Max bytes to write
		void WriteAlignedBytesSafe( void *input, const int inputLength, const int maxBytesToWrite );

		/// Read bits, starting at the next aligned bits. Note that the
		/// modulus 8 starting offset of the sequence must be the same as
		/// was used with WriteBits. This will be a problem with packet
		/// coalescence unless you byte align the coalesced packets.
		/// \param[in] output The byte array larger than @em numberOfBytesToRead
		/// \param[in] numberOfBytesToRead The number of byte to read from the internal state
		/// \return true if there is enough byte.
		bool ReadAlignedBytes( void *output,	const int numberOfBytesToRead );

		/// Reads what was written by WriteAlignedBytesSafe
		/// \param[in] input The data
		/// \param[in] maxBytesToRead Maximum number of bytes to read
		bool ReadAlignedBytesSafe( void *input, int &inputLength, const int maxBytesToRead );

		/// Same as ReadAlignedBytesSafe() but allocates the memory for you using new, rather than assuming it is safe to write to
		/// \param[in] input input will be deleted if it is not a pointer to 0
		bool ReadAlignedBytesSafeAlloc( char **input, int &inputLength, const int maxBytesToRead );

		/// Align the next write and/or read to a byte boundary.  This can
		/// be used to 'waste' bits to byte align for efficiency reasons It
		/// can also be used to force coalesced bitstreams to start on byte
		/// boundaries so so WriteAlignedBits and ReadAlignedBits both
		/// calculate the same offset when aligning.
		void AlignWriteToByteBoundary( void );

		/// Align the next write and/or read to a byte boundary.  This can
		/// be used to 'waste' bits to byte align for efficiency reasons It
		/// can also be used to force coalesced bitstreams to start on byte
		/// boundaries so so WriteAlignedBits and ReadAlignedBits both
		/// calculate the same offset when aligning.
		void AlignReadToByteBoundary( void );

		/// Read \a numberOfBitsToRead bits to the output source
		/// alignBitsToRight should be set to true to convert internal
		/// bitstream data to userdata. It should be false if you used
		/// WriteBits with rightAlignedBits false
		/// \param[in] output The resulting bits array
		/// \param[in] numberOfBitsToRead The number of bits to read
		/// \param[in] alignBitsToRight if true bits will be right aligned.
		/// \return true if there is enough bits to read
		bool ReadBits( unsigned char *output, int numberOfBitsToRead,	const bool alignBitsToRight = true );

		/// Write a 0
		void Write0( void );

		/// Write a 1
		void Write1( void );

		/// Reads 1 bit and returns true if that bit is 1 and false if it is 0
		bool ReadBit( void );

		/// If we used the constructor version with copy data off, this
		/// *makes sure it is set to on and the data pointed to is copied.
		void AssertCopyData( void );

		/// Use this if you pass a pointer copy to the constructor
		/// *(_copyData==false) and want to overallocate to prevent
		/// *reallocation
		void SetNumberOfBitsAllocated( const unsigned int lengthInBits );

		/// Reallocates (if necessary) in preparation of writing numberOfBitsToWrite
		void AddBitsAndReallocate( const int numberOfBitsToWrite );

		/// \internal
		/// \return How many bits have been allocated internally
		unsigned int GetNumberOfBitsAllocated(void) const;

		static bool DoEndianSwap(void);
		static bool IsBigEndian(void);
		static bool IsNetworkOrder(void);
		static void ReverseBytes(unsigned char *input, unsigned char *output, int length);
		static void ReverseBytesInPlace(unsigned char *data, int length);

	private:

		BitStream( const BitStream &invalid) {
			#ifdef _MSC_VER
			#pragma warning(disable:4100)
			// warning C4100: 'invalid' : unreferenced formal parameter
			#endif

		}

		/// Assume the input source points to a native type, compress and write it.
		void WriteCompressed( const unsigned char* input,	const int size, const bool unsignedData );

		/// Assume the input source points to a compressed native type. Decompress and read it.
		bool ReadCompressed( unsigned char* output,	const int size, const bool unsignedData );


		int numberOfBitsUsed;

		int numberOfBitsAllocated;

		int readOffset;

		unsigned char *data;

		/// true if the internal buffer is copy of the data passed to the constructor
		bool copyData;

		/// BitStreams that use less than BITSTREAM_STACK_ALLOCATION_SIZE use the stack, rather than the heap to store data.  It switches over if BITSTREAM_STACK_ALLOCATION_SIZE is exceeded
		unsigned char stackData[BITSTREAM_STACK_ALLOCATION_SIZE];
	};

		inline bool BitStream::SerializeBits(bool writeToBitstream, unsigned char* input, int numberOfBitsToSerialize, const bool rightAlignedBits )
		{
			if (writeToBitstream)
				WriteBits(input,numberOfBitsToSerialize,rightAlignedBits);
			else
				return ReadBits(input,numberOfBitsToSerialize,rightAlignedBits);
			return true;
		}


}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif // VC6

#endif