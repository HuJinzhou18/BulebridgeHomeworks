/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t ucSec;                     	/* Ãë¼ÆÊ± */
uint8_t ucKey, ucDir;             	/* °´¼üÖµ£¬LEDÏÔÊ¾Á÷Ë®·½Ïò */
uint8_t ucLed, ucSec1;            	/* LEDÖµ£¬LEDÏÔÊ¾ÑÓÊ± */
uint8_t ucLcd[21];                 	/* LCDÖµ(\0½áÊø) */
uint16_t usTlcd;                   	/* LCDË¢ĞÂ¼ÆÊ± */
uint8_t ucUrx[20], ucSec2;        	/* UART½ÓÊÕÖµ£¬·¢ËÍÑÓÊ± */
uint8_t ucCnt;    	     						/* Æô¶¯´ÎÊı */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	/* °´¼ü´¦Àí */
void LED_Proc(void);              	/* LED´¦Àí */
void LCD_Proc(void);              	/* LCD´¦Àí */
void UART_Proc(void);              	/* UART´¦Àí */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
//  MX_DMA_Init();
  MX_USART1_UART_Init();
//  MX_SPI2_Init();
  MX_I2C1_Init();
//  MX_ADC1_Init();
//  MX_ADC2_Init();
//  MX_TIM1_Init();
//  MX_TIM2_Init();
//  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();                      	/* LCD³õÊ¼»¯ */
  LCD_Clear(Black);                	/* LCDÇåÆÁ */
  LCD_SetTextColor(White);        	/* ÉèÖÃ×Ö·ûÉ« */
  LCD_SetBackColor(Black);        	/* ÉèÖÃ±³¾°É« */
	
 uint8_t ucTemp; // éªŒè¯ç”¨ä¸´æ—¶å˜é‡ï¼Œåªå®šä¹‰ä¸€æ¬¡
// ç¬¬ä¸€æ­¥ï¼šè¯»å–EEPROMä¸­ä¿å­˜çš„å¯åŠ¨æ¬¡æ•°
EEPROM_Read((uint8_t *)&ucCnt, 0, 1);  

// ç¬¬äºŒæ­¥ï¼šé¦–æ¬¡ä¸Šç”µåˆ¤æ–­ï¼ˆå¤„ç†EEPROMåˆå§‹å€¼0xFFçš„æƒ…å†µï¼‰
if(ucCnt > 200) { // è¯»åˆ°å¼‚å¸¸å€¼ï¼ˆå¦‚0xFFï¼‰ï¼Œåˆ¤å®šä¸ºé¦–æ¬¡ä¸Šç”µ
  ucCnt = 1;      // åˆå§‹åŒ–ä¸º1ï¼Œé¿å…æº¢å‡º
} else {
  ucCnt++;        // éé¦–æ¬¡ä¸Šç”µï¼Œè®¡æ•°+1
}

// ç¬¬ä¸‰æ­¥ï¼šå†™å…¥æ–°çš„è®¡æ•°åˆ°EEPROM
EEPROM_Write((uint8_t *)&ucCnt, 0, 1);
HAL_Delay(10);    // å…³é”®ï¼šç­‰å¾…EEPROMæ“¦å†™å®Œæˆï¼ˆ10msï¼‰

// ç¬¬å››æ­¥ï¼šéªŒè¯å†™å…¥æ˜¯å¦æˆåŠŸï¼ˆæ–°å¢å¯è§†åŒ–æç¤ºï¼‰
EEPROM_Read(&ucTemp, 0, 1);
if(ucTemp != ucCnt) { // è¯»å†™å¤±è´¥
  LCD_SetTextColor(Red); // çº¢è‰²å­—ä½“æç¤ºé”™è¯¯
  LCD_DisplayStringLine(Line7, "EEPROM WRITE ERR!");
  LCD_SetTextColor(White); // æ¢å¤ç™½è‰²å­—ä½“
}
  
  
  
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                   	/* °´¼ü´¦Àí */
    LED_Proc();                    	/* LED´¦Àí */
    LCD_Proc();                    	/* LCD´¦Àí */
    UART_Proc();    	       				/* UART´¦Àí */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
void KEY_Proc(void)               	/* °´¼ü´¦Àí */
{
  uint8_t ucKey1 = 0;

  ucKey1 = KEY_Read();             	/* °´¼ü¶ÁÈ¡ */
  if (ucKey1 != ucKey)             	/* ¼üÖµ±ä»¯ */
    ucKey = ucKey1;                	/* ±£´æ¼üÖµ */
  else
    ucKey1 = 0;                    	/* Çå³ı¼üÖµ */

  switch (ucKey1)
  {
    case 1:                        	/* B1°´¼ü°´ÏÂ */
      ucDir ^= 1;                  	/* ¸Ä±äÁ÷Ë®·½Ïò */
      break;
    case 2:                        	/* B2°´¼ü°´ÏÂ */
      break;
  }
}

void LED_Proc(void)               	/* LED´¦Àí */
{
  if (ucSec1 == ucSec)
    return;                        	/* 1sÎ´µ½·µ»Ø */
  ucSec1 = ucSec;

  if (ucDir == 0)                  	/* LEDÖµ×ó»·ÒÆ */
  {
    ucLed <<= 1; 
    if (ucLed == 0)
      ucLed = 1;
  }
  else                             	/* LEDÖµÓÒ»·ÒÆ */
  {
    ucLed >>= 1;
    if (ucLed == 0)
      ucLed = 0x80;
  }
  LED_Disp(ucLed);                 	/* LEDÏÔÊ¾ */
}

void LCD_Proc(void)   	           	/* LCD´¦Àí */
{
  if (usTlcd < 500)   				/* 500msÎ´µ½ */
    return;
  usTlcd = 0;

  sprintf((char*)ucLcd, " SEC:%03u   CNT:%03u ", ucSec, ucCnt);
  LCD_DisplayStringLine(Line2, ucLcd);
}

void UART_Proc(void)              	/* UART´¦Àí */
{
  if (ucSec2 != ucSec)            	/* 1sµ½ */
  {
    ucSec2 = ucSec;

    printf("%s %03u\r\n", ucLcd, ucSec);
  }
  if (UART_Receive(ucUrx, 2) == 0)  /* ½ÓÊÕµ½×Ö·û */
    ucSec = (ucUrx[0]-0x30)*10+ucUrx[1]-0x30;
}

int fputc(int ch, FILE *f)		    	/* printf()ÊµÏÖ */
{
  UART_Transmit((uint8_t *)&ch, 1);
  return ch;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
