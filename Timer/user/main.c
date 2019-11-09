/* Includes ------------------------------------------------------------------*/
//#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_timer.h"

//opredelenie zaderzhki
#define DELAY(T) for (i = T; i > 0; i--) 
typedef enum {false, true} bool; 
//Timer1_IRQn = 14, /*!< Timer1 Interrupt *///!< Timer1_IRQn
int i;
int j = 0;
	
void Ports_init(void) {
	
	//taktirovanie porta c
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTB, ENABLE);
	
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
	
	PORT_InitTypeDef Nastroyka_B;
	PORT_StructInit (&Nastroyka_B);
	
	Nastroyka_B.PORT_Pin = PORT_Pin_5;
	Nastroyka_B.PORT_OE = PORT_OE_IN;
	Nastroyka_B.PORT_SPEED = PORT_SPEED_SLOW;
	Nastroyka_B.PORT_MODE = PORT_MODE_DIGITAL;
}

void Timer_init(int period) {
	
	//taktirovanie timer'a
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1, ENABLE);
	TIMER_CntInitTypeDef timerCnt;
	TIMER_ChnInitTypeDef timerChn;
	TIMER_ChnOutInitTypeDef timerChnOut;
	
	//default settings
	TIMER_CntStructInit(&timerCnt);
	TIMER_ChnStructInit(&timerChn);
	TIMER_ChnOutStructInit(&timerChnOut);
	
	timerCnt.TIMER_Prescaler = 8000;
	timerCnt.TIMER_Period = period; //schet do 1 i virobotka signala
	
	//delitel chastoti na 128 (80MHz / 128 = 62.5KHz)
	TIMER_BRGInit(MDR_TIMER1, TIMER_HCLKdiv1);
	
	TIMER_CntInit(MDR_TIMER1, &timerCnt);
	
	//i togo 1 takt = 1sec
	NVIC_EnableIRQ (Timer1_IRQn);
	NVIC_SetPriority (Timer1_IRQn, 0);
	TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ZERO, ENABLE);
}

void Timer1_IRQHandler(){
	
	if (TIMER_GetITStatus(MDR_TIMER1, TIMER_STATUS_CNT_ZERO)){
		LED();
		TIMER_ClearITPendingBit(MDR_TIMER1, TIMER_STATUS_CNT_ZERO);
	}
}

void LED(void){
	j += 1;
	if (j%2 == 0){
		PORT_SetBits(MDR_PORTC, PORT_Pin_0);
	} else {
		PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
	}
}



int main () {
	Ports_init();
	Timer_init(500); //500ms
	//razreshenie prerivaniy
	
	TIMER_Cmd(MDR_TIMER1, ENABLE); //start
			 while (1) {
			 
			 }
}

