/*
 * $Id: mxf_utils.c,v 1.10 2011/05/19 17:14:20 philipn Exp $
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
 
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#if defined(_WIN32)

#include <sys/timeb.h>
#include <time.h>
#include <windows.h>

#else

#include <uuid/uuid.h>
#include <time.h>
#include <sys/time.h>

#endif


#include <mxf/mxf.h>


mxf_generate_uuid_func mxf_generate_uuid = mxf_default_generate_uuid;
mxf_get_timestamp_now_func mxf_get_timestamp_now = mxf_default_get_timestamp_now;
mxf_generate_umid_func mxf_generate_umid = mxf_default_generate_umid;
mxf_generate_key_func mxf_generate_key = mxf_default_generate_key;



static size_t utf8_code_len(const char *u8_code)
{
    if ((unsigned char)u8_code[0] < 0x80)
    {
        return 1;
    }
    else if (((unsigned char)u8_code[0] & 0xe0) == 0xc0 &&
             ((unsigned char)u8_code[1] & 0xc0) == 0x80)
    {
        return 2;
    }
    else if (((unsigned char)u8_code[0] & 0xf0) == 0xe0 &&
             ((unsigned char)u8_code[1] & 0xc0) == 0x80 &&
             ((unsigned char)u8_code[2] & 0xc0) == 0x80)
    {
        return 3;
    }
    else if (((unsigned char)u8_code[0] & 0xf8) == 0xf0 &&
             ((unsigned char)u8_code[1] & 0xc0) == 0x80 &&
             ((unsigned char)u8_code[2] & 0xc0) == 0x80 &&
             ((unsigned char)u8_code[3] & 0xc0) == 0x80)
    {
        return 4;
    }

    return -1;
}

static size_t utf16_code_len(const mxfUTF16Char *u16_code)
{
    if (u16_code[0] < 0xd800 || u16_code[0] > 0xdfff)
    {
        return 1;
    }
    else if (((u16_code[0] & 0xfc00) == 0xd800) &&
             ((u16_code[1] & 0xfc00) == 0xdc00))
    {
        return 2;
    }

    return -1;
}

static size_t utf8_code_len_from_utf16(const mxfUTF16Char *u16_code)
{
    if (u16_code[0] < 0x80)
    {
        return 1;
    }
    else if (u16_code[0] < 0x800)
    {
        return 2;
    }
    else if (u16_code[0] < 0xd800 || u16_code[0] > 0xdfff)
    {
        return 3;
    }
    else if (((u16_code[0] & 0xfc00) == 0xd800) &&
             ((u16_code[1] & 0xfc00) == 0xdc00))
    {
        return 4;
    }

    return -1;
}

static size_t utf16_code_len_from_utf8(const char *u8_code)
{
    size_t u8_len = utf8_code_len(u8_code);
    if (u8_len <= 3)
        return 1;
    else if (u8_len == 4)
        return 2;

    return -1;
}

static size_t utf8_code_to_utf16(mxfUTF16Char *u16_code, size_t avail_u16_len, const char *u8_code,
                                 size_t *u8_len, size_t *u16_len)
{
    size_t len16 = utf16_code_len_from_utf8(u8_code);
    size_t len8 = utf8_code_len(u8_code);
    if (len16 == (size_t)(-1) || len8 == (size_t)(-1) || len16 > avail_u16_len)
        return (size_t)(-1);

    if (len8 == 1)
    {
        u16_code[0] = (mxfUTF16Char)u8_code[0];
    }
    else if (len8 == 2)
    {
        u16_code[0] = ((((mxfUTF16Char)u8_code[0]) & 0x1f) << 6) |
                       (((mxfUTF16Char)u8_code[1]) & 0x3f);
    }
    else if (len8 == 3)
    {
        u16_code[0] = ((((mxfUTF16Char)u8_code[0]) & 0x0f) << 12) |
                      ((((mxfUTF16Char)u8_code[1]) & 0x3f) << 6) |
                       (((mxfUTF16Char)u8_code[2]) & 0x3f);
    }
    else
    {
        uint32_t c = ((((mxfUTF16Char)u8_code[0]) & 0x07) << 18) |
                     ((((mxfUTF16Char)u8_code[1]) & 0x3f) << 12) |
                     ((((mxfUTF16Char)u8_code[2]) & 0x3f) << 6) |
                      (((mxfUTF16Char)u8_code[3]) & 0x3f);
        c -= 0x10000;
        u16_code[0] = (mxfUTF16Char)(0xd800 | ((c >> 10) & 0x03ff));
        u16_code[1] = (mxfUTF16Char)(0xdc00 |  (c        & 0x03ff));
    }

    *u16_len = len16;
    *u8_len = len8;
    return len16;
}

static size_t utf16_code_to_utf8(char *u8_code, size_t avail_u8_len, const mxfUTF16Char *u16_code,
                                 size_t *u16_len, size_t *u8_len)
{
    size_t len8 = utf8_code_len_from_utf16(u16_code);
    size_t len16 = utf16_code_len(u16_code);
    if (len8 == (size_t)(-1) || len16 == (size_t)(-1) || len8 > avail_u8_len)
        return (size_t)(-1);

    if (len8 == 1)
    {
        u8_code[0] = (char)(u16_code[0]);
    }
    else if (len8 == 2)
    {
        u8_code[0] = (char)(0xc0 | (u16_code[0] >> 6));
        u8_code[1] = (char)(0x80 | (u16_code[0] & 0x3f));
    }
    else if (len8 == 3)
    {
        u8_code[0] = (char)(0xe0 |  (u16_code[0] >> 12));
        u8_code[1] = (char)(0x80 | ((u16_code[0] >> 6) & 0x3f));
        u8_code[2] = (char)(0x80 |  (u16_code[0]       & 0x3f));
    }
    else
    {
        uint32_t c = ((u16_code[0] & 0x03ff) << 10) |
                      (u16_code[1] & 0x03ff);
        c += 0x10000;
        u8_code[0] = (char)(0xf0 | ((c >> 18) & 0x07));
        u8_code[1] = (char)(0x80 | ((c >> 12) & 0x3f));
        u8_code[2] = (char)(0x80 | ((c >> 6)  & 0x3f));
        u8_code[3] = (char)(0x80 |  (c        & 0x3f));
    }

    *u8_len = len8;
    *u16_len = len16;
    return len8;
}

static size_t utf16_strlen_from_utf8(const char *u8_str)
{
    size_t len = 0;
    const char *u8_str_ptr = u8_str;
    while (*u8_str_ptr) {
        size_t u8_code_len = utf8_code_len(u8_str_ptr);
        size_t u16_code_len = utf16_code_len_from_utf8(u8_str_ptr);
        if (u8_code_len == (size_t)(-1) || u16_code_len == (size_t)(-1))
            return (size_t)(-1);

        u8_str_ptr += u8_code_len;
        len += u16_code_len;
    }

    return len;
}

static size_t utf8_strlen_from_utf16(const mxfUTF16Char *u16_str)
{
    size_t len = 0;
    const mxfUTF16Char *u16_str_ptr = u16_str;
    while (*u16_str_ptr) {
        size_t u8_code_len = utf8_code_len_from_utf16(u16_str_ptr);
        size_t u16_code_len = utf16_code_len(u16_str_ptr);
        if (u8_code_len == (size_t)(-1) || u16_code_len == (size_t)(-1))
            return (size_t)(-1);

        u16_str_ptr += u16_code_len;
        len += u8_code_len;
    }

    return len;
}




void mxf_print_key(const mxfKey* key)
{
    char keyStr[KEY_STR_SIZE];
    mxf_sprint_key(keyStr, key);
    printf("K = %s\n", keyStr);
}

void mxf_sprint_key(char* str, const mxfKey* key)
{
#if defined(_MSC_VER)
    _snprintf(
#else
    snprintf(
#endif
        str, KEY_STR_SIZE, "%02x %02x %02x %02x %02x %02x %02x %02x "
        "%02x %02x %02x %02x %02x %02x %02x %02x", 
        key->octet0, key->octet1, key->octet2, key->octet3,
        key->octet4, key->octet5, key->octet6, key->octet7,
        key->octet8, key->octet9, key->octet10, key->octet11,
        key->octet12, key->octet13, key->octet14, key->octet15);
}

void mxf_print_label(const mxfUL* label)
{
    mxf_print_key((const mxfKey*)label);
}

void mxf_sprint_label(char* str, const mxfUL* label)
{
    mxf_sprint_key(str, (const mxfKey*)label);
}

void mxf_print_umid(const mxfUMID* umid)
{
    char umidStr[UMID_STR_SIZE];
    mxf_sprint_umid(umidStr, umid);
    printf("UMID = %s\n", umidStr);
}

void mxf_sprint_umid(char* str, const mxfUMID* umid)
{
#if defined(_MSC_VER)
    _snprintf(
#else
    snprintf(
#endif
        str, UMID_STR_SIZE, "%02x %02x %02x %02x %02x %02x %02x %02x "
        "%02x %02x %02x %02x %02x %02x %02x %02x " 
        "%02x %02x %02x %02x %02x %02x %02x %02x " 
        "%02x %02x %02x %02x %02x %02x %02x %02x", 
        umid->octet0, umid->octet1, umid->octet2, umid->octet3,
        umid->octet4, umid->octet5, umid->octet6, umid->octet7,
        umid->octet8, umid->octet9, umid->octet10, umid->octet11,
        umid->octet12, umid->octet13, umid->octet14, umid->octet15,
        umid->octet16, umid->octet17, umid->octet18, umid->octet19,
        umid->octet20, umid->octet21, umid->octet22, umid->octet23,
        umid->octet24, umid->octet25, umid->octet26, umid->octet27,
        umid->octet28, umid->octet29, umid->octet30, umid->octet31);
}

void mxf_default_generate_uuid(mxfUUID* uuid)
{
#if defined(_WIN32)

    GUID guid;
    CoCreateGuid(&guid);
    memcpy(uuid, &guid, 16);
    
#else

    uuid_t newUUID;
    uuid_generate(newUUID);
    memcpy(uuid, newUUID, 16);
    
#endif
}

void mxf_default_get_timestamp_now(mxfTimestamp* now)
{
#if (defined(_MSC_VER) && _MSC_VER < 1400) || (defined(_WIN32) && defined(__GNUC__))
    /* MSVC 7 or MinGW */

    /* NOTE: gmtime is not thread safe (not reentrant) */
    
    struct _timeb tb;
    struct tm gmt;
    
    memset(&gmt, 0, sizeof(struct tm));
    
    _ftime(&tb);
    memcpy(&gmt, gmtime(&tb.time), sizeof(struct tm)); /* memcpy does nothing if gmtime returns NULL */
    
    now->year = gmt.tm_year + 1900;
    now->month = gmt.tm_mon + 1;
    now->day = gmt.tm_mday;
    now->hour = gmt.tm_hour;
    now->min = gmt.tm_min;
    now->sec = gmt.tm_sec;
    now->qmsec = (uint8_t)(tb.millitm / 4 + 0.5); /* 1/250th second */
    
