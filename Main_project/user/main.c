/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_eeprom.h"  //библиотека для работы с EEPROM
#include "MDR32F9Qx_port.h"	//библиотека для работы с портами
#include "MDR32F9Qx_rst_clk.h" //библиотека для тактирования
#include "MDR32F9Qx_adc.h"	 //библиотека для работы с АЦП
#include "MDR32F9Qx_dac.h"	 //библиотека для работы с ЦАП

#define EEPROM_PAGE_SIZE (4 * 1024) //1 страница памяти - 4К
#define MAIN_EEPAGE 5
#define ENTIRE_MEM_BYTES 86016 // 0x15000 - свободная память до 0x08020000

/* Процедура задержки */
void Delay(int num)
{
	volatile uint32_t i = 0;
	for (i = 0; i < num; i++)
	{
	}
}

//Настройка частоты работы
void MY_U_RST_Init(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP, ENABLE);
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	while (RST_CLK_HSEstatus() != SUCCESS)
		;

	//12 Мгц
	RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv2,
						  RST_CLK_CPU_PLLmul5);

	RST_CLK_CPU_PLLcmd(ENABLE);

	while (RST_CLK_CPU_PLLstatus() != SUCCESS)
		;

	RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
	RST_CLK_CPU_PLLuse(ENABLE);
	RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
}

/* функция считывания результата преобразования. 
После считывания флаг ADC1_STATUS->Flg_REG_EOCIF будет сброшен
следующее преобразование начнется автоматически через 2 такта*/
uint16_t ADC_Receive_Word()
{
	uint16_t result;
	while (MDR_ADC->ADC1_STATUS & 0x00000004 == 0)
	{
	} //while conversation
	result = MDR_ADC->ADC1_RESULT;
	return result;
}

//===============================================================================
//=========    ОПИСАНИЕ ФУНКЦИЙ ЧТЕНИЯ И ЗАПИСИ В ПАМЯТЬ    =====================
//===============================================================================

//Процедура очистки памяти
void erise_mem()
{
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint32_t i = 0;
	Address = 0x08000000 + EEPROM_PAGE_SIZE * MAIN_EEPAGE;
	BankSelector = EEPROM_Main_Bank_Select;

	//очистка трека (21 странца)
	for (i = 0; i < 21; i++)
	{
		EEPROM_ErasePage(Address + i * EEPROM_PAGE_SIZE, BankSelector);
	}
}

/*Процедура считывания значения напряжения на АЦП и записи в память */
void write_track()
{
	uint32_t Address = 0;
	uint32_t size_in_bytes = ENTIRE_MEM_BYTES; //количество байт для записи
	uint32_t BankSelector = 0;
	uint16_t Data;	 //16 для записи напряжения
	uint8_t Data_8bit; //8 бит для записи результата в память
	uint32_t i = 0;
	Address = 0x08000000 + EEPROM_PAGE_SIZE * MAIN_EEPAGE;
	BankSelector = EEPROM_Main_Bank_Select;

	//Запуск последовательного преобразвоания
	MDR_ADC->ADC1_CFG |= ADC1_CFG_REG_SAMPLE;

	//цикл записи в память
	for (i = 4; i < size_in_bytes; i++)
	{
		Data = ADC_Receive_Word();
		//отбрасывание младших 4 разрядов
		Data_8bit = (uint8_t)(Data >> 4);
		//запись очередного значения в память
		EEPROM_ProgramByte(Address + i, BankSelector, Data_8bit);
	}

	//остановка последовательного преобразования
	MDR_ADC->ADC1_CFG &= ~(ADC1_CFG_REG_SAMPLE);
	//создание метки
	EEPROM_ProgramWord(Address, BankSelector, 0xABCDEFAB);
}

