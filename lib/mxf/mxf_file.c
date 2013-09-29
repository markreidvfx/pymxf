/*
 * $Id: mxf_file.c,v 1.9 2010/07/26 16:02:37 philipn Exp $
 *
 * Wraps a files, buffers etc. in an MXF file and provides low-level functions
 *
 * Copyright (C) 2006  Philip de Nier <philipn@users.sourceforge.net>
 * Copyright (C) 2006  Stuart Cunningham <stuart_hc@users.sourceforge.net>
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
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#else
#include <sys/stat.h>
#endif

#include <mxf/mxf.h>


#if defined(_WIN32)
#if !defined(_MSC_VER) || (_MSC_VER < 1400)
/* use low-level file I/O */
#define USE_LOW_LEVEL_IO 1
#else
/* 64-bit ftell and fseek were introduced in Visual C++ 2005 */
#define fseeko _fseeki64
#define ftello _ftelli64
#define fstat64 _fstati64
#endif
#endif



/* size of buffer used to skip data by reading and discarding */
#define SKIP_BUFFER_SIZE        2048


struct MXFFileSysData
{
    /* Disk file system data */
#if defined(USE_LOW_LEVEL_IO)
    int fileId;
#else
    FILE* file;
#endif
    int isSeekable;
    int haveTestedIsSeekable;

    /* used for stdin only */
    int64_t byteCount;
    
    /* used for byte arrays */
    const uint8_t* data;
    int64_t dataSize;
    int64_t pos;
};


static void disk_file_close(MXFFileSysData* sysData)
{
#if defined(USE_LOW_LEVEL_IO)
    if (sysData->fileId != -1)
    {
        close(sysData->fileId);
        sysData->fileId = -1;
    }
#else
    if (sysData->file != NULL)
    {
        fclose(sysData->file);
        sysData->file = NULL;
    }
#endif
}

static uint32_t disk_file_read(MXFFileSysData* sysData, uint8_t* data, uint32_t count)
{
#if defined(USE_LOW_LEVEL_IO)
    return read(sysData->fileId, data, count);
#else
    return (uint32_t)fread(data, 1, count, sysData->file);
#endif
}

static uint32_t disk_file_write(MXFFileSysData* sysData, const uint8_t* data, uint32_t count)
{
#if defined(USE_LOW_LEVEL_IO)
    return write(sysData->fileId, data, count);
#else
    return (uint32_t)fwrite(data, 1, count, sysData->file);
#endif
}

static int disk_file_getchar(MXFFileSysData* sysData)
{
#if defined(USE_LOW_LEVEL_IO)
    uint8_t c;
    if (read(sysData->fileId, &c, 1) != 1)
    {
        return EOF;
    }
    return c;
#else
    return fgetc(sysData->file);
#endif
}

static int disk_file_putchar(MXFFileSysData* sysData, int c)
{
#if defined(USE_LOW_LEVEL_IO)
    uint8_t cbyte = (uint8_t)c;
    if (write(sysData->fileId, &cbyte, 1) != 1)
    {
        return EOF;
    }
    return c;
#else
    return fputc(c, sysData->file);
#endif
}

static int disk_file_eof(MXFFileSysData* sysData)
{
#if defined(USE_LOW_LEVEL_IO)
    return eof(sysData->fileId);
#else
    return feof(sysData->file);
#endif
}

static int disk_file_seek(MXFFileSysData* sysData, int64_t offset, int whence)
{
#if defined(USE_LOW_LEVEL_IO)
    return _lseeki64(sysData->fileId, offset, whence) != -1;
#else
    return fseeko(sysData->file, offset, whence) == 0;
#endif
}

static int64_t disk_file_tell(MXFFileSysData* sysData)
{
#if defined(USE_LOW_LEVEL_IO)
    return _telli64(sysData->fileId);
#else
    return ftello(sysData->file);
#endif
}

static void free_disk_file(MXFFileSysData* sysData)
{
    if (sysData == NULL)
    {
        return;
    }
    
    free(sysData);
}

static int disk_file_is_seekable(MXFFileSysData* sysData)
{
    if (sysData == NULL)
    {
        return 0;
    }
    
    if (!sysData->haveTestedIsSeekable)
    {
#if defined(USE_LOW_LEVEL_IO)
        sysData->isSeekable = (_lseek(sysData->fileId, 0, SEEK_CUR) != -1);
#else
        sysData->isSeekable = (fseek(sysData->file, 0, SEEK_CUR) == 0);
#endif
        sysData->haveTestedIsSeekable = 1;
    }

    return sysData->isSeekable;
}

