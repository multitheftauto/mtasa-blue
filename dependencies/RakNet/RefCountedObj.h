/// \file
/// \brief \b Reference counted object. Very simple class for quick and dirty uses.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __REF_COUNTED_OBJ_H
#define __REF_COUNTED_OBJ_H

#include "RakMemoryOverride.h"

/// World's simplest class :)
class RefCountedObj
{
	public:
		RefCountedObj() {refCount=1;}
		virtual ~RefCountedObj() {}
		void AddRef(void) {refCount++;}
		void Deref(void) {if (--refCount==0) RakNet::OP_DELETE(this, __FILE__, __LINE__);}
		int refCount;
};

#endif
