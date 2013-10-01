
cimport lib
cimport stdio
from .util cimport error_check, uft16_to_bytes, UUID_to_mxfUL, mxfUL_to_UUID, UUID_to_mxfUUID, mxfUUID_to_UUID
from .datamodel cimport DataModel, SetDef
from .metadata cimport HeaderMetadata 

import datetime

from .util import find_essence_element_key,find_op_pattern_name,find_op_pattern



cdef class Partition(object):
    def __init__(self):
        self.ptr = NULL
        
    def __dealloc__(self):
        if self.ptr:
            lib.mxf_free_partition(&self.ptr)
    
    def essence_containers(self):
        
        cdef lib.MXFListIterator list_iter
        items = []
        lib.mxf_initialise_list_iter(&list_iter, &self.ptr.essenceContainers)

        cdef lib.mxfKey ul
        while lib.mxf_next_list_iter_element(&list_iter):
            lib.mxf_get_ul(<lib.uint8_t*> lib.mxf_get_iter_element(&list_iter), &ul)
            items.append(mxfUL_to_UUID(ul))
        return items
    
    def append_essence_container(self, container_uuid):
        cdef lib.mxfKey ul
        UUID_to_mxfUL(container_uuid, &ul)
        error_check(lib.mxf_append_partition_esscont_label(self.ptr, &ul))

    
    def __repr__(self):
        return '<%s.%s of %s %s at 0x%x>' % (
            self.__class__.__module__,
            self.__class__.__name__,
            self.format,
            self.type_name,
            id(self),
        )
    
    property type_name:
        def __get__(self):
            if lib.mxf_is_header_partition_pack(&self.ptr.key):
                return "Header"
            elif lib.mxf_is_body_partition_pack(&self.ptr.key):
                return "Body"
            elif lib.mxf_is_footer_partition_pack(&self.ptr.key):
                return "Footer"
            elif lib.mxf_is_partition_pack(&self.ptr.key):
                return "Pack"
            else:
                return "Unknown"
    property closed:
        def __get__(self):
            return lib.mxf_partition_is_closed(&self.ptr.key) == 1
    property complete:
        def __get__(self):
            return lib.mxf_partition_is_complete(&self.ptr.key) == 1
        
    property operational_pattern_name:
        def __get__(self):
            operation_pattern = find_op_pattern_name(mxfUL_to_UUID(self.ptr.operationalPattern))
            if operation_pattern:
                return operation_pattern[1]
        def __set__(self, bytes value):
            self.operational_pattern = find_op_pattern(value)

    property operational_pattern:
        def __get__(self):
            return mxfUL_to_UUID(self.ptr.operationalPattern)
        def __set__(self, value):
            cdef lib.mxfUL key
            UUID_to_mxfUL(value, &key)
            self.ptr.operationalPattern = key
    
    property format:
        def __get__(self):
            if lib.is_op_atom(&self.ptr.operationalPattern):
                return 'atom'
            elif lib.is_op_1a(&self.ptr.operationalPattern):
                return '1a'
            elif lib.is_op_1b(&self.ptr.operationalPattern):
                return '1b'
            else:
                return "Unknown"
        
    property key:
        def __get__(self):
            return mxfUL_to_UUID(self.ptr.key)
        def __set__(self, value):
            cdef lib.mxfUL key
            UUID_to_mxfUL(value, &key)
            self.ptr.key = key
    
    property kag_size:
        def __get__(self):
            return self.ptr.kagSize
        def __set__(self, lib.uint32_t value):
            self.ptr.kagSize = value
    
    property major_version:
        def __get__(self):
            return self.ptr.majorVersion
        def __set__(self, lib.uint16_t value):
            self.ptr.majorVersion = value
            
    property minor_version:
        def __get__(self):
            return self.ptr.minorVersion
        def __set__(self, lib.uint16_t value):
            self.ptr.minorVersion = value
    
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
        
    property position:
        def __get__(self):
            return self.ptr.thisPartition
        
    property footer_position:
        def __get__(self):
            return self.ptr.footerPartition
    
    property previous_position:
        def __get__(self):
            return self.ptr.previousPartition