static int64_t disk_file_size(MXFFileSysData* sysData)
{
#if defined(USE_LOW_LEVEL_IO)
    struct _stati64 statBuf;
#elif defined(_MSC_VER)
    int fo;
    struct _stati64 statBuf;
#else
    int fo;
    struct stat statBuf;
#endif    
    
    if (sysData == NULL)
    {
        return -1;
    }
    
#if defined(USE_LOW_LEVEL_IO)
    if (_fstati64(sysData->fileId, &statBuf) != 0) 
    {
        return -1;
    }
    return statBuf.st_size;
    
#elif defined(_MSC_VER)
    if ((fo = _fileno(sysData->file)) == -1)
    {
        return -1;
    }
    else if (_fstati64(fo, &statBuf) != 0) 
    {
        return -1;
    }
    return statBuf.st_size;
    
#else
    if ((fo = fileno(sysData->file)) == -1)
    {
        return -1;
    }
    else if (fstat(fo, &statBuf) != 0) 
    {
        return -1;
    }
    return statBuf.st_size;
#endif
}


static void stdin_file_close(MXFFileSysData* sysData)
{
#if defined(USE_LOW_LEVEL_IO)
    sysData->fileId = -1;
#else
    sysData->file = NULL;
#endif
    sysData->byteCount = -1;
}

static uint32_t stdin_file_read(MXFFileSysData* sysData, uint8_t* data, uint32_t count)
{
    uint32_t numRead;
    
    /* fail if reading the bytes will cause the byte count to exceed the maximum and wrap around */
    if (sysData->byteCount + count < 0)
    {
        return 0;
    }
    
#if defined(USE_LOW_LEVEL_IO)
    numRead = read(sysData->fileId, data, count);
#else
    numRead = (uint32_t)fread(data, 1, count, sysData->file);
#endif

    sysData->byteCount += numRead;
    
    return numRead;
}

static uint32_t stdin_file_write(MXFFileSysData* sysData, const uint8_t* data, uint32_t count)
{
    (void)sysData;
    (void)data;
    (void)count;
    
    /* stdin is not writeable */
    return 0;
}

static int stdin_file_getchar(MXFFileSysData* sysData)
{
#if defined(USE_LOW_LEVEL_IO)
    uint8_t c;
#else
    int c;
#endif

    /* fail if reading the bytes will cause the byte count to exceed the maximum and wrap around */
    if (sysData->byteCount + 1 < 0)
    {
        return 0;
    }
    
#if defined(USE_LOW_LEVEL_IO)
    if (read(sysData->fileId, &c, 1) != 1)
    {
        return EOF;
    }
    sysData->byteCount++;
    return c;
#else
    if ((c = fgetc(sysData->file)) == EOF)
    {
        return EOF;
    }
    sysData->byteCount++;
    return c;
#endif
}

static int stdin_file_putchar(MXFFileSysData* sysData, int c)
{
    (void)sysData;
    (void)c;
    
    /* stdin is not writeable */
    return EOF;
}

static int stdin_file_eof(MXFFileSysData* sysData)
{
#if defined(USE_LOW_LEVEL_IO)
    return eof(sysData->fileId);
#else
    return feof(sysData->file);
#endif
}

static int stdin_file_seek(MXFFileSysData* sysData, int64_t offset, int whence)
{
    (void)sysData;
    (void)offset;
    (void)whence;
    
    /* stdin is not seekable */
    return 0;
}

static int64_t stdin_file_tell(MXFFileSysData* sysData)
{
    return sysData->byteCount;
}

static void free_stdin_file(MXFFileSysData* sysData)
{
    if (sysData == NULL)
    {
        return;
    }

    free(sysData);
}

static int stdin_file_is_seekable(MXFFileSysData* sysData)
{
    (void)sysData;
    
    return 0;
}

static int64_t stdin_file_size(MXFFileSysData* sysData)
{
    (void)sysData;
    
    return -1;
}


static void byte_array_file_close(MXFFileSysData* sysData)
{
    sysData->data = NULL;
    sysData->pos = -1;
}

static uint32_t byte_array_file_read(MXFFileSysData* sysData, uint8_t *data, uint32_t count)
{
    uint32_t numRead;
    
    if (sysData->pos >= sysData->dataSize)
    {
        return 0;
    }
    
    if (sysData->pos + count > sysData->dataSize)
    {
        numRead = (uint32_t)(sysData->dataSize - sysData->pos);
    }
    else
    {
        numRead = count;
    }
    
    memcpy(data, &sysData->data[sysData->pos], numRead);
    sysData->pos += numRead;
    
    return numRead;
}

static uint32_t byte_array_file_write(MXFFileSysData* sysData, const uint8_t* data, uint32_t count)
{
    (void)sysData;
    (void)data;
    (void)count;
    
    /* not allowed for read-only byte array */
    return 0;
}

