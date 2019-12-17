/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_eeprom.h"  //библиотека для работы с EEPROM
#include "MDR32F9Qx_port.h"	//библиотека для работы с портами
#include "MDR32F9Qx_rst_clk.h" //библиотека для тактирования
#include "MDR32F9Qx_adc.h"	 //библиотека для работы с АЦП
#include "MDR32F9Qx_dac.h"	 //библиотека для работы с ЦАП
#include "MDR32F9Qx_timer.h"    // библиотека для работы с таймером


#define EEPROM_PAGE_SIZE (4 * 1024) //1 страница памяти - 4К
#define MAIN_EEPAGE 5
#define ENTIRE_MEM_BYTES 110592 // свободная память до 0x08020000
#define CPU_FREQ 20000000 //20МГц

int global_byte_counter = 4; //начиная с 4 бита 
uint8_t global_adc_result = 0x0800; //среднее значение
uint16_t global_dac_result = 0x80; //ср	еднее значение
uint32_t global_max = 0x0000;
uint32_t global_min = 0xFFFF;

/* Процедура задержки */
void Delay(int num)
{
	volatile uint32_t i = 0;
	for (i = 0; i < num; i++)
	{
	}
}

/* Функция преобразования частоты дискретизации в число тактов отсчета таймера */
int freq_to_tact(int freq_dis) {
	return (int)((1/(double)freq_dis)/(1/(double)CPU_FREQ));
}

/* Процедура настройки таймера 1 */
void Timer1_init(int freq) {
	int period = freq_to_tact(freq);
	//Включение тактирования
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1, ENABLE);

	//Инициализация структур
	TIMER_CntInitTypeDef timerCnt;
	
	//Установка настроек по умолчанию
	TIMER_CntStructInit(&timerCnt);

	//Установка предделителя
	TIMER_BRGInit(MDR_TIMER1, TIMER_HCLKdiv1);
	
	//Установка числа отсчетов до прерывания
	timerCnt.TIMER_Period = period;

	//Применение настроек	
	TIMER_CntInit(MDR_TIMER1, &timerCnt);
	//Установка наибольшего приоритета
	NVIC_SetPriority (Timer1_IRQn, 0);
	//Установка прерывания по окончанию счета таймера (достижения 0)
	TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ZERO, ENABLE);
	//Разрешение прерываний
	__enable_irq();
	//Разрешение прерываний
	NVIC_EnableIRQ (Timer1_IRQn);
	NVIC_ClearPendingIRQ(Timer1_IRQn);
}


/* Обработчик прерывания таймера 1 */
void Timer1_IRQHandler(){
	uint16_t Data;
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	Address = 0x08000000 + EEPROM_PAGE_SIZE * MAIN_EEPAGE;
	BankSelector = EEPROM_Main_Bank_Select;

	if (TIMER_GetITStatus(MDR_TIMER1, TIMER_STATUS_CNT_ZERO)){
		TIMER_ClearITPendingBit(MDR_TIMER1, TIMER_STATUS_CNT_ZERO);
		/* Начало логики обработки прерывания*/

	    //запись очередного значения в память (1514 тактов)
	    EEPROM_ProgramByte(Address + global_byte_counter, BankSelector, global_adc_result);
		//Запуск преобразования и получение нового результата
		Data = ADC_Receive_Word();
	    //отбрасывание младших 4 разрядов
	    global_adc_result = (uint8_t)(Data >> 4);

		//Инкремент счетчика бит
		global_byte_counter += 1;
		//Остановка таймера, если счетчик выходит на пределы доступной памяти
		if (global_byte_counter > 110588) {
			TIMER_Cmd(MDR_TIMER1, DISABLE);
		}
		/* Конец логики обработки прерывания*/
	}
}

/* Процедура настройки таймера 2 */
void Timer2_init(int freq) {
	
	int period = freq_to_tact(freq);
	//Включение тактирования
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER2, ENABLE);

	//Инициализация структур
	TIMER_CntInitTypeDef timerCnt;
	
	//Установка настроек по умолчанию
	TIMER_CntStructInit(&timerCnt);

	//Установка предделителя
	TIMER_BRGInit(MDR_TIMER2, TIMER_HCLKdiv1);
	
	//Установка числа отсчетов до прерывания
	timerCnt.TIMER_Period = period;

	//Применение настроек	
	TIMER_CntInit(MDR_TIMER2, &timerCnt);
	//Установка наибольшего приоритета
	NVIC_SetPriority (Timer2_IRQn, 0);
	//Установка прерывания по окончанию счета таймера (достижения 0)
	TIMER_ITConfig(MDR_TIMER2, TIMER_STATUS_CNT_ZERO, ENABLE);
	//Разрешение прерываний
	__enable_irq();
	//Разрешение прерываний
	NVIC_EnableIRQ (Timer2_IRQn);
	NVIC_ClearPendingIRQ(Timer2_IRQn);
}

