/************************************************************************************
  Проект: Lab1_2
  Программа: Работа с жидкокристаллическим индикатором
  Микроконтроллер: K1986ВЕ92QI
  Устройство: Evaluation Board For MCU MDR32F2Q
  Файл: mlt_lcd.c
  Назначение:  Работа с жидкокристаллическим индикатором MT–12864J
	             под управлением драйвера "Ангстрем" К145ВГ10 (аналог Samsung KS0108)
  Компилятор:  Armcc 5.06.0 из комплекта Keil uVision 5.20.0
************************************************************************************/

#include "mlt_lcd.h"

// Строка, выводимая на индикатор при инициализации
static const char* init_string = "   \xC0\xCE \"\xCC\xE8\xEB\xE0\xED\xE4\xF0\"";  // АО "Миландр"

// Изображение, выводимое на индикатор при инициализации (Эмблема "Миландра")
static const uint8_t init_image[32] = 
{
	254, 254, 254, 254, 254, 126,  30,   6,
		6,  30, 126, 254, 254, 254, 254, 254,
	127,  95,  71,  65, 124, 120, 112,  96,
	 96, 112, 120, 124,  65,  71,  95, 127
};

// Инициализация ЖКИ
void U_MLT_Init (void)
{
	// Инициализировать выводы МК для работы с ЖКИ
	U_MLT_Pin_Cfg ();
	
	// Инициализировать драйвер ЖКИ 
	U_MLT_LCD_Init();
	
	// Дождаться завершения операции с дисплеем 1
	while (U_MLT_Read_Status(1) & 0x80);
	// Включить дисплей 1  
	U_MLT_Disp_On (1);
	
	// Дожаться завершения операции с дисплеем 2
	while (U_MLT_Read_Status(2) & 0x80);
	// Включить дисплей 2 
	U_MLT_Disp_On (2);

	// Очистить дисплей 1 
	U_MLT_Clear_Chip (1);
	// Очистить дисплей 2
	U_MLT_Clear_Chip (2);	
	
	// Вывести начальное сообщение и рисунок
  U_MLT_Put_String (init_string, 1);	
	U_MLT_Put_Image (init_image, 0, 0, 1, 1);

}

// Инициализация выводов МК для работы с ЖКИ
void U_MLT_Pin_Cfg (void)
{
	// Структура данных для инициализации портов
	PORT_InitTypeDef PortInit;
	
  // Разрешить тактирование требуемых периферийных устройств
  RST_CLK_PCLKcmd (RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTF, ENABLE);	
	
	// Заполнить структура общими для всех линий данными
	PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;
	PortInit.PORT_PD = PORT_PD_DRIVER;
	PortInit.PORT_GFEN = PORT_GFEN_OFF;

	// Линии 0..5 порта PORTA использовать как двунаправленную шину данных 
	PortInit.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1 | PORT_Pin_2 | PORT_Pin_3 | PORT_Pin_4 | PORT_Pin_5);
	PortInit.PORT_OE    = PORT_OE_IN;
	PortInit.PORT_FUNC  = PORT_FUNC_PORT;
	PortInit.PORT_MODE  = PORT_MODE_DIGITAL;
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST; // !!! Важно использовать максимальную скорость (короткие фронты), иначе (при длинных фронтах) не работает 
 	PORT_Init(MDR_PORTA, &PortInit);
	
	// Линии 2,3 порта PORTF также использовать в составе двунаправленной шину данных 
	PortInit.PORT_Pin   = (PORT_Pin_2 | PORT_Pin_3);
	PORT_Init(MDR_PORTF, &PortInit);

	// Линии 7..10 порта PORTB сделать цифровыми выходами
	PortInit.PORT_Pin   = PORT_Pin_7 | PORT_Pin_8 | PORT_Pin_9 | PORT_Pin_10;
	PortInit.PORT_OE    = PORT_OE_OUT;
	PORT_Init(MDR_PORTB, &PortInit);

	// Линии 0,1 порта PORTC также сделать цифровыми выходами
	PortInit.PORT_Pin   = (PORT_Pin_0 | PORT_Pin_1);
	PORT_Init(MDR_PORTC, &PortInit);
}

