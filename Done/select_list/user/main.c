/* Includes ------------------------------------------------------------------*/
//#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_timer.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_bkp.h"
#include "MDR32F9Qx_port.h"
#include "lcd.h"
#include "mlt_lcd.h"
#include "time.h"
#include "common.h"
#include <MDR32F9Qx_adc.h>

#define DELAY(T) for (i = T; i > 0; i--) 

// ===========================================================
// ===================== IMPORTANT ===========================
// ===========================================================
//knopka DOWN ne rabotaet na plate, poetomu vmesto nee ispolzuem SELECT (SB3)


//vernet 0, esli knopka ne najata i 1, esli najata , 0xA if error
//btn_name = 1 - UP , btn_name = 0 - DOWN (select na plate)
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
 
void BUTTONS_Init(void) {
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTE, ENABLE); //taktirovanie porta b i e

	//nastroyka UP (s1)
	PORT_InitTypeDef Nastroyka_b;
	PORT_StructInit (&Nastroyka_b);
	
	Nastroyka_b.PORT_Pin = PORT_Pin_5;
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
	

	PORT_Init (MDR_PORTC, &Nastroyka_c);
	
}
 
int i, current_track = 0; 
char stroka[32]; //stroka dlya vivoda resultata
char track_array[3][32] = {"track_1" , "track_2", "track_3"};


int main () {
	
	U_MLT_Init();
	BUTTONS_Init();
	int current_status_up = 0;
	int current_status_down = 0;
	

	while (1) { 
		//UP
		if (current_btn_status(1) == 1 && current_status_up == 0){
			current_status_up = 1;
			current_track += 1;
			if (current_track == 3) current_track = 0;
		}
		current_status_up = current_btn_status(1);
		
		//DOWN
		if (current_btn_status(0) == 1 && current_status_down == 0){
			current_status_down = 1;
			current_track -= 1;
			if (current_track == -1) current_track = 2;
		}
		current_status_down = current_btn_status(0);
		
		U_MLT_Put_String(track_array[current_track], 3);
		DELAY(100);
		
	}
}
		 