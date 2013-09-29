/*
 * $Id: mxf_data_model.c,v 1.8 2010/06/25 13:59:02 philipn Exp $
 *
 * MXF header metadata data model
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



static void clear_type(MXFItemType* type)
{
    size_t i;
    
    if (type == NULL)
    {
        return;
    }
    
    if (type->typeId != 0)
    {
        SAFE_FREE(&type->name);
        if (type->category == MXF_COMPOUND_TYPE_CAT)
        {
            for (i = 0; i < sizeof(type->info.compound.members) / sizeof(MXFCompoundTypeMemberInfo); i++)
            {
                SAFE_FREE(&type->info.compound.members[i].name);
            }
        }
    }
    memset(type, 0, sizeof(MXFItemType));
}

static void free_item_def(MXFItemDef** itemDef)
{
    if (*itemDef == NULL)
    {
        return;
    }
    
    SAFE_FREE(&(*itemDef)->name);
    SAFE_FREE(itemDef);
}

static void free_set_def(MXFSetDef** setDef)
{
    if (*setDef == NULL)
    {
        return;
    }
    
    SAFE_FREE(&(*setDef)->name);
    SAFE_FREE(setDef);
}

static void free_item_def_in_list(void* data)
{
    MXFItemDef* itemDef;
    
    if (data == NULL)
    {
        return;
    }
    
    itemDef = (MXFItemDef*)data;
    free_item_def(&itemDef);
}

static void free_set_def_in_list(void* data)
{
    MXFSetDef* setDef;
    
    if (data == NULL)
    {
        return;
    }
    
    setDef = (MXFSetDef*)data;
    mxf_clear_list(&setDef->itemDefs);
    free_set_def(&setDef);
}

static int set_def_eq(void* data, void* info)
{
    assert(data != NULL && info != NULL);
    
    return mxf_equals_key((mxfKey*)info, &((MXFSetDef*)data)->key);
}

static int item_def_eq(void* data, void* info)
{
    assert(data != NULL && info != NULL);
    
    return mxf_equals_key((mxfKey*)info, &((MXFItemDef*)data)->key);
}

static int add_set_def(MXFDataModel* dataModel, MXFSetDef* setDef)
{
    assert(setDef != NULL);
    
    CHK_ORET(mxf_append_list_element(&dataModel->setDefs, (void*)setDef));
    
    return 1;
}

static int add_item_def(MXFDataModel* dataModel, MXFItemDef* itemDef)
{
    assert(itemDef != NULL);
    
    CHK_ORET(mxf_append_list_element(&dataModel->itemDefs, (void*)itemDef));
    
    return 1;
}

static unsigned int get_type_id(MXFDataModel* dataModel)
{
    size_t i;
    unsigned int lastTypeId;
    unsigned int typeId = 0;
    
    if (dataModel->lastTypeId == 0 ||
        dataModel->lastTypeId >= sizeof(dataModel->types) / sizeof(MXFItemType))
    {
        lastTypeId = MXF_EXTENSION_TYPE;
    }
    else
    {
        lastTypeId = dataModel->lastTypeId;
    }
    
    /* try from the last type id to the end of the list */
    for (i = lastTypeId; i < sizeof(dataModel->types) / sizeof(MXFItemType); i++)
    {
        if (dataModel->types[i].typeId == 0)
        {
            typeId = (unsigned int)i;
            break;
        }
    }
    
    if (typeId == 0 && lastTypeId > MXF_EXTENSION_TYPE)
    {
        /* try from MXF_EXTENSION_TYPE to lastTypeId */
        for (i = MXF_EXTENSION_TYPE; i < lastTypeId; i++)
        {
            if (dataModel->types[i].typeId == 0)
            {
                typeId = (unsigned int)i;
                break;
            }
        }
    }
    
    return typeId;
}