#elif defined(_MSC_VER) || (defined(_WIN32) && defined(__GNUC__))
    /* MSVC 8 (MinGW doesn't seem to have gmtime_s() yet ) */

    struct _timeb tb;
    struct tm gmt;
    
    memset(&gmt, 0, sizeof(struct tm));
    
    /* using the secure _ftime */
    _ftime_s(&tb);
    
    /* using the secure (and reentrant) gmtime */
    gmtime_s(&gmt, &tb.time);

    now->year = (int16_t)gmt.tm_year + 1900;
    now->month = (uint8_t)gmt.tm_mon + 1;
    now->day = (uint8_t)gmt.tm_mday;
    now->hour = (uint8_t)gmt.tm_hour;
    now->min = (uint8_t)gmt.tm_min;
    now->sec = (uint8_t)gmt.tm_sec;
    now->qmsec = (uint8_t)(tb.millitm / 4 + 0.5); /* 1/250th second */
    
#else

    struct timeval tv;
    gettimeofday(&tv, NULL);

    /* use the reentrant gmtime */
    struct tm gmt;
    memset(&gmt, 0, sizeof(struct tm));
    gmtime_r(&tv.tv_sec, &gmt);
    
    now->year = gmt.tm_year + 1900;
    now->month = gmt.tm_mon + 1;
    now->day = gmt.tm_mday;
    now->hour = gmt.tm_hour;
    now->min = gmt.tm_min;
    now->sec = gmt.tm_sec;
    now->qmsec = (uint8_t)(tv.tv_usec / 4000 + 0.5); /* 1/250th second */