static int byte_array_file_getchar(MXFFileSysData* sysData)
{
    if (sysData->pos + 1 > sysData->dataSize)
    {
        return EOF;
    }
    
    return sysData->data[sysData->pos++];    
}

static int byte_array_file_putchar(MXFFileSysData* sysData, int c)
{
    (void)sysData;
    (void)c;
    
    /* not allowed for read-only byte array */
    return 0;
}

static int byte_array_file_eof(MXFFileSysData* sysData)
{
    if (sysData->pos >= sysData->dataSize)
    {
        return 1;
    }
    return 0;
}

static int byte_array_file_seek(MXFFileSysData* sysData, int64_t offset, int whence)
{
    if (whence == SEEK_SET)
    {
        if (offset < 0 || offset >= sysData->dataSize)
        {
            return 0;
        }
        sysData->pos = offset;
    }
    else if (whence == SEEK_CUR)
    {
        if (sysData->pos + offset < 0 || sysData->pos + offset >= sysData->dataSize)
        {
            return 0;
        }
        sysData->pos += offset;
    }
    else /* SEEK_END */
    {
        if (sysData->dataSize - 1 - offset < 0 ||
            sysData->dataSize - 1 - offset >= sysData->dataSize )
        {
            return 0;
        }
        sysData->pos = sysData->dataSize - 1 - offset;
    }
    
    return 1;
}

static int64_t byte_array_file_tell(MXFFileSysData* sysData)
{
    return sysData->pos;
}

static int byte_array_file_is_seekable(MXFFileSysData* sysData)
{
    (void)sysData;
    
    return 1;
}

static int64_t byte_array_size(MXFFileSysData* sysData)
{
    return sysData->dataSize;
}

static void free_byte_array_file(MXFFileSysData* sysData)
{
    if (sysData == NULL)
    {
        return;
    }
    
    free(sysData);
}


int mxf_disk_file_open_new(const char* filename, MXFFile** mxfFile)
{
    MXFFile* newMXFFile = NULL;
    MXFFileSysData* newDiskFile = NULL;
    
    CHK_MALLOC_ORET(newMXFFile, MXFFile);
    memset(newMXFFile, 0, sizeof(MXFFile));
    CHK_MALLOC_OFAIL(newDiskFile, MXFFileSysData);
    memset(newDiskFile, 0, sizeof(MXFFileSysData));
    
#if defined(USE_LOW_LEVEL_IO)
    if ((newDiskFile->fileId = open(filename, _O_BINARY | _O_RDWR | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE)) == -1)
#else
    if ((newDiskFile->file = fopen(filename, "w+b")) == NULL)
#endif
    {
        goto fail;
    }
    
    newMXFFile->close = disk_file_close;
    newMXFFile->read = disk_file_read;
    newMXFFile->write = disk_file_write;
    newMXFFile->get_char = disk_file_getchar;
    newMXFFile->put_char = disk_file_putchar;
    newMXFFile->eof = disk_file_eof;
    newMXFFile->seek = disk_file_seek;
    newMXFFile->tell = disk_file_tell;
    newMXFFile->is_seekable = disk_file_is_seekable;
    newMXFFile->size = disk_file_size;
    newMXFFile->sysData = newDiskFile;
    newMXFFile->free_sys_data = free_disk_file;
    
    *mxfFile = newMXFFile;
    return 1;
    
fail:
    SAFE_FREE(&newMXFFile);
    SAFE_FREE(&newDiskFile);
    return 0;
}

int mxf_disk_file_open_read(const char* filename, MXFFile** mxfFile)
{
    MXFFile* newMXFFile = NULL;
    MXFFileSysData* newDiskFile = NULL;
    
    CHK_MALLOC_ORET(newMXFFile, MXFFile);
    memset(newMXFFile, 0, sizeof(MXFFile));
    CHK_MALLOC_OFAIL(newDiskFile, MXFFileSysData);
    memset(newDiskFile, 0, sizeof(MXFFileSysData));
    
#if defined(USE_LOW_LEVEL_IO)
    if ((newDiskFile->fileId = open(filename, _O_BINARY | _O_RDONLY)) == -1)
#else
    if ((newDiskFile->file = fopen(filename, "rb")) == NULL)
#endif
    {
        goto fail;
    }

    newMXFFile->close = disk_file_close;
    newMXFFile->read = disk_file_read;
    newMXFFile->write = disk_file_write;
    newMXFFile->get_char = disk_file_getchar;
    newMXFFile->put_char = disk_file_putchar;
    newMXFFile->eof = disk_file_eof;
    newMXFFile->seek = disk_file_seek;
    newMXFFile->tell = disk_file_tell;
    newMXFFile->is_seekable = disk_file_is_seekable;
    newMXFFile->size = disk_file_size;
    newMXFFile->sysData = newDiskFile;
    newMXFFile->free_sys_data = free_disk_file;
    
    *mxfFile = newMXFFile;
    return 1;
    
fail:
    SAFE_FREE(&newMXFFile);
    SAFE_FREE(&newDiskFile);
    return 0;
}

