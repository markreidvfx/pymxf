/*
 * $Id: mxf_primer.c,v 1.4 2010/02/12 13:46:27 philipn Exp $
 *
 * MXF header metadata primer pack
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


static void free_primer_pack_entry_in_list(void* data);
static int entry_eq_uid(void* data, void* info);
static int entry_eq_tag(void* data, void* info);
static int add_primer_pack_entry(MXFPrimerPack* primerPack, MXFPrimerPackEntry* entry);
static int create_primer_pack_entry(MXFPrimerPack* primerPack, MXFPrimerPackEntry** entry);
static void free_primer_pack_entry(MXFPrimerPackEntry** entry);


static void free_primer_pack_entry_in_list(void* data)
{
    MXFPrimerPackEntry* entry;
    
    if (data == NULL)
    {
        return;
    }
    
    entry = (MXFPrimerPackEntry*)data;
    free_primer_pack_entry(&entry);
}

static int entry_eq_uid(void* data, void* info)
{
    assert(data != NULL && info != NULL);
    
    return mxf_equals_uid((mxfUID*)info, &((MXFPrimerPackEntry*)data)->uid);
}

static int entry_eq_tag(void* data, void* info)
{
    assert(data != NULL && info != NULL);
    
    return *(mxfLocalTag*)info == ((MXFPrimerPackEntry*)data)->localTag;
}

static int add_primer_pack_entry(MXFPrimerPack* primerPack, MXFPrimerPackEntry* entry)
{
    CHK_ORET(mxf_append_list_element(&primerPack->entries, (void*)entry));
    
    return 1;
}

static int create_primer_pack_entry(MXFPrimerPack* primerPack, MXFPrimerPackEntry** entry)
{
    MXFPrimerPackEntry* newEntry;
    
    CHK_MALLOC_ORET(newEntry, MXFPrimerPackEntry);
    memset(newEntry, 0, sizeof(MXFPrimerPackEntry));
    
    CHK_OFAIL(add_primer_pack_entry(primerPack, newEntry));
    
    *entry = newEntry;
    return 1;   
    
fail:
    free_primer_pack_entry(&newEntry);
    return 0;
}

static void free_primer_pack_entry(MXFPrimerPackEntry** entry)
{
    if (*entry == NULL)
    {
        return;
    }
    
    SAFE_FREE(entry);
}



int mxf_is_primer_pack(const mxfKey* key)
{
    return mxf_equals_key(key, &g_PrimerPack_key);
}

int mxf_create_primer_pack(MXFPrimerPack** primerPack)
{
    MXFPrimerPack* newPrimerPack;
    
    CHK_MALLOC_ORET(newPrimerPack, MXFPrimerPack);
    memset(newPrimerPack, 0, sizeof(MXFPrimerPack));
    mxf_initialise_list(&newPrimerPack->entries, free_primer_pack_entry_in_list);
    newPrimerPack->nextTag = 0xffff; /* we count down when assigning dynamic tags */
    
    *primerPack = newPrimerPack;
    return 1;   
}

void mxf_free_primer_pack(MXFPrimerPack** primerPack)
{
    if (*primerPack == NULL)
    {
        return;
    }
    
    mxf_clear_list(&(*primerPack)->entries);
    SAFE_FREE(primerPack);
}

int mxf_register_primer_entry(MXFPrimerPack* primerPack, const mxfUID* itemUID, mxfLocalTag newTag,
    mxfLocalTag* assignedTag)
{
    MXFPrimerPackEntry* newEntry;
    mxfLocalTag tag;
    void* result;
    
    /* if already exists, then return already assigned tag */
    if ((result = mxf_find_list_element(&primerPack->entries, (void*)itemUID, entry_eq_uid)) != NULL)
    {
        *assignedTag = ((MXFPrimerPackEntry*)result)->localTag;
    }
    /* use the tag */
    else if (newTag != g_Null_LocalTag)
    {
        if (mxf_find_list_element(&primerPack->entries, (void*)&newTag, entry_eq_tag) != NULL)
        {
            mxf_log_error("Local tag %x already in use" LOG_LOC_FORMAT, newTag, LOG_LOC_PARAMS);
            return 0;
        }

        CHK_ORET(create_primer_pack_entry(primerPack, &newEntry));
        newEntry->localTag = newTag;
        newEntry->uid = *itemUID;
        *assignedTag = newTag;
    }
    /* create a new entry with new tag */
    else
    {
        CHK_ORET(mxf_create_item_tag(primerPack, &tag));
        CHK_ORET(create_primer_pack_entry(primerPack, &newEntry));
        newEntry->localTag = tag;
        newEntry->uid = *itemUID;
        *assignedTag = tag;
    }
    
    return 1;
}