/* Процедура считывания памяти и вывода на ЦАП (AUDIO)  
norm = 0 - без нормализации, norm = 1 - с нормализацией*/
void read_track(int norm)
{
	uint32_t size_in_bytes = ENTIRE_MEM_BYTES;
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint16_t Data = 0; //16 бит для вывода на ЦАП
	uint8_t Data_8bit; //8 бит для считывания из памяти
	uint32_t i = 0;
	Address = 0x08000000 + EEPROM_PAGE_SIZE * MAIN_EEPAGE;
	BankSelector = EEPROM_Main_Bank_Select;

	//Вычисление Max и Min для Нормализации
	uint32_t min = 0xFFFF;
	uint32_t max = 0x0000;

	for (i = 4; i < size_in_bytes; i++)
	{
		Data_8bit = (EEPROM_ReadByte(Address + i, BankSelector)) & 0xFF;
		Data = ((0x0000 + Data_8bit) << 4) + 0x7;
		if (Data > max)
		{
			max = Data;
		}
		if (Data < min)
		{
			min = Data;
		}
	}

	//Цикл последовательного вывода значений из памяти на ЦАП
	for (i = 4; i < size_in_bytes; i++)
	{

		Data_8bit = (EEPROM_ReadByte(Address + i, BankSelector)) & 0xFF;
		Data = ((0x0000 + Data_8bit) << 4) + 0x7;
		if (norm == 1)
		{
			//Нормализация
			Data = (uint16_t)(((double)(Data - min)) * (((double)0xFFF) / ((double)(max - min))));
			//Ограничение результатов, больших 0xFFF
			if (Data > 0xFFF)
			{
				Data = 0xFFF;
			}
			//Задержка для корректной скорости проигрывания при использовании нормализации
			Delay(70);
		}
		else
		{
			//Задержка для корректной скорости проигрывания без нормализации
			Delay(105);
		}
		//вывод очередного значения на ЦАП
		DAC2_SetData(Data);
	}
}

/* Функция проверки, записан ли трек под номером num или пуст */
int track_is_empty()
{
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	Address = 0x08000000 + EEPROM_PAGE_SIZE * MAIN_EEPAGE;
	BankSelector = EEPROM_Main_Bank_Select;

	if (EEPROM_ReadWord(Address, BankSelector) == 0xABCDEFAB)
	{
		return 0; //трек пока не записан
	}
	else
	{
		return 1; //трек записан
	}
}

//===============================================================================
//=========  КОНЕЦ ОПИСАНИЯ ФУНКЦИЙ ЧТЕНИЯ И ЗАПИСИ В ПАМЯТЬ   ==================
//===============================================================================

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
	ADC_1_Nastroyka.ADC_ChannelNumber = ADC_CH_ADC7;				  //выбор канала
	ADC_1_Nastroyka.ADC_SamplingMode = ADC_SAMPLING_MODE_CICLIC_CONV; //режим последовательного преобразования
	ADC_1_Nastroyka.ADC_DelayGo = 0b010;							  //задержка между преобразованиями - 2 такта
	ADC1_Init(&ADC_1_Nastroyka);
	ADC1_Cmd(ENABLE);
}

/* настройка ЦАП  */
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
	DAC2_Cmd(ENABLE);	 //включение АЦП
}

/* Функция определения статуа (нажата или нет) кнопок (на пульте оператора)*/
//btn_name = 0 - кнопка SELECT (Очистка памяти)
//btn_name = 2 - кнопка RIGHT (Запись трека)
//btn_name = 3 - кнопка LEFT (Воспроизведение трека)
int current_btn_status(int btn_name)
{
	int status = 0xA;

	//SELECT
	if (btn_name == 0)
	{
		status = PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_2);
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

	//Инвертирование результата (1 - нажата, 0 - не нажата)
	if (status == 1)
	{
		return 0; //не нажата
	}
	if (status == 0)
	{
		return 1; //нажата
	}

	return 0xA; //ошибка
}

/*  Настройка портов ввода-вывода для кнопок  */
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