int mxf_disk_file_open_modify(const char* filename, MXFFile** mxfFile)
{
    MXFFile* newMXFFile = NULL;
    MXFFileSysData* newDiskFile = NULL;
    
    CHK_MALLOC_ORET(newMXFFile, MXFFile);
    memset(newMXFFile, 0, sizeof(MXFFile));
    CHK_MALLOC_OFAIL(newDiskFile, MXFFileSysData);
    memset(newDiskFile, 0, sizeof(MXFFileSysData));
    
#if defined(USE_LOW_LEVEL_IO)
    if ((newDiskFile->fileId = open(filename, _O_BINARY | _O_RDWR)) == -1)
#else
    if ((newDiskFile->file = fopen(filename, "r+b")) == NULL)
#endif
    {
        goto fail;
    }
            
    newMXFFile->close = disk_file_close;
    newMXFFile->read = disk_file_read;
    newMXFFile->write = disk_file_write;
    newMXFFile->get_char = disk_file_getchar;
    newMXFFile->put_char = disk_file_putchar;
    newMXFFile->eof = disk_file_eof;
    newMXFFile->seek = disk_file_seek;
    newMXFFile->tell = disk_file_tell;
    newMXFFile->is_seekable = disk_file_is_seekable;
    newMXFFile->size = disk_file_size;
    newMXFFile->sysData = newDiskFile;
    newMXFFile->free_sys_data = free_disk_file;
    
    *mxfFile = newMXFFile;
    return 1;
    
fail:
    SAFE_FREE(&newMXFFile);
    SAFE_FREE(&newDiskFile);
    return 0;
}


int mxf_stdin_wrap_read(MXFFile** mxfFile)
{
    MXFFile* newMXFFile = NULL;
    MXFFileSysData* newStdInFile = NULL;
    
    CHK_MALLOC_ORET(newMXFFile, MXFFile);
    memset(newMXFFile, 0, sizeof(MXFFile));
    CHK_MALLOC_OFAIL(newStdInFile, MXFFileSysData);
    memset(newStdInFile, 0, sizeof(MXFFileSysData));

#if defined(USE_LOW_LEVEL_IO)
    newStdInFile->fileId = 0;
#else
    newStdInFile->file = stdin;
#endif

    newMXFFile->close = stdin_file_close;
    newMXFFile->read = stdin_file_read;
    newMXFFile->write = stdin_file_write;
    newMXFFile->get_char = stdin_file_getchar;
    newMXFFile->put_char = stdin_file_putchar;
    newMXFFile->eof = stdin_file_eof;
    newMXFFile->seek = stdin_file_seek;
    newMXFFile->tell = stdin_file_tell;
    newMXFFile->is_seekable = stdin_file_is_seekable;
    newMXFFile->size = stdin_file_size;
    newMXFFile->sysData = newStdInFile;
    newMXFFile->free_sys_data = free_stdin_file;
    
    *mxfFile = newMXFFile;
    return 1;
    
fail:
    SAFE_FREE(&newMXFFile);
    SAFE_FREE(&newStdInFile);
    return 0;
}


int mxf_byte_array_wrap_read(const uint8_t* data, int64_t dataSize, MXFFile **mxfFile)
{
    MXFFile* newMXFFile = NULL;
    MXFFileSysData* newSysData = NULL;
    
    if (dataSize < 0)
    {
        return 0;
    }
    
    CHK_MALLOC_ORET(newMXFFile, MXFFile);
    memset(newMXFFile, 0, sizeof(MXFFile));
    CHK_MALLOC_OFAIL(newSysData, MXFFileSysData);
    memset(newSysData, 0, sizeof(MXFFileSysData));
    
    newSysData->data = data;
    newSysData->dataSize = (data != NULL) ? dataSize : 0;

    newMXFFile->close = byte_array_file_close;
    newMXFFile->read = byte_array_file_read;
    newMXFFile->write = byte_array_file_write;
    newMXFFile->get_char = byte_array_file_getchar;
    newMXFFile->put_char = byte_array_file_putchar;
    newMXFFile->eof = byte_array_file_eof;
    newMXFFile->seek = byte_array_file_seek;
    newMXFFile->tell = byte_array_file_tell;
    newMXFFile->is_seekable = byte_array_file_is_seekable;
    newMXFFile->size = byte_array_size;
    newMXFFile->sysData = newSysData;
    newMXFFile->free_sys_data = free_byte_array_file;
    

    *mxfFile = newMXFFile;
    return 1;

fail:
    SAFE_FREE(&newMXFFile);
    SAFE_FREE(&newSysData);
    return 0;
}



