/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    spti.c

Abstract:

    Win32 application that can communicate directly with SCSI devices via
    IOCTLs.  

Author:


Environment:

    User mode.

Notes:


Revision History:

--*/

#ifndef SCSIH
#define SCSIH

#include "DEVIOCTL.h"
#include "NTDDDISK.h"
#include "NTDDSCSI.h"
#include "spti.h"

#define __builtin_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define  CNT   1000000
#define MAX_CMD_LEN 50

SCSI_PASS_THROUGH_WITH_BUFFERS sptwb;
SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER sptdwb;
BYTE find,Pass_Status1,Pass_Status2;
unsigned long Total_Sector;
BYTE Flash_Status,Device_Code;
unsigned char Page_Data[256][528];

char find_device(HANDLE *fileHandle,char Drive,char* str1,unsigned int len1,char* str2,unsigned int len2)
{
   BOOL status = 0;
   DWORD accessMode = 0, shareMode = 0;   
   char devicename[80];
   ULONG length = 0, returned = 0;
   unsigned char *str_ptr1,*str_ptr2;

   strcpy (devicename,"\\\\.\\C:");
   shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;  // default
   accessMode = GENERIC_WRITE | GENERIC_READ;       // default

     devicename[4] = Drive;
  	 *fileHandle = CreateFileA(devicename,
                             accessMode,
                             shareMode,
                             NULL,
                             OPEN_EXISTING,
                             0,
                             NULL);

     if (*fileHandle == INVALID_HANDLE_VALUE) return 0;

     ZeroMemory(&sptwb,sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));
     sptwb.spt.Length = sizeof(SCSI_PASS_THROUGH);
     sptwb.spt.PathId = 0;
     sptwb.spt.TargetId = 1;
     sptwb.spt.Lun = 0;
     sptwb.spt.CdbLength = CDB12GENERIC_LENGTH;
     sptwb.spt.SenseInfoLength = 24;
     sptwb.spt.DataIn = SCSI_IOCTL_DATA_IN;
     sptwb.spt.DataTransferLength = 36;
     sptwb.spt.TimeOutValue = 2;
     sptwb.spt.DataBufferOffset = (unsigned long*) __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucDataBuf);
     sptwb.spt.SenseInfoOffset =
        __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucSenseBuf);
     sptwb.spt.Cdb[0] = SCSIOP_INQUIRY;
     sptwb.spt.Cdb[1] = 0;
     sptwb.spt.Cdb[2] = 0;
     sptwb.spt.Cdb[3] = 0;
     sptwb.spt.Cdb[4] = 36;
     length = __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucDataBuf) +
        sptwb.spt.DataTransferLength;
     status = DeviceIoControl(*fileHandle,
                              IOCTL_SCSI_PASS_THROUGH,
                              &sptwb,
                              sizeof(SCSI_PASS_THROUGH),
                              &sptwb,
                              length,
                              &returned,
                              FALSE);

	 
     if (!status ) return 0;
     if (sptwb.spt.ScsiStatus) return 0;
       
//     PrintStatusResults(status,returned,&sptwb,36);

	 str_ptr1 = ((PUCHAR)sptwb.ucDataBuf)+8;
     str_ptr2 = ((PUCHAR)sptwb.ucDataBuf)+8+8;


     if (!strncmp((const char*)str_ptr1,str1,len1) && !strncmp((const char*)str_ptr2,str2,len2))
     {
        return 1;
     }
	   else
     {
		CloseHandle(*fileHandle);
        *fileHandle = 0;
        return 0;
     }
}




