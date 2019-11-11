// it does't work
#include "MDR32F9Qx_board.h"
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
#include "MDR32F9Qx_dac.h"              // Keil::Drivers:DAC

#define DELAY(T) for (j = T; j > 0; j--) {}
int j;
int m;
	
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
	//define variables
	uint32_t My_data;
	uint32_t Address;
	uint32_t BankSelector;
	//uint8_t Track[1500];

	//init port and LCD
	//PORT_C_Init();
	PORT_C_Init();
	//U_MLT_Init();

	
	//__disable_irq(); // disable interruptions
	//LED(); //turn on led

	
	//init dac1
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
	PORT_InitTypeDef PORT_InitStructure;
	PORT_InitStructure.PORT_Pin = PORT_Pin_9;
	PORT_InitStructure.PORT_OE = PORT_OE_OUT; 
	PORT_InitStructure.PORT_MODE = PORT_MODE_ANALOG; 
	PORT_Init(MDR_PORTE, &PORT_InitStructure); 
	
	DAC1_Init(DAC1_AVCC); //Vybor opornogo napryazheniya
	DAC1_Cmd(ENABLE);
	
	
	//end init dac1
	

  /* Enables the  clock on EEPROM */
  //for (int k = 0; k < 200; k++){
		//Track[k] = k;
	//}
	
int i, out ;
	
//	while(1)
//	{
//		LED();
	//	for (i=0;i<2000;i++)  //inc
//		{
//			if (i % 2 == 0) {out = 0;} else {out = 3000;}
			
//			DAC2_SetData(out);   //Vyvesti peremennuyu v DAC
//			DELAY(15);
//			if (i > 0 && i < 250) {U_MLT_Put_String ("250", 3);}
//			if (i > 250 && i < 500) {U_MLT_Put_String ("500", 3);}
//			if (i > 500 && i < 750) {U_MLT_Put_String ("750", 3);}
//			if (i > 750 && i < 1000) {U_MLT_Put_String ("1000", 3);}
//		}
//		LED();
//		for (i=2000;i>0;i--)  //dec
//		{
//      if (i % 2 == 0) {out = 0;} else {out = 3000;}
//			
//			DAC2_SetData(out);   //Vyvesti peremennuyu v DAC
			
//			DELAY(15);
//			if (i > 0 && i < 250) {U_MLT_Put_String ("250", 3);}
//			if (i > 250 && i < 500) {U_MLT_Put_String ("500", 3);}
//			if (i > 500 && i < 750) {U_MLT_Put_String ("750", 3);}
//			if (i > 750 && i < 1000) {U_MLT_Put_String ("1000", 3);}
//		}
	
//	}



	while(1){ 
		i += 1;
		if (i == 1000) {i = 0;}
		if (i%2 == 0) {out = 0;} else {out = 2000;}
		DELAY(15);
		DAC1_SetData(out);

	}
	
}
