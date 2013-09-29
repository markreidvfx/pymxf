from libc.stdint cimport int64_t, uint64_t
#from libc.stddef cimport wchar_t

ctypedef Py_UNICODE wchar_t

cdef extern from "mxf/mxf.h":
    ctypedef unsigned char uint8_t "uint8_t"
    ctypedef unsigned short int uint16_t "uint16_t"
    ctypedef unsigned long int uint32_t "uint32_t" 
    ctypedef int64_t uint64_t "uint64_t"
    
    ctypedef signed char int8_t "int8_t"
    ctypedef signed short int int16_t "int16_t"
    ctypedef signed long int int32_t "int32_t"
    
    ctypedef struct mxfUL:
        uint8_t octet0
        uint8_t octet1
        uint8_t octet2
        uint8_t octet3
        uint8_t octet4
        uint8_t octet5
        uint8_t octet6
        uint8_t octet7
        uint8_t octet8
        uint8_t octet9
        uint8_t octet10
        uint8_t octet11
        uint8_t octet12
        uint8_t octet13
        uint8_t octet14
        uint8_t octet15
        
    ctypedef mxfUL mxfKey
    
    ctypedef struct mxfUUID:
        uint8_t octet0
        uint8_t octet1
        uint8_t octet2
        uint8_t octet3
        uint8_t octet4
        uint8_t octet5
        uint8_t octet6
        uint8_t octet7
        uint8_t octet8
        uint8_t octet9
        uint8_t octet10
        uint8_t octet11
        uint8_t octet12
        uint8_t octet13
        uint8_t octet14
        uint8_t octet15
        
    ctypedef mxfUL mxfUID

    ctypedef mxfUID mxfAUID

    ctypedef uint16_t mxfLocalTag

    ctypedef uint16_t mxfVersionType
    
    ctypedef struct mxfTimestamp:
        int16_t year
        uint8_t month
        uint8_t day
        uint8_t hour
        uint8_t min
        uint8_t sec
        uint8_t qmsec
        
    ctypedef wchar_t mxfUTF16Char
    
    ctypedef struct mxfUMID:
        uint8_t octet0
        uint8_t octet1
        uint8_t octet2
        uint8_t octet3
        uint8_t octet4
        uint8_t octet5
        uint8_t octet6
        uint8_t octet7
        uint8_t octet8
        uint8_t octet9
        uint8_t octet10
        uint8_t octet11
        uint8_t octet12
        uint8_t octet13
        uint8_t octet14
        uint8_t octet15
        uint8_t octet16
        uint8_t octet17
        uint8_t octet18
        uint8_t octet19
        uint8_t octet20
        uint8_t octet21
        uint8_t octet22
        uint8_t octet23
        uint8_t octet24
        uint8_t octet25
        uint8_t octet26
        uint8_t octet27
        uint8_t octet28
        uint8_t octet29
        uint8_t octet30
        uint8_t octet31
        
    ctypedef struct mxfRational:
        int32_t numerator
        int32_t denominator
        
    ctypedef int64_t mxfPosition

    ctypedef int64_t mxfLength

    ctypedef uint8_t mxfBoolean
    
    ctypedef struct mxfProductVersion:
        uint16_t major
        uint16_t minor
        uint16_t patch
        uint16_t build
        uint16_t release
        
    ctypedef struct mxfRGBALayoutComponent:
        uint8_t code
        uint8_t depth
        
   # Defs
   
    cdef int mxfLocalTag_extlen
    cdef int mxfVersionType_extlen
    cdef int mxfUUID_extlen
    cdef int mxfKey_extlen
    cdef int mxfUID_extlen 
    cdef int mxfUL_extlen
    cdef int mxfAUID_extlen 
    cdef int mxfTimestamp_extlen
    cdef int mxfUTF16Char_extlen
    cdef int mxfUMID_extlen
    cdef int mxfRational_extlen
    cdef int mxfPosition_extlen
    cdef int mxfLength_extlen
    cdef int mxfBoolean_extlen
    cdef int mxfProductVersion_extlen
    cdef int mxfRGBALayoutComponent_extlen
    
    cdef mxfUUID g_Null_UUID
    cdef mxfKey g_Null_Key
    cdef mxfUL g_Null_UL
    cdef mxfLocalTag g_Null_LocalTag
    cdef mxfRational g_Null_Rational
    
        