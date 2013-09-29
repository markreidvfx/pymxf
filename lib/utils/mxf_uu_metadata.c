/*
 * $Id: mxf_uu_metadata.c,v 1.4 2008/11/11 10:34:32 philipn Exp $
 *
 * Utility functions for processing header metadata
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
#include <mxf/mxf_uu_metadata.h>



int mxf_uu_get_top_file_package(MXFHeaderMetadata* headerMetadata, MXFMetadataSet** filePackageSet)
{
    MXFMetadataSet* essContainerDataSet;
    MXFMetadataSet* set;
    mxfUMID topFilePackageUID;
    
    CHK_ORET(mxf_find_singular_set_by_key(headerMetadata, &MXF_SET_K(EssenceContainerData), &essContainerDataSet));
    CHK_ORET(mxf_get_umid_item(essContainerDataSet, &MXF_ITEM_K(EssenceContainerData, LinkedPackageUID), &topFilePackageUID));
    
    CHK_ORET(mxf_uu_get_referenced_package(headerMetadata, &topFilePackageUID, &set));
    CHK_ORET(mxf_is_subclass_of(set->headerMetadata->dataModel, &set->key, &MXF_SET_K(SourcePackage)));

    *filePackageSet = set;
    return 1;
}


int mxf_uu_get_package_tracks(MXFMetadataSet* packageSet, MXFArrayItemIterator* iter)
{
    CHK_ORET(mxf_initialise_array_item_iterator(packageSet, &MXF_ITEM_K(GenericPackage, Tracks), iter));
    return 1;
}

int mxf_uu_next_track(MXFHeaderMetadata* headerMetadata, MXFArrayItemIterator* iter, MXFMetadataSet** trackSet)
{
    uint8_t* arrayData;
    uint32_t arrayDataLen;
    
    if (!mxf_next_array_item_element(iter, &arrayData, &arrayDataLen))
    {
        return 0;
    }
    
    CHK_ORET(mxf_get_strongref(headerMetadata, arrayData, trackSet));
    
    return 1;
}

int mxf_uu_get_track_duration(MXFMetadataSet* trackSet, mxfLength* duration)
{
    MXFMetadataSet* sequenceSet;
    
    CHK_ORET(mxf_get_strongref_item(trackSet, &MXF_ITEM_K(GenericTrack, Sequence), &sequenceSet));
    CHK_ORET(mxf_get_length_item(sequenceSet, &MXF_ITEM_K(StructuralComponent, Duration), duration));
    
    return 1;
}

int mxf_uu_get_track_duration_at_rate(MXFMetadataSet* trackSet, mxfRational* editRate, mxfLength* duration)
{
    mxfLength localDuration;
    mxfRational localEditRate;
    
    CHK_ORET(mxf_uu_get_track_duration(trackSet, &localDuration));
    CHK_ORET(mxf_get_rational_item(trackSet, &MXF_ITEM_K(Track, EditRate), &localEditRate));
    
    if (memcmp(editRate, &localEditRate, sizeof(mxfRational)) == 0)
    {
        *duration = localDuration;
    }
    else
    {
        /* calc rounding up */
        *duration = (mxfLength)(localDuration * editRate->numerator * localEditRate.denominator / 
            (double)(editRate->denominator * localEditRate.numerator) + 0.5);
    }
    return 1;
}

int mxf_uu_get_track_datadef(MXFMetadataSet* trackSet, mxfUL* dataDef)
{
    MXFMetadataSet* sequenceSet;
    
    CHK_ORET(mxf_get_strongref_item(trackSet, &MXF_ITEM_K(GenericTrack, Sequence), &sequenceSet));
    CHK_ORET(mxf_get_ul_item(sequenceSet, &MXF_ITEM_K(StructuralComponent, DataDefinition), dataDef));
    
    return 1;
}

int mxf_uu_get_track_reference(MXFMetadataSet* trackSet, mxfUMID* sourcePackageUID, uint32_t* sourceTrackID)
{
    MXFMetadataSet* sequenceSet;
    MXFMetadataSet* sourceClipSet;
    uint32_t sequenceComponentCount;
    uint8_t* arrayElement;
    uint32_t i;
    
    CHK_ORET(mxf_get_strongref_item(trackSet, &MXF_ITEM_K(GenericTrack, Sequence), &sequenceSet));
    if (!mxf_is_subclass_of(sequenceSet->headerMetadata->dataModel, &sequenceSet->key, &MXF_SET_K(SourceClip)))
    {
        /* move to the first contained SourceClip, eg. skip preceding Filler components */
        CHK_ORET(mxf_get_array_item_count(sequenceSet, &MXF_ITEM_K(Sequence, StructuralComponents), &sequenceComponentCount));
        CHK_ORET(sequenceComponentCount >= 1);
        for (i = 0; i < sequenceComponentCount; i++)
        {
            CHK_ORET(mxf_get_array_item_element(sequenceSet, &MXF_ITEM_K(Sequence, StructuralComponents), i, &arrayElement));
            if (!mxf_get_strongref(sequenceSet->headerMetadata, arrayElement, &sourceClipSet))
            {
                /* probably a Filler if it hasn't been registered in the dictionary */
                continue;
            }
            if (mxf_is_subclass_of(sourceClipSet->headerMetadata->dataModel, &sourceClipSet->key, &MXF_SET_K(SourceClip)))
            {
                break;
            }
        }
        if (!mxf_is_subclass_of(sourceClipSet->headerMetadata->dataModel, &sourceClipSet->key, &MXF_SET_K(SourceClip)))
        {
            return 0;
        }
    }
    else
    {
        /* Track references the SourceClip directly */
        sourceClipSet = sequenceSet;
    }
    CHK_ORET(mxf_get_umid_item(sourceClipSet, &MXF_ITEM_K(SourceClip, SourcePackageID), sourcePackageUID));
    CHK_ORET(mxf_get_uint32_item(sourceClipSet, &MXF_ITEM_K(SourceClip, SourceTrackID), sourceTrackID));
    
    return 1;
}

