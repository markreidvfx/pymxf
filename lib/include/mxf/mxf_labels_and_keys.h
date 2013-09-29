/*
 * $Id: mxf_labels_and_keys.h,v 1.19 2011/11/10 10:22:28 philipn Exp $
 *
 * MXF labels, keys, track numbers, etc
 *
 * Copyright (C) 2006  Philip de Nier <philipn@users.sourceforge.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __MXF_LABELS_AND_KEYS_H__
#define __MXF_LABELS_AND_KEYS_H__



#ifdef __cplusplus
extern "C"
{
#endif



/*
 *
 * Operational patterns labels
 *
 */

#define MXF_OP_L(def, name) g_##name##_op_##def##_label

#define MXF_OP_L_LABEL(regver, complexity, byte14, qualifier) \
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, regver, 0x0d, 0x01, 0x02, 0x01, complexity, byte14, qualifier, 0x00}


/* OP-Atom labels */

#define MXF_ATOM_OP_L(byte14) \
    MXF_OP_L_LABEL(0x02, 0x10, byte14, 0x00)


static const mxfUL MXF_OP_L(atom, 1Track_1SourceClip) =
    MXF_ATOM_OP_L(0x00);

static const mxfUL MXF_OP_L(atom, 1Track_NSourceClips) =
    MXF_ATOM_OP_L(0x01);

static const mxfUL MXF_OP_L(atom, NTracks_1SourceClip) =
    MXF_ATOM_OP_L(0x02);

static const mxfUL MXF_OP_L(atom, NTracks_NSourceClips) =
    MXF_ATOM_OP_L(0x03);


/* OP-1A labels */

#define MXF_1A_OP_L(qualifier) \
    MXF_OP_L_LABEL(0x01, 0x01, 0x01, qualifier)


static const mxfUL MXF_OP_L(1a, UniTrack_Stream_Internal) =
    MXF_1A_OP_L(0x01);

static const mxfUL MXF_OP_L(1a, MultiTrack_Stream_Internal) =
    MXF_1A_OP_L(0x09);

static const mxfUL MXF_OP_L(1a, MultiTrack_Stream_External) =
    MXF_1A_OP_L(0x0b);


/* OP-1B labels */

#define MXF_1B_OP_L(qualifier) \
    MXF_OP_L_LABEL(0x01, 0x01, 0x02, qualifier)


static const mxfUL MXF_OP_L(1b, MultiTrack_Stream_External) =
    MXF_1B_OP_L(0x0b);


int is_op_atom(const mxfUL *label);
int is_op_1a(const mxfUL *label);
int is_op_1b(const mxfUL *label);



/*
 *
 * Data definition labels
 *
 */


#define MXF_DDEF_L(name)    g_##name##_datadef_label


static const mxfUL MXF_DDEF_L(Picture) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x01, 0x03, 0x02, 0x02, 0x01, 0x00, 0x00, 0x00};

static const mxfUL MXF_DDEF_L(Sound) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x01, 0x03, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00};

static const mxfUL MXF_DDEF_L(Timecode) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x01, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00};

static const mxfUL MXF_DDEF_L(LegacyPicture) =
    {0x80, 0x7d, 0x00, 0x60, 0x08, 0x14, 0x3e, 0x6f, 0x6f, 0x3c, 0x8c, 0xe1, 0x6c, 0xef, 0x11, 0xd2};

static const mxfUL MXF_DDEF_L(LegacySound) =
    {0x80, 0x7d, 0x00, 0x60, 0x08, 0x14, 0x3e, 0x6f, 0x78, 0xe1, 0xeb, 0xe1, 0x6c, 0xef, 0x11, 0xd2};

static const mxfUL MXF_DDEF_L(LegacyTimecode) =
    {0x80, 0x7f, 0x00, 0x60, 0x08, 0x14, 0x3e, 0x6f, 0x7f, 0x27, 0x5e, 0x81, 0x77, 0xe5, 0x11, 0xd2};

static const mxfUL MXF_DDEF_L(Data) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x01, 0x03, 0x02, 0x02, 0x03, 0x00, 0x00, 0x00};

static const mxfUL MXF_DDEF_L(DescriptiveMetadata) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x01, 0x03, 0x02, 0x01, 0x10, 0x00, 0x00, 0x00};


int mxf_is_picture(const mxfUL *label);
int mxf_is_sound(const mxfUL *label);
int mxf_is_timecode(const mxfUL *label);
int mxf_is_data(const mxfUL *label);
int mxf_is_descriptive_metadata(const mxfUL *label);



/*
 *
 * Picture/Sound coding labels
 *
 */


#define MXF_CMDEF_L(name)   g_##name##_compdef_label


/* IEC-DV and DV-based mappings */

#define MXF_DV_CMDEV_L(regver, byte14, byte15) \
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, regver, 0x04, 0x01, 0x02, 0x02, 0x02, byte14, byte15, 0x00}


