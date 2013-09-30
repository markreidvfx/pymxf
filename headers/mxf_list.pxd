
cdef extern from "mxf/mxf.h":
    
    ctypedef void *free_func_type(void *data)
    ctypedef void *eq_func_type(void* data, void* info)
    
    ctypedef struct MXFListElement:
        pass
    
    ctypedef struct MXFList:
        MXFListElement* elements
        MXFListElement* lastElement
        long len
        free_func_type freeFunc
        
    ctypedef struct MXFListIterator:
        MXFListElement* nextElement
        void* data
        long index
        
    cdef int mxf_create_list(MXFList** list, free_func_type freeFunc)
    cdef void mxf_free_list(MXFList** list)
    cdef void mxf_initialise_list(MXFList* list, void *)
    cdef void mxf_clear_list(MXFList* list)
    
    cdef int mxf_append_list_element(MXFList* list, void* data)
    cdef int mxf_prepend_list_element(MXFList* list, void* data)
    cdef int mxf_insert_list_element(MXFList* list, long index, int before, void* data)
    cdef long mxf_get_list_length(MXFList* list)
    
    cdef void* mxf_find_list_element(const MXFList* list, void* info, eq_func_type eqFunc)
    cdef void* mxf_remove_list_element(MXFList* list, void* info, eq_func_type eqFunc)
    
    cdef void* mxf_get_list_element(MXFList* list, long index)
    
    cdef void* mxf_get_first_list_element(MXFList* list)
    cdef void* mxf_get_last_list_element(MXFList* list)
    
    cdef void mxf_initialise_list_iter(MXFListIterator* iter, const MXFList* list)
    cdef void mxf_initialise_list_iter_at(MXFListIterator* iter, const MXFList* list, long index)
    cdef int mxf_next_list_iter_element(MXFListIterator* iter)
    cdef void* mxf_get_iter_element(MXFListIterator* iter)
    cdef long mxf_get_list_iter_index(MXFListIterator* iter)