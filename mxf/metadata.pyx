cimport lib
from .util cimport error_check, mxfUL_to_UUID, mxfUUID_to_UUID
from .datamodel cimport DataModel, SetDef, ItemDef, ItemType

from uuid import UUID

cdef class MetaDataItem(object):

    def __init__(self, MetaDataItem obj=None):
        
        if obj:
            self.ptr = obj.ptr
            self.model = obj.model
            self.data_set = obj.data_set
            self.itemdef = obj.itemdef
            
    def __repr__(self):
        return '<%s.%s of %s %s at 0x%x>' % (
            self.__class__.__module__,
            self.__class__.__name__,
            self.name, self.type_name,
            id(self),
        )
    cdef update_ptr(self):
        if self.ptr == NULL:
            error_check(lib.mxf_get_item(self.data_set.ptr, &self.itemdef.ptr.key, &self.ptr))

    def itemtype(self):
        return self.model.find_itemtype(self.itemdef)
        
    property name:
        def __get__(self):
            return self.itemdef.name
    property type_name:
        def __get__(self):
            return self.itemtype().name
        
    property key:
        def __get__(self):
            if self.ptr:
                return mxfUL_to_UUID(self.ptr.key)
    property length:
        def __get__(self):
            if self.ptr:
                return self.ptr.length
        
    property value:
        def __get__(self):
            if self.ptr:
                return self.itemdef.get_value(self.data_set)
        def __set__(self, value):
            self.itemdef.set_value(self.data_set, value)
            self.update_ptr()
    
    property raw_value:
        def __get__(self):
            if self.ptr:
                return self.ptr.value[:self.ptr.length]
            
cdef class MetaDataArrayItem(MetaDataItem):

    def append(self, value):
        self.itemdef.append(self.data_set, value)
        self.update_ptr()
        
cdef class MetaDataSet(object):
            
    def __repr__(self):
        return '<%s.%s of %s at 0x%x>' % (
            self.__class__.__module__,
            self.__class__.__name__,
            self.type_name,
            id(self),
        )
            
    def keys(self):
        setdef = self.model.find_setdef(self)
        return [item.name for item in setdef.iter_itemdefs()]
    
    def has_key(self, bytes key):
        return key in self.keys()
    
    def __getitem__(self, bytes key):
        if not self.has_key(key):
            raise IndexError("Invalid index: %s" % key)
        for item in self.iter_items():
            if item.name == key:
                return item
            
        cdef MetaDataItem item_data = MetaDataItem()
        setdef = self.model.find_setdef(self)
        
        for itemdef in setdef.iter_itemdefs():
            if itemdef.name == key:
                item_data.itemdef = itemdef
                item_data.data_set = self
                item_data.model = self.model
                return resovle_metadata_item(item_data)
            
        raise IndexError("Unable to find index: %s" % key)
    
    def __setitem__(self, bytes key, value):
        if not self.has_key(key):
            raise IndexError("Invalid index: %s" % key)
        
        cdef SetDef setdef = self.model.find_setdef(self)

        for item in setdef.iter_itemdefs():
            if item.name == key:
                item.set_value(self, value)
        
        
    def setdef(self):
        return self.model.find_setdef(self)
            
    def iter_items(self):
        cdef lib.MXFListIterator item_iter
        lib.mxf_initialise_list_iter(&item_iter, &self.ptr.items)
        cdef MetaDataItem item
        while lib.mxf_next_list_iter_element(&item_iter):
            item = MetaDataItem()
            
            item.ptr = <lib.MXFMetadataItem *> lib.mxf_get_iter_element(&item_iter)
            item.model = self.model
            item.data_set = self
            item.itemdef = self.model.find_itemdef(item)
            yield resovle_metadata_item(item)
            
    def __richcmp__(x, y, int op):
        x_instanceID = None
        y_instanceID = None
        if isinstance(x, MetaDataSet):
            x_instanceID = x.instanceID
        else:
            x_instanceID =x
        if isinstance(y, MetaDataSet):
            y_instanceID = y.instanceID
        else:
            y_instanceID = y
        if op == 2:
            return x_instanceID == y_instanceID
        elif op == 3:
            return x_instanceID != y_instanceID
            

    property type_name:
        def __get__(self):
            return self.setdef().name
    property instanceID:
        def __get__(self):
            return mxfUUID_to_UUID(self.ptr.instanceUID)
        
cdef object resovle_metadata_item(MetaDataItem item):
    cdef ItemType item_type = item.itemtype()
    
    if item_type.ptr.typeId in (lib.MXF_UTF16STRINGARRAY_TYPE,
                                lib.MXF_INT32ARRAY_TYPE,
                                lib.MXF_UINT32ARRAY_TYPE,
                                lib.MXF_INT64ARRAY_TYPE,
                                lib.MXF_UINT8ARRAY_TYPE,
                                lib.MXF_ISO7STRING_TYPE,
                                lib.MXF_INT32BATCH_TYPE,
                                lib.MXF_UINT32BATCH_TYPE,
                                lib.MXF_AUIDARRAY_TYPE,
                                lib.MXF_ULARRAY_TYPE,
                                lib.MXF_ULBATCH_TYPE,
                                lib.MXF_STRONGREFARRAY_TYPE,
                                lib.MXF_STRONGREFBATCH_TYPE,
                                lib.MXF_WEAKREFARRAY_TYPE,
                                lib.MXF_WEAKREFBATCH_TYPE,
                                lib.MXF_RATIONALARRAY_TYPE,
                                lib.MXF_RGBALAYOUT_TYPE):
        return MetaDataArrayItem(item)
    
    return item


cdef class HeaderMetadata(object):
    
    def __init__(self, DataModel model):
        self.ptr = NULL
        error_check(lib.mxf_create_header_metadata(&self.ptr, model.ptr))
        self.model = model
        
    def __dealloc__(self): 
        if self.ptr:
            lib.mxf_free_header_metadata(&self.ptr)
            
        
    def create_set(self, name):
        cdef SetDef setdef
        cdef MetaDataSet set_item = MetaDataSet()
        setdef = self.model.find_setdef_by_name(name)
        
        error_check(lib.mxf_create_set(self.ptr, &setdef.ptr.key, &set_item.ptr))
        set_item.model = self.model
        set_item.header= self
        return set_item
    
    def create_avid_metadictionary(self):
        cdef MetaDataSet set_item = MetaDataSet()
        error_check(lib.mxf_avid_create_default_metadictionary(self.ptr, &set_item.ptr)) 
        set_item.model = self.model
        set_item.header= self
        return set_item
        
    def iter_sets(self):
        
        cdef MetaDataSet meta_set
        
        cdef lib.MXFListIterator setsIter
        
        lib.initialise_sets_iter(self.ptr, &setsIter)
        
        while lib.mxf_next_list_iter_element(&setsIter):
            meta_set = MetaDataSet()
            meta_set.ptr = <lib.MXFMetadataSet*> lib.mxf_get_iter_element(&setsIter)
            
            meta_set.model = self.model
            meta_set.header= self
            yield meta_set
