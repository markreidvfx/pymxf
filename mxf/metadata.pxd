cimport lib
from .datamodel cimport DataModel, ItemDef

cdef class MetaDataItem(object):
    cdef lib.MXFMetadataItem *ptr
    cdef DataModel model
    cdef MetaDataSet data_set
    cdef ItemDef itemdef
    
cdef class MetaDataSet:
    cdef lib.MXFMetadataSet *ptr
    cdef DataModel model
    cdef HeaderMetadata header

cdef class HeaderMetadata(object):
    cdef lib.MXFHeaderMetadata *ptr
    cdef DataModel model