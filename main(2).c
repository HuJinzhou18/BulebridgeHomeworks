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
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "dma.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "stdio.h"
#include "i2c.h"
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
uint8_t ucSec;                     	/* 秒计时 */
uint8_t ucKey, ucDir;             	/* 按键值，LED显示流水方向 */
uint8_t ucLed, ucSec1;            	/* LED值，LED显示延时 */
uint8_t ucLcd[21];                 	/* LCD值(\0结束) */
uint16_t usTlcd;                   	/* LCD刷新计时 */
uint8_t ucUrx[20], ucSec2;        	/* UART接收值，发送延时 */
uint8_t ucCnt;    		        			/* 启动次数 */
uint8_t ucMcp=0x0f;			        		/* MCP值 */
uint16_t usAdc[3];			        		/* ADC转换值 */
uint8_t ucTadc;				          		/* ADC刷新时间 */
uint8_t ucDuty=10;	        				/* TIM输出占空比 */
uint16_t usCapt[2];			        		/* TIM输入捕捉值 */
uint16_t k=0;
uint8_t y=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void KEY_Proc(void);              	/* 按键处理 */
void LED_Proc(void);              	/* LED处理 */
void LCD_Proc(void);              	/* LCD处理 */
void UART_Proc(void);              	/* UART处理 */
void ADC_Proc(void);              	/* ADC处理 */
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
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();                      	/* LCD初始化 */
  LCD_Clear(Black);                	/* LCD清屏 */
  LCD_SetTextColor(White);        	/* 设置字符色 */
  LCD_SetBackColor(Black);        	/* 设置背景色 */
	
  MEM_Read((uint8_t *)&ucCnt, 0, 1);	/* 存储器读 */
  ucCnt++;
  MEM_Write((uint8_t *)&ucCnt, 0, 1);	/* 存储器写 */

  MCP_Write(ucMcp);	        				/* MCP写 */

  UART_Receive(ucUrx, 2);           /* 仅用于HAL工程 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    KEY_Proc();                    	/* 按键处理 */
    LED_Proc();                    	/* LED处理 */
    LCD_Proc();                    	/* LCD处理 */
    UART_Proc();    		      			/* UART处理 */
    ADC_Proc();    	        				/* ADC处理 */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
void KEY_Proc(void)               	/* 按键处理 */
{
  uint8_t ucKey_Val = 0;

  ucKey_Val = KEY_Read();         	/* 按键读取 */
  if (ucKey_Val != ucKey)         	/* 键值变化 */
    ucKey = ucKey_Val;            	/* 保存键值 */
  else
    ucKey_Val = 0;                 	/* 清除键值 */

  switch (ucKey_Val)
  {
    case 1:                        	/* B1键按下 */
      ucDir ^= 1;                  	/* 改变流水方向 */
      ucMcp += 0x10;
      if (ucMcp == 0x8f)
        ucMcp = 0x0f;
      MCP_Write(ucMcp);
      break;
    case 2:                        	/* B2键按下 */
      ucDuty += 10;
      if(ucDuty == 100)
        ucDuty = 10;
      TIM1_SetCompare1(ucDuty*5);
      TIM3_SetCompare1(ucDuty*10);
      break;
		case 3:                        	/* B2键按下 */
      y++;
      break;
  }
}

void LED_Proc(void)               	/* LED处理 */
{
  if (ucSec1 == ucSec)
    return;                        	/* 1s未到返回 */
  ucSec1 = ucSec;

  if (ucDir == 0)                  	/* LED值左环移 */
  {
    ucLed <<= 1; 
    if (ucLed == 0)
      ucLed = 1;
  } else {                         	/* LED值右环移 */
    ucLed >>= 1;
    if (ucLed == 0)
      ucLed = 0x80;
  }
  LED_Disp(ucLed);                 	/* LED显示 */
}

void LCD_Proc(void)               	/* LCD处理 */
{
  if (usTlcd < 100)   	      			/* 500ms未到 */
    return;
  usTlcd = 0;
	k++;
	if (k>=319){k=0;}
  sprintf((char*)ucLcd, " SEC:%03u   CNT:%03u ", ucSec, ucCnt);
  LCD_DisplayStringLine(Line2, ucLcd);
  sprintf((char *)ucLcd, " R37:%04u  B1: 0x%02X", usAdc[2], ucMcp);
  LCD_DisplayStringLine(Line4, ucLcd);
  sprintf((char *)ucLcd, " R38:%04u  MCP:%03u", usAdc[0], usAdc[1]);
  LCD_DisplayStringLine(Line5, ucLcd);
  TIM2_GetCapture(usCapt);
  sprintf((char *)ucLcd, " FRE:%04u  B2: %02u%% ", 1000000/usCapt[1], ucDuty);
  LCD_DisplayStringLine(Line7, ucLcd);
  sprintf((char *)ucLcd, " PER:%04u  WID:%03u ", usCapt[1], usCapt[0]);
  LCD_DisplayStringLine(Line8, ucLcd);
	if (usAdc[2]+usAdc[0]>=4095){LCD_Line_Clear(Blue);}
	else {LCD_Line_Clear(Grey);}
	if (y%2==0){
	for (int j=0;j<20;j++){uint32_t x=k+j*20;if(x>=319){x=x-319;}
		LCD_DrawLine(15, x , 4095 ,Green);}
	}
	else if(y%2==1) {for (int j=0;j<20;j++){uint32_t x=k+j*20;if(x>=319){x=x-319;}
		LCD_DrawLine(15, x , 4095 ,Red);}}
}

void UART_Proc(void)              	/* UART处理 */
{
  if (ucSec2 != ucSec)
  {                         	  		/* 1s到 */
    ucSec2 = ucSec;

//    UART_Transmit(ucLcd, 20);		    /* 发送20个字符 */
//    printf("%03u\r\n", ucSec);  		/* 发送秒值和回车换行 */
    printf("%s %03u\r\n", ucLcd, ucSec);
  }

//  if (UART_Receive(ucUrx, 2) == 0)  /* 接收到字符 */
//    ucSec = (ucUrx[0]-0x30)*10+ucUrx[1]-0x30;
}

int fputc(int ch, FILE *f)		    	/* printf()实现 */
{
  UART_Transmit((uint8_t *)&ch, 1);
  return ch;
}

void ADC_Proc(void)               	/* ADC处理 */
{
  if (ucTadc < 100)         				/* 100ms未到 */
    return;
  ucTadc = 0;

  ADC1_Read(usAdc);
  usAdc[2] = ADC2_Read();
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
