/* Includes ------------------------------------------------------------------*/
//#include "MDR32F9Qx_board.h"
// k PORTB_Pin_10 ne podkluchena knopka => programma ne rabotaet
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include <MDR32F9Qx_rst_clk.h>

//opredelenie zaderzhki
#define DELAY(T) for (i = T; i > 0; i--) 
#define DELAY2(T) for (j = T; j > 0; j--)
int i;
int j;
int Time;

void PORTS_INIT(void){
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTC, ENABLE);
	
	
	PORT_InitTypeDef Nastroyka_C;
	PORT_StructInit (&Nastroyka_C);
	Nastroyka_C.PORT_FUNC = PORT_FUNC_PORT;
	Nastroyka_C.PORT_MODE = PORT_MODE_DIGITAL;
	Nastroyka_C.PORT_SPEED = PORT_SPEED_SLOW;
	Nastroyka_C.PORT_OE = PORT_OE_OUT;
	Nastroyka_C.PORT_Pin = PORT_Pin_0 | PORT_Pin_1;
	
	PORT_Init(MDR_PORTC, &Nastroyka_C);
	
	PORT_InitTypeDef Nastroyka_B;
	PORT_StructInit (&Nastroyka_B);
	Nastroyka_B.PORT_FUNC = PORT_FUNC_ALTER;
	Nastroyka_B.PORT_MODE = PORT_MODE_DIGITAL;
	Nastroyka_B.PORT_SPEED = PORT_SPEED_SLOW;
	Nastroyka_B.PORT_OE = PORT_OE_IN;
	Nastroyka_B.PORT_Pin = PORT_Pin_10;
	
	PORT_Init(MDR_PORTB, &Nastroyka_B);
	
	
	
}

	
void EXT_INT2_IRQHandler(void) {
	Time -= 50000;
	if (Time == 0){
		Time = 200000;
	}
	DELAY(100000);
	if (PORT_ReadInputDataBit(MDR_PORTB, PORT_Pin_10) == 0){
			NVIC_ClearPendingIRQ(EXT_INT2_IRQn); //sbros flaga prerivaniya
	}
}

void Interrupt(void){
				NVIC_ClearPendingIRQ(EXT_INT2_IRQn);
				__enable_irq();
				NVIC_EnableIRQ(EXT_INT2_IRQn);
}



int main () {
			PORTS_INIT();
			Interrupt();
			Time = 200000;
	
			 while (1) { 
				 PORT_SetBits( MDR_PORTC, PORT_Pin_0 | PORT_Pin_1);
				 DELAY2(Time);
				 PORT_ResetBits( MDR_PORTC, PORT_Pin_0 | PORT_Pin_1);
				 DELAY2(Time);
	}
}

