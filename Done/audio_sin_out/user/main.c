/* Includes ------------------------------------------------------------------*/
//#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_dac.h"              // Keil::Drivers:DAC
#include "MDR32F9Qx_rst_clk.h"
#include <cmath>

#define PI 3.1415
#define DELAY(T) for (i_delay = T; i_delay > 0; i_delay--);
int i_delay, out, s;

PORT_InitTypeDef PORT_InitStructure;

void MY_DAC2_Init(void){
	
		//Nastroyka vyvoda dlya DAC
	PORT_InitStructure.PORT_Pin = PORT_Pin_0;
	PORT_InitStructure.PORT_OE = PORT_OE_OUT; 
	PORT_InitStructure.PORT_MODE = PORT_MODE_ANALOG; 
	PORT_Init(MDR_PORTE, &PORT_InitStructure); 
	
	DAC2_Init(DAC2_AVCC); //Vybor opornogo napryazheniya
	DAC2_Cmd(ENABLE);
}

void PORTC_Init(void) {
		// Set up pins for LEDs
	PORT_InitStructure.PORT_Pin=(PORT_Pin_0 | PORT_Pin_1); 
	PORT_InitStructure.PORT_OE = PORT_OE_OUT; 
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL; 
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW; 
	PORT_Init(MDR_PORTC, &PORT_InitStructure);
}


int res=0,i; 

int main(void)	
{
	
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA|
	RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTC |
	RST_CLK_PCLK_PORTD | RST_CLK_PCLK_PORTE |
	RST_CLK_PCLK_DAC, ENABLE);
	
	PORTC_Init();
	MY_DAC2_Init();

	s = 0;
	int freq_des = 200;
	int amplitude = 20;
	int d[200];  // freq_des = 200
	
	// formirovande massiva sinusoidalnogo signala
	for (i = 0; i < freq_des; i++){
		d[i] = (int)(10 + 10 * sin((double)(((double)i/(double)freq_des))*2*3.1415));
	}
	int k = 0;
	// ciclichiy vivod sinusoidi
	while(1)
	{
		for (i = 0; i < freq_des ;i++) {
			DAC2_SetData(d[i]);
			DELAY(2); //nastroyka chastoti
		}
		
	}
}
