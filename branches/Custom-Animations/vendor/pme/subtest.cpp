
#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pme.h>


int main ( )
{

	// substitution test
	PME sub1 ( "[\\s]+$" );
	PME sub2 ( "[\\s]+" );

	std::string sResult = 
		sub2.sub ( sub1.sub ( "    \t  blah \t  ", "" ), "" );
	
	printf ( "substition results '%s'\n", sResult.c_str ( ) );
	
	sResult = sub2.sub ( sub1.sub ( "blah", "" ), "" );

	printf ( "substition results '%s'\n", sResult.c_str ( ) );

}
