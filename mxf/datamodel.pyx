from libc.stdlib cimport malloc, free
from libc.stdio cimport sscanf
from libc.string cimport memset
cimport lib
from .util cimport error_check, uft16_to_bytes, mxfUUID_to_UUID, mxfUL_to_UUID, UUID_to_mxfUL, UUID_to_mxfUUID, MXFUL, MXFKEY,MXFUMID
from .metadata cimport MetaDataSet, MetaDataItem  

from datetime import datetime
from struct import unpack,pack

cdef class ItemType(object):

    def __init__(self, ItemType itemtype=None):
        
        if itemtype:
            self.ptr = itemtype.ptr
            
    def append(self, MetaDataSet data_set, ItemDef itemdef, value):
        raise NotImplementedError("append not implemented for type: %s" % self.name)
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        raise NotImplementedError("get_value not implemented for type: %s" % self.name)
    def set_value(self, MetaDataSet data_set, ItemDef itemdef, value):
        raise NotImplementedError("set_value not implemented for type: %s" % self.name)

    property name:
        def __get__(self):
            return self.ptr.name
        
# basic

cdef class IntegerType(ItemType):
    
    def set_value(self, MetaDataSet data_set, ItemDef itemdef, value):
        cdef lib.int8_t int8
        cdef lib.int16_t int16
        cdef lib.int32_t int32
        cdef lib.int64_t int64
        
        cdef lib.uint8_t uint8
        cdef lib.uint16_t uint16
        cdef lib.uint32_t uint32
        cdef lib.uint64_t uint64
        if self.ptr.typeId == lib.MXF_INT8_TYPE:
            int8 = value
            error_check(lib.mxf_set_int8_item(data_set.ptr, &itemdef.ptr.key, int8 ))
        
        elif self.ptr.typeId == lib.MXF_INT16_TYPE:
            int16 = value
            error_check(lib.mxf_set_int16_item(data_set.ptr, &itemdef.ptr.key, int16 ))
        
        elif self.ptr.typeId == lib.MXF_INT32_TYPE:
            int32= value
            error_check(lib.mxf_set_int32_item(data_set.ptr, &itemdef.ptr.key, int32 ))

        elif self.ptr.typeId == lib.MXF_INT64_TYPE:
            int64 = value
            error_check(lib.mxf_set_int64_item(data_set.ptr, &itemdef.ptr.key, int64 ))

        elif self.ptr.typeId == lib.MXF_UINT8_TYPE:
            uint8 = value
            error_check(lib.mxf_set_uint8_item(data_set.ptr, &itemdef.ptr.key, uint8 ))

        elif self.ptr.typeId == lib.MXF_UINT16_TYPE:
            uint16 = value
            error_check(lib.mxf_set_uint16_item(data_set.ptr, &itemdef.ptr.key, uint16 ))

        elif self.ptr.typeId == lib.MXF_UINT32_TYPE:
            uint32 = value
            error_check(lib.mxf_set_uint32_item(data_set.ptr, &itemdef.ptr.key, uint32 ))

        elif self.ptr.typeId == lib.MXF_UINT64_TYPE:
            uint64 = value
            error_check(lib.mxf_set_uint64_item(data_set.ptr, &itemdef.ptr.key, uint64 ))

        else:
            raise NotImplementedError("not implementd for typeid: %d" % self.ptr.typeId)
    
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef lib.int8_t int8
        cdef lib.int16_t int16
        cdef lib.int32_t int32
        cdef lib.int64_t int64
        
        cdef lib.uint8_t uint8
        cdef lib.uint16_t uint16
        cdef lib.uint32_t uint32
        cdef lib.uint64_t uint64
        
        if self.ptr.typeId == lib.MXF_INT8_TYPE:
            error_check(lib.mxf_get_int8_item(data_set.ptr, &itemdef.ptr.key, &int8 ))
            return int8
        
        elif self.ptr.typeId == lib.MXF_INT16_TYPE:
            error_check(lib.mxf_get_int16_item(data_set.ptr, &itemdef.ptr.key, &int16 ))
            return int16
        
        elif self.ptr.typeId == lib.MXF_INT32_TYPE:
            error_check(lib.mxf_get_int32_item(data_set.ptr, &itemdef.ptr.key, &int32 ))
            return int32

        elif self.ptr.typeId == lib.MXF_INT64_TYPE:
            error_check(lib.mxf_get_int64_item(data_set.ptr, &itemdef.ptr.key, &int64 ))
            return int64
        
        elif self.ptr.typeId == lib.MXF_UINT8_TYPE:
            error_check(lib.mxf_get_uint8_item(data_set.ptr, &itemdef.ptr.key, &uint8 ))
            return uint8
        
        elif self.ptr.typeId == lib.MXF_UINT16_TYPE:
            error_check(lib.mxf_get_uint16_item(data_set.ptr, &itemdef.ptr.key, &uint16 ))
            return uint16
        
        elif self.ptr.typeId == lib.MXF_UINT32_TYPE:
            error_check(lib.mxf_get_uint32_item(data_set.ptr, &itemdef.ptr.key, &uint32 ))
            return uint32
        
        elif self.ptr.typeId == lib.MXF_UINT64_TYPE:
            error_check(lib.mxf_get_uint64_item(data_set.ptr, &itemdef.ptr.key, &uint64 ))
            return uint64
        
        else:
            raise NotImplementedError("not implementd for typeid: %d" % self.ptr.typeId)

