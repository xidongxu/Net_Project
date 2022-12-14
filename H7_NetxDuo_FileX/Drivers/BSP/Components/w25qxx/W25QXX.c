/*
 * W25QXX.c
 *
 *  Created on: 2020年7月7日
 *      Author: MichaelLee
 */

#include "W25QXX.h"

extern SPI_HandleTypeDef hspi1;

/**
  * @brief  Initializes the W25QXX interface.
  * @retval None
  */
uint8_t BSP_W25QXX_Init(void)
{
  /* Reset W25QXXxx */
  uint8_t res = 0;
  res = BSP_W25QXX_Reset();
  if(res != W25QXX_OK) 
  {
    return res;
  }
  res = BSP_W25QXX_Enter4ByteAddressMode();
  return res;
}

/**
  * @brief  This function reset the W25QXX.
  * @retval None
  */
uint8_t BSP_W25QXX_Reset(void)
{
  uint32_t tickstart = HAL_GetTick();
  uint8_t cmd[2] = { RESET_ENABLE_CMD, RESET_MEMORY_CMD };

  W25QXX_Enable();
  /* Send the reset command */
  HAL_SPI_Transmit(&hspi1, cmd, 2, W25QXX_TIMEOUT_VALUE);
  W25QXX_Disable();

  /* Wait the end of Flash writing */
  while(BSP_W25QXX_GetStatus() == W25QXX_BUSY)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25QXX_TIMEOUT_VALUE)
    {
      return W25QXX_TIMEOUT;
    }
  }
  return W25QXX_OK;
}

/**
  * @brief  Reads current status of the W25QXX.
  * @retval W25QXX memory status
  */
uint8_t BSP_W25QXX_GetStatus(void)
{
  uint8_t cmd[] = { READ_STATUS_REG1_CMD };
  uint8_t status;

  W25QXX_Enable();
  /* Send the read status command */
  HAL_SPI_Transmit(&hspi1, cmd, 1, W25QXX_TIMEOUT_VALUE);
  /* Reception of the data */
  HAL_SPI_Receive(&hspi1, &status, 1, W25QXX_TIMEOUT_VALUE);
  W25QXX_Disable();

  /* Check the value of the register */
  if((status & W25QXX_FSR_BUSY) != 0)
  {
    return W25QXX_BUSY;
  }
  else
  {
    return W25QXX_OK;
  }
}

/**
  * @brief  This function send a Enter 4-Byte Address Mode (B7h)
  * @retval None
  */
uint8_t BSP_W25QXX_Enter4ByteAddressMode(void)
{
  uint8_t cmd[] = { ENTER_4BYTE_ADDRESS_MODE };
  uint32_t tickstart = HAL_GetTick();

  /*Select the FLASH: Chip Select low */
  W25QXX_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi1, cmd, 1, W25QXX_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25QXX_Disable();

  /* Wait the end of Flash writing */
  while(BSP_W25QXX_GetStatus() == W25QXX_BUSY)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25QXX_TIMEOUT_VALUE)
    {
      return W25QXX_TIMEOUT;
    }
  }
  return W25QXX_OK;
}

/**
  * @brief  This function send a Exit 4-Byte Address Mode (E9h)
  * @retval None
  */
uint8_t BSP_W25QXX_Exit4ByteAddressMode(void)
{
  uint8_t cmd[] = { EXIT_4BYTE_ADDRESS_MODE };
  uint32_t tickstart = HAL_GetTick();

  /*Select the FLASH: Chip Select low */
  W25QXX_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi1, cmd, 1, W25QXX_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25QXX_Disable();

  /* Wait the end of Flash writing */
  while(BSP_W25QXX_GetStatus() == W25QXX_BUSY)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25QXX_TIMEOUT_VALUE)
    {
      return W25QXX_TIMEOUT;
    }
  }
  return W25QXX_OK;
}

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @retval None
  */
