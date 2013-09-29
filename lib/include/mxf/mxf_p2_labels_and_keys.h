/*
 * $Id: mxf_p2_labels_and_keys.h,v 1.1 2009/06/18 15:10:44 philipn Exp $
 *
 * P2 labels, keys, etc.
 *
 * Copyright (C) 2009  Philip de Nier <philipn@users.sourceforge.net>
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
 
#ifndef __MXF_P2_LABELS_AND_KEYS_H__
#define __MXF_P2_LABELS_AND_KEYS_H__


#ifdef __cplusplus
extern "C" 
{
#endif



/*
 *
 * Essence container labels
 *
 */

/* P2 AVC Intra-Frame Coding - version byte is incorrectly set to 0x01 */

static const mxfUL MXF_EC_L(P2AVCIFrameWrapped) =
    MXF_MPEG_EC_L(0x01, 0x10, 0x60, 0x01);

static const mxfUL MXF_EC_L(P2AVCIClipWrapped) =
    MXF_MPEG_EC_L(0x01, 0x10, 0x60, 0x02);




#ifdef __cplusplus
}
#endif


#endif

