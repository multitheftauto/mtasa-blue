/// \file
/// \brief This class allows you to write and read native types as a string of bits.  BitStream is used extensively throughout RakNet and is designed to be used by users as well.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.



#if defined(_MSC_VER) && _MSC_VER < 1299 // VC6 doesn't support template specialization
#include "BitStream_NoTemplate.h"
#else

#ifndef __BITSTREAM_H
#define __BITSTREAM_H

#include "RakMemoryOverride.h"
#include "RakNetDefines.h"
#include "Export.h"
#include "RakNetTypes.h"
#include "RakString.h"
#include "RakAssert.h"
#include <math.h>
#include <float.h>

#ifdef _MSC_VER
#pragma warning( push )
#endif

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
		BitStream( const unsigned int initialBytesToAllocate );

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
		BitStream( unsigned char* _data, const unsigned int lengthInBytes, bool _copyData );

		/// Destructor
		~BitStream();

		/// Resets the bitstream for reuse.
		void Reset( void );

		/// Bidirectional serialize/deserialize any integral type to/from a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] var The value to write
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType>
			bool Serialize(bool writeToBitstream, templateType &var);

		/// Bidirectional serialize/deserialize any integral type to/from a bitstream.  If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against.  Only used if \a writeToBitstream is true.
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType>
			bool SerializeDelta(bool writeToBitstream, templateType &currentValue, templateType lastValue);

		/// Bidirectional version of SerializeDelta when you don't know what the last value is, or there is no last value.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] currentValue The current value to write
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType>
			bool SerializeDelta(bool writeToBitstream, templateType &currentValue);

		/// Bidirectional serialize/deserialize any integral type to/from a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] var The value to write
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType>
			bool SerializeCompressed(bool writeToBitstream, templateType &var);

		/// Bidirectional serialize/deserialize any integral type to/from a bitstream.  If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against.  Only used if \a writeToBitstream is true.
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType>
			bool SerializeCompressedDelta(bool writeToBitstream, templateType &currentValue, templateType lastValue);

		/// Save as SerializeCompressedDelta(templateType &currentValue, templateType lastValue) when we have an unknown second parameter
		template <class templateType>
			bool SerializeCompressedDelta(bool writeToBitstream, templateType &currentValue);

		/// Bidirectional serialize/deserialize an array or casted stream or raw data.  This does NOT do endian swapping.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] input a byte buffer
		/// \param[in] numberOfBytes the size of \a input in bytes
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		bool Serialize(bool writeToBitstream,  char* input, const unsigned int numberOfBytes );

		/// Bidirectional serialize/deserialize a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12-24 bytes.  Will further compress y or z axis aligned vectors.
		/// Accurate to 1/32767.5.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType> // templateType for this function must be a float or double
			bool SerializeNormVector(bool writeToBitstream,  templateType &x, templateType &y, templateType &z );

		/// Bidirectional serialize/deserialize a vector, using 10 bytes instead of 12.
		/// Loses accuracy to about 3/10ths and only saves 2 bytes, so only use if accuracy is not important.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType> // templateType for this function must be a float or double
			bool SerializeVector(bool writeToBitstream,  templateType &x, templateType &y, templateType &z );

		/// Bidirectional serialize/deserialize a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.  Slightly lossy.
		/// \param[in] writeToBitstream true to write from your data to this bitstream.  False to read from this bitstream and write to your data
		/// \param[in] w w
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		/// \return true if \a writeToBitstream is true.  true if \a writeToBitstream is false and the read was successful.  false if \a writeToBitstream is false and the read was not successful.
		template <class templateType> // templateType for this function must be a float or double
			bool SerializeNormQuat(bool writeToBitstream,  templateType &w, templateType &x, templateType &y, templateType &z);

		/// Bidirectional serialize/deserialize an orthogonal matrix by creating a quaternion, and writing 3 components of the quaternion in 2 bytes each
		/// for 6 bytes instead of 36
		/// Lossy, although the result is renormalized
		template <class templateType> // templateType for this function must be a float or double
			bool SerializeOrthMatrix(
			bool writeToBitstream,
			templateType &m00, templateType &m01, templateType &m02,
			templateType &m10, templateType &m11, templateType &m12,
			templateType &m20, templateType &m21, templateType &m22 );

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
		bool SerializeBits(bool writeToBitstream, unsigned char* input, const BitSize_t numberOfBitsToSerialize, const bool rightAlignedBits = true );

		/// Write any integral type to a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// \param[in] var The value to write
		template <class templateType>
			void Write(templateType var);

		/// Write the dereferenced pointer to any integral type to a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// \param[in] var The value to write
		template <class templateType>
			void WritePtr(templateType *var);

		/// Write any integral type to a bitstream.  If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against
		template <class templateType>
			void WriteDelta(templateType currentValue, templateType lastValue);

		/// WriteDelta when you don't know what the last value is, or there is no last value.
		/// \param[in] currentValue The current value to write
		template <class templateType>
			void WriteDelta(templateType currentValue);

		/// Write any integral type to a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
		/// \param[in] var The value to write
		template <class templateType>
			void WriteCompressed(templateType var);

		/// Write any integral type to a bitstream.  If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against
		template <class templateType>
			void WriteCompressedDelta(templateType currentValue, templateType lastValue);

		/// Save as WriteCompressedDelta(templateType currentValue, templateType lastValue) when we have an unknown second parameter
		template <class templateType>
			void WriteCompressedDelta(templateType currentValue);

		/// Read any integral type from a bitstream.  Define __BITSTREAM_NATIVE_END if you need endian swapping.
		/// \param[in] var The value to read
		template <class templateType>
			bool Read(templateType &var);

		/// Read into a pointer to any integral type from a bitstream.  Define __BITSTREAM_NATIVE_END if you need endian swapping.
		/// \param[in] var The value to read
		template <class templateType>
			bool ReadPtr(templateType *var);

		/// Read any integral type from a bitstream.  If the written value differed from the value compared against in the write function,
		/// var will be updated.  Otherwise it will retain the current value.
		/// ReadDelta is only valid from a previous call to WriteDelta
		/// \param[in] var The value to read
		template <class templateType>
			bool ReadDelta(templateType &var);

		/// Read any integral type from a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// \param[in] var The value to read
		template <class templateType>
			bool ReadCompressed(templateType &var);

		/// Read any integral type from a bitstream.  If the written value differed from the value compared against in the write function,
		/// var will be updated.  Otherwise it will retain the current value.
		/// the current value will be updated.
		/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
		/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
		/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
		/// ReadCompressedDelta is only valid from a previous call to WriteDelta
		/// \param[in] var The value to read
		template <class templateType>
			bool ReadCompressedDelta(templateType &var);

		/// Read one bitstream to another
		/// \param[in] numberOfBits bits to read
		/// \param bitStream the bitstream to read into from
		bool Read( BitStream *bitStream, BitSize_t numberOfBits );
		bool Read( BitStream *bitStream );
		bool Read( BitStream &bitStream, BitSize_t numberOfBits );
		bool Read( BitStream &bitStream );

		/// Write an array or casted stream or raw data.  This does NOT do endian swapping.
		/// \param[in] input a byte buffer
		/// \param[in] numberOfBytes the size of \a input in bytes
		void Write( const char* input, const unsigned int numberOfBytes );

		/// Write one bitstream to another
		/// \param[in] numberOfBits bits to write
		/// \param bitStream the bitstream to copy from
		void Write( BitStream *bitStream, BitSize_t numberOfBits );
		void Write( BitStream *bitStream );
		void Write( BitStream &bitStream, BitSize_t numberOfBits );
		void Write( BitStream &bitStream );

		/// Read a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12-24 bytes.  Will further compress y or z axis aligned vectors.
		/// Accurate to 1/32767.5.
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		template <class templateType> // templateType for this function must be a float or double
		void WriteNormVector( templateType x, templateType y, templateType z );

		/// Write a vector, using 10 bytes instead of 12.
		/// Loses accuracy to about 3/10ths and only saves 2 bytes, so only use if accuracy is not important.
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		template <class templateType> // templateType for this function must be a float or double
		void WriteVector( templateType x, templateType y, templateType z );

		/// Write a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.  Slightly lossy.
		/// \param[in] w w
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		template <class templateType> // templateType for this function must be a float or double
		void WriteNormQuat( templateType w, templateType x, templateType y, templateType z);

		/// Write an orthogonal matrix by creating a quaternion, and writing 3 components of the quaternion in 2 bytes each
		/// for 6 bytes instead of 36
		/// Lossy, although the result is renormalized
		template <class templateType> // templateType for this function must be a float or double
		void WriteOrthMatrix(
			templateType m00, templateType m01, templateType m02,
			templateType m10, templateType m11, templateType m12,
			templateType m20, templateType m21, templateType m22 );

		/// Read an array or casted stream of byte. The array
		/// is raw data. There is no automatic endian conversion with this function
		/// \param[in] output The result byte array. It should be larger than @em numberOfBytes.
		/// \param[in] numberOfBytes The number of byte to read
		/// \return true on success false if there is some missing bytes.
		bool Read( char* output, const unsigned int numberOfBytes );

		/// Read a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12-24 bytes.  Will further compress y or z axis aligned vectors.
		/// Accurate to 1/32767.5.
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		template <class templateType> // templateType for this function must be a float or double
		bool ReadNormVector( templateType &x, templateType &y, templateType &z );

		/// Read 3 floats or doubles, using 10 bytes, where those float or doubles comprise a vector
		/// Loses accuracy to about 3/10ths and only saves 2 bytes, so only use if accuracy is not important.
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		template <class templateType> // templateType for this function must be a float or double
		bool ReadVector( templateType &x, templateType &y, templateType &z );

		/// Read a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.
		/// \param[in] w w
		/// \param[in] x x
		/// \param[in] y y
		/// \param[in] z z
		template <class templateType> // templateType for this function must be a float or double
		bool ReadNormQuat( templateType &w, templateType &x, templateType &y, templateType &z);

		/// Read an orthogonal matrix from a quaternion, reading 3 components of the quaternion in 2 bytes each and extrapolatig the 4th.
		/// for 6 bytes instead of 36
		/// Lossy, although the result is renormalized
		template <class templateType> // templateType for this function must be a float or double
		bool ReadOrthMatrix(
			templateType &m00, templateType &m01, templateType &m02,
			templateType &m10, templateType &m11, templateType &m12,
			templateType &m20, templateType &m21, templateType &m22 );

		///Sets the read pointer back to the beginning of your data.
		void ResetReadPointer( void );

		/// Sets the write pointer back to the beginning of your data.
		void ResetWritePointer( void );

		///This is good to call when you are done with the stream to make
		/// sure you didn't leave any data left over void
		void AssertStreamEmpty( void );

		/// RAKNET_DEBUG_PRINTF the bits in the stream.  Great for debugging.
		void PrintBits( char *out ) const;
		void PrintBits( void ) const;
		void PrintHex( char *out ) const;
		void PrintHex( void ) const;

		/// Ignore data we don't intend to read
		/// \param[in] numberOfBits The number of bits to ignore
		void IgnoreBits( const BitSize_t numberOfBits );

		/// Ignore data we don't intend to read
		/// \param[in] numberOfBits The number of bytes to ignore
		void IgnoreBytes( const unsigned int numberOfBytes );

		///Move the write pointer to a position on the array.
		/// \param[in] offset the offset from the start of the array.
		/// \attention
		/// Dangerous if you don't know what you are doing!
		/// For efficiency reasons you can only write mid-stream if your data is byte aligned.
		void SetWriteOffset( const BitSize_t offset );

		/// Returns the length in bits of the stream
		inline BitSize_t GetNumberOfBitsUsed( void ) const {return GetWriteOffset();}
		inline BitSize_t GetWriteOffset( void ) const {return numberOfBitsUsed;}

		///Returns the length in bytes of the stream
		inline BitSize_t GetNumberOfBytesUsed( void ) const {return BITS_TO_BYTES( numberOfBitsUsed );}

		///Returns the number of bits into the stream that we have read
		inline BitSize_t GetReadOffset( void ) const {return readOffset;}

		// Sets the read bit index
		void SetReadOffset( const BitSize_t newReadOffset ) {readOffset=newReadOffset;}

		///Returns the number of bits left in the stream that haven't been read
		inline BitSize_t GetNumberOfUnreadBits( void ) const {return numberOfBitsUsed - readOffset;}

		/// Makes a copy of the internal data for you \a _data will point to
		/// the stream. Returns the length in bits of the stream. Partial
		/// bytes are left aligned
		/// \param[out] _data The allocated copy of GetData()
		BitSize_t CopyData( unsigned char** _data ) const;

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
		void WriteBits( const unsigned char* input, BitSize_t numberOfBitsToWrite, const bool rightAlignedBits = true );

		/// Align the bitstream to the byte boundary and then write the
		/// specified number of bits.  This is faster than WriteBits but
		/// wastes the bits to do the alignment and requires you to call
		/// ReadAlignedBits at the corresponding read position.
		/// \param[in] input The data
		/// \param[in] numberOfBytesToWrite The size of input.
		void WriteAlignedBytes( const unsigned char *input, const unsigned int numberOfBytesToWrite );

		/// Aligns the bitstream, writes inputLength, and writes input. Won't write beyond maxBytesToWrite
		/// \param[in] input The data
		/// \param[in] inputLength The size of input.
		/// \param[in] maxBytesToWrite Max bytes to write
		void WriteAlignedBytesSafe( const char *input, const unsigned int inputLength, const unsigned int maxBytesToWrite );

		/// Read bits, starting at the next aligned bits. Note that the
		/// modulus 8 starting offset of the sequence must be the same as
		/// was used with WriteBits. This will be a problem with packet
		/// coalescence unless you byte align the coalesced packets.
		/// \param[in] output The byte array larger than @em numberOfBytesToRead
		/// \param[in] numberOfBytesToRead The number of byte to read from the internal state
		/// \return true if there is enough byte.
		bool ReadAlignedBytes( unsigned char *output, const unsigned int numberOfBytesToRead );

		/// Reads what was written by WriteAlignedBytesSafe
		/// \param[in] input The data
		/// \param[in] maxBytesToRead Maximum number of bytes to read
		bool ReadAlignedBytesSafe( char *input, int &inputLength, const int maxBytesToRead );
		bool ReadAlignedBytesSafe( char *input, unsigned int &inputLength, const unsigned int maxBytesToRead );

		/// Same as ReadAlignedBytesSafe() but allocates the memory for you using new, rather than assuming it is safe to write to
		/// \param[in] input input will be deleted if it is not a pointer to 0
		bool ReadAlignedBytesSafeAlloc( char **input, int &inputLength, const unsigned int maxBytesToRead );
		bool ReadAlignedBytesSafeAlloc( char **input, unsigned int &inputLength, const unsigned int maxBytesToRead );

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
		bool ReadBits( unsigned char *output, BitSize_t numberOfBitsToRead, const bool alignBitsToRight = true );

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
		void SetNumberOfBitsAllocated( const BitSize_t lengthInBits );

		/// Reallocates (if necessary) in preparation of writing numberOfBitsToWrite
		void AddBitsAndReallocate( const BitSize_t numberOfBitsToWrite );

		/// \internal
		/// \return How many bits have been allocated internally
		BitSize_t GetNumberOfBitsAllocated(void) const;


		// Read strings, non reference
		bool Read(char *var);
		bool Read(unsigned char *var);


		/// ---- Member function template specialization declarations ----
		// Used for VC7