static const mxfUL MXF_CMDEF_L(IECDV_25_525_60) =
    MXF_DV_CMDEV_L(0x01, 0x01, 0x01);

static const mxfUL MXF_CMDEF_L(IECDV_25_625_50) =
    MXF_DV_CMDEV_L(0x01, 0x01, 0x02);

static const mxfUL MXF_CMDEF_L(DVBased_25_525_60) =
    MXF_DV_CMDEV_L(0x01, 0x02, 0x01);

static const mxfUL MXF_CMDEF_L(DVBased_25_625_50) =
    MXF_DV_CMDEV_L(0x01, 0x02, 0x02);

static const mxfUL MXF_CMDEF_L(DVBased_50_525_60) =
    MXF_DV_CMDEV_L(0x01, 0x02, 0x03);

static const mxfUL MXF_CMDEF_L(DVBased_50_625_50) =
    MXF_DV_CMDEV_L(0x01, 0x02, 0x04);

static const mxfUL MXF_CMDEF_L(DVBased_100_1080_60_I) =
    MXF_DV_CMDEV_L(0x01, 0x02, 0x05);

static const mxfUL MXF_CMDEF_L(DVBased_100_1080_50_I) =
    MXF_DV_CMDEV_L(0x01, 0x02, 0x06);

static const mxfUL MXF_CMDEF_L(DVBased_100_720_60_P) =
    MXF_DV_CMDEV_L(0x01, 0x02, 0x07);

static const mxfUL MXF_CMDEF_L(DVBased_100_720_50_P) =
    MXF_DV_CMDEV_L(0x01, 0x02, 0x08);


/* D-10 mappings */

#define MXF_D10_CMDEV_L(regver, byte16) \
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, regver, 0x04, 0x01, 0x02, 0x02, 0x01, 0x02, 0x01, byte16}


static const mxfUL MXF_CMDEF_L(D10_50_625_50) =
    MXF_D10_CMDEV_L(0x01, 0x01);

static const mxfUL MXF_CMDEF_L(D10_50_525_60) =
    MXF_D10_CMDEV_L(0x01, 0x02);

static const mxfUL MXF_CMDEF_L(D10_40_625_50) =
    MXF_D10_CMDEV_L(0x01, 0x03);

static const mxfUL MXF_CMDEF_L(D10_40_525_60) =
    MXF_D10_CMDEV_L(0x01, 0x04);

static const mxfUL MXF_CMDEF_L(D10_30_625_50) =
    MXF_D10_CMDEV_L(0x01, 0x05);

static const mxfUL MXF_CMDEF_L(D10_30_525_60) =
    MXF_D10_CMDEV_L(0x01, 0x06);


/* A-law audio mapping */

static const mxfUL MXF_CMDEF_L(ALaw) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x03 , 0x04, 0x02, 0x02, 0x02, 0x03, 0x01, 0x01, 0x00};


/* MPEG mappings */

#define MXF_MPEG4_CMDEV_L(profile, variant) \
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x03, 0x04, 0x01, 0x02, 0x02, 0x01, 0x20, profile, variant}

static const mxfUL MXF_CMDEF_L(MP4AdvancedRealTimeSimpleL1) =
    MXF_MPEG4_CMDEV_L(0x02, 0x01);

static const mxfUL MXF_CMDEF_L(MP4AdvancedRealTimeSimpleL2) =
    MXF_MPEG4_CMDEV_L(0x02, 0x02);

static const mxfUL MXF_CMDEF_L(MP4AdvancedRealTimeSimpleL3) =
    MXF_MPEG4_CMDEV_L(0x02, 0x03);

static const mxfUL MXF_CMDEF_L(MP4AdvancedRealTimeSimpleL4) =
    MXF_MPEG4_CMDEV_L(0x02, 0x04);


/* AVC Intra-Frame Coding */

#define MXF_AVCI_CMDEV_L(profile, variant) \
    {0x06, 0x0E, 0x2B, 0x34, 0x04, 0x01, 0x01, 0x0A, 0x04, 0x01, 0x02, 0x02, 0x01, 0x32, profile, variant}

static const mxfUL MXF_CMDEF_L(AVCI_50_1080_60_I) =
    MXF_AVCI_CMDEV_L(0x21, 0x01);

static const mxfUL MXF_CMDEF_L(AVCI_50_1080_50_I) =
    MXF_AVCI_CMDEV_L(0x21, 0x02);

static const mxfUL MXF_CMDEF_L(AVCI_50_1080_30_P) =
    MXF_AVCI_CMDEV_L(0x21, 0x03);

static const mxfUL MXF_CMDEF_L(AVCI_50_1080_25_P) =
    MXF_AVCI_CMDEV_L(0x21, 0x04);

static const mxfUL MXF_CMDEF_L(AVCI_50_720_60_P) =
    MXF_AVCI_CMDEV_L(0x21, 0x08);

static const mxfUL MXF_CMDEF_L(AVCI_50_720_50_P) =
    MXF_AVCI_CMDEV_L(0x21, 0x09);

