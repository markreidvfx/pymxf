/*
 * $Id: mxf_partition.c,v 1.8 2010/07/26 16:02:37 philipn Exp $
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
 
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <mxf/mxf.h>


static const mxfKey g_PartitionPackPrefix_key = MXF_PP_KEY(0x01, 0x00, 0x00);



static void free_partition_in_list(void* data)
{
    MXFPartition* tmpPartition;
    
    if (data == NULL)
    {
        return;
    }

    tmpPartition = (MXFPartition*)data;     
    mxf_free_partition(&tmpPartition);
}


int mxf_is_header_partition_pack(const mxfKey* key)
{
    return mxf_equals_key_prefix(key, &g_PartitionPackPrefix_key, 13) && key->octet13 == 0x02;
}

int mxf_is_body_partition_pack(const mxfKey* key)
{
    return mxf_equals_key_prefix(key, &g_PartitionPackPrefix_key, 13) && key->octet13 == 0x03;
}

int mxf_is_footer_partition_pack(const mxfKey* key)
{
    return mxf_equals_key_prefix(key, &g_PartitionPackPrefix_key, 13) && key->octet13 == 0x04;
}

int mxf_is_partition_pack(const mxfKey* key)
{
    return mxf_equals_key_prefix(key, &g_PartitionPackPrefix_key, 13) && 
        key->octet13 >= 0x02 && key->octet13 <= 0x04; 
}

int mxf_is_filler(const mxfKey* key)
{
    return mxf_equals_key_mod_regver(key, &g_KLVFill_key);
}

int mxf_partition_is_closed(const mxfKey* key)
{
    if (!mxf_is_partition_pack(key))
    {
        return 0;
    }
    return key->octet14 == 0x02 || key->octet14 == 0x04;
}

int mxf_partition_is_complete(const mxfKey* key)
{
    if (!mxf_is_partition_pack(key))
    {
        return 0;
    }
    return key->octet14 == 0x03 || key->octet14 == 0x04;
}

int mxf_partition_is_closed_and_complete(const mxfKey* key)
{
    if (!mxf_is_partition_pack(key))
    {
        return 0;
    }
    return key->octet14 == 0x04;
}


int mxf_create_file_partitions(MXFFilePartitions** partitions)
{
    return mxf_create_list(partitions, free_partition_in_list);
}

void mxf_free_file_partitions(MXFFilePartitions** partitions)
{
    mxf_free_list(partitions);
}

void mxf_initialise_file_partitions(MXFFilePartitions* partitions)
{
    mxf_initialise_list(partitions, free_partition_in_list);
}

void mxf_clear_file_partitions(MXFFilePartitions* partitions)
{
    mxf_clear_list(partitions);
}

void mxf_clear_rip(MXFRIP* rip)
{
    mxf_clear_list(&rip->entries);
}


int mxf_create_partition(MXFPartition** partition)
{
    MXFPartition* newPartition;
    
    CHK_MALLOC_ORET(newPartition, MXFPartition);
    mxf_initialise_partition(newPartition);
    
    *partition = newPartition;
    return 1;   
}

int mxf_create_from_partition(const MXFPartition* sourcePartition, MXFPartition** partition)
{
    MXFPartition* newPartition;
    
    CHK_MALLOC_ORET(newPartition, MXFPartition);
    if (!mxf_initialise_with_partition(sourcePartition, newPartition))
    {
        goto fail;
    }
    
    *partition = newPartition;
    return 1;
    
fail:
    mxf_free_partition(&newPartition);
    return 0;
}

void mxf_free_partition(MXFPartition** partition)
{
    if (*partition == NULL)
    {
        return;
    }
    
    mxf_clear_partition(*partition);
    SAFE_FREE(partition);
}

void mxf_initialise_partition(MXFPartition* partition)
{
    memset(partition, 0, sizeof(MXFPartition));
    mxf_initialise_list(&partition->essenceContainers, free);
    partition->kagSize = 1;
    partition->majorVersion = 0x0001;
    partition->minorVersion = 0x0002;
    partition->headerMarkInPos = -1;
    partition->indexMarkInPos = -1;
}

int mxf_initialise_with_partition(const MXFPartition* sourcePartition, MXFPartition* partition)
{
    MXFListIterator iter;
    
    mxf_initialise_partition(partition);
    
    partition->key = g_Null_Key;
    
    partition->majorVersion = sourcePartition->majorVersion;
    partition->minorVersion = sourcePartition->minorVersion;
    partition->kagSize = sourcePartition->kagSize;
    partition->operationalPattern = sourcePartition->operationalPattern;

    mxf_initialise_list_iter(&iter, &sourcePartition->essenceContainers);
    while (mxf_next_list_iter_element(&iter))
    {
        CHK_ORET(mxf_append_partition_esscont_label(partition, (mxfUL*)mxf_get_iter_element(&iter)));
    }
    
    return 1;
}

void mxf_clear_partition(MXFPartition* partition)
{
    if (partition == NULL)
    {
        return;
    }
    
    mxf_clear_list(&partition->essenceContainers);
}


int mxf_append_new_partition(MXFFilePartitions* partitions, MXFPartition** partition)
{
    MXFPartition* newPartition;
    
    CHK_ORET(mxf_create_partition(&newPartition));
    CHK_OFAIL(mxf_append_partition(partitions, newPartition));
    
    *partition = newPartition;
    return 1;

fail:
    mxf_free_partition(&newPartition);
    return 0;
}

int mxf_append_new_from_partition(MXFFilePartitions* partitions, MXFPartition* sourcePartition, 
    MXFPartition** partition)
{
    MXFPartition* newPartition;
    
    CHK_ORET(mxf_create_from_partition(sourcePartition, &newPartition));
    CHK_OFAIL(mxf_append_partition(partitions, newPartition));
    
    *partition = newPartition;
    return 1;

fail:
    mxf_free_partition(&newPartition);
    return 0;
}

int mxf_append_partition(MXFFilePartitions* partitions, MXFPartition* partition)
{
    CHK_ORET(mxf_append_list_element(partitions, partition));
    
    return 1;
}


int mxf_mark_header_start(MXFFile* mxfFile, MXFPartition* partition)
{
    int64_t filePos;
    CHK_ORET((filePos = mxf_file_tell(mxfFile)) >= 0);
    
    partition->headerMarkInPos = filePos;
    return 1;
}

int mxf_mark_header_end(MXFFile* mxfFile, MXFPartition* partition)
{
    int64_t filePos;
    
    CHK_ORET(partition->headerMarkInPos >= 0);
    CHK_ORET((filePos = mxf_file_tell(mxfFile)) >= 0);
    CHK_ORET(filePos >= partition->headerMarkInPos);
    
    partition->headerByteCount = filePos - partition->headerMarkInPos;
    partition->headerMarkInPos = -1;
    return 1;
}

int mxf_mark_index_start(MXFFile* mxfFile, MXFPartition* partition)
{
    int64_t filePos;
    CHK_ORET((filePos = mxf_file_tell(mxfFile)) >= 0);
    
    partition->indexMarkInPos = filePos;
    return 1;
}

int mxf_mark_index_end(MXFFile* mxfFile, MXFPartition* partition)
{
    int64_t filePos;
    
    CHK_ORET(partition->indexMarkInPos >= 0);
    CHK_ORET((filePos = mxf_file_tell(mxfFile)) >= 0);
    CHK_ORET(filePos >= partition->indexMarkInPos);
    
    partition->indexByteCount = filePos - partition->indexMarkInPos;
    partition->indexMarkInPos = -1;
    return 1;
}


int mxf_append_partition_esscont_label(MXFPartition* partition, const mxfUL* label)
{
    mxfUL* newLabel;
    
    CHK_MALLOC_ORET(newLabel, mxfUL);
    *newLabel = *label;
    CHK_OFAIL(mxf_append_list_element(&partition->essenceContainers, newLabel));
        
    return 1;
    
fail:
    SAFE_FREE(&newLabel);
    return 0;
}


int mxf_write_partition(MXFFile* mxfFile, MXFPartition* partition)
{
    uint32_t essenceContainerLen = (uint32_t)mxf_get_list_length(&partition->essenceContainers);
    uint64_t packLen = 88 + mxfUL_extlen * essenceContainerLen;
    int64_t filePos;
    MXFListIterator iter;
    
    CHK_ORET((filePos = mxf_file_tell(mxfFile)) >= 0);
    partition->thisPartition = filePos - mxf_get_runin_len(mxfFile);
    
    CHK_ORET(mxf_write_kl(mxfFile, &partition->key, packLen));
    
    CHK_ORET(mxf_write_uint16(mxfFile, partition->majorVersion));
    CHK_ORET(mxf_write_uint16(mxfFile, partition->minorVersion));
    CHK_ORET(mxf_write_uint32(mxfFile, partition->kagSize));
    CHK_ORET(mxf_write_uint64(mxfFile, partition->thisPartition));
    CHK_ORET(mxf_write_uint64(mxfFile, partition->previousPartition));
    CHK_ORET(mxf_write_uint64(mxfFile, partition->footerPartition));
    CHK_ORET(mxf_write_uint64(mxfFile, partition->headerByteCount));
    CHK_ORET(mxf_write_uint64(mxfFile, partition->indexByteCount));
    CHK_ORET(mxf_write_uint32(mxfFile, partition->indexSID));
    CHK_ORET(mxf_write_uint64(mxfFile, partition->bodyOffset));
    CHK_ORET(mxf_write_uint32(mxfFile, partition->bodySID));
    CHK_ORET(mxf_write_ul(mxfFile, &partition->operationalPattern));
    CHK_ORET(mxf_write_batch_header(mxfFile, essenceContainerLen, mxfUL_extlen));

    mxf_initialise_list_iter(&iter, &partition->essenceContainers);
    while (mxf_next_list_iter_element(&iter))
    {
        CHK_ORET(mxf_write_ul(mxfFile, (mxfUL*)mxf_get_iter_element(&iter)));
    }

    return 1;
}

/* Note: positions file pointer at end of file */
int mxf_update_partitions(MXFFile* mxfFile, MXFFilePartitions* partitions)
{
    MXFPartition* previousPartition;
    MXFPartition* lastPartition;
    MXFListIterator iter;
    int haveFooter;
    
    /* check if anything there to update */
    if (mxf_get_list_length(partitions) == 0)
    {
        return 1;
    }
    
    /* update partition packs with previousPartition and footerPartition (if present) offsets */
    lastPartition = (MXFPartition*)mxf_get_last_list_element(partitions);
    haveFooter = mxf_is_footer_partition_pack(&lastPartition->key);
    previousPartition = NULL;
    mxf_initialise_list_iter(&iter, partitions);
    while (mxf_next_list_iter_element(&iter))
    {
        MXFPartition* partition = (MXFPartition*)mxf_get_iter_element(&iter);
        
        if (previousPartition != NULL)
        {
            partition->previousPartition = previousPartition->thisPartition;
        }
        if (haveFooter)
        {
            partition->footerPartition = lastPartition->thisPartition;
        }
        else
        {
            partition->footerPartition = 0;
        }
        
        previousPartition = partition;
    }

    /* re-write the partition packs */
    mxf_initialise_list_iter(&iter, partitions);
    while (mxf_next_list_iter_element(&iter))
    {
        MXFPartition* partition = (MXFPartition*)mxf_get_iter_element(&iter);
        
        CHK_ORET(mxf_file_seek(mxfFile, (int64_t)partition->thisPartition + 
            mxf_get_runin_len(mxfFile), SEEK_SET));
        CHK_ORET(mxf_write_partition(mxfFile, partition));
    }

    CHK_ORET(mxf_file_seek(mxfFile, 0, SEEK_END));
    
    return 1;
}

