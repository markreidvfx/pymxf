

cdef extern from "mxf/mxf_avid.h":
    pass
cdef extern from "mxf/mxf_uu_metadata.h":
    pass

cdef extern from "mxf/mxf.h":

    cdef enum MXFItemTypeCategory:
        MXF_BASIC_TYPE_CAT
        MXF_ARRAY_TYPE_CAT
        MXF_COMPOUND_TYPE_CAT
        MXF_INTERPRET_TYPE_CAT
        
    ctypedef struct MXFBasicTypeInfo:
        unsigned int size
    
    ctypedef struct MXFArrayTypeInfo:
        unsigned int elementTypeId
        unsigned int fixedSize
    
    ctypedef struct MXFCompoundTypeMemberInfo:
        char* name
        unsigned int typeId
        
    ctypedef struct MXFCompoundTypeInfo:
        MXFCompoundTypeMemberInfo members[16]
        
    ctypedef struct MXFInterpretTypeInfo:
        unsigned int typeId
        unsigned int fixedArraySize
        
    cdef union info_union:
        MXFBasicTypeInfo basic
        MXFArrayTypeInfo array
        MXFCompoundTypeInfo compound
        MXFInterpretTypeInfo interpret
    
    ctypedef struct MXFItemType:
        MXFItemTypeCategory category
        unsigned int typeId
        char* name
        info_union info
    
    ctypedef struct MXFItemDef:
        char* name
        mxfKey setDefKey
        mxfKey key
        mxfLocalTag localTag
        unsigned int typeId
        int isRequired
    
    ctypedef struct MXFSetDef:
        char* name
        mxfKey parentSetDefKey
        mxfKey key
        MXFList itemDefs
        MXFSetDef *parentSetDef
    
    ctypedef struct MXFDataModel:
        MXFList itemDefs
        MXFList setDefs
        MXFItemType types[128]
        unsigned int lastTypeId
        
    cdef enum MXFItemTypeId:
        MXF_UNKNOWN_TYPE
    
        # basic 
        MXF_INT8_TYPE
        MXF_INT16_TYPE
        MXF_INT32_TYPE
        MXF_INT64_TYPE
        MXF_UINT8_TYPE
        MXF_UINT16_TYPE
        MXF_UINT32_TYPE
        MXF_UINT64_TYPE
        MXF_RAW_TYPE
        
        # array
        MXF_UTF16STRING_TYPE
        MXF_UTF16STRINGARRAY_TYPE
        MXF_INT32ARRAY_TYPE
        MXF_UINT32ARRAY_TYPE
        MXF_INT64ARRAY_TYPE
        MXF_UINT8ARRAY_TYPE
        MXF_ISO7STRING_TYPE
        MXF_INT32BATCH_TYPE
        MXF_UINT32BATCH_TYPE
        MXF_AUIDARRAY_TYPE
        MXF_ULARRAY_TYPE
        MXF_ULBATCH_TYPE
        MXF_STRONGREFARRAY_TYPE
        MXF_STRONGREFBATCH_TYPE
        MXF_WEAKREFARRAY_TYPE
        MXF_WEAKREFBATCH_TYPE
        MXF_RATIONALARRAY_TYPE
        MXF_RGBALAYOUT_TYPE
        
        # compound 
        MXF_RATIONAL_TYPE
        MXF_TIMESTAMP_TYPE
        MXF_PRODUCTVERSION_TYPE
        MXF_INDIRECT_TYPE
        MXF_RGBALAYOUTCOMPONENT_TYPE
        
        # interpret 
        MXF_VERSIONTYPE_TYPE
        MXF_UTF16_TYPE
        MXF_BOOLEAN_TYPE
        MXF_ISO7_TYPE
        MXF_LENGTH_TYPE
        MXF_POSITION_TYPE
        MXF_RGBACODE_TYPE
        MXF_STREAM_TYPE
        MXF_DATAVALUE_TYPE
        MXF_IDENTIFIER_TYPE
        MXF_OPAQUE_TYPE
        MXF_UMID_TYPE
        MXF_UID_TYPE
        MXF_UL_TYPE
        MXF_UUID_TYPE
        MXF_AUID_TYPE
        MXF_PACKAGEID_TYPE
        MXF_STRONGREF_TYPE
        MXF_WEAKREF_TYPE
        MXF_ORIENTATION_TYPE
        MXF_CODED_CONTENT_TYPE_TYPE
        
        MXF_EXTENSION_TYPE # extension types must have integer value >= this 
        
        
    cdef int mxf_load_data_model(MXFDataModel** dataModel)
    cdef void mxf_free_data_model(MXFDataModel** dataModel)
    
    
    cdef int mxf_finalise_data_model(MXFDataModel* dataModel)
    
    cdef int mxf_find_set_def(MXFDataModel* dataModel, mxfKey* key, MXFSetDef** setDef)
    cdef int mxf_find_item_def(MXFDataModel* dataModel, mxfKey* key, MXFItemDef** itemDef)
    cdef int mxf_find_item_def_in_set_def(mxfKey* key, MXFSetDef* setDef, MXFItemDef** itemDef)

    cdef MXFItemType* mxf_get_item_def_type(MXFDataModel* dataModel, unsigned int typeId)
    
    
    cdef mxfKey Preface_SET_K "MXF_SET_K(Preface)"
    
    
    cdef mxfKey Preface_ProjectName_SET_ITEM "MXF_ITEM_K(Preface, ProjectName)"
    
    cdef mxfKey MXF_PP_K_ClosedComplete_Header "MXF_PP_K(ClosedComplete, Header)"
    cdef mxfKey MXF_PP_K_ClosedComplete_Body "MXF_PP_K(ClosedComplete, Body)"
    cdef mxfKey MXF_PP_K_ClosedComplete_Footer "MXF_PP_K(ClosedComplete, Footer)"
    cdef mxfKey MXF_OP_L_atom_NTracks_1SourceClip "MXF_OP_L(atom, NTracks_1SourceClip)"
    cdef mxfKey MXF_EC_L_DVBased_50_625_50_ClipWrapped "MXF_EC_L(DVBased_50_625_50_ClipWrapped)"