static const mxfUL MXF_CMDEF_L(AVCI_100_1080_60_I) =
    MXF_AVCI_CMDEV_L(0x31, 0x01);

static const mxfUL MXF_CMDEF_L(AVCI_100_1080_50_I) =
    MXF_AVCI_CMDEV_L(0x31, 0x02);

static const mxfUL MXF_CMDEF_L(AVCI_100_1080_30_P) =
    MXF_AVCI_CMDEV_L(0x31, 0x03);

static const mxfUL MXF_CMDEF_L(AVCI_100_1080_25_P) =
    MXF_AVCI_CMDEV_L(0x31, 0x04);

static const mxfUL MXF_CMDEF_L(AVCI_100_720_60_P) =
    MXF_AVCI_CMDEV_L(0x31, 0x08);

static const mxfUL MXF_CMDEF_L(AVCI_100_720_50_P) =
    MXF_AVCI_CMDEV_L(0x31, 0x09);


/* MPEG-2 Long GOP */

static const mxfUL MXF_CMDEF_L(MPEG2_MP_HL_LONGGOP) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x03, 0x04, 0x01, 0x02, 0x02, 0x01, 0x03, 0x03, 0x00};

static const mxfUL MXF_CMDEF_L(MPEG2_422P_HL_LONGGOP) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x03, 0x04, 0x01, 0x02, 0x02, 0x01, 0x04, 0x03, 0x00};

static const mxfUL MXF_CMDEF_L(MPEG2_MP_H14_LONGGOP) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x08, 0x04, 0x01, 0x02, 0x02, 0x01, 0x05, 0x03, 0x00};


/* DNxHD / VC-3 */

static const mxfUL MXF_CMDEF_L(DNxHD) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x0e, 0x04, 0x02, 0x01, 0x02, 0x04, 0x01, 0x00};


#define MXF_VC3_CMDEV_L(variant) \
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x0a, 0x04, 0x01, 0x02, 0x02, 0x71, variant, 0x00, 0x00}

static const mxfUL MXF_CMDEF_L(VC3_1080P_1235) =
    MXF_VC3_CMDEV_L(0x01);
static const mxfUL MXF_CMDEF_L(VC3_1080P_1237) =
    MXF_VC3_CMDEV_L(0x03);
static const mxfUL MXF_CMDEF_L(VC3_1080P_1238) =
    MXF_VC3_CMDEV_L(0x04);
static const mxfUL MXF_CMDEF_L(VC3_1080I_1241) =
    MXF_VC3_CMDEV_L(0x07);
static const mxfUL MXF_CMDEF_L(VC3_1080I_1242) =
    MXF_VC3_CMDEV_L(0x08);
static const mxfUL MXF_CMDEF_L(VC3_1080I_1243) =
    MXF_VC3_CMDEV_L(0x09);
static const mxfUL MXF_CMDEF_L(VC3_720P_1250) =
    MXF_VC3_CMDEV_L(0x10);
static const mxfUL MXF_CMDEF_L(VC3_720P_1251) =
    MXF_VC3_CMDEV_L(0x11);
static const mxfUL MXF_CMDEF_L(VC3_720P_1252) =
    MXF_VC3_CMDEV_L(0x12);
static const mxfUL MXF_CMDEF_L(VC3_1080P_1253) =
    MXF_VC3_CMDEV_L(0x13);


/* Uncompressed */

/* fourcc 2vuy */
static const mxfUL MXF_CMDEF_L(UNC_8B_422_INTERLEAVED) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x0a, 0x04, 0x01, 0x02, 0x01, 0x01, 0x02, 0x01, 0x01};

/* fourcc v210 */
static const mxfUL MXF_CMDEF_L(UNC_10B_422_INTERLEAVED) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x0a, 0x04, 0x01, 0x02, 0x01, 0x01, 0x02, 0x02, 0x01};



/*
 *
 * Essence container labels
 *
 */

#define MXF_EC_L(name)  g_##name##_esscont_label


#define MXF_GENERIC_CONTAINER_LABEL(regver, eckind, mappingkind, byte15, byte16) \
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, regver, 0x0d, 0x01, 0x03, 0x01, eckind, mappingkind, byte15, byte16}


int mxf_is_generic_container_label(const mxfUL *label);



/* Multiple wrappings for interleaved essence */

static const mxfUL MXF_EC_L(MultipleWrappings) =
    MXF_GENERIC_CONTAINER_LABEL(0x03, 0x02, 0x7f, 0x01, 0x00);


/* AES3/BWF mappings */

#define MXF_AES3BWF_EC_L(regver, byte15) \
    MXF_GENERIC_CONTAINER_LABEL(regver, 0x02, 0x06, byte15, 0x00)


static const mxfUL MXF_EC_L(BWFFrameWrapped) =
    MXF_AES3BWF_EC_L(0x01, 0x01);

static const mxfUL MXF_EC_L(BWFClipWrapped) =
    MXF_AES3BWF_EC_L(0x01, 0x02);