/* Главноя функция */
int32_t main(void)
{
	//char track_array[1][32] = {"\xD2\xF0\xE5\xEA 1"};
	/* определение выводимых на экран строк (кириллица) */
	char ochistka[] = "\xCE\xF7\xE8\xF1\xF2\xEA\xE0";
	char steret[] = "SELECT:\xD1\xF2\xE5\xF0\xE5\xF2\xFC";
	char pamyati[] = "\xEF\xE0\xEC\xFF\xF2\xE8...";
	char zapis[] = "\xC7\xE0\xEF\xE8\xF1\xFC...";
	char zapis_short[] = "RIGHT :\xC7\xE0\xEF\xE8\xF1\xE0\xF2\xFC";
	char zapisan[] = "\xD3\xE6\xE5 \xC7\xE0\xEF\xE8\xF1\xE0\xED";
	char vosproizvedenie[] = "\xC2\xEE\xF1\xEF\xF0\xE8\xE7\xE2\xE5\xE4\xE5\xED\xE8\xE5...";
	char vosproizvedenie_short[] = "LEFT  :\xC2\xEE\xF1\xEF\xF0\xE8\xE7\xE2.";
	char pusto[] = "\xCF\xF3\xF1\xF2\xEE!";
	char smirnov[] = "\xD1\xEC\xE8\xF0\xED\xEE\xE2 \xC0.\xC0.";
	char iu673[] = "\xC8\xD3\x36-73";
	/* Включение тактирование EEPROM */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
	/* вызов инициализирующих функций */
	U_MLT_Init();
	BUTTONS_Init();
	MY_ADC_Init();
	MY_ADC_1_Init();
	MY_DAC2_Init();

	//Установка частоты тактирования
	MY_U_RST_Init();

	/* Вывод имени, фамилии и группы*/
	U_MLT_Put_String("", 0);
	U_MLT_Put_String(smirnov, 1);
	U_MLT_Put_String(iu673, 2);

	/* переменные текущего статуса кнопок */
	int current_status_select = 0; //кнопка очистка памяти
	int current_status_right = 0;  //кнопка записи трека
	int current_status_left = 0;   //кнопка воспроизведения трека

	/* Основной цикл проверки кнопок */
	while (1)
	{
		/* проверка нажатия кнопок */

		//SELECT -> Очистка памяти
		if (current_btn_status(0) == 1 && current_status_select == 0)
		{
			//обработка нажатия (начало)
			if (track_is_empty() == 1)
			{
				U_MLT_Put_String(pusto, 4);
				U_MLT_Put_String("", 5);
				U_MLT_Put_String("", 6);
				U_MLT_Put_String("", 7);
				Delay(5000000);
			}
			else
			{
				current_status_select = 1;
				U_MLT_Put_String(ochistka, 4);
				U_MLT_Put_String(pamyati, 5);
				U_MLT_Put_String("", 6);
				U_MLT_Put_String("", 7);
				erise_mem();
			}

			//обработка нажатия (конец)
		}
		current_status_select = current_btn_status(0);

		//RIGHT -> Запись трека
		if (current_btn_status(2) == 1 && current_status_right == 0)
		{
			//обработка нажатия (начало)
			if (track_is_empty() == 1){
				U_MLT_Put_String(zapis, 4);
				U_MLT_Put_String("", 5);
				U_MLT_Put_String("", 6);
				U_MLT_Put_String("", 7);
				write_track();
			} else {
				U_MLT_Put_String(zapisan, 4);
				U_MLT_Put_String("", 5);
				U_MLT_Put_String("", 6);
				U_MLT_Put_String("", 7);
				Delay(5000000);
			}

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
				U_MLT_Put_String("", 5);
				U_MLT_Put_String("", 6);
				U_MLT_Put_String("", 7);
				Delay(5000000);
			}
			else
			{
				U_MLT_Put_String(vosproizvedenie, 4);
				U_MLT_Put_String("", 5);
				U_MLT_Put_String("", 6);
				U_MLT_Put_String("", 7);
				read_track(1); //воспроизведение трека
			}
			//обработка нажатия (конец)
		}
		current_status_left = current_btn_status(3);

		//Вывод селект-листа треков на экран
		U_MLT_Put_String(zapis_short, 4);
		U_MLT_Put_String(steret, 5);
		U_MLT_Put_String(vosproizvedenie_short, 6);
		U_MLT_Put_String("", 7);
		Delay(500);
	}
}