int mxf_get_item_key(MXFPrimerPack* primerPack, mxfLocalTag localTag, mxfKey* key)
{
    void* result;

    if ((result = mxf_find_list_element(&primerPack->entries, (void*)&localTag, entry_eq_tag)) != NULL)
    {
        *key = ((MXFPrimerPackEntry*)result)->uid;
        return 1;
    }

    return 0;
}

int mxf_get_item_tag(MXFPrimerPack* primerPack, const mxfKey* key, mxfLocalTag* localTag)
{
    void* result;

    if ((result = mxf_find_list_element(&primerPack->entries, (void*)(const mxfUID*)key, entry_eq_uid)) != NULL)
    {
        *localTag = ((MXFPrimerPackEntry*)result)->localTag;
        return 1;
    }

    return 0;
}

int mxf_create_item_tag(MXFPrimerPack* primerPack, mxfLocalTag* localTag)
{
    mxfLocalTag tag;
    
    for(;;)
    {
        tag = primerPack->nextTag--;
        if (tag < 0x8000)
        {
            mxf_log_error("Could not create a unique tag - reached the end of the allowed dynamic tag values" LOG_LOC_FORMAT, LOG_LOC_PARAMS);
            return 0;
        }
        if (mxf_find_list_element(&primerPack->entries, (void*)&tag, entry_eq_tag) == NULL)
        {
            break;
        }
    }
    
    *localTag = tag;
    return 1;
}


int mxf_read_primer_pack(MXFFile* mxfFile, MXFPrimerPack** primerPack)
{
    MXFPrimerPack* newPrimerPack = NULL;
    uint32_t itemLength;
    uint32_t numberOfItems;
    MXFPrimerPackEntry* newEntry = NULL;
    mxfLocalTag localTag;
    mxfUID uid;
    uint32_t i;
    
    CHK_ORET(mxf_create_primer_pack(&newPrimerPack));
    
    CHK_OFAIL(mxf_read_array_header(mxfFile, &numberOfItems, &itemLength));
        
    for (i = 0; i < numberOfItems; i++)
    {
        CHK_OFAIL(mxf_read_local_tag(mxfFile, &localTag));
        CHK_OFAIL(mxf_read_uid(mxfFile, &uid));
        
        CHK_OFAIL(create_primer_pack_entry(newPrimerPack, &newEntry));
        newEntry->localTag = localTag;
        newEntry->uid = uid;
    }
    
    *primerPack = newPrimerPack;
    return 1;
    
fail:
    mxf_free_primer_pack(&newPrimerPack);
    return 0;
}

/* Note: keep this in sync with mxf_get_primer_pack_size */
int mxf_write_primer_pack(MXFFile* mxfFile, MXFPrimerPack* primerPack)
{
    MXFListIterator iter;
    uint32_t numberOfItems = mxf_get_list_length(&primerPack->entries);

    CHK_ORET(mxf_write_kl(mxfFile, &g_PrimerPack_key, 8 + 18 * numberOfItems));

    CHK_ORET(mxf_write_array_header(mxfFile, numberOfItems, 18));
        
    mxf_initialise_list_iter(&iter, &primerPack->entries);
    while (mxf_next_list_iter_element(&iter))
    {
        MXFPrimerPackEntry* entry = (MXFPrimerPackEntry*)mxf_get_iter_element(&iter);
        
        CHK_ORET(mxf_write_local_tag(mxfFile, entry->localTag));
        CHK_ORET(mxf_write_uid(mxfFile, &entry->uid));
    }
    
    return 1;
}

/* Note: keep this in sync with mxf_write_primer_pack */
void mxf_get_primer_pack_size(MXFFile* mxfFile, MXFPrimerPack* primerPack, uint64_t* size)
{
    uint32_t numberOfItems = mxf_get_list_length(&primerPack->entries);

    *size = mxfKey_extlen + mxf_get_llen(mxfFile, 8 + 18 * numberOfItems) +
        8 + 18 * numberOfItems;
}