#MXF_RAW_TYPE

# array
cdef class StringType(ItemType):

    def set_value(self, MetaDataSet data_set, ItemDef itemdef, bytes value):
        
        cdef lib.mxfUTF16Char* utf16Str = NULL
        cdef size_t size = len(value)
        
        utf16Str = <lib.mxfUTF16Char*> malloc( sizeof(lib.mxfUTF16Char) * (size + 1) )
        if utf16Str == NULL:
            raise MemoryError()
        
        memset(utf16Str, 0, sizeof(lib.mxfUTF16Char)* (size + 1))
        
        try:
            s = lib.mxf_utf8_to_utf16(utf16Str, value, size+1)
            error_check(lib.mxf_set_utf16string_item(data_set.ptr, &itemdef.ptr.key, utf16Str))
        finally:
            if utf16Str != NULL:
                free(utf16Str)

    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef lib.uint16_t utf16Size
        cdef lib.mxfUTF16Char* utf16Str = NULL
        error_check(lib.mxf_get_utf16string_item_size(data_set.ptr, &itemdef.ptr.key, &utf16Size))
        utf16Str = <lib.mxfUTF16Char*> malloc(utf16Size * sizeof(lib.mxfUTF16Char))
        if utf16Str == NULL:
            raise MemoryError()
        try:
            error_check(lib.mxf_get_utf16string_item(data_set.ptr, &itemdef.ptr.key, utf16Str))
            return uft16_to_bytes(utf16Str, utf16Size)
        finally:
            if utf16Str != NULL:
                free(utf16Str)

# MXF_UTF16STRINGARRAY_TYPE

# MXF_INT32ARRAY_TYPE MXF_UINT32ARRAY_TYPE MXF_INT64ARRAY_TYPE 
# MXF_UINT8ARRAY_TYPE MXF_INT32BATCH_TYPE MXF_UINT32BATCH_TYPE