void mxf_file_close(MXFFile** mxfFile)
{
    if (*mxfFile == NULL)
    {
        return;
    }
    
    if ((*mxfFile)->sysData != NULL)
    {
        (*mxfFile)->close((*mxfFile)->sysData);
        if ((*mxfFile)->free_sys_data != NULL)
        {
            (*mxfFile)->free_sys_data((*mxfFile)->sysData);
        }
    }
    
    SAFE_FREE(mxfFile);
}

uint32_t mxf_file_read(MXFFile* mxfFile, uint8_t* data, uint32_t count)
{
    return mxfFile->read(mxfFile->sysData, data, count);
}

uint32_t mxf_file_write(MXFFile* mxfFile, const uint8_t* data, uint32_t count)
{
    return mxfFile->write(mxfFile->sysData, data, count);
}

int mxf_file_getc(MXFFile* mxfFile)
{
    return mxfFile->get_char(mxfFile->sysData);
}

int mxf_file_putc(MXFFile* mxfFile, int c)
{
    return mxfFile->put_char(mxfFile->sysData, c);
}

int mxf_file_eof(MXFFile* mxfFile)
{
    return mxfFile->eof(mxfFile->sysData);
}

int mxf_file_seek(MXFFile* mxfFile, int64_t offset, int whence)
{
    return mxfFile->seek(mxfFile->sysData, offset, whence);
}

int64_t mxf_file_tell(MXFFile* mxfFile)
{
    return mxfFile->tell(mxfFile->sysData);
}

int mxf_file_is_seekable(MXFFile* mxfFile)
{
    return mxfFile->is_seekable(mxfFile->sysData);
}

int64_t mxf_file_size(MXFFile* mxfFile)
{
    return mxfFile->size(mxfFile->sysData);
}


void mxf_file_set_min_llen(MXFFile* mxfFile, uint8_t llen)
{
    mxfFile->minLLen = llen;
}

uint8_t mxf_get_min_llen(MXFFile* mxfFile)
{
    if (mxfFile->minLLen != 0)
    {
        return mxfFile->minLLen;
    }
    return 1;
}


int mxf_read_uint8(MXFFile* mxfFile, uint8_t* value)
{
    uint8_t buffer[1];
    CHK_ORET(mxf_file_read(mxfFile, buffer, 1) == 1);
    
    *value = buffer[0];
    
    return 1;
}

int mxf_read_uint16(MXFFile* mxfFile, uint16_t* value)
{
    uint8_t buffer[2];
    CHK_ORET(mxf_file_read(mxfFile, buffer, 2) == 2);
    
    *value = (buffer[0]<<8) | (buffer[1]);
    
    return 1;
}

int mxf_read_uint32(MXFFile* mxfFile, uint32_t* value)
{
    uint8_t buffer[4];
    CHK_ORET(mxf_file_read(mxfFile, buffer, 4) == 4);
    
    *value = (buffer[0]<<24) | (buffer[1]<<16) | (buffer[2]<<8) | (buffer[3]);
    
    return 1;
}

int mxf_read_uint64(MXFFile* mxfFile, uint64_t* value)
{
    uint8_t buffer[8];
    CHK_ORET(mxf_file_read(mxfFile, buffer, 8) == 8);
    
    *value = ((uint64_t)buffer[0]<<56) | ((uint64_t)buffer[1]<<48) | 
        ((uint64_t)buffer[2]<<40) | ((uint64_t)buffer[3]<<32) |
        ((uint64_t)buffer[4]<<24) | ((uint64_t)buffer[5]<<16) | 
        ((uint64_t)buffer[6]<<8) | (buffer[7]);
    
    return 1;
}

int mxf_read_int8(MXFFile* mxfFile, int8_t* value)
{
    return mxf_read_uint8(mxfFile, (uint8_t*)value);
}

int mxf_read_int16(MXFFile* mxfFile, int16_t* value)
{
    return mxf_read_uint16(mxfFile, (uint16_t*)value);
}

int mxf_read_int32(MXFFile* mxfFile, int32_t* value)
{
    return mxf_read_uint32(mxfFile, (uint32_t*)value);
}

int mxf_read_int64(MXFFile* mxfFile, int64_t* value)
{
    return mxf_read_uint64(mxfFile, (uint64_t*)value);
}


int mxf_write_uint8(MXFFile* mxfFile, uint8_t value)
{
    CHK_ORET(mxf_file_write(mxfFile, &value, 1) == 1);
    
    return 1;
}