BYTE Read_Cap(HANDLE fileHandle)
{
   BOOL status = 0;
   ULONG length = 0, returned = 0;
   unsigned char *str_ptr;

   ZeroMemory(&sptwb,sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));
   sptwb.spt.Length = sizeof(SCSI_PASS_THROUGH);
   sptwb.spt.PathId = 0;
   sptwb.spt.TargetId = 1;
   sptwb.spt.Lun = 0;
   sptwb.spt.CdbLength = CDB12GENERIC_LENGTH;
   sptwb.spt.SenseInfoLength = 24;
   sptwb.spt.DataIn = SCSI_IOCTL_DATA_IN;
   sptwb.spt.DataTransferLength = 8;
   sptwb.spt.TimeOutValue = 2;
   sptwb.spt.DataBufferOffset = (unsigned long*) __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS, ucDataBuf);
   sptwb.spt.SenseInfoOffset = 
      __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucSenseBuf);
   sptwb.spt.Cdb[0] = 0x25;
   sptwb.spt.Cdb[1] = 0;
   sptwb.spt.Cdb[2] = 0;
   sptwb.spt.Cdb[3] = 0;
   sptwb.spt.Cdb[4] = 0;
   length = __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucDataBuf) + sptwb.spt.DataTransferLength;

   status = DeviceIoControl(fileHandle,
                            IOCTL_SCSI_PASS_THROUGH,
                            &sptwb,
                            sizeof(SCSI_PASS_THROUGH),
                            &sptwb,
                            length,
                            &returned,
                            FALSE);
     
   
   str_ptr = ((PUCHAR)sptwb.ucDataBuf);
//   printf("%d %d %d %d\n",*str_ptr,*(str_ptr+1),*(str_ptr+2),*(str_ptr+3));
//   printf("%d %d %d %d\n",*(str_ptr+4),*(str_ptr+5),*(str_ptr+6),*(str_ptr+7));
   Total_Sector = *(str_ptr);
   Total_Sector = (Total_Sector << 8) | (*(str_ptr+1));
   Total_Sector = (Total_Sector << 8) | (*(str_ptr+2));
   Total_Sector = (Total_Sector << 8) | (*(str_ptr+3));
  // printf("Max Sector number is %u\n",Total_Sector);

   return (status & (!sptwb.spt.ScsiStatus));
}


BYTE WriteBuf(HANDLE fileHandle, unsigned char cnt)
{
   BOOL status = 0;
   ULONG length = 0, returned = 0;

   sptwb.spt.Length = sizeof(SCSI_PASS_THROUGH);
   sptwb.spt.PathId = 0;
   sptwb.spt.TargetId = 1;
   sptwb.spt.Lun = 0;
   sptwb.spt.CdbLength = CDB12GENERIC_LENGTH;
   sptwb.spt.SenseInfoLength = 24;
   sptwb.spt.DataIn = SCSI_IOCTL_DATA_OUT;
   sptwb.spt.DataTransferLength = 512*cnt;
   sptwb.spt.TimeOutValue = 2;
   sptwb.spt.DataBufferOffset = (unsigned long*) __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucDataBuf);
   sptwb.spt.SenseInfoOffset = 
      __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucSenseBuf);
   sptwb.spt.Cdb[0] = 0x06;
   sptwb.spt.Cdb[1] = 0x20;
   sptwb.spt.Cdb[2] = cnt;
   sptwb.spt.Cdb[3] = 0;
   sptwb.spt.Cdb[4] = 0;
   sptwb.spt.Cdb[5] = 0;
   sptwb.spt.Cdb[6] = 0;
   sptwb.spt.Cdb[7] = 0;
   sptwb.spt.Cdb[8] = 0;
   sptwb.spt.Cdb[9] = 0;
   sptwb.spt.Cdb[10] = 0;
   sptwb.spt.Cdb[11] = 0;

   length = __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucDataBuf) +
      sptwb.spt.DataTransferLength;

   status = DeviceIoControl(fileHandle,
                            IOCTL_SCSI_PASS_THROUGH,
                            &sptwb,
                            length,
                            &sptwb,
                            length,
                            &returned,
                            FALSE);
   //PrintStatusResults(status,returned,&sptwb,8);
   return (status & (!sptwb.spt.ScsiStatus));
}