#endif
}


void mxf_default_generate_umid(mxfUMID* umid)
{
    mxfUUID uuid;
    
    umid->octet0 = 0x06;
    umid->octet1 = 0x0a;
    umid->octet2 = 0x2b;
    umid->octet3 = 0x34;
    umid->octet4 = 0x01;         
    umid->octet5 = 0x01;     
    umid->octet6 = 0x01; 
    umid->octet7 = 0x05; /* registry version */
    umid->octet8 = 0x01;
    umid->octet9 = 0x01;
    umid->octet10 = 0x0f; /* material type not identified */
    umid->octet11 = 0x20; /* UUID/UL material generation method, no instance method defined */
    
    umid->octet12 = 0x13;
    umid->octet13 = 0x00;
    umid->octet14 = 0x00;
    umid->octet15 = 0x00;
    
    /* Note: a UUID is mapped directly and a UL is half swapped */
    mxf_generate_uuid(&uuid);
    memcpy(&umid->octet16, &uuid, 16);
}

void mxf_default_generate_key(mxfKey* key)
{
    mxfUUID uuid;
    
    mxf_generate_uuid(&uuid);
    
    /* half-swap */
    memcpy(key, &uuid.octet8, 8);
    memcpy(&key->octet8, &uuid.octet0, 8);
}

