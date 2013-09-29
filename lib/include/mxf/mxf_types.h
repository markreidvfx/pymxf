/*
 * $Id: mxf_types.h,v 1.2 2007/09/11 13:24:54 stuart_hc Exp $
 *
 * MXF types
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
 
#ifndef __MXF_TYPES_H__
#define __MXF_TYPES_H__

#include <wchar.h>


#ifdef __cplusplus
extern "C" 
{
#endif



#if defined(_MSC_VER) && defined(_WIN32)

// Provide ISO C types which are missing in MSVC
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned long int uint32_t;
typedef unsigned __int64 uint64_t;

typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed long int int32_t;
typedef __int64 int64_t;

#else

// include ISO fixed-width integer types
#include <inttypes.h>

#endif


typedef struct
{
    uint8_t octet0;
    uint8_t octet1;
    uint8_t octet2;
    uint8_t octet3;
    uint8_t octet4;
    uint8_t octet5;
    uint8_t octet6;
    uint8_t octet7;
    uint8_t octet8;
    uint8_t octet9;
    uint8_t octet10;
    uint8_t octet11;
    uint8_t octet12;
    uint8_t octet13;
    uint8_t octet14;
    uint8_t octet15;
} mxfUL;

typedef mxfUL mxfKey;

typedef struct 
{
    uint8_t octet0;
    uint8_t octet1;
    uint8_t octet2;
    uint8_t octet3;
    uint8_t octet4;
    uint8_t octet5;
    uint8_t octet6;
    uint8_t octet7;
    uint8_t octet8;
    uint8_t octet9;
    uint8_t octet10;
    uint8_t octet11;
    uint8_t octet12;
    uint8_t octet13;
    uint8_t octet14;
    uint8_t octet15;
} mxfUUID;

typedef mxfUL mxfUID;

typedef mxfUID mxfAUID;

typedef uint16_t mxfLocalTag;

typedef uint16_t mxfVersionType;

typedef struct
{
    int16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t qmsec;
} mxfTimestamp;

typedef wchar_t mxfUTF16Char;

typedef struct 
{
    uint8_t octet0;
    uint8_t octet1;
    uint8_t octet2;
    uint8_t octet3;
    uint8_t octet4;
    uint8_t octet5;
    uint8_t octet6;
    uint8_t octet7;
    uint8_t octet8;
    uint8_t octet9;
    uint8_t octet10;
    uint8_t octet11;
    uint8_t octet12;
    uint8_t octet13;
    uint8_t octet14;
    uint8_t octet15;
    uint8_t octet16;
    uint8_t octet17;
    uint8_t octet18;
    uint8_t octet19;
    uint8_t octet20;
    uint8_t octet21;
    uint8_t octet22;
    uint8_t octet23;
    uint8_t octet24;
    uint8_t octet25;
    uint8_t octet26;
    uint8_t octet27;
    uint8_t octet28;
    uint8_t octet29;
    uint8_t octet30;
    uint8_t octet31;
} mxfUMID;

typedef struct
{
    int32_t numerator;
    int32_t denominator;
} mxfRational;
    
typedef int64_t mxfPosition;

typedef int64_t mxfLength;

typedef uint8_t mxfBoolean;


typedef struct
{
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
    uint16_t build;
    uint16_t release;
} mxfProductVersion;


typedef struct
{
    uint8_t code;
    uint8_t depth;
} mxfRGBALayoutComponent;


/* external MXF data lengths */
#define mxfLocalTag_extlen              2
#define mxfVersionType_extlen           2
#define mxfUUID_extlen                  16 
#define mxfKey_extlen                   16 
#define mxfUID_extlen                   16 
#define mxfUL_extlen                    16 
#define mxfAUID_extlen                  16 
#define mxfTimestamp_extlen             8
#define mxfUTF16Char_extlen             2
#define mxfUMID_extlen                  32
#define mxfRational_extlen              8
#define mxfPosition_extlen              8
#define mxfLength_extlen                8
#define mxfBoolean_extlen               1
#define mxfProductVersion_extlen        10
#define mxfRGBALayoutComponent_extlen   2


static const mxfUUID g_Null_UUID =
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const mxfKey g_Null_Key =
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const mxfUL g_Null_UL =
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const mxfLocalTag g_Null_LocalTag = 0x0000;

static const mxfUMID g_Null_UMID =
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const mxfRational g_Null_Rational = {0, 0};


#ifdef __cplusplus
}
#endif


#endif 
