

cdef extern from "label_table.h":

    ctypedef struct LabelTableItem:
        char* name
        mxfKey *key

    cdef int load_label_table_essence_containers(MXFList** label_list)