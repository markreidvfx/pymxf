/*
 * $Id: mxf_page_file.h,v 1.2 2008/05/07 15:22:23 philipn Exp $
 *
 * 
 *
 * Copyright (C) 2007  Philip de Nier <philipn@users.sourceforge.net>
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
 
#ifndef __MXF_PAGE_FILE_H__
#define __MXF_PAGE_FILE_H__


#ifdef __cplusplus
extern "C" 
{
#endif


#include <mxf/mxf_file.h>


typedef struct MXFPageFile MXFPageFile;

int mxf_page_file_open_new(const char* filenameTemplate, int64_t pageSize, MXFPageFile** mxfPageFile);
int mxf_page_file_open_read(const char* filenameTemplate, MXFPageFile** mxfPageFile);
int mxf_page_file_open_modify(const char* filenameTemplate, int64_t pageSize, MXFPageFile** mxfPageFile);

MXFFile* mxf_page_file_get_file(MXFPageFile* mxfPageFile);

int64_t mxf_page_file_get_page_size(MXFPageFile* mxfPageFile);
int mxf_page_file_is_page_filename(const char* filename);

/* truncate the file from the front, setting the file sizes to zero; 
the file can be continued to be read but cannot seek backwards or be reopened */ 
int mxf_page_file_forward_truncate(MXFPageFile* mxfPageFile);

int mxf_page_file_remove(const char* filenameTemplate);


#ifdef __cplusplus
}
#endif


#endif

