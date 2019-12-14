#include <MDR32Fx.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "MDR32F9Qx_rst_clk.h"          // Keil::Drivers:RST_CLK

#include <rl_fs.h>

// Кварц 8 МГц * (9+1) = 80 МГц
#define RST_CLK_PLL_CONTROL_PLL_CPU_MUL 9

extern const unsigned char Ver[2];

/*
	Использована среда проектирования Keil uVision ver. 5.16.
	Дата 6.09.2015.
*/

// Номер версии firmware, длина - 2 байта, формат "001.000"
const unsigned char Ver[2] = {1, 0};

void ClkSetup (void)
{
	MDR_RST_CLK->PER_CLOCK = RST_CLK_PER_CLOCK_PCLK_EN_RST_CLK;
	
	MDR_RST_CLK->HS_CONTROL =
		(1 << RST_CLK_HS_CONTROL_HSE_ON_Pos) |
		(0 << RST_CLK_HS_CONTROL_HSE_BYP_Pos);
	
	while (CHF_BIT_PER (MDR_RST_CLK->CLOCK_STATUS, RST_CLK_CLOCK_STATUS_HSE_RDY_Pos));
	
	MDR_RST_CLK->CPU_CLOCK =
		(RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSE << RST_CLK_CPU_CLOCK_CPU_C1_SEL_Pos) |
		(RST_CLK_CPU_CLOCK_CPU_C2_SEL_CPU_C1 << RST_CLK_CPU_CLOCK_CPU_C2_SEL_Pos) |
		(RST_CLK_CPU_CLOCK_CPU_C3_SEL_CPU_C2 << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos) |
		(RST_CLK_CPU_CLOCK_HCLK_SEL_HSI << RST_CLK_CPU_CLOCK_HCLK_SEL_Pos);
	
	MDR_RST_CLK->PLL_CONTROL =
		(0 << RST_CLK_PLL_CONTROL_PLL_USB_ON_Pos) |
		(0 << RST_CLK_PLL_CONTROL_PLL_USB_RLD_Pos) |
		(0 << RST_CLK_PLL_CONTROL_PLL_CPU_ON_Pos) |
		(0 << RST_CLK_PLL_CONTROL_PLL_CPU_PLD_Pos) |
		(0 << RST_CLK_PLL_CONTROL_PLL_USB_MUL_Pos) |
		(RST_CLK_PLL_CONTROL_PLL_CPU_MUL << RST_CLK_PLL_CONTROL_PLL_CPU_MUL_Pos);
	
	SET_BIT_PER (MDR_RST_CLK->PLL_CONTROL, RST_CLK_PLL_CONTROL_PLL_CPU_ON_Pos);
	
	SET_BIT_PER (MDR_RST_CLK->PLL_CONTROL, RST_CLK_PLL_CONTROL_PLL_CPU_PLD_Pos);
	CLR_BIT_PER (MDR_RST_CLK->PLL_CONTROL, RST_CLK_PLL_CONTROL_PLL_CPU_PLD_Pos);
	
	while (CHF_BIT_PER(MDR_RST_CLK->CLOCK_STATUS, RST_CLK_CLOCK_STATUS_PLL_CPU_RDY_Pos));
	
	MDR_RST_CLK->CPU_CLOCK =
		(RST_CLK_CPU_CLOCK_CPU_C1_SEL_HSE << RST_CLK_CPU_CLOCK_CPU_C1_SEL_Pos) |
		(RST_CLK_CPU_CLOCK_CPU_C2_SEL_PLL_CPU << RST_CLK_CPU_CLOCK_CPU_C2_SEL_Pos) |
		(RST_CLK_CPU_CLOCK_CPU_C3_SEL_CPU_C2 << RST_CLK_CPU_CLOCK_CPU_C3_SEL_Pos) |
		(RST_CLK_CPU_CLOCK_HCLK_SEL_CPU_C3 << RST_CLK_CPU_CLOCK_HCLK_SEL_Pos);
}

fsStatus fs;

char label[12];
uint32_t ser_num;

int64_t FreeSize;

#define StrBufSize 200
char StrBuf[StrBufSize];

#define DirName "M0:\\Dir"
#define FileName "FileDir\\File.txt"

FILE *fin, *fout;
int Radius = 10;
int CircleRadius;
float CircleArea;

int main (void)
{
	ClkSetup ();
	
	// Initialize the M0: drive.
  fs = finit ("M0:");
	
  // Mount the M0: drive.
  fs = fmount ("M0:");
  
  fs = fvol ("M0:", label, &ser_num);
	
	FreeSize = ffree ("M0:");
	
	fs = fpwd ("M0:", StrBuf, StrBufSize);
	
	fs = fmkdir (DirName);
	
  fout = fopen (FileName, "w"); /* Create a file in subfolder on SD card.*/
	
  if (fout != NULL)
	{
    // Write data to file
		fprintf (fout, "Площадь круга радиуса %i равна %f.\n", Radius, 3.1415926*Radius*Radius);
    fclose (fout);
	}
	
	fin = fopen (FileName, "r"); /* Open a file in subfolder on SD card.*/
	
  if (fin != NULL)
	{
    // Read data from file
		fscanf (fin, "Площадь круга радиуса %i равна %f.\n", &CircleRadius, &CircleArea);
    fclose (fin);
	}
	
	// The drive is no more needed.
	// Unmount the M0: drive.
  fs = funmount ("M0:");
	
	// Uninitialize the M0: drive.
  fs = funinit ("M0:");
	
	while (true);
}
