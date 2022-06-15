#ifndef __W25Q256_H_
#define __W25Q256_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup W25Q256FV
  * @{
  */

/** @defgroup W25Q256FV_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup W25Q256FV_Exported_Constants
  * @{
  */

/**
  * @brief  W25Q256FV Configuration
  */
#define W25Q256FV_FLASH_SIZE                  0x1000000 /* 128 MBits => 16MBytes */
#define W25Q256FV_BLOCK_SIZE                  0x10000   /* 512 sectors of 64KBytes */
#define W25Q256FV_SECTOR_SIZE                 0x1000    /* 4096 subsectors of 4kBytes */
#define W25Q256FV_PAGE_SIZE                   0x100     /* 65536 pages of 256 bytes */

#define W25Q256FV_DUMMY_CYCLES_READ           4
#define W25Q256FV_DUMMY_CYCLES_READ_QUAD      10

#define W25Q256FV_BULK_ERASE_MAX_TIME         400000
#define W25Q256FV_BLOCK_ERASE_MAX_TIME        5000
#define W25Q256FV_SECTOR_ERASE_MAX_TIME       2000
#define W25Q256_TIMEOUT_VALUE                 1000

/**
  * @brief  W25Q256FV Commands
  */
/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* Identification Operations */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* Read Operations */
#define READ_CMD                             0x03
#define READ_4BTYEADDR_CMD                   0x13
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG1_CMD                 0x05
#define READ_STATUS_REG2_CMD                 0x35
#define READ_STATUS_REG3_CMD                 0x15

#define WRITE_STATUS_REG1_CMD                0x01
#define WRITE_STATUS_REG2_CMD                0x31
#define WRITE_STATUS_REG3_CMD                0x11


/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32


/* Erase Operations */
#define SECTOR_ERASE_CMD                     0x20
#define BLOCK_ERASE_CMD                      0xD8
#define CHIP_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75

/* Address Mode */
#define ENTER_4BYTE_ADDRESS_MODE             0xB7
#define EXIT_4BYTE_ADDRESS_MODE              0xE9

/* Flag Status Register */
#define W25Q256FV_FSR_BUSY                  ((uint8_t)0x01)    /*!< busy */
#define W25Q256FV_FSR_WREN                  ((uint8_t)0x02)    /*!< write enable */
#define W25Q256FV_FSR_QE                    ((uint8_t)0x02)    /*!< quad enable */
#define W25Q256_Enable() 			              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define W25Q256_Disable() 		              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define W25Q256_OK                          ((uint8_t)0x00)
#define W25Q256_ERROR                       ((uint8_t)0x01)
#define W25Q256_BUSY                        ((uint8_t)0x02)
#define W25Q256_TIMEOUT				              ((uint8_t)0x03)
uint8_t BSP_W25Q256_Init(void);
uint8_t	BSP_W25Q256_Reset(void);
uint8_t BSP_W25Q256_GetStatus(void);
uint8_t BSP_W25Q256_WriteEnable(void);
void    BSP_W25Q256_Read_ID(uint8_t *ID);
uint8_t BSP_W25Q256_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t BSP_W25Q256_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
uint8_t BSP_W25Q256_Erase_Sector(uint32_t Address);
uint8_t BSP_W25Q256_Erase_Block(uint32_t Address);
uint8_t BSP_W25Q256_Erase_Chip(void);
uint8_t BSP_W25Q256_Enter4ByteAddressMode(void);
uint8_t BSP_W25Q256_Exit4ByteAddressMode(void);
/**
  * @}
  */
/** @defgroup W25Q256FV_Exported_Functions
  * @{
  */
/**
  * @}
  */
/**
  * @}
  */
/**
  * @}
  */
/**
  * @}
  */
#ifdef __cplusplus
}
#endif
#endif /* __W25Q256_H */

/********************************End of File************************************/
