


cdef extern from "mxf/mxf.h":

    cdef int SEEK_SET
    cdef int SEEK_END
    
    ctypedef struct MXFFile:
        pass
        
    
    cdef int mxf_disk_file_open_new(
        char* filename,
        MXFFile** mxfFile
        )
    cdef int mxf_disk_file_open_read(
        char* filename,
        MXFFile** mxfFile
        )
    cdef int mxf_disk_file_open_modify(
        char* filename,
        MXFFile** mxfFile
        )

        
    cdef void mxf_file_close(MXFFile** mxfFile)
    cdef uint32_t mxf_file_read(MXFFile* mxfFile, uint8_t* data, uint32_t count)
    cdef uint32_t mxf_file_write(MXFFile* mxfFile, const uint8_t* data, uint32_t count)
    cdef int mxf_file_getc(MXFFile* mxfFile)
    cdef int mxf_file_putc(MXFFile* mxfFile, int c)
    cdef int mxf_file_eof(MXFFile* mxfFile)
    cdef int mxf_file_seek(MXFFile* mxfFile, int64_t offset, int whence)
    cdef int64_t mxf_file_tell(MXFFile* mxfFile)
    cdef int mxf_file_is_seekable(MXFFile* mxfFile)
    cdef int64_t mxf_file_size(MXFFile* mxfFile)
 
        
    cdef void mxf_file_set_min_llen(MXFFile* mxfFile, uint8_t llen)
    cdef uint8_t mxf_get_min_llen(MXFFile* mxfFile)
    
    cdef int mxf_write_uint8(MXFFile* mxfFile, uint8_t value)
    cdef int mxf_write_uint16(MXFFile* mxfFile, uint16_t value)
    cdef int mxf_write_uint32(MXFFile* mxfFile, uint32_t value)
    cdef int mxf_write_uint64(MXFFile* mxfFile, uint64_t value)
    cdef int mxf_write_int8(MXFFile* mxfFile, int8_t value)
    cdef int mxf_write_int16(MXFFile* mxfFile, int16_t value)
    cdef int mxf_write_int32(MXFFile* mxfFile, int32_t value)
    cdef int mxf_write_int64(MXFFile* mxfFile, int64_t value)
    cdef int mxf_read_uint8(MXFFile* mxfFile, uint8_t* value)
    cdef int mxf_read_uint16(MXFFile* mxfFile, uint16_t* value)
    cdef int mxf_read_uint32(MXFFile* mxfFile, uint32_t* value)
    cdef int mxf_read_uint64(MXFFile* mxfFile, uint64_t* value)
    cdef int mxf_read_int8(MXFFile* mxfFile, int8_t* value)
    cdef int mxf_read_int16(MXFFile* mxfFile, int16_t* value)
    cdef int mxf_read_int32(MXFFile* mxfFile, int32_t* value)
    cdef int mxf_read_int64(MXFFile* mxfFile, int64_t* value)
    
    cdef int mxf_read_k(MXFFile* mxfFile, mxfKey* key)
    cdef int mxf_read_l(MXFFile* mxfFile, uint8_t* llen, uint64_t* len)
    cdef int mxf_read_kl(MXFFile* mxfFile, mxfKey* key, uint8_t* llen, uint64_t* len)
    cdef int mxf_read_ul(MXFFile* mxfFile, mxfUL* value)
    cdef int mxf_read_key(MXFFile* mxfFile, mxfKey* value)
    cdef int mxf_read_uid(MXFFile* mxfFile, mxfUID* value)
    cdef int mxf_read_uuid(MXFFile* mxfFile, mxfUUID* value)
    cdef int mxf_read_local_tag(MXFFile* mxfFile, mxfLocalTag* tag)
    cdef int mxf_read_local_tl(MXFFile* mxfFile, mxfLocalTag* tag, uint16_t* len)
    
    cdef int mxf_skip(MXFFile* mxfFile, uint64_t len)
    
    
    cdef int mxf_write_local_tag(MXFFile* mxfFile, mxfLocalTag tag)
    cdef int mxf_write_local_tl(MXFFile* mxfFile, mxfLocalTag tag, uint16_t len)
    cdef int mxf_write_k(MXFFile* mxfFile, const mxfKey* key)
    cdef uint8_t mxf_write_l(MXFFile* mxfFile, uint64_t len)
    cdef int mxf_write_kl(MXFFile* mxfFile, const mxfKey* key, uint64_t len)
    cdef int mxf_write_fixed_l(MXFFile* mxfFile, uint8_t llen, uint64_t len)
    cdef int mxf_write_fixed_kl(MXFFile* mxfFile, const mxfKey* key, uint8_t llen, uint64_t len)
    cdef int mxf_write_ul(MXFFile* mxfFile, const mxfUL* label)
    cdef int mxf_write_uid(MXFFile* mxfFile, const mxfUID* uid)
    cdef int mxf_write_uuid(MXFFile* mxfFile, const mxfUUID* uuid)

    cdef uint8_t mxf_get_llen(MXFFile* mxfFile, uint64_t len)

    cdef int mxf_read_batch_header(MXFFile* mxfFile, uint32_t* len, uint32_t* eleLen)
    cdef int mxf_write_batch_header(MXFFile* mxfFile, uint32_t len, uint32_t eleLen)
    cdef int mxf_read_array_header(MXFFile* mxfFile, uint32_t* len, uint32_t* eleLen)
    cdef int mxf_write_array_header(MXFFile* mxfFile, uint32_t len, uint32_t eleLen)

    cdef int mxf_write_zeros(MXFFile* mxfFile, uint64_t len)

    cdef int mxf_equals_key(const mxfKey* keyA, const mxfKey* keyB)
    cdef int mxf_equals_key_prefix(const mxfKey* keyA, const mxfKey* keyB, size_t cmpLen)
    cdef int mxf_equals_key_mod_regver(const mxfKey* keyA, const mxfKey* keyB)
    cdef int mxf_equals_ul(const mxfUL* labelA, const mxfUL* labelB)
    cdef int mxf_equals_ul_mod_regver(const mxfUL* labelA, const mxfUL* labelB)
    cdef int mxf_equals_uuid(const mxfUUID* uuidA, const mxfUUID* uuidB)
    cdef int mxf_equals_uid(const mxfUID* uidA, const mxfUID* uidB)
    cdef int mxf_equals_umid(const mxfUMID* umidA, const mxfUMID* umidB)

    cdef int mxf_is_ul(const mxfUID* uid)

    cdef void mxf_set_runin_len(MXFFile* mxfFile, uint16_t runinLen)
    cdef uint16_t mxf_get_runin_len(MXFFile* mxfFile)