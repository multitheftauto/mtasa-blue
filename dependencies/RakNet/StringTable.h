/// \file
/// \brief A simple class to encode and decode known strings based on a lookup table.  Similar to the StringCompressor class.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __STRING_TABLE_H
#define __STRING_TABLE_H

#include "DS_OrderedList.h"
#include "Export.h"
#include "RakMemoryOverride.h"

/// Forward declaration
namespace RakNet
{
	class BitStream;
};

/// StringTableType should be the smallest type possible, or else it defeats the purpose of the StringTable class, which is to save bandwidth.
typedef unsigned char StringTableType;

/// The string plus a bool telling us if this string was copied or not.
struct StrAndBool
{
	char *str;
	bool b;
};
int RAK_DLL_EXPORT StrAndBoolComp( char *const &key, const StrAndBool &data );

namespace RakNet
{
	/// This is an even more efficient alternative to StringCompressor in that it writes a single byte from a lookup table and only does compression
	/// if the string does not already exist in the table.
	/// All string tables must match on all systems - hence you must add all the strings in the same order on all systems.
	/// Furthermore, this must be done before sending packets that use this class, since the strings are ordered for fast lookup.  Adding after that time would mess up all the indices so don't do it.
	/// Don't use this class to write strings which were not previously registered with AddString, since you just waste bandwidth then.  Use StringCompressor instead.
	/// \brief Writes a string index, instead of the whole string
	class RAK_DLL_EXPORT StringTable
	{
	public:

		/// Destructor	
		~StringTable();

		/// static function because only static functions can access static members
		/// The RakPeer constructor adds a reference to this class, so don't call this until an instance of RakPeer exists, or unless you call AddReference yourself.
		/// \return the unique instance of the StringTable
		static StringTable* Instance(void);

		/// Add a string to the string table.
		/// \param[in] str The string to add to the string table
		/// \param[in] copyString true to make a copy of the passed string (takes more memory), false to not do so (if your string is in static memory).
		void AddString(const char *str, bool copyString);

		/// Writes input to output, compressed.  Takes care of the null terminator for you.
		/// Relies on the StringCompressor class, which is automatically reference counted in the constructor and destructor in RakPeer.  You can call the reference counting functions yourself if you wish too.
		/// \param[in] input Pointer to an ASCII string
		/// \param[in] maxCharsToWrite The size of \a input 
		/// \param[out] output The bitstream to write the compressed string to
		void EncodeString( const char *input, int maxCharsToWrite, RakNet::BitStream *output );

		/// Writes input to output, uncompressed.  Takes care of the null terminator for you.
		/// Relies on the StringCompressor class, which is automatically reference counted in the constructor and destructor in RakPeer.  You can call the reference counting functions yourself if you wish too.
		/// \param[out] output A block of bytes to receive the output
		/// \param[in] maxCharsToWrite Size, in bytes, of \a output .  A NULL terminator will always be appended to the output string.  If the maxCharsToWrite is not large enough, the string will be truncated.
		/// \param[in] input The bitstream containing the compressed string
		bool DecodeString( char *output, int maxCharsToWrite, RakNet::BitStream *input );

		/// Used so I can allocate and deallocate this singleton at runtime
		static void AddReference(void);

		/// Used so I can allocate and deallocate this singleton at runtime
		static void RemoveReference(void);

		/// Private Constructor	
		StringTable();

	protected:
		/// Called when you mess up and send a string using this class that was not registered with AddString
		/// \param[in] maxCharsToWrite Size, in bytes, of \a output .  A NULL terminator will always be appended to the output string.  If the maxCharsToWrite is not large enough, the string will be truncated.
		void LogStringNotFound(const char *strName);

		/// Singleton instance
		static StringTable *instance;
		static int referenceCount;

		DataStructures::OrderedList<char *, StrAndBool, StrAndBoolComp> orderedStringList;
	};
}


#endif