cdef class IntergerArrayType(ItemType):

    def append(self, MetaDataSet data_set, ItemDef itemdef, value):
        cdef lib.int32_t int32
        cdef lib.int64_t int64
        
        cdef lib.uint8_t uint8
        cdef lib.uint32_t uint32
        
        cdef lib.uint32_t element_len
        
        if self.ptr.typeId in (lib.MXF_INT32BATCH_TYPE,lib.MXF_INT32ARRAY_TYPE):
            element_len= sizeof(lib.int32_t)
        elif self.ptr.typeId == lib.MXF_INT64ARRAY_TYPE:
            element_len= sizeof(lib.int64_t)
        elif self.ptr.typeId == lib.MXF_UINT8ARRAY_TYPE:
            element_len= sizeof(lib.uint8_t)
        elif self.ptr.typeId in (lib.MXF_UINT32BATCH_TYPE, lib.MXF_UINT32ARRAY_TYPE):
            element_len= sizeof(lib.uint32)
        else:
            raise NotImplementedError("get_value not implemented for type: %s" % self.name)
        
        cdef lib.uint8_t *array_element
        cdef MetaDataItem data_item = data_set[itemdef.name]
        
        if data_item.ptr == NULL:
            error_check(lib.mxf_alloc_array_item_elements(data_set.ptr, &itemdef.ptr.key, element_len, 1, &array_element))
        else:
            error_check(lib.mxf_grow_array_item(data_set.ptr, &itemdef.ptr.key, element_len, 1, &array_element))
            
            
        if self.ptr.typeId in (lib.MXF_INT32BATCH_TYPE,lib.MXF_INT32ARRAY_TYPE):
            int32 = value 
            lib.mxf_set_int32(int32, array_element)
                
        elif self.ptr.typeId == lib.MXF_INT64ARRAY_TYPE:
            int64 = value
            lib.mxf_set_int32(int64, array_element)

        elif self.ptr.typeId == lib.MXF_UINT8ARRAY_TYPE:
            uint8 = value
            lib.mxf_set_uint8(uint8, array_element)

        elif self.ptr.typeId in (lib.MXF_UINT32BATCH_TYPE, lib.MXF_UINT32ARRAY_TYPE):
            uint32 = value
            lib.mxf_set_uint32(uint32, array_element)
            

    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef lib.uint32_t length
        cdef lib.MXFArrayItemIterator array_iter
        
        cdef lib.mxfUL value
        cdef lib.uint8_t *array_element
        item_list = []
        
        cdef lib.int32_t int32
        cdef lib.int64_t int64
        
        cdef lib.uint8_t uint8
        cdef lib.uint32_t uint32
        
        
        error_check(lib.mxf_initialise_array_item_iterator(data_set.ptr, &itemdef.ptr.key, &array_iter))
        while lib.mxf_next_array_item_element(&array_iter, &array_element, &length):
            
            if self.ptr.typeId in (lib.MXF_INT32BATCH_TYPE,lib.MXF_INT32ARRAY_TYPE):  
                lib.mxf_get_int32(array_element, &int32)
                item_list.append(int32)
                
            elif self.ptr.typeId == lib.MXF_INT64ARRAY_TYPE:
                lib.mxf_get_int32(array_element, &int64)
                item_list.append(int64)
                
            elif self.ptr.typeId == lib.MXF_UINT8ARRAY_TYPE:
                lib.mxf_get_uint8(array_element, &uint8)
                item_list.append(uint8)
                
            elif self.ptr.typeId in (lib.MXF_UINT32BATCH_TYPE, lib.MXF_UINT32ARRAY_TYPE):
                lib.mxf_get_uint32(array_element, &uint32)
                item_list.append(uint32)
                
            else:
                raise NotImplementedError("get_value not implemented for type: %s" % self.name)
        
        return item_list

