/*
 * $Id: mxf_header_metadata.c,v 1.5 2010/06/25 13:59:02 philipn Exp $
 *
 * MXF header metadata
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <mxf/mxf.h>


static void free_metadata_item_value(MXFMetadataItem* item)
{
    SAFE_FREE(&item->value);
    item->length = 0;
}

static void free_metadata_set_in_list(void* data)
{
    MXFMetadataSet* set;
    
    if (data == NULL)
    {
        return;
    }
    
    set = (MXFMetadataSet*)data;
    mxf_free_set(&set);
}

static void free_metadata_item_in_list(void* data)
{
    MXFMetadataItem* item;
    
    if (data == NULL)
    {
        return;
    }
    
    item = (MXFMetadataItem*)data;
    mxf_free_item(&item);
}

static int set_eq_instanceuid(void* data, void* info)
{
    assert(data != NULL && info != NULL);
    
    return mxf_equals_uuid((mxfUUID*)info, &((MXFMetadataSet*)data)->instanceUID);
}

static int item_eq_key(void* data, void* info)
{
    assert(data != NULL && info != NULL);
    
    return mxf_equals_key((mxfKey*)info, &((MXFMetadataItem*)data)->key);
}

static int eq_pointer(void* data, void* info)
{
    assert(data != NULL && info != NULL);
    
    return data == info;
}

static int get_or_create_set_item(MXFHeaderMetadata* headerMetadata, MXFMetadataSet* set,
    const mxfKey* itemKey, MXFMetadataItem** item)
{
    MXFMetadataItem* resultItem = NULL;
    mxfLocalTag tag;
    MXFItemDef* itemDef = NULL;
    
    /* check if item already exists */
    if (!mxf_get_item(set, itemKey, &resultItem))
    {
        /* check if item registered in primer */
        if (!mxf_get_item_tag(headerMetadata->primerPack, itemKey, &tag))
        {
            /* register item in primer */
            CHK_ORET(mxf_find_item_def(headerMetadata->dataModel, itemKey, &itemDef));
            CHK_ORET(mxf_register_primer_entry(headerMetadata->primerPack, (const mxfUID*)itemKey, 
                itemDef->localTag, &tag));
        }
        /* create new item */
        CHK_ORET(mxf_create_item(set, itemKey, tag, &resultItem));
    }
    
    *item = resultItem;
    return 1;    
}

static int create_empty_set(const mxfKey* key, MXFMetadataSet** set)
{
    MXFMetadataSet* newSet;
    
    CHK_MALLOC_ORET(newSet, MXFMetadataSet);
    memset(newSet, 0, sizeof(MXFMetadataSet));
    newSet->key = *key;
    newSet->instanceUID = g_Null_UUID;
    mxf_initialise_list(&newSet->items, free_metadata_item_in_list);

    *set = newSet;
    return 1;
}

static int add_item(MXFMetadataSet* set, MXFMetadataItem* item)
{
    MXFMetadataItem* removedItem;
    
    /* if item already attached to set, then removed it first */
    if (item->set != NULL)
    {
        CHK_ORET(mxf_remove_item(item->set, &item->key, &removedItem));
    }
    
    CHK_ORET(mxf_append_list_element(&set->items, (void*)item));
    item->set = set;
    
    return 1;
}




int mxf_is_header_metadata(const mxfKey* key)
{
    return mxf_is_primer_pack(key);
}


int mxf_create_header_metadata(MXFHeaderMetadata** headerMetadata, MXFDataModel* dataModel)
{
    MXFHeaderMetadata* newHeaderMetadata;
    
    CHK_MALLOC_ORET(newHeaderMetadata, MXFHeaderMetadata);
    memset(newHeaderMetadata, 0, sizeof(MXFHeaderMetadata));
    newHeaderMetadata->dataModel = dataModel;
    mxf_initialise_list(&newHeaderMetadata->sets, free_metadata_set_in_list);
    CHK_OFAIL(mxf_create_primer_pack(&newHeaderMetadata->primerPack));
    
    *headerMetadata = newHeaderMetadata;
    return 1;   
    
fail:
    mxf_free_header_metadata(&newHeaderMetadata);
    return 0;
}

int mxf_create_set(MXFHeaderMetadata* headerMetadata, const mxfKey* key, MXFMetadataSet** set)
{
    MXFMetadataSet* newSet;
    mxfUUID uuid;
    
    CHK_ORET(create_empty_set(key, &newSet));
    
    mxf_generate_uuid(&uuid);
    newSet->instanceUID = uuid;

    CHK_OFAIL(mxf_add_set(headerMetadata, newSet));

    CHK_OFAIL(mxf_set_uuid_item(newSet, &MXF_ITEM_K(InterchangeObject, InstanceUID), &uuid));
    
    *set = newSet;
    return 1;
    
fail:
    mxf_free_set(&newSet);
    return 0;
}

int mxf_create_item(MXFMetadataSet* set, const mxfKey* key, mxfLocalTag tag, MXFMetadataItem** item)
{
    MXFMetadataItem* newItem;
    
    CHK_MALLOC_ORET(newItem, MXFMetadataItem);
    memset(newItem, 0, sizeof(MXFMetadataItem));
    newItem->tag = tag;
    newItem->isPersistent = 0;
    newItem->key = *key;

    CHK_OFAIL(add_item(set, newItem));
    
    *item = newItem;
    return 1;

fail:
    mxf_free_item(&newItem);
    return 0;
}

void mxf_free_header_metadata(MXFHeaderMetadata** headerMetadata)
{
    if (*headerMetadata == NULL)
    {
        return;
    }
    
    mxf_clear_list(&(*headerMetadata)->sets);
    mxf_free_primer_pack(&(*headerMetadata)->primerPack);
    SAFE_FREE(headerMetadata);
}

void mxf_free_set(MXFMetadataSet** set)
{
    if (*set == NULL)
    {
        return;
    }
    
    mxf_clear_list(&(*set)->items);
    SAFE_FREE(set);
}

void mxf_free_item(MXFMetadataItem** item)
{
    if (*item == NULL)
    {
        return;
    }
    
    free_metadata_item_value(*item);
    SAFE_FREE(item);
}


int mxf_add_set(MXFHeaderMetadata* headerMetadata, MXFMetadataSet* set)
{
    /* sets must have an instanceUID 
       Note however that it is not neccessary to have an instanceUID __item__ when
       calling this function. See mxf_create_set() for example */
    CHK_ORET(!mxf_equals_uuid(&set->instanceUID, &g_Null_UUID));
    
    /* if set already attached to header metadata, then removed it first */
    if (set->headerMetadata != NULL)
    {
        CHK_ORET(mxf_remove_set(set->headerMetadata, set));
    }
    
    CHK_ORET(mxf_append_list_element(&headerMetadata->sets, (void*)set));
    set->headerMetadata = headerMetadata;

    return 1;
}


void mxf_set_fixed_set_space_allocation(MXFMetadataSet* set, uint64_t size)
{
    set->fixedSpaceAllocation = size;
}


int mxf_register_item(MXFHeaderMetadata* headerMetadata, const mxfKey* key)
{
    mxfLocalTag tag;
    MXFItemDef* itemDef;
    
    /* register item in primer */
    CHK_ORET(mxf_find_item_def(headerMetadata->dataModel, key, &itemDef));
    CHK_ORET(mxf_register_primer_entry(headerMetadata->primerPack, (const mxfUID*)key, 
        itemDef->localTag, &tag));
    
    return 1;
}

