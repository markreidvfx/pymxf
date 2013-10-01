from libc.stdlib cimport malloc, free
from libc.stdio cimport sscanf

from uuid import UUID
cimport lib
import uuid

class Error(ValueError):
    pass

class LibError(Error):
    
    def __init__(self, msg, code=0):
        super(LibError, self).__init__(msg, code)
    
    @property
    def code(self):
        return self.args[1]
            
    def __str__(self):
        if self.args[1]:
            return '[Errno %d] %s' % (self.args[1], self.args[0])
        else:
            return '%s' % (self.args[0])


cdef object error_check(int result):
    if not result:
        raise LibError("", result)
    
    return result
    
cdef bytes uft16_to_bytes(lib.mxfUTF16Char *wstr, lib.uint16_t size):
    cdef char * str = NULL
    str = <char *> malloc((size) * sizeof(char))
    if str == NULL:
        raise MemoryError()
    try:
        lib.mxf_utf16_to_utf8(str, wstr, size)
        return str[:size-1] # zero Terminated?
    finally:
        if str !=NULL:
            free(str)
            
def get_platform_string():
    return lib.mxf_get_platform_string()
            
            
cdef object mxfUUID_to_UUID(lib.mxfUUID label):

    s = "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"  % \
        (label.octet0, label.octet1, label.octet2, label.octet3,
        label.octet4, label.octet5, label.octet6, label.octet7,
        label.octet8, label.octet9, label.octet10, label.octet11,
        label.octet12, label.octet13, label.octet14, label.octet15)

    return UUID(s)


cdef object mxfUL_to_UUID(lib.mxfUL label):

    s = "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"  % \
        (label.octet0, label.octet1, label.octet2, label.octet3,
        label.octet4, label.octet5, label.octet6, label.octet7,
        label.octet8, label.octet9, label.octet10, label.octet11,
        label.octet12, label.octet13, label.octet14, label.octet15)

    return UUID(s)

cdef object UUID_to_mxfUL(object value, lib.mxfUL *result):
    if isinstance(value, uuid.UUID):
        u = value
    else:
        u = uuid.UUID(value)
    
    cdef lib.mxfUL label 
        
    rc = sscanf(u.hex, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                &label.octet0, &label.octet1, &label.octet2, &label.octet3,
                &label.octet4, &label.octet5, &label.octet6, &label.octet7,
                &label.octet8, &label.octet9, &label.octet10, &label.octet11,
                &label.octet12, &label.octet13, &label.octet14, &label.octet15)
    if rc != 16:
        raise ValueError("Unable to convert UUID: %s" % (str(u)))
    result[0] = label

cdef object UUID_to_mxfUUID(object value, lib.mxfUUID *result):
    if isinstance(value, uuid.UUID):
        u = value
    else:
        u = uuid.UUID(value)
    
    cdef lib.mxfUUID label 
        
    rc = sscanf(u.hex, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                &label.octet0, &label.octet1, &label.octet2, &label.octet3,
                &label.octet4, &label.octet5, &label.octet6, &label.octet7,
                &label.octet8, &label.octet9, &label.octet10, &label.octet11,
                &label.octet12, &label.octet13, &label.octet14, &label.octet15)
    if rc != 16:
        raise ValueError("Unable to convert UUID: %s" % (str(u)))
    result[0] = label
    
def get_Null_UMID():
    cdef MXFUMID umid = MXFUMID()
    umid.label = lib.g_Null_UMID
    return umid

NULL_UL = mxfUL_to_UUID(lib.g_Null_UL)

def is_gc_essence_element(key):
    cdef lib.mxfUL ul
    UUID_to_mxfUL(key, &ul)
    return lib.mxf_is_gc_essence_element(&ul) == 1

def is_partition_pack(key):
    cdef lib.mxfUL ul
    UUID_to_mxfUL(key, &ul)
    return lib.mxf_is_partition_pack(&ul) == 1
    

OPERATION_PATTERNS = {
'atom':{'1Track_1SourceClip'  : mxfUL_to_UUID(lib.op_atom_1Track_1SourceClip),
        '1Track_NSourceClips' : mxfUL_to_UUID(lib.op_atom_1Track_NSourceClips),
        'NTracks_1SourceClip' : mxfUL_to_UUID(lib.op_atom_NTracks_1SourceClip),
        'NTracks_NSourceClips': mxfUL_to_UUID(lib.op_atom_NTracks_NSourceClips)
        },
'1a':  {'UniTrack_Stream_Internal'  : mxfUL_to_UUID(lib.op_1a_UniTrack_Stream_Internal),
        'MultiTrack_Stream_Internal': mxfUL_to_UUID(lib.op_1a_MultiTrack_Stream_Internal),
        'MultiTrack_Stream_External': mxfUL_to_UUID(lib.op_1a_MultiTrack_Stream_External)
        },
'1b':  {'MultiTrack_Stream_External': mxfUL_to_UUID(lib.op_1b_MultiTrack_Stream_External)}                      
}
def find_op_pattern(bytes name, bytes pattern=None):
    
    if pattern is None:
        for format, value in  OPERATION_PATTERNS.items():
            for item_name, type_uuid in value.items():
                if name == item_name:
                    return type_uuid
        raise ValueError("Unkown operation pattern: %s" % name)
    
    return OPERATION_PATTERNS[name][pattern]


