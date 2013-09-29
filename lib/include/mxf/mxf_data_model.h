/*
 * $Id: mxf_data_model.h,v 1.8 2010/06/25 13:59:02 philipn Exp $
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
 
#ifndef __MXF_DATA_MODEL_H__
#define __MXF_DATA_MODEL_H__


#ifdef __cplusplus
extern "C" 
{
#endif


#define MXF_SET_K(name) \
    g_##name##_set_key

#define MXF_ITEM_K(setname, name) \
    g_##setname##_##name##_item_key

typedef enum
{
    MXF_BASIC_TYPE_CAT,
    MXF_ARRAY_TYPE_CAT,
    MXF_COMPOUND_TYPE_CAT,
    MXF_INTERPRET_TYPE_CAT
} MXFItemTypeCategory;

typedef struct
{
    unsigned int size;
} MXFBasicTypeInfo;

typedef struct
{
    unsigned int elementTypeId;
    unsigned int fixedSize;
} MXFArrayTypeInfo;

typedef struct
{
    char* name;
    unsigned int typeId;
} MXFCompoundTypeMemberInfo;

typedef struct
{
    MXFCompoundTypeMemberInfo members[16]; /* array terminated by typeId == 0 */
} MXFCompoundTypeInfo;

typedef struct
{
    unsigned int typeId;
    unsigned int fixedArraySize; /* only used if interpret array types */
} MXFInterpretTypeInfo;

typedef struct
{
    MXFItemTypeCategory category;
    unsigned int typeId;
    char* name;
    union
    {
        MXFBasicTypeInfo basic;
        MXFArrayTypeInfo array;
        MXFCompoundTypeInfo compound;
        MXFInterpretTypeInfo interpret;
    } info;
} MXFItemType;
    
typedef struct
{
    char* name;
    mxfKey setDefKey;
    mxfKey key;
    mxfLocalTag localTag;
    unsigned int typeId;
    int isRequired;
} MXFItemDef;

typedef struct _MXFSetDef
{
    char* name;
    mxfKey parentSetDefKey;
    mxfKey key;
    MXFList itemDefs;
    struct _MXFSetDef* parentSetDef;
} MXFSetDef;

typedef struct
{
    MXFList itemDefs;
    MXFList setDefs;
    MXFItemType types[128]; /* index 0 is not used */
    unsigned int lastTypeId;
} MXFDataModel;


/* built-in types */

typedef enum
{
    MXF_UNKNOWN_TYPE = 0,
    
    /* basic */
    MXF_INT8_TYPE,
    MXF_INT16_TYPE,
    MXF_INT32_TYPE,
    MXF_INT64_TYPE,
    MXF_UINT8_TYPE,
    MXF_UINT16_TYPE,
    MXF_UINT32_TYPE,
    MXF_UINT64_TYPE,
    MXF_RAW_TYPE,
    
    /* array */
    MXF_UTF16STRING_TYPE,
    MXF_UTF16STRINGARRAY_TYPE,
    MXF_INT32ARRAY_TYPE,
    MXF_UINT32ARRAY_TYPE,
    MXF_INT64ARRAY_TYPE,
    MXF_UINT8ARRAY_TYPE,
    MXF_ISO7STRING_TYPE,
    MXF_INT32BATCH_TYPE,
    MXF_UINT32BATCH_TYPE,
    MXF_AUIDARRAY_TYPE,
    MXF_ULARRAY_TYPE,
    MXF_ULBATCH_TYPE,
    MXF_STRONGREFARRAY_TYPE,
    MXF_STRONGREFBATCH_TYPE,
    MXF_WEAKREFARRAY_TYPE,
    MXF_WEAKREFBATCH_TYPE,
    MXF_RATIONALARRAY_TYPE,
    MXF_RGBALAYOUT_TYPE,
    
    /* compound */
    MXF_RATIONAL_TYPE,
    MXF_TIMESTAMP_TYPE,
    MXF_PRODUCTVERSION_TYPE,
    MXF_INDIRECT_TYPE,
    MXF_RGBALAYOUTCOMPONENT_TYPE,
    
    /* interpret */
    MXF_VERSIONTYPE_TYPE,
    MXF_UTF16_TYPE,
    MXF_BOOLEAN_TYPE,
    MXF_ISO7_TYPE,
    MXF_LENGTH_TYPE,
    MXF_POSITION_TYPE,
    MXF_RGBACODE_TYPE,
    MXF_STREAM_TYPE,
    MXF_DATAVALUE_TYPE,
    MXF_IDENTIFIER_TYPE,
    MXF_OPAQUE_TYPE,
    MXF_UMID_TYPE,
    MXF_UID_TYPE,
    MXF_UL_TYPE,
    MXF_UUID_TYPE,
    MXF_AUID_TYPE,
    MXF_PACKAGEID_TYPE,
    MXF_STRONGREF_TYPE,
    MXF_WEAKREF_TYPE,
    MXF_ORIENTATION_TYPE,
    MXF_CODED_CONTENT_TYPE_TYPE,
    
    MXF_EXTENSION_TYPE /* extension types must have integer value >= this */
    
} MXFItemTypeId;


