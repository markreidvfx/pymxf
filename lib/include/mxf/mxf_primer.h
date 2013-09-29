/*
 * $Id: mxf_primer.h,v 1.2 2007/09/11 13:24:54 stuart_hc Exp $
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
 
#ifndef __MXF_PRIMER_H__
#define __MXF_PRIMER_H__



#ifdef __cplusplus
extern "C" 
{
#endif


typedef struct
{
    mxfLocalTag localTag;
    mxfUID uid;
} MXFPrimerPackEntry;

typedef struct
{
    mxfLocalTag nextTag;
    MXFList entries;
} MXFPrimerPack;



int mxf_is_primer_pack(const mxfKey* key);

int mxf_create_primer_pack(MXFPrimerPack** primerPack);
void mxf_free_primer_pack(MXFPrimerPack** primerPack);

int mxf_register_primer_entry(MXFPrimerPack* primerPack, const mxfUID* itemKey, mxfLocalTag newTag,
    mxfLocalTag* assignedTag);

int mxf_get_item_key(MXFPrimerPack* primerPack, mxfLocalTag localTag, mxfKey* key);
int mxf_get_item_tag(MXFPrimerPack* primerPack, const mxfKey* key, mxfLocalTag* localTag);

int mxf_create_item_tag(MXFPrimerPack* primerPack, mxfLocalTag* localTag);

int mxf_write_primer_pack(MXFFile* mxfFile, MXFPrimerPack* primerPack);
int mxf_read_primer_pack(MXFFile* mxfFile, MXFPrimerPack** primerPack);

void mxf_get_primer_pack_size(MXFFile* mxfFile, MXFPrimerPack* primerPack, uint64_t* size);


#ifdef __cplusplus
}
#endif


#endif


