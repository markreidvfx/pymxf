

cdef extern from "mxf/mxf.h":
    
    ctypedef struct MXFPartition:
        mxfUL key
        uint16_t majorVersion
        uint16_t minorVersion
        uint32_t kagSize
        uint64_t thisPartition
        uint64_t previousPartition
        uint64_t footerPartition
        uint64_t headerByteCount
        uint64_t indexByteCount
        uint32_t indexSID
        uint64_t bodyOffset
        uint32_t bodySID
        mxfUL operationalPattern
        MXFList essenceContainers
        
        int64_t headerMarkInPos
        int64_t indexMarkInPos
        
    ctypedef MXFList MXFFilePartitions
    
    ctypedef struct MXFRIPEntry:
        uint32_t bodySID
        uint64_t thisPartition

    ctypedef struct MXFRIP:
        MXFList entries
        
        
    cdef int mxf_is_header_partition_pack(const mxfKey* key)
    cdef int mxf_is_body_partition_pack(const mxfKey* key)
    cdef int mxf_is_footer_partition_pack(const mxfKey* key)
    cdef int mxf_is_partition_pack(const mxfKey* key)
    cdef int mxf_partition_is_closed(const mxfKey* key)
    cdef int mxf_partition_is_complete(const mxfKey* key)
    cdef int mxf_partition_is_closed_and_complete(const mxfKey* key)

    cdef int mxf_create_file_partitions(MXFFilePartitions** partitions)
    cdef void mxf_free_file_partitions(MXFFilePartitions** partitions)
    cdef void mxf_initialise_file_partitions(MXFFilePartitions* partitions)
    cdef void mxf_clear_file_partitions(MXFFilePartitions* partitions)
    cdef void mxf_clear_rip(MXFRIP* rip)
            
    cdef int mxf_create_partition(MXFPartition** partition)
    cdef int mxf_create_from_partition(const MXFPartition* sourcePartition, MXFPartition** partition)
    cdef void mxf_free_partition(MXFPartition** partition)
    
    cdef void mxf_initialise_partition(MXFPartition* partition)
    cdef int mxf_initialise_with_partition(const MXFPartition* sourcePartition, MXFPartition* partition)
    cdef void mxf_clear_partition(MXFPartition* partition)

    cdef int mxf_append_new_partition(MXFFilePartitions* partitions, MXFPartition** partition)
    cdef int mxf_append_new_from_partition(MXFFilePartitions* partitions, MXFPartition* sourcePartition, MXFPartition** partition)
    cdef int mxf_append_partition(MXFFilePartitions* partitions, MXFPartition* partition)

    cdef int mxf_mark_header_start(MXFFile* mxfFile, MXFPartition* partition)
    cdef int mxf_mark_header_end(MXFFile* mxfFile, MXFPartition* partition)
    cdef int mxf_mark_index_start(MXFFile* mxfFile, MXFPartition* partition)
    cdef int mxf_mark_index_end(MXFFile* mxfFile, MXFPartition* partition)

    cdef int mxf_append_partition_esscont_label(MXFPartition* partition, const mxfUL* label)

    cdef int mxf_write_partition(MXFFile* mxfFile, MXFPartition* partition)
    cdef int mxf_update_partitions(MXFFile* mxfFile, MXFFilePartitions* partitions)
    cdef int mxf_read_partition(MXFFile* mxfFile, const mxfKey* key,uint64_t length, MXFPartition** partition)

    cdef int mxf_is_filler(const mxfKey* key)
    cdef int mxf_fill_to_kag(MXFFile* mxfFile, MXFPartition* partition)
    cdef int mxf_fill_to_position(MXFFile* mxfFile, uint64_t position)
    cdef int mxf_allocate_space_to_kag(MXFFile* mxfFile, MXFPartition* partition, uint32_t size)
    cdef int mxf_allocate_space(MXFFile* mxfFile, uint32_t size)
    cdef int mxf_write_fill(MXFFile* mxfFile, uint32_t size)
    cdef int mxf_read_next_nonfiller_kl(MXFFile* mxfFile, mxfKey *key, uint8_t* llen, uint64_t *len)

    cdef int mxf_read_rip(MXFFile* mxfFile, MXFRIP* rip)
    cdef int mxf_write_rip(MXFFile* mxfFile, MXFFilePartitions* partitions)

    cdef int mxf_read_header_pp_kl_with_runin(MXFFile* mxfFile, mxfKey* key, uint8_t* llen, uint64_t* len)
    cdef int mxf_read_header_pp_kl(MXFFile* mxfFile, mxfKey* key, uint8_t* llen, uint64_t* len)