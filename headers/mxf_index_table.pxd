

cdef extern from "mxf/mxf.h":
    ctypedef struct MXFDeltaEntry:
        pass
        
    ctypedef struct MXFIndexEntry:
        pass
        
    ctypedef struct MXFIndexTableSegment:
        mxfUUID instanceUID
        mxfRational indexEditRate
        mxfPosition indexStartPosition
        mxfLength indexDuration
        uint32_t editUnitByteCount
        uint32_t indexSID
        uint32_t bodySID
        uint8_t sliceCount
        uint8_t posTableCount
        MXFDeltaEntry* deltaEntryArray
        MXFIndexEntry* indexEntryArray


    cdef int mxf_create_index_table_segment(MXFIndexTableSegment** segment)
    cdef void mxf_free_index_table_segment(MXFIndexTableSegment** segment)
    
    
    
    cdef int mxf_write_index_table_segment(MXFFile* mxfFile, const MXFIndexTableSegment* segment)