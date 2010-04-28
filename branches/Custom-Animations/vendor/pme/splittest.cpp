


#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pme.h>


int main ( )
{

	PME regex ( ", " );
	int i = regex.split ( "very, very2, tired" );

	for ( int j = 0; j < i; j++ ) {
		
		printf ( "%s\n", regex [ j ].c_str ( ) );

	}

	StringVector sv = regex.GetStringVector ( );
	
	for ( StringVector::iterator i = sv.begin ( );
		  i != sv.end ( );
		  i++ ) {

		printf ( "%s\n", i->c_str ( ) );

	}


	PME match ( "([a-z])([0-9])([A-Z])" );

	match.match ( "b2Z" );

	sv = match.GetStringVector ( );

	for ( StringVector::iterator i = sv.begin ( );
		  i != sv.end ( );
		  i++ ) {

		printf ( "%s\n", i->c_str ( ) );

	}

}
