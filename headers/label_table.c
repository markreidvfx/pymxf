#include <assert.h>
#include <string.h>
#include <label_table.h>




int test(void)
{
    return 10;
    
}

static void free_label_table_item(LabelTableItem** labelItem)
{
    if (labelItem == NULL)
    {
        return;
    }
    SAFE_FREE(&(*labelItem)->name);
    SAFE_FREE(labelItem);
}

static void free_label_table_item_in_list(void *data)
{
    LabelTableItem* labelItem;
    
    if (data == NULL)
    {
        return;
    }
    labelItem = (LabelTableItem*)data;
    free_label_table_item(&labelItem);
    
}

static int add_label_table_item(MXFList* label_list, LabelTableItem* labelItem)
{
    assert(labelItem != NULL);
    CHK_ORET(mxf_append_list_element(label_list, (void*)labelItem));
    
    return 1;
    
}


#define ADD_ITEM_EC(name) \
CHK_OFAIL(register_label_table_item(newlist, #name, &MXF_EC_L(name)));


int load_label_table_essence_containers(MXFList** label_list)
{
    MXFList* newlist;

    mxf_create_list(&newlist, free_label_table_item_in_list);
    
    ADD_ITEM_EC(AvidDV25ClipWrappedEssenceContainer);
    ADD_ITEM_EC(AvidMJPEGClipWrapped);
    ADD_ITEM_EC(AvidIMX50_625_50);
    ADD_ITEM_EC(AvidIMX50_525_60);
    ADD_ITEM_EC(AvidIMX40_625_50);
    ADD_ITEM_EC(AvidIMX40_525_60);
    ADD_ITEM_EC(AvidIMX30_625_50);
    ADD_ITEM_EC(AvidIMX30_525_60);
    ADD_ITEM_EC(AvidMPEG4);
    ADD_ITEM_EC(AvidMPEGClipWrapped);
    ADD_ITEM_EC(DNxHD720p120ClipWrapped);
    ADD_ITEM_EC(DNxHD720p185ClipWrapped);
    ADD_ITEM_EC(DNxHD1080p185XClipWrapped);
    ADD_ITEM_EC(DNxHD1080p120ClipWrapped);
    ADD_ITEM_EC(DNxHD1080p185ClipWrapped);
    ADD_ITEM_EC(DNxHD1080p36ClipWrapped);
    ADD_ITEM_EC(DNxHD1080i185XClipWrapped);
    ADD_ITEM_EC(DNxHD1080i120ClipWrapped);
    ADD_ITEM_EC(DNxHD1080i185ClipWrapped);
    
    *label_list = newlist;
    return 1;
    
fail:
    
    mxf_clear_list(&newlist);
    return 0;
    
}

int register_label_table_item(MXFList* label_list, char* name, mxfKey* label)
{
    
    LabelTableItem* newItem = NULL;
    CHK_MALLOC_ORET(newItem, LabelTableItem);
    memset(newItem, 0, sizeof(LabelTableItem));
    
    if (name!=NULL)
    {
        CHK_MALLOC_ARRAY_OFAIL(newItem->name, char, strlen(name) + 1);
        strcpy(newItem->name, name);
        
    }
    newItem->key = label;
    CHK_OFAIL(add_label_table_item(label_list, newItem));
    
    return 1;
fail:
    free_label_table_item(&newItem);
    return 0;
}


