/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_eeprom.h"  //библиотека для работы с EEPROM
#include "MDR32F9Qx_port.h"	//библиотека для работы с портами
#include "MDR32F9Qx_rst_clk.h" //библиотека для тактирования
#include "MDR32F9Qx_adc.h"	 //библиотека для работы с АЦП
#include "MDR32F9Qx_dac.h"	 //библиотека для работы с ЦАП
#include <cmath>			   // библиотека для отладки вывода звука (sin...)
#include "MDR32F9Qx_timer.h"            // Keil::Drivers:TIMER
#define EEPROM_PAGE_SIZE (4 * 1024) //1 страница памяти - 4К
#define MAIN_EEPAGE 5
#define ENTIRE_MEM_HALF_WORDS 43008 // 0x15000 - свободная память до 0x08020000
int timer_count = 0;

//Настройка частоты работы
void MY_U_RST_Init(void)
{
	RST_CLK_PCLKcmd (RST_CLK_PCLK_BKP, ENABLE);
	RST_CLK_HSEconfig (RST_CLK_HSE_ON);
	while (RST_CLK_HSEstatus () != SUCCESS);

	//40 MHz
	RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSEdiv1,
	RST_CLK_CPU_PLLmul5);
	
	RST_CLK_CPU_PLLcmd (ENABLE);

	while (RST_CLK_CPU_PLLstatus () != SUCCESS);

	RST_CLK_CPUclkPrescaler (RST_CLK_CPUclkDIV1);
	RST_CLK_CPU_PLLuse (ENABLE);
	RST_CLK_CPUclkSelection (RST_CLK_CPUclkCPU_C3);
}

//===============================================================================
//=========    ОПИСАНИЕ ФУНКЦИЙ ДЛЯ РАБОТЫ С ПАМЯТЬЮ    =========================
//===============================================================================

//Функция очистки областей памяти, куда были записаны треки
void erise_mem()
{
	//всего 26624 слова по 32 бита памяти доступно
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint32_t i = 0;
	Address = 0x08000000 + EEPROM_PAGE_SIZE*MAIN_EEPAGE;
	BankSelector = EEPROM_Main_Bank_Select;

	//очистка трека (26 страниц)
	for (i = 0; i < 21; i++)
	{
		EEPROM_ErasePage(Address + i * EEPROM_PAGE_SIZE, BankSelector);
	}

}

/*Функция считывания значения напряжения на АЦП и записи в память EEPROM
num >= 0 (1,2..) */
void write_track()
{
	//Write to EEPROM
	uint32_t Address = 0;
	uint32_t size_in_half_words = ENTIRE_MEM_HALF_WORDS; //same as EEPROM_PAGE_SIZE*26
	uint32_t BankSelector = 0;
	uint16_t Data; //16 бит на одну запись
	uint32_t i = 0;
	Address = 0x08000000 + EEPROM_PAGE_SIZE*MAIN_EEPAGE;
	BankSelector = EEPROM_Main_Bank_Select;													 

	for (i = 2; i < size_in_half_words; i++)
	{
		Data = ADC_Receive_Word();
		Delay(150);
		EEPROM_ProgramHalfWord (Address + i*2, BankSelector, Data);
	}

	//создание метки
	EEPROM_ProgramWord(Address, BankSelector, 0xABCDEFAB);
}

/* Функция считывания памяти и вывода на ЦАП (AUDIO)  */
void read_track()
{
	//чтение из EEPROM
	uint32_t size_in_half_words = ENTIRE_MEM_HALF_WORDS; //same as EEPROM_PAGE_SIZE*13
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint16_t Data = 0;
	uint32_t i = 0;
	int j = 0;

	char stroka[33];
	Address = 0x08000000 + EEPROM_PAGE_SIZE*MAIN_EEPAGE;
	BankSelector = EEPROM_Main_Bank_Select;
	for (i = 2; i < size_in_half_words; i++)
	{
		Data = (EEPROM_ReadHalfWord(Address + i*2, BankSelector)) & 0x0FFF;
		Delay(250);
		DAC2_SetData(Data);
	}

}