static const mxfUL MXF_EC_L(AES3FrameWrapped) =
    MXF_AES3BWF_EC_L(0x01, 0x03);

static const mxfUL MXF_EC_L(AES3ClipWrapped) =
    MXF_AES3BWF_EC_L(0x01, 0x04);

static const mxfUL MXF_EC_L(BWFCustomWrapped) =
    MXF_AES3BWF_EC_L(0x05, 0x08);

static const mxfUL MXF_EC_L(AES3CustomWrapped) =
    MXF_AES3BWF_EC_L(0x05, 0x09);


/* IEC-DV and DV-based mappings */

#define MXF_DV_EC_L(regver, byte15, byte16) \
    MXF_GENERIC_CONTAINER_LABEL(regver, 0x02, 0x02, byte15, byte16)


static const mxfUL MXF_EC_L(IECDV_25_525_60_FrameWrapped) =
    MXF_DV_EC_L(0x01, 0x01, 0x01);

static const mxfUL MXF_EC_L(IECDV_25_525_60_ClipWrapped) =
    MXF_DV_EC_L(0x01, 0x01, 0x02);

static const mxfUL MXF_EC_L(IECDV_25_625_50_FrameWrapped) =
    MXF_DV_EC_L(0x01, 0x02, 0x01);

static const mxfUL MXF_EC_L(IECDV_25_625_50_ClipWrapped) =
    MXF_DV_EC_L(0x01, 0x02, 0x02);

static const mxfUL MXF_EC_L(DVBased_25_525_60_FrameWrapped) =
    MXF_DV_EC_L(0x01, 0x40, 0x01);

static const mxfUL MXF_EC_L(DVBased_25_525_60_ClipWrapped) =
    MXF_DV_EC_L(0x01, 0x40, 0x02);

static const mxfUL MXF_EC_L(DVBased_25_625_50_FrameWrapped) =
    MXF_DV_EC_L(0x01, 0x41, 0x01);

static const mxfUL MXF_EC_L(DVBased_25_625_50_ClipWrapped) =
    MXF_DV_EC_L(0x01, 0x41, 0x02);

static const mxfUL MXF_EC_L(DVBased_50_525_60_FrameWrapped) =
    MXF_DV_EC_L(0x01, 0x50, 0x01);

static const mxfUL MXF_EC_L(DVBased_50_525_60_ClipWrapped) =
    MXF_DV_EC_L(0x01, 0x50, 0x02);

static const mxfUL MXF_EC_L(DVBased_50_625_50_FrameWrapped) =
    MXF_DV_EC_L(0x01, 0x51, 0x01);

static const mxfUL MXF_EC_L(DVBased_50_625_50_ClipWrapped) =
    MXF_DV_EC_L(0x01, 0x51, 0x02);

static const mxfUL MXF_EC_L(DVBased_100_1080_60_I_FrameWrapped) =
    MXF_DV_EC_L(0x01, 0x60, 0x01);

static const mxfUL MXF_EC_L(DVBased_100_1080_60_I_ClipWrapped) =
    MXF_DV_EC_L(0x01, 0x60, 0x02);

static const mxfUL MXF_EC_L(DVBased_100_1080_50_I_FrameWrapped) =
    MXF_DV_EC_L(0x01, 0x61, 0x01);

static const mxfUL MXF_EC_L(DVBased_100_1080_50_I_ClipWrapped) =
    MXF_DV_EC_L(0x01, 0x61, 0x02);

static const mxfUL MXF_EC_L(DVBased_100_720_60_P_FrameWrapped) =
    MXF_DV_EC_L(0x01, 0x62, 0x01);

static const mxfUL MXF_EC_L(DVBased_100_720_60_P_ClipWrapped) =
    MXF_DV_EC_L(0x01, 0x62, 0x02);

static const mxfUL MXF_EC_L(DVBased_100_720_50_P_FrameWrapped) =
    MXF_DV_EC_L(0x01, 0x63, 0x01);

static const mxfUL MXF_EC_L(DVBased_100_720_50_P_ClipWrapped) =
    MXF_DV_EC_L(0x01, 0x63, 0x02);


/* Uncompressed mappings */

#define MXF_UNC_EC_L(regver, byte15, byte16) \
    MXF_GENERIC_CONTAINER_LABEL(regver, 0x02, 0x05, byte15, byte16)

/* SD */

static const mxfUL MXF_EC_L(SD_Unc_525_5994i_422_135_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x01, 0x01);

static const mxfUL MXF_EC_L(SD_Unc_525_5994i_422_135_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x01, 0x02);

static const mxfUL MXF_EC_L(SD_Unc_625_50i_422_135_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x01, 0x05);

static const mxfUL MXF_EC_L(SD_Unc_625_50i_422_135_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x01, 0x06);

/* HD 1080 */

static const mxfUL MXF_EC_L(HD_Unc_1080_25p_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x21);

static const mxfUL MXF_EC_L(HD_Unc_1080_25p_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x22);

