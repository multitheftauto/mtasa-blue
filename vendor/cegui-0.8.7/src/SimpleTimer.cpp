/***********************************************************************
    created:    Sat Feb 18 2012
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2012 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/SimpleTimer.h"

#if (defined(__WIN32__) || defined(_WIN32) ) && (!defined __MINGW32__)

#ifdef WIN32_LEAN_AND_MEAN
    #undef WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

double CEGUI::SimpleTimer::currentTime()
{
    return timeGetTime() / 1000.0;
}

#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__HAIKU__) || defined(__CYGWIN__) || defined(__MINGW32__)
#include <sys/time.h>
double CEGUI::SimpleTimer::currentTime()
{
    timeval timeStructure;
    gettimeofday(&timeStructure, 0);
    return timeStructure.tv_sec + timeStructure.tv_usec / 1000000.0;
}
#else
#error "SimpleTimer not available for this platform, please implement it"
#endif
