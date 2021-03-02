#include "USBHost.h"
//#include "USBfilesystem/USBFileSystem.h"
#include "mbed.h"


static USBHost usb;
static mbed::USBFileSystem usbfs("AAA", "0:");
//static char mo[4];

//REDIRECT_STDOUT_TO(Serial);


char mo[4] = "0:";

static const tusbh_hub_class_t cls_hub = {
  .backend = &tusbh_hub_backend,
};

extern "C" {
  int msc_ff_mount(tusbh_interface_t* interface, int max_lun, const tusbh_block_info_t* blocks);
  int msc_ff_unmount(tusbh_interface_t* interface);
  FRESULT scan_files1(char* path);
}

static const tusbh_msc_class_t cls_msc_bot = {
  .backend = &tusbh_msc_bot_backend,
  .mount = msc_ff_mount,
  .unmount = msc_ff_unmount,
};


static const tusbh_class_reg_t class_table[] = {
  //(tusbh_class_reg_t)&cls_hub,
  (tusbh_class_reg_t)&cls_msc_bot,
  0,
};

void setup()
{
  Serial1.begin(115200);
  usb.Init(USB_CORE_ID_FS, class_table);

}


void loop() {
  //usb.Task();
  if (mount_itf != NULL) {

    FATFS  fs;
    int res =  usbfs.mount(&mount_ff, mo, 1);
    printf("MSC mount [%s] r = %d\n", mo, res);
    mbed::fs_file_t fsrc;
    delay(5000);
    res = usbfs.file_open(&fsrc, "0://preso.txt", FA_WRITE | FA_CREATE_ALWAYS );
    printf("open res = %d\n", res);
    delay(100);

    res = usbfs.file_write(fsrc, "provacat", 8);
    printf("\nwrite res= %d\n", res);
    delay(100);
    res = usbfs.file_close(fsrc);
    delay(100);
    printf("close res= %d\n", res);
    //delay(10000);
    mount_itf = NULL;

    printf(" itf");
  }
}


#define stdin_recvchar  Serial1.write

extern "C" {
  int msc_ff_mount(tusbh_interface_t* interface, int max_lun, const tusbh_block_info_t* blocks)
  {
    //SCB_DisableDCache();
    //    for (int i = 0; i < MAX_FF; i++) {
    //      if (mount_itf[i] == 0) {
    // intf = interface;
    mount_itf = interface;
    //printf(" i = %d\n", );
    //printf(" interface id = %d\n", intf);


    //    char mo[4] = "0:";
    // mo[0] += 1;
    //usbfs.getRoot().toCharArray(mo, 4);


    //        int res =  usbfs.mount(&mount_ff, mo, 1);
    //        printf("MSC mount [%s] r = %d\n", mo, res);
    //
    //    initusb = true;
    // break;
    //}
    //}

    return 0;
  }
  //    }
  //    printf("MSC mount fail, no space\n");
  //    return 1;



  int msc_ff_unmount(tusbh_interface_t* interface)
  {
    //    for (int i = 0; i < MAX_FF; i++) {
    //      if (mount_itf[i] == interface) {
    char mo[4] = "0:";
    //usbfs.getRoot().toCharArray(mo, 4);
    mo[0] += 0;
    FRESULT r = _f_mount(0, mo, 0);
    printf("MSC unmount [%s] r = %d\n", mo, r);
    mount_itf = 0;
    //        return 1;
    //      }
    //    }
    //    printf("MSC unmount fail, not found\n");
    //    return 0;
  }

  /*-----------------------------------------------------------------------*/
  /* Inidialize a Drive                                                    */
  /*-----------------------------------------------------------------------*/

  DSTATUS _disk_initialize (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
  )
  {
    //  if (pdrv < MAX_FF) {
    if (mount_itf) {

      if (tusbh_msc_is_unit_ready(mount_itf, 0)) {
        return 0;
      }
      return STA_NOINIT;
    }
    //}
    return STA_NODISK;
  }



  /*-----------------------------------------------------------------------*/
  /* Read Sector(s)                                                        */
  /*-----------------------------------------------------------------------*/

  DRESULT _disk_read (
    BYTE pdrv,    /* Physical drive nmuber to identify the drive */
    BYTE * buff,  /* Data buffer to store read data */
    LBA_t sector, /* Start sector in LBA */
    UINT count    /* Number of sectors to read */
  )
  {

    //if (pdrv < MAX_FF) {
    // tusbh_interface_t* itf = mount_itf[pdrv];

    if (mount_itf) {
      int res = tusbh_msc_block_read(mount_itf, 0, sector, count, buff);
      if (res < 0) {
        return RES_ERROR;
      }
      return RES_OK;
    }
    //}
    return RES_PARERR;
  }



  /*-----------------------------------------------------------------------*/
  /* Write Sector(s)                                                       */
  /*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

  DRESULT _disk_write (
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    const BYTE *buff, /* Data to be written */
    LBA_t sector,   /* Start sector in LBA */
    UINT count      /* Number of sectors to write */
  )
  {
    //if (pdrv < MAX_FF) {
    // tusbh_interface_t* itf = mount_itf[pdrv];
    if (mount_itf) {
      int  res = tusbh_msc_block_write(mount_itf, 0, sector, count, (void*)buff);
      if (res < 0) {
        return RES_ERROR;
      }
      return RES_OK;
    }
    //}
    return RES_PARERR;
  }