static const mxfUL MXF_EC_L(HD_Unc_1080_50i_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x29);

static const mxfUL MXF_EC_L(HD_Unc_1080_50i_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x2a);

static const mxfUL MXF_EC_L(HD_Unc_1080_2997p_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x31);

static const mxfUL MXF_EC_L(HD_Unc_1080_2997p_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x32);

static const mxfUL MXF_EC_L(HD_Unc_1080_5994i_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x39);

static const mxfUL MXF_EC_L(HD_Unc_1080_5994i_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x3a);

static const mxfUL MXF_EC_L(HD_Unc_1080_30p_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x41);

static const mxfUL MXF_EC_L(HD_Unc_1080_30p_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x42);

static const mxfUL MXF_EC_L(HD_Unc_1080_60i_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x49);

static const mxfUL MXF_EC_L(HD_Unc_1080_60i_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x4a);

static const mxfUL MXF_EC_L(HD_Unc_1080_50p_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x51);

static const mxfUL MXF_EC_L(HD_Unc_1080_50p_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x52);

static const mxfUL MXF_EC_L(HD_Unc_1080_5994p_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x59);

static const mxfUL MXF_EC_L(HD_Unc_1080_5994p_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x5a);

static const mxfUL MXF_EC_L(HD_Unc_1080_60p_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x61);

static const mxfUL MXF_EC_L(HD_Unc_1080_60p_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x02, 0x62);

/* HD 720 */

static const mxfUL MXF_EC_L(HD_Unc_720_25p_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x03, 0x09);

static const mxfUL MXF_EC_L(HD_Unc_720_25p_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x03, 0x0a);

static const mxfUL MXF_EC_L(HD_Unc_720_2997p_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x03, 0x11);

static const mxfUL MXF_EC_L(HD_Unc_720_2997p_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x03, 0x12);

static const mxfUL MXF_EC_L(HD_Unc_720_30p_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x03, 0x15);

static const mxfUL MXF_EC_L(HD_Unc_720_30p_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x03, 0x16);

static const mxfUL MXF_EC_L(HD_Unc_720_50p_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x03, 0x19);

static const mxfUL MXF_EC_L(HD_Unc_720_50p_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x03, 0x1a);

static const mxfUL MXF_EC_L(HD_Unc_720_5994p_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x03, 0x21);

static const mxfUL MXF_EC_L(HD_Unc_720_5994p_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x03, 0x22);

static const mxfUL MXF_EC_L(HD_Unc_720_60p_422_FrameWrapped) =
    MXF_UNC_EC_L(0x01, 0x03, 0x25);

static const mxfUL MXF_EC_L(HD_Unc_720_60p_422_ClipWrapped) =
    MXF_UNC_EC_L(0x01, 0x03, 0x26);


/* D-10 mapping */

#define MXF_D10_EC_L(regver, byte15, byte16) \
    MXF_GENERIC_CONTAINER_LABEL(regver, 0x02, 0x01, byte15, byte16)


static const mxfUL MXF_EC_L(D10_50_625_50_defined_template) =
    MXF_D10_EC_L(0x01, 0x01, 0x01);

static const mxfUL MXF_EC_L(D10_50_625_50_extended_template) =
    MXF_D10_EC_L(0x01, 0x01, 0x02);

static const mxfUL MXF_EC_L(D10_50_625_50_picture_only) =
    MXF_D10_EC_L(0x02, 0x01, 0x7f);

static const mxfUL MXF_EC_L(D10_50_525_60_defined_template) =
    MXF_D10_EC_L(0x01, 0x02, 0x01);

static const mxfUL MXF_EC_L(D10_50_525_60_extended_template) =
    MXF_D10_EC_L(0x01, 0x02, 0x02);

static const mxfUL MXF_EC_L(D10_50_525_60_picture_only) =
    MXF_D10_EC_L(0x02, 0x02, 0x7f);

static const mxfUL MXF_EC_L(D10_40_625_50_defined_template) =
    MXF_D10_EC_L(0x01, 0x03, 0x01);

static const mxfUL MXF_EC_L(D10_40_625_50_extended_template) =
    MXF_D10_EC_L(0x01, 0x03, 0x02);

static const mxfUL MXF_EC_L(D10_40_625_50_picture_only) =
    MXF_D10_EC_L(0x02, 0x03, 0x7f);

static const mxfUL MXF_EC_L(D10_40_525_60_defined_template) =
    MXF_D10_EC_L(0x01, 0x04, 0x01);

static const mxfUL MXF_EC_L(D10_40_525_60_extended_template) =
    MXF_D10_EC_L(0x01, 0x04, 0x02);

static const mxfUL MXF_EC_L(D10_40_525_60_picture_only) =
    MXF_D10_EC_L(0x02, 0x04, 0x7f);

static const mxfUL MXF_EC_L(D10_30_625_50_defined_template) =
    MXF_D10_EC_L(0x01, 0x05, 0x01);

