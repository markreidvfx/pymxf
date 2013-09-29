#ifndef __MXF_UU_METADATA_H__
#define __MXF_UU_METADATA_H__


#ifdef __cplusplus
extern "C" 
{
#endif



/*
* Get the top level file source package assuming that there is only a single EssenceContainer.
*/
int mxf_uu_get_top_file_package(MXFHeaderMetadata* headerMetadata, MXFMetadataSet** filePackageSet);


/*
* Get package tracks
*/
int mxf_uu_get_package_tracks(MXFMetadataSet* packageSet, MXFArrayItemIterator* iter);


/*
* Get next track in item array
* return 1 on success, 0 when track not found (dark set or invalid reference) or end of array
*/
int mxf_uu_next_track(MXFHeaderMetadata* headerMetadata, MXFArrayItemIterator* iter, MXFMetadataSet** trackSet);


/*
* Get track info held in the sequence
*/
int mxf_uu_get_track_duration(MXFMetadataSet* trackSet, mxfLength* duration);
int mxf_uu_get_track_duration_at_rate(MXFMetadataSet* trackSet, mxfRational* editRate, mxfLength* duration);
int mxf_uu_get_track_datadef(MXFMetadataSet* trackSet, mxfUL* dataDef);


/*
* Get the source package reference for a single SourceClip contained in track
*/
int mxf_uu_get_track_reference(MXFMetadataSet* trackSet, mxfUMID* sourcePackageUID, uint32_t* sourceTrackID);


/*
* Get the package referenced
*/
int mxf_uu_get_referenced_package(MXFHeaderMetadata* headerMetadata, mxfUMID* sourcePackageUID, 
    MXFMetadataSet** packageSet);

    
/*
* Get the track referenced
*/
int mxf_uu_get_referenced_track(MXFHeaderMetadata* headerMetadata, mxfUMID* sourcePackageUID, 
    uint32_t sourceTrackID, MXFMetadataSet** sourceTrackSet);

    
/*
* Get descriptor linked with the track; if source package descriptor is not a multiple descriptor
* then that descriptor is returned is linked track ID matches or is NULL
*/
int mxf_uu_get_track_descriptor(MXFMetadataSet* sourcePackageSet, uint32_t trackID, 
    MXFMetadataSet** linkedDescriptorSet);

/*
* Get the string item and allocate the required memory
*/
int mxf_uu_get_utf16string_item(MXFMetadataSet* set, const mxfKey* itemKey, mxfUTF16Char** value);


#ifdef __cplusplus
}
#endif


#endif



