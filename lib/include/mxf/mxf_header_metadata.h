/*
 * $Id: mxf_header_metadata.h,v 1.4 2010/06/25 13:59:02 philipn Exp $
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
 
#ifndef __MXF_HEADER_METADATA_H__
#define __MXF_HEADER_METADATA_H__


#ifdef __cplusplus
extern "C" 
{
#endif


typedef struct
{
    mxfKey key;
    uint16_t tag;
    int isPersistent;
    uint16_t length;
    uint8_t* value;
    struct _MXFMetadataSet* set;
} MXFMetadataItem;

typedef struct _MXFMetadataSet
{
    mxfKey key;
    mxfUUID instanceUID;
    MXFList items;
    struct _MXFHeaderMetadata* headerMetadata;
    uint64_t fixedSpaceAllocation;
} MXFMetadataSet;

typedef struct _MXFHeaderMetadata
{
    MXFDataModel* dataModel;
    MXFPrimerPack* primerPack;
    MXFList sets;
} MXFHeaderMetadata;

typedef struct
{
    MXFMetadataItem* item;
    uint32_t numElements;
    uint32_t elementLength;
    uint32_t currentElement;
} MXFArrayItemIterator;


typedef struct
{
    int (*before_set_read) (void* privateData, MXFHeaderMetadata* headerMetadata, 
        const mxfKey* key, uint8_t llen, uint64_t len, int* skip);
    int (*after_set_read) (void* privateData, MXFHeaderMetadata* headerMetadata, MXFMetadataSet* set, 
        int* skip);
    void* privateData;
} MXFReadFilter;


int mxf_is_header_metadata(const mxfKey* key);


int mxf_create_header_metadata(MXFHeaderMetadata** headerMetadata, MXFDataModel* dataModel);
int mxf_create_set(MXFHeaderMetadata* headerMetadata, const mxfKey* key, MXFMetadataSet** set);
int mxf_create_item(MXFMetadataSet* set, const mxfKey* key, mxfLocalTag tag, MXFMetadataItem** item);
void mxf_free_header_metadata(MXFHeaderMetadata** headerMetadata);
void mxf_free_set(MXFMetadataSet** set);
void mxf_free_item(MXFMetadataItem** item);

void mxf_set_fixed_set_space_allocation(MXFMetadataSet* set, uint64_t size);

int mxf_add_set(MXFHeaderMetadata* headerMetadata, MXFMetadataSet* set);

int mxf_register_item(MXFHeaderMetadata* headerMetadata, const mxfKey* key);
int mxf_register_set_items(MXFHeaderMetadata* headerMetadata, const mxfKey* key);

int mxf_remove_set(MXFHeaderMetadata* headerMetadata, MXFMetadataSet* set);
int mxf_remove_item(MXFMetadataSet* set, const mxfKey* itemKey, MXFMetadataItem** item);

int mxf_find_set_by_key(MXFHeaderMetadata* headerMetadata, const mxfKey* key, MXFList** setList);
int mxf_find_singular_set_by_key(MXFHeaderMetadata* headerMetadata, const mxfKey* key, MXFMetadataSet** set);
int mxf_get_item(MXFMetadataSet* set, const mxfKey* key, MXFMetadataItem** resultItem);
int mxf_have_item(MXFMetadataSet* set, const mxfKey* key);

int mxf_set_is_subclass_of(MXFMetadataSet* set, const mxfKey* parentSetKey);

int mxf_clone_set(MXFMetadataSet* fromSet, MXFHeaderMetadata* toHeaderMetadata, MXFMetadataSet** toSet);


int mxf_read_header_metadata(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata,
    uint64_t headerByteCount, const mxfKey* key, uint8_t llen, uint64_t len);
int mxf_read_filtered_header_metadata(MXFFile* mxfFile, MXFReadFilter* filter, 
    MXFHeaderMetadata* headerMetadata, uint64_t headerByteCount, const mxfKey* key, uint8_t llen, uint64_t len);
int mxf_read_set(MXFFile* mxfFile, const mxfKey* key, uint64_t len,
    MXFHeaderMetadata* headerMetadata, int addToHeaderMetadata);
/* returns 1 on success, 0 for failure, 2 if it is an unknown set and "set" parameter is set to NULL */
int mxf_read_and_return_set(MXFFile* mxfFile, const mxfKey* key, uint64_t len,
    MXFHeaderMetadata* headerMetadata, int addToHeaderMetadata, MXFMetadataSet** set);
