/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_eeprom.h" 	//библиотека для работы с EEPROM
#include "MDR32F9Qx_port.h" 	//библиотека для работы с портами
#include "MDR32F9Qx_rst_clk.h"  //библиотека для тактирования
#include "MDR32F9Qx_adc.h" 		//библиотека для работы с АЦП
#include "MDR32F9Qx_dac.h"    //библиотека для работы с ЦАП
#include <cmath>

#define EEPROM_PAGE_SIZE				(4*1024)  //1 страница памяти - 4К
#define MAIN_EEPAGE							5     
/* Private variables ---------------------------------------------------------*/


//===============================================================================
//=========    ОПИСАНИЕ ФУНКЦИЙ ДЛЯ РАБОТЫ С ПАМЯТЬЮ    =========================
//===============================================================================

//Функция очистки областей памяти, куда были записаны треки
void erise_mem(){
	//всего 26624 слова по 32 бита памяти доступно
	int num_of_tracks = 2;
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint32_t i = 0;
	Address = 0x08000000 + MAIN_EEPAGE * EEPROM_PAGE_SIZE;
	BankSelector = EEPROM_Main_Bank_Select;

	//очистка первого трека (13 страниц)
	for (i = 0; i < 13; i++) {
		EEPROM_ErasePage(Address + i*EEPROM_PAGE_SIZE, BankSelector);
	}
	//очистка второго трека (13 страниц)
	for (i = 0; i < 13; i++){
		EEPROM_ErasePage(Address + 13*EEPROM_PAGE_SIZE + i*EEPROM_PAGE_SIZE, BankSelector);
	}

}

/*Функция считывания значения напряжения на АЦП и записи в память EEPROM
num >= 0 (1,2..) */
void write_track(int num){
//Write to EEPROM
	uint32_t Address = 0;
	uint32_t size_in_words = 13312; //same as EEPROM_PAGE_SIZE*13
	uint32_t BankSelector = 0;
	uint32_t Data = num; //16 bit na odnu zipis'
	uint32_t i = 0;
	Address = 0x08000000 + MAIN_EEPAGE * EEPROM_PAGE_SIZE + (num - 1)*EEPROM_PAGE_SIZE*13 ; // Address = 0x08012000 dlya treka 2
	BankSelector = EEPROM_Main_Bank_Select;																				//Address = 0x08005000 dlya traka 1

	for (i = 1; i < size_in_words; i++)
	{
		//Data = ADC_Receive_Word();
		Data = 2000;
		//EEPROM_ProgramWord (Address + i*4, BankSelector, Data);
	}

	//sozdanie metki
	EEPROM_ProgramWord (Address, BankSelector, 0xABCDEFAB );
}

/* Функция считывания памяти и вывода на ЦАП (AUDIO)  */
void read_track(int num){
  	//Read from EEPROM 
	uint32_t size_in_words = 13312; //same as EEPROM_PAGE_SIZE*13
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint32_t Data = 0;
	uint32_t i = 0;
	int j = 0;
	
	//формирование синусоиды =======================================================<
	int freq_des = 200;
	int amplitude = 20;
	int d[200];  // freq_des = 200
	int freq_des = 200;

	for (i = 0; i < freq_des; i++){
		d[i] = (int)(10 + 10 * sin((double)(((double)i/(double)freq_des))*2*3.1415));
	}
	//конец формирования синусоиды ================================================>

	char stroka[33];
	Address = 0x08000000 + MAIN_EEPAGE * EEPROM_PAGE_SIZE + (num - 1)*EEPROM_PAGE_SIZE*13;
	BankSelector = EEPROM_Main_Bank_Select;
	for (i = 1; i < size_in_words; i++)
	{
		// Data = (EEPROM_ReadWord(Address + i*4, BankSelector) + j) & 0x00000FFF;
		// DAC2_SetData(Data);
		// Delay(200);
		
		//вывод синусоиды ==========================================================<
		for (j = 0; j < freq_des ;j++) {
			DAC2_SetData(d[j]);
			Delay(10); //настройка частоты
		}
		//конец вывода синусоиды ===================================================>
	}
		
	//DAC2_SetData(0);
}

/* Функция проверки, записан ли трек под номером num или пуст 
num = 1,2... (>0)*/
int track_is_empty(int num){
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint32_t Data = 0;
	uint32_t i = 0;
	Address = 0x08000000 + MAIN_EEPAGE * EEPROM_PAGE_SIZE;
	BankSelector = EEPROM_Main_Bank_Select;

	if (EEPROM_ReadWord(Address + (num - 1)*EEPROM_PAGE_SIZE*13, BankSelector) == 0xABCDEFAB){
		return 0;
	} else {
		return 1;
	}
}