int mxf_register_set_items(MXFHeaderMetadata* headerMetadata, const mxfKey* key)
{
    MXFSetDef* setDef;
    MXFItemDef* itemDef;
    MXFListIterator iter;
    mxfLocalTag tag;
    
    CHK_ORET(mxf_find_set_def(headerMetadata->dataModel, key, &setDef));
    
    /* go though the set def and parents and register the items */
    while (setDef != NULL)
    {
        mxf_initialise_list_iter(&iter, &setDef->itemDefs);
        while (mxf_next_list_iter_element(&iter))
        {
            itemDef = (MXFItemDef*)mxf_get_iter_element(&iter);
            
            CHK_ORET(mxf_register_primer_entry(headerMetadata->primerPack, (const mxfUID*)&itemDef->key, 
                itemDef->localTag, &tag));
        }
        
        setDef = setDef->parentSetDef;
    }
    
    return 1;
}

int mxf_remove_set(MXFHeaderMetadata* headerMetadata, MXFMetadataSet* set)
{
    void* result;
    
    if ((result = mxf_remove_list_element(&headerMetadata->sets, (void*)set, eq_pointer)) != NULL)
    {
        set->headerMetadata = NULL;
        return 1;
    }
    
    return 0;
}

int mxf_remove_item(MXFMetadataSet* set, const mxfKey* itemKey, MXFMetadataItem** item)
{
    void* result;
    
    if ((result = mxf_remove_list_element(&set->items, (void*)itemKey, item_eq_key)) != NULL)
    {
        *item = (MXFMetadataItem*)result;
        (*item)->set = NULL;
        return 1;
    }
    
    return 0;
}

int mxf_find_set_by_key(MXFHeaderMetadata* headerMetadata, const mxfKey* key, MXFList** setList)
{
    MXFListIterator iter;
    MXFList* newList = NULL;

    CHK_ORET(mxf_create_list(&newList, NULL)); /* free func == NULL because newList doesn't own the data */
    
    mxf_initialise_list_iter(&iter, &headerMetadata->sets);
    while (mxf_next_list_iter_element(&iter))
    {
        MXFMetadataSet* set = (MXFMetadataSet*)mxf_get_iter_element(&iter);
        if (mxf_equals_key(key, &set->key))
        {
            CHK_OFAIL(mxf_append_list_element(newList, (void*)set));
        }
    }

    *setList = newList;    
    return 1;
    
fail:
    mxf_free_list(&newList);
    return 0;
}

int mxf_find_singular_set_by_key(MXFHeaderMetadata* headerMetadata, const mxfKey* key, MXFMetadataSet** set)
{
    MXFList* setList = NULL;

    if (!mxf_find_set_by_key(headerMetadata, key, &setList))
    {
        return 0;
    }
    if (mxf_get_list_length(setList) == 0)
    {
        mxf_free_list(&setList);
        return 0;
    }
    
    CHK_OFAIL(mxf_get_list_length(setList) == 1);

    *set = (MXFMetadataSet*)mxf_get_first_list_element(setList);
    
    mxf_free_list(&setList);
    return 1;
    
fail:
    mxf_free_list(&setList);
    return 0;
}

int mxf_get_item(MXFMetadataSet* set, const mxfKey* key, MXFMetadataItem** resultItem)
{
    void* result;
    
    if ((result = mxf_find_list_element(&set->items, (void*)key, item_eq_key)) != NULL)
    {
        *resultItem = (MXFMetadataItem*)result;
        return 1;
    }
    
    return 0;
}

int mxf_have_item(MXFMetadataSet* set, const mxfKey* key)
{
    MXFMetadataItem* item;
    return mxf_get_item(set, key, &item);
}


int mxf_set_is_subclass_of(MXFMetadataSet* set, const mxfKey* parentSetKey)
{
    CHK_ORET(set->headerMetadata != NULL && set->headerMetadata->dataModel != NULL);
    
    return mxf_is_subclass_of(set->headerMetadata->dataModel, &set->key, parentSetKey);
}


int mxf_clone_set(MXFMetadataSet* fromSet, MXFHeaderMetadata* toHeaderMetadata, MXFMetadataSet** toSet)
{
    MXFMetadataSet* clonedSet = NULL;
    MXFSetDef* fromSetDef;
    MXFSetDef* toSetDef;
    MXFListIterator fromItemIter;
    MXFMetadataItem* toItem = NULL;
    MXFItemDef* toItemDef = NULL;
    MXFMetadataSet* fromRefSet = NULL;
    MXFMetadataSet* toRefSet = NULL;
    MXFArrayItemIterator fromArrayIter;
    uint32_t count;
    uint8_t* toUUIDElement;
    uint8_t* fromUUIDElement;
    uint32_t fromUUIDElementLen;
    MXFListIterator fromSetsIter;

    CHK_ORET(mxf_find_set_def(fromSet->headerMetadata->dataModel, &fromSet->key, &fromSetDef));
    CHK_ORET(mxf_clone_set_def(fromSet->headerMetadata->dataModel, fromSetDef,
                               toHeaderMetadata->dataModel, &toSetDef));
    CHK_ORET(mxf_create_set(toHeaderMetadata, &fromSet->key, &clonedSet));

    mxf_initialise_list_iter(&fromSetsIter, &fromSet->headerMetadata->sets);

    mxf_initialise_list_iter(&fromItemIter, &fromSet->items);
    while (mxf_next_list_iter_element(&fromItemIter))
    {
        MXFMetadataItem* fromItem = (MXFMetadataItem*)mxf_get_iter_element(&fromItemIter);
        
        CHK_OFAIL(mxf_find_item_def_in_set_def(&fromItem->key, toSetDef, &toItemDef));
        
        if (toItemDef->typeId == MXF_WEAKREFARRAY_TYPE ||
            toItemDef->typeId == MXF_WEAKREFBATCH_TYPE ||
            toItemDef->typeId == MXF_WEAKREF_TYPE)
        {
            mxf_log_error("Not cloning item because weak reference clones are not yet supported" LOG_LOC_FORMAT, LOG_LOC_PARAMS);
            continue;
        }
        else if (toItemDef->typeId == MXF_STRONGREFARRAY_TYPE ||
                 toItemDef->typeId == MXF_STRONGREFBATCH_TYPE)
        {
            CHK_OFAIL(mxf_get_array_item_count(fromSet, &fromItem->key, &count));
            CHK_OFAIL(mxf_alloc_array_item_elements(clonedSet, &fromItem->key, mxfUUID_extlen, count, &toUUIDElement));

            CHK_OFAIL(mxf_initialise_array_item_iterator(fromSet, &fromItem->key, &fromArrayIter));
            while (mxf_next_array_item_element(&fromArrayIter, &fromUUIDElement, &fromUUIDElementLen))
            {
                CHK_OFAIL(mxf_get_strongref_s(fromSet->headerMetadata, &fromSetsIter, fromUUIDElement, &fromRefSet));
                CHK_OFAIL(mxf_clone_set(fromRefSet, toHeaderMetadata, &toRefSet));
                mxf_set_strongref(toRefSet, toUUIDElement);
                
                toUUIDElement += mxfUUID_extlen;
            }
        }
        else if (toItemDef->typeId == MXF_STRONGREF_TYPE)
        {
            CHK_OFAIL(mxf_get_strongref_s(fromSet->headerMetadata, &fromSetsIter, fromItem->value, &fromRefSet));
            CHK_OFAIL(mxf_clone_set(fromRefSet, toHeaderMetadata, &toRefSet));
            CHK_OFAIL(mxf_set_strongref_item(clonedSet, &fromItem->key, toRefSet));
        }
        else if (!mxf_equals_key(&toItemDef->key, &MXF_ITEM_K(InterchangeObject, InstanceUID)) &&
                 !mxf_equals_key(&toItemDef->key, &MXF_ITEM_K(InterchangeObject, GenerationUID)))
        {
            CHK_OFAIL(get_or_create_set_item(toHeaderMetadata, clonedSet, &fromItem->key, &toItem));
            CHK_OFAIL(mxf_set_item_value(toItem, fromItem->value, fromItem->length));
        }
    }

    *toSet = clonedSet;
    return 1;

fail:
    mxf_remove_set(toHeaderMetadata, clonedSet);
    return 0;
}