BYTE ReadBuf(HANDLE fileHandle, unsigned char cnt)
{
   BOOL status = 0;
   ULONG length = 0, returned = 0;
   unsigned char *str_ptr;
   unsigned int i;
   
   ZeroMemory(&sptwb,sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));
   sptwb.spt.Length = sizeof(SCSI_PASS_THROUGH);
   sptwb.spt.PathId = 0;
   sptwb.spt.TargetId = 1;
   sptwb.spt.Lun = 0;
   sptwb.spt.CdbLength = CDB12GENERIC_LENGTH;
   sptwb.spt.SenseInfoLength = 24;
   sptwb.spt.DataIn = SCSI_IOCTL_DATA_IN;
   sptwb.spt.DataTransferLength = 512*cnt;
   sptwb.spt.TimeOutValue = 2;
   sptwb.spt.DataBufferOffset = (unsigned long*) __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucDataBuf);
   sptwb.spt.SenseInfoOffset = 
      __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucSenseBuf);
   sptwb.spt.Cdb[0] = 0x06;
   sptwb.spt.Cdb[1] = 0x21;
   sptwb.spt.Cdb[2] = cnt;
   sptwb.spt.Cdb[3] = 0;
   sptwb.spt.Cdb[4] = 0;
   sptwb.spt.Cdb[5] = 0;
   sptwb.spt.Cdb[6] = 0;
   sptwb.spt.Cdb[7] = 0;
   sptwb.spt.Cdb[8] = 0;
   sptwb.spt.Cdb[9] = 0;
   sptwb.spt.Cdb[10] = 0;
   sptwb.spt.Cdb[11] = 0;

   length = __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucDataBuf) +
      sptwb.spt.DataTransferLength;

   status = DeviceIoControl(fileHandle,
                            IOCTL_SCSI_PASS_THROUGH,
                            &sptwb,
                            sizeof(SCSI_PASS_THROUGH),
                            &sptwb,
                            length,
                            &returned,
                            FALSE);
   
	 str_ptr = ((PUCHAR)sptwb.ucDataBuf);

	 for(i=0;i<cnt;i++)
        memcpy(Page_Data[128+i],str_ptr+512*i,64);
     
     //memcpy(Page_Data[8],str_ptr,512);
	 //memcpy(Page_Data[9],str_ptr+512*1,512);
	 //memcpy(Page_Data[10],str_ptr+512*2,512);
	 //memcpy(Page_Data[11],str_ptr+512*3,512);
	 return (status & (!sptwb.spt.ScsiStatus));
}


BYTE SCSICMD(HANDLE fileHandle, UCHAR data_dir,UCHAR CMD[CDB12GENERIC_LENGTH],int data_length, UCHAR data[65536])
{
   BOOL status = 0;
   ULONG length = 0, returned = 0;
   unsigned char *str_ptr;
   int i;

   ZeroMemory(&sptwb,sizeof(SCSI_PASS_THROUGH_WITH_BUFFERS));
   sptwb.spt.Length = sizeof(SCSI_PASS_THROUGH);
   sptwb.spt.PathId = 0;
   sptwb.spt.TargetId = 1;
   sptwb.spt.Lun = 0;
   sptwb.spt.CdbLength = CDB12GENERIC_LENGTH;
   sptwb.spt.SenseInfoLength = 24;
   sptwb.spt.DataIn = data_dir;
   sptwb.spt.DataTransferLength = data_length;
   sptwb.spt.TimeOutValue = 20000;
   sptwb.spt.DataBufferOffset = (unsigned long*) __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucDataBuf);
   sptwb.spt.SenseInfoOffset =
      __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucSenseBuf);
   for(i=0;i<CDB12GENERIC_LENGTH;i++)
      sptwb.spt.Cdb[i] = CMD[i];

   length = __builtin_offsetof(SCSI_PASS_THROUGH_WITH_BUFFERS,ucDataBuf) +
      sptwb.spt.DataTransferLength;
   if(data_dir == SCSI_IOCTL_DATA_OUT)
   {
      str_ptr = ((PUCHAR)sptwb.ucDataBuf);
      memcpy(str_ptr,data,data_length);
      status = DeviceIoControl(fileHandle,
                                IOCTL_SCSI_PASS_THROUGH,
                                &sptwb,
                                length,
                                &sptwb,
                                length,
                                &returned,
                                FALSE);
   }

     if(data_dir == SCSI_IOCTL_DATA_IN)
     {
        status = DeviceIoControl(fileHandle,
                                IOCTL_SCSI_PASS_THROUGH,
                                &sptwb,
                                sizeof(SCSI_PASS_THROUGH),
                                &sptwb,
                                length,
                                &returned,
                                FALSE);
	    str_ptr = ((PUCHAR)sptwb.ucDataBuf);
        memcpy(data,str_ptr,data_length);
     }
	 return (status & (!sptwb.spt.ScsiStatus));
}    

#endif