# MXF_ISO7STRING_TYPE
# MXF_AUIDARRAY_TYPE
# MXF_ULARRAY_TYPE
# MXF_ULBATCH_TYPE
cdef class ULArrayType(ItemType):
    def append(self, MetaDataSet data_set, ItemDef itemdef, value):
        cdef lib.mxfUL key
        UUID_to_mxfUL(value, &key)
        cdef lib.uint8_t *array_element 

        cdef MetaDataItem data_item = data_set[itemdef.name]
        if data_item.ptr == NULL:
            error_check(lib.mxf_alloc_array_item_elements(data_set.ptr, &itemdef.ptr.key, lib.mxfUL_extlen, 1, &array_element))
        else:
            error_check(lib.mxf_grow_array_item(data_set.ptr, &itemdef.ptr.key, lib.mxfUL_extlen, 1, &array_element))
            
        lib.mxf_set_ul(&key, array_element)
        
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        
        cdef lib.uint32_t length
        cdef lib.MXFArrayItemIterator array_iter
        
        cdef lib.mxfUL value
        cdef lib.uint8_t *array_element
        item_list = []
       
        error_check(lib.mxf_initialise_array_item_iterator(data_set.ptr, &itemdef.ptr.key, &array_iter))
        while lib.mxf_next_array_item_element(&array_iter, &array_element, &length):
            lib.mxf_get_ul(array_element, &value)
            item_list.append(mxfUL_to_UUID(value))            
        return item_list

# MXF_STRONGREFARRAY_TYPE
cdef class StrongRefArrayType(ItemType):
    def append(self, MetaDataSet data_set, ItemDef itemdef, MetaDataSet value):
        error_check(lib.mxf_add_array_item_strongref(data_set.ptr, &itemdef.ptr.key, value.ptr))
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        
        cdef MetaDataSet set_item
        cdef lib.MXFArrayItemIterator array_iter
        cdef lib.uint32_t length
        
        cdef lib.mxfUL label
        
        cdef lib.uint8_t *array_element
        item_list = []
        
        error_check(lib.mxf_initialise_array_item_iterator(data_set.ptr, &itemdef.ptr.key, &array_iter))
        
        while lib.mxf_next_array_item_element(&array_iter, &array_element, &length):
            try:
                set_item = MetaDataSet()
                error_check(lib.mxf_get_strongref(data_set.header.ptr, array_element, &set_item.ptr))
                set_item.model = data_set.model
                set_item.header = data_set.header
                item_list.append(set_item)
            except:
                lib.mxf_get_ul(array_element, &label)
                item_list.append( mxfUL_to_UUID(label))
        
        
        return item_list
    
# MXF_STRONGREFBATCH_TYPE
# MXF_WEAKREFARRAY_TYPE
# MXF_WEAKREFBATCH_TYPE
# MXF_RATIONALARRAY_TYPE
# MXF_RGBALAYOUT_TYPE

# 0x52 red
# 0x47 green
# 0x42 blue
# 0x41 alpha
# 0x46 fill
# 0x50 palette
# 0x00 terminates list

def chunks(l, n):
    """ Yield successive n-sized chunks from l.
    """
    for i in xrange(0, len(l), n):
        yield l[i:i+n]
        
cdef class RGBALayoutType(ItemType):
    
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        
        cdef MetaDataItem data_item = data_set[itemdef.name]
        items = []
        for layout in chunks(data_item.raw_value, 2):
            code, depth = unpack(">BB",layout)
            if code == 0x0:
                break
            items.append({'code':code, 'depth': depth})
        return items
        #cdef lib.mxfRGBALayoutComponent value
        #error_check(lib.mxf_get_rgba_layout_component_item(data_set.ptr, &itemdef.ptr.key, &value))
        #return value

# compound 

cdef class RationalType(ItemType):

    def set_value(self, MetaDataSet data_set, ItemDef itemdef, value):
        
        num, den = value.split("/")
        
        cdef lib.mxfRational rat
        rat.numerator = int(num)
        rat.denominator = int(den)
        error_check(lib.mxf_set_rational_item(data_set.ptr, &itemdef.ptr.key, &rat))
        
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef lib.mxfRational value
        error_check(lib.mxf_get_rational_item(data_set.ptr, &itemdef.ptr.key, &value))
        
        return "%d/%d" % (value.numerator, value.denominator)
    