void mxf_set_regtest_funcs()
{
    mxf_generate_uuid = mxf_regtest_generate_uuid;
    mxf_get_timestamp_now = mxf_regtest_get_timestamp_now;
    mxf_generate_umid = mxf_regtest_generate_umid;
    mxf_generate_key = mxf_regtest_generate_key;
}

void mxf_regtest_generate_uuid(mxfUUID* uuid)
{
    static uint32_t count = 1;

    memset(uuid, 0, sizeof(*uuid));
    uuid->octet12 = (uint8_t)((count >> 24) & 0xff);
    uuid->octet13 = (uint8_t)((count >> 16) & 0xff);
    uuid->octet14 = (uint8_t)((count >> 8) & 0xff);
    uuid->octet15 = (uint8_t)(count & 0xff);

    count++;
}

void mxf_regtest_get_timestamp_now(mxfTimestamp* now)
{
    memset(now, 0, sizeof(*now));
}

void mxf_regtest_generate_umid(mxfUMID* umid)
{
    static uint32_t count = 1;

    memset(umid, 0, sizeof(*umid));
    umid->octet28 = (uint8_t)((count >> 24) & 0xff);
    umid->octet29 = (uint8_t)((count >> 16) & 0xff);
    umid->octet30 = (uint8_t)((count >> 8) & 0xff);
    umid->octet31 = (uint8_t)(count & 0xff);

    count++;
}

void mxf_regtest_generate_key(mxfKey* key)
{
    static uint32_t count = 1;

    memset(key, 0, sizeof(*key));
    key->octet12 = (uint8_t)((count >> 24) & 0xff);
    key->octet13 = (uint8_t)((count >> 16) & 0xff);
    key->octet14 = (uint8_t)((count >> 8) & 0xff);
    key->octet15 = (uint8_t)(count & 0xff);

    count++;
}

size_t mxf_utf16_to_utf8(char *u8_str, const mxfUTF16Char *u16_str, size_t u8_size)
{
    size_t u8_len;
    size_t convert_size = 0;
    const mxfUTF16Char *u16_str_ptr = u16_str;
    char *u8_str_ptr = u8_str;
    size_t u8_code_len = 0;
    size_t u16_code_len = 0;

    if (!u16_str)
        return (size_t)(-1);

    u8_len = utf8_strlen_from_utf16(u16_str);
    if (u8_len == (size_t)(-1) || !u8_str)
        return u8_len;

    while (*u16_str_ptr && convert_size < u8_size) {
        if (utf16_code_to_utf8(u8_str_ptr, u8_size - convert_size, u16_str_ptr,
                               &u16_code_len, &u8_code_len) == (size_t)(-1))
        {
            break;
        }

        u8_str_ptr += u8_code_len;
        u16_str_ptr += u16_code_len;
        convert_size += u8_code_len;
    }
    if (convert_size < u8_size)
        *u8_str_ptr = 0;

    return convert_size;
}

size_t mxf_utf8_to_utf16(mxfUTF16Char *u16_str, const char *u8_str, size_t u16_size)
{
    size_t u16_len;
    size_t convert_size = 0;
    mxfUTF16Char *u16_str_ptr = u16_str;
    const char *u8_str_ptr = u8_str;
    size_t u8_code_len = 0;
    size_t u16_code_len = 0;

    if (!u8_str)
        return (size_t)(-1);

    u16_len = utf16_strlen_from_utf8(u8_str);
    if (u16_len == (size_t)(-1) || !u16_str)
        return u16_len;

    while (*u8_str_ptr && convert_size < u16_size) {
        if (utf8_code_to_utf16(u16_str_ptr, u16_size - convert_size, u8_str_ptr,
                               &u8_code_len, &u16_code_len) == (size_t)(-1))
        {
            break;
        }

        u8_str_ptr += u8_code_len;
        u16_str_ptr += u16_code_len;
        convert_size += u16_code_len;
    }
    if (convert_size < u16_size)
        *u16_str_ptr = 0;

    return convert_size;
}