int mxf_read_partition(MXFFile* mxfFile, const mxfKey* key, MXFPartition** partition)
{
    MXFPartition* newPartition;
    uint32_t len;
    uint32_t eleLen;
    mxfUL label;
    uint32_t i;
    
    CHK_ORET(mxf_create_partition(&newPartition));
    newPartition->key = *key;
    
    CHK_OFAIL(mxf_read_uint16(mxfFile, &newPartition->majorVersion));
    CHK_OFAIL(mxf_read_uint16(mxfFile, &newPartition->minorVersion));
    CHK_OFAIL(mxf_read_uint32(mxfFile, &newPartition->kagSize));
    CHK_OFAIL(mxf_read_uint64(mxfFile, &newPartition->thisPartition));
    CHK_OFAIL(mxf_read_uint64(mxfFile, &newPartition->previousPartition));
    CHK_OFAIL(mxf_read_uint64(mxfFile, &newPartition->footerPartition));
    CHK_OFAIL(mxf_read_uint64(mxfFile, &newPartition->headerByteCount));
    CHK_OFAIL(mxf_read_uint64(mxfFile, &newPartition->indexByteCount));
    CHK_OFAIL(mxf_read_uint32(mxfFile, &newPartition->indexSID));
    CHK_OFAIL(mxf_read_uint64(mxfFile, &newPartition->bodyOffset));
    CHK_OFAIL(mxf_read_uint32(mxfFile, &newPartition->bodySID));
    CHK_OFAIL(mxf_read_ul(mxfFile, &newPartition->operationalPattern));

    CHK_OFAIL(mxf_read_batch_header(mxfFile, &len, &eleLen));
    for (i = 0; i < len; i++)
    {
        CHK_OFAIL(mxf_read_ul(mxfFile, &label));
        CHK_OFAIL(mxf_append_partition_esscont_label(newPartition, &label));
    }

    *partition = newPartition;
    return 1;
    
fail:
    mxf_free_partition(&newPartition);
    return 0;
}


