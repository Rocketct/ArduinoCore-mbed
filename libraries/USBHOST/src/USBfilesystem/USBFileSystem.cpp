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



#include "USBFileSystem.h"
using namespace mbed;
//Assegnare il nome quindi creare una variabile name
USBFileSystem::USBFileSystem(const char *name, mbed::BlockDevice *bd) {

}

//Assegnare il nome quindi creare una variabile name
USBFileSystem::USBFileSystem(const char *name, const char * path): _root(path)
{

}

USBFileSystem::~USBFileSystem() {

}

int USBFileSystem::mount(mbed::BlockDevice *bd){

}

int USBFileSystem::mount(FATFS* fs, const TCHAR* path, BYTE opt) {
  return _f_mount(fs,path, opt);
}

int USBFileSystem::unmount() {
  const TCHAR* path = getRoot();
  return _f_unmount(path);
}

int USBFileSystem::unmount(const TCHAR* path) {
  return _f_unmount(path);
}

int USBFileSystem::remove(const char *path) {
  return _f_unlink(path);
}
int USBFileSystem::rename(const char *path, const char *newpath) {
  return _f_rename(path, newpath);
}

//file info trovare RRR
// int USBFileSystem::stat(const char *path, FILINFO* fno) {
//   FILINFO* fno;
//   return _f_stat(path, fno);
// }

//aggiungere CH_MODE in basae a mode RRR
int USBFileSystem::mkdir(const char *path, mode_t mode) {
  return _f_mkdir(path);
}

// da implementare? RRR
// int USBFileSystem::statvfs(const char *path, struct statvfs *buf) {

// }

const char * USBFileSystem::getRoot() {
  return _root;
}
void USBFileSystem::setRoot(const char *root){
  _root = root;
}

// RRR
int USBFileSystem::file_open(fs_file_t * file, const char *path, int flags) {
 //BYTE flags decidere se fare un switch per il tipo di flag RRR
  FIL *fil= new FIL;
  int res = _f_open(fil,  path, flags);
  if(res !=FR_OK) {
        file = NULL;
    return res;
  }
  *file = fil;
  return res;
}

int USBFileSystem::file_close(fs_file_t file) {
  FIL *fil = (FIL*)file;
  return _f_close(fil);
}


ssize_t USBFileSystem::file_read(fs_file_t file, void *buffer, size_t size) {
  FIL *fil = static_cast<FIL*>(file);
  UINT len = size;
  UINT byteread = 0;
  FRESULT res =_f_read(fil, buffer, len, &byteread);
  if (res == FR_OK){
    return byteread;
  }
  return 0;
}

ssize_t USBFileSystem::file_write(fs_file_t file, const void *buffer, size_t size) {
  FIL *fil = static_cast<FIL*>(file);
  UINT len = size;
  UINT bytewrite;
  FRESULT res =_f_write(fil, buffer, len, &bytewrite);
  if (res == FR_OK){
    return bytewrite;
  }
  return 0;
}

int USBFileSystem::file_sync(fs_file_t file){
   FIL *fil = static_cast<FIL*>(file);
  return _f_sync(fil);
}

off_t USBFileSystem::file_seek(fs_file_t file, off_t offset, int whence) {
  FIL *fil = static_cast<FIL*>(file);
  return _f_lseek(fil, offset);
}

off_t USBFileSystem::file_tell(fs_file_t file) {
  FIL *fil = static_cast<FIL*>(file);
  return _f_tell(fil);
}

off_t USBFileSystem::file_size(fs_file_t file) {
  FIL *fil = (FIL*)&file;
  return _f_size(fil);
}

int USBFileSystem::file_truncate(fs_file_t file, off_t length) {
  FIL *fil = (FIL*)&file;
  return _f_truncate(fil);
}

int USBFileSystem::dir_open(fs_dir_t *dir, const char *path) {
  _DIR *dr = (_DIR*)dir;
  return _f_opendir(dr, path);
}

int USBFileSystem::dir_close(fs_dir_t dir) {
  _DIR *dr = (_DIR*)&dir;
  return _f_closedir(dr);
}
//(fs_dir_t dir, struct dirent *ent); RRR
ssize_t USBFileSystem::dir_read(fs_dir_t dir, struct dirent *ent) {
  _DIR *dr = (_DIR*)&dir;
  FILINFO *file_info;
  _f_readdir(dr, file_info);

  ent->d_type = (file_info->fattrib & AM_DIR) ? DT_DIR : DT_REG;
  #if FF_USE_LFN
    if (ent->d_name[0] == 0) {
          // No long filename so use short filename.
          strncpy(ent->d_name, file_info->fname, FF_LFN_BUF);
    }
  #else
    strncpy(ent->d_name, file_info->fname, FF_SFN_BUF);
  #endif

  return 1;
}
//
void USBFileSystem::dir_seek(fs_dir_t dir, off_t offset) {
  _DIR *dr = (_DIR*)&dir;
  off_t dptr = (off_t)(dr->dptr);
  FILINFO *file_info;

  _f_readdir(dr, file_info);
  
  if (offset < dptr) {
      _f_rewinddir(dr);
  }
  while (dptr < offset) {
      FILINFO finfo;
      FRESULT res;

      res = _f_readdir(dr, &finfo);
      dptr = dr->dptr;
      if (res != FR_OK) {
          break;
      } else if (finfo.fname[0] == 0) {
        break;
      }
  }
}

off_t USBFileSystem::dir_tell(fs_dir_t dir) {
  _DIR *dr = (_DIR*)&dir;
  return _f_dirtell(dr);
}

void USBFileSystem::dir_rewind(fs_dir_t dir) {
   _DIR *dr = (_DIR*)&dir;
  _f_rewinddir(dr);
}

int USBFileSystem::usb_putc(TCHAR c, FIL* fp){
 return _f_putc(c, fp);
}
int USBFileSystem::usb_puts(const TCHAR* str, FIL* cp)
{
 return  _f_puts(str, cp);
}

int USBFileSystem::usb_printf(FIL* fp, const TCHAR* str){
    return _f_printf(fp,str);
}

TCHAR* USBFileSystem::usb_gets(TCHAR* buff, int len, FIL* fp){
 return _f_gets(buff, len,  fp);
}

