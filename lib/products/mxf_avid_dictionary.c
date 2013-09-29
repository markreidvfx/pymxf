/*
 * $Id: mxf_avid_dictionary.c,v 1.3 2010/07/26 16:02:37 philipn Exp $
 *
 * Avid dictionary
 *
 * Copyright (C) 2008  Philip de Nier <philipn@users.sourceforge.net>
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mxf/mxf.h>
#include <mxf/mxf_avid.h>


typedef struct
{
    int skipDataDefs;
} MXFDictReadFilterData;




static int dict_before_set_read(void* privateData, MXFHeaderMetadata* headerMetadata, 
        const mxfKey* key, uint8_t llen, uint64_t len, int* skip)
{
    MXFDictReadFilterData* filterData = (MXFDictReadFilterData*)privateData;
    
    (void)llen;
    (void)len;
    
    if (mxf_avid_is_dictionary(headerMetadata->dataModel, key))
    {
        *skip = 1;
    }
    else if (mxf_avid_is_def_object(headerMetadata->dataModel, key) &&
        (filterData->skipDataDefs || 
            !mxf_is_subclass_of(headerMetadata->dataModel, key, &MXF_SET_K(DataDefinition))))
    {
        *skip = 1;
    }
    else
    {
        *skip = 0;
    }
    
    return 1;
}

static mxfUL* bounce_label(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, 
    uint8_t d6, uint8_t d7, uint8_t d8, uint8_t d9, uint8_t d10, uint8_t d11, uint8_t d12, uint8_t d13, 
    uint8_t d14, uint8_t d15, mxfUL* result)
{
    result->octet0 = d0;
    result->octet1 = d1;
    result->octet2 = d2;
    result->octet3 = d3;
    result->octet4 = d4;
    result->octet5 = d5;
    result->octet6 = d6;
    result->octet7 = d7;
    result->octet8 = d8;
    result->octet9 = d9;
    result->octet10 = d10;
    result->octet11 = d11;
    result->octet12 = d12;
    result->octet13 = d13;
    result->octet14 = d14;
    result->octet15 = d15;

    return result;
}





int mxf_avid_is_dictionary(MXFDataModel* dataModel, const mxfKey* setKey)
{
    return mxf_is_subclass_of(dataModel, setKey, &MXF_SET_K(Dictionary));
}

int mxf_avid_is_def_object(MXFDataModel* dataModel, const mxfKey* setKey)
{
    return mxf_is_subclass_of(dataModel, setKey, &MXF_SET_K(DefinitionObject));
}


int mxf_avid_create_dictionary(MXFHeaderMetadata* headerMetadata, MXFMetadataSet** dictSet)
{
    MXFMetadataSet* newSet = NULL;
    
    CHK_ORET(mxf_create_set(headerMetadata, &MXF_SET_K(Dictionary), &newSet));

    
    *dictSet = newSet;
    return 1;
}


int mxf_avid_set_defobject_items(MXFMetadataSet* defSet, const mxfUL* id, const mxfUTF16Char* name, const mxfUTF16Char* description)
{
    CHK_ORET(mxf_set_ul_item(defSet, &MXF_ITEM_K(DefinitionObject, Identification), id));
    CHK_ORET(name != NULL);
    CHK_ORET(mxf_set_utf16string_item(defSet, &MXF_ITEM_K(DefinitionObject, Name), name));
    if (description != NULL)
    {
        CHK_ORET(mxf_set_utf16string_item(defSet, &MXF_ITEM_K(DefinitionObject, Description), description));
    }

    return 1;
}

int mxf_avid_create_datadef(MXFMetadataSet* dictSet, const mxfUL* id, const mxfUTF16Char* name, const mxfUTF16Char* description, MXFMetadataSet** defSet)
{
    MXFMetadataSet* newSet = NULL;
    
    CHK_ORET(mxf_create_set(dictSet->headerMetadata, &MXF_SET_K(DataDefinition), &newSet));
    CHK_ORET(mxf_add_array_item_strongref(dictSet, &MXF_ITEM_K(Dictionary, DataDefinitions), newSet));
    
    mxf_avid_set_defobject_items(newSet, id, name, description);

    *defSet = newSet;
    return 1;
}

int mxf_avid_create_containerdef(MXFMetadataSet* dictSet, const mxfUL* id, const mxfUTF16Char* name, const mxfUTF16Char* description, MXFMetadataSet** defSet)
{
    MXFMetadataSet* newSet = NULL;
    
    CHK_ORET(mxf_create_set(dictSet->headerMetadata, &MXF_SET_K(ContainerDefinition), &newSet));
    CHK_ORET(mxf_add_array_item_strongref(dictSet, &MXF_ITEM_K(Dictionary, ContainerDefinitions), newSet));
    
    mxf_avid_set_defobject_items(newSet, id, name, description);

    *defSet = newSet;
    return 1;
}

int mxf_avid_create_taggedvaluedef(MXFMetadataSet* dictSet, const mxfUL* id, const mxfUTF16Char* name, const mxfUTF16Char* description, MXFMetadataSet** defSet)
{
    MXFMetadataSet* newSet = NULL;
    
    CHK_ORET(mxf_create_set(dictSet->headerMetadata, &MXF_SET_K(TaggedValueDefinition), &newSet));
    CHK_ORET(mxf_add_array_item_strongref(dictSet, &MXF_ITEM_K(Dictionary, TaggedValueDefinitions), newSet));
    
    mxf_avid_set_defobject_items(newSet, id, name, description);

    *defSet = newSet;
    return 1;
}


int mxf_initialise_dict_read_filter(MXFReadFilter* filter, int skipDataDefs)
{
    MXFDictReadFilterData* newData = NULL;
    
    CHK_MALLOC_ORET(newData, MXFDictReadFilterData);
    newData->skipDataDefs = skipDataDefs;
    
    filter->privateData = newData;
    filter->before_set_read = dict_before_set_read;
    filter->after_set_read = NULL;
    
    return 1;
}

void mxf_clear_dict_read_filter(MXFReadFilter* filter)
{
    if (filter == NULL)
    {
        return;
    }
    
    SAFE_FREE(&filter->privateData);
}



#define LABEL(d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15) \
    bounce_label(d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15, &label1)

#define TAGGEDVALUE_DEF(id, name, description) \
    CHK_OFAIL(mxf_avid_create_taggedvaluedef(newSet, id, name, description, &defSet));

#define DATA_DEF(id, name, description) \
    CHK_OFAIL(mxf_avid_create_datadef(newSet, id, name, description, &defSet));

#define CONTAINER_DEF(id, name, description) \
    CHK_OFAIL(mxf_avid_create_containerdef(newSet, id, name, description, &defSet));


int mxf_avid_create_default_dictionary(MXFHeaderMetadata* headerMetadata, MXFMetadataSet** metaDictSet)
{
    MXFMetadataSet* newSet = NULL;
    mxfUL label1;
    MXFMetadataSet* defSet;
    
    CHK_OFAIL(mxf_avid_create_dictionary(headerMetadata, &newSet));

#include "mxf_avid_dictionary_data.h"


    *metaDictSet = newSet;
    return 1;
    
fail:
    if (newSet != NULL)
    {
        mxf_remove_set(headerMetadata, newSet);
        mxf_free_set(&newSet);
    }
    return 0;
}


