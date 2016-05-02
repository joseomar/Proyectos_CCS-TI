#ifndef __SPI__
#define __SPI__

/* Includes */

#include "FslTypes.h"
#include "derivative.h"


/* definitions */


//#define SPI_SS    PTGD_PTGD3      /* Slave Select EN REALIDAD PTB3*/
//#define _SPI_SS   PTGDD_PTGDD3 

#define SPI_SS    PTED_PTED7      /* Slave Select placa edukit jm128 ss_spi*/
#define _SPI_SS   PTEDD_PTEDD7    
   

//#define SPI_SS    PTBD_PTBD5      /* Slave Select */
//#define _SPI_SS   PTBDD_PTBDD5    



#define ENABLE    0
#define DISABLE   1

/* Global Variables */

/* Prototypes */
void SD_ChipSelect(int);//Deberia llamarse SPI_ChipSelect
void SPI_Init(void);
void SPI_Send_byte(UINT8 u8Data);
UINT8 SPI_Receive_byte(void);
void SPI_High_rate(void);


#endif /* __SPI__ */