int mxf_read_header_metadata(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata,
    uint64_t headerByteCount, const mxfKey* pkey, uint8_t pllen, uint64_t plen)
{
    return mxf_read_filtered_header_metadata(mxfFile, NULL, headerMetadata, headerByteCount, pkey,
        pllen, plen);
}

/* Read primer pack followed by sets. The inputs pkey, pllen, plen must 
   correspond to that for the primer pack */
int mxf_read_filtered_header_metadata(MXFFile* mxfFile, MXFReadFilter* filter, 
    MXFHeaderMetadata* headerMetadata, uint64_t headerByteCount, 
    const mxfKey* pkey, uint8_t pllen, uint64_t plen)
{
    mxfKey key;
    uint8_t llen;
    uint64_t len;    
    int skip = 0;
    MXFMetadataSet* newSet = NULL;
    uint64_t count = 0;
    int result;

    CHK_ORET(headerByteCount != 0);
    
    /* check that input pkey is as expected, and assume pllen and plen are also ok */
    CHK_ORET(mxf_is_primer_pack(pkey));  
    count += mxfKey_extlen + pllen;
    
    if (headerMetadata->primerPack != NULL)
    {
        mxf_free_primer_pack(&headerMetadata->primerPack);
    }
    CHK_ORET(mxf_read_primer_pack(mxfFile, &headerMetadata->primerPack));
    count += plen;
    
    while (count < headerByteCount)
    {
        CHK_ORET(mxf_read_kl(mxfFile, &key, &llen, &len));
        count += mxfKey_extlen + llen;
        
        if (mxf_is_filler(&key))
        {
            CHK_ORET(mxf_skip(mxfFile, len));
        }
        else
        {
            if (filter != NULL)
            {
                /* signal before read */
                skip = 0;
                if (filter->before_set_read != NULL)
                {
                    CHK_ORET(filter->before_set_read(filter->privateData, headerMetadata, &key, llen, len, &skip));
                }
                
                if (!skip)
                {
                    CHK_ORET((result = mxf_read_and_return_set(mxfFile, &key, len, headerMetadata, 0, &newSet)) > 0);

                    if (result == 1) /* set was read and returned in "set" parameter */
                    {
                        /* signal after read */
                        skip = 0;
                        if (filter->after_set_read != NULL)
                        {
                            CHK_OFAIL(filter->after_set_read(filter->privateData, headerMetadata, newSet, &skip));
                        }
                        
                        if (!skip)
                        {
                            CHK_OFAIL(mxf_add_set(headerMetadata, newSet));
                        }
                        else
                        {
                            mxf_free_set(&newSet);
                        }
                    }
                }
                else
                {
                    CHK_ORET(mxf_skip(mxfFile, len));
                }
            }
            else
            {
                CHK_ORET(mxf_read_set(mxfFile, &key, len, headerMetadata, 1) > 0);
            }
        }
        count += len;
    }
    CHK_ORET(count == headerByteCount);
    
    return 1;
    
fail:
    mxf_free_set(&newSet);
    return 0;
}

int mxf_read_set(MXFFile* mxfFile, const mxfKey* key, uint64_t len, 
    MXFHeaderMetadata* headerMetadata, int addToHeaderMetadata)
{
    MXFMetadataSet* set;
    return mxf_read_and_return_set(mxfFile, key, len, headerMetadata, addToHeaderMetadata, &set);
}

int mxf_read_and_return_set(MXFFile* mxfFile, const mxfKey* key, uint64_t len,
    MXFHeaderMetadata* headerMetadata, int addToHeaderMetadata, MXFMetadataSet** set)
{
    MXFMetadataSet* newSet = NULL;
    MXFSetDef* setDef = NULL;
    uint64_t totalLen = 0;
    mxfLocalTag itemTag;
    uint16_t itemLen;
    int haveInstanceUID = 0;
    mxfKey itemKey;
    MXFItemDef* itemDef = NULL;
    MXFMetadataItem* newItem;

    assert(headerMetadata->primerPack != NULL);

    /* only read sets with known definitions */    
    if (mxf_find_set_def(headerMetadata->dataModel, key, &setDef))
    {
        CHK_ORET(create_empty_set(key, &newSet));
    
        /* read each item in the set*/
        haveInstanceUID = 0;
        do
        {
            CHK_OFAIL(mxf_read_item_tl(mxfFile, &itemTag, &itemLen));
            /* check the item tag is registered in the primer */
            if (mxf_get_item_key(headerMetadata->primerPack, itemTag, &itemKey))
            {
                /* only read items with known definition */
                if (mxf_find_item_def_in_set_def(&itemKey, setDef, &itemDef))
                {
                    CHK_OFAIL(mxf_create_item(newSet, &itemKey, itemTag, &newItem));
                    newItem->isPersistent = 1;
                    CHK_OFAIL(mxf_read_item(mxfFile, newItem, itemLen));
                    if (mxf_equals_key(&MXF_ITEM_K(InterchangeObject, InstanceUID), &itemKey))
                    {
                        mxf_get_uuid(newItem->value, &newSet->instanceUID);
                        haveInstanceUID = 1;
                    }
                }
                /* skip items with unknown definition */
                else
                {
                    CHK_OFAIL(mxf_skip(mxfFile, (int64_t)itemLen));
                }
            }
            /* skip items not registered in the primer. Log warning because the file is invalid */
            else
            {
                mxf_log_warn("Encountered item with tag %d not registered in the primer" LOG_LOC_FORMAT,
                    itemTag, LOG_LOC_PARAMS);
                CHK_OFAIL(mxf_skip(mxfFile, (int64_t)itemLen));
            }
            
            totalLen += 4 + itemLen;        
        }
        while (totalLen < len);
        
        if (totalLen != len)
        {
            mxf_log_error("Incorrect metadata set length encountered" LOG_LOC_FORMAT, LOG_LOC_PARAMS);
            goto fail;
        }
        if (!haveInstanceUID)
        {
            mxf_log_error("Metadata set does not have InstanceUID item" LOG_LOC_FORMAT, LOG_LOC_PARAMS);
            goto fail;
        }

        /* ok to add set */
        if (addToHeaderMetadata)
        {
            CHK_OFAIL(mxf_add_set(headerMetadata, newSet));
        }
    
        *set = newSet;
        return 1;
    }

    /* skip the set if the def is unknown */
    CHK_ORET(mxf_skip(mxfFile, (int64_t)len));
    *set = NULL;
    return 2;

fail:
    mxf_free_set(&newSet);
    return 0;    
}

int mxf_read_item_tl(MXFFile* mxfFile, mxfLocalTag* itemTag, uint16_t* itemLen)
{
    CHK_ORET(mxf_read_local_tag(mxfFile, itemTag));
    CHK_ORET(mxf_read_uint16(mxfFile, itemLen));
    
    return 1;
}

int mxf_read_item(MXFFile* mxfFile, MXFMetadataItem* item, uint16_t len)
{
    uint8_t buffer[65536];

    CHK_ORET(mxf_file_read(mxfFile, buffer, len) == len);

    CHK_MALLOC_ARRAY_ORET(item->value, uint8_t, len);
    memcpy(item->value, buffer, len);
    item->length = len;
    
    return 1;
}