/* Функция проверки, записан ли трек под номером num или пуст 
num = 1,2... (>0)*/
int track_is_empty()
{
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint32_t Data = 0;
	uint32_t i = 0;
	Address = 0x08000000 + EEPROM_PAGE_SIZE*MAIN_EEPAGE;
	BankSelector = EEPROM_Main_Bank_Select;

	if (EEPROM_ReadWord(Address, BankSelector) == 0xABCDEFAB)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

//===============================================================================
//=========  КОНЕЦ ОПИСАНИЯ ФУНКЦИЙ ДЛЯ РАБОТЫ С ПАМЯТЬЮ   ======================
//===============================================================================

//btn_name = 0 - кнопка SELECT (Очистка памяти)
//btn_name = 1 - кнопка UP
//btn_name = 2 - кнопка RIGHT (Запись трека)
//btn_name = 3 - кнопка LEFT (Воспроизведение трека)
//btn_name = 4 - кнопка DOWN
int current_btn_status(int btn_name)
{
	int status = 0xA;

	//SELECT
	if (btn_name == 0)
	{
		status = PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_2);
	}

	//UP
	if (btn_name == 1)
	{
		status = PORT_ReadInputDataBit(MDR_PORTB, PORT_Pin_5);
	}

	//RIGHT
	if (btn_name == 2)
	{
		status = PORT_ReadInputDataBit(MDR_PORTB, PORT_Pin_6);
	}

	//LEFT
	if (btn_name == 3)
	{
		status = PORT_ReadInputDataBit(MDR_PORTE, PORT_Pin_3);
	}

	//DOWN
	if (btn_name == 4)
	{
		status = PORT_ReadInputDataBit(MDR_PORTE, PORT_Pin_1);
	}

	//Инвертирование результата (1 - нажата, 0 - не нажата)
	if (status == 1)
	{
		return 0;
	}
	if (status == 0)
	{
		return 1;
	}
	if (status != 0 && status != 1)
	{
		return 0xA;
	}
}

/* Настройка АЦП */
void MY_ADC_Init(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_ADC, ENABLE);
	ADC_InitTypeDef ADC_Nastroyka;
	ADC_StructInit(&ADC_Nastroyka);
	ADC_Init(&ADC_Nastroyka);
}

/* Настройка АЦП1 */
void MY_ADC_1_Init(void)
{
	ADCx_InitTypeDef ADC_1_Nastroyka;
	ADCx_StructInit(&ADC_1_Nastroyka);
	ADC_1_Nastroyka.ADC_ChannelNumber = ADC_CH_ADC7; //channel 7 connected to BNC
	ADC1_Init(&ADC_1_Nastroyka);
	ADC1_Cmd(ENABLE);
}

/*  Настройка кнопок  */
void BUTTONS_Init(void)
{

	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTC, ENABLE); //taktirovanie porta B, C, E

	//Настройка PB5 (UP) и PB6 (RIGHT)
	PORT_InitTypeDef Nastroyka_b;
	PORT_StructInit(&Nastroyka_b);
	Nastroyka_b.PORT_Pin = PORT_Pin_5 | PORT_Pin_6;
	Nastroyka_b.PORT_MODE = PORT_MODE_DIGITAL;
	Nastroyka_b.PORT_OE = PORT_OE_IN;
	Nastroyka_b.PORT_SPEED = PORT_SPEED_SLOW;
	Nastroyka_b.PORT_PD = PORT_PD_DRIVER;
	PORT_Init(MDR_PORTB, &Nastroyka_b);

	//Настройка PC2 (SELECT)
	PORT_InitTypeDef Nastroyka_c;
	PORT_StructInit(&Nastroyka_c);
	Nastroyka_c.PORT_Pin = PORT_Pin_2;
	Nastroyka_c.PORT_MODE = PORT_MODE_DIGITAL;
	Nastroyka_c.PORT_OE = PORT_OE_IN;
	Nastroyka_c.PORT_SPEED = PORT_SPEED_SLOW;
	Nastroyka_c.PORT_PD = PORT_PD_DRIVER;
	PORT_Init(MDR_PORTC, &Nastroyka_c);

	//Настройка PE3 (LEFT) и PE1 (DOWN)
	PORT_InitTypeDef Nastroyka_e;
	PORT_StructInit(&Nastroyka_e);
	Nastroyka_e.PORT_Pin = PORT_Pin_3 | PORT_Pin_1;
	Nastroyka_e.PORT_MODE = PORT_MODE_DIGITAL;
	Nastroyka_e.PORT_OE = PORT_OE_IN;
	Nastroyka_e.PORT_SPEED = PORT_SPEED_SLOW;
	Nastroyka_e.PORT_PD = PORT_PD_DRIVER;
	PORT_Init(MDR_PORTE, &Nastroyka_e);
}

/* Функция задержки */
void Delay(int num)
{
	volatile uint32_t i = 0;
	for (i = 0; i < num; i++)
	{
	}
}

/* функция считывания результата преобразования. 
После считывания флаг будет сброшен и может быть произведено
следующее преобразование командой ADC1_Start()*/
uint16_t ADC_Receive_Word()
{
	uint16_t result;
	ADC1_Start();
	while (ADC1_GetFlagStatus(ADC1_FLAG_END_OF_CONVERSION) == 0);
	result = ADC1_GetResult() & 0x00000FFF;//получение 12-разрядного результата
	return result;
}

/* Функция настройки ЦАП  */
void MY_DAC2_Init(void)
{
	//Настройка порта для вывода аналогового сигнала на AUDIO выход

	//Тактирование порта вывода - PORTE и ЦАП
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE | RST_CLK_PCLK_DAC, ENABLE);
	PORT_InitTypeDef PORT_InitStructure;
	PORT_InitStructure.PORT_Pin = PORT_Pin_0; //PE0
	PORT_InitStructure.PORT_OE = PORT_OE_OUT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_ANALOG;
	PORT_Init(MDR_PORTE, &PORT_InitStructure);

	DAC2_Init(DAC2_AVCC); //выбор опорного напряжения (3.3V)
	DAC2_Cmd(ENABLE);
}

