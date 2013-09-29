


cdef extern from "mxf/mxf.h":

    cdef void mxf_generate_aafsdk_umid(mxfUMID* umid)
     
    cdef int mxf_avid_load_extensions(MXFDataModel* dataModel)
    
    cdef int mxf_avid_read_filtered_header_metadata(
        MXFFile* mxfFile, 
        int skipDataDefs, 
        MXFHeaderMetadata* headerMetadata, 
        uint64_t headerByteCount, 
        const mxfKey* key, 
        uint8_t llen, 
        uint64_t len
        )
        
    cdef int mxf_avid_write_header_metadata(
        MXFFile* mxfFile, 
        MXFHeaderMetadata* headerMetadata, 
        MXFPartition* headerPartition)
        
    cdef int mxf_avid_read_string_tagged_values(MXFMetadataSet* set, const mxfKey* itemKey, MXFList** names, MXFList** values)
    cdef int mxf_avid_read_string_tagged_value(MXFMetadataSet* taggedValueSet, mxfUTF16Char** name, mxfUTF16Char** value)