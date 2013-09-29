cimport lib


cdef object error_check(int result)
cdef bytes uft16_to_bytes(lib.mxfUTF16Char *wstr, lib.uint16_t size)

cdef object mxfUUID_to_UUID(lib.mxfUUID label)
cdef object mxfUL_to_UUID(lib.mxfUL label)
cdef object UUID_to_mxfUL(object value, lib.mxfUL *label)
cdef object UUID_to_mxfUUID(object value, lib.mxfUUID *label)

cdef class MXFUL(object):
    cdef lib.mxfUL label
    
cdef class MXFKEY(object):
    cdef lib.mxfKey label

cdef class MXFUMID(object):
    cdef lib.mxfUMID label