static const mxfUL MXF_EC_L(D10_30_625_50_extended_template) =
    MXF_D10_EC_L(0x01, 0x05, 0x02);

static const mxfUL MXF_EC_L(D10_30_625_50_picture_only) =
    MXF_D10_EC_L(0x02, 0x05, 0x7f);

static const mxfUL MXF_EC_L(D10_30_525_60_defined_template) =
    MXF_D10_EC_L(0x01, 0x06, 0x01);

static const mxfUL MXF_EC_L(D10_30_525_60_extended_template) =
    MXF_D10_EC_L(0x01, 0x06, 0x02);

static const mxfUL MXF_EC_L(D10_30_525_60_picture_only) =
    MXF_D10_EC_L(0x02, 0x06, 0x7f);


/* A-law mapping */

#define MXF_ALAW_EC_L(byte15) \
    MXF_GENERIC_CONTAINER_LABEL(0x03, 0x02, 0x0A, byte15, 0x00)


static const mxfUL MXF_EC_L(ALawFrameWrapped) =
    MXF_ALAW_EC_L(0x01);

static const mxfUL MXF_EC_L(ALawClipWrapped) =
    MXF_ALAW_EC_L(0x02);

static const mxfUL MXF_EC_L(ALawCustomWrapped) =
    MXF_ALAW_EC_L(0x03);


/* MPEG mapping */

#define MXF_MPEG_EC_L(regver, eckind, byte15, byte16) \
    MXF_GENERIC_CONTAINER_LABEL(regver, 0x02, eckind, byte15, byte16)


/* AVC Intra-Frame Coding */

static const mxfUL MXF_EC_L(AVCIFrameWrapped) =
    MXF_MPEG_EC_L(0x0A, 0x10, 0x60, 0x01);

static const mxfUL MXF_EC_L(AVCIClipWrapped) =
    MXF_MPEG_EC_L(0x0A, 0x10, 0x60, 0x02);


/* MPEG ES VideoStream-0 SID */

static const mxfUL MXF_EC_L(MPEGES0FrameWrapped) =
    MXF_MPEG_EC_L(0x02, 0x04, 0x60, 0x01);

static const mxfUL MXF_EC_L(MPEGES0ClipWrapped) =
    MXF_MPEG_EC_L(0x02, 0x04, 0x60, 0x02);


/* VC-3 */

#define MXF_VC3_EC_L(byte15) \
    MXF_GENERIC_CONTAINER_LABEL(0x0a, 0x02, 0x11, byte15, 0x00)

static const mxfUL MXF_EC_L(VC3FrameWrapped) =
    MXF_VC3_EC_L(0x01);

static const mxfUL MXF_EC_L(VC3ClipWrapped) =
    MXF_VC3_EC_L(0x02);



/*
 *
 * Descriptive metadata schemes labels
 *
 */


#define MXF_DM_L(name)  g_##name##_dmscheme_label

/* legacy/experimental DMS-1 label with version == 0x01 */
static const mxfUL MXF_DM_L(LegacyDMS1) =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x01, 0x01, 0x01, 0x00};

#define MXF_DMS1_L(framework, variant) \
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x01, 0x01, framework, variant};

static const mxfUL MXF_DM_L(DMS1_Production_NoExtensions) =
    MXF_DMS1_L(0x01, 0x01);

static const mxfUL MXF_DM_L(DMS1_Production_Extensions) =
    MXF_DMS1_L(0x01, 0x02);

static const mxfUL MXF_DM_L(DMS1_Clip_NoExtensions) =
    MXF_DMS1_L(0x02, 0x01);

static const mxfUL MXF_DM_L(DMS1_Clip_Extensions) =
    MXF_DMS1_L(0x02, 0x02);

static const mxfUL MXF_DM_L(DMS1_Scene_NoExtensions) =
    MXF_DMS1_L(0x03, 0x01);

static const mxfUL MXF_DM_L(DMS1_Scene_Extensions) =
    MXF_DMS1_L(0x03, 0x02);



/*
 *
 * Miscellaneous labels
 *
 */


/* Transfer characteristic labels (Gamma) */

static const mxfUL ITUR_BT470_TRANSFER_CH =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x04, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00};

static const mxfUL ITUR_BT709_TRANSFER_CH =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x04, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00};


/* Coding equation labels */

static const mxfUL ITUR_BT601_CODING_EQ =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x04, 0x01, 0x01, 0x01, 0x02, 0x01, 0x00, 0x00};

static const mxfUL ITUR_BT709_CODING_EQ =
    {0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x04, 0x01, 0x01, 0x01, 0x02, 0x02, 0x00, 0x00};



/*
 *
 * Essence element keys
 *
 */

#define MXF_EE_K(name)  g_##name##_esselement_key



#define MXF_GENERIC_CONTAINER_ELEMENT_KEY(regver, itemtype, elecount, eletype, elenum) \
    {0x06, 0x0e, 0x2b, 0x34, 0x01, 0x02, 0x01, regver, 0x0d, 0x01, 0x03, 0x01, itemtype, elecount, eletype, elenum}