cdef class TimeStampType(ItemType):
    def set_value(self, MetaDataSet data_set, ItemDef itemdef, bytes value):
        d = datetime.strptime(value, "%Y-%m-%d %H:%M:%S.%f")
        
        cdef lib.mxfTimestamp ts
        ts.year = d.year
        ts.month = d.month
        ts.day = d.day
        ts.hour = d.hour
        ts.min = d.minute
        ts.qmsec = int(255 * d.microsecond/1000000.0)
        #ts.qmsec = 0
        
        error_check(lib.mxf_set_timestamp_item(data_set.ptr, &itemdef.ptr.key, &ts))
        
        

    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef lib.mxfTimestamp value
        error_check(lib.mxf_get_timestamp_item(data_set.ptr, &itemdef.ptr.key, &value))

        microsecond = int(value.qmsec/255.0 * 1000000)

        d = datetime(year = value.year,
                      month = value.month,
                      day = value.day,
                      hour = value.hour,
                      minute = value.min,
                      second = value.sec,
                      microsecond = microsecond
                      )
        return d
# MXF_PRODUCTVERSION_TYPE
cdef class ProductVersionType(ItemType):
    def set_value(self, MetaDataSet data_set, ItemDef itemdef, value):
        error_check(lib.mxf_set_product_version_item(data_set.ptr, &itemdef.ptr.key, lib.mxf_get_version()))
    
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef lib.mxfProductVersion value
        cdef MetaDataItem data_item = data_set[itemdef.name]
        if data_item.length == 9:
            value.major, value.minor, value.patch,value.build, value.release = unpack(">HHHHB", data_item.raw_value)
            return value
        error_check(lib.mxf_get_product_version_item(data_set.ptr, &itemdef.ptr.key, &value))
        return value
        
# MXF_INDIRECT_TYPE
cdef class IndirectType(ItemType):
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef lib.mxfUTF16Char *name
        cdef lib.mxfUTF16Char *value
        cdef size_t value_size
        try:
            error_check(lib.mxf_avid_read_string_tagged_value(data_set.ptr, &name, &value))
            
            value_size = lib.mxf_utf16_to_utf8(NULL, value, 0)
            return uft16_to_bytes(value, value_size)
        finally:
            if name:
                free(name)
            if value:
                free(value)
        
        #cdef MetaDataSet value = MetaDataSet()
        #error_check(lib.mxf_dereference(data_set.ptr, &itemdef.ptr.key, &value.ptr))
        #return value
# MXF_RGBALAYOUTCOMPONENT_TYPE

# interpret 

cdef class VersionType(ItemType):
    
    def set_value(self, MetaDataSet data_set, ItemDef itemdef, lib.mxfVersionType value):
        
        error_check(lib.mxf_set_version_type_item(data_set.ptr, &itemdef.ptr.key, value))
    
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        
        cdef lib.mxfVersionType value
        error_check(lib.mxf_get_version_type_item(data_set.ptr, &itemdef.ptr.key, &value))
        return value
# MXF_UTF16_TYPE
# MXF_BOOLEAN_TYPE
cdef class BooleanType(ItemType):
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef lib.mxfBoolean value
        error_check(lib.mxf_get_boolean_item(data_set.ptr, &itemdef.ptr.key, &value))
        if value:
            return True
        else:
            return False

# MXF_ISO7_TYPE
# MXF_LENGTH_TYPE
cdef class LengthType(ItemType):
    def set_value(self, MetaDataSet data_set, ItemDef itemdef, lib.mxfLength value):
        error_check(lib.mxf_set_length_item(data_set.ptr, &itemdef.ptr.key, value))
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef lib.mxfLength value
        error_check(lib.mxf_get_length_item(data_set.ptr, &itemdef.ptr.key, &value))
        return value

