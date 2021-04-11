#include "fatfs.h"

uint8_t retUSER;    /* Return value for USER */
char USERPath[4];   /* USER logical drive path */
FATFS USERFatFS;    /* File system object for USER logical drive */
FIL USERFile;       /* File object for USER */

void FATFS_Init(void)
{
  retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);
}
DWORD get_fattime(void)
{
  return 0;
}