int mxf_write_header_metadata(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata)
{
    CHK_ORET(mxf_write_header_primer_pack(mxfFile, headerMetadata));
    CHK_ORET(mxf_write_header_sets(mxfFile, headerMetadata));
    
    return 1;
}

int mxf_write_header_primer_pack(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata)
{
    CHK_ORET(mxf_write_primer_pack(mxfFile, headerMetadata->primerPack));
    
    return 1;
}

int mxf_write_header_sets(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata)
{
    MXFListIterator iter;
    MXFMetadataSet* prefaceSet;
    
    /* must write the Preface set first (and there must be a Preface set) */
    CHK_ORET(mxf_find_singular_set_by_key(headerMetadata, &MXF_SET_K(Preface), &prefaceSet));
    CHK_ORET(mxf_write_set(mxfFile, prefaceSet));
    
    mxf_initialise_list_iter(&iter, &headerMetadata->sets);
    while (mxf_next_list_iter_element(&iter))
    {
        MXFMetadataSet* set = (MXFMetadataSet*)mxf_get_iter_element(&iter);
        
        if (!mxf_equals_key(&set->key, &MXF_SET_K(Preface)))
        {
            CHK_ORET(mxf_write_set(mxfFile, set));
        }
    }
    
    return 1;
}

/* note: keep in sync with mxf_get_set_len */
int mxf_write_set(MXFFile* mxfFile, MXFMetadataSet* set)
{
    MXFListIterator iter;
    uint64_t setLen = 0;
    uint64_t setSize = 0;
    
    mxf_initialise_list_iter(&iter, &set->items);
    while (mxf_next_list_iter_element(&iter))
    {
        setLen += ((MXFMetadataItem*)mxf_get_iter_element(&iter))->length + 4;
    }
 
    if (mxf_get_llen(mxfFile, setLen) <= 4)
    {
        /* spec says preferred 4-byte BER encoded len for sets */ 
        CHK_ORET(mxf_write_fixed_kl(mxfFile, &set->key, 4, setLen));
        setSize = mxfKey_extlen + 4 + setLen;
    }
    else
    {
        CHK_ORET(mxf_write_kl(mxfFile, &set->key, setLen));
        setSize = mxfKey_extlen + mxf_get_llen(mxfFile, setLen) + setLen;
    }
    
    mxf_initialise_list_iter(&iter, &set->items);
    while (mxf_next_list_iter_element(&iter))
    {
        CHK_ORET(mxf_write_item(mxfFile, (MXFMetadataItem*)mxf_get_iter_element(&iter)));
    }
    
    if (set->fixedSpaceAllocation > 0)
    {
        /* check that we can achieve the fixed size, possibly using a filler */
        CHK_ORET(setSize == set->fixedSpaceAllocation || 
            (setSize < set->fixedSpaceAllocation && 
                setSize + mxf_get_min_llen(mxfFile) + mxfKey_extlen <= set->fixedSpaceAllocation));
            
        if (setSize < set->fixedSpaceAllocation)
        {
            /* add filler */
            CHK_ORET(mxf_write_fill(mxfFile, (uint32_t)(set->fixedSpaceAllocation - setSize)));
        }
    }
    
    return 1;
}

int mxf_write_item(MXFFile* mxfFile, MXFMetadataItem* item)
{
    CHK_ORET(mxf_write_local_tag(mxfFile, item->tag));
    CHK_ORET(mxf_write_uint16(mxfFile, item->length));
    CHK_ORET(mxf_file_write(mxfFile, item->value, item->length) == item->length);
    item->isPersistent = 1;
    
    return 1;
}

void mxf_get_header_metadata_size(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata, uint64_t* size)
{
    MXFListIterator iter;
    uint64_t primerSize;
    
    mxf_get_primer_pack_size(mxfFile, headerMetadata->primerPack, &primerSize);
    *size = primerSize;
    
    mxf_initialise_list_iter(&iter, &headerMetadata->sets);
    while (mxf_next_list_iter_element(&iter))
    {
        *size += mxf_get_set_size(mxfFile, (MXFMetadataSet*)mxf_get_iter_element(&iter));
    }        
}

/* note: keep in sync with mxf_write_set */
uint64_t mxf_get_set_size(MXFFile* mxfFile, MXFMetadataSet* set)
{
    MXFListIterator iter;
    uint64_t len;
    uint8_t llen;

    if (set->fixedSpaceAllocation > 0)
    {
        return set->fixedSpaceAllocation;
    }
    
    len = 0;
    mxf_initialise_list_iter(&iter, &set->items);
    while (mxf_next_list_iter_element(&iter))
    {
        len += ((MXFMetadataItem*)mxf_get_iter_element(&iter))->length + 4;
    }
    llen = mxf_get_llen(mxfFile, len);
    if (llen < 4)
    {
        /* spec says preferred 4-byte BER encoded len for sets */ 
        llen = 4;
    }
    
    return mxfKey_extlen + len + llen;
}


void mxf_get_uint8(const uint8_t* value, uint8_t* result)
{
    *result = *value;
}

void mxf_get_uint16(const uint8_t* value, uint16_t* result)
{
    *result = ((uint16_t)value[0] << 8) | (uint16_t)value[1];
}

void mxf_get_uint32(const uint8_t* value, uint32_t* result)
{
    *result = ((uint32_t)value[0] << 24) | ((uint32_t)value[1] << 16) | 
        ((uint32_t)value[2] << 8) | (uint32_t)value[3];
}

void mxf_get_uint64(const uint8_t* value, uint64_t* result)
{
    *result = ((uint64_t)value[0] << 56) | ((uint64_t)value[1] << 48) | 
        ((uint64_t)value[2] << 40) | ((uint64_t)value[3] << 32) | 
        ((uint64_t)value[4] << 24) | ((uint64_t)value[5] << 16) | 
        ((uint64_t)value[6] << 8) | (uint64_t)value[7];
}

void mxf_get_int8(const uint8_t* value, int8_t* result)
{
    mxf_get_uint8(value, (uint8_t*)result);
}

void mxf_get_int16(const uint8_t* value, int16_t* result)
{
    mxf_get_uint16(value, (uint16_t*)result);
}

void mxf_get_int32(const uint8_t* value, int32_t* result)
{
    mxf_get_uint32(value, (uint32_t*)result);
}

void mxf_get_int64(const uint8_t* value, int64_t* result)
{
    mxf_get_uint64(value, (uint64_t*)result);
}

void mxf_get_version_type(const uint8_t* value, mxfVersionType* result)
{
    mxf_get_uint16(value, result);
}

void mxf_get_uuid(const uint8_t* value, mxfUUID* result)
{
    memcpy(result, value, mxfUUID_extlen);
}

void mxf_get_ul(const uint8_t* value, mxfUL* result)
{
    memcpy(result, value, mxfUL_extlen);
}

void mxf_get_auid(const uint8_t* value, mxfAUID* result)
{
    memcpy(result, value, mxfAUID_extlen);
}

void mxf_get_umid(const uint8_t* value, mxfUMID* result)
{
    memcpy(result, value, mxfUMID_extlen);
}

void mxf_get_timestamp(const uint8_t* value, mxfTimestamp* result)
{
    mxf_get_int16(value, &result->year);
    mxf_get_uint8(&value[2], &result->month);
    mxf_get_uint8(&value[3], &result->day);
    mxf_get_uint8(&value[4], &result->hour);
    mxf_get_uint8(&value[5], &result->min);
    mxf_get_uint8(&value[6], &result->sec);
    mxf_get_uint8(&value[7], &result->qmsec);
}

void mxf_get_length(const uint8_t* value, mxfLength* result)
{
    mxf_get_int64(value, result);
}

