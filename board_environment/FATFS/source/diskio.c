/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include "diskio.h"			/* FatFs lower layer API */
//#include "sdmmc_sdcard.h"
#include "w25qxx.h"
#include "malloc.h"	 
#include "nand.h"	 
#include "ftl.h"	

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//FATFS底层(diskio) 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/8/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


#define SD_CARD	 	0  			//SD卡,卷标为0
#define EX_FLASH 	1			//外部spi flash,卷标为1
#define EX_NAND  	2			//外部nand flash,卷标为2

//对于W25Q256
//前25M字节给fatfs用,25M字节后,用于存放字库,字库占用6.01M.	剩余部分,给客户自己用	 
#define SPI_FLASH_SECTOR_SIZE 	512	
#define SPI_FLASH_SECTOR_COUNT 	1024*25*2	//W25Q256,前25M字节给FATFS占用	
#define SPI_FLASH_BLOCK_SIZE   	8     		//每个BLOCK有8个扇区		
  
  
  
//获得磁盘状态
DSTATUS disk_status (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{ 
	return RES_OK;
}  


//初始化磁盘
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	u8 res=0;	    
	switch(pdrv)
	{
		case SD_CARD:		//SD卡
			//res=SD_Init();	//SD卡初始化 
  			break;
		case EX_FLASH:		//外部flash
			W25QXX_Init();  //W25QXX初始化
 			break;
		case EX_NAND:		//外部NAND
			res=FTL_Init();	//NAND初始化
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //初始化成功 
} 
//读扇区
//pdrv:磁盘编号0~9
//*buff:数据接收缓冲首地址
//sector:扇区地址
//count:需要读取的扇区数
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	u8 res=0; 
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(pdrv)
	{
		case SD_CARD://SD卡
//			res=SD_ReadDisk(buff,sector,count);	 
//			while(res)//读出错
//			{
//				SD_Init();	//重新初始化SD卡
//				res=SD_ReadDisk(buff,sector,count);	
//				//printf("sd rd error:%d\r\n",res);
//			}
			break;
		case EX_FLASH://外部flash
			for(;count>0;count--)
			{
				W25QXX_Read(buff,sector*SPI_FLASH_SECTOR_SIZE,SPI_FLASH_SECTOR_SIZE);
				sector++;
				buff+=SPI_FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		case EX_NAND:		//外部NAND
			res=FTL_ReadSectors(buff,sector,512,count);	//读取数据			
			break;
		default:
			res=1; 
	}
   //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}
//写扇区
//pdrv:磁盘编号0~9
//*buff:发送数据首地址
//sector:扇区地址
//count:需要写入的扇区数 
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	u8 res=0;  
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	 
	switch(pdrv)
	{
		case SD_CARD://SD卡
//			res=SD_WriteDisk((u8*)buff,sector,count);
//			while(res)//写出错
//			{
//				SD_Init();	//重新初始化SD卡
//				res=SD_WriteDisk((u8*)buff,sector,count);	
//				//printf("sd wr error:%d\r\n",res);
//			}
			break;
		case EX_FLASH://外部flash
			for(;count>0;count--)
			{										    
				W25QXX_Write((u8*)buff,sector*SPI_FLASH_SECTOR_SIZE,SPI_FLASH_SECTOR_SIZE);
				sector++;
				buff+=SPI_FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		case EX_NAND:		//外部NAND
			res=FTL_WriteSectors((u8*)buff,sector,512,count);//写入数据
			break;
		default:
			res=1; 
	}
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	
} 
//其他表参数的获得
//pdrv:磁盘编号0~9
//ctrl:控制代码
//*buff:发送/接收缓冲区指针 
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
DRESULT res;						  			     
	if(pdrv==SD_CARD)//SD卡
	{
//	    switch(cmd)
//	    {
//		    case CTRL_SYNC:
//				res = RES_OK; 
//		        break;	 
//		    case GET_SECTOR_SIZE:
//				*(DWORD*)buff = 512; 
//		        res = RES_OK;
//		        break;	 
//		    case GET_BLOCK_SIZE:
//				*(WORD*)buff = SDCardInfo.CardBlockSize;
//		        res = RES_OK;
//		        break;	 
//		    case GET_SECTOR_COUNT:
//		        *(DWORD*)buff = SDCardInfo.CardCapacity/512;
//		        res = RES_OK;
//		        break;
//		    default:
//		        res = RES_PARERR;
//		        break;
//	    }
	}else if(pdrv==EX_FLASH)	//外部FLASH  
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = SPI_FLASH_SECTOR_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = SPI_FLASH_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SPI_FLASH_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else if(pdrv==EX_NAND)	//外部NAND FLASH
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;	//NAND FLASH扇区强制为512字节大小
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = nand_dev.page_mainsize/512;//block大小,定义成一个page的大小
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = nand_dev.valid_blocknum*nand_dev.block_pagenum*nand_dev.page_mainsize/512;//NAND FLASH的总扇区大小
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else res=RES_ERROR;//其他的不支持
    return res;
} 




















