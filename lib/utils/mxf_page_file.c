/*
 * $Id: mxf_page_file.c,v 1.6 2010/02/12 13:46:27 philipn Exp $
 *
 * 
 *
 * Copyright (C) 2007  Philip de Nier <philipn@users.sourceforge.net>
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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(_WIN32)
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <mxf/mxf.h>
#include <mxf/mxf_page_file.h>

#if defined(_WIN32)
#if !defined(_MSC_VER) || (_MSC_VER < 1400)
/* use low-level file I/O */
#define USE_LOW_LEVEL_IO 1
#else
/* 64-bit ftell and fseek were introduced in Visual C++ 2005 */
#define fseeko _fseeki64
#define ftello _ftelli64
#define fstat64 _fstati64
#define USE_STATI64 1
#endif
#else
#define fstat64 fstat
#endif



#define MAX_FILE_DESCRIPTORS        32

#define PAGE_ALLOC_INCR             64


typedef enum
{
    READ_MODE,
    WRITE_MODE,
    MODIFY_MODE
} FileMode;

typedef struct FileDescriptor
{
    struct FileDescriptor* prev;
    struct FileDescriptor* next;
    
    struct Page* page;
    
#if defined(USE_LOW_LEVEL_IO)
    int fileId;
#else
    FILE* file;
#endif
} FileDescriptor;

typedef struct Page
{
    int wasRemoved;
    
    FileDescriptor* fileDescriptor;
    int wasOpenedBefore;
    int index;
    
    int64_t size;
    int64_t offset;
} Page;

struct MXFPageFile
{
    MXFFile* mxfFile;
};

struct MXFFileSysData
{
    MXFPageFile mxfPageFile;
    
    int64_t pageSize;
    FileMode mode;
    char* filenameTemplate;
    
    int64_t position;
    
    Page* pages;
    int numPages;
    int numPagesAllocated;

    FileDescriptor* fileDescriptorHead;
    FileDescriptor* fileDescriptorTail;
    int numFileDescriptors;
};


static void disk_file_close(FileDescriptor* fileDesc)
{
#if defined(USE_LOW_LEVEL_IO)
    if (fileDesc->fileId >= 0)
    {
        close(fileDesc->fileId);
        fileDesc->fileId = -1;
    }
#else
    if (fileDesc->file != NULL)
    {
        fclose(fileDesc->file);
        fileDesc->file = NULL;
    }
#endif
}

static uint32_t disk_file_read(FileDescriptor* fileDesc, uint8_t* data, uint32_t count)
{
#if defined(USE_LOW_LEVEL_IO)
    return read(fileDesc->fileId, data, count);
#else
    return (uint32_t)fread(data, 1, count, fileDesc->file);
#endif
}

static uint32_t disk_file_write(FileDescriptor* fileDesc, const uint8_t* data, uint32_t count)
{
#if defined(USE_LOW_LEVEL_IO)
    return write(fileDesc->fileId, data, count);
#else
    return (uint32_t)fwrite(data, 1, count, fileDesc->file);
#endif
}

static int disk_file_seek(FileDescriptor* fileDesc, int64_t offset, int whence)
{
#if defined(USE_LOW_LEVEL_IO)
    return _lseeki64(fileDesc->fileId, offset, whence) != -1;
#else
    return fseeko(fileDesc->file, offset, whence) == 0;
#endif
}

static int64_t disk_file_size(const char* filename)
{
#if defined(_WIN32)
    struct _stati64 statBuf;
#else
    struct stat statBuf;
#endif

#if defined(_WIN32)
    if (_stati64(filename, &statBuf) != 0) 
    {
        return -1;
    }
    return statBuf.st_size;
#else
    if (stat(filename, &statBuf) != 0) 
    {
        return -1;
    }
    return statBuf.st_size;
#endif
}



