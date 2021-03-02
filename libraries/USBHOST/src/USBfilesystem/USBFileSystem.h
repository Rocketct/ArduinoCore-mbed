/* mbed Microcontroller Library
   Copyright (c) 2006-2012 ARM Limited

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

/** \addtogroup storage */
/** @{*/

#ifndef MBED_USBFILESYSTEM_H
#define MBED_USBFILESYSTEM_H

#include "Arduino.h"

#include "features/storage/filesystem/FileSystem.h"
#include "features/storage/blockdevice/BlockDevice.h"

extern  "C" {
#include "USBfilesystem/ff.h"      /* Obtains integer types */
#include "USBfilesystem/diskio.h"   /* Declarations of disk functions */
#include "class/host/tusbh_msc.h"
#include <string.h>
}


#define MAX_FF  4
static tusbh_interface_t* mount_itf= NULL;
static FATFS mount_ff;
static tusbh_interface_t* intf = NULL;

namespace mbed{
/**
   FAT file system based on ChaN's FAT file system library v0.8

   Synchronization level: Thread safe
*/
class USBFileSystem : public FileSystem {
  public:
    USBFileSystem(const char *name = NULL, mbed::BlockDevice *bd = NULL);
    USBFileSystem(const char *name = NULL, const char * path = NULL);
    virtual ~USBFileSystem();
    virtual int mount(BlockDevice *bd = NULL); // da reimplementare
    virtual int mount(FATFS* fs, const TCHAR* path, BYTE opt);
    virtual int unmount(); //RRR da riefinire
    virtual int unmount(const TCHAR* path);
    virtual int remove(const char *path);
    virtual int rename(const char *path, const char *newpath);
    //virtual int stat(const char *path, struct stat *st);
    //virtual int stat(const char *path, FILINFO* fno);
    virtual int mkdir(const char *path, mode_t mode);
    //virtual int statvfs(const char *path, struct statvfs *buf);
    virtual const char * getRoot();
    virtual void setRoot(const char * root);
    const char * _root;

    FRESULT scan_files(char* path);

    virtual int file_open(fs_file_t *file, const char *path, int flags);
    virtual int file_close(fs_file_t file);
    virtual ssize_t file_read(fs_file_t file, void *buffer, size_t size);
    virtual ssize_t file_write(fs_file_t file, const void *buffer, size_t size);
    virtual int file_sync(fs_file_t file);
    virtual off_t file_seek(fs_file_t file, off_t offset, int whence);
    virtual off_t file_tell(fs_file_t file);
    virtual off_t file_size(fs_file_t file);
    virtual int file_truncate(fs_file_t file, off_t length);
    virtual int dir_open(fs_dir_t *dir, const char *path);
    virtual int dir_close(fs_dir_t dir);
    virtual ssize_t dir_read(fs_dir_t dir, struct dirent *ent);
    virtual void dir_seek(fs_dir_t dir, off_t offset);
    virtual off_t dir_tell(fs_dir_t dir);
    virtual void dir_rewind(fs_dir_t dir);
    virtual int usb_putc(TCHAR c, FIL* fp);
    virtual int usb_puts(const TCHAR* str, FIL* cp);
    virtual int usb_printf(FIL* fp, const TCHAR* str);
    virtual TCHAR* usb_gets(TCHAR* buff, int len, FIL* fp);
  private:
    FATFS _fs; // Work area (file system object) for logical drive.
    char _fsid[sizeof("0:")];
    int _id;


  protected:
    //  virtual void lock();
    // virtual void unlock();
};
}


#endif

/** @}*/

// virtual void file_rewind(fs_file_t file);
// virtual size_t dir_size(fs_dir_t dir);