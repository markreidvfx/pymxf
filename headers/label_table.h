
#include <mxf/mxf.h>
#include <mxf/mxf_avid_labels_and_keys.h>
#include <mxf/mxf_p2_labels_and_keys.h>

#ifdef __cplusplus
extern "C"
{
#endif


    
typedef struct
{
    char* name;
    mxfKey *key;
}LabelTableItem;
    
int load_label_table_essence_containers(MXFList** label_list);
    

#ifdef __cplusplus
}
#endif