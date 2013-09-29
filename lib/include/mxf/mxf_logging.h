/*
 * $Id: mxf_logging.h,v 1.3 2008/11/07 14:12:59 philipn Exp $
 *
 * libMXF logging functions
 *
 * Copyright (C) 2006  Philip de Nier <philipn@users.sourceforge.net>
 * Copyright (C) 2006  Stuart Cunningham <stuart_hc@users.sourceforge.net>
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
 
#ifndef __MXF_LOGGING_H__
#define __MXF_LOGGING_H__


#include <stdarg.h>


#ifdef __cplusplus
extern "C" 
{
#endif


typedef enum
{
    MXF_DLOG = 0, 
    MXF_ILOG, 
    MXF_WLOG, 
    MXF_ELOG
} MXFLogLevel;


typedef void (*mxf_vlog_func) (MXFLogLevel level, const char* format, va_list p_arg);
typedef void (*mxf_log_func) (MXFLogLevel level, const char* format, ...);

/* is set by default to the mxf_log_default function */
extern mxf_vlog_func mxf_vlog;
extern mxf_log_func mxf_log;
extern MXFLogLevel g_mxfLogLevel;


/* outputs to stderr for MXF_ELOG or stdout for the other levels */
void mxf_vlog_default(MXFLogLevel level, const char* format, va_list p_arg);
void mxf_log_default(MXFLogLevel level, const char* format, ...);


/* sets mxf_log to log to the file 'filename' */
int mxf_log_file_open(const char* filename);

void mxf_log_file_flush();
void mxf_log_file_close();


/* log level in function name */
void mxf_log_debug(const char* format, ...);
void mxf_log_info(const char* format, ...);
void mxf_log_warn(const char* format, ...);
void mxf_log_error(const char* format, ...);



#ifdef __cplusplus
}
#endif


#endif 



