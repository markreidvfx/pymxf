/*
 * $Id: mxf_avid.h,v 1.13 2011/04/19 09:46:29 philipn Exp $
 *
 * Avid data model extensions and utilities
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
 
#ifndef __MXF_AVID_H__
#define __MXF_AVID_H__


#ifdef __cplusplus
extern "C" 
{
#endif


#include <mxf/mxf_avid_labels_and_keys.h>
#include <mxf/mxf_avid_metadictionary.h>
#include <mxf/mxf_avid_dictionary.h>


#define MXF_LABEL(d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15) \
    {d0, d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15}

#define MXF_SET_DEFINITION(parentName, name, label) \
    static const mxfUL MXF_SET_K(name) = label;
    
#define MXF_ITEM_DEFINITION(setName, name, label, localTag, typeId, isRequired) \
    static const mxfUL MXF_ITEM_K(setName, name) = label;

#include <mxf/mxf_avid_extensions_data_model.h>


typedef struct
{
    uint16_t red;
    uint16_t green;
    uint16_t blue;
} RGBColor;


typedef void (*mxf_generate_aafsdk_umid_func)(mxfUMID* umid);
typedef void (*mxf_generate_old_aafsdk_umid_func)(mxfUMID* umid);



int mxf_avid_load_extensions(MXFDataModel* dataModel);


int mxf_avid_read_filtered_header_metadata(MXFFile* mxfFile, int skipDataDefs, MXFHeaderMetadata* headerMetadata, 
    uint64_t headerByteCount, const mxfKey* key, uint8_t llen, uint64_t len);

int mxf_avid_write_header_metadata(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata, MXFPartition* headerPartition);


extern mxf_generate_aafsdk_umid_func mxf_generate_aafsdk_umid;
extern mxf_generate_old_aafsdk_umid_func mxf_generate_old_aafsdk_umid;

void mxf_default_generate_aafsdk_umid(mxfUMID* umid);
void mxf_default_generate_old_aafsdk_umid(mxfUMID* umid);

void mxf_avid_set_regtest_funcs();
void mxf_regtest_generate_aafsdk_umid(mxfUMID* umid);
void mxf_regtest_generate_old_aafsdk_umid(mxfUMID* umid);


int mxf_avid_set_indirect_string_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfUTF16Char* value);
int mxf_avid_set_indirect_int32_item(MXFMetadataSet *set, const mxfKey *itemKey, int32_t value);

int mxf_avid_set_rgb_color_item(MXFMetadataSet* set, const mxfKey* itemKey, const RGBColor* value);
int mxf_avid_get_rgb_color_item(MXFMetadataSet* set, const mxfKey* itemKey, RGBColor* value);

int mxf_avid_get_data_def(MXFHeaderMetadata* headerMetadata, mxfUUID* uuid, mxfUL* dataDef);

int mxf_avid_write_index_entry_array_header(MXFFile* mxfFile, uint8_t sliceCount, uint8_t posTableCount, 
    uint32_t numIndexEntries);

int mxf_avid_attach_mob_attribute(MXFHeaderMetadata* headerMetadata, MXFMetadataSet* packageSet, 
    mxfUTF16Char* name, mxfUTF16Char* value);
int mxf_avid_attach_int32_mob_attribute(MXFHeaderMetadata *headerMetadata, MXFMetadataSet *packageSet,
                                        const mxfUTF16Char *name, int32_t value);
int mxf_avid_attach_user_comment(MXFHeaderMetadata* headerMetadata, MXFMetadataSet* packageSet, 
    mxfUTF16Char* name, mxfUTF16Char* value);
    
int mxf_avid_read_string_mob_attributes(MXFMetadataSet* packageSet, MXFList** names, MXFList** values);
int mxf_avid_read_string_user_comments(MXFMetadataSet* packageSet, MXFList** names, MXFList** values);

int mxf_avid_read_string_tagged_values(MXFMetadataSet* set, const mxfKey* itemKey, MXFList** names, MXFList** values);
int mxf_avid_read_string_tagged_value(MXFMetadataSet* taggedValueSet, mxfUTF16Char** name, mxfUTF16Char** value);

int mxf_avid_get_mob_attribute(const mxfUTF16Char* name, const MXFList* names, const MXFList* values, const mxfUTF16Char** value);
int mxf_avid_get_user_comment(const mxfUTF16Char* name, const MXFList* names, const MXFList* values, const mxfUTF16Char** value);

int mxf_avid_set_product_version_item(MXFMetadataSet* set, const mxfKey* itemKey, const mxfProductVersion* value);


int mxf_avid_read_index_table_segment(MXFFile* mxfFile, uint64_t segmentLen, MXFIndexTableSegment** segment);
int mxf_avid_read_index_table_segment_2(MXFFile* mxfFile, uint64_t segmentLen,
    mxf_add_delta_entry* addDeltaEntry, void* addDeltaEntryData,
    mxf_add_index_entry* addIndexEntry, void* addIndexEntryData,
    MXFIndexTableSegment** segment);


int mxf_avid_is_mjpeg_essence_element(const mxfKey* key);
int mxf_avid_is_dnxhd_essence_element(const mxfKey* key);
int mxf_avid_is_mpeg_essence_element(const mxfKey* key);
int mxf_avid_is_unc10bit_essence_element(const mxfKey* key);
int mxf_avid_is_essence_element(const mxfKey* key);


void mxf_avid_set_auid(const mxfUL* ul, mxfAUID* auid);



#ifdef __cplusplus
}
#endif


#endif


