/************************************************************************************
  Проект: Lab1_2
  Программа: Работа с жидкокристаллическим индикатором
  Микроконтроллер: K1986ВЕ92QI
  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: mlt_lcd.h 
  Назначение:  Работа с жидкокристаллическим индикатором MT–12864J
	             под управлением драйвера "Ангстрем" К145ВГ10 (аналог Samsung KS0108)
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/

#ifndef __MLT_LCD_H
  #define __MLT_LCD_H
	
#include "common.h"

#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "string.h"
#include "mlt_font.h"
#include "MDR32F9Qx_rst_clk.h"         
#include "MDR32F9Qx_port.h"

// Ширина экрана в символах
#define	U_MLT_SCREEN_WIDTH_SYMBOLS	16
// Высота экрана в символах
#define	U_MLT_SCREEN_HEIGHT_SYMBOLS	4

// Установить и сбросить вывод Reset
#define	U_MLT_Set_Res_Pin		PORT_SetBits   (MDR_PORTB, PORT_Pin_9);
#define	U_MLT_Clr_Res_Pin		PORT_ResetBits (MDR_PORTB, PORT_Pin_9);

// Установить и сбросить вывод E
#define	U_MLT_Set_Stb_Pin		PORT_SetBits   (MDR_PORTC, PORT_Pin_1);
#define	U_MLT_Clr_Stb_Pin		PORT_ResetBits (MDR_PORTC, PORT_Pin_1);

// Установить и сбросить вывод RW
#define	U_MLT_Set_RW_Pin		PORT_SetBits   (MDR_PORTB, PORT_Pin_10);
#define	U_MLT_Clr_RW_Pin		PORT_ResetBits (MDR_PORTB, PORT_Pin_10);

// Установить и сбросить вывод A0
#define	U_MLT_Set_A0_Pin		PORT_SetBits   (MDR_PORTC, PORT_Pin_0);
#define	U_MLT_Clr_A0_Pin		PORT_ResetBits (MDR_PORTC, PORT_Pin_0);

// Установить и сбросить вывод E1
#define	U_MLT_Set_E1_Pin		PORT_SetBits   (MDR_PORTB, PORT_Pin_7);
#define	U_MLT_Clr_E1_Pin		PORT_ResetBits (MDR_PORTB, PORT_Pin_7);

// Установить и сбросить вывод E2
#define	U_MLT_Set_E2_Pin		PORT_SetBits   (MDR_PORTB, PORT_Pin_8);
#define	U_MLT_Clr_E2_Pin		PORT_ResetBits (MDR_PORTB, PORT_Pin_8);

// Установить направление приема данных для линий из порта A
#define U_MLT_Data_Dir_Input_PA  MDR_PORTA->OE &= 0xFFC0;
// Установить направление передачи данных для линий из порта A
#define U_MLT_Data_Dir_Output_PA MDR_PORTA->OE |= 0x003F;
// Установить направление приема данных для линий из порта F
#define U_MLT_Data_Dir_Input_PF  MDR_PORTF->OE &= 0xFFF3;
// Установить направление передачи данных для линий из порта F
#define U_MLT_Data_Dir_Output_PF MDR_PORTF->OE |= 0x000C;

// Передаваемые данные 
#define U_MLT_Output_Data		(uint8_t)(MDR_PORTA->RXTX & 0x3F) | (uint8_t)((MDR_PORTF->RXTX << 4) & 0xC0);

// Инициализация ЖКИ
void U_MLT_Init (void);

// Инициализация выводов МК для работы с ЖКИ
void U_MLT_Pin_Cfg (void);
// Инициализация драйвера ЖКИ
void U_MLT_LCD_Init(void);

// Выставить данные на шину данных
void U_MLT_Set_Data_Bits (uint8_t value);
// Задержка
void U_MLT_Delay (uint32_t value);
// Получить статус операции
uint8_t U_MLT_Read_Status (uint8_t Chip);

// Включить и выключить дисплей
void U_MLT_Disp_On (uint8_t Chip);
void U_MLT_Disp_Off (uint8_t Chip);

// Задать текущую страницу
void U_MLT_Set_Page (uint8_t Chip, uint8_t page);
// Задать текущий адрес
void U_MLT_Set_Address (uint8_t Chip, uint8_t address);
// Записать данные в ЖКИ
void U_MLT_Write_Data (uint8_t Chip, uint8_t data);
// Прочитать данные с ЖКИ
uint8_t U_MLT_Read_Data (uint8_t Chip);

// Очистить страницу
void U_MLT_Clear_Page (uint8_t Chip, uint8_t Page);
// Очистить дисплей
void U_MLT_Clear_Chip (uint8_t Chip);
// Вывести символ
void U_MLT_Put_Char (uint8_t symbol, int32_t X, int32_t Y);
// Вывести строку
void U_MLT_Put_String (const char* str, int32_t Y);
// Прокрутить строку на cnt шагов и вывести ее
void U_MLT_Scroll_String (const char* str, int32_t Y, int32_t cnt);
// Вывести изображение
void U_MLT_Put_Image (const uint8_t* image, int32_t top, int32_t left, int32_t bottom, int32_t right);

#endif 