#if defined(_MSC_VER) && _MSC_VER == 1300
		/// Write a bool to a bitstream
		/// \param[in] var The value to write
		template <>
			void Write(bool var);

		/// Write a systemAddress to a bitstream
		/// \param[in] var The value to write
		template <>
			void Write(SystemAddress var);

		/// Write a RakNetGUID to a bitsteam
		/// \param[in] var The value to write
		template <>
			void Write(RakNetGuid var);

		/// Write an networkID to a bitstream
		/// \param[in] var The value to write
		template <>
			void Write(NetworkID var);

		/// Write a string to a bitstream
		/// \param[in] var The value to write
		template <>
			void Write(const char* var);
		template <>
			void Write(const unsigned char* var);
		template <>
			void Write(char* var);
		template <>
			void Write(unsigned char* var);
		template <>
			void Write(RakString var);

		/// Write a systemAddress.  If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against
		template <>
			void WriteDelta(SystemAddress currentValue, SystemAddress lastValue);

		template <>
			void WriteDelta(RakNetGUID currentValue, RakNetGUID lastValue);

		/// Write an networkID.  If the current value is different from the last value
		/// the current value will be written.  Otherwise, a single bit will be written
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against
		template <>
			void WriteDelta(NetworkID currentValue, NetworkID lastValue);

		/// Write a bool delta.  Same thing as just calling Write
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against
		template <>
			void WriteDelta(bool currentValue, bool lastValue);

		template <>
			void WriteCompressed(SystemAddress var);

		template <>
			void WriteCompressed(RakNetGUID var);

		template <>
			void WriteCompressed(NetworkID var);

		template <>
			void WriteCompressed(bool var);

		/// For values between -1 and 1
		template <>
			void WriteCompressed(float var);

		/// For values between -1 and 1
		template <>
			void WriteCompressed(double var);

		/// Compressed string
		template <>
			void WriteCompressed(const char* var);
		template <>
			void WriteCompressed(const unsigned char* var);
		template <>
			void WriteCompressed(char* var);
		template <>
			void WriteCompressed(unsigned char* var);
		template <>
			void WriteCompressed(RakString var);

		/// Write a bool delta.  Same thing as just calling Write
		/// \param[in] currentValue The current value to write
		/// \param[in] lastValue The last value to compare against
		template <>
			void WriteCompressedDelta(bool currentValue, bool lastValue);

		/// Save as WriteCompressedDelta(bool currentValue, templateType lastValue) when we have an unknown second bool
		template <>
			void WriteCompressedDelta(bool currentValue);

		/// Read a bool from a bitstream
		/// \param[in] var The value to read
		template <>
			bool Read(bool &var);

		/// Read a systemAddress from a bitstream
		/// \param[in] var The value to read
		template <>
			bool Read(SystemAddress &var);

		template <>
			bool Read(RakNetGUID &var);

		/// Read an NetworkID from a bitstream
		/// \param[in] var The value to read
		template <>
			bool Read(NetworkID &var);

		/// Read a String from a bitstream
		/// \param[in] var The value to read
		template <>
			bool Read(char *&var);
		template <>
			bool Read(unsigned char *&var);
		template <>
			bool Read(RakString &var);

		/// Read a bool from a bitstream
		/// \param[in] var The value to read
		template <>
			bool ReadDelta(bool &var);

		template <>
			bool ReadCompressed(SystemAddress &var);

		template <>
			bool ReadCompressed(RakNetGUID &var);

		template <>
			bool ReadCompressed(NetworkID &var);

		template <>
			bool ReadCompressed(bool &var);

		template <>
			bool ReadCompressed(float &var);

		/// For values between -1 and 1
		template <>
		bool ReadCompressed(double &var);

		template <>
			bool ReadCompressed(char* &var);
		template <>
			bool ReadCompressed(unsigned char *&var);
		template <>
			bool ReadCompressed(RakString &var);

		/// Read a bool from a bitstream
		/// \param[in] var The value to read
		template <>
			bool ReadCompressedDelta(bool &var);