cdef class File(object):

    def __init__(self, bytes path, bytes mode=None):
        
        if mode == 'r':
            error_check(lib.mxf_disk_file_open_read(path, &self.ptr))
        
        elif mode  == 'w':
            error_check(lib.mxf_disk_file_open_new(path, &self.ptr))
            lib.mxf_initialise_file_partitions(&self.partitions)
            self.min_llen = 4
            #lib.mxf_file_set_min_llen(self.c_file, 4)
            
        elif mode == 'rw':
            raise NotImplementedError("mode: %s" % mode)
            #error_check(lib.mxf_disk_file_open_modify(path, &self.c_file))
        else:
            raise ValueError("invalid mode: %s" % mode)
        
    def __dealloc__(self):
       pass
    
    def seek(self, lib.int64_t position, bytes mode=b"SEEK_SET"):
        cdef int whence
        if mode.lower() == "seek_set":
            whence = lib.SEEK_SET
        elif mode.lower() == 'seek_cur':
            whence = lib.SEEK_CUR
        elif mode.lower() == 'seek_end':
            whence = lib.SEEK_END
        else:
            raise ValueError("invalid seek mode:%s" % mode)
        
        error_check(lib.mxf_file_seek(self.ptr, position, whence))
        
    def tell(self):
        return lib.mxf_file_tell(self.ptr)

    def create_essence(self, essence_element_key):
        cdef lib.mxfUL key
        UUID_to_mxfUL(essence_element_key, &key)
        cdef EssenceElement element = EssenceElement()
        error_check(lib.mxf_open_essence_element_write(self.ptr, &key, 8, 0, &element.ptr))
        element.file = self
        return element
    
    def create_partition(self, bytes kind=None):
        cdef Partition part = Partition()
        error_check(lib.mxf_append_new_partition(&self.partitions, &part.ptr))
        
        if not kind:
            kind = b""
        
        if kind.lower() == "header":
            part.ptr.key = lib.MXF_PP_K_ClosedComplete_Header
            part.ptr.majorVersion = 1
            part.ptr.minorVersion = 2
            part.ptr.kagSize = 0x100
            #part.ptr.operationalPattern = lib.MXF_OP_L_atom_NTracks_1SourceClip
            
        elif kind.lower() == 'body':
            part.ptr.key = lib.MXF_PP_K_ClosedComplete_Body
            part.ptr.kagSize = 0x200
            part.ptr.bodySID = 1
            
        elif kind.lower() == 'footer':
            part.ptr.key = lib.MXF_PP_K_ClosedComplete_Footer
            part.ptr.kagSize = 0x200
            part.ptr.indexSID = 2
                
        return part
    
    def write_partition(self, Partition partition):
        error_check(lib.mxf_write_partition(self.ptr, partition.ptr))
        error_check(lib.mxf_fill_to_kag(self.ptr, partition.ptr))

    def write_header(self, HeaderMetadata header, Partition partition):
        error_check(lib.mxf_mark_header_start(self.ptr, partition.ptr))
        error_check(lib.mxf_avid_write_header_metadata(self.ptr, header.ptr, partition.ptr))
        error_check(lib.mxf_fill_to_kag(self.ptr, partition.ptr))
        error_check(lib.mxf_mark_header_end(self.ptr, partition.ptr))

    def write_index(self, IndexTableSegment index, Partition partition):
        error_check(lib.mxf_write_index_table_segment(self.ptr, index.ptr))
        error_check(lib.mxf_fill_to_kag(self.ptr, partition.ptr))
        
    def update_header(self, HeaderMetadata header, Partition partition, lib.int64_t position):
        error_check(lib.mxf_file_seek(self.ptr, position, lib.SEEK_SET));
        self.write_header(header, partition)

    def update_partitions(self):
        error_check(lib.mxf_update_partitions(self.ptr, &self.partitions))
        
    def read_kl(self):
        cdef lib.mxfKey key
        cdef lib.uint8_t llen
        cdef lib.uint64_t length 
        error_check(lib.mxf_read_kl(self.ptr, &key, &llen, &length))
        return mxfUL_to_UUID(key), llen, length
    
    def read_next_nonfiller_kl(self):
        cdef lib.mxfKey key
        cdef lib.uint8_t llen
        cdef lib.uint64_t length
        error_check(lib.mxf_read_next_nonfiller_kl(self.ptr, &key, &llen, &length))
        return mxfUL_to_UUID(key), llen, length
    
    def read_partition(self, key):
        cdef lib.mxfKey ul
        UUID_to_mxfUL(key, &ul)
        cdef Partition part = Partition()
        error_check(lib.mxf_read_partition(self.ptr, &ul, &part.ptr))
        return part
        
    def read_header_partition(self):
        cdef lib.mxfKey key
        cdef lib.uint8_t llen
        cdef lib.uint64_t length
        if not lib.mxf_read_header_pp_kl(self.ptr, &key, &llen, &length):
            raise IOError("Unable to read header")
        
        part = self.read_partition(mxfUL_to_UUID(key))
        return part
    
    def read_header(self, Partition header_partition=None):
        if not header_partition:
            self.seek(0)
            header_partition = self.read_header_partition()
        cdef lib.mxfKey key
        cdef lib.uint8_t llen
        cdef lib.uint64_t length
        
        uuid_key, llen, length = self.read_next_nonfiller_kl()
        UUID_to_mxfUL(uuid_key, &key)

        if not lib.mxf_is_header_metadata(&key):
            raise IOError("cannot find header")
        
        cdef HeaderMetadata header = HeaderMetadata()
        error_check(lib.mxf_avid_read_filtered_header_metadata(self.ptr, 0, header.ptr, header_partition.ptr.headerByteCount,
                                                               &key, llen, length))
        
        return header
    
    def read_partitions(self, Partition header_partition=None):
        
        if not header_partition:
            self.seek(0)
            header_partition = self.read_header_partition()
            
        cdef lib.mxfKey mxf_key
        cdef lib.uint8_t llen
        cdef lib.uint64_t length
        
        cdef lib.uint64_t this_partition
        
        cdef lib.MXFRIP rip
        cdef lib.MXFRIPEntry *rip_entry
        cdef lib.MXFListIterator list_iter
        
        cdef Partition part 
        
        partition_list = []
        
        # use the RIP if there is one
        if lib.mxf_read_rip(self.ptr, &rip):
            try:
                lib.mxf_initialise_list_iter(&list_iter, &rip.entries)
                while lib.mxf_next_list_iter_element(&list_iter):
                    rip_entry = <lib.MXFRIPEntry*> lib.mxf_get_iter_element(&list_iter)
                    self.seek(self.runin_len + rip_entry.thisPartition, "SEEK_SET")
                    
                    key, llen, length = self.read_kl()
                    part = self.read_partition(key)
                    partition_list.append(part)
            finally:
                lib.mxf_clear_rip(&rip)
        else:
            # start from footer partition and work back to the header partition
            pos = header_partition.footer_position
            if pos <= header_partition.position:
                return
            
            
            while True:
                self.seek(self.runin_len + pos, "SEEK_SET")
                key, llen, length = self.read_kl()
                part = self.read_partition(key)
                partition_list.append(part)
                
                print part.footer_position, part.position   
                pos = part.previous_position
                
                if not pos <  part.position: 
                    break
            partition_list.reverse()
                
        return partition_list
    
    def close(self):
        lib.mxf_file_close(&self.ptr)
        
    property size:
        def __get__(self):
            return lib.mxf_file_size(self.ptr)
        
    property min_llen:
        def __get__(self):
            return lib.mxf_get_min_llen(self.ptr)
        def __set__(self, lib.uint8_t value):
            lib.mxf_file_set_min_llen(self.ptr, value)
            
    property runin_len:
        def __get__(self):
            return lib.mxf_get_runin_len(self.ptr)
        def __set__(self, lib.uint16_t value):
            lib.mxf_set_runin_len(self.ptr, value)

    property seekable:
        def __get__(self):
            return lib.mxf_file_is_seekable(self.ptr) == 1
            
    property eof:
        def __get__(self):
            return lib.mxf_file_eof(self.ptr) == 1
    
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
            error_check(lib.mxf_write_essence_element_data(self.file.ptr, self.ptr, buffer, <lib.uint32_t> numRead))
        stdio.fclose(c_file)
    def close(self):
        error_check(lib.mxf_finalize_essence_element_write(self.file.ptr, self.ptr))
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