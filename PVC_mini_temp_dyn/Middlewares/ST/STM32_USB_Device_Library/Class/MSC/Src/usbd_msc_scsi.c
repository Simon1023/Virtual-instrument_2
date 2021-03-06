/**
  ******************************************************************************
  * @file    usbd_msc_scsi.c
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   This file provides all the USBD SCSI layer functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_msc_bot.h"
#include "usbd_msc_scsi.h"
#include "usbd_msc.h"
#include "usbd_msc_data.h"
#include "def_type.h"
#include "segmentation.h"

/* USER CODE BEGIN 0 */
#include "Transmission_mode.h"

extern volatile uint8_t isReady;
extern volatile uint8_t isRead;

//extern uint8_t pData[NR][NC];
extern sd_uchar pData[NUM_OF_PIXELS];
extern sd_uchar dstBuf[NUM_OF_PIXELS];
extern sd_uchar roiBuf[NUM_OF_PIXELS];

extern ROI gRoi;
//extern int gRoiHandvalue;
//extern int gRoiWaveValue;

uint8_t *pCurCmd =NULL;

//20180425 Simon: This flag is to avoid executing image processing more than one time.
//                Because it only transfers 512 bytes at least each time. 
static uint8_t gTransfering=0;
    
int imageProcessing(unsigned char *src , unsigned char *dst , int nr , int nc);
int PNN_Calculate(unsigned char* img,int nc,unsigned char index);

