/*
 * $Id: mxf_index_table.h,v 1.3 2009/10/12 15:25:57 philipn Exp $
 *
 * MXF index table
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
 
#ifndef __MXF_INDEX_TABLE_H__
#define __MXF_INDEX_TABLE_H__


#ifdef __cplusplus
extern "C" 
{
#endif


typedef struct _MXFDeltaEntry
{
    struct _MXFDeltaEntry* next;
    
    int8_t posTableIndex;
    uint8_t slice;
    uint32_t elementData;
} MXFDeltaEntry;

typedef struct _MXFIndexEntry
{
    struct _MXFIndexEntry* next;
    
    int8_t temporalOffset;
    int8_t keyFrameOffset;
    uint8_t flags;
    uint64_t streamOffset;
    uint32_t* sliceOffset;
    mxfRational* posTable;
} MXFIndexEntry;

typedef struct
{
    mxfUUID instanceUID;
    mxfRational indexEditRate;
    mxfPosition indexStartPosition;
    mxfLength indexDuration;
    uint32_t editUnitByteCount;
    uint32_t indexSID;
    uint32_t bodySID;
    uint8_t sliceCount;
    uint8_t posTableCount;
    MXFDeltaEntry* deltaEntryArray;
    MXFIndexEntry* indexEntryArray;
} MXFIndexTableSegment;


typedef int (mxf_add_delta_entry)(void* data, uint32_t numEntries, MXFIndexTableSegment* segment, int8_t posTableIndex,
    uint8_t slice, uint32_t elementData);
typedef int (mxf_add_index_entry)(void* data, uint32_t numEntries, MXFIndexTableSegment* segment, int8_t temporalOffset,
    int8_t keyFrameOffset, uint8_t flags, uint64_t streamOffset, 
    uint32_t* sliceOffset /* assumes sliceCount set in segment */,
    mxfRational* posTable /* assumes posTableCount set in segment */);


int mxf_is_index_table_segment(const mxfKey* key);

int mxf_create_index_table_segment(MXFIndexTableSegment** segment);
void mxf_free_index_table_segment(MXFIndexTableSegment** segment);

int mxf_default_add_delta_entry(void* data/*ignored*/, uint32_t numEntries/*ignored*/, MXFIndexTableSegment* segment,
    int8_t posTableIndex, uint8_t slice, uint32_t elementData);

int mxf_default_add_index_entry(void* data/*ignored*/, uint32_t numEntries/*ignored*/, MXFIndexTableSegment* segment,
    int8_t temporalOffset, int8_t keyFrameOffset, uint8_t flags, uint64_t streamOffset, uint32_t* sliceOffset,
    mxfRational* posTable);

int mxf_write_index_table_segment(MXFFile* mxfFile, const MXFIndexTableSegment* segment);

int mxf_read_index_table_segment(MXFFile* mxfFile, uint64_t segmentLen, MXFIndexTableSegment** segment);
int mxf_read_index_table_segment_2(MXFFile* mxfFile, uint64_t segmentLen,
    mxf_add_delta_entry* addDeltaEntry, void* addDeltaEntryData,
    mxf_add_index_entry* addIndexEntry, void* addIndexEntryData,
    MXFIndexTableSegment** segment);

int mxf_write_index_table_segment_header(MXFFile* mxfFile, const MXFIndexTableSegment* segment, 
    uint32_t numDeltaEntries, uint32_t numIndexEntries);
int mxf_write_delta_entry_array_header(MXFFile* mxfFile, uint32_t numDeltaEntries);
int mxf_write_delta_entry(MXFFile* mxfFile, MXFDeltaEntry* entry);
int mxf_write_index_entry_array_header(MXFFile* mxfFile, uint8_t sliceCount, uint8_t posTableCount,
    uint32_t numIndexEntries);
int mxf_write_index_entry(MXFFile* mxfFile, uint8_t sliceCount, uint8_t posTableCount, MXFIndexEntry* entry);


#ifdef __cplusplus
}
#endif


#endif


