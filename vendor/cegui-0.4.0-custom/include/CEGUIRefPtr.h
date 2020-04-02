/************************************************************************
	filename: 	CEGUIRefPtr.h
	created:	15/10/2004
	author:		Gerald Lindsly
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
#ifndef _CEGUIRefPtr_h_
#define _CEGUIRefPtr_h_

#include "CEGUIBase.h"

namespace CEGUI {

class CEGUIEXPORT Referenced
{
public:
  Referenced() : d_refCount(0) {}
  Referenced(const Referenced&) : d_refCount(0) {}

  Referenced& operator=(Referenced&) { return *this; }

  /* Increment the reference count by one, indicating that 
      a pointer to this object is referencing it. */
  void addRef() const { ++d_refCount; }
  
  /* Decrement the reference count by one, indicating that 
     a pointer to this object is no longer referencing it. 
     if the count drops to zero, this is deleted. */
  void release() const { if (!--d_refCount) delete this; }
  
  /* Decrement the reference count by one, indicating that 
      a pointer to this object is no longer referencing it;
      however, do not delete it, even if ref count goes to 0. */
  void releaseButKeep() const { --d_refCount; }
  
  /** return the number pointers currently referencing this object. */
  int refCount() const { return d_refCount; }
 
protected:
  virtual ~Referenced();
  mutable int d_refCount;
};


template<class T> class RefPtr
{
  T* d_p;

public:
//  RefPtr()     : d_p(0) {}
  
  RefPtr()     : d_p(new T()) { d_p->addRef(); }

  RefPtr(T* t) : d_p(t) { if (t)   d_p->addRef(); }
  RefPtr(const RefPtr& r) : d_p(r.d_p)
                        { if (r.d_p) d_p->addRef(); }
  ~RefPtr()
  { if (d_p) {
      d_p->release();
      d_p = 0;
    }
  }

  RefPtr& operator=(T* q)
  {
    if (d_p != q) {
      T* t = d_p;
      d_p = q;
      if (q) q->addRef();
      if (t) t->release();
    }
    return *this;
  }

  RefPtr& operator=(const RefPtr& r) { return *this = r.d_p; }

  bool operator==(const RefPtr& r) const { return d_p == r.d_p; }
  bool operator!=(const RefPtr& r) const { return d_p != r.d_p; }
  bool operator< (const RefPtr& r) const { return d_p <  r.d_p; }
  bool operator> (const RefPtr& r) const { return d_p >  r.d_p; }

  bool operator ==(const T* q) const { return d_p == q; }
  bool operator !=(const T* q) const { return d_p != q; }
  bool operator < (const T* q) const { return d_p <  q; }
  bool operator > (const T* q) const { return d_p >  q; }


        T& operator*()        { return *d_p; }
  const T& operator*() const  { return *d_p; }

        T* operator->()       { return d_p; }
  const T* operator->() const { return d_p; }

        T* get()              { return d_p; }
  const T* get() const        { return d_p; }

  bool operator!() const	    { return d_p == 0; }
  bool valid() const	        { return d_p != 0; }

  T* release()
  {
    T* t = d_p;
    if (d_p) {
      d_p->releaseButKeep();
      d_p = 0;
    } 
    return t;
  }
};

} // end namespace CEGUI

#endif
