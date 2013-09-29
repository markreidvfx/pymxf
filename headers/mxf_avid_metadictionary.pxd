

cdef extern from "mxf/mxf.h":

    ctypedef struct AvidMetaDictionary:
        pass
        
    cdef int mxf_avid_is_metadictionary(MXFDataModel* dataModel, const mxfKey* setKey)
    cdef int mxf_avid_is_metadef(MXFDataModel* dataModel, const mxfKey* setKey)

    cdef int mxf_avid_create_metadictionary(MXFHeaderMetadata* headerMetadata, AvidMetaDictionary** metaDict)
    cdef int mxf_avid_add_default_metadictionary(AvidMetaDictionary* metaDict)
    cdef int mxf_avid_finalise_metadictionary(AvidMetaDictionary** metaDict, MXFMetadataSet** metaDictSet)
    
    cdef int mxf_avid_create_default_metadictionary(MXFHeaderMetadata* headerMetadata, MXFMetadataSet** metaDictSet)