int mxf_write_uint16(MXFFile* mxfFile, uint16_t value)
{
    uint8_t buffer[2];
    buffer[0] = (uint8_t)((value>>8) & 0xff);
    buffer[1] = (uint8_t)((value) & 0xff);
    CHK_ORET(mxf_file_write(mxfFile, buffer, 2) == 2);
    
    return 1;
}

int mxf_write_uint32(MXFFile* mxfFile, uint32_t value)
{
    uint8_t buffer[4];
    buffer[0] = (uint8_t)((value>>24) & 0xff);
    buffer[1] = (uint8_t)((value>>16) & 0xff);
    buffer[2] = (uint8_t)((value>>8) & 0xff);
    buffer[3] = (uint8_t)((value) & 0xff);
    CHK_ORET(mxf_file_write(mxfFile, buffer, 4) == 4);
    
    return 1;
}

int mxf_write_uint64(MXFFile* mxfFile, uint64_t value)
{
    uint8_t buffer[8];
    buffer[0] = (uint8_t)((value>>56) & 0xff);
    buffer[1] = (uint8_t)((value>>48) & 0xff);
    buffer[2] = (uint8_t)((value>>40) & 0xff);
    buffer[3] = (uint8_t)((value>>32) & 0xff);
    buffer[4] = (uint8_t)((value>>24) & 0xff);
    buffer[5] = (uint8_t)((value>>16) & 0xff);
    buffer[6] = (uint8_t)((value>>8) & 0xff);
    buffer[7] = (uint8_t)((value) & 0xff);
    CHK_ORET(mxf_file_write(mxfFile, buffer, 8) == 8);
    
    return 1;
}

int mxf_write_int8(MXFFile* mxfFile, int8_t value)
{
    return mxf_write_uint8(mxfFile, *(uint8_t*)&value);
}

int mxf_write_int16(MXFFile* mxfFile, int16_t value)
{
    return mxf_write_uint16(mxfFile, *(uint16_t*)&value);
}

int mxf_write_int32(MXFFile* mxfFile, int32_t value)
{
    return mxf_write_uint32(mxfFile, *(uint32_t*)&value);
}

int mxf_write_int64(MXFFile* mxfFile, int64_t value)
{
    return mxf_write_uint64(mxfFile, *(uint64_t*)&value);
}


int mxf_read_ul(MXFFile* mxfFile, mxfUL* value)
{
    CHK_ORET(mxf_file_read(mxfFile, (uint8_t*)value, 16) == 16); 
    
    return 1;
}

int mxf_read_k(MXFFile* mxfFile, mxfKey* key)
{
    CHK_ORET(mxf_file_read(mxfFile, (uint8_t*)key, 16) == 16);
    
    return 1;
}

int mxf_read_l(MXFFile* mxfFile, uint8_t* llen, uint64_t* len)
{
    int i;
    int c;
    uint64_t length;
    uint8_t llength;
    
    CHK_ORET((c = mxf_file_getc(mxfFile)) != EOF); 

    length = 0;
    llength = 1;
    if (c < 0x80) 
    {
        length = c;
    }
    else 
    {
        uint8_t bytesToRead = ((uint8_t)c) & 0x7f;
        CHK_ORET(bytesToRead <= 8); 
        for (i = 0; i < bytesToRead; i++) 
        {
            CHK_ORET((c = mxf_file_getc(mxfFile)) != EOF); 
            length <<= 8;
            length |= (uint8_t)c;
        }
        llength += bytesToRead;
    }
    
    *llen = llength;
    *len = length;
    
    return 1;
}

int mxf_read_kl(MXFFile* mxfFile, mxfKey* key, uint8_t* llen, uint64_t *len)
{
    CHK_ORET(mxf_read_k(mxfFile, key)); 
    CHK_ORET(mxf_read_l(mxfFile, llen, len));
    
    return 1; 
}


int mxf_read_key(MXFFile* mxfFile, mxfKey* value)
{
    return mxf_read_ul(mxfFile, (mxfUL*)value);
}

int mxf_read_uid(MXFFile* mxfFile, mxfUID* value)
{
    return mxf_read_ul(mxfFile, (mxfUL*)value);
}

int mxf_read_uuid(MXFFile* mxfFile, mxfUUID* value)
{
    return mxf_read_ul(mxfFile, (mxfUL*)value);
}

int mxf_read_local_tag(MXFFile* mxfFile, mxfLocalTag* tag)
{
    return mxf_read_uint16(mxfFile, tag);
}

int mxf_read_local_tl(MXFFile* mxfFile, mxfLocalTag* tag, uint16_t* len)
{
    CHK_ORET(mxf_read_local_tag(mxfFile, tag));
    CHK_ORET(mxf_read_uint16(mxfFile, len));
    
    return 1;
}