// Инициализация драйвера ЖКИ
void U_MLT_LCD_Init(void)
{
	// Сформировать сигнал RESET
	U_MLT_Clr_Res_Pin;
	U_MLT_Delay(200);
	
	U_MLT_Set_Res_Pin;
	U_MLT_Delay(5);

	// Дождаться окончания инициализации
	while ((U_MLT_Read_Status (1) & 0x90) && (U_MLT_Read_Status (2) & 0x90));	
}

// Выставить данные на шину данных
void U_MLT_Set_Data_Bits (uint8_t value)
{
	MDR_PORTA->RXTX &= 0xFFC0;
	MDR_PORTA->RXTX |= (value & 0x3F); 
	MDR_PORTF->RXTX &= 0xFFF3;
	MDR_PORTF->RXTX |= ((value >> 4) & 0x0c); 
}

// Задержка 
void U_MLT_Delay (uint32_t value)
{
	volatile uint32_t x = value;
	
	while ((x--) != 0);
}

// Получить статус операции
uint8_t U_MLT_Read_Status (uint8_t Chip)
{
  uint8_t data;

	// Настроить шину данных на прием данных
	U_MLT_Data_Dir_Input_PA;
	U_MLT_Data_Dir_Input_PF;
	
	// Если выбран дисплей 1, то установить вывод Е1 (дисплей 1 выбран)
	if (Chip == 1) 
	{
    U_MLT_Set_E1_Pin;
  }
	// Если выбран дисплей 2, то установить вывод Е2 (дисплей 2 выбран)
	else 
  {
    U_MLT_Set_E2_Pin;
  }

	// Сбросить вывод А0
	U_MLT_Clr_A0_Pin;

	// Установить вывод RW (прием данных)
  U_MLT_Set_RW_Pin;
	U_MLT_Delay(5);
	
	// Установить вывод E
	U_MLT_Set_Stb_Pin;
	U_MLT_Delay(5);
	
	// Прочитать данные
	data = U_MLT_Output_Data;
	U_MLT_Delay(5);
	
	// Сбросить вывод E
	U_MLT_Clr_Stb_Pin;
	U_MLT_Delay(5);
	
	// Сбросить вывод A0
	U_MLT_Clr_A0_Pin;

	// Сбросить вывод RW
	U_MLT_Clr_RW_Pin;

	// Если выбран дисплей 1, то сбросить вывод Е1 (дисплей 1 НЕ выбран)
	if (Chip == 1) 
	{
    U_MLT_Clr_E1_Pin;
  }
	// Если выбран дисплей 2, то сбросить вывод Е2 (дисплей 1 НЕ выбран)
	else 
  {
    U_MLT_Clr_E2_Pin;
  }

	return data;
}

// Включить дисплей
void U_MLT_Disp_On (uint8_t Chip)
{
	// Если выбран дисплей 1, то установить вывод Е1 (дисплей 1 выбран)
	if (Chip == 1) 
	{
    U_MLT_Set_E1_Pin;
  }
	// Если выбран дисплей 2, то установить вывод Е2 (дисплей 2 выбран)
	else 
  {
    U_MLT_Set_E2_Pin;
  }

	// Сбросить вывод А0
	U_MLT_Clr_A0_Pin;

	// Сбросить вывод RW (передача данных)
	U_MLT_Clr_RW_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Data_Dir_Output_PA;
	U_MLT_Data_Dir_Output_PF;
	
	// Выдать данные на шину
	U_MLT_Set_Data_Bits (0x3F);
	U_MLT_Delay(5);

	U_MLT_Set_Stb_Pin;
	U_MLT_Delay(5);

	U_MLT_Clr_Stb_Pin;
	U_MLT_Delay(5);

  U_MLT_Clr_A0_Pin;

	U_MLT_Clr_RW_Pin;


	U_MLT_Data_Dir_Input_PA;
	U_MLT_Data_Dir_Input_PF;

	// Если выбран дисплей 1, то сбросить вывод Е1 (дисплей 1 НЕ выбран)
	if (Chip == 1) 
	{
    U_MLT_Clr_E1_Pin;
  }
	// Если выбран дисплей 2, то сбросить вывод Е2 (дисплей 1 НЕ выбран)
	else 
  {
    U_MLT_Clr_E2_Pin;
  }
}

