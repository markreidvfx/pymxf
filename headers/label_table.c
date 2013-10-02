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
    
    //Multiple wrappings for interleaved essence
    ADD_ITEM_EC(MultipleWrappings);
    
    //AES3/BWF mapping
    ADD_ITEM_EC(BWFFrameWrapped);
    ADD_ITEM_EC(AES3FrameWrapped);
    ADD_ITEM_EC(AES3ClipWrapped);
    ADD_ITEM_EC(BWFCustomWrapped);
    ADD_ITEM_EC(AES3CustomWrapped);
    
    //IEC-DV and DV-based mappings
    ADD_ITEM_EC(IECDV_25_525_60_FrameWrapped);
    ADD_ITEM_EC(IECDV_25_525_60_ClipWrapped);
    ADD_ITEM_EC(IECDV_25_625_50_FrameWrapped);
    ADD_ITEM_EC(IECDV_25_625_50_ClipWrapped);
    ADD_ITEM_EC(DVBased_25_525_60_FrameWrapped);
    ADD_ITEM_EC(DVBased_25_525_60_ClipWrapped);
    ADD_ITEM_EC(DVBased_25_625_50_FrameWrapped);
    ADD_ITEM_EC(DVBased_25_625_50_ClipWrapped);
    ADD_ITEM_EC(DVBased_50_525_60_FrameWrapped);
    ADD_ITEM_EC(DVBased_50_525_60_ClipWrapped);
    ADD_ITEM_EC(DVBased_50_625_50_FrameWrapped);
    ADD_ITEM_EC(DVBased_50_625_50_ClipWrapped);
    ADD_ITEM_EC(DVBased_100_1080_60_I_FrameWrapped);
    ADD_ITEM_EC(DVBased_100_1080_60_I_ClipWrapped);
    ADD_ITEM_EC(DVBased_100_1080_50_I_FrameWrapped);
    ADD_ITEM_EC(DVBased_100_1080_50_I_ClipWrapped);
    ADD_ITEM_EC(DVBased_100_720_60_P_FrameWrapped);
    ADD_ITEM_EC(DVBased_100_720_60_P_ClipWrapped);
    ADD_ITEM_EC(DVBased_100_720_50_P_FrameWrapped);
    ADD_ITEM_EC(DVBased_100_720_50_P_ClipWrapped);
    
    //Uncompressed mappings

    //SD
    ADD_ITEM_EC(SD_Unc_525_5994i_422_135_FrameWrapped);
    ADD_ITEM_EC(SD_Unc_525_5994i_422_135_ClipWrapped);
    ADD_ITEM_EC(SD_Unc_625_50i_422_135_FrameWrapped);
    ADD_ITEM_EC(SD_Unc_625_50i_422_135_ClipWrapped);
    
    //HD 1080
    ADD_ITEM_EC(HD_Unc_1080_25p_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_1080_25p_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_1080_50i_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_1080_50i_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_1080_2997p_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_1080_2997p_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_1080_5994i_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_1080_5994i_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_1080_30p_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_1080_30p_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_1080_60i_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_1080_60i_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_1080_50p_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_1080_50p_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_1080_5994p_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_1080_5994p_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_1080_60p_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_1080_60p_422_ClipWrapped);
    
    /* HD 720 */
    ADD_ITEM_EC(HD_Unc_720_25p_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_720_25p_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_720_2997p_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_720_2997p_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_720_30p_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_720_30p_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_720_50p_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_720_50p_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_720_5994p_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_720_5994p_422_ClipWrapped);
    ADD_ITEM_EC(HD_Unc_720_60p_422_FrameWrapped);
    ADD_ITEM_EC(HD_Unc_720_60p_422_ClipWrapped);
    
    /* D-10 mapping */
    ADD_ITEM_EC(D10_50_625_50_defined_template);
    ADD_ITEM_EC(D10_50_625_50_extended_template);
    ADD_ITEM_EC(D10_50_625_50_picture_only);
    ADD_ITEM_EC(D10_50_525_60_defined_template);
    ADD_ITEM_EC(D10_50_525_60_extended_template);
    ADD_ITEM_EC(D10_50_525_60_picture_only);
    ADD_ITEM_EC(D10_40_625_50_defined_template);
    ADD_ITEM_EC(D10_40_625_50_extended_template);
    ADD_ITEM_EC(D10_40_625_50_picture_only);
    ADD_ITEM_EC(D10_40_525_60_defined_template);
    ADD_ITEM_EC(D10_40_525_60_extended_template);
    ADD_ITEM_EC(D10_40_525_60_picture_only);
    ADD_ITEM_EC(D10_30_625_50_defined_template);
    ADD_ITEM_EC(D10_30_625_50_extended_template);
    ADD_ITEM_EC(D10_30_625_50_picture_only);
    ADD_ITEM_EC(D10_30_525_60_defined_template);
    ADD_ITEM_EC(D10_30_525_60_extended_template);
    ADD_ITEM_EC(D10_30_525_60_picture_only);
    
    /* A-law mapping */
    ADD_ITEM_EC(ALawFrameWrapped);
    ADD_ITEM_EC(ALawClipWrapped);
    ADD_ITEM_EC(ALawCustomWrapped);
    
    /* AVC Intra-Frame Coding */
    ADD_ITEM_EC(AVCIFrameWrapped);
    ADD_ITEM_EC(AVCIClipWrapped);
    
    /* MPEG ES VideoStream-0 SID */
    ADD_ITEM_EC(MPEGES0FrameWrapped);
    ADD_ITEM_EC(MPEGES0ClipWrapped);
    
    /* VC-3 */
    ADD_ITEM_EC(VC3FrameWrapped);
    ADD_ITEM_EC(VC3ClipWrapped);
    
    // Avid Labels
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
    
    /* P2 AVC Intra-Frame Coding - version byte is incorrectly set to 0x01 */
    ADD_ITEM_EC(P2AVCIFrameWrapped);
    ADD_ITEM_EC(P2AVCIClipWrapped);
    
    *label_list = newlist;
    return 1;
    