def find_op_pattern_name(item_uuid):
    if item_uuid == NULL_UL:
        return None
    for format, value in OPERATION_PATTERNS.items():
        for name, type_uuid in value.items():
            if item_uuid == type_uuid:
                return format, name
            
    raise ValueError("Uknown operation pattern: %s" % (str(item_uuid)))

DATA_DEFS = {
'Picture':mxfUL_to_UUID(lib.DDEF_Picture),
'Sound':mxfUL_to_UUID(lib.DDEF_Sound),
'Timecode':mxfUL_to_UUID(lib.DDEF_Timecode),
'LegacyPicture':mxfUL_to_UUID(lib.DDEF_LegacyPicture),
'LegacySound':mxfUL_to_UUID(lib.DDEF_LegacySound),
'LegacyTimecode':mxfUL_to_UUID(lib.DDEF_LegacyTimecode),
'Data':mxfUL_to_UUID(lib.DDEF_Data),
'DescriptiveMetadata':mxfUL_to_UUID(lib.DDEF_DescriptiveMetadata),
}

def find_datadef(bytes name):
    return DATA_DEFS[name]

ESSENCE_ELEMENT_KEYS = {
'AvidMJPEGClipWrapped': mxfUL_to_UUID(lib.EE_AvidMJPEGClipWrapped),
'AvidMPEGClipWrapped': mxfUL_to_UUID(lib.EE_AvidMPEGClipWrapped),
'DNxHD': mxfUL_to_UUID(lib.EE_DNxHD),
'DVClipWrapped': mxfUL_to_UUID(lib.EE_DVClipWrapped),
'BWFClipWrapped': mxfUL_to_UUID(lib.EE_BWFClipWrapped),
'AES3ClipWrapped': mxfUL_to_UUID(lib.EE_AES3ClipWrapped),
'UncClipWrapped': mxfUL_to_UUID(lib.EE_UncClipWrapped),
'IMX': mxfUL_to_UUID(lib.EE_IMX),
'AvidUnc10BitClipWrapped': mxfUL_to_UUID(lib.EE_AvidUnc10BitClipWrapped),
}
def find_essence_element_key(bytes name):
    return ESSENCE_ELEMENT_KEYS[name]

def find_essence_coding_label(bytes name):
    for key_name, key in iter_labels_and_keys('essence_coding_labels'):
        if name == key_name:
            return key
        
    raise IndexError("No label named: %s" % name)

def find_essence_container_label(bytes name):
    
    for key_name, essence_key in iter_labels_and_keys('essence_containers'):
        if name == key_name:
            return essence_key
        
    raise IndexError("No label named: %s" % name)

def find_essence_container_label_name(key):
    for key_name, essence_key in iter_labels_and_keys('essence_containers'):
        if key == essence_key:
            return key_name
    

def generate_umid():
    cdef MXFUMID umid = MXFUMID()
    lib.mxf_generate_aafsdk_umid(&umid.label)
    return umid

    
cdef class MXFUL(object):
    pass

cdef class MXFKEY(object):
    pass

cdef class MXFUMID(object):

    def to_string(self):
        
        s = ("urn:smpte:umid:%02x%02x%02x%02x.%02x%02x%02x%02x.%02x%02x%02x%02x." +\
             "%02x" + \
             "%02x%02x%02x.") %\
        (self.label.octet0, self.label.octet1, self.label.octet2, self.label.octet3,
        self.label.octet4, self.label.octet5, self.label.octet6, self.label.octet7,
        self.label.octet8, self.label.octet9, self.label.octet10, self.label.octet11,
        self.label.octet12, # length
        self.label.octet13, self.label.octet14, self.label.octet15)
        #instanceHigh         instanceMid           instanceMid
        
        material =  "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x" %\
        (self.label.octet16, self.label.octet17, self.label.octet18, self.label.octet19,
        self.label.octet20, self.label.octet21, self.label.octet22, self.label.octet23,
        self.label.octet24, self.label.octet25, self.label.octet26, self.label.octet27,
        self.label.octet28, self.label.octet29, self.label.octet30, self.label.octet31)
        
        
        return s + str(UUID(material)).replace("-", '.')
    
    
    def __str__(self):
        return self.to_string()
    
    def __richcmp__(x, y, int op):
        
        if op == 2:
            return str(x) == str(y)
        
        elif op == 3:
            return str(x) != str(y)
       
        

def iter_labels_and_keys(bytes kind):
    cdef lib.MXFList *label_list
    
    if kind == 'essence_containers':
        error_check(lib.load_label_table_essence_containers(&label_list))
    elif kind == 'essence_coding_labels':
        error_check(lib.load_label_table_coding_labels(&label_list))
    else:
        raise ValueError("Invalid kind: %s" % kind)
    
    cdef lib.LabelTableItem *label_item
    
    cdef lib.MXFListIterator item_iter
    lib.mxf_initialise_list_iter(&item_iter, label_list)
    try:
        while lib.mxf_next_list_iter_element(&item_iter):
            label_item = <lib.LabelTableItem*> lib.mxf_get_iter_element(&item_iter)
            name = None
            key = None
            if label_item.name:
                name = label_item.name
            if label_item.key:
                key = mxfUL_to_UUID(label_item.key[0])
            yield name,key
    
    finally:
        lib.mxf_free_list(&label_list)