void mxf_get_rational(const uint8_t* value, mxfRational* result)
{
    mxf_get_int32(value, &result->numerator);
    mxf_get_int32(&value[4], &result->denominator);
}

void mxf_get_position(const uint8_t* value, mxfPosition* result)
{
    mxf_get_int64(value, result);
}

void mxf_get_boolean(const uint8_t* value, mxfBoolean* result)
{
    mxf_get_uint8(value, result);
}

void mxf_get_product_version(const uint8_t* value, mxfProductVersion* result)
{
    mxf_get_uint16(value, &result->major);
    mxf_get_uint16(&value[2], &result->minor);
    mxf_get_uint16(&value[4], &result->patch);
    mxf_get_uint16(&value[6], &result->build);
    mxf_get_uint16(&value[8], &result->release);
}

void mxf_get_rgba_layout_component(const uint8_t* value, mxfRGBALayoutComponent* result)
{
    mxf_get_uint8(value, &result->code);
    mxf_get_uint8(&value[1], &result->depth);
}

void mxf_get_array_header(const uint8_t* value, uint32_t* arrayLen, uint32_t* arrayItemLen)
{
    mxf_get_uint32(value, arrayLen);
    mxf_get_uint32(&value[4], arrayItemLen);
}

/* Note: the size always includes a null terminator */
uint16_t mxf_get_utf16string_size(const uint8_t* value, uint16_t valueLen)
{
    uint16_t i;
    int haveNullTerminator = 0;
    uint16_t size = 0;
    
    /* get characters until end of value or null terminator */
    for (i = 0; i < valueLen / 2; i++)
    {
        size += 1;
        if (value[2*i] == 0 && value[2*i + 1] == 0)
        {
            haveNullTerminator = 1;
            break;
        }
    }
    
    /* add 1 for null terminator if not present */
    if (!haveNullTerminator)
    {
        size += 1;
    }
    
    return size;
}

/* Note: returns a null-terminated UTF16 string*/
void mxf_get_utf16string(const uint8_t* value, uint16_t valueLen, mxfUTF16Char* result)
{
    uint16_t i;
    int haveNullTerminator = 0;
    uint16_t c;
    
    /* get characters until end of value or null terminator */
    for (i = 0; i < valueLen / 2; i++)
    {
        mxf_get_uint16(&value[2 * i], &c);
        result[i] = c;
        if (result[i] == 0)
        {
            haveNullTerminator = 1;
            break;
        }
    }
    
    /* add null terminator if none is present */
    if (!haveNullTerminator)
    {
        result[i] = 0;
    }
}

int mxf_get_strongref(MXFHeaderMetadata* headerMetadata, const uint8_t* value, MXFMetadataSet** set)
{
    mxfUUID uuid;
    
    mxf_get_uuid(value, &uuid);
    return mxf_dereference(headerMetadata, &uuid, set);
}

int mxf_get_weakref(MXFHeaderMetadata* headerMetadata, const uint8_t* value, MXFMetadataSet** set)
{
    mxfUUID uuid;
    
    mxf_get_uuid(value, &uuid);
    return mxf_dereference(headerMetadata, &uuid, set);
}

int mxf_dereference(MXFHeaderMetadata* headerMetadata, const mxfUUID* uuid, MXFMetadataSet** set)
{
    void* result;
    
    if ((result = mxf_find_list_element(&headerMetadata->sets, (void*)uuid, set_eq_instanceuid)) == NULL)
    {
        return 0;
    }

    *set = (MXFMetadataSet*)result;
    return 1;
}


void initialise_sets_iter(MXFHeaderMetadata* headerMetadata, MXFListIterator* setsIter)
{
    mxf_initialise_list_iter(setsIter, &headerMetadata->sets);
}

int mxf_get_strongref_s(MXFHeaderMetadata* headerMetadata, MXFListIterator* setsIter, const uint8_t* value, MXFMetadataSet** set)
{
    mxfUUID uuid;
    
    mxf_get_uuid(value, &uuid);
    return mxf_dereference_s(headerMetadata, setsIter, &uuid, set);
}

int mxf_get_weakref_s(MXFHeaderMetadata* headerMetadata, MXFListIterator* setsIter, const uint8_t* value, MXFMetadataSet** set)
{
    mxfUUID uuid;
    
    mxf_get_uuid(value, &uuid);
    return mxf_dereference_s(headerMetadata, setsIter, &uuid, set);
}

/* this will be faster is we de-reference multiple sets in the same order as they were written */
int mxf_dereference_s(MXFHeaderMetadata* headerMetadata, MXFListIterator* setsIter, const mxfUUID* uuid, MXFMetadataSet** set)
{
    MXFMetadataSet* setInList;
    long startIndex = mxf_get_list_iter_index(setsIter);

    /* try find it at the previous position in the list */
    if (startIndex >= 0)
    {
        setInList = (MXFMetadataSet*)mxf_get_iter_element(setsIter);
        if (mxf_equals_uuid(uuid, &setInList->instanceUID))
        {
            *set = setInList;
            return 1;
        }
    }
        
    /* try find it starting from the previous position in the list */
    while (mxf_next_list_iter_element(setsIter))
    {
        setInList = (MXFMetadataSet*)mxf_get_iter_element(setsIter);
        
        if (mxf_equals_uuid(uuid, &setInList->instanceUID))
        {
            *set = setInList;
            return 1;
        }
    }
    
    /* go back to beginning and try find it before the previous position in the list */
    initialise_sets_iter(headerMetadata, setsIter);
    while (mxf_next_list_iter_element(setsIter) && mxf_get_list_iter_index(setsIter) < startIndex)
    {
        setInList = (MXFMetadataSet*)mxf_get_iter_element(setsIter);
        
        if (mxf_equals_uuid(uuid, &setInList->instanceUID))
        {
            *set = setInList;
            return 1;
        }
    }

    return 0;
}



void mxf_set_uint8(uint8_t value, uint8_t* result)
{
    result[0] = value & 0xff;
}

void mxf_set_uint16(uint16_t value, uint8_t* result)
{
    result[0] = (uint8_t)((value >> 8) & 0xff);
    result[1] = (uint8_t)(value & 0xff);
}

void mxf_set_uint32(uint32_t value, uint8_t* result)
{
    result[0] = (uint8_t)((value >> 24) & 0xff);
    result[1] = (uint8_t)((value >> 16) & 0xff);
    result[2] = (uint8_t)((value >> 8) & 0xff);
    result[3] = (uint8_t)(value & 0xff);
}

void mxf_set_uint64(uint64_t value, uint8_t* result)
{
    result[0] = (uint8_t)((value >> 56) & 0xff);
    result[1] = (uint8_t)((value >> 48) & 0xff);
    result[2] = (uint8_t)((value >> 40) & 0xff);
    result[3] = (uint8_t)((value >> 32) & 0xff);
    result[4] = (uint8_t)((value >> 24) & 0xff);
    result[5] = (uint8_t)((value >> 16) & 0xff);
    result[6] = (uint8_t)((value >> 8) & 0xff);
    result[7] = (uint8_t)(value & 0xff);
}

void mxf_set_int8(int8_t value, uint8_t* result)
{
    mxf_set_uint8(*(uint8_t*)&value, result);
}

void mxf_set_int16(int16_t value, uint8_t* result)
{
    mxf_set_uint16(*(uint16_t*)&value, result);
}

void mxf_set_int32(int32_t value, uint8_t* result)
{
    mxf_set_uint32(*(uint32_t*)&value, result);
}

void mxf_set_int64(int64_t value, uint8_t* result)
{
    mxf_set_uint64(*(uint64_t*)&value, result);
}

void mxf_set_version_type(mxfVersionType value, uint8_t* result)
{
    mxf_set_uint16(value, result);
}

