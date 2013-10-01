
cimport lib

from .datamodel cimport DataModel
from .metadata cimport HeaderMetadata

cdef class Partition(object):
    cdef lib.MXFPartition *ptr
    
cdef class File(object):
    cdef lib.MXFFile *ptr    
    cdef lib.MXFFilePartitions partitions

cdef class EssenceElement(object):
    cdef lib.MXFEssenceElement *ptr
    cdef File file
    
cdef class IndexTableSegment(object):
    cdef lib.MXFIndexTableSegment *ptr