# MXF_POSITION_TYPE
cdef class PositionType(ItemType):
    def set_value(self, MetaDataSet data_set, ItemDef itemdef, lib.mxfPosition value):
        error_check(lib.mxf_set_position_item(data_set.ptr, &itemdef.ptr.key, value))
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef lib.mxfPosition value
        error_check(lib.mxf_get_position_item(data_set.ptr, &itemdef.ptr.key, &value))
        return value
# MXF_RGBACODE_TYPE
# MXF_STREAM_TYPE
# MXF_DATAVALUE_TYPE
# MXF_IDENTIFIER_TYPE
# MXF_OPAQUE_TYPE
# MXF_UMID_TYPE
cdef class UMIDType(ItemType):
    def set_value(self, MetaDataSet data_set, ItemDef itemdef, MXFUMID value):
        error_check(lib.mxf_set_umid_item(data_set.ptr, &itemdef.ptr.key, &value.label))
        
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef MXFUMID umid = MXFUMID()
        error_check(lib.mxf_get_umid_item(data_set.ptr, &itemdef.ptr.key, &umid.label))
        return umid

# MXF_UID_TYPE
# MXF_UL_TYPE

cdef class ULType(ItemType):
    
    def set_value(self, MetaDataSet data_set, ItemDef itemdef, object value):
        cdef lib.mxfUL key
        UUID_to_mxfUL(value, &key)
        error_check(lib.mxf_set_ul_item(data_set.ptr, &itemdef.ptr.key, &key))
    
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef lib.mxfUL key
        error_check(lib.mxf_get_ul_item(data_set.ptr, &itemdef.ptr.key, &key))
        return mxfUL_to_UUID(key)

#MXF_UUID_TYPE          
cdef class UUIDType(ItemType):

    def set_value(self, MetaDataSet data_set, ItemDef itemdef, object value):
        cdef lib.mxfUUID label
        UUID_to_mxfUUID(value, &label)
        error_check(lib.mxf_set_uuid_item(data_set.ptr, &itemdef.ptr.key, &label))
        
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef lib.mxfUUID label
        error_check(lib.mxf_get_uuid_item(data_set.ptr, &itemdef.ptr.key, &label))
        return mxfUUID_to_UUID(label)
    
# MXF_AUID_TYPE
# MXF_PACKAGEID_TYPE
# MXF_STRONGREF_TYPE
cdef class StrongRefType(ItemType):
    def set_value(self, MetaDataSet data_set, ItemDef itemdef, MetaDataSet value):
        error_check(lib.mxf_set_strongref_item(data_set.ptr, &itemdef.ptr.key, value.ptr))
    
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef MetaDataSet value = MetaDataSet()
        cdef MetaDataItem data_item = data_set[itemdef.name]
        
        cdef lib.mxfUL label
        
        try:
            error_check(lib.mxf_get_strongref_item(data_set.ptr, &itemdef.ptr.key, &value.ptr))
            value.model = data_set.model
            value.header = data_set.header
            return value
        except:
            lib.mxf_get_ul(data_item.ptr.value, &label)
            return mxfUL_to_UUID(label)
            
            
        
# MXF_WEAKREF_TYPE
cdef class WeakRefType(ItemType):
    def set_value(self, MetaDataSet data_set, ItemDef itemdef, MetaDataSet value):
        error_check(lib.mxf_set_weakref_item(data_set.ptr, &itemdef.ptr.key, value.ptr))
        
    def get_value(self, MetaDataSet data_set, ItemDef itemdef):
        cdef MetaDataSet value = MetaDataSet()
        error_check(lib.mxf_get_weakref_item(data_set.ptr, &itemdef.ptr.key, &value.ptr))
        value.model = data_set.model
        value.header = data_set.header
        return value
# MXF_ORIENTATION_TYPE
# MXF_CODED_CONTENT_TYPE_TYPE

