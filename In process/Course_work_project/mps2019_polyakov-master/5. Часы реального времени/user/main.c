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

#define U_ALARM_WAIT_TIME 12
#define EVENT_ALARM 0x0001
// Preddelitel' chastoty dlya RTC
// Pri vneshnem kvartse 32768Gts diskretnost' RTC - 1c
#define RTC_PRESCALER 32768/10
#define RTC_CALIBRATION 0
// Nachal'nyye data i vremya
#define RTC_INIT_TIMESTAMP 0  // 01.01.2012 00:00:00

#define UP PORT_Pin_5//define pin 5  to UP button
#define SELECT PORT_Pin_2//define pin 1 to SELECT button
#define DOWN PORT_Pin_1//define pin 1 to DOWN button

int32_t i;
int16_t stop = 0;
int16_t start = 0;
time_t current;
time_t alarm;


//static char message[128];


int main (void)
{
	PORT_InitTypeDef PortInitStructure;	
	PORT_InitTypeDef GPIO_user_ini;
	
  RST_CLK_PCLKcmd (RST_CLK_PCLK_BKP | RST_CLK_PCLK_PORTA|
	RST_CLK_PCLK_PORTB | RST_CLK_PCLK_PORTC |
	RST_CLK_PCLK_PORTD | RST_CLK_PCLK_PORTE, ENABLE);
	
	U_MLT_Init();
	
  PORT_StructInit (&PortInitStructure);	
	
	//init port B like input	
	GPIO_user_ini.PORT_OE = PORT_OE_IN; // port mode -> input
	GPIO_user_ini.PORT_FUNC = PORT_FUNC_PORT; // port mode
	GPIO_user_ini.PORT_MODE = PORT_MODE_DIGITAL; // digital port mode
	GPIO_user_ini.PORT_SPEED = PORT_SPEED_SLOW;  // choose slow mode for front
	GPIO_user_ini.PORT_Pin = UP; // pin number 5 (PB5) which is connected to button
	PORT_Init(MDR_PORTB, &GPIO_user_ini); // init port
	
	//init port C like input
	GPIO_user_ini.PORT_OE = PORT_OE_IN; // port mode -> input
	GPIO_user_ini.PORT_FUNC = PORT_FUNC_PORT; // port mode
	GPIO_user_ini.PORT_MODE = PORT_MODE_DIGITAL; // digital port mode
	GPIO_user_ini.PORT_SPEED = PORT_SPEED_SLOW;  // choose slow mode for front
	GPIO_user_ini.PORT_Pin = SELECT; // pin number 5 (PB5) which is connected to button
	PORT_Init(MDR_PORTC, &GPIO_user_ini); // init port
	
	//init port E like input
	GPIO_user_ini.PORT_OE = PORT_OE_IN; // port mode -> input
	GPIO_user_ini.PORT_FUNC = PORT_FUNC_PORT; // port mode
	GPIO_user_ini.PORT_MODE = PORT_MODE_DIGITAL; // digital port mode
	GPIO_user_ini.PORT_SPEED = PORT_SPEED_SLOW;  // choose slow mode for front
	GPIO_user_ini.PORT_Pin = DOWN; // pin number 1 (PE1) which is connected to button
	PORT_Init(MDR_PORTE, &GPIO_user_ini); // init port

  // Sdelat' analogovym vkhodom linii (PE6, PE7), k kotoromu podklyuchen chasovoy kvarts
	PortInitStructure.PORT_Pin   = PORT_Pin_6 | PORT_Pin_7;
  PortInitStructure.PORT_MODE =  PORT_MODE_ANALOG;
  PORT_Init (MDR_PORTE, &PortInitStructure);		
	
	// Vklyuchit' chasovoy generator na vneshnem kvartse
  RST_CLK_LSEconfig (RST_CLK_LSE_ON); 
  while (RST_CLK_LSEstatus() != SUCCESS);	
	
	// Vybrat' LSE v kachestve istochnika taktirovaniya RTC
  BKP_RTC_WaitForUpdate ();         
  BKP_RTCclkSource (BKP_RTC_LSEclk);
	
  BKP_RTC_WaitForUpdate ();         
  BKP_RTC_SetPrescaler (RTC_PRESCALER);

	// Korrektirovka skorosti khoda
  BKP_RTC_WaitForUpdate ();          
  BKP_RTC_Calibration (RTC_CALIBRATION);

  BKP_RTC_WaitForUpdate ();          
  BKP_RTC_Enable (ENABLE);
	
	// Ustanovit' nachal'nyye daty i vremya
  BKP_RTC_WaitForUpdate ();          
  U_Alarm_Set_DateTime_Stamp(RTC_INIT_TIMESTAMP);
	
  BKP_RTC_WaitForUpdate ();          
	
  NVIC_SetPriority (BACKUP_IRQn, 0x02);
	
  NVIC_EnableIRQ (BACKUP_IRQn);

starts:
while(1)
	{
		if (PORT_ReadInputDataBit(MDR_PORTE, DOWN) == 0)
					U_Alarm_Set_DateTime_Stamp(0);
		if (PORT_ReadInputDataBit(MDR_PORTB, UP) == 0)
		{
			stop += 1;
			start = 0;
		}			
		if (stop!=0)
			while(1) {
				if (PORT_ReadInputDataBit(MDR_PORTC, SELECT) == 0)
				{
					start += 1;
					stop = 0;
					U_Alarm_Set_DateTime_Stamp(i);
					goto starts;
				}
				if (PORT_ReadInputDataBit(MDR_PORTE, DOWN) == 0)
				{
					U_Alarm_Set_DateTime_Stamp(0);
				  U_LCD_Task_Function(0);
					i = 0;
				}
			};
		current = U_Alarm_Get_DateTime_Stamp();
		
    BKP_RTC_WaitForUpdate ();     
		
		i = current; 
		
		U_LCD_Task_Function(current);
	}
}

//poluch datu
time_t U_Alarm_Get_DateTime_Stamp(void)
{
  BKP_RTC_WaitForUpdate ();          
  return BKP_RTC_GetCounter();
}

//ust datu
void U_Alarm_Set_DateTime_Stamp (time_t TimeStamp)
{
  BKP_RTC_WaitForUpdate ();          
  BKP_RTC_SetCounter (TimeStamp);
}


void BACKUP_IRQHandler (void)
{
  if(BKP_RTC_GetFlagStatus (BKP_RTC_FLAG_ALRF) == SET)	
	{
   BKP_RTC_ITConfig (BKP_RTC_IT_ALRF, DISABLE);
	}
}