int mxf_fill_to_kag(MXFFile* mxfFile, MXFPartition* partition)
{
    return mxf_allocate_space_to_kag(mxfFile, partition, 0);
}

int mxf_fill_to_position(MXFFile* mxfFile, uint64_t position)
{
    int64_t filePos;
    uint64_t fillSize;
    uint8_t llen;
    
    CHK_ORET((filePos = mxf_file_tell(mxfFile)) >= 0);

    if ((uint64_t)filePos == position)
    {
        return 1;
    }
        
    CHK_ORET(((uint64_t)filePos <= position - mxf_get_min_llen(mxfFile) + mxfKey_extlen));
    
    CHK_ORET(mxf_write_k(mxfFile, &g_KLVFill_key));

    fillSize = position - filePos - mxfKey_extlen;
    llen = mxf_get_llen(mxfFile, fillSize);
    assert(fillSize >= llen);
    fillSize -= llen;

    CHK_ORET(mxf_write_l(mxfFile, fillSize));
    CHK_ORET(mxf_write_zeros(mxfFile, fillSize));
    
    return 1;
}

int mxf_allocate_space_to_kag(MXFFile* mxfFile, MXFPartition* partition, uint32_t size)
{
    int64_t filePos;
    uint64_t relativeFilePos;
    int64_t fillSize;
    uint8_t llen;
        
    assert(partition->kagSize > 0);
    
    CHK_ORET((filePos = mxf_file_tell(mxfFile)) >= 0);
    CHK_ORET((uint64_t)filePos > partition->thisPartition);
    relativeFilePos = filePos + size - partition->thisPartition;
    
    if (size != 0 || (relativeFilePos % partition->kagSize) != 0)
    {
        CHK_ORET(mxf_write_k(mxfFile, &g_KLVFill_key));
        
        fillSize = (int64_t)size - mxfKey_extlen;
        if (partition->kagSize > 1)
        {
            fillSize += partition->kagSize - relativeFilePos % partition->kagSize;
        }
        
        if (fillSize >= 0)
        {
            llen = mxf_get_llen(mxfFile, fillSize);
        }
        else
        {
            llen = 0;
        }
        while (fillSize - llen < 0)
        {
            fillSize += partition->kagSize;
            if (fillSize >= 0)
            {
                llen = mxf_get_llen(mxfFile, fillSize);
            }
            else
            {
                llen = 0;
            }
        }
        fillSize -= llen;
        
        CHK_ORET(mxf_write_l(mxfFile, fillSize));
        CHK_ORET(mxf_write_zeros(mxfFile, fillSize));
    }
    
    return 1;
}