#define MXF_TRACK_NUM(itemtype, elecount, eletype, elenum) \
    ((((uint32_t)itemtype) << 24) | \
    (((uint32_t)elecount) << 16) | \
    (((uint32_t)eletype) << 8) | \
    ((uint32_t)elenum))


void mxf_complete_essence_element_key(mxfKey* key, uint8_t count, uint8_t type, uint8_t num);
void mxf_complete_essence_element_track_num(uint32_t* trackNum, uint8_t count, uint8_t type, uint8_t num);


/* AES3/BWF mappings */

#define MXF_AES3BWF_EE_K(elecount, eletype, elenum) \
    MXF_GENERIC_CONTAINER_ELEMENT_KEY(0x01, 0x16, elecount, eletype, elenum)

#define MXF_AES3BWF_TRACK_NUM(elecount, eletype, elenum) \
    MXF_TRACK_NUM(0x16, elecount, eletype, elenum)

#define MXF_BWF_FRAME_WRAPPED_EE_TYPE       0x01
#define MXF_BWF_CLIP_WRAPPED_EE_TYPE        0x02
#define MXF_AES3_FRAME_WRAPPED_EE_TYPE      0x03
#define MXF_AES3_CLIP_WRAPPED_EE_TYPE       0x04
#define MXF_BWF_CUSTOM_WRAPPED_EE_TYPE      0x0b
#define MXF_AES3_CUSTOM_WRAPPED_EE_TYPE     0x0c


/* IEC-DV and DV-based mappings */

#define MXF_DV_EE_K(elecount, eletype, elenum) \
    MXF_GENERIC_CONTAINER_ELEMENT_KEY(0x01, 0x18, elecount, eletype, elenum)

#define MXF_DV_TRACK_NUM(elecount, eletype, elenum) \
    MXF_TRACK_NUM(0x18, elecount, eletype, elenum)

#define MXF_DV_FRAME_WRAPPED_EE_TYPE        0x01
#define MXF_DV_CLIP_WRAPPED_EE_TYPE         0x02


/* Uncompressed mappings */

#define MXF_UNC_EE_K(elecount, eletype, elenum) \
    MXF_GENERIC_CONTAINER_ELEMENT_KEY(0x01, 0x15, elecount, eletype, elenum)

#define MXF_UNC_TRACK_NUM(elecount, eletype, elenum) \
    MXF_TRACK_NUM(0x15, elecount, eletype, elenum)

#define MXF_UNC_FRAME_WRAPPED_EE_TYPE       0x02
#define MXF_UNC_CLIP_WRAPPED_EE_TYPE        0x03
#define MXF_UNC_LINE_WRAPPED_EE_TYPE        0x04



/* System items */

#define MXF_GC_SYSTEM_ITEM_ELEMENT_KEY(regver, itemtype, schemeid, eleid, elenum) \
    {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, regver , 0x0d, 0x01, 0x03, 0x01, itemtype, schemeid, eleid, elenum}

/* System Scheme 1 - GC compatible */

#define MXF_SS1_ELEMENT_KEY(eleid, elenum) \
    MXF_GC_SYSTEM_ITEM_ELEMENT_KEY(0x01, 0x14, 0x02, eleid, elenum)

/* SDTI-CP */
static const mxfKey MXF_EE_K(SDTI_CP_System_Pack) =
    {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x05, 0x01, 0x01, 0x0d, 0x01, 0x03, 0x01, 0x04, 0x01, 0x01, 0x00};

#define MXF_SDTI_CP_PACKAGE_METADATA_KEY(elecount) \
    {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x43, 0x01, 0x01, 0x0d, 0x01, 0x03, 0x01, 0x04, 0x01, 0x02, elecount}



/* D-10 mappings */

#define MXF_D10_PICTURE_EE_K(elenum) \
    MXF_GENERIC_CONTAINER_ELEMENT_KEY(0x01, 0x05, 0x01, 0x01, elenum)

#define MXF_D10_PICTURE_TRACK_NUM(elenum) \
    MXF_TRACK_NUM(0x05, 0x01, 0x01, elenum)

#define MXF_D10_SOUND_EE_K(elenum) \
    MXF_GENERIC_CONTAINER_ELEMENT_KEY(0x01, 0x06, 0x01, 0x10, elenum)

#define MXF_D10_SOUND_TRACK_NUM(elenum) \
    MXF_TRACK_NUM(0x06, 0x01, 0x10, elenum)

#define MXF_D10_AUX_EE_K(elecount, eletype, elenum) \
    MXF_GENERIC_CONTAINER_ELEMENT_KEY(0x01, 0x07, elecount, eletype, elenum)

#define MXF_D10_AUX_TRACK_NUM(elecount, eletype, elenum) \
    MXF_TRACK_NUM(0x07, elecount, eletype, elenum)


/* A-law mappings */