void mxf_set_uuid(const mxfUUID* value, uint8_t* result)
{
    memcpy(result, value, mxfUUID_extlen);
}

void mxf_set_ul(const mxfUL* value, uint8_t* result)
{
    mxf_set_uuid((const mxfUUID*)value, result);
}

void mxf_set_auid(const mxfAUID* value, uint8_t* result)
{
    mxf_set_uuid((const mxfUUID*)value, result);
}

void mxf_set_umid(const mxfUMID* value, uint8_t* result)
{
    memcpy(result, value, mxfUMID_extlen);
}

void mxf_set_timestamp(const mxfTimestamp* value, uint8_t* result)
{
    mxf_set_int16(value->year, result);
    memcpy(&result[2], &value->month, mxfTimestamp_extlen - 2); /* month, day, hour, min, sec, qmsec */
}

uint16_t mxf_get_external_utf16string_size(const mxfUTF16Char* value)
{
    return (uint16_t)((wcslen(value) + 1) * mxfUTF16Char_extlen);
}

/* Note: string must be null terminated */
void mxf_set_utf16string(const mxfUTF16Char* value, uint8_t* result)
{
    uint16_t size = (uint16_t)(wcslen(value) + 1);
    uint16_t i;
    for (i = 0; i < size; i++)
    {
        result[i * 2] = (value[i] >> 8) & 0xff;
        result[i * 2 + 1] = value[i] & 0xff;
    }
}

/* Note: string must be null terminated */
void mxf_set_fixed_size_utf16string(const mxfUTF16Char* value, uint16_t size, uint8_t* result)
{
    uint16_t stringSize = (uint16_t)(wcslen(value) + 1);
    uint16_t i;
    
    if (stringSize > size)
    {
        stringSize = size;
    }
    
    for (i = 0; i < stringSize; i++)
    {
        result[i * 2] = (value[i] >> 8) & 0xff;
        result[i * 2 + 1] = value[i] & 0xff;
    }

    /* pad remaining space with zeros */
    if (stringSize < size)
    {
        memset(&result[stringSize * mxfUTF16Char_extlen], 0, (size - stringSize) * mxfUTF16Char_extlen);
    }
}

void mxf_set_strongref(const MXFMetadataSet* value, uint8_t* result)
{
    mxf_set_uuid(&value->instanceUID, result);
}

void mxf_set_weakref(const MXFMetadataSet* value, uint8_t* result)
{
    mxf_set_uuid(&value->instanceUID, result);
}

void mxf_set_rational(const mxfRational* value, uint8_t* result)
{
    mxf_set_int32(value->numerator, result); 
    mxf_set_int32(value->denominator, &result[4]); 
}

void mxf_set_position(mxfPosition value, uint8_t* result)
{
    mxf_set_int64(value, result); 
}

void mxf_set_length(mxfLength value, uint8_t* result)
{
    mxf_set_int64(value, result); 
}

void mxf_set_boolean(mxfBoolean value, uint8_t* result)
{
    mxf_set_uint8(value, result); 
}

void mxf_set_product_version(const mxfProductVersion* value, uint8_t* result)
{
    mxf_set_uint16(value->major, result); 
    mxf_set_uint16(value->minor, &result[2]); 
    mxf_set_uint16(value->patch, &result[4]); 
    mxf_set_uint16(value->build, &result[6]); 
    mxf_set_uint16(value->release, &result[8]); 
}

void mxf_set_rgba_layout_component(const mxfRGBALayoutComponent* value, uint8_t* result)
{
    mxf_set_uint8(value->code, result); 
    mxf_set_uint8(value->depth, &result[1]); 
}

void mxf_set_array_header(uint32_t arrayLen, uint32_t arrayElementLen, uint8_t* result)
{
    mxf_set_uint32(arrayLen, result);
    mxf_set_uint32(arrayElementLen, &result[4]);
}


int mxf_set_item_value(MXFMetadataItem* item, const uint8_t* value, uint16_t len)
{
    if (item->value != NULL && item->length != len)
    {
        free_metadata_item_value(item);
    }
    if (item->value == NULL)
    {
        CHK_MALLOC_ARRAY_ORET(item->value, uint8_t, len);
    }
    memcpy(item->value, value, len);
    item->length = len;
    item->isPersistent = 0;
    
    return 1;
}

int mxf_set_item(MXFMetadataSet* set, const mxfKey* itemKey, const uint8_t* value, uint16_t len)
{
    MXFMetadataItem* newItem = NULL;

    assert(set->headerMetadata != NULL);

    CHK_ORET(get_or_create_set_item(set->headerMetadata, set, itemKey, &newItem));
    CHK_ORET(mxf_set_item_value(newItem, value, len));
    
    return 1;
}

int mxf_clone_item(MXFMetadataSet* sourceSet, const mxfKey* itemKey, MXFMetadataSet* destSet)
{
    MXFMetadataItem* sourceItem = NULL;
    MXFMetadataItem* newItem = NULL;
    
    assert(destSet->headerMetadata != NULL);

    CHK_ORET(mxf_get_item(sourceSet, itemKey, &sourceItem));
    CHK_ORET(get_or_create_set_item(destSet->headerMetadata, destSet, itemKey, &newItem));
    CHK_ORET(mxf_set_item_value(newItem, sourceItem->value, sourceItem->length));

    return 1;
}


#define SET_VALUE(len, set_func) \
    MXFMetadataItem* newItem = NULL; \
    uint8_t buffer[len]; \
    \
    assert(set->headerMetadata != NULL); \
    \
    CHK_ORET(get_or_create_set_item(set->headerMetadata, set, itemKey, &newItem)); \
    \
    set_func(value, buffer); \
    CHK_ORET(mxf_set_item_value(newItem, buffer, len)); \
    \
    return 1;
    

int mxf_set_uint8_item(MXFMetadataSet* set, const mxfKey* itemKey, uint8_t value)
{
    SET_VALUE(1, mxf_set_uint8);
}

int mxf_set_uint16_item(MXFMetadataSet* set, const mxfKey* itemKey, uint16_t value)
{
    SET_VALUE(2, mxf_set_uint16);
}

int mxf_set_uint32_item(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t value)
{
    SET_VALUE(4, mxf_set_uint32);
}

int mxf_set_uint64_item(MXFMetadataSet* set, const mxfKey* itemKey, uint64_t value)
{
    SET_VALUE(8, mxf_set_uint64);
}

int mxf_set_int8_item(MXFMetadataSet* set, const mxfKey* itemKey, int8_t value)
{
    SET_VALUE(1, mxf_set_int8);
}

int mxf_set_int16_item(MXFMetadataSet* set, const mxfKey* itemKey, int16_t value)
{
    SET_VALUE(2, mxf_set_int16);
}

int mxf_set_int32_item(MXFMetadataSet* set, const mxfKey* itemKey, int32_t value)
{
    SET_VALUE(4, mxf_set_int32);
}

int mxf_set_int64_item(MXFMetadataSet* set, const mxfKey* itemKey, int64_t value)
{
    SET_VALUE(8, mxf_set_int64);
}

int mxf_set_version_type_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfVersionType value)
{
    SET_VALUE(mxfVersionType_extlen, mxf_set_version_type);
}

int mxf_set_uuid_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfUUID* value)
{
    SET_VALUE(mxfUUID_extlen, mxf_set_uuid);
}

int mxf_set_ul_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfUL* value)
{
    SET_VALUE(mxfUL_extlen, mxf_set_ul);
}

int mxf_set_auid_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfAUID* value)
{
    SET_VALUE(mxfAUID_extlen, mxf_set_auid);
}

