/*
 * $Id: mxf_macros.h,v 1.6 2011/01/10 17:05:15 john_f Exp $
 *
 * General purpose macros
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
 
#ifndef __MXF_MACROS_H__
#define __MXF_MACROS_H__


#ifdef __cplusplus
extern "C" 
{
#endif


/*
* Command checking macros
*   ...ORET - check succeeds, otherwise return 0
*   ...OFAIL - check succeeds, otherwise goto fail
*/

#define CHK_ORET(cmd) \
    if (!(cmd)) \
    { \
        mxf_log_error("'%s' failed, in %s:%d\n", #cmd, __FILE__, __LINE__); \
        return 0; \
    }

#define CHK_OFAIL(cmd) \
    if (!(cmd)) \
    { \
        mxf_log_error("'%s' failed, in %s:%d\n", #cmd, __FILE__, __LINE__); \
        goto fail; \
    }

#define CHK_MALLOC_ORET(var, type) \
    CHK_ORET((var = (type*)malloc(sizeof(type))) != NULL);
    
#define CHK_MALLOC_OFAIL(var, type) \
    CHK_OFAIL((var = (type*)malloc(sizeof(type))) != NULL);
    
#define CHK_MALLOC_ARRAY_ORET(var, type, length) \
    CHK_ORET((var = (type*)malloc(sizeof(type) * (length))) != NULL);
    
#define CHK_MALLOC_ARRAY_OFAIL(var, type, length) \
    CHK_OFAIL((var = (type*)malloc(sizeof(type) * (length))) != NULL);
    
    
/*
* Free the memory and set the variable to NULL
*/
    
#define SAFE_FREE(d_ptr) \
    if (*d_ptr != NULL) \
    { \
        free(*d_ptr); \
        *d_ptr = NULL; \
    }


/* 
* Helpers for logging
*/

/* e.g. mxf_log_error("Some error %d" LOG_LOC_FORMAT, x, LOG_LOC_PARAMS); */
#define LOG_LOC_FORMAT      ", in %s:%d\n"
#define LOG_LOC_PARAMS      __FILE__, __LINE__      


/*
* 64-bit printf formatting
*/
#if defined(_MSC_VER)
#define PFi64 "I64d"
#define PFu64 "I64u"
#else
#define PFi64 PRId64
#define PFu64 PRIu64
#endif

/*
* size_t printf formatting varies by platform
*/
#if defined(__APPLE__) || defined(__x86_64__)
#define PFszt "lu"
#else
#define PFszt "u"
#endif



#ifdef __cplusplus
}
#endif


#endif