static int clone_item_type(MXFDataModel* fromDataModel, unsigned int fromItemTypeId,
                           MXFDataModel* toDataModel, MXFItemType** toItemType)
{
    MXFItemType* clonedItemType = NULL;
    MXFItemType* fromItemType;
    MXFItemType* toRefItemType;
    int i;

    clonedItemType = mxf_get_item_def_type(toDataModel, fromItemTypeId);
    if (clonedItemType)
    {
        *toItemType = clonedItemType;
        return 1;
    }

    CHK_ORET((fromItemType = mxf_get_item_def_type(fromDataModel, fromItemTypeId)) != NULL);
    
    switch (fromItemType->category)
    {
        case MXF_BASIC_TYPE_CAT:
            clonedItemType = mxf_register_basic_type(toDataModel, fromItemType->name, fromItemType->typeId,
                                                     fromItemType->info.basic.size);
            CHK_ORET(clonedItemType);
            break;
        case MXF_ARRAY_TYPE_CAT:
            CHK_ORET(clone_item_type(fromDataModel, fromItemType->info.array.elementTypeId, toDataModel, &toRefItemType));
            clonedItemType = mxf_register_array_type(toDataModel, fromItemType->name, fromItemType->typeId,
                                                     fromItemType->info.array.elementTypeId,
                                                     fromItemType->info.array.fixedSize);
            CHK_ORET(clonedItemType);
            break;
        case MXF_COMPOUND_TYPE_CAT:
            clonedItemType = mxf_register_compound_type(toDataModel, fromItemType->name, fromItemType->typeId);
            CHK_ORET(clonedItemType);
            
            i = 0;
            while (fromItemType->info.compound.members[i].typeId)
            {
                CHK_ORET(clone_item_type(fromDataModel, fromItemType->info.compound.members[i].typeId,
                                         toDataModel, &toRefItemType));
                CHK_ORET(mxf_register_compound_type_member(clonedItemType, fromItemType->info.compound.members[i].name,
                                                           fromItemType->info.compound.members[i].typeId));
                i++;
            }
            break;
        case MXF_INTERPRET_TYPE_CAT:
            CHK_ORET(clone_item_type(fromDataModel, fromItemType->info.interpret.typeId,
                                     toDataModel, &toRefItemType));
            clonedItemType = mxf_register_interpret_type(toDataModel, fromItemType->name, fromItemType->typeId,
                                                         fromItemType->info.interpret.typeId,
                                                         fromItemType->info.interpret.fixedArraySize);
            CHK_ORET(clonedItemType);
            break;
    }
    
    *toItemType = clonedItemType;
    return 1;
}

static int clone_item_def(MXFDataModel* fromDataModel, MXFItemDef* fromItemDef,
                          MXFDataModel* toDataModel, MXFItemDef** toItemDef)
{
    MXFItemType* toItemType;
    
    CHK_ORET(clone_item_type(fromDataModel, fromItemDef->typeId, toDataModel, &toItemType));
    
    CHK_ORET(mxf_register_item_def(toDataModel, fromItemDef->name, &fromItemDef->setDefKey, &fromItemDef->key,
                                   fromItemDef->localTag, fromItemDef->typeId, fromItemDef->isRequired));
    
    *toItemDef = (MXFItemDef*)mxf_get_last_list_element(&toDataModel->itemDefs);
    return 1;
}



#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable:4706)
#endif


#define MXF_BASIC_TYPE_DEF(id, name, size) \
    CHK_OFAIL(mxf_register_basic_type(newDataModel, name, id, size));    

#define MXF_ARRAY_TYPE_DEF(id, name, elementTypeId, fixedSize) \
    CHK_OFAIL(mxf_register_array_type(newDataModel, name, id, elementTypeId, fixedSize));    

#define MXF_COMPOUND_TYPE_DEF(id, name) \
    CHK_OFAIL(itemType = mxf_register_compound_type(newDataModel, name, id));    

#define MXF_COMPOUND_TYPE_MEMBER(name, typeId) \
    CHK_OFAIL(mxf_register_compound_type_member(itemType, name, typeId));    

#define MXF_INTERPRETED_TYPE_DEF(id, name, typeId, fixedSize) \
    CHK_OFAIL(mxf_register_interpret_type(newDataModel, name, id, typeId, fixedSize));    

    
