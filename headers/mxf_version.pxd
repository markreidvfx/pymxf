

cdef extern from "mxf/mxf.h":
    cdef mxfProductVersion* mxf_get_version()
    cdef char* mxf_get_platform_string()
    cdef  mxfUTF16Char* mxf_get_platform_wstring()