#include <lib_stm32f7_ov5653.h>
#include "DCMI_OV5653_INITTABLE.h"
#include <stdlib.h>
#include "stm32f7xx_hal.h"
#include "def_type.h"

#include "Transmission_mode.h"
extern uint8_t pData[NR][NC];

//original------------------------------------
extern I2C_HandleTypeDef hi2c3;
uint32_t DMA_FRAME_BUFFER = (2592*1944/4);

//#pragma arm section rwdata = "SDRAM1", zidata = "SDRAM1"
//unsigned char OrgImgBuf[480][640];	// cap_rows = 240 , cap_cols = 320
//unsigned char OrgImgBuf[307200];		
//#pragma arm section rwdata
sd_uchar OrgImgBuf[2592*1944];

void OV5653_Reset(void) 
{
	uint8_t sccb_wrdata[] = {0x30, 0x08, 0x80};		
	HAL_I2C_Master_Transmit(&hi2c3, 0x6C, sccb_wrdata, 3, 100);	 
	HAL_Delay(20);
}

void OV5653_InitConfig(void)
{
	int i,j;
	HAL_Delay(20);
	uint32_t *reg_index;
	
	reg_index = (uint32_t *)reset_seq;	
	for(i = 0; i < 2; i++)
	{ 
		HAL_I2C_Master_Transmit(&hi2c3, 0x6C, (uint8_t *)reg_index++, 3, 100);	 
		HAL_Delay(1);
	}	

	reg_index = (uint32_t *)mode_start;	
	for(i = 0; i < 44; i++)
	{ 
		HAL_I2C_Master_Transmit(&hi2c3, 0x6C, (uint8_t *)reg_index++, 3, 100);	 
		HAL_Delay(1);
	}	

//	j=(sizeof(mode_2592x1944)>>2);
//	reg_index = (uint32_t *)mode_2592x1944;	
//	j=(sizeof(mode_640x480)>>2);
//	reg_index = (uint32_t *)mode_640x480;
	j=(sizeof(mode_320x240)>>2);
	reg_index = (uint32_t *)mode_320x240;
//	j=(sizeof(mode_160x120)>>2);
//	reg_index = (uint32_t *)mode_160x120;
	for(i = 0; i < j; i++)
	{ 
		HAL_I2C_Master_Transmit(&hi2c3, 0x6C, (uint8_t *)reg_index++, 3, 100);	 
		HAL_Delay(1);
	}	
	
	reg_index = (uint32_t *)mode_end;	
	for(i = 0; i < 1; i++)
	{ 
		HAL_I2C_Master_Transmit(&hi2c3, 0x6C, (uint8_t *)reg_index++, 3, 100);	 
		HAL_Delay(1);
	}		
//	reg_index = (uint8_t *) init_reg1;	
//	for(i = 0; i < 117; i++)
//	{ 
//		HAL_I2C_Master_Transmit(&hi2c3, 0x6C, reg_index+(i*3), 3, 100);	 
//		HAL_Delay(1);
//	}	
	
//	reg_index = (uint8_t *) mode_320x240;	
//	for(i = 0; i < 132; i++)
//	{ 
//		HAL_I2C_Master_Transmit(&hi2c3, 0x6C, reg_index+(i*3), 3, 100);	 
//		HAL_Delay(1);
//	}	
	
//	/*TEST START*/
//	uint8_t AAA[INIT_REG_NUM];
//	uint8_t *BBB = AAA;
//	for(i = 0; i < INIT_REG_NUM; i++)
//	{ 
//		HAL_I2C_Master_Transmit(&hi2c3, 0x6C, reg_index+(i*3), 2, 100);		
//		HAL_I2C_Master_Receive(&hi2c3, 0x6C, BBB+i, 1, 100);	
//		HAL_Delay(1);
//	}			
//	/*TEST END*/
}
//--------------------------------

//funtion to interpolaze the bayer pattern to color image
//void bayer2rgb(unsigned char *CAMBuffer)
//{
//		int i,j,R,G,B, index;
//		int nr = NR, nc = NC;

//		for(i=0;i<nr;i++)
//		{
//			for(j=0;j<nc;j++)
//			{
//				if(i%2==1 && j%2==0)
//				{
//					R=(pData[i-1][j-1]+pData[i-1][j+1]+pData[i+1][j-1]+pData[i+1][j+1])/4;
//					G=(pData[i][j-1]+pData[i][j+1]+pData[i-1][j]+pData[i+1][j])/4;
//					B=pData[i][j];
//				}
//				else if(i%2==1 && j%2==1)
//				{
//					R=(pData[i-1][j]+pData[i+1][j])/2;
//					G=pData[i][j];
//					B=(pData[i][j-1]+pData[i][j+1])/2;
//				}
//				else if(i%2==0 && j%2==0)
//				{
//					R=(pData[i][j-1]+pData[i][j+1])/2;
//					G=pData[i][j];
//					B=(pData[i-1][j]+pData[i+1][j])/2;
//				} 
//				else if(i%2==0 && j%2==1)
//				{
//					R=pData[i][j];
//					G=(pData[i][j-1]+pData[i][j+1]+pData[i-1][j]+pData[i+1][j])/4;
//					B=(pData[i-1][j-1]+pData[i-1][j+1]+pData[i+1][j-1]+pData[i+1][j+1])/4;
//				}

