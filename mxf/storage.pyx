
cimport lib
cimport stdio
from .util cimport error_check, uft16_to_bytes, MXFUL, MXFKEY, UUID_to_mxfUL,UUID_to_mxfUUID, mxfUUID_to_UUID
from .datamodel cimport DataModel, SetDef
from .metadata cimport HeaderMetadata 

import datetime

from .util import find_essence_element_key



cdef class Partition(object):
    def __init__(self):
        self.ptr = NULL
        
    def __dealloc__(self):
        if self.ptr:
            lib.mxf_free_partition(&self.ptr)
            
    def is_op_1a(self):
        
        if lib.is_op_1a(&self.ptr.operationalPattern):
            return True
        return False
            
    def is_op_atom(self):
        
        if lib.is_op_atom(&self.ptr.operationalPattern):
            return True
        return False
    
    def essence_containers(self):
        
        cdef lib.MXFListIterator list_iter
        items = []
        lib.mxf_initialise_list_iter(&list_iter, &self.ptr.essenceContainers)

        cdef MXFUL ul
        
        while lib.mxf_next_list_iter_element(&list_iter):
            ul = MXFUL()
            lib.mxf_get_ul(<lib.uint8_t*> lib.mxf_get_iter_element(&list_iter), &ul.label)
            items.append(ul)
        return items

cdef class File(object):

    def __init__(self, bytes path, bytes mode=None):
        
        if mode == 'r':
            error_check(lib.mxf_disk_file_open_read(path, &self.c_file))
        
        elif mode  == 'w':
            error_check(lib.mxf_disk_file_open_new(path, &self.c_file))
            
        elif mode == 'rw':
            raise NotImplementedError("mode: %s" % mode)
            #error_check(lib.mxf_disk_file_open_modify(path, &self.c_file))
        else:
            raise ValueError("invalid mode: %s" % mode)
        
    def __dealloc__(self):
        pass
        #lib.mxf_clear_file_partitions(&self.partitions)
    
    def seek(self, lib.int64_t position, int whence):
        error_check(lib.mxf_file_seek(self.c_file, position, whence))
        
        
    def open_essence(self, bytes essence_element_type, bytes mode):
        cdef lib.mxfUL key
        UUID_to_mxfUL(find_essence_element_key(essence_element_type), &key)
        
        cdef EssenceElement element = EssenceElement()
        
        if mode == 'w':
            error_check(lib.mxf_open_essence_element_write(self.c_file, &key, 8, 0, &element.ptr))
        
        else:
            raise NotImplementedError("not implements for mode: %s" % mode)
        
        element.file = self
        
        return element
        
        
    def create_header(self):

        lib.mxf_initialise_file_partitions(&self.partitions)
        
        lib.mxf_file_set_min_llen(self.c_file, 4)
        
        # load the Data model plus AVID extensions
        cdef DataModel model = DataModel()
        
        
        self.header_partition = Partition()
        
        # write the header partition pack
        error_check(lib.mxf_append_new_partition(&self.partitions, &self.header_partition.ptr))
        
        self.header_partition.ptr.key = lib.MXF_PP_K_ClosedComplete_Header
        self.header_partition.ptr.majorVersion = 1
        self.header_partition.ptr.minorVersion = 2
        self.header_partition.ptr.kagSize = 0x100
        self.header_partition.ptr.operationalPattern = lib.MXF_OP_L_atom_NTracks_1SourceClip
        
        error_check(lib.mxf_append_partition_esscont_label(self.header_partition.ptr, &lib.MXF_EC_L_DVBased_50_625_50_ClipWrapped ))
        
        error_check(lib.mxf_write_partition(self.c_file, self.header_partition.ptr))
        error_check(lib.mxf_fill_to_kag(self.c_file, self.header_partition.ptr))
        
        # create the header metadata
        
        cdef HeaderMetadata header = HeaderMetadata(model)
        
        # create the Avid meta-dictionary
        cdef lib.MXFMetadataSet* metaDictSet
        error_check(lib.mxf_avid_create_default_metadictionary(header.ptr, &metaDictSet))

        self.header = header

        self.body_partition = Partition()
        self.footer_partition = Partition()

        return header

    def write_header(self):
        self.header_metadata_pos = lib.mxf_file_tell(self.c_file)
        assert self.header_metadata_pos >= 0
        error_check(lib.mxf_mark_header_start(self.c_file, self.header_partition.ptr))
        error_check(lib.mxf_avid_write_header_metadata(self.c_file, self.header.ptr, self.header_partition.ptr))
        error_check(lib.mxf_fill_to_kag(self.c_file, self.header_partition.ptr))
        error_check(lib.mxf_mark_header_end(self.c_file, self.header_partition.ptr))
        
        cdef lib.uint32_t bodySID = 1
        
        error_check(lib.mxf_append_new_from_partition(&self.partitions, self.header_partition.ptr, &self.body_partition.ptr))
        
        self.body_partition.ptr.key = lib.MXF_PP_K_ClosedComplete_Body
        self.body_partition.ptr.kagSize = 0x200
        self.body_partition.ptr.bodySID = bodySID
        
        error_check(lib.mxf_write_partition(self.c_file, self.body_partition.ptr))
        error_check(lib.mxf_fill_to_kag(self.c_file, self.body_partition.ptr))
        
    def write_footer(self):
        cdef lib.uint32_t indexSID = 2
        
        error_check(lib.mxf_append_new_from_partition(&self.partitions, self.header_partition.ptr, &self.footer_partition.ptr))
        
        self.footer_partition.ptr.key = lib.MXF_PP_K_ClosedComplete_Footer
        self.footer_partition.ptr.kagSize = 0x200
        self.footer_partition.ptr.indexSID = indexSID
        
        error_check(lib.mxf_write_partition(self.c_file, self.footer_partition.ptr))
        error_check(lib.mxf_fill_to_kag(self.c_file, self.footer_partition.ptr))
        
    def write_index(self, IndexTableSegment index):
        error_check(lib.mxf_write_index_table_segment(self.c_file, index.ptr))
        error_check(lib.mxf_fill_to_kag(self.c_file, self.footer_partition.ptr))
        
    def update_header(self):
        error_check(lib.mxf_file_seek(self.c_file, self.header_metadata_pos, lib.SEEK_SET));
        error_check(lib.mxf_mark_header_start(self.c_file, self.header_partition.ptr));
        error_check(lib.mxf_avid_write_header_metadata(self.c_file, self.header.ptr, self.header_partition.ptr));
        error_check(lib.mxf_fill_to_kag(self.c_file, self.header_partition.ptr));
        error_check(lib.mxf_mark_header_end(self.c_file, self.header_partition.ptr));
        
    def update_partitions(self):
        error_check(lib.mxf_update_partitions(self.c_file, &self.partitions))
    
    def read_header(self):
        cdef lib.mxfKey key
        cdef lib.uint8_t llen
        cdef lib.uint64_t length
        
        error_check(lib.mxf_read_header_pp_kl(self.c_file, &key, &llen, &length))

        self.header_partition = Partition()
        error_check(lib.mxf_read_partition(self.c_file, &key, &self.header_partition.ptr))
        
         # load the Data model plus AVID extensions
        cdef DataModel model = DataModel()
        self.header = HeaderMetadata(model)
        
        error_check(lib.mxf_read_next_nonfiller_kl(self.c_file, &key, &llen, &length))
        assert lib.mxf_is_header_metadata(&key)
        

        error_check(lib.mxf_avid_read_filtered_header_metadata(self.c_file, 0, self.header.ptr, self.header_partition.ptr.headerByteCount,
                                                               &key, llen, length))
        

        return self.header
    
    def close(self):
        lib.mxf_file_close(&self.c_file)
    