int mxf_read_item_tl(MXFFile* mxfFile, mxfLocalTag* itemTag, uint16_t* itemLen);
int mxf_read_item(MXFFile* mxfFile, MXFMetadataItem* item, uint16_t len);
    
int mxf_write_header_metadata(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata);
int mxf_write_header_primer_pack(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata);
int mxf_write_header_sets(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata);
int mxf_write_set(MXFFile* mxfFile, MXFMetadataSet* set);
int mxf_write_item(MXFFile* mxfFile, MXFMetadataItem* item);
void mxf_get_header_metadata_size(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata, uint64_t* size);
uint64_t mxf_get_set_size(MXFFile* mxfFile, MXFMetadataSet* set);


void mxf_get_uint8(const uint8_t* value, uint8_t* result);
void mxf_get_uint16(const uint8_t* value, uint16_t* result);
void mxf_get_uint32(const uint8_t* value, uint32_t* result);
void mxf_get_uint64(const uint8_t* value, uint64_t* result);
void mxf_get_int8(const uint8_t* value, int8_t* result);
void mxf_get_int16(const uint8_t* value, int16_t* result);
void mxf_get_int32(const uint8_t* value, int32_t* result);
void mxf_get_int64(const uint8_t* value, int64_t* result);
void mxf_get_version_type(const uint8_t* value, mxfVersionType* result);
void mxf_get_uuid(const uint8_t* value, mxfUUID* result);
void mxf_get_ul(const uint8_t* value, mxfUL* result);
void mxf_get_auid(const uint8_t* value, mxfAUID* result);
void mxf_get_umid(const uint8_t* value, mxfUMID* result);
void mxf_get_timestamp(const uint8_t* value, mxfTimestamp* result);
void mxf_get_length(const uint8_t* value, mxfLength* result);
void mxf_get_rational(const uint8_t* value, mxfRational* result);
void mxf_get_position(const uint8_t* value, mxfPosition* result);
void mxf_get_boolean(const uint8_t* value, mxfBoolean* result);
void mxf_get_product_version(const uint8_t* value, mxfProductVersion* result);
void mxf_get_rgba_layout_component(const uint8_t* value, mxfRGBALayoutComponent* result);
void mxf_get_array_header(const uint8_t* value, uint32_t* arrayLen, uint32_t* arrayItemLen);
uint16_t mxf_get_utf16string_size(const uint8_t* value, uint16_t valueLen);
void mxf_get_utf16string(const uint8_t* value, uint16_t valueLen, mxfUTF16Char* result);

int mxf_get_strongref(MXFHeaderMetadata* headerMetadata, const uint8_t* value, MXFMetadataSet** set);
int mxf_get_weakref(MXFHeaderMetadata* headerMetadata, const uint8_t* value, MXFMetadataSet** set);
int mxf_dereference(MXFHeaderMetadata* headerMetadata, const mxfUUID* uuid, MXFMetadataSet** set);

