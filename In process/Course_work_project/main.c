// Подключение заголовочных файлов необходимых библиотек
#include <MDR32F9Qx_port.h> 
#include <MDR32F9Qx_rst_clk.h> 
#include <MDR32F9Qx_adc.h>
#include "lcd.h" 
  // Определение функции задержки 
  #define DELAY(T) for (i = T; i > 0; i--) 
  
  // Определение калибровочной константы 
  #define KALIBR 1247 
  
  // Объявление переменных 
  int i; 
  float U; 
  uint32_t RESULT;
  char stroka[17]; // Размер массива для строки

// Процедура общей настройки АЦП 
void ADCInit() { 
	// Включение тактирования АЦП 
	RST_CLK_PCLKcmd(RST_CLK_PCLK_ADC, ENABLE);
	 // Объявление структур для общей настройки АЦП 
	 ADC_InitTypeDef ADC; 
	 // Загрузка значений по умолчанию в структуру 
	 ADC ADC_StructInit(&ADC); 
	 // Инициализация АЦП объявленной структурой 
	 ADC_Init(&ADC); 
	 } 

	 // Процедура настройки АЦП1 
void ADC1Init() { 
	// Объявление структур для общей настройки АЦП1 
	ADCx_InitTypeDef ADC1;
	 // Загрузка значений по умолчанию в структуру 
	 ADC1 ADCx_StructInit(&ADC1); 
	 // Установка номера канала АЦП, 
	 // подключенного к резистору R1 платы
	  ADC1.ADC_ChannelNumber = ADC_CH_ADC7; 
	  // Инициализация первого АЦП объявленной структурой 
	  ADC1_Init(&ADC1); 
	  // Включение первого АЦП 
	  ADC1_Cmd(ENABLE);
 } 
	  
	  // Объявление главной функции
	   int main () {
		   // ADCInit(); 
		   // Вызов функции общей настройки АЦП ADC1Init(); // Вызов функции индивидуальной настройки АЦП1 LCD_Init(); // Вызов функции инициализации ЖК-модуля // Основной цикл while (1) { ADC1_Start(); // Начало преобразования // Ожидание флага завершения преобразования while (ADC1_GetFlagStatus(ADC1_FLAG_END_OF_CONVERSION) == 0); // Чтение результата преобразования RESULT = ADC1_GetResult() & 0x00000FFF; // Калибровка результата преобразований U = (float)RESULT / KALIBR; // Вывод результата на экран
	   }

//snprintf(stroka, 17, "U = %.2fВ", U); LCD_PutString(stroka, 4); // Задержка изображения на экране DELAY(0xFFFF); } }