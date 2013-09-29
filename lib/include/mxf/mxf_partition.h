/*
 * $Id: mxf_partition.h,v 1.3 2010/06/02 10:59:20 philipn Exp $
 *
 * MXF file partitions
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
 
#ifndef __MXF_PARTITION_H__
#define __MXF_PARTITION_H__


#ifdef __cplusplus
extern "C" 
{
#endif


typedef struct
{
    mxfUL key;
    uint16_t majorVersion;
    uint16_t minorVersion;
    uint32_t kagSize;
    uint64_t thisPartition;
    uint64_t previousPartition;
    uint64_t footerPartition;
    uint64_t headerByteCount;
    uint64_t indexByteCount;
    uint32_t indexSID;
    uint64_t bodyOffset;
    uint32_t bodySID;
    mxfUL operationalPattern;
    MXFList essenceContainers;

    /* used when marking header and index starts to calc header/indexByteCount */
    int64_t headerMarkInPos;
    int64_t indexMarkInPos;
} MXFPartition;

typedef MXFList MXFFilePartitions;

typedef struct
{
    uint32_t bodySID;
    uint64_t thisPartition;
} MXFRIPEntry;

typedef struct 
{
    MXFList entries;
} MXFRIP;


int mxf_is_header_partition_pack(const mxfKey* key);
int mxf_is_body_partition_pack(const mxfKey* key);
int mxf_is_footer_partition_pack(const mxfKey* key);
int mxf_is_partition_pack(const mxfKey* key);
int mxf_partition_is_closed(const mxfKey* key);
int mxf_partition_is_complete(const mxfKey* key);
int mxf_partition_is_closed_and_complete(const mxfKey* key);

int mxf_create_file_partitions(MXFFilePartitions** partitions);
void mxf_free_file_partitions(MXFFilePartitions** partitions);
void mxf_initialise_file_partitions(MXFFilePartitions* partitions);
void mxf_clear_file_partitions(MXFFilePartitions* partitions);
void mxf_clear_rip(MXFRIP* rip);

int mxf_create_partition(MXFPartition** partition);
int mxf_create_from_partition(const MXFPartition* sourcePartition, MXFPartition** partition);
void mxf_free_partition(MXFPartition** partition);

void mxf_initialise_partition(MXFPartition* partition);
int mxf_initialise_with_partition(const MXFPartition* sourcePartition, MXFPartition* partition);
void mxf_clear_partition(MXFPartition* partition);

int mxf_append_new_partition(MXFFilePartitions* partitions, MXFPartition** partition);
int mxf_append_new_from_partition(MXFFilePartitions* partitions, MXFPartition* sourcePartition, MXFPartition** partition);
int mxf_append_partition(MXFFilePartitions* partitions, MXFPartition* partition);

int mxf_mark_header_start(MXFFile* mxfFile, MXFPartition* partition);
int mxf_mark_header_end(MXFFile* mxfFile, MXFPartition* partition);
int mxf_mark_index_start(MXFFile* mxfFile, MXFPartition* partition);
int mxf_mark_index_end(MXFFile* mxfFile, MXFPartition* partition);

int mxf_append_partition_esscont_label(MXFPartition* partition, const mxfUL* label);

int mxf_write_partition(MXFFile* mxfFile, MXFPartition* partition);
int mxf_update_partitions(MXFFile* mxfFile, MXFFilePartitions* partitions);
int mxf_read_partition(MXFFile* mxfFile, const mxfKey* key, MXFPartition** partition);

int mxf_is_filler(const mxfKey* key);
int mxf_fill_to_kag(MXFFile* mxfFile, MXFPartition* partition);
int mxf_fill_to_position(MXFFile* mxfFile, uint64_t position);
int mxf_allocate_space_to_kag(MXFFile* mxfFile, MXFPartition* partition, uint32_t size);
int mxf_allocate_space(MXFFile* mxfFile, uint32_t size);
int mxf_write_fill(MXFFile* mxfFile, uint32_t size);
int mxf_read_next_nonfiller_kl(MXFFile* mxfFile, mxfKey *key, uint8_t* llen, uint64_t *len);

int mxf_read_rip(MXFFile* mxfFile, MXFRIP* rip);
int mxf_write_rip(MXFFile* mxfFile, MXFFilePartitions* partitions);

int mxf_read_header_pp_kl_with_runin(MXFFile* mxfFile, mxfKey* key, uint8_t* llen, uint64_t* len);
int mxf_read_header_pp_kl(MXFFile* mxfFile, mxfKey* key, uint8_t* llen, uint64_t* len);


#ifdef __cplusplus
}
#endif


#endif


