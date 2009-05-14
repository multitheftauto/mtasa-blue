#ifndef __RAK_STRING_H
#define __RAK_STRING_H 

#include "Export.h"
#include "DS_List.h"
#include "RakNetTypes.h" // int64_t
#include <stdio.h>

namespace RakNet
{

class BitStream;

/// \brief String class
/// Has the following improvements over std::string
/// Reference counting: Suitable to store in lists
/// Varidic assignment operator
/// Doesn't cause linker errors
class RAK_DLL_EXPORT RakString
{
public:
	/// Constructors
	RakString();
	RakString(char input);
	RakString(unsigned char input);
	RakString(const unsigned char *format, ...);
	RakString(const char *format, ...);
	~RakString();
	RakString( const RakString & rhs);

	/// Implicit return of const char*
	operator const char* () const {return sharedString->c_str;}

	/// Same as std::string::c_str
	const char *C_String(void) const {return sharedString->c_str;}

	// Lets you modify the string. Do not make the string longer - however, you can make it shorter, or change the contents.
	char *C_StringUnsafe(void) {Clone(); return sharedString->c_str;}

	/// Assigment operators
	RakString& operator = ( const RakString& rhs );
	RakString& operator = ( const char *str );
	RakString& operator = ( char *str );
	RakString& operator = ( const char c );

	/// Concatenation
	RakString& operator +=( const RakString& rhs);
	RakString& operator += ( const char *str );
	RakString& operator += ( char *str );
	RakString& operator += ( const char c );

	/// Character index. Do not use to change the string however.
	unsigned char operator[] ( const unsigned int position ) const;

	/// Equality
	bool operator==(const RakString &rhs) const;
	bool operator==(const char *str) const;
	bool operator==(char *str) const;

	// Comparison
	bool operator < ( const RakString& right ) const;
	bool operator <= ( const RakString& right ) const;
	bool operator > ( const RakString& right ) const;
	bool operator >= ( const RakString& right ) const;

	/// Inequality
	bool operator!=(const RakString &rhs) const;

	/// Change all characters to lowercase
	const char * ToLower(void);

	/// Change all characters to uppercase
	const char * ToUpper(void);

	/// Set the value of the string
	void Set(const char *format, ...);

	/// Returns if the string is empty. Also, C_String() would return ""
	bool IsEmpty(void) const;

	/// Returns the length of the string
	size_t GetLength(void) const;

	/// Replace character(s) in starting at index, for count, with c
	void Replace(unsigned index, unsigned count, unsigned char c);

	/// Replace character at index with c
	void SetChar( unsigned index, unsigned char c );

	/// Replace character at index with string s
	void SetChar( unsigned index, RakNet::RakString s );

	// Gets the substring starting at index for count characters
	RakString SubStr(unsigned int index, unsigned int count) const;

	/// Erase characters out of the string at index for count
	void Erase(unsigned int index, unsigned int count);

	/// Compare strings (case sensitive)
	int StrCmp(const RakString &rhs) const;

	/// Compare strings (not case sensitive)
	int StrICmp(const RakString &rhs) const;

	/// Clear the string
	void Clear(void);

	/// Print the string to the screen
	void Printf(void);

	/// Print the string to a file
	void FPrintf(FILE *fp);

	/// Does the given IP address match the IP address encoded into this string, accounting for wildcards?
	bool IPAddressMatch(const char *IP);

	/// Does the string contain non-printable characters other than spaces?
	bool ContainsNonprintableExceptSpaces(void) const;

	/// Is this a valid email address?
	bool IsEmailAddress(void) const;

	/// URL Encode the string. See http://www.codeguru.com/cpp/cpp/cpp_mfc/article.php/c4029/
	void URLEncode(void);

	/// RakString uses a freeList of old no-longer used strings
	/// Call this function to clear this memory on shutdown
	static void FreeMemory(void);
	/// \internal
	static void FreeMemoryNoMutex(void);

	/// Serialize to a bitstream, uncompressed (slightly faster)
	/// \param[out] bs Bitstream to serialize to
	void Serialize(BitStream *bs);

	/// Static version of the Serialize function
	static void Serialize(const char *str, BitStream *bs);

	/// Serialize to a bitstream, compressed (better bandwidth usage)
	/// \param[out]  bs Bitstream to serialize to
	/// \param[in] languageId languageId to pass to the StringCompressor class
	/// \param[in] writeLanguageId encode the languageId variable in the stream. If false, 0 is assumed, and DeserializeCompressed will not look for this variable in the stream (saves bandwidth)
	/// \pre StringCompressor::AddReference must have been called to instantiate the class (Happens automatically from RakPeer::Startup())
	void SerializeCompressed(BitStream *bs, int languageId=0, bool writeLanguageId=false);

	/// Static version of the SerializeCompressed function
	static void SerializeCompressed(const char *str, BitStream *bs, int languageId=0, bool writeLanguageId=false);

	/// Deserialize what was written by Serialize
	/// \param[in] bs Bitstream to serialize from
	/// \return true if the deserialization was successful
	bool Deserialize(BitStream *bs);

	/// Static version of the Deserialize() function
	static bool Deserialize(char *str, BitStream *bs);

	/// Deserialize compressed string, written by SerializeCompressed
	/// \param[in] bs Bitstream to serialize from
	/// \param[in] readLanguageId If true, looks for the variable langaugeId in the data stream. Must match what was passed to SerializeCompressed
	/// \return true if the deserialization was successful
	/// \pre StringCompressor::AddReference must have been called to instantiate the class (Happens automatically from RakPeer::Startup())
	bool DeserializeCompressed(BitStream *bs, bool readLanguageId=false);

	/// Static version of the DeserializeCompressed() function
	static bool DeserializeCompressed(char *str, BitStream *bs, bool readLanguageId=false);

	static const char *ToString(int64_t i);
	static const char *ToString(uint64_t i);

	/// \internal
	static size_t GetSizeToAllocate(size_t bytes)
	{
		const size_t smallStringSize = 128-sizeof(unsigned int)-sizeof(size_t)-sizeof(char*)*2;
		if (bytes<=smallStringSize)
			return smallStringSize;
		else
			return bytes*2;
	}

	/// \internal
	struct SharedString
	{
		unsigned int refCount;
		size_t bytesUsed;
		char *bigString;
		char *c_str;
		char smallString[128-sizeof(unsigned int)-sizeof(size_t)-sizeof(char*)*2];		
	};

	/// \internal
	RakString( SharedString *_sharedString );

	/// \internal
	SharedString *sharedString;

//	static SimpleMutex poolMutex;
//	static DataStructures::MemoryPool<SharedString> pool;
	/// \internal
	static SharedString emptyString;

	//static SharedString *sharedStringFreeList;
	//static unsigned int sharedStringFreeListAllocationCount;
	/// \internal
	/// List of free objects to reduce memory reallocations
	static DataStructures::List<SharedString*> freeList;

	/// Means undefined position
	static unsigned int nPos;


	static int RakStringComp( RakString const &key, RakString const &data );

	static void LockMutex(void);
	static void UnlockMutex(void);

protected:
	void Allocate(size_t len);
	void Assign(const char *str);
	void Clone(void);
	void Free(void);
	unsigned char ToLower(unsigned char c);
	unsigned char ToUpper(unsigned char c);
	void Realloc(SharedString *sharedString, size_t bytes);
};

}

const RakNet::RakString operator+(const RakNet::RakString &lhs, const RakNet::RakString &rhs);


#endif
