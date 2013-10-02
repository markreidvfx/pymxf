
#include <mxf/mxf.h>
#include <mxf/mxf_avid_labels_and_keys.h>
//#include <mxf/mxf_p2_labels_and_keys.h>

#ifdef __cplusplus
extern "C"
{
#endif


    
typedef struct
{
    char* name;
    const mxfKey *key;
}LabelTableItem;
    
int load_label_table_essence_containers(MXFList** label_list);
int load_label_table_essence_coding_labels(MXFList** label_list);
int load_label_table_essence_element_keys(MXFList** label_list);
    

#ifdef __cplusplus
}
#endif