int mxf_uu_get_referenced_package(MXFHeaderMetadata* headerMetadata, mxfUMID* sourcePackageUID, 
    MXFMetadataSet** packageSet)
{
    MXFMetadataSet* contentStorageSet;
    MXFMetadataSet* set;
    MXFArrayItemIterator iter;
    uint8_t* arrayElementValue;
    uint32_t arrayElementLength;
    mxfUMID packageUID;
    int foundIt = 0;

    CHK_ORET(mxf_find_singular_set_by_key(headerMetadata, &MXF_SET_K(ContentStorage), &contentStorageSet));
    
    CHK_ORET(mxf_initialise_array_item_iterator(contentStorageSet, &MXF_ITEM_K(ContentStorage, Packages), &iter));
    while (mxf_next_array_item_element(&iter, &arrayElementValue, &arrayElementLength))
    {
        if (mxf_get_strongref(headerMetadata, arrayElementValue, &set))
        {
            CHK_ORET(mxf_get_umid_item(set, &MXF_ITEM_K(GenericPackage, PackageUID), &packageUID));
            if (mxf_equals_umid(&packageUID, sourcePackageUID))
            {
                *packageSet = set;
                foundIt = 1;
                break;
            }
        }
    }
    
    return foundIt;
}

int mxf_uu_get_referenced_track(MXFHeaderMetadata* headerMetadata, mxfUMID* sourcePackageUID, 
    uint32_t sourceTrackID, MXFMetadataSet** sourceTrackSet)
{
    MXFMetadataSet* packageSet;
    MXFMetadataSet* trackSet;
    MXFArrayItemIterator iter;
    uint32_t trackID;
    int foundIt = 0;
    
    CHK_ORET(mxf_uu_get_referenced_package(headerMetadata, sourcePackageUID, &packageSet));
    CHK_ORET(mxf_is_subclass_of(packageSet->headerMetadata->dataModel, &packageSet->key, &MXF_SET_K(GenericPackage)));
    
    CHK_ORET(mxf_uu_get_package_tracks(packageSet, &iter));
    while (mxf_uu_next_track(headerMetadata, &iter, &trackSet))
    {
        CHK_ORET(mxf_get_uint32_item(trackSet, &MXF_ITEM_K(GenericTrack, TrackID), &trackID));
        if (trackID == sourceTrackID)
        {
            *sourceTrackSet = trackSet;
            foundIt = 1;
            break;
        }
    }

    return foundIt;
}

int mxf_uu_get_track_descriptor(MXFMetadataSet* sourcePackageSet, uint32_t trackID, 
    MXFMetadataSet** linkedDescriptorSet)
{
    MXFMetadataSet* descriptorSet;
    MXFMetadataSet* childDescriptorSet;
    MXFArrayItemIterator iter;
    uint8_t* arrayElementValue;
    uint32_t arrayElementLength;
    uint32_t linkedTrackID;
    
    CHK_ORET(mxf_get_strongref_item(sourcePackageSet, &MXF_ITEM_K(SourcePackage, Descriptor), &descriptorSet));
    if (mxf_is_subclass_of(descriptorSet->headerMetadata->dataModel, &descriptorSet->key, &MXF_SET_K(MultipleDescriptor)))
    {
        CHK_ORET(mxf_initialise_array_item_iterator(descriptorSet, &MXF_ITEM_K(MultipleDescriptor, SubDescriptorUIDs), &iter));
        while (mxf_next_array_item_element(&iter, &arrayElementValue, &arrayElementLength))
        {
            if (mxf_get_strongref(sourcePackageSet->headerMetadata, arrayElementValue, &childDescriptorSet))
            {
                if (mxf_have_item(childDescriptorSet, &MXF_ITEM_K(FileDescriptor, LinkedTrackID)))
                {
                    CHK_ORET(mxf_get_uint32_item(childDescriptorSet, &MXF_ITEM_K(FileDescriptor, LinkedTrackID), &linkedTrackID));
                    if (trackID == linkedTrackID)
                    {
                        *linkedDescriptorSet = childDescriptorSet;
                        return 1;
                    }
                }
            }
        }
    }
    else
    {
        if (mxf_have_item(descriptorSet, &MXF_ITEM_K(FileDescriptor, LinkedTrackID)))
        {
            CHK_ORET(mxf_get_uint32_item(descriptorSet, &MXF_ITEM_K(FileDescriptor, LinkedTrackID), &linkedTrackID));
            if (trackID == linkedTrackID)
            {
                *linkedDescriptorSet = descriptorSet;
                return 1;
            }
        }
        else
        {
            *linkedDescriptorSet = descriptorSet;
            return 1;
        }
    }

    return 0;
}


int mxf_uu_get_utf16string_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfUTF16Char** value)
{
    mxfUTF16Char* newValue = NULL;
    uint16_t size;
    
    CHK_ORET(mxf_get_utf16string_item_size(set, itemKey, &size));
    CHK_MALLOC_ARRAY_ORET(newValue, mxfUTF16Char, size);
    CHK_OFAIL(mxf_get_utf16string_item(set, itemKey, newValue));
    
    *value = newValue;
    return 1;
    
fail:
    SAFE_FREE(&newValue);
    return 0;
}

