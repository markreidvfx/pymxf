/*
 * $Id: mxf_logging.c,v 1.3 2008/11/07 14:12:59 philipn Exp $
 *
 * libMXF logging functions
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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <time.h>


#include <mxf/mxf_logging.h>


mxf_vlog_func mxf_vlog = mxf_vlog_default;
mxf_log_func mxf_log = mxf_log_default;
MXFLogLevel g_mxfLogLevel = MXF_DLOG;


static FILE* g_mxfFileLog = NULL;

static void logmsg(FILE* file, MXFLogLevel level, const char* format, va_list p_arg)
{
    switch (level)
    {
        case MXF_DLOG:
            fprintf(file, "Debug: ");
            break;            
        case MXF_ILOG:
            fprintf(file, "Info: ");
            break;            
        case MXF_WLOG:
            fprintf(file, "Warning: ");
            break;            
        case MXF_ELOG:
            fprintf(file, "ERROR: ");
            break;            
    };

    vfprintf(file, format, p_arg);
}

static void vlog_to_file(MXFLogLevel level, const char* format, va_list p_arg)
{
    char timeStr[128];
    const time_t t = time(NULL);
    const struct tm* gmt = gmtime(&t);

    if (level < g_mxfLogLevel)
    {
        return;
    }
    
    assert(gmt != NULL);
    assert(g_mxfFileLog != NULL);
    if (g_mxfFileLog == NULL || gmt == NULL)
    {
        return;
    }
    
    strftime(timeStr, 128, "%Y-%m-%d %H:%M:%S", gmt);
    fprintf(g_mxfFileLog, "(%s) ", timeStr);
    
    logmsg(g_mxfFileLog, level, format, p_arg);
}

static void log_to_file(MXFLogLevel level, const char* format, ...)
{
    va_list p_arg;

    va_start(p_arg, format);
    vlog_to_file(level, format, p_arg);
    va_end(p_arg);
}



void mxf_vlog_default(MXFLogLevel level, const char* format, va_list p_arg)
{
    if (level < g_mxfLogLevel)
    {
        return;
    }
    
    if (level == MXF_ELOG)
    {
        logmsg(stderr, level, format, p_arg);
    }
    else
    {
        logmsg(stdout, level, format, p_arg);
    }
}

void mxf_log_default(MXFLogLevel level, const char* format, ...)
{
    va_list p_arg;
    
    va_start(p_arg, format);
    mxf_vlog_default(level, format, p_arg);
    va_end(p_arg);
}

int mxf_log_file_open(const char* filename)
{
    if ((g_mxfFileLog = fopen(filename, "wb")) == NULL)
    {
        return 0;
    }
    
    mxf_vlog = vlog_to_file;
    mxf_log = log_to_file;
    return 1;
}

void mxf_log_file_flush()
{
    if (g_mxfFileLog != NULL)
    {
        fflush(g_mxfFileLog);
    }
}

void mxf_log_file_close()
{
    if (g_mxfFileLog != NULL)
    {
        fclose(g_mxfFileLog);
        g_mxfFileLog = NULL;
    }
}


void mxf_log_debug(const char* format, ...)
{
    va_list p_arg;
    
    va_start(p_arg, format);
    mxf_vlog(MXF_DLOG, format, p_arg);
    va_end(p_arg);
}

void mxf_log_info(const char* format, ...)
{
    va_list p_arg;
    
    va_start(p_arg, format);
    mxf_vlog(MXF_ILOG, format, p_arg);
    va_end(p_arg);
}

void mxf_log_warn(const char* format, ...)
{
    va_list p_arg;
    
    va_start(p_arg, format);
    mxf_vlog(MXF_WLOG, format, p_arg);
    va_end(p_arg);
}

void mxf_log_error(const char* format, ...)
{
    va_list p_arg;
    
    va_start(p_arg, format);
    mxf_vlog(MXF_ELOG, format, p_arg);
    va_end(p_arg);
}


