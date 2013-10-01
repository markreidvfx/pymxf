
cimport lib

from .datamodel cimport DataModel
from .metadata cimport HeaderMetadata

cdef class Partition(object):
    cdef lib.MXFPartition *ptr
    
cdef class PartitionList(object):
    cdef lib.MXFFilePartitions ptr
    
cdef class MXFFile(object):
    cdef lib.MXFFile *ptr
    cdef readonly list partitions
    
cdef class File(object):
    cdef lib.MXFFile *c_file
    cdef readonly HeaderMetadata header
    
    cdef Partition header_partition
    cdef Partition body_partition
    cdef Partition footer_partition
    
    cdef lib.MXFFilePartitions partitions
    
    cdef lib.int64_t header_metadata_pos
    
cdef class EssenceElement(object):
    cdef lib.MXFEssenceElement *ptr
    cdef File file
    
cdef class EssenceElement2(object):
    cdef lib.MXFEssenceElement *ptr
    cdef MXFFile file
    
cdef class IndexTableSegment(object):
    cdef lib.MXFIndexTableSegment *ptr