int mxf_allocate_space(MXFFile* mxfFile, uint32_t size)
{
    uint64_t fillSize;
    uint8_t llen;
    
    CHK_ORET(size >= (uint32_t)(mxf_get_min_llen(mxfFile) + mxfKey_extlen));
    
    CHK_ORET(mxf_write_k(mxfFile, &g_KLVFill_key));

    fillSize = size - mxfKey_extlen;
    llen = mxf_get_llen(mxfFile, fillSize);
    assert(fillSize >= llen);
    fillSize -= llen;

    CHK_ORET(mxf_write_l(mxfFile, fillSize));
    CHK_ORET(mxf_write_zeros(mxfFile, fillSize));
    
    return 1;
}

int mxf_write_fill(MXFFile* mxfFile, uint32_t size)
{
    return mxf_allocate_space(mxfFile, size); 
}

int mxf_read_next_nonfiller_kl(MXFFile* mxfFile, mxfKey* key, uint8_t* llen, uint64_t *len)
{
    mxfKey tkey;
    uint8_t tllen;
    uint64_t tlen;
    CHK_ORET(mxf_read_kl(mxfFile, &tkey, &tllen, &tlen));
    while (mxf_is_filler(&tkey))
    {
        CHK_ORET(mxf_skip(mxfFile, (int64_t)tlen));
        CHK_ORET(mxf_read_kl(mxfFile, &tkey, &tllen, &tlen));
    }
    
    *key = tkey;
    *llen = tllen;
    *len = tlen;
    return 1;    
}