//===============================================================================
//=========  КОНЕЦ ОПИСАНИЯ ФУНКЦИЙ ДЛЯ РАБОТЫ С ПАМЯТЬЮ   ======================
//===============================================================================

void Screen_put_number(int num, int position) {
	char stroka[33];
	sprintf(stroka, "%d", num);
	U_MLT_Put_String(stroka, position);
}

//btn_name = 0 - SELECT BUTTON
//btn_name = 1 - UP BUTTON
//btn_name = 2 - RIGHT BUTTON
//btn_name = 3 - LEFT BUTTON
//btn_name = 4 - DOWN BUTTON
int current_btn_status(int btn_name) {
  int status = 0xA;
	
	//SELECT
	if (btn_name == 0) {
		status = PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_2);
	}

	//UP
	if (btn_name == 1) {
		status = PORT_ReadInputDataBit(MDR_PORTB, PORT_Pin_5);
	}

	//RIGHT
	if (btn_name == 2){
		status = PORT_ReadInputDataBit(MDR_PORTB, PORT_Pin_6);
	}

	//LEFT
	if (btn_name == 3){
		status = PORT_ReadInputDataBit(MDR_PORTE, PORT_Pin_3);
	}

	//DOWN
	if (btn_name == 4) {
		status = PORT_ReadInputDataBit(MDR_PORTE, PORT_Pin_1);
	}
	
	//invertirovanie resultata
	if (status == 1) {
		return 0;}
	if (status == 0) {
		return 1;}
	if (status != 0 && status != 1){
		return 0xA;
	}
	
}

//init ADC
void MY_ADC_Init(void){
	RST_CLK_PCLKcmd(RST_CLK_PCLK_ADC, ENABLE);
	ADC_InitTypeDef ADC_Nastroyka;
	ADC_StructInit(&ADC_Nastroyka);
	ADC_Init(&ADC_Nastroyka);
}

//init ADC1
void MY_ADC_1_Init(void){
	ADCx_InitTypeDef ADC_1_Nastroyka;
	ADCx_StructInit(&ADC_1_Nastroyka);
	ADC_1_Nastroyka.ADC_ChannelNumber = ADC_CH_ADC7; //channel 7 connected to BNC
	ADC1_Init(&ADC_1_Nastroyka);
	ADC1_Cmd(ENABLE);
}


void BUTTONS_Init(void) {

	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTC, ENABLE); //taktirovanie porta B, C, E

	//nastroyka PB5 (UP) and PB6 (RIGHT) knopki
	PORT_InitTypeDef Nastroyka_b;
	PORT_StructInit (&Nastroyka_b);
	Nastroyka_b.PORT_Pin = PORT_Pin_5 | PORT_Pin_6;
	Nastroyka_b.PORT_MODE = PORT_MODE_DIGITAL;
	Nastroyka_b.PORT_OE = PORT_OE_IN;
	Nastroyka_b.PORT_SPEED = PORT_SPEED_SLOW;
	Nastroyka_b.PORT_PD = PORT_PD_DRIVER;	
	PORT_Init (MDR_PORTB, &Nastroyka_b);
	
	//Nastroyka PC2 (SELECT)
	PORT_InitTypeDef Nastroyka_c;
	PORT_StructInit (&Nastroyka_c);
	Nastroyka_c.PORT_Pin = PORT_Pin_2;
	Nastroyka_c.PORT_MODE = PORT_MODE_DIGITAL;
	Nastroyka_c.PORT_OE = PORT_OE_IN;
	Nastroyka_c.PORT_SPEED = PORT_SPEED_SLOW;
	Nastroyka_c.PORT_PD = PORT_PD_DRIVER;
	PORT_Init (MDR_PORTC, &Nastroyka_c);


	//Nastroyka PE3 (LEFT) and PE1 (DOWN) knopki
	PORT_InitTypeDef Nastroyka_e;
	PORT_StructInit(&Nastroyka_e);
	Nastroyka_e.PORT_Pin = PORT_Pin_3 | PORT_Pin_1;
	Nastroyka_e.PORT_MODE = PORT_MODE_DIGITAL;
	Nastroyka_e.PORT_OE = PORT_OE_IN;
	Nastroyka_e.PORT_SPEED = PORT_SPEED_SLOW;
	Nastroyka_e.PORT_PD = PORT_PD_DRIVER;
	PORT_Init(MDR_PORTE, &Nastroyka_e);
	
}


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
uint32_t ADC_Receive_Word(){
	ADC1_Start();
	while (ADC1_GetFlagStatus(ADC1_FLAG_END_OF_CONVERSION) == 0); 
	return ADC1_GetResult() & 0x00000FFF; //получение 12-разрядного результата
}