/* USER CODE END 0 */
/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup MSC_SCSI 
  * @brief Mass storage SCSI layer module
  * @{
  */ 

/** @defgroup MSC_SCSI_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_Variables
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_FunctionPrototypes
  * @{
  */ 
	
/* USER CODE BEGIN 1 */
static int8_t MSC_BufferRead (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_BufferRead (USBD_HandleTypeDef  *pdev, uint8_t lun);
static int8_t MSC_FlagRead (USBD_HandleTypeDef  *pdev,  uint8_t lun, uint8_t *params);
static int8_t SCSI_FlagRead (USBD_HandleTypeDef  *pdev, uint8_t lun);
static int8_t MSC_ImageInfoRead (USBD_HandleTypeDef  *pdev,  uint8_t lun, uint8_t *params);
static int8_t SCSI_ImageInfoRead (USBD_HandleTypeDef  *pdev, uint8_t lun);
static int8_t SCSI_ClearFlag (USBD_HandleTypeDef  *pdev, uint8_t lun);
static int8_t MSC_ClearFlag (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t MSC_GetRoiInfo (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t MSC_GetSegmentCount (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t MSC_GetSegmentInfo (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);

/* USER CODE END 1 */
static int8_t SCSI_TestUnitReady(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_Inquiry(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_ReadFormatCapacity(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_ReadCapacity10(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_RequestSense (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_StartStopUnit(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_ModeSense6 (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_ModeSense10 (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_Write10(USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params);
static int8_t SCSI_Read10(USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params);
static int8_t SCSI_Verify10(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params);
static int8_t SCSI_CheckAddressRange (USBD_HandleTypeDef  *pdev, 
                                      uint8_t lun , 
                                      uint32_t blk_offset , 
                                      uint16_t blk_nbr);
static int8_t SCSI_ProcessRead (USBD_HandleTypeDef  *pdev,
                                uint8_t lun);

static int8_t SCSI_ProcessWrite (USBD_HandleTypeDef  *pdev,
                                 uint8_t lun);
static int8_t SCSI_ReadSegCount (USBD_HandleTypeDef  *pdev, uint8_t lun);
static int8_t SCSI_ReadSegInfo(USBD_HandleTypeDef *pdev, uint8_t lun, uint32_t length);
/**
  * @}
  */ 


/** @defgroup MSC_SCSI_Private_Functions
  * @{
  */ 


/**
* @brief  SCSI_ProcessCmd
*         Process SCSI commands
* @param  pdev: device instance
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
int8_t SCSI_ProcessCmd(USBD_HandleTypeDef  *pdev,
                           uint8_t lun, 
                           uint8_t *params)
{
  
	pCurCmd = params;
	
  switch (params[0])
  {
  case SCSI_TEST_UNIT_READY:
    return SCSI_TestUnitReady(pdev, lun, params);
    
  case SCSI_REQUEST_SENSE:
    return SCSI_RequestSense (pdev, lun, params);
	
  case SCSI_INQUIRY:
    return SCSI_Inquiry(pdev, lun, params);
    
  case SCSI_START_STOP_UNIT:
    return SCSI_StartStopUnit(pdev, lun, params);
    
  case SCSI_ALLOW_MEDIUM_REMOVAL:
    return SCSI_StartStopUnit(pdev, lun, params);
    
  case SCSI_MODE_SENSE6:
    return SCSI_ModeSense6 (pdev, lun, params);
    
  case SCSI_MODE_SENSE10:
    return SCSI_ModeSense10 (pdev, lun, params);
    
  case SCSI_READ_FORMAT_CAPACITIES:
    return SCSI_ReadFormatCapacity(pdev, lun, params);
    
  case SCSI_READ_CAPACITY10:
    return SCSI_ReadCapacity10(pdev, lun, params);
    
  case SCSI_READ10:
    return SCSI_Read10(pdev, lun, params); 
    
  case SCSI_WRITE10:
    return SCSI_Write10(pdev, lun, params);
    
  case SCSI_VERIFY10:
    return SCSI_Verify10(pdev, lun, params);
    
  case SCSI_CHECK_IMAGE_READY:
    return MSC_FlagRead(pdev, lun, params);
    
  case SCSI_GET_IMAGE:
    return MSC_BufferRead(pdev, lun, params);
    
	case SCSI_GET_IMAGE_INFO:
		return MSC_ImageInfoRead(pdev, lun, params);
	
	case SCSI_CLEAR_FLAG:
    {
        gTransfering=0;
        
		return MSC_ClearFlag(pdev, lun, params);
    }
    
	//20180109 Simon Windows AP sends the information to PVC mini 
	case SCSI_SEND_ROI_INFO:
    case SCSI_SEND_ROI_DIGIT_INFO:
    case SCSI_SEND_ROI_WAVE_INFO:
    case SCSI_SEND_ROI_HAND_INFO:
		return MSC_GetRoiInfo(pdev, lun, params);

	//20180116 Simon: Windows AP gets the image of ROI from PVC mini 
	case SCSI_GET_ROI_IMAGE:
	{
		char* pSrc = (char*)pData+NC*gRoi.y+gRoi.x;
		char* pDest = (char*)roiBuf;
         
		int i;
        
		for(i=0;i<gRoi.h;i++)
		{	
			int j;
	
			//Thre's a restriction of aliment for memcpy
			for(j=0;j<gRoi.w;j++)
				*(pDest+j)=	*(pSrc+j);
            
			pSrc+=NC;
			pDest+=gRoi.w;
		}

		//2018/01/27 Simon :Image Processing
        if(gTransfering==0)
            imageProcessing(roiBuf , dstBuf , gRoi.h , gRoi.w);
        
        gTransfering=1;
        
		return MSC_BufferRead(pdev, lun, params);
	}
	
	//20180210 Simon : Send the count of characters
	case SCSI_GET_SEGMENT_COUNT:
	{
		return MSC_GetSegmentCount(pdev, lun, params);
	}

	//20180220 Simon: Get the command from PC to get the information of segmented characters
	case SCSI_GET_SEGMENT_INFO:
	{
		return MSC_GetSegmentInfo(pdev, lun, params);
	}
	
  default:
    SCSI_SenseCode(pdev, 
                   lun,
                   ILLEGAL_REQUEST, 
                   INVALID_CDB);    
    return -1;
  }
}

/* USER CODE BEGIN 2 */

//20180109 Simon Windows AP sends the information to PVC mini 
static int8_t MSC_GetRoiInfo (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
	//IDLE 初始化
  if(hmsc->bot_state == USBD_BOT_IDLE)  /* Idle */
  {
    /* case 10 : Ho <> Di */
		/*
    if ((hmsc->cbw.bmFlags & (0x80)) != 0x80)
    {
      SCSI_SenseCode(pdev,
										 hmsc->cbw.bLUN, 
										 ILLEGAL_REQUEST, 
										 INVALID_CDB);
      return -1;
    }    
		*/
		
		hmsc->scsi_blk_addr = 0;
		hmsc->scsi_blk_len = 1;
    
    if( SCSI_CheckAddressRange(pdev, lun, hmsc->scsi_blk_addr, hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */
    }
    
		gRoi.x = params[1]| params[2] << 8;
		gRoi.y = params[3]| params[4] << 8;
		gRoi.w = params[5]| params[6] << 8;
		gRoi.h = params[7]| params[8] << 8;
        
        //20180414 Simon: Check the type of ROI by SCSI command
        if(params[0] == SCSI_SEND_ROI_DIGIT_INFO)
        {
            gRoi.type = ROI_TYPE_DIGIT;
            gRoi.r = params[9];
            gRoi.g = params[10];
            gRoi.b = params[11];
        }
        else if(params[0] == SCSI_SEND_ROI_WAVE_INFO)
        {
            gRoi.type = ROI_TYPE_WAVE;   
        }
        else if(params[0] == SCSI_SEND_ROI_HAND_INFO)
        {
            gRoi.type = ROI_TYPE_HAND;   
        }
  }
	
	hmsc->bot_data_length = 0;
	
	return 0;
}

//20180210 Simon : Send the count of characters
static int8_t MSC_GetSegmentCount (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
	//IDLE 初始化
  if(hmsc->bot_state == USBD_BOT_IDLE)  /* Idle */
  {
    /* case 10 : Ho <> Di */
    if ((hmsc->cbw.bmFlags & (0x80)) != 0x80)
    {
      SCSI_SenseCode(pdev,
										 hmsc->cbw.bLUN, 
										 ILLEGAL_REQUEST, 
										 INVALID_CDB);
      return -1;
    }    
		
		hmsc->scsi_blk_addr = 0;
		hmsc->scsi_blk_len = 1;
    
    if( SCSI_CheckAddressRange(pdev, lun, hmsc->scsi_blk_addr, hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */
    }
    
    hmsc->bot_state = USBD_BOT_DATA_IN;
//    hmsc->scsi_blk_addr *= hmsc->scsi_blk_size;
//    hmsc->scsi_blk_len  *= hmsc->scsi_blk_size;
    
  }
  hmsc->bot_data_length = 1;  
  
  return SCSI_ReadSegCount(pdev, lun); 
}

static int8_t SCSI_ReadSegCount (USBD_HandleTypeDef  *pdev, uint8_t lun)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
  uint32_t len =1;;
  uint8_t  count = segmentGetCount();
		  
	hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  USBD_LL_Transmit (pdev, 
             MSC_EPIN_ADDR,
             &count,
             len);
  
  hmsc->scsi_blk_addr   += len; 
  hmsc->scsi_blk_len    -= len;  
  
  /* case 6 : Hi = Di */
  hmsc->csw.dDataResidue -= len;
  
  if (hmsc->scsi_blk_len == 0)
  {
    hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  }
  return 0;
}

//20180220 Simon: Get the command from PC to get the information of segmented characters 
static int8_t MSC_GetSegmentInfo (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData;
	uint32_t length;
	
	//IDLE 初始化
  if(hmsc->bot_state == USBD_BOT_IDLE)  /* Idle */
  {
    /* case 10 : Ho <> Di */
    if ((hmsc->cbw.bmFlags & (0x80)) != 0x80)
    {
      SCSI_SenseCode(pdev,
										 hmsc->cbw.bLUN, 
										 ILLEGAL_REQUEST, 
										 INVALID_CDB);
      return -1;
    }    
		
		hmsc->scsi_blk_addr = 0;
		hmsc->scsi_blk_len = length = segmentGetCount()*sizeof(CHAR_INFO);
    
    if( SCSI_CheckAddressRange(pdev, lun, hmsc->scsi_blk_addr, hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */
    }
    
    hmsc->bot_state = USBD_BOT_DATA_IN;    
  }
  hmsc->bot_data_length = MSC_MEDIA_PACKET;  
  
  return SCSI_ReadSegInfo(pdev, lun, length); 
}

//20180220 Simon: Reply the information of segmented characters to PC
static int8_t SCSI_ReadSegInfo(USBD_HandleTypeDef *pdev, uint8_t lun, uint32_t length)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
	uint8_t *pBuf = (uint8_t *)segmentGetInfo();
	uint32_t len = length;
	
	hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  USBD_LL_Transmit (pdev, 
             MSC_EPIN_ADDR,
             pBuf,
             len);
  
  hmsc->scsi_blk_addr   += len; 
  hmsc->scsi_blk_len    -= len;  
  
  /* case 6 : Hi = Di */
  hmsc->csw.dDataResidue -= len;
  
  if (hmsc->scsi_blk_len == 0)
  {
    hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  }
  return 0;
}


static int8_t MSC_FlagRead (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
	//IDLE 初始化
  if(hmsc->bot_state == USBD_BOT_IDLE)  /* Idle */
  {
    /* case 10 : Ho <> Di */
    if ((hmsc->cbw.bmFlags & (0x80)) != 0x80)
    {
      SCSI_SenseCode(pdev,
										 hmsc->cbw.bLUN, 
										 ILLEGAL_REQUEST, 
										 INVALID_CDB);
      return -1;
    }    
		
		hmsc->scsi_blk_addr = 0;
		hmsc->scsi_blk_len = 1;
    
    if( SCSI_CheckAddressRange(pdev, lun, hmsc->scsi_blk_addr, hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */
    }
    
    hmsc->bot_state = USBD_BOT_DATA_IN;
//    hmsc->scsi_blk_addr *= hmsc->scsi_blk_size;
//    hmsc->scsi_blk_len  *= hmsc->scsi_blk_size;
    
  }
  hmsc->bot_data_length = 1;  
  
  return SCSI_FlagRead(pdev, lun); 
} // MSC_BufferRead2()

static int8_t SCSI_FlagRead (USBD_HandleTypeDef  *pdev, uint8_t lun)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
  uint32_t len;
  uint8_t *finish_buffer;
	
	finish_buffer = &isReady;
	
	len = 1;
  
	hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  USBD_LL_Transmit (pdev, 
             MSC_EPIN_ADDR,
             (finish_buffer),
             len);
  
  hmsc->scsi_blk_addr   += len; 
  hmsc->scsi_blk_len    -= len;  
  
  /* case 6 : Hi = Di */
  hmsc->csw.dDataResidue -= len;
  
  if (hmsc->scsi_blk_len == 0)
  {
    hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  }
  return 0;
}

static int8_t MSC_ClearFlag (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
	//IDLE 初始化
  if(hmsc->bot_state == USBD_BOT_IDLE)  /* Idle */
  {
    /* case 10 : Ho <> Di */
    if ((hmsc->cbw.bmFlags & (0x80)) != 0x80)
    {
      SCSI_SenseCode(pdev,
										 hmsc->cbw.bLUN, 
										 ILLEGAL_REQUEST, 
										 INVALID_CDB);
      return -1;
    }    
		
		hmsc->scsi_blk_addr = 0;
		hmsc->scsi_blk_len = 1;
    
    if( SCSI_CheckAddressRange(pdev, lun, hmsc->scsi_blk_addr, hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */
    }
    
    hmsc->bot_state = USBD_BOT_DATA_IN;
//    hmsc->scsi_blk_addr *= hmsc->scsi_blk_size;
//    hmsc->scsi_blk_len  *= hmsc->scsi_blk_size;
    
  }
  hmsc->bot_data_length = 1;  
  
  return SCSI_ClearFlag(pdev, lun); 
} // MSC_BufferRead2()

static int8_t SCSI_ClearFlag (USBD_HandleTypeDef  *pdev, uint8_t lun)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
  uint32_t len;
  uint8_t *finish_buffer;
	
	finish_buffer = &isRead;
	isRead = 1;
	len = 1;
  
	hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  USBD_LL_Transmit (pdev, 
             MSC_EPIN_ADDR,
             (finish_buffer),
             len);
  
  hmsc->scsi_blk_addr   += len; 
  hmsc->scsi_blk_len    -= len;  
  
  /* case 6 : Hi = Di */
  hmsc->csw.dDataResidue -= len;
  
  if (hmsc->scsi_blk_len == 0)
  {
    hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  }
  return 0;
}

static int8_t MSC_BufferRead (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
  if(hmsc->bot_state == USBD_BOT_IDLE)  /* Idle */
  {
    /* case 10 : Ho <> Di */
    if ((hmsc->cbw.bmFlags & (0x80)) != 0x80)
    {
      SCSI_SenseCode(pdev,
										 hmsc->cbw.bLUN, 
										 ILLEGAL_REQUEST, 
										 INVALID_CDB);
      return -1;
    }    
    
//     if(USBD_STORAGE_fops->IsReady(lun) !=0 )
//     {
//       SCSI_SenseCode(lun,
//                      NOT_READY, 
//                      MEDIUM_NOT_PRESENT);
//       return -1;
//     } 
		
		
    hmsc->scsi_blk_addr = (params[2] << 24) | \
      (params[3] << 16) | \
        (params[4] <<  8) | \
          params[5];
   
	
    hmsc->scsi_blk_len =  (params[7] <<  8) | \
      params[8];  
    
    if( SCSI_CheckAddressRange(pdev, lun, hmsc->scsi_blk_addr, hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */
    }
    
    hmsc->bot_state = USBD_BOT_DATA_IN;
    hmsc->scsi_blk_addr *= hmsc->scsi_blk_size;
    hmsc->scsi_blk_len  *= hmsc->scsi_blk_size;
    
    /* cases 4,5 : Hi <> Dn */
    if (hmsc->cbw.dDataLength != hmsc->scsi_blk_len)
    {
      SCSI_SenseCode(pdev,
										 hmsc->cbw.bLUN, 
										 ILLEGAL_REQUEST, 
										 INVALID_CDB);
      return -1;
    }
  }
  hmsc->bot_data_length = MSC_MEDIA_PACKET;  
  
  return SCSI_BufferRead(pdev, lun); 
} // MSC_CAMBufferRead()
/***********************************************************/
static int8_t SCSI_BufferRead (USBD_HandleTypeDef  *pdev, uint8_t lun)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
  uint32_t len;

	uint8_t *image_data;
	int imageSize;

	image_data = (uint8_t *)pData;
	
	//20180116 Simon: Windows AP gets the image of ROI from PVC mini
	if(TRANSFER_ROI_IMAGE && (pCurCmd[0] == SCSI_GET_ROI_IMAGE))
	{
		imageSize = gRoi.w*gRoi.h;
		
		//2018/01/27 Simon :Image Processing
		image_data = dstBuf;
	}
	else
		imageSize = NUM_OF_PIXELS;

//	if(isReady)
//	   audio_data = audio_data1;
//	else
//		 audio_data = audio_data2;
	
  len = MIN(hmsc->scsi_blk_len , MSC_MEDIA_PACKET); 
  
  if ( ( hmsc->scsi_blk_addr + len ) > imageSize) { 
    len = imageSize - hmsc->scsi_blk_addr;
    hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  } // if 
  
//  if(hmsc->scsi_blk_addr <= imageSize + 3)
  USBD_LL_Transmit (pdev, 
             MSC_EPIN_ADDR,
             (image_data+hmsc->scsi_blk_addr),
             len);
//  else
//  USBD_LL_Transmit (pdev, 
//             MSC_EPIN_ADDR,
//             (image_data),
//             len);
  
  hmsc->scsi_blk_addr   += len; 
  hmsc->scsi_blk_len    -= len;  
  
  /* case 6 : Hi = Di */
  hmsc->csw.dDataResidue -= len;
  
  if (hmsc->scsi_blk_len == 0)
  {
    hmsc->bot_state = USBD_BOT_LAST_DATA_IN;      
  }
  
  return 0;
}

static int8_t MSC_ImageInfoRead (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
	//IDLE 初始化
  if(hmsc->bot_state == USBD_BOT_IDLE)  /* Idle */
  {
    /* case 10 : Ho <> Di */
    if ((hmsc->cbw.bmFlags & (0x80)) != 0x80)
    {
      SCSI_SenseCode(pdev,
										 hmsc->cbw.bLUN, 
										 ILLEGAL_REQUEST, 
										 INVALID_CDB);
      return -1;
    }    
		
		hmsc->scsi_blk_addr = 0;
		hmsc->scsi_blk_len = 4;
    
    if( SCSI_CheckAddressRange(pdev, lun, hmsc->scsi_blk_addr, hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */
    }
    
    hmsc->bot_state = USBD_BOT_DATA_IN;
//    hmsc->scsi_blk_addr *= hmsc->scsi_blk_size;
//    hmsc->scsi_blk_len  *= hmsc->scsi_blk_size;
    
  }
  hmsc->bot_data_length = 4;  
  
  return SCSI_ImageInfoRead(pdev, lun); 
} // MSC_ImageInfoRead()

static int8_t SCSI_ImageInfoRead (USBD_HandleTypeDef  *pdev, uint8_t lun)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
  uint32_t len;
  uint8_t *info_buffer;
	uint8_t image_info[4];
	
	image_info[0] = NC & 0xFF;
	image_info[1] = (NC >> 8) & 0xFF;
	image_info[2] = NR & 0xFF;
	image_info[3] = (NR >> 8) & 0xFF;
	info_buffer = image_info;
	
	len = 4;
  
	hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  USBD_LL_Transmit (pdev, 
             MSC_EPIN_ADDR,
             (info_buffer),
             len);
  
  hmsc->scsi_blk_addr   += len; 
  hmsc->scsi_blk_len    -= len;  
  
  /* case 6 : Hi = Di */
  hmsc->csw.dDataResidue -= len;
  
  if (hmsc->scsi_blk_len == 0)
  {
    hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  }
  return 0;
}

/* USER CODE END 2 */
/**
* @brief  SCSI_TestUnitReady
*         Process SCSI Test Unit Ready Command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_TestUnitReady(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData;  
    
  /* case 9 : Hi > D0 */
  if (hmsc->cbw.dDataLength != 0)
  {
    SCSI_SenseCode(pdev,
                   hmsc->cbw.bLUN, 
                   ILLEGAL_REQUEST, 
                   INVALID_CDB);
    return -1;
  }  
  
//  if(((USBD_StorageTypeDef *)pdev->pUserData)->IsReady(lun) !=0 )
//  {
//    SCSI_SenseCode(pdev,
//                   lun,
//                   NOT_READY, 
//                   MEDIUM_NOT_PRESENT);
//    
//    hmsc->bot_state = USBD_BOT_NO_DATA;
//    return -1;
//  } 
  hmsc->bot_data_length = 0;
  return 0;
}

/**
* @brief  SCSI_Inquiry
*         Process Inquiry command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t  SCSI_Inquiry(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  //uint8_t* pPage;
  //uint16_t len;
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
//  
//  if (params[1] & 0x01)/*Evpd is set*/
//  {
//    pPage = (uint8_t *)MSC_Page00_Inquiry_Data;
//    len = LENGTH_INQUIRY_PAGE00;
//  }
//  else
//  {
//    
//    pPage = (uint8_t *)&((USBD_StorageTypeDef *)pdev->pUserData)->pInquiry[lun * STANDARD_INQUIRY_DATA_LEN];
//    len = pPage[4] + 5;
//    
//    if (params[4] <= len)
//    {
//      len = params[4];
//    }
//  }
//  hmsc->bot_data_length = len;
//  
//  while (len) 
//  {
//    len--;
//    hmsc->bot_data[len] = pPage[len];
//  }

  hmsc->bot_data[ 0] = 0x00;          /* Direct Access Device */
  hmsc->bot_data[ 1] = 0x80;          /* RMB = 1: Removable Medium */
  hmsc->bot_data[ 2] = 0x00;          /* Version: No conformance claim to standard */
  hmsc->bot_data[ 3] = 0x01;

  hmsc->bot_data[ 4] = 36-4;          /* Additional Length */
  hmsc->bot_data[ 5] = 0x80;          /* SCCS = 1: Storage Controller Component */
  hmsc->bot_data[ 6] = 0x00;
  hmsc->bot_data[ 7] = 0x00;

  hmsc->bot_data[ 8] = 'M';           /* Vendor Identification */
  hmsc->bot_data[ 9] = 'I';
  hmsc->bot_data[10] = 'A';
  hmsc->bot_data[11] = 'T';
  hmsc->bot_data[12] = ' ';
  hmsc->bot_data[13] = ' ';
  hmsc->bot_data[14] = ' ';
  hmsc->bot_data[15] = ' ';

  hmsc->bot_data[16] = 'S';           /* Product Identification */
  hmsc->bot_data[17] = 'T';
  hmsc->bot_data[18] = 'M';
  hmsc->bot_data[19] = '0';
  hmsc->bot_data[20] = '3';
  hmsc->bot_data[21] = 'V';
  hmsc->bot_data[22] = '1';
  hmsc->bot_data[23] = ' ';
  hmsc->bot_data[24] = ' ';
  hmsc->bot_data[25] = 'D';
  hmsc->bot_data[26] = 'I';
  hmsc->bot_data[27] = 'S';
  hmsc->bot_data[28] = 'K';
  hmsc->bot_data[29] = '2';
  hmsc->bot_data[30] = ' ';
  hmsc->bot_data[31] = ' ';

  hmsc->bot_data[32] = '1';           /* Product Revision Level */
  hmsc->bot_data[33] = '.';
  hmsc->bot_data[34] = '0';
  hmsc->bot_data[35] = ' ';

  hmsc->bot_data_length = 36;
	
  return 0;
}

/**
* @brief  SCSI_ReadCapacity10
*         Process Read Capacity 10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_ReadCapacity10(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
  
  if(((USBD_StorageTypeDef *)pdev->pUserData)->GetCapacity(lun, &hmsc->scsi_blk_nbr, &hmsc->scsi_blk_size) != 0)
  {
    SCSI_SenseCode(pdev,
                   lun,
                   NOT_READY, 
                   MEDIUM_NOT_PRESENT);
    return -1;
  } 
  else
  {
    
    hmsc->bot_data[0] = (uint8_t)((hmsc->scsi_blk_nbr - 1) >> 24);
    hmsc->bot_data[1] = (uint8_t)((hmsc->scsi_blk_nbr - 1) >> 16);
    hmsc->bot_data[2] = (uint8_t)((hmsc->scsi_blk_nbr - 1) >>  8);
    hmsc->bot_data[3] = (uint8_t)(hmsc->scsi_blk_nbr - 1);
    
    hmsc->bot_data[4] = (uint8_t)(hmsc->scsi_blk_size >>  24);
    hmsc->bot_data[5] = (uint8_t)(hmsc->scsi_blk_size >>  16);
    hmsc->bot_data[6] = (uint8_t)(hmsc->scsi_blk_size >>  8);
    hmsc->bot_data[7] = (uint8_t)(hmsc->scsi_blk_size);
    
    hmsc->bot_data_length = 8;
    return 0;
  }
}
/**
* @brief  SCSI_ReadFormatCapacity
*         Process Read Format Capacity command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_ReadFormatCapacity(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
  
  uint16_t blk_size;
  uint32_t blk_nbr;
  uint16_t i;
  
  for(i=0 ; i < 12 ; i++) 
  {
    hmsc->bot_data[i] = 0;
  }
  
  if(((USBD_StorageTypeDef *)pdev->pUserData)->GetCapacity(lun, &blk_nbr, &blk_size) != 0)
  {
    SCSI_SenseCode(pdev,
                   lun,
                   NOT_READY, 
                   MEDIUM_NOT_PRESENT);
    return -1;
  } 
  else
  {
    hmsc->bot_data[3] = 0x08;
    hmsc->bot_data[4] = (uint8_t)((blk_nbr - 1) >> 24);
    hmsc->bot_data[5] = (uint8_t)((blk_nbr - 1) >> 16);
    hmsc->bot_data[6] = (uint8_t)((blk_nbr - 1) >>  8);
    hmsc->bot_data[7] = (uint8_t)(blk_nbr - 1);
    
    hmsc->bot_data[8] = 0x02;
    hmsc->bot_data[9] = (uint8_t)(blk_size >>  16);
    hmsc->bot_data[10] = (uint8_t)(blk_size >>  8);
    hmsc->bot_data[11] = (uint8_t)(blk_size);
    
    hmsc->bot_data_length = 12;
    return 0;
  }
}
/**
* @brief  SCSI_ModeSense6
*         Process Mode Sense6 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_ModeSense6 (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
  uint16_t len = 8 ;
  hmsc->bot_data_length = len;
  
//  while (len) 
//  {
//    len--;
//    hmsc->bot_data[len] = MSC_Mode_Sense6_data[len];
//  }
	hmsc->bot_data[ 0] = 0x03;
  hmsc->bot_data[ 1] = 0x00;
  hmsc->bot_data[ 2] = 0x00;
  hmsc->bot_data[ 3] = 0x00;
  hmsc->bot_data_length = 4 ;
  return 0;
}

/**
* @brief  SCSI_ModeSense10
*         Process Mode Sense10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_ModeSense10 (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  uint16_t len = 8;
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
  
  hmsc->bot_data_length = len;

//  while (len) 
//  {
//    len--;
//    hmsc->bot_data[len] = MSC_Mode_Sense10_data[len];
//  }
 	hmsc->bot_data[ 0] = 0x00;
  hmsc->bot_data[ 1] = 0x06;
  hmsc->bot_data[ 2] = 0x00;
  hmsc->bot_data[ 3] = 0x00;
  hmsc->bot_data[ 4] = 0x00;
  hmsc->bot_data[ 5] = 0x00;
  hmsc->bot_data[ 6] = 0x00;
  hmsc->bot_data[ 7] = 0x00;
  return 0;
}

/**
* @brief  SCSI_RequestSense
*         Process Request Sense command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/

static int8_t SCSI_RequestSense (USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  uint8_t i;
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
  
  for(i=0 ; i < REQUEST_SENSE_DATA_LEN ; i++) 
  {
    hmsc->bot_data[i] = 0;
  }
  
  hmsc->bot_data[0]	= 0x70;		
  hmsc->bot_data[7]	= REQUEST_SENSE_DATA_LEN - 6;	
  
  if((hmsc->scsi_sense_head != hmsc->scsi_sense_tail)) {
    
    hmsc->bot_data[2]     = hmsc->scsi_sense[hmsc->scsi_sense_head].Skey;		
    hmsc->bot_data[12]    = hmsc->scsi_sense[hmsc->scsi_sense_head].w.b.ASCQ;	
    hmsc->bot_data[13]    = hmsc->scsi_sense[hmsc->scsi_sense_head].w.b.ASC;	
    hmsc->scsi_sense_head++;
    
    if (hmsc->scsi_sense_head == SENSE_LIST_DEEPTH)
    {
      hmsc->scsi_sense_head = 0;
    }
  }
  hmsc->bot_data_length = REQUEST_SENSE_DATA_LEN;  
  
  if (params[4] <= REQUEST_SENSE_DATA_LEN)
  {
    hmsc->bot_data_length = params[4];
  }
  return 0;
}

/**
* @brief  SCSI_SenseCode
*         Load the last error code in the error list
* @param  lun: Logical unit number
* @param  sKey: Sense Key
* @param  ASC: Additional Sense Key
* @retval none

*/
void SCSI_SenseCode(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t sKey, uint8_t ASC)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData; 
  
  hmsc->scsi_sense[hmsc->scsi_sense_tail].Skey  = sKey;
  hmsc->scsi_sense[hmsc->scsi_sense_tail].w.ASC = ASC << 8;
  hmsc->scsi_sense_tail++;
  if (hmsc->scsi_sense_tail == SENSE_LIST_DEEPTH)
  {
    hmsc->scsi_sense_tail = 0;
  }
}
/**
* @brief  SCSI_StartStopUnit
*         Process Start Stop Unit command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_StartStopUnit(USBD_HandleTypeDef  *pdev, uint8_t lun, uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*) pdev->pClassData;   
  hmsc->bot_data_length = 0;
  return 0;
}

/**
* @brief  SCSI_Read10
*         Process Read10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/
static int8_t SCSI_Read10(USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*) pdev->pClassData; 
  
  if(hmsc->bot_state == USBD_BOT_IDLE)  /* Idle */
  {
    
    /* case 10 : Ho <> Di */
    
    if ((hmsc->cbw.bmFlags & 0x80) != 0x80)
    {
      SCSI_SenseCode(pdev,
                     hmsc->cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }    
    
    if(((USBD_StorageTypeDef *)pdev->pUserData)->IsReady(lun) !=0 )
    {
      SCSI_SenseCode(pdev,
                     lun,
                     NOT_READY, 
                     MEDIUM_NOT_PRESENT);
      return -1;
    } 
    
    hmsc->scsi_blk_addr = (params[2] << 24) | \
      (params[3] << 16) | \
        (params[4] <<  8) | \
          params[5];
    
    hmsc->scsi_blk_len =  (params[7] <<  8) | \
      params[8];  
    
    
    
    if( SCSI_CheckAddressRange(pdev, lun, hmsc->scsi_blk_addr, hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */
    }
    
    hmsc->bot_state = USBD_BOT_DATA_IN;
    hmsc->scsi_blk_addr *= hmsc->scsi_blk_size;
    hmsc->scsi_blk_len  *= hmsc->scsi_blk_size;
    
    /* cases 4,5 : Hi <> Dn */
    if (hmsc->cbw.dDataLength != hmsc->scsi_blk_len)
    {
      SCSI_SenseCode(pdev,
                     hmsc->cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }
  }
  hmsc->bot_data_length = MSC_MEDIA_PACKET;  
  
  return SCSI_ProcessRead(pdev, lun);
}

/**
* @brief  SCSI_Write10
*         Process Write10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/

static int8_t SCSI_Write10 (USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*) pdev->pClassData; 
  
  if (hmsc->bot_state == USBD_BOT_IDLE) /* Idle */
  {
    
    /* case 8 : Hi <> Do */
    
    if ((hmsc->cbw.bmFlags & 0x80) == 0x80)
    {
      SCSI_SenseCode(pdev,
                     hmsc->cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }
    
    /* Check whether Media is ready */
//    if(((USBD_StorageTypeDef *)pdev->pUserData)->IsReady(lun) !=0 )
//    {
//      SCSI_SenseCode(pdev,
//                     lun,
//                     NOT_READY, 
//                     MEDIUM_NOT_PRESENT);
//      return -1;
//    } 
//    
//    /* Check If media is write-protected */
//    if(((USBD_StorageTypeDef *)pdev->pUserData)->IsWriteProtected(lun) !=0 )
//    {
//      SCSI_SenseCode(pdev,
//                     lun,
//                     NOT_READY, 
//                     WRITE_PROTECTED);
//      return -1;
//    } 
    
    
    hmsc->scsi_blk_addr = (params[2] << 24) | \
      (params[3] << 16) | \
        (params[4] <<  8) | \
          params[5];
    hmsc->scsi_blk_len = (params[7] <<  8) | \
      params[8];  
    
    /* check if LBA address is in the right range */
    if(SCSI_CheckAddressRange(pdev,
                              lun,
                              hmsc->scsi_blk_addr,
                              hmsc->scsi_blk_len) < 0)
    {
      return -1; /* error */      
    }
    
    hmsc->scsi_blk_addr *= hmsc->scsi_blk_size;
    hmsc->scsi_blk_len  *= hmsc->scsi_blk_size;
    
    /* cases 3,11,13 : Hn,Ho <> D0 */
    if (hmsc->cbw.dDataLength != hmsc->scsi_blk_len)
    {
      SCSI_SenseCode(pdev,
                     hmsc->cbw.bLUN, 
                     ILLEGAL_REQUEST, 
                     INVALID_CDB);
      return -1;
    }
    
    /* Prepare EP to receive first data packet */
    hmsc->bot_state = USBD_BOT_DATA_OUT;  
    USBD_LL_PrepareReceive (pdev,
                      MSC_EPOUT_ADDR,
                      hmsc->bot_data, 
                      MIN (hmsc->scsi_blk_len, MSC_MEDIA_PACKET));  
  }
  else /* Write Process ongoing */
  {
    return SCSI_ProcessWrite(pdev, lun);
  }
  return 0;
}


/**
* @brief  SCSI_Verify10
*         Process Verify10 command
* @param  lun: Logical unit number
* @param  params: Command parameters
* @retval status
*/

static int8_t SCSI_Verify10(USBD_HandleTypeDef  *pdev, uint8_t lun , uint8_t *params)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*) pdev->pClassData; 
  
  if ((params[1]& 0x02) == 0x02) 
  {
    SCSI_SenseCode (pdev,
                    lun, 
                    ILLEGAL_REQUEST, 
                    INVALID_FIELED_IN_COMMAND);
    return -1; /* Error, Verify Mode Not supported*/
  }
  
  if(SCSI_CheckAddressRange(pdev,
                            lun, 
                            hmsc->scsi_blk_addr, 
                            hmsc->scsi_blk_len) < 0)
  {
    return -1; /* error */      
  }
  hmsc->bot_data_length = 0;
  return 0;
}

/**
* @brief  SCSI_CheckAddressRange
*         Check address range
* @param  lun: Logical unit number
* @param  blk_offset: first block address
* @param  blk_nbr: number of block to be processed
* @retval status
*/
static int8_t SCSI_CheckAddressRange (USBD_HandleTypeDef  *pdev, uint8_t lun , uint32_t blk_offset , uint16_t blk_nbr)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*) pdev->pClassData; 
  
  if ((blk_offset + blk_nbr) > hmsc->scsi_blk_nbr )
  {
    SCSI_SenseCode(pdev,
                   lun, 
                   ILLEGAL_REQUEST, 
                   ADDRESS_OUT_OF_RANGE);
    return -1;
  }
  return 0;
}

/**
* @brief  SCSI_ProcessRead
*         Handle Read Process
* @param  lun: Logical unit number
* @retval status
*/
static int8_t SCSI_ProcessRead (USBD_HandleTypeDef  *pdev, uint8_t lun)
{
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*)pdev->pClassData;   
  uint32_t len;
  
  len = MIN(hmsc->scsi_blk_len , MSC_MEDIA_PACKET); 
  
//  if( ((USBD_StorageTypeDef *)pdev->pUserData)->Read(lun ,
//                              hmsc->bot_data, 
//                              hmsc->scsi_blk_addr / hmsc->scsi_blk_size, 
//                              len / hmsc->scsi_blk_size) < 0)
//  {
//    
//    SCSI_SenseCode(pdev,
//                   lun, 
//                   HARDWARE_ERROR, 
//                   UNRECOVERED_READ_ERROR);
//    return -1; 
//  }
  
  
  USBD_LL_Transmit (pdev, 
             MSC_EPIN_ADDR,
             hmsc->bot_data,
             len);
  
  
  hmsc->scsi_blk_addr   += len; 
  hmsc->scsi_blk_len    -= len;  
  
  /* case 6 : Hi = Di */
  hmsc->csw.dDataResidue -= len;
  
  if (hmsc->scsi_blk_len == 0)
  {
    hmsc->bot_state = USBD_BOT_LAST_DATA_IN;
  }
  return 0;
}

/**
* @brief  SCSI_ProcessWrite
*         Handle Write Process
* @param  lun: Logical unit number
* @retval status
*/

static int8_t SCSI_ProcessWrite (USBD_HandleTypeDef  *pdev, uint8_t lun)
{
  uint32_t len;
  USBD_MSC_BOT_HandleTypeDef  *hmsc = (USBD_MSC_BOT_HandleTypeDef*) pdev->pClassData; 
  
  len = MIN(hmsc->scsi_blk_len , MSC_MEDIA_PACKET); 
  
  if(((USBD_StorageTypeDef *)pdev->pUserData)->Write(lun ,
                              hmsc->bot_data, 
                              hmsc->scsi_blk_addr / hmsc->scsi_blk_size, 
                              len / hmsc->scsi_blk_size) < 0)
  {
    SCSI_SenseCode(pdev,
                   lun, 
                   HARDWARE_ERROR, 
                   WRITE_FAULT);     
    return -1; 
  }
  
  
  hmsc->scsi_blk_addr  += len; 
  hmsc->scsi_blk_len   -= len; 
  
  /* case 12 : Ho = Do */
  hmsc->csw.dDataResidue -= len;
  
  if (hmsc->scsi_blk_len == 0)
  {
    MSC_BOT_SendCSW (pdev, USBD_CSW_CMD_PASSED);
  }
  else
  {
    /* Prepare EP to Receive next packet */
    USBD_LL_PrepareReceive (pdev,
                            MSC_EPOUT_ADDR,
                            hmsc->bot_data, 
                            MIN (hmsc->scsi_blk_len, MSC_MEDIA_PACKET)); 
  }
  
  return 0;
}
/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
