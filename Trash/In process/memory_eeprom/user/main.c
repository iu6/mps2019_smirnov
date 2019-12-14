// it does't work
#include "MDR32F9Qx_board.h"
//#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
//#include "MDR32F9Qx_timer.h"
//#include "MDR32F9Qx_bkp.h"
#include "lcd.h"
#include "mlt_lcd.h"
//#include "time.h"
//#include "common.h"
//#include <MDR32F9Qx_adc.h>
//#include <stdio.h>

#define DELAY(T) for (j = T; j > 0; j--) {}
int j;
	
//string to print data from mamory on LCD
char stroka[32];

int led_i = 0;
void LED(void){
	
	if (led_i%2 == 0){
		PORT_SetBits(MDR_PORTC, PORT_Pin_0);
	} else {
		PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
	}
	led_i+=1;
}

void PORT_C_Init() {
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	
	PORT_InitTypeDef nastroyka_c;
	PORT_StructInit(&nastroyka_c);
	
	nastroyka_c.PORT_OE = PORT_OE_OUT;
	nastroyka_c.PORT_SPEED = PORT_SPEED_SLOW;
	nastroyka_c.PORT_MODE = PORT_MODE_DIGITAL;
	
	PORT_Init( MDR_PORTC, &nastroyka_c);
}

int main (void)
{
		__set_FAULTMASK(1); //blokirovka prerivaniy

	//define variables
	uint32_t My_data;
	uint32_t Address;
	uint32_t BankSelector;

	//init port and LCD
	PORT_C_Init();
	U_MLT_Init();
	
	LED(); //turn on led


  /* Enables the  clock on EEPROM */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);

	//erise page mem
	Address = 0x08000000;
  EEPROM_ErasePage (Address, EEPROM_Main_Bank_Select);
	
	//write word
	EEPROM_ProgramWord (Address, EEPROM_Main_Bank_Select, 0x0000000A);
	
	//read word
  My_data = EEPROM_ReadWord (Address, EEPROM_Main_Bank_Select);
	
	sprintf(stroka, "%d", My_data);
	
	DELAY(0xFFFF);
	LED();
	
	while(1) {
		
		//vivod chisla
		U_MLT_Put_String(stroka, 3);
		
	}
}
