
cdef extern from "mxf/mxf.h":
    
    ctypedef struct MXFMetadataItem:
        mxfKey key
        uint16_t tag
        int isPersistent
        uint16_t length
        uint8_t* value
        MXFMetadataSet* set
        
    ctypedef struct MXFMetadataSet:
        mxfKey key
        mxfUUID instanceUID
        MXFList items
        MXFHeaderMetadata* headerMetadata
        uint64_t fixedSpaceAllocation
   
    ctypedef struct MXFHeaderMetadata:
        pass
        
    ctypedef struct MXFArrayItemIterator:
        pass
    
    ctypedef struct MXFReadFilter:
        pass
        
    cdef int mxf_is_header_metadata(mxfKey* key)
    
    cdef int mxf_create_header_metadata(MXFHeaderMetadata** headerMetadata, MXFDataModel* dataModel)
    cdef int mxf_create_set(MXFHeaderMetadata* headerMetadata, mxfKey* key, MXFMetadataSet** set)
    cdef int mxf_create_item(MXFMetadataSet* set, mxfKey* key, mxfLocalTag tag, MXFMetadataItem** item)
    cdef void mxf_free_header_metadata(MXFHeaderMetadata** headerMetadata)
    cdef void mxf_free_set(MXFMetadataSet** set)
    cdef void mxf_free_item(MXFMetadataItem** item)
    
    cdef int mxf_find_set_by_key(MXFHeaderMetadata* headerMetadata, mxfKey* key, MXFList** setList)
    cdef int mxf_find_singular_set_by_key(MXFHeaderMetadata* headerMetadata, mxfKey* key, MXFMetadataSet** set)
    cdef int mxf_get_item(MXFMetadataSet* set, mxfKey* key, MXFMetadataItem** resultItem)
    cdef int mxf_have_item(MXFMetadataSet* set, mxfKey* key)
    
    cdef int mxf_write_header_metadata(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata)
    cdef int mxf_write_header_primer_pack(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata)
    cdef int mxf_write_header_sets(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata)
    cdef int mxf_write_set(MXFFile* mxfFile, MXFMetadataSet* set)
    cdef int mxf_write_item(MXFFile* mxfFile, MXFMetadataItem* item)
    cdef void mxf_get_header_metadata_size(MXFFile* mxfFile, MXFHeaderMetadata* headerMetadata, uint64_t* size)
    cdef uint64_t mxf_get_set_size(MXFFile* mxfFile, MXFMetadataSet* set)
       
    cdef void mxf_get_uint8(uint8_t* value, uint8_t* result)
    cdef void mxf_get_uint16(uint8_t* value, uint16_t* result)
    cdef void mxf_get_uint32(uint8_t* value, uint32_t* result)
    cdef void mxf_get_uint64(uint8_t* value, uint64_t* result)
    cdef void mxf_get_int8(uint8_t* value, int8_t* result)
    cdef void mxf_get_int16(uint8_t* value, int16_t* result)
    cdef void mxf_get_int32(uint8_t* value, int32_t* result)
    cdef void mxf_get_int64(uint8_t* value, int64_t* result)
    cdef void mxf_get_version_type(uint8_t* value, mxfVersionType* result)
    cdef void mxf_get_uuid(uint8_t* value, mxfUUID* result)
    cdef void mxf_get_ul(uint8_t* value, mxfUL* result)
    cdef void mxf_get_auid(uint8_t* value, mxfAUID* result)
    cdef void mxf_get_umid(uint8_t* value, mxfUMID* result)
    cdef void mxf_get_timestamp(uint8_t* value, mxfTimestamp* result)
    cdef void mxf_get_length(uint8_t* value, mxfLength* result)
    cdef void mxf_get_rational(uint8_t* value, mxfRational* result)
    cdef void mxf_get_position(uint8_t* value, mxfPosition* result)
    cdef void mxf_get_boolean(uint8_t* value, mxfBoolean* result)
    cdef void mxf_get_product_version(uint8_t* value, mxfProductVersion* result)
    cdef void mxf_get_rgba_layout_component(uint8_t* value, mxfRGBALayoutComponent* result)
    cdef void mxf_get_array_header(uint8_t* value, uint32_t* arrayLen, uint32_t* arrayItemLen)
    cdef uint16_t mxf_get_utf16string_size(uint8_t* value, uint16_t valueLen)
    cdef void mxf_get_utf16string(uint8_t* value, uint16_t valueLen, mxfUTF16Char* result)
    
    cdef int mxf_get_strongref(MXFHeaderMetadata* headerMetadata, uint8_t* value, MXFMetadataSet** set)
    cdef int mxf_get_weakref(MXFHeaderMetadata* headerMetadata, uint8_t* value, MXFMetadataSet** set)
    cdef int mxf_dereference(MXFHeaderMetadata* headerMetadata, mxfUUID* uuid, MXFMetadataSet** set)

    cdef void mxf_initialise_sets_iter(MXFHeaderMetadata *headerMetadata, MXFListIterator *setsIter)
    cdef int mxf_get_strongref_s(MXFHeaderMetadata* headerMetadata, MXFListIterator* setsIter, uint8_t* value, MXFMetadataSet** set)
    cdef int mxf_get_weakref_s(MXFHeaderMetadata* headerMetadata, MXFListIterator* setsIter, uint8_t* value, MXFMetadataSet** set)
    cdef int mxf_dereference_s(MXFHeaderMetadata* headerMetadata, MXFListIterator* setsIter, mxfUUID* uuid, MXFMetadataSet** set)
    
    cdef void mxf_set_uint8(uint8_t value, uint8_t* result)
    cdef void mxf_set_uint16(uint16_t value, uint8_t* result)
    cdef void mxf_set_uint32(uint32_t value, uint8_t* result)
    cdef void mxf_set_uint64(uint64_t value, uint8_t* result)
    cdef void mxf_set_int8(int8_t value, uint8_t* result)
    cdef void mxf_set_int16(int16_t value, uint8_t* result)
    cdef void mxf_set_int32(int32_t value, uint8_t* result)
    cdef void mxf_set_int64(int64_t value, uint8_t* result)
    cdef void mxf_set_version_type(mxfVersionType value, uint8_t* result)
    cdef void mxf_set_uuid(const mxfUUID* value, uint8_t* result)
    cdef void mxf_set_ul(const mxfUL* value, uint8_t* result)
    cdef void mxf_set_auid(const mxfAUID* value, uint8_t* result)
    cdef void mxf_set_umid(const mxfUMID* value, uint8_t* result)
    cdef void mxf_set_timestamp(const mxfTimestamp* value, uint8_t* result)
    cdef uint16_t mxf_get_external_utf16string_size(const mxfUTF16Char* value)
    cdef void mxf_set_utf16string(const mxfUTF16Char* value, uint8_t* result)
    cdef void mxf_set_fixed_size_utf16string(const mxfUTF16Char* value, uint16_t size, uint8_t* result)
    cdef void mxf_set_strongref(const MXFMetadataSet* value, uint8_t* result)
    cdef void mxf_set_weakref(const MXFMetadataSet* value, uint8_t* result)
    cdef void mxf_set_rational(const mxfRational* value, uint8_t* result)
    cdef void mxf_set_position(mxfPosition value, uint8_t* result)
    cdef void mxf_set_length(mxfLength value, uint8_t* result)
    cdef void mxf_set_boolean(mxfBoolean value, uint8_t* result)
    cdef void mxf_set_product_version(const mxfProductVersion* value, uint8_t* result)
    cdef void mxf_set_rgba_layout_component(const mxfRGBALayoutComponent* value, uint8_t* result)
    cdef void mxf_set_array_header(uint32_t arrayLen, uint32_t arrayElementLen, uint8_t* result)
    
    cdef int mxf_set_item_value(MXFMetadataItem* item, uint8_t* value, uint16_t len)

    cdef int mxf_set_item(MXFMetadataSet* set, mxfKey* itemKey, uint8_t* value, uint16_t len)
    cdef int mxf_clone_item(MXFMetadataSet* sourceSet, mxfKey* itemKey, MXFMetadataSet* destSet)

    cdef int mxf_set_uint8_item(MXFMetadataSet* set, mxfKey* itemKey, uint8_t value)
    cdef int mxf_set_uint16_item(MXFMetadataSet* set, mxfKey* itemKey, uint16_t value)
    cdef int mxf_set_uint32_item(MXFMetadataSet* set, mxfKey* itemKey, uint32_t value)
    cdef int mxf_set_uint64_item(MXFMetadataSet* set, mxfKey* itemKey, uint64_t value)
    cdef int mxf_set_int8_item(MXFMetadataSet* set, mxfKey* itemKey, int8_t value)
    cdef int mxf_set_int16_item(MXFMetadataSet* set, mxfKey* itemKey, int16_t value)
    cdef int mxf_set_int32_item(MXFMetadataSet* set, mxfKey* itemKey, int32_t value)
    cdef int mxf_set_int64_item(MXFMetadataSet* set, mxfKey* itemKey, int64_t value)
    cdef int mxf_set_version_type_item(MXFMetadataSet* set, mxfKey* itemKey, mxfVersionType value)
    cdef int mxf_set_uuid_item(MXFMetadataSet* set, mxfKey* itemKey, mxfUUID* value)
    cdef int mxf_set_ul_item(MXFMetadataSet* set, mxfKey* itemKey, mxfUL* value)
    cdef int mxf_set_auid_item(MXFMetadataSet* set, mxfKey* itemKey, mxfAUID* value)
    cdef int mxf_set_umid_item(MXFMetadataSet* set, mxfKey* itemKey, mxfUMID* value)
    cdef int mxf_set_timestamp_item(MXFMetadataSet* set, mxfKey* itemKey, mxfTimestamp* value)
    cdef int mxf_set_utf16string_item(MXFMetadataSet* set, mxfKey* itemKey, mxfUTF16Char* value)
    cdef int mxf_set_fixed_size_utf16string_item(MXFMetadataSet* set, mxfKey* itemKey, mxfUTF16Char* value, uint16_t size)
    cdef int mxf_set_strongref_item(MXFMetadataSet* set, mxfKey* itemKey, MXFMetadataSet* value)
    cdef int mxf_set_weakref_item(MXFMetadataSet* set, mxfKey* itemKey, MXFMetadataSet* value)
    cdef int mxf_set_rational_item(MXFMetadataSet* set, mxfKey* itemKey, mxfRational* value)
    cdef int mxf_set_position_item(MXFMetadataSet* set, mxfKey* itemKey, mxfPosition value)
    cdef int mxf_set_length_item(MXFMetadataSet* set, mxfKey* itemKey, mxfLength value)
    cdef int mxf_set_boolean_item(MXFMetadataSet* set, mxfKey* itemKey, mxfBoolean value)
    cdef int mxf_set_product_version_item(MXFMetadataSet* set, mxfKey* itemKey, mxfProductVersion* value)
    cdef int mxf_set_rgba_layout_component_item(MXFMetadataSet* set, mxfKey* itemKey, mxfRGBALayoutComponent* value)

    cdef int mxf_alloc_array_item_elements(MXFMetadataSet* set, mxfKey* itemKey, uint32_t elementLen, uint32_t count, uint8_t** elements)
    cdef int mxf_grow_array_item(MXFMetadataSet* set, mxfKey* itemKey, uint32_t elementLen, uint32_t count, uint8_t** newElements)
    cdef int mxf_set_empty_array_item(MXFMetadataSet* set, mxfKey* itemKey, uint32_t elementLen)
    cdef int mxf_add_array_item_strongref(MXFMetadataSet* set, mxfKey* itemKey, MXFMetadataSet* value)
    cdef int mxf_add_array_item_weakref(MXFMetadataSet* set, mxfKey* itemKey, MXFMetadataSet* value)
    
    cdef int mxf_get_item_len(MXFMetadataSet* set, mxfKey* itemKey, uint16_t* len)

    cdef int mxf_get_uint8_item(MXFMetadataSet* set, mxfKey* itemKey, uint8_t* value)
    cdef int mxf_get_uint16_item(MXFMetadataSet* set, mxfKey* itemKey, uint16_t* value)
    cdef int mxf_get_uint32_item(MXFMetadataSet* set, mxfKey* itemKey, uint32_t* value)
    cdef int mxf_get_uint64_item(MXFMetadataSet* set, mxfKey* itemKey, uint64_t* value)
    cdef int mxf_get_int8_item(MXFMetadataSet* set, mxfKey* itemKey, int8_t* value)
    cdef int mxf_get_int16_item(MXFMetadataSet* set, mxfKey* itemKey, int16_t* value)
    cdef int mxf_get_int32_item(MXFMetadataSet* set, mxfKey* itemKey, int32_t* value)
    cdef int mxf_get_int64_item(MXFMetadataSet* set, mxfKey* itemKey, int64_t* value)
    cdef int mxf_get_version_type_item(MXFMetadataSet* set, mxfKey* itemKey, mxfVersionType* value)
    cdef int mxf_get_uuid_item(MXFMetadataSet* set, mxfKey* itemKey, mxfUUID* value)
    cdef int mxf_get_ul_item(MXFMetadataSet* set, mxfKey* itemKey, mxfUL* value)
    cdef int mxf_get_auid_item(MXFMetadataSet* set, mxfKey* itemKey, mxfAUID* value)
    cdef int mxf_get_umid_item(MXFMetadataSet* set, mxfKey* itemKey, mxfUMID* value)
    cdef int mxf_get_timestamp_item(MXFMetadataSet* set, mxfKey* itemKey, mxfTimestamp* value)
    cdef int mxf_get_utf16string_item_size(MXFMetadataSet* set, mxfKey* itemKey, uint16_t* size)
    cdef int mxf_get_utf16string_item(MXFMetadataSet* set, mxfKey* itemKey, mxfUTF16Char* value)
    cdef int mxf_get_strongref_item(MXFMetadataSet* set, mxfKey* itemKey, MXFMetadataSet** value)
    cdef int mxf_get_weakref_item(MXFMetadataSet* set, mxfKey* itemKey, MXFMetadataSet** value)
    cdef int mxf_get_strongref_item_s(MXFListIterator* setsIter, MXFMetadataSet* set, mxfKey* itemKey, MXFMetadataSet** value)
    cdef int mxf_get_weakref_item_s(MXFListIterator* setsIter, MXFMetadataSet* set, mxfKey* itemKey, MXFMetadataSet** value)
    cdef int mxf_get_length_item(MXFMetadataSet* set, mxfKey* itemKey, mxfLength* value)
    cdef int mxf_get_rational_item(MXFMetadataSet* set, mxfKey* itemKey, mxfRational* value)
    cdef int mxf_get_position_item(MXFMetadataSet* set, mxfKey* itemKey, mxfPosition* value)
    cdef int mxf_get_boolean_item(MXFMetadataSet* set, mxfKey* itemKey, mxfBoolean* value)
    cdef int mxf_get_product_version_item(MXFMetadataSet* set, mxfKey* itemKey, mxfProductVersion* value)
    cdef int mxf_get_rgba_layout_component_item(MXFMetadataSet* set, mxfKey* itemKey, mxfRGBALayoutComponent* value)

    cdef int mxf_get_array_item_count(MXFMetadataSet* set, mxfKey* itemKey, uint32_t* count)
    cdef int mxf_get_array_item_element_len(MXFMetadataSet* set, mxfKey* itemKey, uint32_t* elementLen)
    cdef int mxf_get_array_item_element(MXFMetadataSet* set, mxfKey* itemKey, uint32_t index, uint8_t** element)
    
    cdef int mxf_initialise_array_item_iterator(MXFMetadataSet* set, mxfKey* itemKey, MXFArrayItemIterator* arrayIter)
    cdef int mxf_next_array_item_element(MXFArrayItemIterator* arrayIter, uint8_t** value, uint32_t* length)
    
    