#define MXF_ALAW_EE_K(elecount, eletype, elenum) \
    MXF_GENERIC_CONTAINER_ELEMENT_KEY(0x01, 0x16, elecount, eletype, elenum)

#define MXF_ALAW_TRACK_NUM(elecount, eletype, elenum) \
    MXF_TRACK_NUM(0x16, elecount, eletype, elenum)

#define MXF_ALAW_FRAME_WRAPPED_EE_TYPE      0x08
#define MXF_ALAW_CLIP_WRAPPED_EE_TYPE       0x09
#define MXF_ALAW_CUSTOM_WRAPPED_EE_TYPE     0x0a


/* MPEG mappings */

#define MXF_MPEG_PICT_EE_K(elecount, eletype, elenum) \
    MXF_GENERIC_CONTAINER_ELEMENT_KEY(0x01, 0x15, elecount, eletype, elenum)

#define MXF_MPEG_PICT_TRACK_NUM(elecount, eletype, elenum) \
    MXF_TRACK_NUM(0x15, elecount, eletype, elenum)

#define MXF_MPEG_PICT_FRAME_WRAPPED_EE_TYPE      0x05
#define MXF_MPEG_PICT_CLIP_WRAPPED_EE_TYPE       0x06
#define MXF_MPEG_PICT_CUSTOM_WRAPPED_EE_TYPE     0x07


/* VC-3 mappings */

#define MXF_VC3_EE_K(elecount, eletype, elenum) \
    MXF_GENERIC_CONTAINER_ELEMENT_KEY(0x01, 0x15, elecount, eletype, elenum)

#define MXF_VC3_TRACK_NUM(elecount, eletype, elenum) \
    MXF_TRACK_NUM(0x15, elecount, eletype, elenum)

#define MXF_VC3_FRAME_WRAPPED_EE_TYPE      0x05
#define MXF_VC3_CLIP_WRAPPED_EE_TYPE       0x06



/*
 *
 * Partition pack keys
 *
 */

#define MXF_PP_K(statusName, kindName)  g_##statusName##_##kindName##_pp_key

#define MXF_PP_KEY(regver, kind, status) \
    {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x05, 0x01, regver, 0x0d, 0x01, 0x02, 0x01, 0x01, kind, status, 0x00}


static const mxfKey MXF_PP_K(OpenIncomplete, Header) =
    MXF_PP_KEY(0x01, 0x02, 0x01);

static const mxfKey MXF_PP_K(ClosedIncomplete, Header) =
    MXF_PP_KEY(0x01, 0x02, 0x02);

static const mxfKey MXF_PP_K(OpenComplete, Header) =
    MXF_PP_KEY(0x01, 0x02, 0x03);

static const mxfKey MXF_PP_K(ClosedComplete, Header) =
    MXF_PP_KEY(0x01, 0x02, 0x04);

static const mxfKey MXF_PP_K(OpenIncomplete, Body) =
    MXF_PP_KEY(0x01, 0x03, 0x01);

static const mxfKey MXF_PP_K(ClosedIncomplete, Body) =
    MXF_PP_KEY(0x01, 0x03, 0x02);

static const mxfKey MXF_PP_K(OpenComplete, Body) =
    MXF_PP_KEY(0x01, 0x03, 0x03);

static const mxfKey MXF_PP_K(ClosedComplete, Body) =
    MXF_PP_KEY(0x01, 0x03, 0x04);

static const mxfKey MXF_PP_K(OpenIncomplete, Footer) =
    MXF_PP_KEY(0x01, 0x04, 0x01);

static const mxfKey MXF_PP_K(ClosedIncomplete, Footer) =
    MXF_PP_KEY(0x01, 0x04, 0x02);

static const mxfKey MXF_PP_K(OpenComplete, Footer) =
    MXF_PP_KEY(0x01, 0x04, 0x03);

static const mxfKey MXF_PP_K(ClosedComplete, Footer) =
    MXF_PP_KEY(0x01, 0x04, 0x04);



/*
 *
 * Miscellaneous keys
 *
 */


/* Filler key */

/* Note: byte 7 (registry version) should actually be 0x02, but 0x01 is defined here
   for compatibility with existing applications */
static const mxfKey g_KLVFill_key =
    {0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x02, 0x10, 0x01, 0x00, 0x00, 0x00};


/* Random index pack key */

static const mxfKey g_RandomIndexPack_key =
    {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x05, 0x01, 0x01, 0x0d, 0x01, 0x02, 0x01, 0x01, 0x11, 0x01, 0x00};


/* Primer pack key */

static const mxfKey g_PrimerPack_key =
    {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x05, 0x01, 0x01, 0x0d, 0x01, 0x02, 0x01, 0x01, 0x05, 0x01, 0x00};


/* Index table segment key */

static const mxfKey g_IndexTableSegment_key =
    {0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01, 0x0d, 0x01, 0x02, 0x01, 0x01, 0x10, 0x01, 0x00};




#ifdef __cplusplus
}
#endif


#endif