//				#if IMAGE_TYPE == GRAY_IMAGE
//					CAMBuffer[(i*nc + j)]=(R+G+B)/3;
//				#elif IMAGE_TYPE == COLOR_IMAGE
//					index = (i * nc + j) * 3;
//					#if SEND_MODE == USB_SEND_IMAGE
//						CAMBuffer[index]=B;
//						CAMBuffer[index+1]=G;
//						CAMBuffer[index+2]=R;				
//					#elif SEND_MODE == UART_SEND_IMAGE
//						CAMBuffer[index]=R;
//						CAMBuffer[index+1]=G;
//						CAMBuffer[index+2]=B;
//					#endif
//				#endif
//			}
//		}
//}

void bayer2rgb(uint8_t *CAMBuffer)
{
	int i,j;
	uint8_t R,G,B;
	uint16_t nr = NR, nc = NC;
	uint32_t index;

		for(i=1;i<nr-1;i++)
		{
			for(j=1;j<nc-1;j++)
			{
//				uint32_t index = (i * nc + j);
				if(i%2==1 && j%2==0)	//G
				{
//					R=(pData[i-1][j-1]+pData[i-1][j+1]+pData[i+1][j-1]+pData[i+1][j+1])/4;
//					G=(pData[i][j-1]+pData[i][j+1]+pData[i-1][j]+pData[i+1][j])/4;
//					B=pData[i][j];
//            R = (pData[index-1] + pData[index+1])/2 ;
//            G = pData[index];
//            B = (pData[index-nc] + pData[index+nc])/2;
					R=(pData[i][j-1]+pData[i][j+1])/2;
					G=pData[i][j];
					B=(pData[i-1][j]+pData[i+1][j])/2;
				}
				else if(i%2==1 && j%2==1)	//R
				{
//					R=(pData[i-1][j]+pData[i+1][j])/2;
//					G=pData[i][j];
//					B=(pData[i][j-1]+pData[i][j+1])/2;
//					R = pData[index];
//					G = (pData[index-nc] + pData[index+nc] + pData[index-1] + pData[index+1])/4;
//					B = (pData[index-nc-1] + pData[index-nc+1] + pData[index+nc-1] + pData[index+nc+1])/4;
					R=pData[i][j];
					G=(pData[i][j-1]+pData[i][j+1]+pData[i-1][j]+pData[i+1][j])/4;
					B=(pData[i-1][j-1]+pData[i-1][j+1]+pData[i+1][j-1]+pData[i+1][j+1])/4;
				}
				else if(i%2==0 && j%2==0)	//B
				{
//					R=(pData[i][j-1]+pData[i][j+1])/2;
//					G=pData[i][j];
//					B=(pData[i-1][j]+pData[i+1][j])/2;
//					R = (pData[index-nc-1] + pData[index-nc+1] +pData[index+nc-1] + pData[index+nc+1])/4;
//          G = (pData[index-nc] + pData[index+nc] +pData[index-1] + pData[index+1])/4;
//          B = pData[index];
					R=(pData[i-1][j-1]+pData[i-1][j+1]+pData[i+1][j-1]+pData[i+1][j+1])/4;
					G=(pData[i][j-1]+pData[i][j+1]+pData[i-1][j]+pData[i+1][j])/4;
					B=pData[i][j];
				} 
				else if(i%2==0 && j%2==1)	//G
				{
//					R=pData[i][j];
//					G=(pData[i][j-1]+pData[i][j+1]+pData[i-1][j]+pData[i+1][j])/4;
//					B=(pData[i-1][j-1]+pData[i-1][j+1]+pData[i+1][j-1]+pData[i+1][j+1])/4;
//					R = (pData[index-1] + pData[index+1])/2 ;
//          G = pData[index];
//          B = (pData[index-nc] + pData[index+nc])/2;
					R=(pData[i][j-1]+pData[i][j+1])/2;
					G=pData[i][j];
					B=(pData[i-1][j]+pData[i+1][j])/2;
				}
/**/
//				uint32_t RGBindex = (i * nc + j) * 3;
//				CAMBuffer[RGBindex+0]=R;
//				CAMBuffer[RGBindex+1]=G;
//				CAMBuffer[RGBindex+2]=B;
/**/
				#if IMAGE_TYPE == GRAY_IMAGE
					CAMBuffer[(i*nc + j)]=(R+G+B)/3;
				#elif IMAGE_TYPE == COLOR_IMAGE
					index = (i * nc + j) * 3;
					#if SEND_MODE == USB_SEND_IMAGE
						CAMBuffer[index]=B;
						CAMBuffer[index+1]=G;
						CAMBuffer[index+2]=R;				
					#elif SEND_MODE == UART_SEND_IMAGE
						CAMBuffer[index]=R;
						CAMBuffer[index+1]=G;
						CAMBuffer[index+2]=B;
					#endif
				#endif
			}
		}
}