void initialise_sets_iter(MXFHeaderMetadata* headerMetadata, MXFListIterator* setsIter);
int mxf_get_strongref_s(MXFHeaderMetadata* headerMetadata, MXFListIterator* setsIter, const uint8_t* value, MXFMetadataSet** set);
int mxf_get_weakref_s(MXFHeaderMetadata* headerMetadata, MXFListIterator* setsIter, const uint8_t* value, MXFMetadataSet** set);
int mxf_dereference_s(MXFHeaderMetadata* headerMetadata, MXFListIterator* setsIter, const mxfUUID* uuid, MXFMetadataSet** set);

    
void mxf_set_uint8(uint8_t value, uint8_t* result);
void mxf_set_uint16(uint16_t value, uint8_t* result);
void mxf_set_uint32(uint32_t value, uint8_t* result);
void mxf_set_uint64(uint64_t value, uint8_t* result);
void mxf_set_int8(int8_t value, uint8_t* result);
void mxf_set_int16(int16_t value, uint8_t* result);
void mxf_set_int32(int32_t value, uint8_t* result);
void mxf_set_int64(int64_t value, uint8_t* result);
void mxf_set_version_type(mxfVersionType value, uint8_t* result);
void mxf_set_uuid(const mxfUUID* value, uint8_t* result);
void mxf_set_ul(const mxfUL* value, uint8_t* result);
void mxf_set_auid(const mxfAUID* value, uint8_t* result);
void mxf_set_umid(const mxfUMID* value, uint8_t* result);
void mxf_set_timestamp(const mxfTimestamp* value, uint8_t* result);
uint16_t mxf_get_external_utf16string_size(const mxfUTF16Char* value);
void mxf_set_utf16string(const mxfUTF16Char* value, uint8_t* result);
void mxf_set_fixed_size_utf16string(const mxfUTF16Char* value, uint16_t size, uint8_t* result);
void mxf_set_strongref(const MXFMetadataSet* value, uint8_t* result);
void mxf_set_weakref(const MXFMetadataSet* value, uint8_t* result);
void mxf_set_rational(const mxfRational* value, uint8_t* result);
void mxf_set_position(mxfPosition value, uint8_t* result);
void mxf_set_length(mxfLength value, uint8_t* result);
void mxf_set_boolean(mxfBoolean value, uint8_t* result);
void mxf_set_product_version(const mxfProductVersion* value, uint8_t* result);
void mxf_set_rgba_layout_component(const mxfRGBALayoutComponent* value, uint8_t* result);
void mxf_set_array_header(uint32_t arrayLen, uint32_t arrayElementLen, uint8_t* result);


int mxf_set_item_value(MXFMetadataItem* item, const uint8_t* value, uint16_t len);

int mxf_set_item(MXFMetadataSet* set, const mxfKey* itemKey, const uint8_t* value, uint16_t len);
int mxf_clone_item(MXFMetadataSet* sourceSet, const mxfKey* itemKey, MXFMetadataSet* destSet);

int mxf_set_uint8_item(MXFMetadataSet* set, const mxfKey* itemKey, uint8_t value);
int mxf_set_uint16_item(MXFMetadataSet* set, const mxfKey* itemKey, uint16_t value);
int mxf_set_uint32_item(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t value);
int mxf_set_uint64_item(MXFMetadataSet* set, const mxfKey* itemKey, uint64_t value);
int mxf_set_int8_item(MXFMetadataSet* set, const mxfKey* itemKey, int8_t value);
int mxf_set_int16_item(MXFMetadataSet* set, const mxfKey* itemKey, int16_t value);
int mxf_set_int32_item(MXFMetadataSet* set, const mxfKey* itemKey, int32_t value);
int mxf_set_int64_item(MXFMetadataSet* set, const mxfKey* itemKey, int64_t value);
int mxf_set_version_type_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfVersionType value);
int mxf_set_uuid_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfUUID* value);
int mxf_set_ul_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfUL* value);
int mxf_set_auid_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfAUID* value);
int mxf_set_umid_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfUMID* value);
int mxf_set_timestamp_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfTimestamp* value);
int mxf_set_utf16string_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfUTF16Char* value);
int mxf_set_fixed_size_utf16string_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfUTF16Char* value, 
    uint16_t size);
