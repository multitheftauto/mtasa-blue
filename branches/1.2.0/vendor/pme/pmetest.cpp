


#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pme.h>


using namespace std;

int main ( int argc, char ** argv, char ** env )
{

	
	if ( argc != 3 && argc != 4 ) {

		printf ( "Usage: %s <regex> <string> [modifiers]\n", argv [ 0 ] );
		printf ( "\tMake sure to put arguments in quotes just to be safe\n" );
		exit ( 0 );

	}

	std::string sRegex = argv [ 1 ];
	std::string sString = argv [ 2 ];
	std::string sModifiers = "";
	if ( argc == 4 ) {
		sModifiers = argv [ 3 ];
	}
	
/*	printf ( "Running '%s' on string '%s'\n",
			 sRegex.c_str ( ),
			 sString.c_str ( ) );
*/

	PME oRegex ( sRegex, sModifiers );

	if ( !oRegex.IsValid ( ) ) {
		printf ( "Syntax error in regex, failed to compile\n" );
		return 0;
	}

	int nMatches = oRegex.match ( sString );

	if ( nMatches > 0 ) {
		for ( int i = 0; i < nMatches; i++ ) {
			
			printf ( "[%d] - '%s'\n", i, oRegex [ i ].c_str ( ) );
			
		}
	} else if ( nMatches == 0 ) {

		printf ( "Found no matches\n" );

	} else {

		printf ( "Error running regex\n" );

	}




#if 0 // old stuff
/*
	PME re ( "([A-Z]+)([a-z]+)([0-9]+)", "i" );


	re.match ( "ABCabc123" );

	// re[0] is the full string
	cout << re[1] << " " << re[2] << " " << re[3] << endl;


	PME re2 ( "[a-z]", "gi" );

	while ( re2.match ( "abcABC123" ) ) {
		printf ( "%s\n", re2[0].c_str ( ) );
	}

	PME re3 ( "(a)(b)(c)(d)", "gi" );
	std::string resultstring = re3.sub ( "abcdefghijklmnopqrstuvwxyz", "[$4$3$2$1]" );
	
	cout << resultstring << std::endl;
	
	std::string list = "abc def ghi jkl ";

	PME l ( "\\s+" );

	int numsplits = l.split ( list );

	cout << "Number of splits: " << numsplits << std::endl;

	for ( int i = 0; i < numsplits; i++ ) {

		printf ( "'%s'\n", l[i].c_str ( ) );

	}
*/
#endif

}