#endif



  /*-----------------------------------------------------------------------*/
  /* Miscellaneous Functions                                               */
  /*-----------------------------------------------------------------------*/

  DRESULT _disk_ioctl (
    BYTE pdrv,    /* Physical drive nmuber (0..) */
    BYTE cmd,   /* Control code */
    void *buff    /* Buffer to send/receive control data */
  )
  {
    //if (pdrv >= MAX_FF) return RES_PARERR;
    //tusbh_interface_t* interface = mount_itf[pdrv];
    if (!mount_itf)return RES_NOTRDY;
    tusbh_msc_info_t* info = tusbh_get_info(mount_itf, tusbh_msc_info_t);
    switch (cmd) {
      case CTRL_SYNC:
        return RES_OK;
      case GET_SECTOR_COUNT:
        *(LBA_t*)buff = info->blocks[0].block_count;
        return RES_OK;
      case GET_SECTOR_SIZE:
        *(DWORD*)buff = info->blocks[0].block_size;
        return RES_OK;
      case GET_BLOCK_SIZE:
        *(DWORD*)buff = info->blocks[0].block_size;
        return RES_OK;
    }
    return RES_PARERR;
  }

  FRESULT scan_files1(char* path)
  {
    FRESULT res;
    _DIR dir;
    UINT i;
    static FILINFO fno;
    res = _f_opendir(&dir, path);
    if (res == FR_OK) {
      for (;;) {
        res = _f_readdir(&dir, &fno);                   /* Read a directory item */
        if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        if (fno.fattrib & AM_DIR) {                    /* It is a directory */
          i = strlen(path);
          sprintf(&path[i], "/%s", fno.fname);
          res = scan_files1(path);                    /* Enter the directory */
          if (res != FR_OK) break;
          path[i] = 0;
        } else {                                       /* It is a file. */
          printf("%s/%s\n", path, fno.fname);
        }
      }
      _f_closedir(&dir);
    } else {
      printf("Fail to open dir %s, res = %d\n", path, res);
    }
    return res;
  }


  DSTATUS _disk_status (
    BYTE pdrv   /* Physical drive nmuber to identify the drive */
  )
  {
    // if (pdrv < MAX_FF) {
    if (mount_itf) {
      return 0;
    }
    //}
    return STA_NODISK;
  }


}

//
//  void cmd_ls(char* argv[], int argc)
//  {
//    if (argc < 2) {
//      printf("ls <dir>\n");
//      return;
//    }
//    scan_files1(argv[1]);
//  }
//
//  void cmd_mkdir(char* argv[], int argc)
//  {
//    if (argc < 2) {
//      printf("mkdir <dir>\n");
//      return;
//    }
//    FRESULT res = _f_mkdir(argv[1]);
//    if (res != FR_OK) {
//      printf("Fail to execute _f_mkdir %d\n", res);
//    }
//  }
//
//  void cmd_mv(char* argv[], int argc)
//  {
//    if (argc < 3) {
//      printf("mv <from> <to>\n");
//      return;
//    }
//    FRESULT res = _f_rename(argv[1], argv[2]);
//    if (res != FR_OK) {
//      printf("Fail to execute _f_rename %d\n", res);
//    }
//  }
//
//  void cmd_cat(char* argv[], int argc)
//  {
//    if (argc < 2) {
//      printf("cat <file>\n");
//      return;
//    }
//    FIL fp;
//    FRESULT res = _f_open(&fp, argv[1], FA_READ);
//    if (res != FR_OK) {
//      printf("Fail to open %s, res = %d\n", argv[1], res);
//    }
//    char line[64];
//    while (_f_gets(line, sizeof(line), &fp)) {
//      printf(line);
//    }
//    printf("\n");
//    _f_close(&fp);
//  }
//
//  void cmd_cp(char* argv[], int argc)
//  {
//    if (argc < 3) {
//      printf("cp <source> <dest>\n");
//      return;
//    }
//
//    FIL fsrc, fdst;
//    FRESULT res = _f_open(&fsrc, argv[1], FA_READ);
//    if (res != FR_OK) {
//      printf("Fail to open %s, res = %d\n", argv[1], res);
//      return;
//    }
//
//    res = _f_open(&fdst, argv[2], FA_WRITE | FA_CREATE_ALWAYS);
//    if (res != FR_OK) {
//      printf("Fail to open %s, res = %d\n", argv[2], res);
//      _f_close(&fsrc);
//      return;
//    }
//
//    UINT br, bw;
//    char buffer[64];
//    for (;;) {
//      res = _f_read(&fsrc, buffer, sizeof(buffer), &br);
//      if (res || br == 0) break; /* error or eof */
//      res = _f_write(&fdst, buffer, br, &bw);
//      if (res || bw < br) break; /* error or disk full */
//    }
//
//    _f_close(&fsrc);
//    _f_close(&fdst);
//  }
//
//  void cmd_rm(char* argv[], int argc)
//  {
//    if (argc < 2) {
//      printf("rm <target>\n");
//      return;
//    }
//    FRESULT res = _f_unlink(argv[1]);
//    if (res != FR_OK) {
//      printf("Fail to execute _f_unlink %d\n", res);
//    }
//  }
//
//  void cmd_append(char* argv[], int argc)
//  {
//    if (argc < 3) {
//      printf("append <file> <txt1> [txt2 txt3 ...]\n");
//      return;
//    }
//    FIL fp;
//    FRESULT res = (&fp, argv[1], FA_OPEN_APPEND | FA_WRITE | FA_READ);
//    if (res != FR_OK) {
//      printf("Fail to open %s, res = %d\n", argv[1], res);
//      return;
//    }
//    for (int i = 2; i < argc; i++) {
//      _f_puts(argv[i], &fp);
//    }
//    _f_puts("\n", &fp);
//    _f_close(&fp);
//  }