uint8_t BSP_W25QXX_WriteEnable(void)
{
  uint8_t cmd[] = { WRITE_ENABLE_CMD };
  uint32_t tickstart = HAL_GetTick();

  /*Select the FLASH: Chip Select low */
  W25QXX_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi1, cmd, 1, W25QXX_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25QXX_Disable();
  
  /* Wait the end of Flash writing */
  while(BSP_W25QXX_GetStatus() == W25QXX_BUSY)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25QXX_TIMEOUT_VALUE)
    {
      return W25QXX_TIMEOUT;
    }
  }
  return W25QXX_OK;
}

/**
  * @brief  Read Manufacture/Device ID.
	* @param  return value address
  * @retval None
  */
void BSP_W25QXX_Read_ID(uint8_t *ID)
{
  uint8_t cmd[4] = { READ_ID_CMD, 0x00, 0x00, 0x00 };

  W25QXX_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi1, cmd, 4, W25QXX_TIMEOUT_VALUE);
  /* Reception of the data */
  HAL_SPI_Receive(&hspi1, ID, 2, W25QXX_TIMEOUT_VALUE);
  W25QXX_Disable();
}

/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  pData: Pointer to data to be read
  * @param  ReadAddr: Read start address
  * @param  Size: Size of data to read
  * @retval QSPI memory status
  */
uint8_t BSP_W25QXX_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
  uint8_t cmd[5] = { 0x00 };

  /* Check parameter */
  if(ReadAddr >= W25QXX_FLASH_SIZE || (ReadAddr + Size) >= W25QXX_FLASH_SIZE)
  {
    return W25QXX_ERROR;
  }
  /* Configure the command */
  cmd[0] = READ_CMD;
  cmd[1] = (uint8_t)(ReadAddr >> 24);
  cmd[2] = (uint8_t)(ReadAddr >> 16);
  cmd[3] = (uint8_t)(ReadAddr >> 8);
  cmd[4] = (uint8_t)(ReadAddr);

  W25QXX_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi1, cmd, 5, W25QXX_TIMEOUT_VALUE);

  /* Reception of the data */
  if (HAL_SPI_Receive(&hspi1, pData, Size, W25QXX_TIMEOUT_VALUE) != HAL_OK)
  {
    return W25QXX_ERROR;
  }

  W25QXX_Disable();
  return W25QXX_OK;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  * @param  pData: Pointer to data to be written
  * @param  WriteAddr: Write start address
  * @param  Size: Size of data to write,No more than 256byte.
  * @retval QSPI memory status
  */
