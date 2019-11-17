/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#define EEPROM_PAGE_SIZE				(4*1024)
#define MAIN_EEPAGE							5
#define UP											PORT_Pin_5//define pin 5  to UP butto
/* Private variables ---------------------------------------------------------*/

void erise_mem(size_in_bytes){
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint32_t i = 0;
	Address = 0x08000000 + MAIN_EEPAGE * EEPROM_PAGE_SIZE;
	BankSelector = EEPROM_Main_Bank_Select;

	for (i = 0; i < size_in_bytes; i++){
		EEPROM_ProgramWord(Address + i, BankSelector, 0xFF);
	}
}

void write_track(int num, int size_in_bytes){
//Write to EEPROM
	uint32_t size_in_words = size_in_bytes / 4;
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint32_t Data = num;
	uint32_t i = 0;
	Address = 0x08000000 + MAIN_EEPAGE * EEPROM_PAGE_SIZE + 0x1500*(num - 1); // Address = 0x08005000
	BankSelector = EEPROM_Main_Bank_Select;

	for (i = 0; i < size_in_words - 1; i++)
	{
	  EEPROM_ProgramWord (Address + i*4, BankSelector, Data);
	}

	//metka, chto dannie zapisani
	EEPROM_ProgramWord(Address + size_in_words*4, BankSelector, 0xAAAA);
}


void read_track(int num, int size_in_bytes){
  //Read from EEPROM 
 	uint32_t size_in_words = size_in_bytes / 4;
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint32_t Data = 0;
	uint32_t i = 0;
	char stroka[33];
	Address = 0x08000000 + MAIN_EEPAGE * EEPROM_PAGE_SIZE + 0x1500*(num - 1);
	BankSelector = EEPROM_Main_Bank_Select;
	for (i = 0; i < size_in_words - 1; i++)
	{
		Data = EEPROM_ReadWord(Address + i*4, BankSelector);
		sprintf(stroka, "%d", Data);
		U_MLT_Put_String(stroka, 3);
		Delay(1000);
	}
}

int track_is_empty(int num, int size_in_bytes){
	uint32_t size_in_words = size_in_bytes / 4;
	uint32_t Address = 0;
	uint32_t BankSelector = 0;
	uint32_t Data = 0;
	uint32_t i = 0;
	Address = 0x08000000 + MAIN_EEPAGE * EEPROM_PAGE_SIZE + 0x1500*(num - 1);
	BankSelector = EEPROM_Main_Bank_Select;

	if (EEPROM_ReadWord(Address + 4*size_in_words, BankSelector) == 0xAAAA){
		return 0;
	} else {
		return 1;
	}
}

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

void PORTC_Init(void) {
	PORT_InitTypeDef Nastroyka_c_diodes;
	PORT_StructInit(&Nastroyka_c_diodes);
  	/* Enables the clock on PORTC */
  	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	//init port C like out	
	Nastroyka_c_diodes.PORT_OE = PORT_OE_OUT; // out
	Nastroyka_c_diodes.PORT_MODE = PORT_MODE_DIGITAL; // digital port mode
	Nastroyka_c_diodes.PORT_SPEED = PORT_SPEED_SLOW;  // choose slow mode for front
	Nastroyka_c_diodes.PORT_Pin = PORT_Pin_1; // pin number 1 (PC1) 
	
	PORT_Init(MDR_PORTB, &Nastroyka_c_diodes); // init port
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

uint32_t Pseudo_Rand(uint32_t addr)
{
  uint32_t hash = 0;
  uint32_t i = 0;
  uint8_t* key = (uint8_t *)&addr;

  for (i = 0; i < 4; i++)
  {
    hash += key[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }

  for (i = 0; i < 256; i++)
  {
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash;
}

void Delay(int num)
{
  volatile uint32_t i = 0;
  for (i = 0; i < num; i++)
  {
  }
}

int32_t main (void)
{
	int current_track = 0, num_of_tracks = 10; 
	char stroka[40]; //stroka dlya vivoda resultata
	char track_array[10][32] = {"track_1" , "track_2", "track_3","track_4" , "track_5", "track_6", "track_7" , "track_8", "track_9", "track_10" };
	int track_size_in_bytes = 0x1500;
	

	uint32_t ibegin = 0;
	uint32_t isend = 0;
	uint32_t iend = 9;
	char str[33];


  /* Enables the clock on EEPROM */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
	
	U_MLT_Init();
	BUTTONS_Init();
	PORTC_Init();
	
	
	int current_status_up = 0;
	int current_status_down = 0;
	
	erise_mem(0x1500*10);

	write_track(1, track_size_in_bytes);
	write_track(2, track_size_in_bytes);
	write_track(5, track_size_in_bytes);

	Screen_put_number(track_is_empty(1, track_size_in_bytes), 3);
	Delay(300000);
	Screen_put_number(0xAAAA, 3);
	Delay(300000);
	Screen_put_number(track_is_empty(2, track_size_in_bytes), 3);
	Delay(300000);
	Screen_put_number(0xAAAA, 3);
	Delay(300000);
	Screen_put_number(track_is_empty(3, track_size_in_bytes), 3);
	Delay(300000);



	while (1) { 
		
		// //proverka knopok i vivod spiska na ekran
		// //UP
		// if (current_btn_status(1) == 1 && current_status_up == 0){
		// 	current_status_up = 1;
		// 	current_track += 1;
		// 	if (current_track == num_of_tracks) current_track = 0;
		// }
		// current_status_up = current_btn_status(1);
		
		// //DOWN
		// if (current_btn_status(4) == 1 && current_status_down == 0){
		// 	current_status_down = 1;
		// 	current_track -= 1; 
		// 	if (current_track == -1) current_track = num_of_tracks-1;
		// }
		// current_status_down = current_btn_status(4);
		
		// U_MLT_Put_String(track_array[current_track], 3);
		// Delay(100);

	}
	
	while (1)
  {
		
  }
}

