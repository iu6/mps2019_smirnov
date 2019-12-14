
//#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_eeprom.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_bkp.h"
#include "lcd.h"
#include "mlt_lcd.h"
#include "time.h"
#include "common.h"
#include <MDR32F9Qx_adc.h>
#include <stdio.h>

#define DELAY(T) for(i = T; i > 0; i--){}
int i;

void PORT_C_Init() {
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	
	PORT_InitTypeDef nastroyka_c;
	PORT_StructInit(&nastroyka_c);
	
	nastroyka_c.PORT_OE = PORT_OE_OUT;
	nastroyka_c.PORT_SPEED = PORT_SPEED_SLOW;
	nastroyka_c.PORT_MODE = PORT_MODE_DIGITAL;
	nastroyka_c.PORT_Pin = PORT_Pin_0 | PORT_Pin_1;
	
	PORT_Init( MDR_PORTC, &nastroyka_c);
}


int32_t main (void)
{
	char stroka[33];
  uint32_t Address = 0;
  uint32_t BankSelector = 0;
  uint32_t Data = 0;
	uint32_t My_data = 0;
  uint32_t i = 0;
  
	PORT_C_Init();
	//U_MLT_Init();
	
  /* Enables the  clock on EEPROM */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_EEPROM, ENABLE);

	//erise page mem
	Address = 0x08000000;
  EEPROM_ErasePage (Address, EEPROM_Main_Bank_Select);
	PORT_SetBits(MDR_PORTC, PORT_Pin_0);
	//write word
	EEPROM_ProgramWord (Address, EEPROM_Main_Bank_Select, 0x0000000A);
  My_data = EEPROM_ReadWord (Address, EEPROM_Main_Bank_Select);
	DELAY(1000000);
	PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
	
	sprintf(stroka , "%d", My_data);

		
	while(1) {
		//U_MLT_Put_String("test", 3);
		if (My_data == 0) {
			PORT_SetBits(MDR_PORTC, PORT_Pin_1);
		} else {
			PORT_ResetBits(MDR_PORTC, PORT_Pin_1);
		}
	}
}