uint8_t BSP_W25QXX_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
  uint8_t cmd[5] = { 0x00 };
  uint32_t end_addr, current_size, current_addr;
  uint32_t tickstart = HAL_GetTick();

  /* Check parameter */
  if(WriteAddr >= W25QXX_FLASH_SIZE || (WriteAddr + Size) >= W25QXX_FLASH_SIZE)
  {
    return W25QXX_ERROR;
  }
  /* Calculation of the size between the write address and the end of the page */
  current_addr = 0;

  while (current_addr <= WriteAddr)
  {
    current_addr += W25QXX_PAGE_SIZE;
  }

  current_size = current_addr - WriteAddr;

  /* Check if the size of the data is less than the remaining place in the page */
  if (current_size > Size)
  {
    current_size = Size;
  }

  /* Initialize the adress variables */
  current_addr = WriteAddr;
  end_addr = WriteAddr + Size;

  /* Perform the write page by page */
  do
  {
    /* Configure the command */
    cmd[0] = PAGE_PROG_CMD;
    cmd[1] = (uint8_t)(current_addr >> 24);
    cmd[2] = (uint8_t)(current_addr >> 16);
    cmd[3] = (uint8_t)(current_addr >> 8);
    cmd[4] = (uint8_t)(current_addr);

    /* Enable write operations */
    BSP_W25QXX_WriteEnable();

    W25QXX_Enable();

    /* Send the command */
    if (HAL_SPI_Transmit(&hspi1, cmd, 5, W25QXX_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25QXX_ERROR;
    }

    /* Transmission of the data */
    if (HAL_SPI_Transmit(&hspi1, pData, current_size, W25QXX_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25QXX_ERROR;
    }

    W25QXX_Disable();

    /* Wait the end of Flash writing */
    while(BSP_W25QXX_GetStatus() == W25QXX_BUSY)
    {
      /* Check for the Timeout */
      if((HAL_GetTick() - tickstart) > W25QXX_TIMEOUT_VALUE)
      {
        return W25QXX_TIMEOUT;
      }
    }

    /* Update the address and size variables for next page programming */
    current_addr += current_size;
    pData += current_size;
    current_size = ((current_addr + W25QXX_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25QXX_PAGE_SIZE;
  }
  while (current_addr < end_addr);

  return W25QXX_OK;
}

/**
  * @brief  Erases the specified sector of the QSPI memory.
  * @param  Address: Sector address to erase
  * @retval QSPI memory status
  */
uint8_t BSP_W25QXX_Erase_Sector(uint32_t Address)
{
  uint8_t cmd[5] = { 0x00 };
  uint32_t tickstart = HAL_GetTick();

  /* Check parameter */
  if(Address >= W25QXX_FLASH_SIZE)
  {
    return W25QXX_ERROR;
  }
  cmd[0] = SECTOR_ERASE_CMD;
  cmd[1] = (uint8_t)(Address >> 24);
  cmd[2] = (uint8_t)(Address >> 16);
  cmd[3] = (uint8_t)(Address >> 8);
  cmd[4] = (uint8_t)(Address);

  /* Enable write operations */
  BSP_W25QXX_WriteEnable();

  /*Select the FLASH: Chip Select low */
  W25QXX_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi1, cmd, 5, W25QXX_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25QXX_Disable();

  /* Wait the end of Flash writing */
  while(BSP_W25QXX_GetStatus() == W25QXX_BUSY)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25QXX_SECTOR_ERASE_MAX_TIME)
    {
      return W25QXX_TIMEOUT;
    }
  }
  return W25QXX_OK;
}

/**
  * @brief  Erases the specified block of the QSPI memory.
  * @param  BlockAddress: Block address to erase
  * @retval QSPI memory status
  */
uint8_t BSP_W25QXX_Erase_Block(uint32_t Address)
{
  uint8_t cmd[5] = { 0x00 };
  uint32_t tickstart = HAL_GetTick();

  if(Address >= W25QXX_FLASH_SIZE)
  {
    return W25QXX_ERROR;
  }
  cmd[0] = BLOCK_ERASE_CMD;
  cmd[1] = (uint8_t)(Address >> 24);
  cmd[2] = (uint8_t)(Address >> 16);
  cmd[3] = (uint8_t)(Address >> 8);
  cmd[4] = (uint8_t)(Address);

  /* Enable write operations */
  BSP_W25QXX_WriteEnable();

  /*Select the FLASH: Chip Select low */
  W25QXX_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi1, cmd, 5, W25QXX_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25QXX_Disable();

  /* Wait the end of Flash writing */
  while(BSP_W25QXX_GetStatus() == W25QXX_BUSY)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25QXX_BLOCK_ERASE_MAX_TIME)
    {
      return W25QXX_TIMEOUT;
    }
  }
  return W25QXX_OK;
}

/**
  * @brief  Erases the entire QSPI memory.This function will take a very long time.
  * @retval QSPI memory status
  */
uint8_t BSP_W25QXX_Erase_Chip(void)
{
  uint8_t cmd[4] = { CHIP_ERASE_CMD, 0x00, 0x00, 0x00 };
  uint32_t tickstart = HAL_GetTick();

  /* Enable write operations */
  BSP_W25QXX_WriteEnable();

  /*Select the FLASH: Chip Select low */
  W25QXX_Enable();
  /* Send the read ID command */
  HAL_SPI_Transmit(&hspi1, cmd, 1, W25QXX_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25QXX_Disable();

  /* Wait the end of Flash writing */
  while(BSP_W25QXX_GetStatus() != W25QXX_BUSY)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25QXX_BULK_ERASE_MAX_TIME)
    {
      return W25QXX_TIMEOUT;
    }
  }
  return W25QXX_OK;
}

/********************************End of File************************************/
