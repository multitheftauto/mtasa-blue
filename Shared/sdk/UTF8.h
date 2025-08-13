/*
 * Smart Common Input Method
 *
 * Copyright (c) 2002-2005 James Su <suzhe@tsinghua.org.cn>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * $Id: scim_utility.cpp,v 1.48.2.5 2006/11/02 04:11:51 suzhe Exp $
 */
#pragma once

#include <string>

int utf8_mbtowc(wchar_t* pwc, const unsigned char* src, int src_len);

int utf8_wctomb(unsigned char* dest, wchar_t wc, int dest_size);

//////////////////////////////////////////////////
//
// Original - For testing
//

std::wstring utf8_mbstowcs_orig(const std::string& str);

std::string utf8_wcstombs_orig(const std::wstring& wstr);

//////////////////////////////////////////////////
//
// Optimized - faster for smaller strings
//

std::wstring utf8_mbstowcs(const std::string& str);

std::string utf8_wcstombs(const std::wstring& wstr);