int mxf_read_rip(MXFFile* mxfFile, MXFRIP* rip)
{
    uint32_t size;
    mxfKey key;
    uint8_t llen;
    uint64_t len;
    uint32_t numEntries;
    MXFRIPEntry* newEntry = NULL;
    MXFRIPEntry* entry;
    uint32_t i;

    mxf_initialise_list(&rip->entries, free);
    
    /* read RIP size at end of file */    
    if (!mxf_file_seek(mxfFile, -4, SEEK_END))
    {
        return 0;
    }
    CHK_ORET(mxf_read_uint32(mxfFile, &size));
        
    /* check size > min and can seek */
    if (size < 33 || /* min RIP size = 16 + 1 + (4 + 8) * 1 + 4 */ 
        !mxf_file_seek(mxfFile, (int64_t)0 - size, SEEK_CUR))
    {
        return 0;
    }

    /* check found RIP key */
    CHK_ORET(mxf_read_kl(mxfFile, &key, &llen, &len));
    if (!mxf_equals_key(&key, &g_RandomIndexPack_key))
    {
        return 0;
    }
     
    /* read RIP */
    
    numEntries = ((uint32_t)len - 4) / 12;
    CHK_ORET(((uint32_t)len - 4) % 12 == 0);
    
    for (i = 0; i < numEntries; i++)
    {
        CHK_MALLOC_OFAIL(newEntry, MXFRIPEntry);
        CHK_OFAIL(mxf_append_list_element(&rip->entries, newEntry));
        entry = newEntry;
        newEntry = NULL; /* entry assigned to list so set to NULL so not free'ed in fail */
        CHK_OFAIL(mxf_read_uint32(mxfFile, &entry->bodySID));
        CHK_OFAIL(mxf_read_uint64(mxfFile, &entry->thisPartition));
    }
    
    return 1;

fail:
    if (newEntry != NULL)
    {
        free(newEntry);
    }
    mxf_clear_list(&rip->entries);
    return 0;
}

