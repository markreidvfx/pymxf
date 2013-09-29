


cdef extern from "mxf/mxf.h":

    cdef int SEEK_SET
    
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
    cdef uint16_t mxf_get_runin_len(MXFFile* mxfFile)