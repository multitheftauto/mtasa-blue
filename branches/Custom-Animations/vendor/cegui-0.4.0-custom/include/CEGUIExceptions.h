/************************************************************************
	filename: 	CEGUIExceptions.h
	created:	20/2/2004
	author:		Paul D Turner
	
	purpose:	Defines exceptions used within the system
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#ifndef _CEGUIExceptions_h_
#define _CEGUIExceptions_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Root exception class used within the GUI system.
*/
class  CEGUIEXPORT Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	Exception(const String& message);
	virtual ~Exception(void);

	/*!
	\brief
		Return a reference to the String object describing the reason for the exception being thrown.

	\return
		String object containing a message describing the reason for the exception.
	*/
	const String&	getMessage(void) const		{return d_message;}


protected:
	String	d_message;
};

/*!
\brief
	Exception class used when none of the other classes are applicable
*/
class CEGUIEXPORT GenericException : public Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	GenericException(const String& message) : Exception(message) {}
};

/*!
\brief
	Exception class used when a request was made using a name of an unknown object
*/
class CEGUIEXPORT UnknownObjectException : public Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	UnknownObjectException(const String& message) : Exception(message) {}
};

/*!
\brief
	Exception class used when some impossible request was made for the current system state
*/
class CEGUIEXPORT InvalidRequestException : public Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	InvalidRequestException(const String& message) : Exception(message) {}
};

/*!
\brief
	Exception class used when a file handling problem occurs
*/
class CEGUIEXPORT FileIOException : public Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	FileIOException(const String& message) : Exception(message) {}
};

/*!
\brief
	Exception class used when an problem is detected within the Renderer or related objects
*/
class CEGUIEXPORT RendererException : public Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	RendererException(const String& message) : Exception(message) {}
};

/*!
\brief
	Exception class used when an attempt is made to use an object name that is already in use within the system
*/
class CEGUIEXPORT AlreadyExistsException : public Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	AlreadyExistsException(const String& message) : Exception(message) {}
};

/*!
\brief
	Exception class used when a memory handling error is detected
*/
class CEGUIEXPORT MemoryException : public Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	MemoryException(const String& message) : Exception(message) {}
};

/*!
\brief
	Exception class used when some required object or parameter is null
*/
class CEGUIEXPORT NullObjectException : public Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	NullObjectException(const String& message) : Exception(message) {}
};

/*!
\brief
	Exception class used when some attempt to delete, remove, or otherwise invalidate some object that is still in use occurs.
*/
class CEGUIEXPORT ObjectInUseException : public Exception
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	ObjectInUseException(const String& message) : Exception(message) {}
};


} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIExceptions_h_