cdef class ItemDef(object):

    def itemtype(self):
        return self.model.find_itemtype(self)
    
    def get_value(self, MetaDataSet data_set):
        return self.itemtype().get_value(data_set, self)
    
    def set_value(self,  MetaDataSet data_set, value):
        self.itemtype().set_value(data_set, self, value)
        
    def append(self, MetaDataSet data_set, value):
        self.itemtype().append(data_set, self, value)


    property name:
        def __get__(self):
            return self.ptr.name
    property key:
        def __get__(self):
            cdef MXFKEY key = MXFKEY()
            key.label = self.ptr.key
            return key
            

cdef class SetDef(object):

    def __repr__(self):
        return '<%s.%s of %s at 0x%x>' % (
            self.__class__.__module__,
            self.__class__.__name__,
            self.name,
            id(self),
        )
    
    def iter_itemdefs(self):
        cdef ItemDef itemdef
        cdef lib.MXFListIterator item_iter
        lib.mxf_initialise_list_iter(&item_iter, &self.ptr.itemDefs)
        while lib.mxf_next_list_iter_element(&item_iter):
            itemdef = ItemDef()
            itemdef.ptr = <lib.MXFItemDef *> lib.mxf_get_iter_element(&item_iter)
            itemdef.model = self.model
            yield itemdef
            
        parent = self.parent_setdef()
        if parent:
            for itemdef in parent.iter_itemdefs():
                yield itemdef
                
    def parent_setdef(self):
        cdef SetDef parent
        if self.ptr.parentSetDef:
            parent = SetDef()
            parent.ptr = self.ptr.parentSetDef
            parent.model = self.model
            return parent

    property name:
        def __get__(self):
            return self.ptr.name
    property key:
        def __get__(self):
            return mxfUL_to_UUID(self.ptr.key)


cdef class DataModel(object):
    def __init__(self):
        error_check(lib.mxf_load_data_model(&self.ptr))
        error_check(lib.mxf_avid_load_extensions(self.ptr))
        error_check(lib.mxf_finalise_data_model(self.ptr))
        
    def __dealloc__(self):
        if self.ptr:
            lib.mxf_free_data_model(&self.ptr)
            
    def find_setdef_by_name(self, bytes name):
        for setdef in self.iter_setdefs():
            if setdef.name == name:
                return setdef
        
        raise ValueError("no such set named: %s" % name )
            
    def iter_setdefs(self):
        cdef SetDef setdef
        
        set_def_list = setdef_tree_to_list(&self.ptr.setDefs)
        for setdef in set_def_list:
            setdef.model = self
            yield setdef
#         
#         cdef lib.MXFListIterator set_iter
#         lib.mxf_initialise_list_iter(&set_iter, &self.ptr.setDefs)
#         while lib.mxf_next_list_iter_element(&set_iter):
#             setdef = SetDef()
#             setdef.ptr = <lib.MXFSetDef *> lib.mxf_get_iter_element(&set_iter)
#             setdef.model = self
#             yield setdef
#     
    def iter_itemdefs(self):
        cdef ItemDef itemdef
        cdef lib.MXFListIterator item_iter
        lib.mxf_initialise_list_iter(&item_iter, &self.ptr.itemDefs)
        while lib.mxf_next_list_iter_element(&item_iter):
            itemdef = ItemDef()
            itemdef.ptr = <lib.MXFItemDef *> lib.mxf_get_iter_element(&item_iter)
            itemdef.model = self
            yield itemdef
            
    def find_setdef(self,MetaDataSet setitem):
        cdef SetDef setdef = SetDef()
        error_check(lib.mxf_find_set_def(self.ptr, &setitem.ptr.key, &setdef.ptr))
        setdef.model = self
        return setdef
    
    def find_itemdef(self,MetaDataItem item):
        cdef ItemDef itemdef = ItemDef()
        error_check(lib.mxf_find_item_def(self.ptr, &item.ptr.key, &itemdef.ptr))
        itemdef.model = self
        return itemdef
    
    def find_itemtype(self,ItemDef itemdef):
        cdef ItemType itemtype = ItemType()
        itemtype.ptr = lib.mxf_get_item_def_type(self.ptr, itemdef.ptr.typeId)
        itemtype.model = self
        return resolve_itemtype(itemtype)
    