/* Функция настройки ЦАП  */
void MY_DAC2_Init(void){
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


int32_t main (void)
{
	int current_track = 0;  //current_track = 0,1,...
	int num_of_tracks = 2;
	char stroka[40]; //stroka dlya vivoda resultata
	char track_array[10][32] = {"\xD2\xF0\xE5\xEA 1" , "\xD2\xF0\xE5\xEA 2", "\xD2\xF0\xE5\xEA 3","\xD2\xF0\xE5\xEA 4" , "\xD2\xF0\xE5\xEA 5", "\xD2\xF0\xE5\xEA 6", "\xD2\xF0\xE5\xEA 7" , "\xD2\xF0\xE5\xEA 8", "\xD2\xF0\xE5\xEA 9", "\xD2\xF0\xE5\xEA 10" };	
	/* opredelenie strok */
	char ochistka[] = "\xCE\xF7\xE8\xF1\xF2\xEA\xE0";
	char pamyati[] = "\xEF\xE0\xEC\xFF\xF2\xE8...";
	char zapis[] = "\xC7\xE0\xEF\xE8\xF1\xFC...";
	char vosproiz[] = "\xC2\xEE\xF1\xEF\xF0\xE8\xE7";
	char vedenie[] = "\xE2\xE5\xE4\xE5\xED\xE8\xE5...";
	char vosproizvedenie[] = "\xC2\xEE\xF1\xEF\xF0\xE8\xE7\xE2\xE5\xE4\xE5\xED\xE8\xE5...";
	char pusto[] = "\xCF\xF3\xF1\xF2\xEE!";
	char smirnov[] = "\xD1\xEC\xE8\xF0\xED\xEE\xE2 \xC0.\xC0.";
	char iu673[] = "\xC8\xD3\x36-73";
  /* Enables the clock on EEPROM */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
	
	U_MLT_Init();
	BUTTONS_Init();
	MY_ADC_Init();
	MY_ADC_1_Init();
	MY_DAC2_Init();
	
	/* Init surname and group */ 
	U_MLT_Put_String("", 0);
	U_MLT_Put_String(smirnov, 1);
	U_MLT_Put_String(iu673, 2);
	
	int current_status_up = 0;		//button to select track above
	int current_status_down = 0;	//butotn to select tarck below
	int current_status_select = 0;  //button to erase memory
	int current_status_right = 0;   //button to record track
	int current_status_left = 0;    //button to play track
	

	while (1) { 
		
		//proverka knopok i vivod spiska na ekran

		//UP -> SELECT TRACK ABOVE
		if (current_btn_status(1) == 1 && current_status_up == 0){
			//begin action
			current_status_up = 1;
			current_track += 1;
			if (current_track == num_of_tracks) current_track = 0;
			//end action
		}
		current_status_up = current_btn_status(1);
		
		//DOWN -> SELECT TRACK BELOW
		if (current_btn_status(4) == 1 && current_status_down == 0){
			//begin action
			current_status_down = 1;
			current_track -= 1; 
			if (current_track == -1) current_track = num_of_tracks-1;
			//end action
		}
		current_status_down = current_btn_status(4);

		//SELECT -> ERASE MEMROY
		if (current_btn_status(0) == 1 && current_status_select == 0){
			//begin action
			current_status_select = 1;
			U_MLT_Put_String(ochistka, 4);
			U_MLT_Put_String(pamyati, 5);
			erise_mem();

			Delay(500000);
			//end action
		}
		current_status_select = current_btn_status(0);

		//RIGHT -> RECORD TRACK
		if (current_btn_status(2) == 1 && current_status_right == 0){
			//begin action
			U_MLT_Put_String(zapis, 4);
			write_track(current_track + 1);
			Delay(500000);
			//end action
		}
		current_status_right = current_btn_status(2);


		//LEFT -> PLAY TRACK 
		if (current_btn_status(3) == 1 && current_status_left == 0) {
			//begin action
			if (track_is_empty(current_track + 1) == 1){
					U_MLT_Put_String(pusto, 4);
				} else {
					U_MLT_Put_String(vosproizvedenie, 4);
					read_track(current_track + 1);
				}
				Delay(500000);
			//end action
			}
		current_status_left = current_btn_status(3);

		//print select-list on LCD
		U_MLT_Put_String(track_array[current_track], 4);
		U_MLT_Put_String("", 5);
		Delay(500);
	}

}
	

