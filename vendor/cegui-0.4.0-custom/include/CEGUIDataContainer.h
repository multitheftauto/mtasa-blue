/************************************************************************
	filename: 	CEGUIDataContainer.h
	created:	10/8/2004
	author:		James '_mental_' O'Sullivan
	
	purpose:	Defines abstract base class for Window objects
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
#ifndef _CEGUIDataContainer_h_
#define _CEGUIDataContainer_h_

#include "CEGUIBase.h"

#include <malloc.h>
//#include <xercesc/sax/InputSource.hpp>

// Start of CEGUI namespace section
namespace CEGUI
{

template <class T>
class CEGUIEXPORT DataContainer
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for DataContainer class
	*/
    DataContainer()
      : mData(0),
        mSize(0)
    {
    }

	/*!
	\brief
		Destructor for DataContainer class
	*/
	virtual ~DataContainer(void) 
    { 
        release();
    }

	/*************************************************************************
		Accessor functions
	*************************************************************************/
	/*!
	\brief
		Set a pointer to the external data.

	\param data
        Pointer to a object of type T, where T defined when the template is specialized.
	*/
    void setData(T* data) { mData = data; }

	/*!
	\brief
		Return a pointer to the external data

	\return
		Pointer to an object of type T, where T defined when the template is specialized.
	*/
    T* getDataPtr(void) { return mData; }

	/*!
	\brief
	    Set the size of the external data.  This maybe zero depending on the type of T.

	\param size
	    size_t containing the size of the external data
	*/
    void setSize(size_t size) { mSize = size; }

	/*!
	\brief
		Get the size of the external data.  This maybe zero depending on the type of T.

	\return
	    size_t containing the size of the external data
	*/
    size_t getSize(void) const { return mSize; }

	/*!
	\brief
		Release supplied data.

	*/
    virtual void release(void)
    {
        if(mData)
        {
            delete mData;
            mData = 0;
        }
    }
	/*************************************************************************
		Implementation Data
	*************************************************************************/
protected:
    T* mData;
    size_t mSize;
};

// Specialized templates
class RawDataContainer : public DataContainer<unsigned char> 
{
public:
    RawDataContainer() : DataContainer<unsigned char>()
    {
    }

    ~RawDataContainer() 
    {
        release();
    }

    void release(void)
    {
        if(mData)
        {
            delete [] mData;
            mData = 0;
        }
    }
};

//typedef DataContainer<unsigned char> RawDataContainer;
//typedef DataContainer<XERCES_CPP_NAMESPACE::InputSource> InputSourceContainer;

} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIDataContainer_h_