// Включить и выключить дисплей
void U_MLT_Disp_Off (uint8_t Chip)
{
	// Если выбран дисплей 1, то установить вывод Е1 (дисплей 1 выбран)
	if (Chip == 1) 
	{
    U_MLT_Set_E1_Pin;
  }
	// Если выбран дисплей 2, то установить вывод Е2 (дисплей 2 выбран)
	else 
  {
    U_MLT_Set_E2_Pin;
  }

	U_MLT_Clr_A0_Pin;

	U_MLT_Clr_RW_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Data_Dir_Output_PA;
	U_MLT_Data_Dir_Output_PF;
	
	U_MLT_Set_Data_Bits (0x3E);
	U_MLT_Delay(5);
	
	U_MLT_Set_Stb_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Clr_Stb_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Clr_A0_Pin;

	U_MLT_Clr_RW_Pin;

	U_MLT_Data_Dir_Input_PA;
	U_MLT_Data_Dir_Input_PF;
	
	// Если выбран дисплей 1, то сбросить вывод Е1 (дисплей 1 НЕ выбран)
	if (Chip == 1) 
	{
    U_MLT_Clr_E1_Pin;
  }
	// Если выбран дисплей 2, то сбросить вывод Е2 (дисплей 1 НЕ выбран)
	else 
  {
    U_MLT_Clr_E2_Pin;
  }
}

// Задать текущую страницу
void U_MLT_Set_Page (uint8_t Chip, uint8_t page)
{
	// Если выбран дисплей 1, то установить вывод Е1 (дисплей 1 выбран)
	if (Chip == 1) 
	{
    U_MLT_Set_E1_Pin;
  }
	// Если выбран дисплей 2, то установить вывод Е2 (дисплей 2 выбран)
	else 
  {
    U_MLT_Set_E2_Pin;
  }

	U_MLT_Clr_A0_Pin;

	U_MLT_Clr_RW_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Data_Dir_Output_PA;
	U_MLT_Data_Dir_Output_PF;
	
	U_MLT_Set_Data_Bits (0xB8 | page);
	U_MLT_Delay(5);
	
	U_MLT_Set_Stb_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Clr_Stb_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Data_Dir_Input_PA;
	U_MLT_Data_Dir_Input_PF;

	U_MLT_Clr_A0_Pin;

	U_MLT_Clr_RW_Pin;

	// Если выбран дисплей 1, то сбросить вывод Е1 (дисплей 1 НЕ выбран)
	if (Chip == 1) 
	{
    U_MLT_Clr_E1_Pin;
  }
	// Если выбран дисплей 2, то сбросить вывод Е2 (дисплей 1 НЕ выбран)
	else 
  {
    U_MLT_Clr_E2_Pin;
  }
}

// Задать текущий адрес
void U_MLT_Set_Address (uint8_t Chip, uint8_t address)
{
	// Если выбран дисплей 1, то установить вывод Е1 (дисплей 1 выбран)
	if (Chip == 1) 
	{
    U_MLT_Set_E1_Pin;
  }
	// Если выбран дисплей 2, то установить вывод Е2 (дисплей 2 выбран)
	else 
  {
    U_MLT_Set_E2_Pin;
  }

	U_MLT_Clr_A0_Pin;

	U_MLT_Clr_RW_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Data_Dir_Output_PA;
	U_MLT_Data_Dir_Output_PF;
	
	U_MLT_Set_Data_Bits (0x40 | address);
	U_MLT_Delay(5);
	
	U_MLT_Set_Stb_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Clr_Stb_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Data_Dir_Input_PA;
	U_MLT_Data_Dir_Input_PF;

	U_MLT_Clr_A0_Pin;

	U_MLT_Clr_RW_Pin;

	// Если выбран дисплей 1, то сбросить вывод Е1 (дисплей 1 НЕ выбран)
	if (Chip == 1) 
	{
    U_MLT_Clr_E1_Pin;
  }
	// Если выбран дисплей 2, то сбросить вывод Е2 (дисплей 1 НЕ выбран)
	else 
  {
    U_MLT_Clr_E2_Pin;
  }
}