/* Обработчик прерывания таймера 2 */
void Timer2_IRQHandler(){
	uint16_t Data;
	uint8_t Data_8bit;
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	Address = 0x08000000 + EEPROM_PAGE_SIZE * MAIN_EEPAGE;
	BankSelector = EEPROM_Main_Bank_Select;

	if (TIMER_GetITStatus(MDR_TIMER2, TIMER_STATUS_CNT_ZERO)){
		TIMER_ClearITPendingBit(MDR_TIMER2, TIMER_STATUS_CNT_ZERO);
		/* Начало логики обработки прерывания*/
		//Вывод предыдущего значения на ЦАП
		DAC2_SetData(global_dac_result);
		//Чтение очередного значения
		Data_8bit = (EEPROM_ReadByte(Address + global_byte_counter, BankSelector));
	    Data = (((uint16_t)Data_8bit) << 4) + 0x7;
		//Нормализация
		//Data = ((uint16_t)(((double)(Data - global_min - 0x0800))*(((double)0xFFF) / ((double)(global_max - global_min)))) + 0x800) & 0xFFF;
		global_dac_result = Data;
		//Инкремент счетчика бит
		global_byte_counter += 1;
		//Остановка таймера, если счетчик выходит на пределы доступной памяти
		if (global_byte_counter > 110588) {
			TIMER_Cmd(MDR_TIMER2, DISABLE);
		}
		/* Конец логики обработки прерывания*/
	}
}

//Настройка частоты работы
void MY_U_RST_Init(void)
{
	RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP, ENABLE);
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	while (RST_CLK_HSEstatus() != SUCCESS);

	//20 Мгц
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
	ADC1_Start();
	while (ADC1_GetFlagStatus(ADC1_FLAG_END_OF_CONVERSION) == 0);
	result = ADC1_GetResult() & 0x00000FFF;//получение 12-разрядного результата
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

	//очистка трека (27 странц)
	for (i = 0; i < 27; i++)
	{
		EEPROM_ErasePage(Address + i * EEPROM_PAGE_SIZE, BankSelector);
	}
}

/*Процедура считывания значения напряжения на АЦП и записи в память */
void write_track()
{
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	Address = 0x08000000 + EEPROM_PAGE_SIZE * MAIN_EEPAGE;
	BankSelector = EEPROM_Main_Bank_Select;

	//Запуск таймера
	TIMER_Cmd(MDR_TIMER1, ENABLE);
	//ожидание окончания записи
	while (global_byte_counter <= 110588){}
	global_byte_counter = 4;
	//создание метки
	EEPROM_ProgramWord(Address, BankSelector, 0xABCDEFAB);
}

/* Процедура считывания памяти и вывода на ЦАП (AUDIO) */
void read_track()
{
	uint32_t size_in_bytes = ENTIRE_MEM_BYTES;
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint16_t Data = 0; //16 бит для вывода на ЦАП
	uint8_t Data_8bit; //8 бит для считывания из памяти
	uint32_t i = 0;
	Address = 0x08000000 + EEPROM_PAGE_SIZE * MAIN_EEPAGE;
	BankSelector = EEPROM_Main_Bank_Select;

	//Вычислиение global_max и global_min
	for (i = 4; i < size_in_bytes; i++)
	{
		Data_8bit = (EEPROM_ReadByte(Address + i, BankSelector)) & 0xFF;
		Data = ((0x0000 + Data_8bit) << 4) + 0x7;
		if (Data > global_max)
		{
			global_max = Data;
		}
		if (Data < global_min)
		{
			global_min = Data;
		}
	}

	//Запуск таймера
	TIMER_Cmd(MDR_TIMER2, ENABLE);
	//Ожидание завершения воспроизведения
	while (global_byte_counter <= 110588){}
	global_byte_counter = 4;
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
	ADC_1_Nastroyka.ADC_ChannelNumber = ADC_CH_ADC7; //channel 7 connected to BNC
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

/* Главная функция */
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
	int freq_dis = 8000; //Установка частоты дискретизации записи и воспроизведения (не больше 10к)
	/* Включение тактирование EEPROM */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
	/* вызов инициализирующих функций */
	U_MLT_Init();
	BUTTONS_Init();
	MY_ADC_Init();
	MY_ADC_1_Init();
	MY_DAC2_Init();
	Timer1_init(freq_dis);
	Timer2_init(freq_dis);
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
				read_track(); //воспроизведение трека
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