int mxf_set_strongref_item(MXFMetadataSet* set, const mxfKey* itemKey, const MXFMetadataSet* value);
int mxf_set_weakref_item(MXFMetadataSet* set, const mxfKey* itemKey, const MXFMetadataSet* value);
int mxf_set_rational_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfRational* value);
int mxf_set_position_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfPosition value);
int mxf_set_length_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfLength value);
int mxf_set_boolean_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfBoolean value);
int mxf_set_product_version_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfProductVersion* value);
int mxf_set_rgba_layout_component_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfRGBALayoutComponent* value);

int mxf_alloc_array_item_elements(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t elementLen, 
    uint32_t count, uint8_t** elements);
int mxf_grow_array_item(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t elementLen, 
    uint32_t count, uint8_t** newElements);
int mxf_set_empty_array_item(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t elementLen);
int mxf_add_array_item_strongref(MXFMetadataSet* set, const mxfKey* itemKey, const MXFMetadataSet* value);
int mxf_add_array_item_weakref(MXFMetadataSet* set, const mxfKey* itemKey, const MXFMetadataSet* value);


int mxf_get_item_len(MXFMetadataSet* set, const mxfKey* itemKey, uint16_t* len);

int mxf_get_uint8_item(MXFMetadataSet* set, const mxfKey* itemKey, uint8_t* value);
int mxf_get_uint16_item(MXFMetadataSet* set, const mxfKey* itemKey, uint16_t* value);
int mxf_get_uint32_item(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t* value);
int mxf_get_uint64_item(MXFMetadataSet* set, const mxfKey* itemKey, uint64_t* value);
int mxf_get_int8_item(MXFMetadataSet* set, const mxfKey* itemKey, int8_t* value);
int mxf_get_int16_item(MXFMetadataSet* set, const mxfKey* itemKey, int16_t* value);
int mxf_get_int32_item(MXFMetadataSet* set, const mxfKey* itemKey, int32_t* value);
int mxf_get_int64_item(MXFMetadataSet* set, const mxfKey* itemKey, int64_t* value);
int mxf_get_version_type_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfVersionType* value);
int mxf_get_uuid_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfUUID* value);
int mxf_get_ul_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfUL* value);
int mxf_get_auid_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfAUID* value);
int mxf_get_umid_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfUMID* value);
int mxf_get_timestamp_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfTimestamp* value);
int mxf_get_utf16string_item_size(MXFMetadataSet* set, const mxfKey* itemKey, uint16_t* size);
int mxf_get_utf16string_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfUTF16Char* value);
int mxf_get_strongref_item(MXFMetadataSet* set, const mxfKey* itemKey, MXFMetadataSet** value);
int mxf_get_weakref_item(MXFMetadataSet* set, const mxfKey* itemKey, MXFMetadataSet** value);
int mxf_get_strongref_item_s(MXFListIterator* setsIter, MXFMetadataSet* set, const mxfKey* itemKey, MXFMetadataSet** value);
int mxf_get_weakref_item_s(MXFListIterator* setsIter, MXFMetadataSet* set, const mxfKey* itemKey, MXFMetadataSet** value);
int mxf_get_length_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfLength* value);
int mxf_get_rational_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfRational* value);
int mxf_get_position_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfPosition* value);
int mxf_get_boolean_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfBoolean* value);
int mxf_get_product_version_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfProductVersion* value);
int mxf_get_rgba_layout_component_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfRGBALayoutComponent* value);

int mxf_get_array_item_count(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t* count);
int mxf_get_array_item_element_len(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t* elementLen);
int mxf_get_array_item_element(MXFMetadataSet* set, const mxfKey* itemKey, uint32_t index, uint8_t** element);

int mxf_initialise_array_item_iterator(MXFMetadataSet* set, const mxfKey* itemKey, MXFArrayItemIterator* arrayIter);
int mxf_next_array_item_element(MXFArrayItemIterator* arrayIter, uint8_t** value, uint32_t* length);


#ifdef __cplusplus
}
#endif


#endif