int mxf_skip(MXFFile* mxfFile, uint64_t len)
{
    if (mxf_file_is_seekable(mxfFile))
    {
        return mxf_file_seek(mxfFile, len, SEEK_CUR);
    }
    else
    {
        /* skip by reading and discarding data */
        uint8_t buffer[SKIP_BUFFER_SIZE];
        uint32_t numRead;
        uint64_t totalRead = 0;
        while (totalRead < len)
        {
            if (len - totalRead > SKIP_BUFFER_SIZE)
            {
                numRead = SKIP_BUFFER_SIZE;
            }
            else
            {
                numRead = (uint32_t)(len - totalRead);
            }
            if (mxf_file_read(mxfFile, buffer, numRead) != numRead)
            {
                return 0;
            }
            totalRead += numRead;
        }
    
        return 1;
    }
}


int mxf_write_local_tag(MXFFile* mxfFile, mxfLocalTag tag)
{
    return mxf_write_uint16(mxfFile, tag);
}

int mxf_write_local_tl(MXFFile* mxfFile, mxfLocalTag tag, uint16_t len)
{
    CHK_ORET(mxf_write_local_tag(mxfFile, tag));
    CHK_ORET(mxf_write_uint16(mxfFile, len));
    
    return 1;
}

int mxf_write_k(MXFFile* mxfFile, const mxfKey* key)
{
    CHK_ORET(mxf_file_write(mxfFile, (const uint8_t*)key, 16) == 16); 
    
    return 1;
}

uint8_t mxf_write_l(MXFFile* mxfFile, uint64_t len)
{
    uint8_t llen = mxf_get_llen(mxfFile, len);
    
    CHK_ORET(mxf_write_fixed_l(mxfFile, llen, len));
    
    return llen;
}

int mxf_write_kl(MXFFile* mxfFile, const mxfKey* key, uint64_t len)
{
    CHK_ORET(mxf_write_k(mxfFile, key));
    CHK_ORET(mxf_write_l(mxfFile, len));
    
    return 1;
}

int mxf_write_fixed_l(MXFFile* mxfFile, uint8_t llen, uint64_t len)
{
    uint8_t buffer[9];
    uint8_t i;

    assert(llen > 0 && llen <= 9);
    
    if (llen == 1)
    {
        if (len >= 0x80)
        {
            mxf_log_error("Could not write BER length %"PFi64" for llen equal 1" LOG_LOC_FORMAT, len, LOG_LOC_PARAMS);
            return 0;
        }
        
        if (mxf_file_putc(mxfFile, (int)len) != (int)len) 
        {
            return 0;
        }
    }
    else
    {
        if (llen != 9 && (len >> ((llen - 1) * 8)) > 0)
        {
            mxf_log_error("Could not write BER length %"PFu64" for llen equal %u" LOG_LOC_FORMAT, len, llen, LOG_LOC_PARAMS);
            return 0;
        }
        
        for (i = 0; i < llen - 1; i++)
        {
            buffer[llen - 1 - i - 1] = (uint8_t)((len >> (i * 8)) & 0xff);
        }
        CHK_ORET(mxf_file_putc(mxfFile, 0x80 + llen - 1) == 0x80 + llen - 1); 
        CHK_ORET(mxf_file_write(mxfFile, buffer, llen - 1) == (uint8_t)(llen - 1));
    }
    
    return 1;
}

int mxf_write_fixed_kl(MXFFile* mxfFile, const mxfKey* key, uint8_t llen, uint64_t len)
{
    CHK_ORET(mxf_write_k(mxfFile, key));
    CHK_ORET(mxf_write_fixed_l(mxfFile, llen, len));
    
    return 1;
}

int mxf_write_ul(MXFFile* mxfFile, const mxfUL* label)
{
    return mxf_write_k(mxfFile, (const mxfKey*)label);
}

int mxf_write_uid(MXFFile* mxfFile, const mxfUID* uid)
{
    return mxf_write_k(mxfFile, (const mxfKey*)uid);
}

int mxf_write_uuid(MXFFile* mxfFile, const mxfUUID* uuid)
{
    return mxf_write_k(mxfFile, (const mxfKey*)uuid);
}


uint8_t mxf_get_llen(MXFFile* mxfFile, uint64_t len)
{
    uint8_t llen;
    
    if (len < 0x80)
    {
        llen = 1;
    }
    else
    {
        if ((len>>56) != 0)
        {
            llen = 9;
        }
        else if ((len>>48) != 0)
        {
            llen = 8;
        }
        else if ((len>>40) != 0)
        {
            llen = 7;
        }
        else if ((len>>32) != 0)
        {
            llen = 6;
        }
        else if ((len>>24) != 0)
        {
            llen = 5;
        }
        else if ((len>>16) != 0)
        {
            llen = 4;
        }
        else if ((len>>8) != 0)
        {
            llen = 3;
        }
        else
        {
            llen = 2;
        }
    }
    
    if (mxfFile != NULL && mxfFile->minLLen > 0 && llen < mxfFile->minLLen)
    {
        llen = mxfFile->minLLen;
    }

    return llen;
}