fail:
    
    mxf_clear_list(&newlist);
    return 0;
    
}

#define ADD_ITEM_CMDEF(name) \
CHK_OFAIL(register_label_table_item(newlist, #name, &MXF_CMDEF_L(name)));


int load_label_table_essence_coding_labels(MXFList** label_list)
{
    MXFList* newlist;
    
    mxf_create_list(&newlist, free_label_table_item_in_list);
    
    //IEC-DV and DV-based mappings
    ADD_ITEM_CMDEF(IECDV_25_525_60);
    ADD_ITEM_CMDEF(IECDV_25_625_50);
    ADD_ITEM_CMDEF(DVBased_25_525_60);
    ADD_ITEM_CMDEF(DVBased_25_625_50);
    ADD_ITEM_CMDEF(DVBased_50_525_60);
    ADD_ITEM_CMDEF(DVBased_50_625_50);
    ADD_ITEM_CMDEF(DVBased_100_1080_60_I);
    ADD_ITEM_CMDEF(DVBased_100_1080_50_I);
    ADD_ITEM_CMDEF(DVBased_100_720_60_P);
    ADD_ITEM_CMDEF(DVBased_100_720_50_P);
    
    /* D-10 mappings */
    ADD_ITEM_CMDEF(D10_50_625_50);
    ADD_ITEM_CMDEF(D10_50_525_60);
    ADD_ITEM_CMDEF(D10_40_625_50);
    ADD_ITEM_CMDEF(D10_40_525_60);
    ADD_ITEM_CMDEF(D10_30_625_50);
    ADD_ITEM_CMDEF(D10_30_525_60);
    
    /* A-law audio mapping */
    ADD_ITEM_CMDEF(ALaw);
    
    /* MPEG mappings */
    ADD_ITEM_CMDEF(MP4AdvancedRealTimeSimpleL1);
    ADD_ITEM_CMDEF(MP4AdvancedRealTimeSimpleL2);
    ADD_ITEM_CMDEF(MP4AdvancedRealTimeSimpleL3);
    ADD_ITEM_CMDEF(MP4AdvancedRealTimeSimpleL4);
    
    /* AVC Intra-Frame Coding */
    ADD_ITEM_CMDEF(AVCI_50_1080_60_I);
    ADD_ITEM_CMDEF(AVCI_50_1080_50_I);
    ADD_ITEM_CMDEF(AVCI_50_1080_30_P);
    ADD_ITEM_CMDEF(AVCI_50_1080_25_P);
    ADD_ITEM_CMDEF(AVCI_50_720_60_P);
    ADD_ITEM_CMDEF(AVCI_50_720_50_P);
    ADD_ITEM_CMDEF(AVCI_100_1080_60_I);
    ADD_ITEM_CMDEF(AVCI_100_1080_50_I);
    ADD_ITEM_CMDEF(AVCI_100_1080_30_P);
    ADD_ITEM_CMDEF(AVCI_100_1080_25_P);
    ADD_ITEM_CMDEF(AVCI_100_720_60_P);
    ADD_ITEM_CMDEF(AVCI_100_720_50_P);
    
    /* MPEG-2 Long GOP */
    ADD_ITEM_CMDEF(MPEG2_MP_HL_LONGGOP);
    ADD_ITEM_CMDEF(MPEG2_422P_HL_LONGGOP);
    ADD_ITEM_CMDEF(MPEG2_MP_H14_LONGGOP);
    
    /* DNxHD / VC-3 */
    
    ADD_ITEM_CMDEF(DNxHD);
    ADD_ITEM_CMDEF(VC3_1080P_1235);
    ADD_ITEM_CMDEF(VC3_1080P_1237);
    ADD_ITEM_CMDEF(VC3_1080P_1238);
    ADD_ITEM_CMDEF(VC3_1080I_1241);
    ADD_ITEM_CMDEF(VC3_1080I_1242);
    ADD_ITEM_CMDEF(VC3_1080I_1243);
    ADD_ITEM_CMDEF(VC3_720P_1250);
    ADD_ITEM_CMDEF(VC3_720P_1251);
    ADD_ITEM_CMDEF(VC3_720P_1252);
    ADD_ITEM_CMDEF(VC3_1080P_1253);
    
    /* Uncompressed */
    ADD_ITEM_CMDEF(UNC_8B_422_INTERLEAVED);
    ADD_ITEM_CMDEF(UNC_10B_422_INTERLEAVED);
    
    // Avid Labels
    
    ADD_ITEM_CMDEF(AvidMJPEG1110B_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG21_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG21_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG31_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG31_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG101_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG101_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG201_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG201_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG41m_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG41m_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG101m_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG101m_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG21s_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG21s_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG41s_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG41s_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG151s_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG151s_NTSC);
    
    ADD_ITEM_CMDEF(AvidMJPEG351p_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG351p_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG281p_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG281p_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG141p_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG141p_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG31p_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG31p_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG21p_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG21p_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG31m_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG31m_NTSC);
    ADD_ITEM_CMDEF(AvidMJPEG81m_PAL);
    ADD_ITEM_CMDEF(AvidMJPEG81m_NTSC);

    *label_list = newlist;
    return 1;
    
fail:
    
    mxf_clear_list(&newlist);
    return 0;
    
}
    
    

#define ADD_ITEM_EE(name) \
CHK_OFAIL(register_label_table_item(newlist, #name, &MXF_EE_K(name)));


int load_label_table_essence_element_keys(MXFList** label_list)
{
    MXFList* newlist;
    
    mxf_create_list(&newlist, free_label_table_item_in_list);
    
    ADD_ITEM_EE(SDTI_CP_System_Pack);
    
    ADD_ITEM_EE(AvidMJPEGClipWrapped);
    ADD_ITEM_EE(AvidMPEGClipWrapped);
    ADD_ITEM_EE(DNxHD);
    ADD_ITEM_EE(DVClipWrapped);
    ADD_ITEM_EE(BWFClipWrapped);
    ADD_ITEM_EE(AES3ClipWrapped);
    ADD_ITEM_EE(UncClipWrapped);
    ADD_ITEM_EE(IMX);
    ADD_ITEM_EE(AvidUnc10BitClipWrapped);
    //ADD_ITEM_EE(AvidUncRGBA);
    
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


