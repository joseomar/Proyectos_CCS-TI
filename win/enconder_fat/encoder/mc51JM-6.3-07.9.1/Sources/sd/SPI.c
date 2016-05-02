#include "SPI.h"

/********** Adaptación de SDCS a GPIO ***********/
static int InitFlag=0;
void SD_ChipSelect(int State){
  if(InitFlag==0){
    PTGDD_PTGDD3=1; 
    InitFlag=1;
  }
  PTGD_PTGD3=State;
}
 
/************************************************/
void SPI_Init(void)
{
  
  //SOPT2 = SOPT2_SPI1PS_MASK;  // Drive PTE as SPI port
  
  SPI_SS = 1;
  _SPI_SS= 1;
  
  SPI1BR = 0x06; // 375 Khz    
  SPI1BR = 0x12; //en jm128=0x12 375 Khz  Bus 6MHz CPU 48 MHz  PLL Bypassed
  SPI1BR = 0x43; //en jm128=0x33 375 Khz  Bus 27MHz CPU 48 MHz  PLL Enganged
 		
  SPI1C2 = 0x00;  
  SPI1C1 = SPI1C1_SPE_MASK | SPI1C1_MSTR_MASK;

}

/************************************************/
void SPI_Send_byte(UINT8 u8Data)
{
	while(!SPI1S_SPTEF) ;
  (void)SPI1S;
	SPI1DL=u8Data;
	
	
}

/************************************************/
UINT8 SPI_Receive_byte(void)
{
  (void)SPI1DL;
	SPI1DL=0xFF;
	while(!SPI1S_SPRF) ;	
	return(SPI1DL);
}

/************************************************/
void SPI_High_rate(void)
{
  SPI1C1 = 0x00;
  //SPI1BR = 0x12; // 4 Mhz, lo mas proximo posible a 4.5 Mhz     		
  SPI1BR = 0x20; // en jm128= 0x20  1.0 MHz    Bus 6MHz CPU 48 MHz  PLL Bypassed
  SPI1BR = 0x21; //en jm128=0x21 1.225 Mhz  Bus 27MHz CPU 54 MHz  PLL Enganged
  SPI1BR = 0x10; //en jm128=0x10 6.000 Mhz  Bus 27MHz CPU 48 MHz  PLL Enganged

  SPI1C1 = SPI1C1_SPE_MASK | SPI1C1_MSTR_MASK;

}
