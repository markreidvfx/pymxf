cdef extern from "mxf/mxf.h":
    ctypedef struct MXFTreeNode:
        pass
    ctypedef struct MXFTree:
        pass
        
    cdef int mxf_tree_traverse(MXFTree*, void* process_f, void* process_data)