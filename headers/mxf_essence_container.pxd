

cdef extern from "mxf/mxf.h":
    
    ctypedef struct MXFEssenceElement:
        mxfKey key
        uint8_t llen
        uint64_t startFilePos
        uint64_t totalLen
        uint64_t currentFilePos

    cdef int mxf_is_gc_essence_element(const mxfKey* key)

    cdef int mxf_open_essence_element_write(MXFFile* mxfFile, mxfKey* key, uint8_t llen, uint64_t len, MXFEssenceElement** essenceElement)
    cdef int mxf_write_essence_element_data(MXFFile* mxfFile, MXFEssenceElement* essenceElement, uint8_t* data, uint32_t len)
    cdef int mxf_finalize_essence_element_write(MXFFile* mxfFile, MXFEssenceElement* essenceElement)

    cdef int mxf_open_essence_element_read(MXFFile* mxfFile, mxfKey* key, uint8_t llen, uint64_t len, MXFEssenceElement** essenceElement)
    cdef int mxf_read_essence_element_data(MXFFile* mxfFile, MXFEssenceElement* essenceElement, uint32_t len, uint8_t* data, uint32_t* numRead)

    cdef void mxf_close_essence_element(MXFEssenceElement** essenceElement)

    cdef uint64_t mxf_get_essence_element_size(MXFEssenceElement* essenceElement)

    cdef uint32_t mxf_get_track_number(mxfKey* essenceElementKey)
    cdef uint8_t mxf_get_essence_element_item_type(uint32_t trackNumber)
    cdef uint8_t mxf_get_essence_element_count(uint32_t trackNumber)
    cdef uint8_t mxf_get_essence_element_type(uint32_t trackNumber)
    cdef uint8_t mxf_get_essence_element_number(uint32_t trackNumber)