#define MXF_SET_DEFINITION(parentName, name, label) \
    CHK_OFAIL(mxf_register_set_def(newDataModel, #name, &MXF_SET_K(parentName), &MXF_SET_K(name)));
    
#define MXF_ITEM_DEFINITION(setName, name, label, tag, typeId, isRequired) \
    CHK_OFAIL(mxf_register_item_def(newDataModel, #name, &MXF_SET_K(setName), &MXF_ITEM_K(setName, name), tag, typeId, isRequired));


int mxf_load_data_model(MXFDataModel** dataModel)
{
    MXFDataModel* newDataModel;
    MXFItemType* itemType = NULL;
    
    CHK_MALLOC_ORET(newDataModel, MXFDataModel);
    memset(newDataModel, 0, sizeof(MXFDataModel));
    mxf_initialise_list(&newDataModel->itemDefs, free_item_def_in_list); 
    mxf_initialise_list(&newDataModel->setDefs, free_set_def_in_list); 
    
#define KEEP_DATA_MODEL_DEFS 1
#include <mxf/mxf_baseline_data_model.h>

#undef KEEP_DATA_MODEL_DEFS
#include <mxf/mxf_extensions_data_model.h>

    *dataModel = newDataModel;
    return 1;
    
fail:
    mxf_free_data_model(&newDataModel);
    return 0;
}

void mxf_free_data_model(MXFDataModel** dataModel)
{
    size_t i;

    if (*dataModel == NULL)
    {
        return;
    }
    
    mxf_clear_list(&(*dataModel)->setDefs);
    mxf_clear_list(&(*dataModel)->itemDefs);
    
    for (i = 0; i < sizeof((*dataModel)->types) / sizeof(MXFItemType); i++)
    {
        clear_type(&(*dataModel)->types[i]);
    }
    
    SAFE_FREE(dataModel);
}

int mxf_register_set_def(MXFDataModel* dataModel, const char* name, const mxfKey* parentKey, 
    const mxfKey* key)
{
    MXFSetDef* newSetDef = NULL;
    
    CHK_MALLOC_ORET(newSetDef, MXFSetDef);
    memset(newSetDef, 0, sizeof(MXFSetDef));
    if (name != NULL)
    {
        CHK_MALLOC_ARRAY_OFAIL(newSetDef->name, char, strlen(name) + 1); 
        strcpy(newSetDef->name, name);
    }
    newSetDef->parentSetDefKey = *parentKey;
    newSetDef->key = *key;
    mxf_initialise_list(&newSetDef->itemDefs, NULL);
    
    CHK_OFAIL(add_set_def(dataModel, newSetDef));
    
    return 1;
    
fail:
    free_set_def(&newSetDef);
    return 0;
}

int mxf_register_item_def(MXFDataModel* dataModel, const char* name, const mxfKey* setKey, 
    const mxfKey* key, mxfLocalTag tag, unsigned int typeId, int isRequired)
{
    MXFItemDef* newItemDef = NULL;
    
    CHK_MALLOC_ORET(newItemDef, MXFItemDef);
    memset(newItemDef, 0, sizeof(MXFItemDef));
    if (name != NULL)
    {
        CHK_MALLOC_ARRAY_OFAIL(newItemDef->name, char, strlen(name) + 1); 
        strcpy(newItemDef->name, name);
    }
    newItemDef->setDefKey = *setKey;
    newItemDef->key = *key;
    newItemDef->localTag = tag;
    newItemDef->typeId = typeId;
    newItemDef->isRequired = isRequired;
    
    CHK_OFAIL(add_item_def(dataModel, newItemDef));
    
    return 1;
    
fail:
    free_item_def(&newItemDef);
    return 0;
}


MXFItemType* mxf_register_basic_type(MXFDataModel* dataModel, const char* name, unsigned int typeId, unsigned int size)
{
    MXFItemType* type;
    
    /* basic types can only be built-in */
    CHK_ORET(typeId > 0 && typeId < MXF_EXTENSION_TYPE);
    
    /* check the type id is valid and free */
    CHK_ORET(typeId < sizeof(dataModel->types) / sizeof(MXFItemType) && 
        dataModel->types[typeId].typeId == 0);
    
    type = &dataModel->types[typeId];
    type->typeId = typeId; /* set first to indicate type is present */
    type->category = MXF_BASIC_TYPE_CAT;
    if (name != NULL)
    {
        CHK_MALLOC_ARRAY_OFAIL(type->name, char, strlen(name) + 1); 
        strcpy(type->name, name);
    }
    type->info.basic.size = size;
    
    return type;

fail:  
    clear_type(type);
    return NULL;
}

MXFItemType* mxf_register_array_type(MXFDataModel* dataModel, const char* name, unsigned int typeId, unsigned int elementTypeId, unsigned int fixedSize)
{
    unsigned int actualTypeId;
    MXFItemType* type;
    
    if (typeId <= 0)
    {
        actualTypeId = get_type_id(dataModel);
    }
    else
    {
        /* check the type id is valid and free */
        CHK_ORET(typeId < sizeof(dataModel->types) / sizeof(MXFItemType) && 
            dataModel->types[typeId].typeId == 0);
        actualTypeId = typeId;
    }
    
    type = &dataModel->types[actualTypeId];
    type->typeId = actualTypeId; /* set first to indicate type is present */
    type->category = MXF_ARRAY_TYPE_CAT;
    if (name != NULL)
    {
        CHK_MALLOC_ARRAY_OFAIL(type->name, char, strlen(name) + 1); 
        strcpy(type->name, name);
    }
    type->info.array.elementTypeId = elementTypeId;
    type->info.array.fixedSize = fixedSize;
    
    return type;

fail:    
    clear_type(type);
    return NULL;
}

MXFItemType* mxf_register_compound_type(MXFDataModel* dataModel, const char* name, unsigned int typeId)
{
    unsigned int actualTypeId;
    MXFItemType* type = NULL;
    
    if (typeId == 0)
    {
        actualTypeId = get_type_id(dataModel);
    }
    else
    {
        /* check the type id is valid and free */
        CHK_ORET(typeId < sizeof(dataModel->types) / sizeof(MXFItemType) && 
            dataModel->types[typeId].typeId == 0);
        actualTypeId = typeId;
    }
    
    type = &dataModel->types[actualTypeId];
    type->typeId = actualTypeId; /* set first to indicate type is present */
    type->category = MXF_COMPOUND_TYPE_CAT;
    if (name != NULL)
    {
        CHK_MALLOC_ARRAY_OFAIL(type->name, char, strlen(name) + 1); 
        strcpy(type->name, name);
    }
    memset(type->info.compound.members, 0, sizeof(type->info.compound.members));
    
    return type;
    
fail:
    clear_type(type);
    return NULL;
}

int mxf_register_compound_type_member(MXFItemType* type, const char* memberName, unsigned int memberTypeId)
{
    size_t memberIndex;
    size_t maxMembers = sizeof(type->info.compound.members) / sizeof(MXFCompoundTypeMemberInfo) - 1;
    
    /* find null terminator (eg. typeId == 0) */
    for (memberIndex = 0; memberIndex < maxMembers; memberIndex++)
    {
        if (type->info.compound.members[memberIndex].typeId == 0)
        {
            break;
        }
    }
    if (memberIndex == maxMembers)
    {
        mxf_log_error("Number of compound item type members exceeds hardcoded maximum %d" 
            LOG_LOC_FORMAT, maxMembers, LOG_LOC_PARAMS);
        return 0;
    }
    
    
    /* set member info */
    CHK_MALLOC_ARRAY_ORET(type->info.compound.members[memberIndex].name, char, strlen(memberName) + 1); 
    strcpy(type->info.compound.members[memberIndex].name, memberName);
    type->info.compound.members[memberIndex].typeId = memberTypeId; /* set last when everything ok */

    return 1;    
}

MXFItemType* mxf_register_interpret_type(MXFDataModel* dataModel, const char* name, unsigned int typeId, 
    unsigned int interpretedTypeId, unsigned int fixedArraySize)
{
    unsigned int actualTypeId;
    MXFItemType* type;
    
    if (typeId == 0)
    {
        actualTypeId = get_type_id(dataModel);
    }
    else
    {
        /* check the type id is valid and free */
        CHK_ORET(typeId < sizeof(dataModel->types) / sizeof(MXFItemType) && 
            dataModel->types[typeId].typeId == 0);
        actualTypeId = typeId;
    }
    
    type = &dataModel->types[actualTypeId];
    type->typeId = actualTypeId; /* set first to indicate type is present */
    type->category = MXF_INTERPRET_TYPE_CAT;
    if (name != NULL)
    {
        CHK_MALLOC_ARRAY_OFAIL(type->name, char, strlen(name) + 1); 
        strcpy(type->name, name);
    }
    type->info.interpret.typeId = interpretedTypeId;
    type->info.interpret.fixedArraySize = fixedArraySize;
    
    return type;
    
fail:    
    clear_type(type);
    return NULL;
}


int mxf_finalise_data_model(MXFDataModel* dataModel)
{
    MXFListIterator iter;
    MXFItemDef* itemDef;
    MXFSetDef* setDef;

    /* reset set defs and set the parent set def if the parent set def key != g_Null_Key */
    mxf_initialise_list_iter(&iter, &dataModel->setDefs);
    while (mxf_next_list_iter_element(&iter))
    {
        setDef = (MXFSetDef*)mxf_get_iter_element(&iter);
        mxf_clear_list(&setDef->itemDefs);
        setDef->parentSetDef = NULL;

        if (!mxf_equals_key(&setDef->parentSetDefKey, &g_Null_Key))
        {
            CHK_ORET(mxf_find_set_def(dataModel, &setDef->parentSetDefKey, &setDef->parentSetDef));
        }
    }
    
    /* add item defs to owner set def */
    mxf_initialise_list_iter(&iter, &dataModel->itemDefs);
    while (mxf_next_list_iter_element(&iter))
    {
        itemDef = (MXFItemDef*)mxf_get_iter_element(&iter);
        CHK_ORET(mxf_find_set_def(dataModel, &itemDef->setDefKey, &setDef));
        CHK_ORET(mxf_append_list_element(&setDef->itemDefs, (void*)itemDef));
    }
    
    return 1;
}

int mxf_check_data_model(MXFDataModel* dataModel)
{
    MXFListIterator iter1;
    MXFListIterator iter2;
    MXFSetDef* setDef1;
    MXFSetDef* setDef2;
    MXFItemDef* itemDef1;
    MXFItemDef* itemDef2;
    long listIndex;

    
    /* check that the set defs are unique */
    listIndex = 0;
    mxf_initialise_list_iter(&iter1, &dataModel->setDefs);
    while (mxf_next_list_iter_element(&iter1))
    {
        setDef1 = (MXFSetDef*)mxf_get_iter_element(&iter1);

        /* check with set defs with higher index in list */        
        mxf_initialise_list_iter_at(&iter2, &dataModel->setDefs, listIndex + 1);
        while (mxf_next_list_iter_element(&iter2))
        {
            setDef2 = (MXFSetDef*)mxf_get_iter_element(&iter2);
            if (mxf_equals_key(&setDef1->key, &setDef2->key))
            {
                char keyStr[KEY_STR_SIZE];
                mxf_sprint_key(keyStr, &setDef1->key);
                mxf_log_warn("Duplicate set def found. Key = %s" 
                    LOG_LOC_FORMAT, keyStr, LOG_LOC_PARAMS); 
                return 0;
            }
        }
        listIndex++;
    }

    /* check that the item defs are unique (both key and static local tag),
       , that the item def is contained in a set def
       and the item type is known */
    listIndex = 0;
    mxf_initialise_list_iter(&iter1, &dataModel->itemDefs);
    while (mxf_next_list_iter_element(&iter1))
    {
        itemDef1 = (MXFItemDef*)mxf_get_iter_element(&iter1);

        /* check item def is contained in a set def */
        if (mxf_equals_key(&itemDef1->setDefKey, &g_Null_Key))
        {
            char keyStr[KEY_STR_SIZE];
            mxf_sprint_key(keyStr, &itemDef1->key);
            mxf_log_warn("Found item def not contained in any set def. Key = %s" 
                LOG_LOC_FORMAT, keyStr, LOG_LOC_PARAMS); 
            return 0;
        }
        
        /* check with item defs with higher index in list */        
        mxf_initialise_list_iter_at(&iter2, &dataModel->itemDefs, listIndex + 1);
        while (mxf_next_list_iter_element(&iter2))
        {
            itemDef2 = (MXFItemDef*)mxf_get_iter_element(&iter2);
            if (mxf_equals_key(&itemDef1->key, &itemDef2->key))
            {
                char keyStr[KEY_STR_SIZE];
                mxf_sprint_key(keyStr, &itemDef1->key);
                mxf_log_warn("Duplicate item def found. Key = %s" 
                    LOG_LOC_FORMAT, keyStr, LOG_LOC_PARAMS); 
                return 0;
            }
            if (itemDef1->localTag != 0 && itemDef1->localTag == itemDef2->localTag)
            {
                char keyStr[KEY_STR_SIZE];
                mxf_sprint_key(keyStr, &itemDef1->key);
                mxf_log_warn("Duplicate item def local tag found. LocalTag = 0x%04x, Key = %s" 
                    LOG_LOC_FORMAT, itemDef1->localTag, keyStr, LOG_LOC_PARAMS); 
                return 0;
            }
        }
        
        /* check item type is valid and known */
        if (mxf_get_item_def_type(dataModel, itemDef1->typeId) == NULL)
        {
            char keyStr[KEY_STR_SIZE];
            mxf_sprint_key(keyStr, &itemDef1->key);
            mxf_log_warn("Item def has unknown type (%d). LocalTag = 0x%04x, Key = %s" 
                LOG_LOC_FORMAT, itemDef1->typeId, itemDef1->localTag, keyStr, LOG_LOC_PARAMS); 
            return 0;
        }
        
        listIndex++;
    }
    
    return 1;
}

int mxf_find_set_def(MXFDataModel* dataModel, const mxfKey* key, MXFSetDef** setDef)
{
    void* result;
    
    if ((result = mxf_find_list_element(&dataModel->setDefs, (void*)key, set_def_eq)) != NULL)
    {
        *setDef = (MXFSetDef*)result;
        return 1;
    }

    return 0;
}

int mxf_find_item_def(MXFDataModel* dataModel, const mxfKey* key, MXFItemDef** itemDef)
{
    void* result;
    
    if ((result = mxf_find_list_element(&dataModel->itemDefs, (void*)key, item_def_eq)) != NULL)
    {
        *itemDef = (MXFItemDef*)result;
        return 1;
    }

    return 0;
}

int mxf_find_item_def_in_set_def(const mxfKey* key, const MXFSetDef* setDef, MXFItemDef** itemDef)
{
    void* result;
    
    if ((result = mxf_find_list_element(&setDef->itemDefs, (void*)key, item_def_eq)) != NULL)
    {
        *itemDef = (MXFItemDef*)result;
        return 1;
    }
    
    if (setDef->parentSetDef != NULL)
    {
        return mxf_find_item_def_in_set_def(key, setDef->parentSetDef, itemDef);
    }
    
    return 0;
}


MXFItemType* mxf_get_item_def_type(MXFDataModel* dataModel, unsigned int typeId)
{
    if (typeId == 0 || typeId >= sizeof(dataModel->types) / sizeof(MXFItemType))
    {
        return NULL;
    }
    if (dataModel->types[typeId].typeId == MXF_UNKNOWN_TYPE)
    {
        return NULL;
    }
    
    return &dataModel->types[typeId];
}



int mxf_is_subclass_of(MXFDataModel* dataModel, const mxfKey* setKey, const mxfKey* parentSetKey)
{
    MXFSetDef* setDef;

    if (mxf_equals_key(setKey, parentSetKey))
    {
        return 1;
    }
    
    if (!mxf_find_set_def(dataModel, setKey, &setDef))
    {
        return 0;
    }

    return mxf_is_subclass_of_2(dataModel, setDef, parentSetKey);
}

int mxf_is_subclass_of_2(MXFDataModel* dataModel, MXFSetDef* setDef, const mxfKey* parentSetKey)
{
    if (mxf_equals_key(&setDef->key, parentSetKey))
    {
        return 1;
    }

    if (setDef->parentSetDef == NULL ||
        mxf_equals_key(&setDef->key, &setDef->parentSetDefKey))
    {
        return 0;
    }

    return mxf_is_subclass_of_2(dataModel, setDef->parentSetDef, parentSetKey);
}


int mxf_clone_set_def(MXFDataModel* fromDataModel, MXFSetDef* fromSetDef,
                      MXFDataModel* toDataModel, MXFSetDef** toSetDef)
{
    MXFSetDef* clonedSetDef = NULL;
    MXFSetDef* toParentSetDef = NULL;
    MXFListIterator iter;
    MXFItemDef* fromItemDef;
    MXFItemDef* toItemDef = NULL;
    
    if (!mxf_find_set_def(toDataModel, &fromSetDef->key, &clonedSetDef))
    {
        if (fromSetDef->parentSetDef)
        {
            CHK_ORET(mxf_clone_set_def(fromDataModel, fromSetDef->parentSetDef, toDataModel, &toParentSetDef));
        }
        
        CHK_ORET(mxf_register_set_def(toDataModel, fromSetDef->name, &fromSetDef->parentSetDefKey, &fromSetDef->key));
        clonedSetDef = (MXFSetDef*)mxf_get_last_list_element(&toDataModel->setDefs);
        clonedSetDef->parentSetDef = toParentSetDef;
    }
    
    mxf_initialise_list_iter(&iter, &fromSetDef->itemDefs);
    while (mxf_next_list_iter_element(&iter))
    {
        fromItemDef = (MXFItemDef*)mxf_get_iter_element(&iter);
        
        if (mxf_find_item_def_in_set_def(&fromItemDef->key, clonedSetDef, &toItemDef))
        {
            continue;
        }
        
        CHK_ORET(clone_item_def(fromDataModel, fromItemDef, toDataModel, &toItemDef));
        CHK_ORET(mxf_append_list_element(&clonedSetDef->itemDefs, (void*)toItemDef));
    }
    
    *toSetDef = clonedSetDef;
    return 1;
}