#endif

		static bool DoEndianSwap(void);
		static bool IsBigEndian(void);
		static bool IsNetworkOrder(void);
		static void ReverseBytes(unsigned char *input, unsigned char *output, const unsigned int length);
		static void ReverseBytesInPlace(unsigned char *data,const unsigned int length);

	private:

		BitStream( const BitStream &invalid) {
			(void) invalid;
			RakAssert(0);
		}

		/// Assume the input source points to a native type, compress and write it.
		void WriteCompressed( const unsigned char* input, const unsigned int size, const bool unsignedData );

		/// Assume the input source points to a compressed native type. Decompress and read it.
		bool ReadCompressed( unsigned char* output,	const unsigned int size, const bool unsignedData );


		BitSize_t numberOfBitsUsed;

		BitSize_t numberOfBitsAllocated;

		BitSize_t readOffset;

		unsigned char *data;

		/// true if the internal buffer is copy of the data passed to the constructor
		bool copyData;

		/// BitStreams that use less than BITSTREAM_STACK_ALLOCATION_SIZE use the stack, rather than the heap to store data.  It switches over if BITSTREAM_STACK_ALLOCATION_SIZE is exceeded
		unsigned char stackData[BITSTREAM_STACK_ALLOCATION_SIZE];
	};

		template <class templateType>
		inline bool BitStream::Serialize(bool writeToBitstream, templateType &var)
		{
			if (writeToBitstream)
				Write(var);
			else
				return Read(var);
			return true;
		}

		template <class templateType>
		inline bool BitStream::SerializeDelta(bool writeToBitstream, templateType &currentValue, templateType lastValue)
		{
			if (writeToBitstream)
				WriteDelta(currentValue, lastValue);
			else
				return ReadDelta(currentValue);
			return true;
		}

		template <class templateType>
		inline bool BitStream::SerializeDelta(bool writeToBitstream, templateType &currentValue)
		{
			if (writeToBitstream)
				WriteDelta(currentValue);
			else
				return ReadDelta(currentValue);
			return true;
		}

		template <class templateType>
		inline bool BitStream::SerializeCompressed(bool writeToBitstream, templateType &var)
		{
			if (writeToBitstream)
				WriteCompressed(var);
			else
				return ReadCompressed(var);
			return true;
		}

		template <class templateType>
		inline bool BitStream::SerializeCompressedDelta(bool writeToBitstream, templateType &currentValue, templateType lastValue)
		{
			if (writeToBitstream)
				WriteCompressedDelta(currentValue,lastValue);
			else
				return ReadCompressedDelta(currentValue);
			return true;
		}

		template <class templateType>
		inline bool BitStream::SerializeCompressedDelta(bool writeToBitstream, templateType &currentValue)
		{
			if (writeToBitstream)
				WriteCompressedDelta(currentValue);
			else
				return ReadCompressedDelta(currentValue);
			return true;
		}

		inline bool BitStream::Serialize(bool writeToBitstream, char* input, const unsigned int numberOfBytes )
		{
			if (writeToBitstream)
				Write(input, numberOfBytes);
			else
				return Read(input, numberOfBytes);
			return true;
		}

		template <class templateType>
		inline bool BitStream::SerializeNormVector(bool writeToBitstream, templateType &x, templateType &y, templateType &z )
		{
			if (writeToBitstream)
				WriteNormVector(x,y,z);
			else
				return ReadNormVector(x,y,z);
			return true;
		}

		template <class templateType>
		inline bool BitStream::SerializeVector(bool writeToBitstream,  templateType &x, templateType &y, templateType &z )
		{
			if (writeToBitstream)
				WriteVector(x,y,z);
			else
				return ReadVector(x,y,z);
			return true;
		}

		template <class templateType>
		inline bool BitStream::SerializeNormQuat(bool writeToBitstream,  templateType &w, templateType &x, templateType &y, templateType &z)
		{
			if (writeToBitstream)
				WriteNormQuat(w,x,y,z);
			else
				return ReadNormQuat(w,x,y,z);
			return true;
		}

		template <class templateType>
		inline bool BitStream::SerializeOrthMatrix(
		bool writeToBitstream,
		templateType &m00, templateType &m01, templateType &m02,
		templateType &m10, templateType &m11, templateType &m12,
		templateType &m20, templateType &m21, templateType &m22 )
		{
			if (writeToBitstream)
				WriteOrthMatrix(m00,m01,m02,m10,m11,m12,m20,m21,m22);
			else
				return ReadOrthMatrix(m00,m01,m02,m10,m11,m12,m20,m21,m22);
			return true;
		}

		inline bool BitStream::SerializeBits(bool writeToBitstream, unsigned char* input, const BitSize_t numberOfBitsToSerialize, const bool rightAlignedBits )
		{
			if (writeToBitstream)
				WriteBits(input,numberOfBitsToSerialize,rightAlignedBits);
			else
				return ReadBits(input,numberOfBitsToSerialize,rightAlignedBits);
			return true;
		}

	template <class templateType>
		inline void BitStream::Write(templateType var)
	{
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
		if (sizeof(var)==1)
			WriteBits( ( unsigned char* ) & var, sizeof( templateType ) * 8, true );
		else
		{
#ifndef __BITSTREAM_NATIVE_END
			if (DoEndianSwap())
			{
				unsigned char output[sizeof(templateType)];
				ReverseBytes((unsigned char*)&var, output, sizeof(templateType));
				WriteBits( ( unsigned char* ) output, sizeof(templateType) * 8, true );
			}
			else
#endif
				WriteBits( ( unsigned char* ) & var, sizeof(templateType) * 8, true );
		}
	}

	template <class templateType>
	inline void BitStream::WritePtr(templateType *var)
	{
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
		if (sizeof(var)==1)
			WriteBits( ( unsigned char* ) var, sizeof( templateType ) * 8, true );
		else
		{
#ifndef __BITSTREAM_NATIVE_END
			if (DoEndianSwap())
			{
				unsigned char output[sizeof(templateType)];
				ReverseBytes((unsigned char*) var, output, sizeof(templateType));
				WriteBits( ( unsigned char* ) output, sizeof(templateType) * 8, true );
			}
			else
#endif
				WriteBits( ( unsigned char* ) var, sizeof(templateType) * 8, true );
		}
	}

	/// Write a bool to a bitstream
	/// \param[in] var The value to write
	template <>
		inline void BitStream::Write(bool var)
	{
		if ( var )
			Write1();
		else
			Write0();
	}

	/// Write a systemAddress to a bitstream
	/// \param[in] var The value to write
	template <>
		inline void BitStream::Write(SystemAddress var)
	{
		// Hide the address so routers don't modify it
		var.binaryAddress=~var.binaryAddress;
		// Don't endian swap the address
		WriteBits((unsigned char*)&var.binaryAddress, sizeof(var.binaryAddress)*8, true);
		Write(var.port);
	}

	template <>
		inline void BitStream::Write(RakNetGUID var)
		{
			for (unsigned int i=0; i < sizeof(var.g) / sizeof(var.g[0]); i++)
				Write(var.g[i]);
		}

	/// Write an networkID to a bitstream
	/// \param[in] var The value to write
	template <>
		inline void BitStream::Write(NetworkID var)
	{
#if defined (NETWORK_ID_SUPPORTS_PEER_TO_PEER)
		RakAssert(NetworkID::IsPeerToPeerMode());
//		if (NetworkID::IsPeerToPeerMode()) // Use the function rather than directly access the member or DLL users will get an undefined external error
		{
			if (var.guid!=UNASSIGNED_RAKNET_GUID)
			{
				Write(true);
				Write(var.guid);
			}
			else
				Write(false);
			if (var.systemAddress!=UNASSIGNED_SYSTEM_ADDRESS)
			{
				Write(true);
				Write(var.systemAddress);
			}
			else
				Write(false);
		}
#endif
		/*
		Write(var.guid);
		Write(var.systemAddress);
		*/
		Write(var.localSystemAddress);
	}

	/// Write a string to a bitstream
	/// \param[in] var The value to write
	template <>
		inline void BitStream::Write(RakString var)
	{
		var.Serialize(this);
	}
	template <>
		inline void BitStream::Write(const char * var)
	{
		RakString::Serialize(var, this);
	}
	template <>
		inline void BitStream::Write(const unsigned char * var)
	{
		Write((const char*)var);
	}
	template <>
		inline void BitStream::Write(char * var)
	{
		Write((const char*)var);
	}
	template <>
		inline void BitStream::Write(unsigned char * var)
	{
		Write((const char*)var);
	}

	/// Write any integral type to a bitstream.  If the current value is different from the last value
	/// the current value will be written.  Otherwise, a single bit will be written
	/// \param[in] currentValue The current value to write
	/// \param[in] lastValue The last value to compare against
	template <class templateType>
		inline void BitStream::WriteDelta(templateType currentValue, templateType lastValue)
	{
		if (currentValue==lastValue)
		{
			Write(false);
		}
		else
		{
			Write(true);
			Write(currentValue);
		}
	}

	/// Write a systemAddress.  If the current value is different from the last value
	/// the current value will be written.  Otherwise, a single bit will be written
	/// \param[in] currentValue The current value to write
	/// \param[in] lastValue The last value to compare against
	template <>
		inline void BitStream::WriteDelta(SystemAddress currentValue, SystemAddress lastValue)
	{
		if (currentValue==lastValue)
		{
			Write(false);
		}
		else
		{
			Write(true);
			Write(currentValue);
		}
	}

	template <>
		inline void BitStream::WriteDelta(RakNetGUID currentValue, RakNetGUID lastValue)
		{
			if (currentValue==lastValue)
			{
				Write(false);
			}
			else
			{
				Write(true);
				Write(currentValue);
			}
		}

	/// Write a systemAddress.  If the current value is different from the last value
	/// the current value will be written.  Otherwise, a single bit will be written
	/// \param[in] currentValue The current value to write
	/// \param[in] lastValue The last value to compare against
	template <>
		inline void BitStream::WriteDelta(NetworkID currentValue, NetworkID lastValue)
	{
		if (currentValue==lastValue)
		{
			Write(false);
		}
		else
		{
			Write(true);
			Write(currentValue);
		}
	}

	/// Write a bool delta.  Same thing as just calling Write
	/// \param[in] currentValue The current value to write
	/// \param[in] lastValue The last value to compare against
	template <>
		inline void BitStream::WriteDelta(bool currentValue, bool lastValue)
	{
		(void) lastValue;

		Write(currentValue);
	}

	/// WriteDelta when you don't know what the last value is, or there is no last value.
	/// \param[in] currentValue The current value to write
	template <class templateType>
		inline void BitStream::WriteDelta(templateType currentValue)
	{
		Write(true);
		Write(currentValue);
	}

	/// Write any integral type to a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
	/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
	/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
	/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
	/// \param[in] var The value to write
	template <class templateType>
		inline void BitStream::WriteCompressed(templateType var)
	{
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
		if (sizeof(var)==1)
			WriteCompressed( ( unsigned char* ) & var, sizeof( templateType ) * 8, true );
		else
		{
#ifndef __BITSTREAM_NATIVE_END
#ifdef _MSC_VER
#pragma warning(disable:4244)   // '=' : conversion from 'unsigned long' to 'unsigned short', possible loss of data
#endif

			if (DoEndianSwap())
			{
				unsigned char output[sizeof(templateType)];
				ReverseBytes((unsigned char*)&var, output, sizeof(templateType));
				WriteCompressed( ( unsigned char* ) output, sizeof(templateType) * 8, true );
			}
			else
#endif
				WriteCompressed( ( unsigned char* ) & var, sizeof(templateType) * 8, true );
		}
	}

	template <>
		inline void BitStream::WriteCompressed(SystemAddress var)
	{
		Write(var);
	}

	template <>
	inline void BitStream::WriteCompressed(RakNetGUID var)
	{
		Write(var);
	}

	template <>
		inline void BitStream::WriteCompressed(NetworkID var)
	{
		Write(var);
	}

	template <>
		inline void BitStream::WriteCompressed(bool var)
	{
		Write(var);
	}

	/// For values between -1 and 1
	template <>
		inline void BitStream::WriteCompressed(float var)
	{
		RakAssert(var > -1.01f && var < 1.01f);
		if (var < -1.0f)
			var=-1.0f;
		if (var > 1.0f)
			var=1.0f;
		Write((unsigned short)((var+1.0f)*32767.5f));
	}

	/// For values between -1 and 1
	template <>
		inline void BitStream::WriteCompressed(double var)
	{
		RakAssert(var > -1.01 && var < 1.01);
		if (var < -1.0f)
			var=-1.0f;
		if (var > 1.0f)
			var=1.0f;
#ifdef _DEBUG
		RakAssert(sizeof(unsigned long)==4);
#endif
		Write((unsigned long)((var+1.0)*2147483648.0));
	}

	/// Compress the string
	template <>
		inline void BitStream::WriteCompressed(RakString var)
	{
		var.SerializeCompressed(this,0,false);
	}
	template <>
		inline void BitStream::WriteCompressed(const char * var)
	{
		RakString::SerializeCompressed(var,this,0,false);
	}
	template <>
		inline void BitStream::WriteCompressed(const unsigned char * var)
	{
		WriteCompressed((const char*) var);
	}
	template <>
		inline void BitStream::WriteCompressed(char * var)
	{
		WriteCompressed((const char*) var);
	}
	template <>
		inline void BitStream::WriteCompressed(unsigned char * var)
	{
		WriteCompressed((const char*) var);
	}

	/// Write any integral type to a bitstream.  If the current value is different from the last value
	/// the current value will be written.  Otherwise, a single bit will be written
	/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
	/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
	/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
	/// \param[in] currentValue The current value to write
	/// \param[in] lastValue The last value to compare against
	template <class templateType>
		inline void BitStream::WriteCompressedDelta(templateType currentValue, templateType lastValue)
	{
		if (currentValue==lastValue)
		{
			Write(false);
		}
		else
		{
			Write(true);
			WriteCompressed(currentValue);
		}
	}

	/// Write a bool delta.  Same thing as just calling Write
	/// \param[in] currentValue The current value to write
	/// \param[in] lastValue The last value to compare against
	template <>
		inline void BitStream::WriteCompressedDelta(bool currentValue, bool lastValue)
	{
		(void) lastValue;

		Write(currentValue);
	}

	/// Save as WriteCompressedDelta(templateType currentValue, templateType lastValue) when we have an unknown second parameter
	template <class templateType>
		inline void BitStream::WriteCompressedDelta(templateType currentValue)
	{
		Write(true);
		WriteCompressed(currentValue);
	}

	/// Save as WriteCompressedDelta(bool currentValue, templateType lastValue) when we have an unknown second bool
	template <>
		inline void BitStream::WriteCompressedDelta(bool currentValue)
	{
		Write(currentValue);
	}

	/// Read any integral type from a bitstream.  Define __BITSTREAM_NATIVE_END if you need endian swapping.
	/// \param[in] var The value to read
	template <class templateType>
		inline bool BitStream::Read(templateType &var)
	{
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
		if (sizeof(var)==1)
			return ReadBits( ( unsigned char* ) &var, sizeof(templateType) * 8, true );
		else
		{
#ifndef __BITSTREAM_NATIVE_END
#ifdef _MSC_VER
#pragma warning(disable:4244)   // '=' : conversion from 'unsigned long' to 'unsigned short', possible loss of data
#endif
			if (DoEndianSwap())
			{
				unsigned char output[sizeof(templateType)];
				if (ReadBits( ( unsigned char* ) output, sizeof(templateType) * 8, true ))
				{
					ReverseBytes(output, (unsigned char*)&var, sizeof(templateType));
					return true;
				}
				return false;
			}
			else
#endif
				return ReadBits( ( unsigned char* ) & var, sizeof(templateType) * 8, true );
		}
	}

	template <class templateType>
	inline bool BitStream::ReadPtr(templateType *var)
	{
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
		if (sizeof(var)==1)
			return ReadBits( ( unsigned char* ) var, sizeof(templateType) * 8, true );
		else
		{
#ifndef __BITSTREAM_NATIVE_END
#ifdef _MSC_VER
#pragma warning(disable:4244)   // '=' : conversion from 'unsigned long' to 'unsigned short', possible loss of data
#endif
			if (DoEndianSwap())
			{
				unsigned char output[sizeof(templateType)];
				if (ReadBits( ( unsigned char* ) output, sizeof(templateType) * 8, true ))
				{
					ReverseBytes(output, (unsigned char*)var, sizeof(templateType));
					return true;
				}
				return false;
			}
			else
#endif
				return ReadBits( ( unsigned char* ) var, sizeof(templateType) * 8, true );
		}
	}

	/// Read a bool from a bitstream
	/// \param[in] var The value to read
	template <>
		inline bool BitStream::Read(bool &var)
	{
		if ( readOffset + 1 > numberOfBitsUsed )
			return false;

		if ( data[ readOffset >> 3 ] & ( 0x80 >> ( readOffset & 7 ) ) )   // Is it faster to just write it out here?
			var = true;
		else
			var = false;

		// Has to be on a different line for Mac
		readOffset++;

		return true;
	}

	/// Read a systemAddress from a bitstream
	/// \param[in] var The value to read
	template <>
		inline bool BitStream::Read(SystemAddress &var)
	{
		//Read(var.binaryAddress);
		// Don't endian swap the address
		ReadBits( ( unsigned char* ) & var.binaryAddress, sizeof(var.binaryAddress) * 8, true );
		// Unhide the IP address, done to prevent routers from changing it
		var.binaryAddress=~var.binaryAddress;
		return Read(var.port);
	}

	template <>
	inline bool BitStream::Read(RakNetGUID &var)
	{
		bool b=true;
		for (unsigned int i=0; i < sizeof(var.g) / sizeof(var.g[0]); i++)
			b=Read(var.g[i]);
		return b;
	}

	/// Read an networkID from a bitstream
	/// \param[in] var The value to read
	template <>
		inline bool BitStream::Read(NetworkID &var)
	{
#if defined (NETWORK_ID_SUPPORTS_PEER_TO_PEER)
		RakAssert(NetworkID::IsPeerToPeerMode());
		//if (NetworkID::IsPeerToPeerMode()) // Use the function rather than directly access the member or DLL users will get an undefined external error
		{
			bool hasGuid, hasSystemAddress;
			Read(hasGuid);
			if (hasGuid)
				Read(var.guid);
			else
				var.guid=UNASSIGNED_RAKNET_GUID;
			Read(hasSystemAddress);
			if (hasSystemAddress)
				Read(var.systemAddress);
			else
				var.systemAddress=UNASSIGNED_SYSTEM_ADDRESS;
		}
#endif
		/*
		Read(var.guid);
		Read(var.systemAddress);
		*/
		return Read(var.localSystemAddress);
	}

	/// Read an networkID from a bitstream
	/// \param[in] var The value to read
	template <>
		inline bool BitStream::Read(RakString &var)
	{
		return var.Deserialize(this);
	}
	template <>
		inline bool BitStream::Read(char *&var)
	{
		return RakString::Deserialize(var,this);
	}
	template <>
		inline bool BitStream::Read(unsigned char *&var)
	{
		return RakString::Deserialize((char*) var,this);
	}

	/// Read any integral type from a bitstream.  If the written value differed from the value compared against in the write function,
	/// var will be updated.  Otherwise it will retain the current value.
	/// ReadDelta is only valid from a previous call to WriteDelta
	/// \param[in] var The value to read
	template <class templateType>
		inline bool BitStream::ReadDelta(templateType &var)
	{
		bool dataWritten;
		bool success;
		success=Read(dataWritten);
		if (dataWritten)
			success=Read(var);
		return success;
	}

	/// Read a bool from a bitstream
	/// \param[in] var The value to read
	template <>
		inline bool BitStream::ReadDelta(bool &var)
	{
		return Read(var);
	}

	/// Read any integral type from a bitstream.  Undefine __BITSTREAM_NATIVE_END if you need endian swapping.
	/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
	/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
	/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
	/// \param[in] var The value to read
	template <class templateType>
		inline bool BitStream::ReadCompressed(templateType &var)
	{
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
		if (sizeof(var)==1)
			return ReadCompressed( ( unsigned char* ) &var, sizeof(templateType) * 8, true );
		else
		{
#ifndef __BITSTREAM_NATIVE_END
			if (DoEndianSwap())
			{
				unsigned char output[sizeof(templateType)];
				if (ReadCompressed( ( unsigned char* ) output, sizeof(templateType) * 8, true ))
				{
					ReverseBytes(output, (unsigned char*)&var, sizeof(templateType));
					return true;
				}
				return false;
			}
			else
#endif
				return ReadCompressed( ( unsigned char* ) & var, sizeof(templateType) * 8, true );
		}
	}

	template <>
		inline bool BitStream::ReadCompressed(SystemAddress &var)
	{
		return Read(var);
	}

	template <>
	inline bool BitStream::ReadCompressed(RakNetGUID &var)
	{
		return Read(var);
	}

	template <>
		inline bool BitStream::ReadCompressed(NetworkID &var)
	{
		return Read(var);
	}

	template <>
		inline bool BitStream::ReadCompressed(bool &var)
	{
		return Read(var);
	}

	/// For values between -1 and 1
	template <>
		inline bool BitStream::ReadCompressed(float &var)
	{
		unsigned short compressedFloat;
		if (Read(compressedFloat))
		{
			var = ((float)compressedFloat / 32767.5f - 1.0f);
			return true;
		}
		return false;
	}

	/// For values between -1 and 1
	template <>
		inline bool BitStream::ReadCompressed(double &var)
	{
		unsigned long compressedFloat;
		if (Read(compressedFloat))
		{
			var = ((double)compressedFloat / 2147483648.0 - 1.0);
			return true;
		}
		return false;
	}

	/// For strings
	template <>
		inline bool BitStream::ReadCompressed(RakString &var)
	{
		return var.DeserializeCompressed(this,false);
	}
	template <>
	inline bool BitStream::ReadCompressed(char *&var)
	{
		return RakString::DeserializeCompressed(var,this,false);
	}
	template <>
	inline bool BitStream::ReadCompressed(unsigned char *&var)
	{
		return RakString::DeserializeCompressed((char*) var,this,false);
	}

	/// Read any integral type from a bitstream.  If the written value differed from the value compared against in the write function,
	/// var will be updated.  Otherwise it will retain the current value.
	/// the current value will be updated.
	/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
	/// For non-floating point, this is lossless, but only has benefit if you use less than half the range of the type
	/// If you are not using __BITSTREAM_NATIVE_END the opposite is true for types larger than 1 byte
	/// ReadCompressedDelta is only valid from a previous call to WriteDelta
	/// \param[in] var The value to read
	template <class templateType>
		inline bool BitStream::ReadCompressedDelta(templateType &var)
	{
		bool dataWritten;
		bool success;
		success=Read(dataWritten);
		if (dataWritten)
			success=ReadCompressed(var);
		return success;
	}

	/// Read a bool from a bitstream
	/// \param[in] var The value to read
	template <>
		inline bool BitStream::ReadCompressedDelta(bool &var)
	{
		return Read(var);
	}


	template <class templateType> // templateType for this function must be a float or double
		void BitStream::WriteNormVector( templateType x, templateType y, templateType z )
	{
#ifdef _DEBUG
		RakAssert(x <= 1.01 && y <= 1.01 && z <= 1.01 && x >= -1.01 && y >= -1.01 && z >= -1.01);
#endif
		if (x>1.0)
			x=1.0;
		if (y>1.0)
			y=1.0;
		if (z>1.0)
			z=1.0;
		if (x<-1.0)
			x=-1.0;
		if (y<-1.0)
			y=-1.0;
		if (z<-1.0)
			z=-1.0;

		Write((bool) (x < 0.0));
		if (y==0.0)
			Write(true);
		else
		{
			Write(false);
			WriteCompressed((float)y);
			//Write((unsigned short)((y+1.0f)*32767.5f));
		}
		if (z==0.0)
			Write(true);
		else
		{
			Write(false);
			WriteCompressed((float)z);
			//Write((unsigned short)((z+1.0f)*32767.5f));
		}
	}

	template <class templateType> // templateType for this function must be a float or double
		void BitStream::WriteVector( templateType x, templateType y, templateType z )
	{
		templateType magnitude = sqrt(x * x + y * y + z * z);
		Write((float)magnitude);
		if (magnitude > 0.00001f)
		{
			WriteCompressed((float)(x/magnitude));
			WriteCompressed((float)(y/magnitude));
			WriteCompressed((float)(z/magnitude));
			//	Write((unsigned short)((x/magnitude+1.0f)*32767.5f));
			//	Write((unsigned short)((y/magnitude+1.0f)*32767.5f));
			//	Write((unsigned short)((z/magnitude+1.0f)*32767.5f));
		}
	}

	template <class templateType> // templateType for this function must be a float or double
		void BitStream::WriteNormQuat( templateType w, templateType x, templateType y, templateType z)
	{
		Write((bool)(w<0.0));
		Write((bool)(x<0.0));
		Write((bool)(y<0.0));
		Write((bool)(z<0.0));
		Write((unsigned short)(fabs(x)*65535.0));
		Write((unsigned short)(fabs(y)*65535.0));
		Write((unsigned short)(fabs(z)*65535.0));
		// Leave out w and calculate it on the target
	}

	template <class templateType> // templateType for this function must be a float or double
		void BitStream::WriteOrthMatrix(
		templateType m00, templateType m01, templateType m02,
		templateType m10, templateType m11, templateType m12,
		templateType m20, templateType m21, templateType m22 )
	{

		double qw;
		double qx;
		double qy;
		double qz;

		// Convert matrix to quat
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
		float sum;
		sum = 1 + m00 + m11 + m22;
		if (sum < 0.0f) sum=0.0f;
		qw = sqrt( sum  ) / 2;
		sum = 1 + m00 - m11 - m22;
		if (sum < 0.0f) sum=0.0f;
		qx = sqrt( sum  ) / 2;
		sum = 1 - m00 + m11 - m22;
		if (sum < 0.0f) sum=0.0f;
		qy = sqrt( sum  ) / 2;
		sum = 1 - m00 - m11 + m22;
		if (sum < 0.0f) sum=0.0f;
		qz = sqrt( sum  ) / 2;
		if (qw < 0.0) qw=0.0;
		if (qx < 0.0) qx=0.0;
		if (qy < 0.0) qy=0.0;
		if (qz < 0.0) qz=0.0;
		qx = _copysign( qx, m21 - m12 );
		qy = _copysign( qy, m02 - m20 );
		qz = _copysign( qz, m10 - m01 );

		WriteNormQuat(qw,qx,qy,qz);
	}

	template <class templateType> // templateType for this function must be a float or double
		bool BitStream::ReadNormVector( templateType &x, templateType &y, templateType &z )
	{
		//	unsigned short sy, sz;
		bool yZero, zZero;
		bool xNeg;
		float cy,cz;

		Read(xNeg);

		Read(yZero);
		if (yZero)
			y=0.0;
		else
		{
			ReadCompressed((float)cy);
			y=cy;
			//Read(sy);
			//y=((float)sy / 32767.5f - 1.0f);
		}

		if (!Read(zZero))
			return false;

		if (zZero)
			z=0.0;
		else
		{
			//	if (!Read(sz))
			//		return false;

			//	z=((float)sz / 32767.5f - 1.0f);
			if (!ReadCompressed((float)cz))
				return false;
			z=cz;
		}

		x = (templateType) (sqrtf((templateType)1.0 - y*y - z*z));
		if (xNeg)
			x=-x;
		return true;
	}

	template <class templateType> // templateType for this function must be a float or double
		bool BitStream::ReadVector( templateType &x, templateType &y, templateType &z )
	{
		float magnitude;
		//unsigned short sx,sy,sz;
		if (!Read(magnitude))
			return false;
		if (magnitude>0.00001f)
		{
			//	Read(sx);
			//	Read(sy);
			//	if (!Read(sz))
			//		return false;
			//	x=((float)sx / 32767.5f - 1.0f) * magnitude;
			//	y=((float)sy / 32767.5f - 1.0f) * magnitude;
			//	z=((float)sz / 32767.5f - 1.0f) * magnitude;
			float cx,cy,cz;
			ReadCompressed(cx);
			ReadCompressed(cy);
			if (!ReadCompressed(cz))
				return false;
			x=cx;
			y=cy;
			z=cz;
			x*=magnitude;
			y*=magnitude;
			z*=magnitude;
		}
		else
		{
			x=0.0;
			y=0.0;
			z=0.0;
		}
		return true;
	}

	template <class templateType> // templateType for this function must be a float or double
		bool BitStream::ReadNormQuat( templateType &w, templateType &x, templateType &y, templateType &z)
	{
		bool cwNeg, cxNeg, cyNeg, czNeg;
		unsigned short cx,cy,cz;
		Read(cwNeg);
		Read(cxNeg);
		Read(cyNeg);
		Read(czNeg);
		Read(cx);
		Read(cy);
		if (!Read(cz))
			return false;

		// Calculate w from x,y,z
		x=(templateType)(cx/65535.0);
		y=(templateType)(cy/65535.0);
		z=(templateType)(cz/65535.0);
		if (cxNeg) x=-x;
		if (cyNeg) y=-y;
		if (czNeg) z=-z;
		float difference = 1.0f - x*x - y*y - z*z;
		if (difference < 0.0f)
			difference=0.0f;
		w = (templateType)(sqrt(difference));
		if (cwNeg)
			w=-w;

		return true;
	}

	template <class templateType> // templateType for this function must be a float or double
		bool BitStream::ReadOrthMatrix(
		templateType &m00, templateType &m01, templateType &m02,
		templateType &m10, templateType &m11, templateType &m12,
		templateType &m20, templateType &m21, templateType &m22 )
	{
		float qw,qx,qy,qz;
		if (!ReadNormQuat(qw,qx,qy,qz))
			return false;

		// Quat to orthogonal rotation matrix
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
		double sqw = (double)qw*(double)qw;
		double sqx = (double)qx*(double)qx;
		double sqy = (double)qy*(double)qy;
		double sqz = (double)qz*(double)qz;
		m00 =  (templateType)(sqx - sqy - sqz + sqw); // since sqw + sqx + sqy + sqz =1
		m11 = (templateType)(-sqx + sqy - sqz + sqw);
		m22 = (templateType)(-sqx - sqy + sqz + sqw);

		double tmp1 = (double)qx*(double)qy;
		double tmp2 = (double)qz*(double)qw;
		m10 = (templateType)(2.0 * (tmp1 + tmp2));
		m01 = (templateType)(2.0 * (tmp1 - tmp2));

		tmp1 = (double)qx*(double)qz;
		tmp2 = (double)qy*(double)qw;
		m20 =(templateType)(2.0 * (tmp1 - tmp2));
		m02 = (templateType)(2.0 * (tmp1 + tmp2));
		tmp1 = (double)qy*(double)qz;
		tmp2 = (double)qx*(double)qw;
		m21 = (templateType)(2.0 * (tmp1 + tmp2));
		m12 = (templateType)(2.0 * (tmp1 - tmp2));

		return true;
	}

	template <class templateType>
	BitStream& operator<<(BitStream& out, templateType& c)
	{
		out.Write(c);
		return out;
	}
	template <class templateType>
	BitStream& operator>>(BitStream& in, templateType& c)
	{
		bool success = in.Read(c);
		RakAssert(success);
		return in;
	}

}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif

#endif // VC6
