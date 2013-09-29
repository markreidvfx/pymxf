/*
 * $Id: mxf_version.c,v 1.3 2008/11/07 14:12:59 philipn Exp $
 *
 * libMXF version information
 *
 * Copyright (C) 2006  Philip de Nier <philipn@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
#include <mxf/mxf.h>


static mxfProductVersion g_libmxfVersion = {1, 0, 0, 0, 0};


#if defined (__linux__)
static const char* g_libmxfPlatformString = "libMXF (Linux)";
static const mxfUTF16Char* g_libmxfPlatformWString = L"libMXF (Linux)";
#elif defined(_WIN32)
static const char* g_libmxfPlatformString = "libMXF (Win32)";
static const mxfUTF16Char* g_libmxfPlatformWString = L"libMXF (Win32)";
#else
static const char* g_libmxfPlatformString = "libMXF (Unknown)";
static const mxfUTF16Char* g_libmxfPlatformWString = L"libMXF (Unknown)";
#endif


const mxfProductVersion* mxf_get_version(void)
{
    return &g_libmxfVersion;
}


const char* mxf_get_platform_string(void)
{
    return g_libmxfPlatformString;
}

const mxfUTF16Char* mxf_get_platform_wstring(void)
{
    return g_libmxfPlatformWString;
}