int mxf_write_rip(MXFFile* mxfFile, MXFFilePartitions* partitions)
{
    uint32_t numPartitions;
    uint64_t len;
    uint8_t llen;
    MXFListIterator iter;

    numPartitions = (uint32_t)mxf_get_list_length(partitions);
    len = (4 + 8) * numPartitions + 4;
    
    CHK_ORET(mxf_write_k(mxfFile, &g_RandomIndexPack_key));
    CHK_ORET((llen = mxf_write_l(mxfFile, len)) != 0);
    
    mxf_initialise_list_iter(&iter, partitions);
    while (mxf_next_list_iter_element(&iter))
    {
        MXFPartition* partition = (MXFPartition*)mxf_get_iter_element(&iter);
        
        CHK_ORET(mxf_write_uint32(mxfFile, partition->bodySID));
        CHK_ORET(mxf_write_uint64(mxfFile, partition->thisPartition));
    }
    CHK_ORET(mxf_write_uint32(mxfFile, (uint32_t)(16 + llen + len)));
    
    return 1;
}


int mxf_read_header_pp_kl(MXFFile* mxfFile, mxfKey* key, uint8_t* llen, uint64_t* len)
{
    mxfKey tkey;
    uint8_t tllen;
    uint64_t tlen;
    
    CHK_ORET(mxf_read_kl(mxfFile, &tkey, &tllen, &tlen));
    
    if (!mxf_is_header_partition_pack(&tkey))
    {
        return 0;
    }
    
    *key = tkey;
    *llen = tllen;
    *len = tlen;
    return 1;    
}

int mxf_read_header_pp_kl_with_runin(MXFFile* mxfFile, mxfKey* key, uint8_t* llen, uint64_t* len)
{
    uint8_t keyCompareByte = 0;
    uint32_t runinCheckCount = 0;
    mxfKey k;
    uint8_t* keyPtr = (uint8_t*)&k;
    int byte;
    
    /* try find first 11 bytes of partition pack */
    while (runinCheckCount < MAX_RUNIN_LEN + 11)
    {
        if ((byte = mxf_file_getc(mxfFile)) == EOF)
        {
            return 0;
        }

        runinCheckCount++;
        if (byte == ((const uint8_t*)(&g_PartitionPackPrefix_key))[keyCompareByte])
        {
            keyPtr[keyCompareByte++] = (uint8_t)byte;
            if (keyCompareByte == 11)
            {
                break;
            }
        }
        else
        {
            if (runinCheckCount >= MAX_RUNIN_LEN)
            {
                return 0;
            }
            keyCompareByte = 0;
        }
    }
    if (runinCheckCount >= MAX_RUNIN_LEN + 11)
    {
        return 0;
    }

    /* read the remaing bytes of the key */
    for (; keyCompareByte < 16; keyCompareByte++)
    {
        if ((byte = mxf_file_getc(mxfFile)) == EOF)
        {
            return 0;
        }
        keyPtr[keyCompareByte] = (uint8_t)byte;
    }
    
    if (!mxf_is_header_partition_pack(&k))
    {
        return 0;
    }

    if (!mxf_read_l(mxfFile, llen, len))
    {
        return 0;
    }

    mxf_set_runin_len(mxfFile, (uint16_t)(runinCheckCount - 11));
    
    *key = k;
    return 1;
}

