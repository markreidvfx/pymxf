/*
 * $Id: mxf_essence_container.c,v 1.3 2009/10/22 14:22:34 john_f Exp $
 *
 * MXF file body essence container functions
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


int mxf_is_gc_essence_element(const mxfKey* key)
{
    /* generic container picture/sound/data element */
    if (key->octet0 == 0x06 &&
        key->octet1 == 0x0e &&
        key->octet2 == 0x2b &&
        key->octet3 == 0x34 &&
        key->octet4 == 0x01 &&
        key->octet6 == 0x01 &&
        key->octet8 == 0x0d &&
        key->octet9 == 0x01 &&
        key->octet10 == 0x03 &&
        key->octet11 == 0x01)
    {
        return 1;
    }
    /* generic container system element */
    else if (key->octet0 == 0x06 &&
        key->octet1 == 0x0e &&
        key->octet2 == 0x2b &&
        key->octet3 == 0x34 &&
        key->octet4 == 0x02 &&
        key->octet6 == 0x01 &&
        key->octet8 == 0x0d &&
        key->octet9 == 0x01 &&
        key->octet10 == 0x03 &&
        key->octet11 == 0x01)
    {
        return 1;
    }
    
    return 0;
}

    
    
static int create_essence_element(const mxfKey* key, uint8_t llen, MXFEssenceElement** essenceElement)
{
    MXFEssenceElement* newEssenceElement = NULL;
    
    CHK_MALLOC_ORET(newEssenceElement, MXFEssenceElement);
    memset(newEssenceElement, 0, sizeof(MXFEssenceElement));
    newEssenceElement->key = *key;
    newEssenceElement->llen = llen;
    
    *essenceElement = newEssenceElement;
    return 1;
}

static void free_essence_element(MXFEssenceElement** essenceElement)
{
    if (*essenceElement == NULL)
    {
        return;
    }
    
    SAFE_FREE(essenceElement);
}

    

int mxf_open_essence_element_write(MXFFile* mxfFile, const mxfKey* key, uint8_t llen, uint64_t len,
    MXFEssenceElement** essenceElement)
{
    MXFEssenceElement* newEssenceElement = NULL;
    int64_t filePos;
    
    CHK_ORET(create_essence_element(key, llen, &newEssenceElement));
    
    CHK_OFAIL((filePos = mxf_file_tell(mxfFile)) >= 0);
    newEssenceElement->startFilePos = filePos; 
    newEssenceElement->currentFilePos = newEssenceElement->startFilePos;
    CHK_OFAIL(mxf_write_k(mxfFile, key));
    CHK_OFAIL(mxf_write_fixed_l(mxfFile, llen, len));

    *essenceElement = newEssenceElement;
    return 1;
    
fail:
    SAFE_FREE(&newEssenceElement);
    return 0;
}

int mxf_write_essence_element_data(MXFFile* mxfFile, MXFEssenceElement* essenceElement,
    const uint8_t* data, uint32_t len)
{
    uint64_t numWritten = mxf_file_write(mxfFile, data, len);
    essenceElement->totalLen += numWritten;
    essenceElement->currentFilePos += numWritten;
    
    if (numWritten != len)
    {
        return 0;
    }
    return 1;
}
        
int mxf_finalize_essence_element_write(MXFFile* mxfFile, MXFEssenceElement* essenceElement)
{
    int64_t filePos;

    assert(essenceElement != NULL);

    CHK_ORET((filePos = mxf_file_tell(mxfFile)) >= 0);

    CHK_ORET(mxf_file_seek(mxfFile, essenceElement->startFilePos + 16, SEEK_SET));
    CHK_ORET(mxf_write_fixed_l(mxfFile, essenceElement->llen, essenceElement->totalLen));
    
    CHK_ORET(mxf_file_seek(mxfFile, filePos, SEEK_SET));
    
    return 1;
}


int mxf_open_essence_element_read(MXFFile* mxfFile, const mxfKey* key, uint8_t llen, uint64_t len, 
    MXFEssenceElement** essenceElement)
{
    MXFEssenceElement* newEssenceElement = NULL;
    int64_t filePos;
    
    CHK_ORET(create_essence_element(key, llen, &newEssenceElement));
    newEssenceElement->totalLen = len;
    
    CHK_OFAIL((filePos = mxf_file_tell(mxfFile)) >= 0);
    newEssenceElement->startFilePos = filePos;
    newEssenceElement->currentFilePos = newEssenceElement->startFilePos;
    
    *essenceElement = newEssenceElement;
    return 1;
    
fail:
    SAFE_FREE(&newEssenceElement);
    return 0;
}

int mxf_read_essence_element_data(MXFFile* mxfFile, MXFEssenceElement* essenceElement,
    uint32_t len, uint8_t* data, uint32_t* numRead)
{
    uint32_t actualNumRead = 0;
    uint32_t actualLen = len;
    
    if ((uint64_t)(essenceElement->currentFilePos - essenceElement->startFilePos) >= essenceElement->totalLen)
    {
        *numRead = 0;
        return 1;
    }
    
    if (actualLen + (uint64_t)(essenceElement->currentFilePos - essenceElement->startFilePos) > essenceElement->totalLen)
    {
        actualLen = (uint32_t)(essenceElement->totalLen - (uint64_t)(essenceElement->currentFilePos - essenceElement->startFilePos));
    }
    
    actualNumRead = (uint32_t)mxf_file_read(mxfFile, data, actualLen);
    essenceElement->currentFilePos += actualNumRead;
    CHK_ORET(actualNumRead == actualLen);
    
    *numRead = actualNumRead;
    return 1;
}

void mxf_close_essence_element(MXFEssenceElement** essenceElement)
{
    free_essence_element(essenceElement);
}

uint64_t mxf_get_essence_element_size(MXFEssenceElement* essenceElement)
{
    return essenceElement->totalLen;
}


uint32_t mxf_get_track_number(const mxfKey* essenceElementKey)
{
    return (((uint32_t)essenceElementKey->octet12 & 0xff) << 24) |
           (((uint32_t)essenceElementKey->octet13 & 0xff) << 16) |
           (((uint32_t)essenceElementKey->octet14 & 0xff) << 8) |
           ((uint32_t)essenceElementKey->octet15 & 0xff);    
}

uint8_t mxf_get_essence_element_item_type(uint32_t trackNumber)
{
    return (uint8_t)((trackNumber & 0xff000000) >> 24);
}

uint8_t mxf_get_essence_element_count(uint32_t trackNumber)
{
    return (uint8_t)((trackNumber & 0x00ff0000) >> 16);
}

uint8_t mxf_get_essence_element_type(uint32_t trackNumber)
{
    return (uint8_t)((trackNumber & 0x0000ff00) >> 8);
}

uint8_t mxf_get_essence_element_number(uint32_t trackNumber)
{
    return (uint8_t)((trackNumber & 0x000000ff));
}