int mxf_read_batch_header(MXFFile* mxfFile, uint32_t* len, uint32_t* eleLen)
{
    CHK_ORET(mxf_read_uint32(mxfFile, len));
    CHK_ORET(mxf_read_uint32(mxfFile, eleLen));
    
    return 1;
}

int mxf_write_batch_header(MXFFile* mxfFile, uint32_t len, uint32_t eleLen)
{
    CHK_ORET(mxf_write_uint32(mxfFile, len));
    CHK_ORET(mxf_write_uint32(mxfFile, eleLen));
    
    return 1;
}

int mxf_read_array_header(MXFFile* mxfFile, uint32_t* len, uint32_t* eleLen)
{
    return mxf_read_batch_header(mxfFile, len, eleLen);
}

int mxf_write_array_header(MXFFile* mxfFile, uint32_t len, uint32_t eleLen)
{
    return mxf_write_batch_header(mxfFile, len, eleLen);
}


int mxf_write_zeros(MXFFile* mxfFile, uint64_t len)
{
    static const unsigned char zeros[1024] = {0};
    
    uint64_t completeCount = len / sizeof(zeros);
    uint32_t partialCount = (uint32_t)(len % sizeof(zeros));
    uint64_t i;
    
    for (i = 0; i < completeCount; i++)
    {
        CHK_ORET(mxf_file_write(mxfFile, zeros, sizeof(zeros)) == sizeof(zeros));
    }
    
    if (partialCount > 0)
    {
        CHK_ORET(mxf_file_write(mxfFile, zeros, partialCount) == partialCount);
    }
 
    return 1;
}


int mxf_equals_key(const mxfKey* keyA, const mxfKey* keyB)
{
    return memcmp((const void*)keyA, (const void*)keyB, sizeof(mxfKey)) == 0;
}

int mxf_equals_key_prefix(const mxfKey* keyA, const mxfKey* keyB, size_t cmpLen)
{
    return memcmp((const void*)keyA, (const void*)keyB, cmpLen) == 0;
}

int mxf_equals_key_mod_regver(const mxfKey* keyA, const mxfKey* keyB)
{
    /* ignore difference in octet7, the registry version */
    return memcmp((const void*)keyA, (const void*)keyB, 7) == 0 &&
        memcmp((const void*)&keyA->octet8, (const void*)&keyB->octet8, 8) == 0;
}

int mxf_equals_ul(const mxfUL* labelA, const mxfUL* labelB)
{
    return memcmp((const void*)labelA, (const void*)labelB, sizeof(mxfUL)) == 0;
}

int mxf_equals_ul_mod_regver(const mxfUL* labelA, const mxfUL* labelB)
{
    /* ignore difference in octet7, the registry version */
    return memcmp((const void*)labelA, (const void*)labelB, 7) == 0 &&
        memcmp((const void*)&labelA->octet8, (const void*)&labelB->octet8, 8) == 0;
}

int mxf_equals_uuid(const mxfUUID* uuidA, const mxfUUID* uuidB)
{
    return memcmp((const void*)uuidA, (const void*)uuidB, sizeof(mxfUUID)) == 0;
}

int mxf_equals_uid(const mxfUID* uidA, const mxfUID* uidB)
{
    return memcmp((const void*)uidA, (const void*)uidB, sizeof(mxfUID)) == 0;
}

int mxf_equals_umid(const mxfUMID* umidA, const mxfUMID* umidB)
{
    return memcmp((const void*)umidA, (const void*)umidB, sizeof(mxfUMID)) == 0;
}

/* Note: this function only works if half-swapping is used
   a UL always has the MSB of the 1st byte == 0 and a UUID (non-NCS) has the MSB of the 9th byte == 1 
   The UUID should be half swapped when used where a UL is expected
   Note: the UL is half swapped in AAF AUIDs
   Note: the UL is half swapped in UMIDs when using the UUID/UL material generation method */
int mxf_is_ul(const mxfUID* uid)
{
    return (uid->octet0 & 0x80) == 0x00;
}


void mxf_set_runin_len(MXFFile* mxfFile, uint16_t runinLen)
{
    mxfFile->runinLen = runinLen;
}

uint16_t mxf_get_runin_len(MXFFile* mxfFile)
{
    return mxfFile->runinLen;
}