int mxf_set_timestamp_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfTimestamp* value)
{
    SET_VALUE(mxfTimestamp_extlen, mxf_set_timestamp);
}

/* string must be null terminated */
int mxf_set_utf16string_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfUTF16Char* value)
{
    MXFMetadataItem* newItem = NULL;
    uint8_t buffer[65536];
    
    assert(set->headerMetadata != NULL);

    CHK_ORET(get_or_create_set_item(set->headerMetadata, set, itemKey, &newItem));

    mxf_set_utf16string(value, buffer);
    CHK_ORET(mxf_set_item_value(newItem, buffer, (uint16_t)((wcslen(value) + 1) * mxfUTF16Char_extlen)));

    return 1;
}

/* string must be null terminated */
int mxf_set_fixed_size_utf16string_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfUTF16Char* value, 
    uint16_t size)
{
    MXFMetadataItem* newItem = NULL;
    uint8_t buffer[65536];
    
    assert(set->headerMetadata != NULL);

    CHK_ORET(get_or_create_set_item(set->headerMetadata, set, itemKey, &newItem));

    mxf_set_fixed_size_utf16string(value, size, buffer);
    CHK_ORET(mxf_set_item_value(newItem, buffer, size * mxfUTF16Char_extlen));

    return 1;
}

int mxf_set_strongref_item(MXFMetadataSet* set, const mxfKey* itemKey, const MXFMetadataSet* value)
{
    SET_VALUE(mxfUUID_extlen, mxf_set_strongref);
}

int mxf_set_weakref_item(MXFMetadataSet* set, const mxfKey* itemKey, const MXFMetadataSet* value)
{
    SET_VALUE(mxfUUID_extlen, mxf_set_weakref);
}

int mxf_set_umid_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfUMID* value)
{
    SET_VALUE(mxfUMID_extlen, mxf_set_umid);
}

int mxf_set_rational_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfRational* value)
{
    SET_VALUE(mxfRational_extlen, mxf_set_rational);
}

int mxf_set_position_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfPosition value)
{
    SET_VALUE(mxfPosition_extlen, mxf_set_position);
}

int mxf_set_length_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfLength value)
{
    SET_VALUE(mxfLength_extlen, mxf_set_length);
}

int mxf_set_boolean_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfBoolean value)
{
    SET_VALUE(mxfBoolean_extlen, mxf_set_boolean);
}

int mxf_set_product_version_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfProductVersion* value)
{
    SET_VALUE(mxfProductVersion_extlen, mxf_set_product_version);
}

int mxf_set_rgba_layout_component_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfRGBALayoutComponent* value)
{
    SET_VALUE(mxfRGBALayoutComponent_extlen, mxf_set_rgba_layout_component);
}


int mxf_alloc_array_item_elements(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t elementLen, 
    uint32_t count, uint8_t** elements)
{
    MXFMetadataItem* newItem = NULL;
    uint8_t* buffer = NULL;

    assert(set->headerMetadata != NULL);
    
    if (count == 0)
    {
        int result = mxf_set_empty_array_item(set, itemKey, elementLen);
        if (result)
        {
            *elements = NULL;
        }
        return result;
    }

    CHK_OFAIL(8 + count * elementLen < 65536);
    
    CHK_ORET(get_or_create_set_item(set->headerMetadata, set, itemKey, &newItem));

    CHK_MALLOC_ARRAY_ORET(buffer, uint8_t, 8 + count * elementLen);
    mxf_set_array_header(count, elementLen, buffer);
    memset(&buffer[8], 0, elementLen * count);
    
    CHK_OFAIL(mxf_set_item_value(newItem, buffer, (uint16_t)(8 + count * elementLen)));
    *elements = &newItem->value[8];
    
    SAFE_FREE(&buffer);
    return 1;
    
fail:
    SAFE_FREE(&buffer);
    return 0;
}

int mxf_grow_array_item(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t elementLen, 
    uint32_t count, uint8_t** newElements)
{
    MXFMetadataItem* newItem = NULL;
    uint8_t* buffer = NULL;
    uint32_t arrayLen;
    uint32_t existElementLen;

    assert(set->headerMetadata != NULL);
    
    if (count == 0)
    {
        int result = mxf_set_empty_array_item(set, itemKey, elementLen);
        if (result)
        {
            *newElements = NULL;
        }
        return result;
    }
    
    CHK_ORET(get_or_create_set_item(set->headerMetadata, set, itemKey, &newItem));

    if (newItem->value == NULL)
    {
        arrayLen = count;
        CHK_MALLOC_ARRAY_ORET(buffer, uint8_t, 8 + arrayLen * elementLen);
    }
    else
    {
        CHK_ORET(newItem->length >= 8);
        mxf_get_array_header(newItem->value, &arrayLen, &existElementLen);
        CHK_ORET(elementLen == existElementLen);
        CHK_ORET(newItem->length == 8 + arrayLen * existElementLen);

        arrayLen += count;
        CHK_MALLOC_ARRAY_ORET(buffer, uint8_t, 8 + arrayLen * elementLen);
        memcpy(buffer, newItem->value, newItem->length);
    }
    
    /* (re-)write batch/array header and set new element to 0 */    
    mxf_set_array_header(arrayLen, elementLen, buffer);
    memset(&buffer[8 + (arrayLen - count) * elementLen], 0, elementLen * count);
    
    CHK_OFAIL(8 + arrayLen * elementLen < 65536);
    CHK_OFAIL(mxf_set_item_value(newItem, buffer, (uint16_t)(8 + arrayLen * elementLen)));
    *newElements = &newItem->value[8 + (arrayLen - count) * elementLen];
    
    SAFE_FREE(&buffer);
    return 1;
    
fail:
    SAFE_FREE(&buffer);
    return 0;
}

int mxf_set_empty_array_item(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t elementLen)
{
    MXFMetadataItem* newItem = NULL;
    uint8_t* buffer = NULL;
    
    assert(set->headerMetadata != NULL);
    
    CHK_MALLOC_ARRAY_ORET(buffer, uint8_t, 8);
    mxf_set_array_header(0, elementLen, buffer);

    CHK_OFAIL(get_or_create_set_item(set->headerMetadata, set, itemKey, &newItem));
    
    CHK_OFAIL(mxf_set_item_value(newItem, buffer, 8));
    
    SAFE_FREE(&buffer);
    return 1;
    
fail:
    SAFE_FREE(&buffer);
    return 0;
}

int mxf_add_array_item_strongref(MXFMetadataSet* set, const mxfKey* itemKey, const MXFMetadataSet* value)
{
    uint8_t* arrayElement;
    
    CHK_ORET(mxf_grow_array_item(set, itemKey, mxfUUID_extlen, 1, &arrayElement));
    mxf_set_strongref(value, arrayElement);
    
    return 1;
}

int mxf_add_array_item_weakref(MXFMetadataSet* set, const mxfKey* itemKey, const MXFMetadataSet* value)
{
    uint8_t* arrayElement;
    
    CHK_ORET(mxf_grow_array_item(set, itemKey, mxfUUID_extlen, 1, &arrayElement));
    mxf_set_weakref(value, arrayElement);
    
    return 1;
}


int mxf_get_item_len(MXFMetadataSet* set, const mxfKey* itemKey, uint16_t* len)
{
    MXFMetadataItem* item = NULL;

    CHK_ORET(mxf_get_item(set, itemKey, &item));
    
    *len = item->length;
    return 1;
}


#define GET_VALUE(len, get_func) \
    MXFMetadataItem* item = NULL; \
    \
    CHK_ORET(mxf_get_item(set, itemKey, &item)); \
    CHK_ORET(item->length == len); \
    \
    get_func(item->value, value); \
    return 1;

    
