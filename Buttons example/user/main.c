/* Includes ------------------------------------------------------------------*/
//#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include <MDR32F9Qx_rst_clk.h>

//opredelenie zaderzhki
#define DELAY(T) for (i = T; i > 0; i--) 
int i;
	




int main () {
	//taktirovanie porta
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	
	//nastroyka porta c
	PORT_InitTypeDef PortInitStructure;
	PORT_StructInit (&PortInitStructure);
	
	PortInitStructure.PORT_Pin = PORT_Pin_0 | PORT_Pin_1;
	PortInitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PortInitStructure.PORT_OE = PORT_OE_OUT;
	PortInitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PortInitStructure.PORT_PD = PORT_PD_DRIVER;
	PortInitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
	PortInitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
	
	//priminenie nastroek
	PORT_Init (MDR_PORTC, &PortInitStructure);
	
	//konfiguriruem port b na vhod (dlya knopki)
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE); //taktirovanie porta b

	PORT_InitTypeDef OtherStructure;
	PORT_StructInit (&OtherStructure);
	
	OtherStructure.PORT_Pin = PORT_Pin_5;
	OtherStructure.PORT_MODE = PORT_MODE_DIGITAL;
	OtherStructure.PORT_OE = PORT_OE_IN;
	OtherStructure.PORT_SPEED = PORT_SPEED_SLOW;
	OtherStructure.PORT_PD = PORT_PD_DRIVER;
	//OtherStructure.PORT_PULL_UP = PORT_PULL_UP_ON; //vkluchenie podtyagivaushego rezistora
	
	PORT_Init (MDR_PORTB, &OtherStructure);
				
				//perekluchenie svetodiodov
			 while (1) { 
				//PORT_SetBits(MDR_PORTC, PORT_Pin_0 | PORT_Pin_1);
				//DELAY(100000);
				//PORT_ResetBits (MDR_PORTC, PORT_Pin_0 | PORT_Pin_1);
				//DELAY(100000);
				 
				//proverka najatiya knopki
				uint16_t in_data;
				in_data = PORT_ReadInputData (MDR_PORTB);
				if (in_data == PORT_Pin_5) {
					PORT_ResetBits(MDR_PORTC, PORT_Pin_0 | PORT_Pin_1);
				} else {
					PORT_SetBits (MDR_PORTC, PORT_Pin_0 | PORT_Pin_1);
				}
				
	}
}