static int open_file(MXFFileSysData* sysData, Page* page)
{
#if defined(USE_LOW_LEVEL_IO)
    int newFile = -1;
#else
    FILE* newFile = NULL;
#endif    
    char filename[4096];
    FileDescriptor* newFileDescriptor = NULL;

    if (page->wasRemoved)
    {
        mxf_log_warn("Failed to open mxf page file which was removed after truncation\n");
        return 0;
    }

    /* move file descriptor to tail if already open, and return */    
    if (page->fileDescriptor != NULL)
    {
        if (page->fileDescriptor == sysData->fileDescriptorTail)
        {
            return 1;
        }
        
        /* extract file descriptor */
        if (page->fileDescriptor->next != NULL)
        {
            page->fileDescriptor->next->prev = page->fileDescriptor->prev;
        }
        if (page->fileDescriptor->prev != NULL)
        {
            page->fileDescriptor->prev->next = page->fileDescriptor->next;
        }
        if (sysData->fileDescriptorHead == page->fileDescriptor)
        {
            sysData->fileDescriptorHead = page->fileDescriptor->next;
        }
        
        /* put file descriptor at tail */
        page->fileDescriptor->next = NULL;
        page->fileDescriptor->prev = sysData->fileDescriptorTail;
        if (sysData->fileDescriptorTail != NULL)
        {
            sysData->fileDescriptorTail->next = page->fileDescriptor;
        }
        sysData->fileDescriptorTail = page->fileDescriptor;
        
        return 1;
    }
    
    
    /* close the least used file descriptor (the head) if too many file descriptors are open */
    if (sysData->numFileDescriptors >= MAX_FILE_DESCRIPTORS)
    {
        if (sysData->fileDescriptorTail == sysData->fileDescriptorHead)
        {
            /* single file descriptor */
            
            sysData->fileDescriptorHead->page->fileDescriptor = NULL;
            disk_file_close(sysData->fileDescriptorHead);
            SAFE_FREE(&sysData->fileDescriptorHead);
            
            sysData->fileDescriptorHead = NULL;
            sysData->fileDescriptorTail = NULL;
            sysData->numFileDescriptors--;
        }
        else
        {
            /* multiple file descriptors */
            
            FileDescriptor* newHead = sysData->fileDescriptorHead->next;
            
            sysData->fileDescriptorHead->page->fileDescriptor = NULL;
            disk_file_close(sysData->fileDescriptorHead);
            SAFE_FREE(&sysData->fileDescriptorHead);
    
            sysData->fileDescriptorHead = newHead;
            newHead->prev = NULL;
            sysData->numFileDescriptors--;
        }
    }
    
    /* open the file */
    sprintf(filename, sysData->filenameTemplate, page->index);
    switch (sysData->mode)
    {
        case READ_MODE:
#if defined(USE_LOW_LEVEL_IO)
            newFile = open(filename, _O_BINARY | _O_RDONLY);
#else
            newFile = fopen(filename, "rb");
#endif
            break;
        case WRITE_MODE:
            if (!page->wasOpenedBefore)
            {
#if defined(USE_LOW_LEVEL_IO)
                newFile = open(filename, _O_BINARY | _O_RDWR | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
#else
                newFile = fopen(filename, "w+b");
#endif
            }
            else
            {
#if defined(USE_LOW_LEVEL_IO)
                newFile = open(filename, _O_BINARY | _O_RDWR);
#else
                newFile = fopen(filename, "r+b");
#endif
            }
            break;
        case MODIFY_MODE:
#if defined(USE_LOW_LEVEL_IO)
            newFile = open(filename, _O_BINARY | _O_RDWR);
            if (newFile < 0)
            {
                newFile = open(filename, _O_BINARY | _O_RDWR | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
            }
#else
            newFile = fopen(filename, "r+b");
            if (newFile == NULL)
            {
                newFile = fopen(filename, "w+b");
            }
#endif
            break;
    }
#if defined(USE_LOW_LEVEL_IO)
    if (newFile < 0)
#else
    if (newFile == NULL)
#endif
    {
        mxf_log_error("Failed to open paged mxf file '%s': %s\n", filename, strerror(errno));
        return 0;
    }
    
    /* create the new file descriptor */
    CHK_MALLOC_OFAIL(newFileDescriptor, FileDescriptor);
    memset(newFileDescriptor, 0, sizeof(*newFileDescriptor));
#if defined(USE_LOW_LEVEL_IO)
    newFileDescriptor->fileId = newFile;
    newFile = -1;
#else
    newFileDescriptor->file = newFile;
    newFile = NULL;
#endif
    newFileDescriptor->page = page;
    
    page->fileDescriptor = newFileDescriptor;
    page->wasOpenedBefore = 1;
    page->offset = 0;
    
    if (sysData->fileDescriptorTail != NULL)
    {
        sysData->fileDescriptorTail->next = newFileDescriptor;
    }
    newFileDescriptor->prev = sysData->fileDescriptorTail;
    sysData->fileDescriptorTail = newFileDescriptor;
    if (sysData->fileDescriptorHead == NULL)
    {
        sysData->fileDescriptorHead = newFileDescriptor;
    }
    sysData->numFileDescriptors++;

    
    return 1;
    
fail:
#if defined(USE_LOW_LEVEL_IO)
    if (newFile >= 0)
    {
        close(newFile);
    }
#else
    if (newFile != NULL)
    {
        fclose(newFile);
    }
#endif        
    return 0;
}

static Page* open_page(MXFFileSysData* sysData, int64_t position)
{
    int i;
    int page;
    
    page = (int)(position / sysData->pageSize);
    if (page > sysData->numPages)
    {
        /* only allowed to open pages 0 .. last + 1 */
        return 0;
    }
    
    if (page == sysData->numPages)
    {
        if (sysData->mode == READ_MODE)
        {
            /* no more pages to open */
            return 0;
        }

        if (sysData->numPages == sysData->numPagesAllocated)
        {
            /* reallocate the pages */
            
            Page* newPages;
            CHK_MALLOC_ARRAY_ORET(newPages, Page, sysData->numPagesAllocated + PAGE_ALLOC_INCR);
            memcpy(newPages, sysData->pages, sizeof(Page) * sysData->numPagesAllocated);
            SAFE_FREE(&sysData->pages);
            sysData->pages = newPages;
            sysData->numPagesAllocated += PAGE_ALLOC_INCR;
            
            /* reset the link back from file descriptors to the new pages */
            for (i = 0; i < sysData->numPages; i++)
            {
                if (sysData->pages[i].fileDescriptor != NULL)
                {
                    sysData->pages[i].fileDescriptor->page = &sysData->pages[i];
                }
            }
        }
        
        /* set new page data */
        memset(&sysData->pages[sysData->numPages], 0, sizeof(Page));
        sysData->pages[sysData->numPages].index = sysData->numPages;
        
        sysData->numPages++;
    }
    
    /* open the file */
    if (!open_file(sysData, &sysData->pages[page]))
    {
        return 0;
    }
    
    return &sysData->pages[page];
}

static uint32_t read_from_page(MXFFileSysData* sysData, uint8_t* data, uint32_t count)
{
    uint32_t numRead;
    Page* page;
    
    page = open_page(sysData, sysData->position);
    if (page == 0)
    {
        return 0;
    }
    
    if (sysData->position > sysData->pageSize * page->index + page->size)
    {
        /* can't read beyond the end of the data in the page */
        /* TODO: assertion here? */
        return 0;
    }
    
    /* set the file at the current position */
    if (page->offset < 0 ||
        sysData->position != sysData->pageSize * page->index + page->offset)
    {
        int64_t offset = sysData->position - sysData->pageSize * page->index;
        if (!disk_file_seek(page->fileDescriptor, offset, SEEK_SET))
        {
            page->offset = -1; /* invalidate the position within the page */
            return 0;
        }
        page->offset = offset;
        page->size = (page->offset > page->size) ? page->offset : page->size;
    }
    
    /* read count bytes or 'till the end of the page */
    numRead = (count > (uint32_t)(sysData->pageSize - page->offset)) ? 
        (uint32_t)(sysData->pageSize - page->offset) : count;
    numRead = disk_file_read(page->fileDescriptor, data, numRead);
    
    page->offset += numRead;
    page->size = (page->offset > page->size) ? page->offset : page->size;

    sysData->position += numRead;
    
    return numRead;
}

static uint32_t write_to_page(MXFFileSysData* sysData, const uint8_t* data, uint32_t count)
{
    uint32_t numWrite;
    Page* page;
    
    page = open_page(sysData, sysData->position);
    if (page == 0)
    {
        return 0;
    }

    if (sysData->position > sysData->pageSize * page->index + page->size)
    {
        /* can't write from beyond the end of the data in the page */
        /* TODO: assertion here? */
        return 0;
    }
    
    /* set the file at the current position */
    if (page->offset < 0 ||
        sysData->position != sysData->pageSize * page->index + page->offset)
    {
        int64_t offset = sysData->position - sysData->pageSize * page->index;
        if (!disk_file_seek(page->fileDescriptor, offset, SEEK_SET))
        {
            page->offset = -1; /* invalidate the position within the page */
            return 0;
        }
        page->offset = offset;
        page->size = (page->offset > page->size) ? page->offset : page->size;
    }
    
    /* write count bytes or 'till the end of the page */
    numWrite = (count > (uint32_t)(sysData->pageSize - page->offset)) ? 
        (uint32_t)(sysData->pageSize - page->offset) : count;
    numWrite = disk_file_write(page->fileDescriptor, data, numWrite);

    page->offset += numWrite;
    page->size = (page->offset > page->size) ? page->offset : page->size;
    
    
    sysData->position += numWrite;
    
    return numWrite;
}





static void free_page_file(MXFFileSysData* sysData)
{
    if (sysData == NULL)
    {
        return;
    }
    
    free(sysData);
}

static void page_file_close(MXFFileSysData* sysData)
{
    FileDescriptor* fd;
    FileDescriptor* nextFd;

    if (sysData == NULL)
    {
        return;
    }
    
    SAFE_FREE(&sysData->filenameTemplate);
    
    SAFE_FREE(&sysData->pages);
    sysData->numPages = 0;
    sysData->numPagesAllocated = 0;
    
    fd = sysData->fileDescriptorHead;
    while (fd != NULL)
    {
        disk_file_close(fd);
        nextFd = fd->next;
        SAFE_FREE(&fd);
        fd = nextFd;
    }
    sysData->fileDescriptorHead = NULL;
    sysData->fileDescriptorTail = NULL;
    sysData->numFileDescriptors = 0;
    
    sysData->position = 0;
}

static int64_t page_file_size(MXFFileSysData* sysData)
{
    if (sysData == NULL)
    {
        return -1;
    }
    
    if (sysData->numPages == 0)
    {
        return 0;
    }
    
    return sysData->pageSize * (sysData->numPages - 1) + sysData->pages[sysData->numPages - 1].size;
}

static uint32_t page_file_read(MXFFileSysData* sysData, uint8_t* data, uint32_t count)
{
    uint32_t numRead = 0;
    uint32_t totalRead = 0;
    while (totalRead < count)
    {
        numRead = read_from_page(sysData, &data[totalRead], count - totalRead);
        totalRead += numRead;
        
        if (numRead == 0)
        {
            break;
        }
    }

    return totalRead;
}

static uint32_t page_file_write(MXFFileSysData* sysData, const uint8_t* data, uint32_t count)
{
    uint32_t numWrite = 0;
    uint32_t totalWrite = 0;
    while (totalWrite < count)
    {
        numWrite = write_to_page(sysData, &data[totalWrite], count - totalWrite);
        totalWrite += numWrite;
        
        if (numWrite == 0)
        {
            break;
        }
    }

    return totalWrite;
}

static int page_file_getchar(MXFFileSysData* sysData)
{
    uint8_t data[1];
    
    if (read_from_page(sysData, &data[0], 1) == 0)
    {
        return EOF;
    }
    
    return (int)data[0];
}

static int page_file_putchar(MXFFileSysData* sysData, int c)
{
    uint8_t data[1];
    data[0] = (char)c;
    
    if (write_to_page(sysData, &data[0], 1) == 0)
    {
        return EOF;
    }
    
    return c;
}

static int page_file_eof(MXFFileSysData* sysData)
{
    int64_t size = page_file_size(sysData);
    if (size < 0)
    {
        return 1;
    }
    
    return sysData->position >= size;
}

static int page_file_seek(MXFFileSysData* sysData, int64_t offset, int whence)
{
    int64_t position;
    int64_t size;
    
    size = page_file_size(sysData);
    if (size < 0)
    {
        return 0;
    }
    
    switch (whence)
    {
        case SEEK_SET:
            position = offset;
            break;
        case SEEK_CUR:
            position = sysData->position + offset;
            break;
        case SEEK_END:
            position = size + offset;
            break;
        default:
            position = sysData->position;
            break;
    }
    if (position < 0 || position > size)
    {
        return 0;
    }
    
    sysData->position = position;
    
    return 1;
}

static int64_t page_file_tell(MXFFileSysData* sysData)
{
    return sysData->position;
}

static int page_file_is_seekable(MXFFileSysData* sysData)
{
    return sysData != NULL;
}



int mxf_page_file_open_new(const char* filenameTemplate, int64_t pageSize, MXFPageFile** mxfPageFile)
{
    MXFFile* newMXFFile = NULL;
    
    if (strstr(filenameTemplate, "%d") == NULL)
    {
        mxf_log_error("Filename template '%s' doesn't contain %%d\n", filenameTemplate);
        return 0;
    }
    
    CHK_MALLOC_ORET(newMXFFile, MXFFile);
    memset(newMXFFile, 0, sizeof(*newMXFFile));

    newMXFFile->close = page_file_close;
    newMXFFile->read = page_file_read;
    newMXFFile->write = page_file_write;
    newMXFFile->get_char = page_file_getchar;
    newMXFFile->put_char = page_file_putchar;
    newMXFFile->eof = page_file_eof;
    newMXFFile->seek = page_file_seek;
    newMXFFile->tell = page_file_tell;
    newMXFFile->is_seekable = page_file_is_seekable;
    newMXFFile->size = page_file_size;
    newMXFFile->free_sys_data = free_page_file;
    
    
    CHK_MALLOC_OFAIL(newMXFFile->sysData, MXFFileSysData);
    memset(newMXFFile->sysData, 0, sizeof(*newMXFFile->sysData));
    
    CHK_MALLOC_ARRAY_OFAIL(newMXFFile->sysData->filenameTemplate, char, strlen(filenameTemplate) + 1);
    strcpy(newMXFFile->sysData->filenameTemplate, filenameTemplate);
    newMXFFile->sysData->pageSize = pageSize;
    newMXFFile->sysData->mode = WRITE_MODE;
    newMXFFile->sysData->mxfPageFile.mxfFile = newMXFFile;
    
    
    *mxfPageFile = &newMXFFile->sysData->mxfPageFile;
    return 1;
    
fail:
    if (newMXFFile != NULL)
    {
        mxf_file_close(&newMXFFile);
    }
    return 0;
}

int mxf_page_file_open_read(const char* filenameTemplate, MXFPageFile** mxfPageFile)
{
    MXFFile* newMXFFile = NULL;
    int pageCount;
    int allocatedPages;
    char filename[4096];
    FILE* file;
    struct stat st;
    
    
    if (strstr(filenameTemplate, "%d") == NULL)
    {
        mxf_log_error("Filename template '%s' doesn't contain %%d\n", filenameTemplate);
        return 0;
    }
    
    /* count number of page files */
    pageCount = 0;
    for(;;)
    {
        sprintf(filename, filenameTemplate, pageCount);
        if ((file = fopen(filename, "rb")) == NULL)
        {
            break;
        }
        fclose(file);
        pageCount++;
    }
    
    if (pageCount == 0)
    {
        /* file not found */
        return 0;
    }

    
    CHK_MALLOC_ORET(newMXFFile, MXFFile);
    memset(newMXFFile, 0, sizeof(*newMXFFile));

    newMXFFile->close = page_file_close;
    newMXFFile->read = page_file_read;
    newMXFFile->write = page_file_write;
    newMXFFile->get_char = page_file_getchar;
    newMXFFile->put_char = page_file_putchar;
    newMXFFile->eof = page_file_eof;
    newMXFFile->seek = page_file_seek;
    newMXFFile->tell = page_file_tell;
    newMXFFile->is_seekable = page_file_is_seekable;
    newMXFFile->size = page_file_size;
    newMXFFile->free_sys_data = free_page_file;

    
    CHK_MALLOC_OFAIL(newMXFFile->sysData, MXFFileSysData);
    memset(newMXFFile->sysData, 0, sizeof(*newMXFFile->sysData));
    
    CHK_MALLOC_ARRAY_OFAIL(newMXFFile->sysData->filenameTemplate, char, strlen(filenameTemplate) + 1);
    strcpy(newMXFFile->sysData->filenameTemplate, filenameTemplate);
    newMXFFile->sysData->mode = READ_MODE;
    newMXFFile->sysData->mxfPageFile.mxfFile = newMXFFile;
    
    
    
    /* get the page size from the first file */
    sprintf(filename, filenameTemplate, 0);
    if (stat(filename, &st) != 0)
    {
        mxf_log_error("Failed to stat file '%s': %s\n", filename, strerror(errno));
        goto fail;
    }
    newMXFFile->sysData->pageSize = st.st_size;
    
    /* allocate pages */
    allocatedPages = (pageCount < PAGE_ALLOC_INCR) ? PAGE_ALLOC_INCR : pageCount;
    CHK_MALLOC_ARRAY_ORET(newMXFFile->sysData->pages, Page, allocatedPages);
    memset(newMXFFile->sysData->pages, 0, allocatedPages * sizeof(Page));
    newMXFFile->sysData->numPages = pageCount;
    newMXFFile->sysData->numPagesAllocated = allocatedPages;
    for (pageCount = 0; pageCount < newMXFFile->sysData->numPages; pageCount++)
    {
        newMXFFile->sysData->pages[pageCount].index = pageCount;
        newMXFFile->sysData->pages[pageCount].size = newMXFFile->sysData->pageSize;
    }
    
    /* set the file size of the last file, which could be less than newMXFFile->sysData->pageSize */
    sprintf(filename, filenameTemplate, newMXFFile->sysData->numPages - 1);
    if (stat(filename, &st) != 0)
    {
        mxf_log_error("Failed to stat file '%s': %s\n", filename, strerror(errno));
        goto fail;
    }
    newMXFFile->sysData->pages[newMXFFile->sysData->numPages - 1].size = st.st_size;

    
    *mxfPageFile = &newMXFFile->sysData->mxfPageFile;
    return 1;
    
fail:
    if (newMXFFile != NULL)
    {
        mxf_file_close(&newMXFFile);
    }
    return 0;
}

int mxf_page_file_open_modify(const char* filenameTemplate, int64_t pageSize, MXFPageFile** mxfPageFile)
{
    MXFFile* newMXFFile = NULL;
    int pageCount;
    int allocatedPages;
    char filename[4096];
    FILE* file;
    int64_t fileSize;
    
    
    if (strstr(filenameTemplate, "%d") == NULL)
    {
        mxf_log_error("Filename template '%s' doesn't contain %%d\n", filenameTemplate);
        return 0;
    }

    /* count number of page files */
    pageCount = 0;
    for(;;)
    {
        sprintf(filename, filenameTemplate, pageCount);
        if ((file = fopen(filename, "rb")) == NULL)
        {
            break;
        }
        fclose(file);
        pageCount++;
    }
    
    if (pageCount == 0)
    {
        /* file not found */
        return 0;
    }

    /* check the size of the first file equals the pageSize */
    if (pageCount > 1)
    {
        sprintf(filename, filenameTemplate, 0);
        fileSize = disk_file_size(filename);
        if (fileSize < 0)
        {
            mxf_log_error("Failed to stat file '%s': %s\n", filename, strerror(errno));
            return 0;
        }
        if (pageSize != fileSize)
        {
            mxf_log_error("Size of first file '%s' (%"PFi64" does not equal page size %"PFi64"\n", filename, fileSize, pageSize);
            return 0;
        }
    }
    
    
    CHK_MALLOC_ORET(newMXFFile, MXFFile);
    memset(newMXFFile, 0, sizeof(*newMXFFile));

    newMXFFile->close = page_file_close;
    newMXFFile->read = page_file_read;
    newMXFFile->write = page_file_write;
    newMXFFile->get_char = page_file_getchar;
    newMXFFile->put_char = page_file_putchar;
    newMXFFile->eof = page_file_eof;
    newMXFFile->seek = page_file_seek;
    newMXFFile->tell = page_file_tell;
    newMXFFile->is_seekable = page_file_is_seekable;
    newMXFFile->size = page_file_size;
    newMXFFile->free_sys_data = free_page_file;
    
    
    CHK_MALLOC_OFAIL(newMXFFile->sysData, MXFFileSysData);
    memset(newMXFFile->sysData, 0, sizeof(*newMXFFile->sysData));
    
    CHK_MALLOC_ARRAY_OFAIL(newMXFFile->sysData->filenameTemplate, char, strlen(filenameTemplate) + 1);
    strcpy(newMXFFile->sysData->filenameTemplate, filenameTemplate);
    newMXFFile->sysData->pageSize = pageSize;
    newMXFFile->sysData->mode = MODIFY_MODE;
    newMXFFile->sysData->mxfPageFile.mxfFile = newMXFFile;
    
    
    /* allocate pages */
    allocatedPages = (pageCount < PAGE_ALLOC_INCR) ? PAGE_ALLOC_INCR : pageCount;
    CHK_MALLOC_ARRAY_ORET(newMXFFile->sysData->pages, Page, allocatedPages);
    memset(newMXFFile->sysData->pages, 0, allocatedPages * sizeof(Page));
    newMXFFile->sysData->numPages = pageCount;
    newMXFFile->sysData->numPagesAllocated = allocatedPages;
    for (pageCount = 0; pageCount < newMXFFile->sysData->numPages; pageCount++)
    {
        newMXFFile->sysData->pages[pageCount].index = pageCount;
        newMXFFile->sysData->pages[pageCount].size = pageSize;
    }
    
    /* set the files size of the last file, which could have size < pageSize */
    sprintf(filename, filenameTemplate, newMXFFile->sysData->numPages - 1);
    fileSize = disk_file_size(filename);
    if (fileSize < 0)
    {
        mxf_log_error("Failed to stat file '%s': %s\n", filename, strerror(errno));
        goto fail;
    }
    newMXFFile->sysData->pages[newMXFFile->sysData->numPages - 1].size = fileSize;
    
    
    *mxfPageFile = &newMXFFile->sysData->mxfPageFile;
    return 1;
    
fail:
    if (newMXFFile != NULL)
    {
        mxf_file_close(&newMXFFile);
    }
    return 0;
}

MXFFile* mxf_page_file_get_file(MXFPageFile* mxfPageFile)
{
    return mxfPageFile->mxfFile;
}

int64_t mxf_page_file_get_page_size(MXFPageFile* mxfPageFile)
{
    return mxfPageFile->mxfFile->sysData->pageSize;
}

int mxf_page_file_is_page_filename(const char* filename)
{
    return strstr(filename, "%d") != NULL;
}

int mxf_page_file_forward_truncate(MXFPageFile* mxfPageFile)
{
    MXFFileSysData* sysData = mxfPageFile->mxfFile->sysData;
    int page = (int)(sysData->position / sysData->pageSize);
    int i;
    char filename[4096];
#if defined(_WIN32)
    int fileid;
#endif
    
    if (sysData->mode == READ_MODE)
    {
        mxf_log_error("Cannot forward truncate read-only mxf page file\n");
        return 0;
    }
    
    /* close and truncate to zero length page files before the current one */
    for (i = 0; i < page; i++)
    {
        if (sysData->pages[i].wasRemoved)
        {
            continue;
        }
        
        if (sysData->pages[i].fileDescriptor != NULL)
        {
            /* close the file */
            disk_file_close(sysData->pages[i].fileDescriptor);
            
            /* remove the file descriptor from the list */
            if (sysData->fileDescriptorHead == sysData->pages[i].fileDescriptor)
            {
                sysData->fileDescriptorHead = sysData->fileDescriptorHead->next;
            }
            else
            {
                sysData->pages[i].fileDescriptor->prev->next = sysData->pages[i].fileDescriptor->next;
            }
            if (sysData->fileDescriptorTail == sysData->pages[i].fileDescriptor)
            {
                sysData->fileDescriptorTail = sysData->fileDescriptorTail->prev;
            }
            else
            {
                sysData->pages[i].fileDescriptor->next->prev = sysData->pages[i].fileDescriptor->prev;
            }
            SAFE_FREE(&sysData->pages[i].fileDescriptor);
        }

        /* truncate the file to zero length */
        sprintf(filename, sysData->filenameTemplate, sysData->pages[i].index);

#if defined(_WIN32) && defined(_MSC_VER)
        /* WIN32 does not have truncate() so open the file with _O_TRUNC then close it */
        if ((fileid = _open(filename, _O_CREAT | _O_TRUNC, _S_IWRITE)) == -1 ||
            _close(fileid) == -1)
#elif defined(_WIN32)
        /* WIN32 does not have truncate() so open the file with _O_TRUNC then close it */
        if ((fileid = open(filename, _O_CREAT | _O_TRUNC, _S_IWRITE)) == -1 ||
            close(fileid) == -1)
#else
        if (truncate(filename, 0) != 0)
#endif
        {
            mxf_log_warn("Failed to truncate '%s' to zero length: %s\n", filename, strerror(errno));
        }
        sysData->pages[i].wasRemoved = 1;
    }
    
    return 1;
}

int mxf_page_file_remove(const char* filenameTemplate)
{
    int index = 0;
    char filename[4096];
    
    for(;;)
    {
        sprintf(filename, filenameTemplate, index);
        if (remove(filename) != 0)
        {
            break;
        }
        
        index++;
    }
    
    if (index == 0)
    {
        /* first file couldn't be removed or does not exist */
        return 0;
    }
    
    return 1;
}