// Записать данные в ЖКИ
void U_MLT_Write_Data (uint8_t Chip, uint8_t data)
{
	// Если выбран дисплей 1, то установить вывод Е1 (дисплей 1 выбран)
	if (Chip == 1) 
	{
    U_MLT_Set_E1_Pin;
  }
	// Если выбран дисплей 2, то установить вывод Е2 (дисплей 2 выбран)
	else 
  {
    U_MLT_Set_E2_Pin;
  }

	U_MLT_Set_A0_Pin;

	U_MLT_Clr_RW_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Data_Dir_Output_PA;
	U_MLT_Data_Dir_Output_PF;
	
	U_MLT_Set_Data_Bits (data);
	U_MLT_Delay(5);
	
	U_MLT_Set_Stb_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Clr_Stb_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Data_Dir_Input_PA;
	U_MLT_Data_Dir_Input_PF;

	U_MLT_Clr_A0_Pin;

	U_MLT_Clr_RW_Pin;

	// Если выбран дисплей 1, то сбросить вывод Е1 (дисплей 1 НЕ выбран)
	if (Chip == 1) 
	{
    U_MLT_Clr_E1_Pin;
  }
	// Если выбран дисплей 2, то сбросить вывод Е2 (дисплей 1 НЕ выбран)
	else 
  {
    U_MLT_Clr_E2_Pin;
  }
}

// Прочитать данные с ЖКИ
uint8_t U_MLT_Read_Data (uint8_t Chip)
{
  uint8_t data = 0;

	// Если выбран дисплей 1, то установить вывод Е1 (дисплей 1 выбран)
	if (Chip == 1) 
	{
    U_MLT_Set_E1_Pin;
  }
	// Если выбран дисплей 2, то установить вывод Е2 (дисплей 2 выбран)
	else 
  {
    U_MLT_Set_E2_Pin;
  }

	U_MLT_Set_A0_Pin;

	U_MLT_Set_RW_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Data_Dir_Input_PA;
	U_MLT_Data_Dir_Input_PF;
	
	U_MLT_Set_Data_Bits (data);
	U_MLT_Delay(5);
	
	U_MLT_Set_Stb_Pin;
	U_MLT_Delay(5);
	
	data = U_MLT_Output_Data;
	U_MLT_Delay(5);
	
	U_MLT_Clr_Stb_Pin;
	U_MLT_Delay(5);
	
	U_MLT_Clr_A0_Pin;

	U_MLT_Clr_RW_Pin;

	// Если выбран дисплей 1, то сбросить вывод Е1 (дисплей 1 НЕ выбран)
	if (Chip == 1) 
	{
    U_MLT_Clr_E1_Pin;
  }
	// Если выбран дисплей 2, то сбросить вывод Е2 (дисплей 1 НЕ выбран)
	else 
  {
    U_MLT_Clr_E2_Pin;
  }

	return (data);
}

// Очистить страницу
void U_MLT_Clear_Page (uint8_t Chip, uint8_t Page)
{
  int32_t i;

	U_MLT_Set_Page (Chip, Page);

	U_MLT_Set_Address (Chip, 0x00);

	for (i = 0; i < 64; i++)
	{	
		while (U_MLT_Read_Status (Chip) & 0x80);
		
		U_MLT_Write_Data (Chip, 0x00);
	}
}

// Очистить дисплей
void U_MLT_Clear_Chip (uint8_t Chip)
{
  int32_t i;

	for (i = 0; i < 8; i++)
		U_MLT_Clear_Page (Chip, i);
}

