/*
 * $Id: mxf_utils.h,v 1.5 2011/02/23 14:42:29 philipn Exp $
 *
 * General purpose utilities
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
 
#ifndef __MXF_UTILS_H__
#define __MXF_UTILS_H__



#ifdef __cplusplus
extern "C" 
{
#endif


#define KEY_STR_SIZE        48
#define LABEL_STR_SIZE      48
#define UMID_STR_SIZE       96


typedef void (*mxf_generate_uuid_func)(mxfUUID* uuid);
typedef void (*mxf_get_timestamp_now_func)(mxfTimestamp* now);
typedef void (*mxf_generate_umid_func)(mxfUMID* umid);
typedef void (*mxf_generate_key_func)(mxfKey* key);



void mxf_print_key(const mxfKey* key);
void mxf_sprint_key(char* str, const mxfKey* key);

void mxf_print_label(const mxfUL* label);
void mxf_sprint_label(char* str, const mxfUL* label);

void mxf_print_umid(const mxfUMID* umid);
void mxf_sprint_umid(char* str, const mxfUMID* umid);


extern mxf_generate_uuid_func mxf_generate_uuid;
extern mxf_get_timestamp_now_func mxf_get_timestamp_now;
extern mxf_generate_umid_func mxf_generate_umid;
extern mxf_generate_key_func mxf_generate_key;

void mxf_default_generate_uuid(mxfUUID* uuid);
void mxf_default_get_timestamp_now(mxfTimestamp* now);
void mxf_default_generate_umid(mxfUMID* umid);
void mxf_default_generate_key(mxfKey* key);

void mxf_set_regtest_funcs();
void mxf_regtest_generate_uuid(mxfUUID* uuid);
void mxf_regtest_get_timestamp_now(mxfTimestamp* now);
void mxf_regtest_generate_umid(mxfUMID* umid);
void mxf_regtest_generate_key(mxfKey* key);


size_t mxf_utf16_to_utf8(char *u8_str, const mxfUTF16Char *u16_str, size_t u8_size);
size_t mxf_utf8_to_utf16(mxfUTF16Char *u16_str, const char *u8_str, size_t u16_size);



#ifdef __cplusplus
}
#endif


#endif