int32_t main(void)
{
	int current_track = 0; //current_track = 0,1,...
	int num_of_tracks = 1;
	char stroka[40]; //строка для хранения и вывода результата вычислений
	char track_array[10][32] = {"\xD2\xF0\xE5\xEA 1", "\xD2\xF0\xE5\xEA 2", "\xD2\xF0\xE5\xEA 3", "\xD2\xF0\xE5\xEA 4", "\xD2\xF0\xE5\xEA 5", "\xD2\xF0\xE5\xEA 6", "\xD2\xF0\xE5\xEA 7", "\xD2\xF0\xE5\xEA 8", "\xD2\xF0\xE5\xEA 9", "\xD2\xF0\xE5\xEA 10"};
	/* определение выводимых на экран строк (кириллица) */
	char ochistka[] = "\xCE\xF7\xE8\xF1\xF2\xEA\xE0";
	char pamyati[] = "\xEF\xE0\xEC\xFF\xF2\xE8...";
	char zapis[] = "\xC7\xE0\xEF\xE8\xF1\xFC...";
	char vosproiz[] = "\xC2\xEE\xF1\xEF\xF0\xE8\xE7";
	char vedenie[] = "\xE2\xE5\xE4\xE5\xED\xE8\xE5...";
	char vosproizvedenie[] = "\xC2\xEE\xF1\xEF\xF0\xE8\xE7\xE2\xE5\xE4\xE5\xED\xE8\xE5...";
	char pusto[] = "\xCF\xF3\xF1\xF2\xEE!";
	char smirnov[] = "\xD1\xEC\xE8\xF0\xED\xEE\xE2 \xC0.\xC0.";
	char iu673[] = "\xC8\xD3\x36-73";
	/* Включение тактирование EEPROM */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);

	U_MLT_Init();
	BUTTONS_Init();
	MY_ADC_Init();
	MY_ADC_1_Init();
	MY_DAC2_Init();
	//Установка частоты тактирования 80МГц
	MY_U_RST_Init();

	/* Вывод имени, фамилии и группы*/
	U_MLT_Put_String("", 0);
	U_MLT_Put_String(smirnov, 1);
	U_MLT_Put_String(iu673, 2);

	/* переменные текущего статуса кнопок */
	int current_status_up = 0;	 //кнопка перехода к треку выше
	int current_status_down = 0;   //кнопка перехода к треку ниже
	int current_status_select = 0; //кнопка очистка памяти
	int current_status_right = 0;  //кнопка записи трека
	int current_status_left = 0;   //кнопка воспроизведения трека

	/* Основной цикл */
	while (1)
	{
		/* проверка нажатия кнопок */

		//UP -> Перейти к треку выше
		if (current_btn_status(1) == 1 && current_status_up == 0)
		{
			//обработка нажатия (начало)
			current_status_up = 1;
			current_track += 1;
			if (current_track == num_of_tracks)
				current_track = 0;
			//обработка нажатия (конец)
		}
		current_status_up = current_btn_status(1);

		//DOWN -> Перейти к треку ниже
		if (current_btn_status(4) == 1 && current_status_down == 0)
		{
			//обработка нажатия (начало)
			current_status_down = 1;
			current_track -= 1;
			if (current_track == -1)
				current_track = num_of_tracks - 1;
			//обработка нажатия (конец)
		}
		current_status_down = current_btn_status(4);

		//SELECT -> Очистка памяти
		if (current_btn_status(0) == 1 && current_status_select == 0)
		{
			//обработка нажатия (начало)
			current_status_select = 1;
			U_MLT_Put_String(ochistka, 4);
			U_MLT_Put_String(pamyati, 5);
			erise_mem();

			Delay(500000);
			//обработка нажатия (конец)
		}
		current_status_select = current_btn_status(0);

		//RIGHT -> Запись трека
		if (current_btn_status(2) == 1 && current_status_right == 0)
		{
			//обработка нажатия (начало)
			U_MLT_Put_String(zapis, 4);
			write_track();
			Delay(500000);
			//обработка нажатия (конец)
		}
		current_status_right = current_btn_status(2);

		//LEFT -> Воспроизведение трека
		if (current_btn_status(3) == 1 && current_status_left == 0)
		{
			//обработка нажатия (начало)
			if (track_is_empty() == 1)
			{
				U_MLT_Put_String(pusto, 4);
			}
			else
			{
				U_MLT_Put_String(vosproizvedenie, 4);
				read_track();
			}
			Delay(500000);
			//обработка нажатия (конец)
		}
		current_status_left = current_btn_status(3);

		//Вывод селект-листа треков на экран
		U_MLT_Put_String(track_array[current_track], 4);
		U_MLT_Put_String("", 5);
		Delay(500);
	}
}