// Вывести символ
void U_MLT_Put_Char (uint8_t symbol, int32_t X, int32_t Y)
{
  int32_t chp, page, adr;
  int32_t i;
	int32_t symbol_image_index;  // Индекс элемента в массиве шрифта, с которого начинается изображение данного символа.

	chp  = (int32_t)(X / 8) + 1;
	page = Y;
	adr  = (int32_t)((X % 8) * 8);	

	U_MLT_Set_Page (chp, page);
	U_MLT_Set_Address (chp, adr);
	
	// Проверить, чтобы символ не выходил за диапазон, определенный в шрифте.
	if (symbol >= MLT_FONT_MIN_CODE && symbol <= MLT_FONT_MAX_CODE)
	{
	  symbol_image_index = (((uint32_t) symbol) - MLT_FONT_MIN_CODE) * 8;
	}
	// Символ вне диапазона
	else
	{
	  symbol_image_index = 0;  // Выводим самый первый символ, как правило,- пробел.
  }
	
	for (i = symbol_image_index; i < symbol_image_index + 8; i++)
	{	
		while (U_MLT_Read_Status (chp) & 0x80);
		
		U_MLT_Write_Data (chp, MLT_Font[i]);
	}

}

// Вывести строку
void U_MLT_Put_String (const char* str, int32_t Y)
{
  int32_t i;

	for (i = 0; i < U_MLT_SCREEN_WIDTH_SYMBOLS; i++)
	{
	  // Достигнут конец строки
		if (!str[i])
      break;

    U_MLT_Put_Char (str[i], i, Y); // Вывести символ
  }
	
	for (; i < U_MLT_SCREEN_WIDTH_SYMBOLS; i++)
	{
 	  U_MLT_Put_Char (32, i, Y);  // Вывести пробел
  }
}

// Прокрутить строку на cnt шагов и вывести ее
void U_MLT_Scroll_String (const char* str, int32_t Y, int32_t cnt)
{
  int32_t i, j, L;
	char c;
	char s[U_MLT_SCREEN_WIDTH_SYMBOLS + 1]; // Строка размером с ЖКИ, которую получим в результате прокрутки.
  
	L = strlen(str); // Длина исходной строки
	s[U_MLT_SCREEN_WIDTH_SYMBOLS] = 0;      // Получаемую строку надо закрыть финишным нулем
	
	// Если исходная строка пустая 
	if (!L)
		return; 

  // Если пытаемся несколько раз обойти вокруг Земли :-)	
	cnt %= U_MLT_SCREEN_WIDTH_SYMBOLS;

	// Если исходная строка короче экрана
	if (L < U_MLT_SCREEN_WIDTH_SYMBOLS)
		for (i = 0; i < U_MLT_SCREEN_WIDTH_SYMBOLS; i++)
		{
			j = i + cnt;

			if (j >= U_MLT_SCREEN_WIDTH_SYMBOLS)
				j %= U_MLT_SCREEN_WIDTH_SYMBOLS;
			
			// Если исходная строка еще не кончилась
			if (j < L)  			
				c = str[j];
			// Если исходная строка уже кончилась
			else
				c = 32; // Пробел
			
			s[i] = c;
		}
	// Если исходная строка длиннее экрана
	else
		for (i = 0; i < U_MLT_SCREEN_WIDTH_SYMBOLS; i++)
		{
			j = i + cnt;

			if (j >= L)
				j %= L;
			
      s[i] = str[j];
		}
	
	// Вывести прокрученную строку на ЖКИ	
	U_MLT_Put_String (s, Y);
}

// Вывести изображение
void U_MLT_Put_Image (const uint8_t* image, int32_t top, int32_t left, int32_t bottom, int32_t right)
{
	int32_t i, j, y;
	int32_t chp, page, adr;
	int32_t cnt = 0;	

	for	(i = top; i <= bottom; i++)
		for (j = left; j <= right; j++)
		{
			chp  = (int32_t)(j / 8) + 1;
			page = i;
			adr  = (int32_t)((j % 8) * 8);

			U_MLT_Set_Page (chp, page);
			U_MLT_Set_Address (chp, adr);
			
			for (y = 0; y < 8; y++)
			{
  		  while (U_MLT_Read_Status (chp) & 0x80);

				U_MLT_Write_Data (chp, image[cnt + y]);
			}
			cnt += 8;
		}
}
