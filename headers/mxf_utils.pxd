


cdef extern from "mxf/mxf.h":

    cdef void mxf_generate_uuid(mxfUUID* uuid)
    cdef void mxf_get_timestamp_now(mxfTimestamp* now)
    cdef void mxf_generate_umid(mxfUMID* umid)
    cdef void mxf_generate_key(mxfKey* key)

    cdef mxf_print_key(mxfKey* key)
    cdef mxf_sprint_key(char* str, mxfKey* key)

    cdef void mxf_print_label(mxfUL* label)
    cdef void mxf_sprint_label(char* str, mxfUL* label)
    
    cdef size_t mxf_utf16_to_utf8(char *u8_str, mxfUTF16Char *u16_str, size_t u8_size)
    cdef size_t mxf_utf8_to_utf16(mxfUTF16Char *u16_str, char *u8_str, size_t u16_size)