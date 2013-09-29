/*
 * $Id: mxf_list.h,v 1.2 2007/09/11 13:24:54 stuart_hc Exp $
 *
 * General purpose linked list
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
 
#ifndef __MXF_LIST_H__
#define __MXF_LIST_H__


#ifdef __cplusplus
extern "C" 
{
#endif


typedef void (*free_func_type)(void* data);
typedef int (*eq_func_type)(void* data, void* info);

typedef struct _MXFListElement
{
    struct _MXFListElement* next;
    void* data;
} MXFListElement;

typedef struct
{
    MXFListElement* elements;
    MXFListElement* lastElement;
    long len;
    free_func_type freeFunc;
} MXFList;

typedef struct
{
    MXFListElement* nextElement;
    void* data;
    long index;
} MXFListIterator;


int mxf_create_list(MXFList** list, free_func_type freeFunc);
void mxf_free_list(MXFList** list);
void mxf_initialise_list(MXFList* list, free_func_type freeFunc);
void mxf_clear_list(MXFList* list);

int mxf_append_list_element(MXFList* list, void* data);
int mxf_prepend_list_element(MXFList* list, void* data);
int mxf_insert_list_element(MXFList* list, long index, int before, void* data);
long mxf_get_list_length(MXFList* list);

void* mxf_find_list_element(const MXFList* list, void* info, eq_func_type eqFunc);
void* mxf_remove_list_element(MXFList* list, void* info, eq_func_type eqFunc);

void* mxf_get_list_element(MXFList* list, long index);

void* mxf_get_first_list_element(MXFList* list);
void* mxf_get_last_list_element(MXFList* list);

void mxf_initialise_list_iter(MXFListIterator* iter, const MXFList* list);
void mxf_initialise_list_iter_at(MXFListIterator* iter, const MXFList* list, long index);
int mxf_next_list_iter_element(MXFListIterator* iter);
void* mxf_get_iter_element(MXFListIterator* iter);
long mxf_get_list_iter_index(MXFListIterator* iter);


#ifdef __cplusplus
}
#endif


#endif