/* declare the baseline set and item keys */

#define MXF_LABEL(d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15) \
    {d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15}

#define MXF_SET_DEFINITION(parentName, name, label) \
    static const mxfUL MXF_SET_K(name) = label;
    
#define MXF_ITEM_DEFINITION(setName, name, label, localTag, typeId, isRequired) \
    static const mxfUL MXF_ITEM_K(setName, name) = label;

#define KEEP_DATA_MODEL_DEFS 1
#include <mxf/mxf_baseline_data_model.h>

/* declare the extensions set and item keys */

#undef KEEP_DATA_MODEL_DEFS
#include <mxf/mxf_extensions_data_model.h>


int mxf_load_data_model(MXFDataModel** dataModel);
void mxf_free_data_model(MXFDataModel** dataModel);

int mxf_register_set_def(MXFDataModel* dataModel, const char* name, const mxfKey* parentKey, 
    const mxfKey* key);
int mxf_register_item_def(MXFDataModel* dataModel, const char* name, const mxfKey* setKey, 
    const mxfKey* key, mxfLocalTag tag, unsigned int typeId, int isRequired);

/* if the typeId parameter is 0 in the following functions, then a new id is created */
MXFItemType* mxf_register_basic_type(MXFDataModel* dataModel, const char* name, unsigned int typeId, unsigned int size);
MXFItemType* mxf_register_array_type(MXFDataModel* dataModel, const char* name, unsigned int typeId, unsigned int elementTypeId, unsigned int fixedSize);
MXFItemType* mxf_register_compound_type(MXFDataModel* dataModel, const char* name, unsigned int typeId);
/* adds a member to the end of the list */
int mxf_register_compound_type_member(MXFItemType* type, const char* memberName, unsigned int memberTypeId);
MXFItemType* mxf_register_interpret_type(MXFDataModel* dataModel, const char* name, unsigned int typeId, unsigned int interpretedTypeId, unsigned int fixedArraySize);


int mxf_finalise_data_model(MXFDataModel* dataModel);
int mxf_check_data_model(MXFDataModel* dataModel);

int mxf_find_set_def(MXFDataModel* dataModel, const mxfKey* key, MXFSetDef** setDef);
int mxf_find_item_def(MXFDataModel* dataModel, const mxfKey* key, MXFItemDef** itemDef);
int mxf_find_item_def_in_set_def(const mxfKey* key, const MXFSetDef* setDef, MXFItemDef** itemDef);

MXFItemType* mxf_get_item_def_type(MXFDataModel* dataModel, unsigned int typeId);

int mxf_is_subclass_of(MXFDataModel* dataModel, const mxfKey* setKey, const mxfKey* parentSetKey);
int mxf_is_subclass_of_2(MXFDataModel* dataModel, MXFSetDef* setDef, const mxfKey* parentSetKey);


int mxf_clone_set_def(MXFDataModel* fromDataModel, MXFSetDef* fromSetDef,
                      MXFDataModel* toDataModel, MXFSetDef** toSetDef);


#ifdef __cplusplus
}
#endif


#endif