int mxf_get_uint8_item(MXFMetadataSet* set, const mxfKey* itemKey, uint8_t* value)
{
    GET_VALUE(1, mxf_get_uint8);
}

int mxf_get_uint16_item(MXFMetadataSet* set, const mxfKey* itemKey, uint16_t* value)
{
    GET_VALUE(2, mxf_get_uint16);
}

int mxf_get_uint32_item(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t* value)
{
    GET_VALUE(4, mxf_get_uint32);
}

int mxf_get_uint64_item(MXFMetadataSet* set, const mxfKey* itemKey, uint64_t* value)
{
    GET_VALUE(8, mxf_get_uint64);
}

int mxf_get_int8_item(MXFMetadataSet* set, const mxfKey* itemKey, int8_t* value)
{
    GET_VALUE(1, mxf_get_int8);
}

int mxf_get_int16_item(MXFMetadataSet* set, const mxfKey* itemKey, int16_t* value)
{
    GET_VALUE(2, mxf_get_int16);
}

int mxf_get_int32_item(MXFMetadataSet* set, const mxfKey* itemKey, int32_t* value)
{
    GET_VALUE(4, mxf_get_int32);
}

int mxf_get_int64_item(MXFMetadataSet* set, const mxfKey* itemKey, int64_t* value)
{
    GET_VALUE(8, mxf_get_int64);
}

int mxf_get_version_type_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfVersionType* value)
{
    GET_VALUE(mxfVersionType_extlen, mxf_get_version_type);
}

int mxf_get_uuid_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfUUID* value)
{
    GET_VALUE(mxfUUID_extlen, mxf_get_uuid);
}

int mxf_get_ul_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfUL* value)
{
    GET_VALUE(mxfUL_extlen, mxf_get_ul);
}

int mxf_get_auid_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfAUID* value)
{
    GET_VALUE(mxfAUID_extlen, mxf_get_auid);
}

int mxf_get_umid_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfUMID* value)
{
    GET_VALUE(mxfUMID_extlen, mxf_get_umid);
}

int mxf_get_timestamp_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfTimestamp* value)
{
    GET_VALUE(mxfTimestamp_extlen, mxf_get_timestamp);
}

/* Note: the size always includes a null terminator */
int mxf_get_utf16string_item_size(MXFMetadataSet* set, const mxfKey* itemKey, uint16_t* size)
{
    MXFMetadataItem* item = NULL;

    CHK_ORET(mxf_get_item(set, itemKey, &item));

    *size = mxf_get_utf16string_size(item->value, item->length);
    
    return 1;
}

/* Note: returns a null-terminated UTF16 string*/
int mxf_get_utf16string_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfUTF16Char* value)
{
    MXFMetadataItem* item = NULL;

    CHK_ORET(mxf_get_item(set, itemKey, &item));

    mxf_get_utf16string(item->value, item->length, value);
    
    return 1;
}

int mxf_get_strongref_item(MXFMetadataSet* set, const mxfKey* itemKey, MXFMetadataSet** value)
{
    mxfUUID uuidValue;
    
    CHK_ORET(set->headerMetadata != NULL);
    
    CHK_ORET(mxf_get_uuid_item(set, itemKey, &uuidValue));
    CHK_ORET(mxf_dereference(set->headerMetadata, &uuidValue, value));
    
    return 1;
}

int mxf_get_weakref_item(MXFMetadataSet* set, const mxfKey* itemKey, MXFMetadataSet** value)
{
    mxfUUID uuidValue;
    
    CHK_ORET(set->headerMetadata != NULL);
    
    CHK_ORET(mxf_get_uuid_item(set, itemKey, &uuidValue));
    CHK_ORET(mxf_dereference(set->headerMetadata, &uuidValue, value));
    
    return 1;
}

int mxf_get_strongref_item_s(MXFListIterator* setsIter, MXFMetadataSet* set, const mxfKey* itemKey, MXFMetadataSet** value)
{
    mxfUUID uuidValue;
    
    CHK_ORET(set->headerMetadata != NULL);
    
    CHK_ORET(mxf_get_uuid_item(set, itemKey, &uuidValue));
    CHK_ORET(mxf_dereference_s(set->headerMetadata, setsIter, &uuidValue, value));
    
    return 1;
}

int mxf_get_weakref_item_s(MXFListIterator* setsIter, MXFMetadataSet* set, const mxfKey* itemKey, MXFMetadataSet** value)
{
    mxfUUID uuidValue;
    
    CHK_ORET(set->headerMetadata != NULL);
    
    CHK_ORET(mxf_get_uuid_item(set, itemKey, &uuidValue));
    CHK_ORET(mxf_dereference_s(set->headerMetadata, setsIter, &uuidValue, value));
    
    return 1;
}

int mxf_get_length_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfLength* value)
{
    GET_VALUE(mxfLength_extlen, mxf_get_length);
}

int mxf_get_rational_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfRational* value)
{
    GET_VALUE(mxfRational_extlen, mxf_get_rational);
}

int mxf_get_position_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfPosition* value)
{
    GET_VALUE(mxfPosition_extlen, mxf_get_position);
}

int mxf_get_boolean_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfBoolean* value)
{
    GET_VALUE(mxfBoolean_extlen, mxf_get_boolean);
}

int mxf_get_product_version_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfProductVersion* value)
{
    GET_VALUE(mxfProductVersion_extlen, mxf_get_product_version);
}

int mxf_get_rgba_layout_component_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfRGBALayoutComponent* value)
{
    GET_VALUE(mxfRGBALayoutComponent_extlen, mxf_get_rgba_layout_component);
}


int mxf_get_array_item_count(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t* count)
{
    MXFMetadataItem* item = NULL;
    uint32_t elementLength;
    
    CHK_ORET(mxf_get_item(set, itemKey, &item));
    CHK_ORET(item->length >= 8);
    
    mxf_get_array_header(item->value, count, &elementLength);
    
    return 1;
}

int mxf_get_array_item_element_len(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t* elementLen)
{
    MXFMetadataItem* item = NULL;
    uint32_t count;
    
    CHK_ORET(mxf_get_item(set, itemKey, &item));
    CHK_ORET(item->length >= 8);
    
    mxf_get_array_header(item->value, &count, elementLen);
    
    return 1;
}

int mxf_get_array_item_element(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t index, uint8_t** element)
{
    MXFMetadataItem* item = NULL;
    uint32_t elementLen;
    uint32_t count;
    
    CHK_ORET(mxf_get_item(set, itemKey, &item));
    CHK_ORET(item->length >= 8);
    
    mxf_get_array_header(item->value, &count, &elementLen);
    
    CHK_ORET(index < count);
    
    *element = &item->value[8 + index * elementLen];
    
    return 1;
}

int mxf_initialise_array_item_iterator(MXFMetadataSet* set, const mxfKey* itemKey, MXFArrayItemIterator* arrayIter)
{
    MXFMetadataItem* item = NULL;
    
    CHK_ORET(mxf_get_item(set, itemKey, &item));
    CHK_ORET(item->length >= 8);
    
    arrayIter->item = item;
    mxf_get_array_header(item->value, &arrayIter->numElements, &arrayIter->elementLength);
    arrayIter->currentElement = 0;
    
    return 1;
}

int mxf_next_array_item_element(MXFArrayItemIterator* arrayIter, uint8_t** value, uint32_t* length)
{
    if (arrayIter->numElements == 0 || arrayIter->currentElement >= arrayIter->numElements)
    {
        return 0;
    }
    
    *value = &arrayIter->item->value[8 + arrayIter->currentElement * arrayIter->elementLength];
    *length = arrayIter->elementLength;
    arrayIter->currentElement++;
    return 1;
}



