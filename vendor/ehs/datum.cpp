
#include "datum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define strcasecmp stricmp
#endif



Datum & Datum::operator= ( long inLong )
{
	char psBuffer [ 100 ];
	sprintf ( psBuffer, "%ld", inLong );
	sDatum = psBuffer;
	return *this;
}
Datum & Datum::operator= ( int inInt )
{
	char psBuffer [ 100 ];
	sprintf ( psBuffer, "%d", inInt );
	sDatum = psBuffer;
	return *this;
}
Datum & Datum::operator= ( double inDouble )
{
	char psBuffer [ 100 ];
	sprintf ( psBuffer, "%lf", inDouble );
	sDatum = psBuffer;
	return *this;
}
Datum & Datum::operator= ( std::string isString )
{
	sDatum = isString;
	return *this;
}

Datum & Datum::operator= ( char * ipsString )
{
	sDatum = ipsString;
	return *this;
}
Datum & Datum::operator= ( const char * ipsString )
{
	sDatum = ipsString;
	return *this;
}

Datum & Datum::operator= ( void * ipVoid )
{
	pDatum = ipVoid;
	return *this;
}
	
bool Datum::operator== ( const char * ipsString )
{

	return sDatum == ipsString;

}

Datum::operator long ( )
{
	return atol ( sDatum.c_str ( ) );
}

Datum::operator int ( )
{
	return atoi ( sDatum.c_str ( ) );
}

Datum::operator double ( )
{
	return atof ( sDatum.c_str ( ) );
}

Datum::operator std::string ( )
{
	return sDatum;
}

Datum::operator const char * ( )
{
	return sDatum.c_str ( );
}

bool Datum::operator!= ( int inCompare )
{
	return ((int)*this) != inCompare;
}

bool Datum::operator != ( const char * ipsCompare ) 
{
	return strcasecmp ( (const char*)*this, ipsCompare );
}