cdef class EssenceElement(object):
    
    
    def import_from_file(self, bytes path):
        cdef lib.uint32_t essenceBufferSize = 4096
        cdef size_t numRead
        
        cdef lib.uint8_t buffer[4096]
        cdef int done = 0
        
        
        cdef stdio.FILE *c_file
        
        c_file = stdio.fopen(path, 'rb')
        
        while not done:
            numRead = stdio.fread(buffer, 1, essenceBufferSize, c_file)
            
            if numRead < essenceBufferSize:
                if not stdio.feof(c_file):
                    raise IOError("Failed to read bytes from %s" % path)
                
                done= 1
            error_check(lib.mxf_write_essence_element_data(self.file.c_file, self.ptr, buffer, <lib.uint32_t> numRead))
    
    def close(self):
        error_check(lib.mxf_finalize_essence_element_write(self.file.c_file, self.ptr))
        lib.mxf_close_essence_element(&self.ptr)
        
    property size:
        def __get__(self):
            if self.ptr:
                return self.ptr.totalLen
        
        
cdef class IndexTableSegment(object):
    
    def __init__(self):
        error_check(lib.mxf_create_index_table_segment(&self.ptr))
        
    def __dealloc__(self):
        if self.ptr:
            lib.mxf_free_index_table_segment(&self.ptr)
            
    property instanceUID:
        def __get__(self):
            return mxfUUID_to_UUID(self.ptr.instanceUID)
        def __set__(self, value):
            cdef lib.mxfUUID label
            UUID_to_mxfUUID(value, &label)
            self.ptr.instanceUID = label
        
    property edit_rate:
        def __get__(self):
            return "%d/%d" % (self.ptr.indexEditRate.numerator, self.ptr.indexEditRate.denominator)
        def __set__(self, value):
            num, den = value.split('/')
            self.ptr.indexEditRate.numerator = int(num)
            self.ptr.indexEditRate.denominator = int(den)
    property start_position:
        def __get__(self):
            return self.ptr.indexStartPosition
        def __set__(self, lib.mxfPosition value):
            self.ptr.indexStartPosition = value

    property duration:
        def __get__(self):
            return self.ptr.indexDuration
        def __set__(self, lib.mxfLength value):
            self.ptr.indexDuration = value
        
    property edit_unit_byte_count:
        def __get__(self):
            return self.ptr.editUnitByteCount
        def __set__(self, lib.uint32_t value):
            self.ptr.editUnitByteCount =value
        
    property indexSID:
        def __get__(self):
            return self.ptr.indexSID
        def __set__(self, lib.uint32_t value):
            self.ptr.indexSID = value

    property bodySID:
        def __get__(self):
            return self.ptr.bodySID
        def __set__(self, lib.uint32_t value):
            self.ptr.bodySID = value
        
    property slice_count:
        def __get__(self):
            return self.ptr.sliceCount
        def __set__(self, lib.uint8_t value):
            self.ptr.sliceCount = value
        
    property pos_table_count:
        def __get__(self):
            return self.ptr.posTableCount
        def __set__(self, lib.uint8_t value):
            self.ptr.posTableCount = value
        


open = File