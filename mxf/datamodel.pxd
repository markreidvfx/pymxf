cimport lib

cdef class ItemType(object):
    cdef lib.MXFItemType *ptr
    cdef DataModel model
    
cdef class ItemDef(object):
    cdef lib.MXFItemDef *ptr
    cdef DataModel model
    
cdef class SetDef(object):
    cdef lib.MXFSetDef *ptr
    cdef DataModel model

cdef class DataModel(object):
    cdef lib.MXFDataModel *ptr