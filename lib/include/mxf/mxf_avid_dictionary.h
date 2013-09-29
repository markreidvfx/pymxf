/*
 * $Id: mxf_avid_dictionary.h,v 1.1 2008/11/07 14:12:59 philipn Exp $
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
 
#ifndef __MXF_AVID_DICTIONARY_H__
#define __MXF_AVID_DICTIONARY_H__


#ifdef __cplusplus
extern "C" 
{
#endif



int mxf_avid_is_dictionary(MXFDataModel* dataModel, const mxfKey* setKey);
int mxf_avid_is_def_object(MXFDataModel* dataModel, const mxfKey* setKey);


int mxf_avid_create_dictionary(MXFHeaderMetadata* headerMetadata, MXFMetadataSet** dictSet);


int mxf_avid_set_defobject_items(MXFMetadataSet* dictSet, const mxfUL* id, const mxfUTF16Char* name, const mxfUTF16Char* description);

int mxf_avid_create_datadef(MXFMetadataSet* dictSet, const mxfUL* id, const mxfUTF16Char* name, const mxfUTF16Char* description, MXFMetadataSet** defSet);
int mxf_avid_create_containerdef(MXFMetadataSet* dictSet, const mxfUL* id, const mxfUTF16Char* name, const mxfUTF16Char* description, MXFMetadataSet** defSet);
int mxf_avid_create_taggedvaluedef(MXFMetadataSet* dictSet, const mxfUL* id, const mxfUTF16Char* name, const mxfUTF16Char* description, MXFMetadataSet** defSet);

/* additional defs are codec, operation, parameter, plugin, interpolation and klv def */


int mxf_initialise_dict_read_filter(MXFReadFilter* filter, int skipDataDefs);
void mxf_clear_dict_read_filter(MXFReadFilter* filter);


int mxf_avid_create_default_dictionary(MXFHeaderMetadata* headerMetadata, MXFMetadataSet** dictSet);


#ifdef __cplusplus
}
#endif


#endif


