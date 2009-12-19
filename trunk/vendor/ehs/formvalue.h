
#ifndef FORMVALUE_H
#define FORMVALUE_H

#include <string>

#include "contentdisposition.h"

/// This is how data is passed in from the client has to be more complicated because of metadata associated with MIME-style attachments
/**
 * This is how data is passed in from the client has to be more complicated because of metadata 
 *   associated with MIME-style attachments.  Each element of a form is put into a ContentDisposition
 *   object which can be looked at in HandleRequest to see what data might have been sent in.
 */
class FormValue {
	
  public:
	
	/// for MIME attachments only, normal header information like content-type -- everything except content-disposition, which is in oContentDisposition
	StringMap oFormHeaders;

	/// everything in the content disposition line
	ContentDisposition oContentDisposition; 
	
	/// the body of the value.  For non-MIME-style attachments, this is the only part used.
	std::string sBody; 
	
	/// Default constructor
	FormValue ( ) {
#ifdef EHS_MEMORY
		fprintf ( stderr, "[EHS_MEMORY] Allocated: FormValue\n" );
#endif		
	}
	
	/// Constructor 
	FormValue ( std::string & irsBody, ///< body for the form value
				ContentDisposition & ioContentDisposition ///< content disposition type string
		) :
		oContentDisposition ( ioContentDisposition ),
		sBody ( irsBody ) {
#ifdef EHS_MEMORY
		fprintf ( stderr, "[EHS_MEMORY] Allocated: FormValue\n" );
#endif		
	}

#ifdef EHS_MEMORY
	/// This is only for watching memory allocation
	FormValue ( const FormValue & iroFormValue ) {
		*this = iroFormValue;
		fprintf ( stderr, "[EHS_MEMORY] Allocated: FormValue (Copy Constructor)\n" );
	}
#endif		

	/// destructor
	virtual ~FormValue ( ) {
#ifdef EHS_MEMORY
		fprintf ( stderr, "[EHS_MEMORY] Deallocated: FormValue\n" );
#endif		
	}
   
};

#endif // FORMVALUE_H
