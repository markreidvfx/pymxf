/*
 * $Id: mxf_list.c,v 1.2 2007/09/11 13:24:55 stuart_hc Exp $
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
 
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mxf/mxf.h>



int mxf_create_list(MXFList** list, free_func_type freeFunc)
{
    MXFList* newList;
    
    CHK_MALLOC_ORET(newList, MXFList);
    mxf_initialise_list(newList, freeFunc);

    *list = newList;
    return 1;
}

void mxf_free_list(MXFList** list)
{
    if (*list == NULL)
    {
        return;
    }
    
    mxf_clear_list(*list);
    SAFE_FREE(list);
}

void mxf_initialise_list(MXFList* list, free_func_type freeFunc)
{
    memset(list, 0, sizeof(MXFList));
    list->freeFunc = freeFunc;
}

void mxf_clear_list(MXFList* list)
{
    MXFListElement* element;
    MXFListElement* nextElement;
    
    if (list == NULL)
    {
        return;
    }
    
    element = list->elements;
    while (element != NULL)
    {
        nextElement = element->next;
        
        if (list->freeFunc != NULL)
        {
            list->freeFunc(element->data);
        }
        SAFE_FREE(&element);
        
        element = nextElement;
    }
    
    list->elements = NULL;
    list->lastElement = NULL;
    list->len = 0;
}

int mxf_append_list_element(MXFList* list, void* data)
{
    MXFListElement* newElement;
    
    CHK_MALLOC_ORET(newElement, MXFListElement);
    memset(newElement, 0, sizeof(MXFListElement));
    newElement->data = data;

    if (list->elements == NULL)
    {
        list->elements = newElement;
    }
    else
    {
        list->lastElement->next = newElement;
    }
    list->lastElement = newElement;
    
    list->len++;
    return 1;
}

int mxf_prepend_list_element(MXFList* list, void* data)
{
    MXFListElement* newElement;
    
    CHK_MALLOC_ORET(newElement, MXFListElement);
    memset(newElement, 0, sizeof(MXFListElement));
    newElement->data = data;

    if (list->elements == NULL)
    {
        list->elements = newElement;
        list->lastElement = newElement;
    }
    else
    {
        newElement->next = list->elements;
        list->elements = newElement;
    }
    
    list->len++;
    return 1;
}

int mxf_insert_list_element(MXFList* list, long index, int before, void* data)
{
    MXFListElement* newElement;
    MXFListElement* nextElement;
    MXFListElement* prevElement;
    long currentIndex;

    
    /* create new element */
    CHK_MALLOC_ORET(newElement, MXFListElement);
    memset(newElement, 0, sizeof(MXFListElement));
    newElement->data = data;

    /* special case when list is empty */
    if (list->elements == NULL)
    {
        list->elements = newElement;
        list->lastElement = newElement;
        list->len++;
        return 1;
    }
    
    nextElement = list->elements;
    prevElement = NULL;
    currentIndex = 0;
    if (before)
    {
        /* move to index position */
        while (currentIndex < index && nextElement != NULL)
        {
            prevElement = nextElement;
            nextElement = nextElement->next;
            currentIndex++;
        }
        if (currentIndex != index)
        {
            goto fail;
        }
    }
    else
    {
        /* move to after index position */
        while (currentIndex <= index && nextElement != NULL)
        {
            prevElement = nextElement;
            nextElement = nextElement->next;
            currentIndex++;
        }
        if (currentIndex != index + 1)
        {
            goto fail;
        }
    }
    
    /* insert element */
    if (prevElement == NULL)
    {
        list->elements = newElement;
    }
    else
    {
        prevElement->next = newElement;
    }
    newElement->next = nextElement;
    if (newElement->next == NULL)
    {
        list->lastElement = newElement;
    }
    
    list->len++;
    return 1;
    
fail:
    SAFE_FREE(&newElement);
    return 0;
}

long mxf_get_list_length(MXFList* list)
{
    return list->len;
}

void* mxf_find_list_element(const MXFList* list, void* info, eq_func_type eqFunc)
{
    void* result = NULL;
    MXFListElement* element = list->elements;
    
    while (element != NULL)
    {
        if (eqFunc(element->data, info))
        {
            result = element->data;
            break;
        }
        element = element->next;
    }
    
    return result;
}

void* mxf_remove_list_element(MXFList* list, void* info, eq_func_type eqFunc)
{
    void* result = NULL;
    MXFListElement* element = list->elements;
    MXFListElement* prevElement = NULL;
    
    while (element != NULL)
    {
        if (eqFunc(element->data, info))
        {
            result = element->data;
            if (prevElement == NULL)
            {
                list->elements = element->next;
                if (list->elements == NULL)
                {
                    list->lastElement = list->elements;
                }
            }
            else
            {
                prevElement->next = element->next;
                if (prevElement->next == NULL)
                {
                    list->lastElement = prevElement;
                }
            }
            SAFE_FREE(&element); /* must free the wrapper element because we only return the data */
            list->len--;
            break;
        }
        prevElement = element;
        element = element->next;
    }

    
    return result;
}

void* mxf_get_list_element(MXFList* list, long index)
{
    long currentIndex = 0;
    MXFListElement* element = list->elements;
    
    if (index < 0 || index > list->len - 1)
    {
        return NULL;
    }
    
    if (index == 0)
    {
        return mxf_get_first_list_element(list);
    }
    if (index == list->len - 1)
    {
        return mxf_get_last_list_element(list);
    }
    
    while (currentIndex != index && element != NULL)
    {
        currentIndex++;
        element = element->next;
    }
    
    if (currentIndex != index || element == NULL)
    {
        return NULL;
    }
    
    return element->data;
}

void* mxf_get_first_list_element(MXFList* list)
{
    if (list->elements == NULL)
    {
        return NULL;
    }
    return list->elements->data;
}

void* mxf_get_last_list_element(MXFList* list)
{
    if (list->lastElement == NULL)
    {
        return NULL;
    }
    return list->lastElement->data;
}

void mxf_initialise_list_iter(MXFListIterator* iter, const MXFList* list)
{
    iter->nextElement = list->elements;
    iter->data = NULL;
    iter->index = -1;
}

void mxf_initialise_list_iter_at(MXFListIterator* iter, const MXFList* list, long index)
{
    if (index < 0)
    {
        mxf_initialise_list_iter(iter, list);
    }
    else
    {
        iter->nextElement = list->elements;
        iter->data = NULL;
        iter->index = 0;
    
        while (iter->index != index && iter->nextElement != NULL)
        {
            iter->index++;
            iter->nextElement = iter->nextElement->next;
        }
    }
}

int mxf_next_list_iter_element(MXFListIterator* iter)
{
    if (iter->nextElement != NULL)
    {
        iter->data = iter->nextElement->data;
        iter->nextElement = iter->nextElement->next;
    }
    else
    {
        iter->data = NULL;
    }
    
    if (iter->data != NULL)
    {
        iter->index++;
    }
    else
    {
        iter->index = -1;
    }
    
    return iter->data != NULL;
}

void* mxf_get_iter_element(MXFListIterator* iter)
{
    return iter->data;
}

long mxf_get_list_iter_index(MXFListIterator* iter)
{
    return iter->index;
}