cdef int setdef_tree_append_list(void * node_data, void*process_data):
    cdef lib.MXFSetDef* set_def = <lib.MXFSetDef*> node_data
    cdef list python_list = <list> process_data
    cdef SetDef set_def_obj = SetDef()
    set_def_obj.ptr = set_def
    python_list.append(set_def_obj)
    return 1
    
cdef object setdef_tree_to_list(lib.MXFTree *tree):
    cdef list python_list = list()
    error_check(lib.mxf_tree_traverse(tree, <void *> setdef_tree_append_list, <void *> python_list))
    return python_list
    
cdef object resolve_itemtype(ItemType itemtype):

    # basic 
    if itemtype.ptr.typeId in (lib.MXF_INT8_TYPE, lib.MXF_INT16_TYPE, lib.MXF_INT32_TYPE,  lib.MXF_INT64_TYPE,
                               lib.MXF_UINT8_TYPE,lib.MXF_UINT16_TYPE,lib.MXF_UINT32_TYPE, lib.MXF_UINT64_TYPE):
        return IntegerType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_RAW_TYPE:
        pass
    
    # array
    
    elif itemtype.ptr.typeId == lib.MXF_UTF16STRING_TYPE:
        return StringType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_UTF16STRINGARRAY_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_ISO7STRING_TYPE:
        pass
    
    
    elif itemtype.ptr.typeId in (lib.MXF_INT32ARRAY_TYPE,lib.MXF_INT32BATCH_TYPE,
                                 lib.MXF_INT64ARRAY_TYPE,
                                 lib.MXF_UINT8ARRAY_TYPE,
                                 lib.MXF_UINT32ARRAY_TYPE,lib.MXF_UINT32BATCH_TYPE):
        return IntergerArrayType(itemtype)
    
    
    elif itemtype.ptr.typeId == lib.MXF_AUIDARRAY_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_ULARRAY_TYPE:
        return ULArrayType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_ULBATCH_TYPE:
        return ULArrayType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_STRONGREFARRAY_TYPE:
        return StrongRefArrayType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_STRONGREFBATCH_TYPE:
        return StrongRefArrayType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_WEAKREFARRAY_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_WEAKREFBATCH_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_RATIONALARRAY_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_RGBALAYOUT_TYPE:
        return RGBALayoutType(itemtype)
    
    # compound 
    elif itemtype.ptr.typeId == lib.MXF_RATIONAL_TYPE:
        return RationalType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_TIMESTAMP_TYPE:
        return TimeStampType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_PRODUCTVERSION_TYPE:
        return ProductVersionType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_INDIRECT_TYPE:
        return IndirectType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_RGBALAYOUTCOMPONENT_TYPE:
        pass
    
    # interpret 
    elif itemtype.ptr.typeId == lib.MXF_VERSIONTYPE_TYPE:
        return VersionType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_UTF16_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_BOOLEAN_TYPE:
        return BooleanType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_ISO7_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_LENGTH_TYPE:
        return LengthType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_POSITION_TYPE:
        return PositionType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_RGBACODE_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_STREAM_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_DATAVALUE_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_IDENTIFIER_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_OPAQUE_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_UMID_TYPE:
        return UMIDType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_UID_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_UL_TYPE:
        return ULType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_UUID_TYPE:
        return UUIDType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_AUID_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_PACKAGEID_TYPE:
        return UMIDType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_STRONGREF_TYPE:
        return StrongRefType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_WEAKREF_TYPE:
        return WeakRefType(itemtype)
    elif itemtype.ptr.typeId == lib.MXF_ORIENTATION_TYPE:
        pass
    elif itemtype.ptr.typeId == lib.MXF_CODED_CONTENT_TYPE_TYPE:
        pass

    
    return itemtype