/// \file
/// \brief \b [Internal] Generates and validates checksums
///
/// \note I didn't write this, but took it from http://www.flounder.com/checksum.htm
///

#ifndef __CHECKSUM_H
#define __CHECKSUM_H

#include "RakMemoryOverride.h"

/// Generates and validates checksums
class CheckSum
{

public:
	
 /// Default constructor
	
	CheckSum()
	{
		Clear();
	}
	
	void Clear()
	{
		sum = 0;
		r = 55665;
		c1 = 52845;
		c2 = 22719;
	}
	
	void Add ( unsigned int w );
	
	
	void Add ( unsigned short w );
	
	void Add ( unsigned char* b, unsigned int length );
	
	void Add ( unsigned char b );
	
	unsigned int Get ()
	{
		return sum;
	}
	
protected:
	unsigned short r;
	unsigned short c1;
	unsigned short c2;
	unsigned int sum;
};

#endif
