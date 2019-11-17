/* Includes ------------------------------------------------------------------*/
#include "MDR32Fx.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"

#define EEPROM_PAGE_SIZE             (4*1024)
#define MAIN_EEPAGE                  5
#define UP PORT_Pin_5//define pin 5  to UP butto

/* Private variables ---------------------------------------------------------*/
int current_btn_status(int btn_name) {
  int status = 0xA;
	if (btn_name == 1) {
		status = PORT_ReadInputDataBit(MDR_PORTB, PORT_Pin_5);
	}
	
	if (btn_name == 0) {
		status = PORT_ReadInputDataBit(MDR_PORTC, PORT_Pin_2);

	}
	
	if (status == 1) {
		return 0;}
	if (status == 0) {
		return 1;}
	if (status != 0 && status != 1){
		return 0xA;}
}

void PORTS_Init(void) {
	PORT_InitTypeDef GPIO_user_ini;
  /* Enables the clock on PORTC */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTB, ENABLE);
	//init port B like input	
	GPIO_user_ini.PORT_OE = PORT_OE_IN; // port mode -> input
	GPIO_user_ini.PORT_FUNC = PORT_FUNC_PORT; // port mode
	GPIO_user_ini.PORT_MODE = PORT_MODE_DIGITAL; // digital port mode
	GPIO_user_ini.PORT_SPEED = PORT_SPEED_SLOW;  // choose slow mode for front
	GPIO_user_ini.PORT_Pin = UP; // pin number 5 (PB5) which is connected to button
	
	PORT_Init(MDR_PORTB, &GPIO_user_ini); // init port
}

void BUTTONS_Init(void) {
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTE, ENABLE); //taktirovanie porta b i e

	//nastroyka UP (s1)
	PORT_InitTypeDef Nastroyka_b;
	PORT_StructInit (&Nastroyka_b);
	
	Nastroyka_b.PORT_Pin = PORT_Pin_5 | PORT_Pin_6;
	Nastroyka_b.PORT_MODE = PORT_MODE_DIGITAL;
	Nastroyka_b.PORT_OE = PORT_OE_IN;
	Nastroyka_b.PORT_SPEED = PORT_SPEED_SLOW;
	Nastroyka_b.PORT_PD = PORT_PD_DRIVER;
	//OtherStructure.PORT_PULL_UP = PORT_PULL_UP_ON; //vkluchenie podtyagivaushego rezistora
	
	PORT_Init (MDR_PORTB, &Nastroyka_b);
	
	//nastroyka DOWN (s3 - select na plate)
	PORT_InitTypeDef Nastroyka_c;
	PORT_StructInit (&Nastroyka_c);
	
	//Nastroyka_e.PORT_Pin = PORT_Pin_1;
	Nastroyka_c.PORT_Pin = PORT_Pin_2;
	Nastroyka_c.PORT_MODE = PORT_MODE_DIGITAL;
	Nastroyka_c.PORT_OE = PORT_OE_IN;
	Nastroyka_c.PORT_SPEED = PORT_SPEED_SLOW;
	Nastroyka_c.PORT_PD = PORT_PD_DRIVER;
	//OtherStructure.PORT_PULL_UP = PORT_PULL_UP_ON; //vkluchenie podtyagivaushego rezistora
	
	PORT_InitTypeDef Nastroyka_e;
	PORT_StructInit(&Nastroyka_e);
	

	PORT_Init (MDR_PORTC, &Nastroyka_c);
	
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
	char stroka[32]; //stroka dlya vivoda resultata
	char track_array[10][32] = {"track_1" , "track_2", "track_3","track_4" , "track_5", "track_6", "track_7" , "track_8", "track_9", "track_10" };
	
  uint32_t Address = 0;
  uint32_t BankSelector = 0;
  uint32_t Data = 0;
  uint32_t i = 0;
  uint32_t ibegin = 0;
  uint32_t isend = 0;
  uint32_t iend = 9;
	char str[33];


  /* Enables the clock on EEPROM */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);
	
	U_MLT_Init();
	BUTTONS_Init();
	
	
	int current_status_up = 0;
	int current_status_down = 0;
	

	while (1) { 
		
		//proverka knopok i vivod spiska na ekran
		//UP
		if (current_btn_status(1) == 1 && current_status_up == 0){
			current_status_up = 1;
			current_track += 1;
			if (current_track == num_of_tracks) current_track = 0;
		}
		current_status_up = current_btn_status(1);
		
		//DOWN
		if (current_btn_status(0) == 1 && current_status_down == 0){
			current_status_down = 1;
			current_track -= 1; 
			if (current_track == -1) current_track = num_of_tracks-1;
		}
		current_status_down = current_btn_status(0);
		
		U_MLT_Put_String(track_array[current_track], 3);
		Delay(100);
		
	}
	
	while (1)
  {
		
  }
}





/*

//Write to EEPROM
  Address = 0x08000000 + MAIN_EEPAGE * EEPROM_PAGE_SIZE;
  BankSelector = EEPROM_Main_Bank_Select;

  for (i = 0; i < 10000; i++)
  {
    Data += 1;
    EEPROM_ProgramWord (Address + i*4, BankSelector, Data);
  }
	
  //Read from EEPROM 
  Address = 0x08000000 + MAIN_EEPAGE * EEPROM_PAGE_SIZE;
  BankSelector = EEPROM_Main_Bank_Select;
  for (i = 0; i < 10000; i++)
  {
    Data = EEPROM_ReadWord(Address + i*4, BankSelector);
		sprintf(stroka, "%d", Data);
		U_MLT_Put_String(stroka, 3);
		Delay(1000);
  }
*/

