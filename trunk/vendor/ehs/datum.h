
#ifndef DATUM_H
#define DATUM_H

#include <string>


/// class that makes it easy to go between numbers and strings
/** 
 * class that can take data in string or number form and can return it 
 * in either form.  Very similar to a perl scalar (without references)
 */
class Datum {

  protected:

	/// holds the data in string form
	std::string sDatum;

	/// holds the data in pointer form
	void * pDatum;

  public:

	/// assignment operator for longs
	Datum & operator= ( long inLong );

	/// assignment operator for ints
	Datum & operator= ( int inInt );

	/// assignment operator for doubles
	Datum & operator= ( double inDouble );

	/// assignment operator for std::strings
	Datum & operator= ( std::string isString );

	/// assignment operator for char * strings
	Datum & operator= ( char * ipsString );

	/// assignment operator for const char * strings
	Datum & operator= ( const char * ipsString );

	/// assignment operator for void *
	Datum & operator= ( void * ipVoid );

	/// equality operator for const char * string
	bool operator== ( const char * ipsString );

	/// conversion operaor to return an in
	operator int ( );

	/// explicit accessor for int
	int GetInt() { return (int)(*this); }

	/// conversion operator for long
	operator long ( );

	/// conversion operator for double
	operator double ( );

	/// conversion operator for std::string
	operator std::string ( );

	/// conversion operator for const char *
	operator const char * ( );

	/// explicit accessor for const char *
	const char * GetCharString ( ) { return (const char*)(*this); }
  

	/// inequality operator to test against int
	bool operator!= ( int );

	/// inequality operator to test against const char *
	bool operator!= ( const char * );

};

#endif // DATUM_H
