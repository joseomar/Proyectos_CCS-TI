/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : Cpu.C
**     Project   : mc51_6_3
**     Processor : MCF51AC256ACLK
**     Component : MCF51AC256A_80
**     Version   : Component 01.005, Driver 01.08, CPU db: 3.00.053
**     Datasheet : MCF51AC256RM Rev. 4, 5/2009
**     Compiler  : CodeWarrior ColdFireV1 C Compiler
**     Date/Time : 30/03/2011, 06:50 p.m.
**     Abstract  :
**         This bean "MCF51AC256A_80" contains initialization of the
**         CPU and provides basic methods and events for CPU core
**         settings.
**     Settings  :
**
**     Contents  :
**         EnableInt  - void Cpu_EnableInt(void);
**         DisableInt - void Cpu_DisableInt(void);
**
**     Copyright : 1997 - 2009 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

/* MODULE Cpu. */
#include "SCI3.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Events.h"
#include "Cpu.h"

/* Global variables */
volatile far word SR_reg;              /* Current CCR register */
volatile byte SR_lock = 0x00;


/*
** ===================================================================
**     Method      :  Cpu_Cpu_Interrupt (component MCF51AC256A_80)
**
**     Description :
**         This ISR services an unused interrupt/exception vector.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
ISR(Cpu_Interrupt)
{
  /* This code can be changed using the CPU bean property "Build Options / Unhandled int code" */
  /* asm (HALT) */
}


/*
** ===================================================================
**     Method      :  Cpu_DisableInt (component MCF51AC256A_80)
**
**     Description :
**         Disables maskable interrupts
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/*
void Cpu_DisableInt(void)

**      This method is implemented as macro in the header module. **
*/

/*
** ===================================================================
**     Method      :  Cpu_EnableInt (component MCF51AC256A_80)
**
**     Description :
**         Enables maskable interrupts
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
/*
void Cpu_EnableInt(void)

**      This method is implemented as macro in the header module. **
*/

/*
** ===================================================================
**     Method      :  __initialize_hardware (component MCF51AC256A_80)
**
**     Description :
**         Configure the basic system functions (timing, etc.).
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/

void __initialize_hardware(void)
{
  /* ### MCF51AC256A_80 "Cpu" init code ... */
  /*  PE initialization code after reset */

  /* Common initialization of the write once registers */
  /* SOPT: COPE=0,COPT=1,STOPE=0,WAITE=1,??=0,??=0,??=1,??=1 */
  setReg8(SOPT, 0x53);                  
  /* SPMSC1: LVDF=0,LVDACK=0,LVDIE=0,LVDRE=1,LVDSE=1,LVDE=1,??=0,BGBE=0 */
  setReg8(SPMSC1, 0x1C);                
  /* SPMSC2: LVWF=0,LVWACK=0,LVDV=0,LVWV=0,PPDF=0,PPDACK=0,??=0,PPDC=0 */
  setReg8(SPMSC2, 0x00);                
  /* SMCLK: MPE=0,MCSEL=0 */
  clrReg8Bits(SMCLK, 0x17);             
  /* Initialization of CPU registers */
  asm {
    /* VBR: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,ADDRESS=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
    clr.l d0
    movec d0,VBR
    /* CPUCR: ARD=0,IRD=0,IAE=0,IME=0,BWD=0,??=0,FSD=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
    clr.l d0
    movec d0,CPUCR
  }
  /*  System clock initialization */
  /* MCGC2: BDIV=1,RANGE=0,HGO=0,LP=0,EREFS=0,ERCLKEN=0,EREFSTEN=0 */
  setReg8(MCGC2, 0x40);                /* Set MCGC2 register */ 
  /* MCGC1: CLKS=0,RDIV=0,IREFS=1,IRCLKEN=0,IREFSTEN=0 */
  setReg8(MCGC1, 0x04);                /* Set MCGC1 register */ 
  /* MCGC3: LOLIE=0,PLLS=0,CME=0,DIV32=0,VDIV=1 */
  setReg8(MCGC3, 0x01);                /* Set MCGC3 register */ 
  /* MCGC4: ??=0,??=0,DMX32=0,??=0,??=0,??=0,DRST_DRS=0 */
  setReg8(MCGC4, 0x00);                /* Set MCGC4 register */ 
  while(!MCGSC_LOCK) {                 /* Wait until FLL is locked */
  }
  
  /*** End of PE initialization code after reset ***/
}

/*
** ===================================================================
**     Method      :  PE_low_level_init (component MCF51AC256A_80)
**
**     Description :
**         Initializes beans and provides common register initialization. 
**         The method is called automatically as a part of the 
**         application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void PE_low_level_init(void)
{
  /* SCGC1: TPM3=1,FTM2=1,FTM1=1,ADC=1,CAN=1,IIC=1,SCI2=1,SCI1=1 */
  setReg8(SCGC1, 0xFF);                 
  /* SCGC2: CRC=1,FLS=1,IRQ=1,KBI=1,ACMP=1,RTI=1,SPI2=1,SPI1=1 */
  setReg8(SCGC2, 0xFF);                 
  /* Common initialization of the CPU registers */
  /* PTASE: PTASE7=0,PTASE6=0,PTASE5=0,PTASE4=0,PTASE3=0,PTASE2=0,PTASE1=0,PTASE0=0 */
  setReg8(PTASE, 0x00);                 
  /* PTBSE: PTBSE7=0,PTBSE6=0,PTBSE5=0,PTBSE4=0,PTBSE3=0,PTBSE2=0,PTBSE1=0,PTBSE0=0 */
  setReg8(PTBSE, 0x00);                 
  /* PTCSE: PTCSE6=0,PTCSE5=0,PTCSE4=0,PTCSE3=0,PTCSE2=0,PTCSE1=0,PTCSE0=0 */
  clrReg8Bits(PTCSE, 0x7F);             
  /* PTDSE: PTDSE7=0,PTDSE6=0,PTDSE5=0,PTDSE4=0,PTDSE3=0,PTDSE2=0,PTDSE1=0,PTDSE0=0 */
  setReg8(PTDSE, 0x00);                 
  /* PTESE: PTESE7=0,PTESE6=0,PTESE5=0,PTESE4=0,PTESE3=0,PTESE2=0,PTESE1=0,PTESE0=0 */
  setReg8(PTESE, 0x00);                 
  /* PTFSE: PTFSE7=0,PTFSE6=0,PTFSE5=0,PTFSE4=0,PTFSE3=0,PTFSE2=0,PTFSE1=0,PTFSE0=0 */
  setReg8(PTFSE, 0x00);                 
  /* PTGSE: PTGSE6=0,PTGSE5=0,PTGSE4=0,PTGSE3=0,PTGSE2=0,PTGSE1=0,PTGSE0=0 */
  clrReg8Bits(PTGSE, 0x7F);             
  /* PTHSE: PTHSE6=0,PTHSE5=0,PTHSE4=0,PTHSE3=0,PTHSE2=0,PTHSE1=0,PTHSE0=0 */
  clrReg8Bits(PTHSE, 0x7F);             
  /* PTJSE: PTJSE7=0,PTJSE6=0,PTJSE5=0,PTJSE4=0,PTJSE3=0,PTJSE2=0,PTJSE1=0,PTJSE0=0 */
  setReg8(PTJSE, 0x00);                 
  /* PTADS: PTADS7=0,PTADS6=0,PTADS5=0,PTADS4=0,PTADS3=0,PTADS2=0,PTADS1=0,PTADS0=0 */
  setReg8(PTADS, 0x00);                 
  /* PTBDS: PTBDS7=0,PTBDS6=0,PTBDS5=0,PTBDS4=0,PTBDS3=0,PTBDS2=0,PTBDS1=0,PTBDS0=0 */
  setReg8(PTBDS, 0x00);                 
  /* PTCDS: ??=0,PTCDS6=0,PTCDS5=0,PTCDS4=0,PTCDS3=0,PTCDS2=0,PTCDS1=0,PTCDS0=0 */
  setReg8(PTCDS, 0x00);                 
  /* PTDDS: PTDDS7=0,PTDDS6=0,PTDDS5=0,PTDDS4=0,PTDDS3=0,PTDDS2=0,PTDDS1=0,PTDDS0=0 */
  setReg8(PTDDS, 0x00);                 
  /* PTEDS: PTEDS7=0,PTEDS6=0,PTEDS5=0,PTEDS4=0,PTEDS3=0,PTEDS2=0,PTEDS1=0,PTEDS0=0 */
  setReg8(PTEDS, 0x00);                 
  /* PTFDS: PTFDS7=0,PTFDS6=0,PTFDS5=0,PTFDS4=0,PTFDS3=0,PTFDS2=0,PTFDS1=0,PTFDS0=0 */
  setReg8(PTFDS, 0x00);                 
  /* PTGDS: ??=0,PTGDS6=0,PTGDS5=0,PTGDS4=0,PTGDS3=0,PTGDS2=0,PTGDS1=0,PTGDS0=0 */
  setReg8(PTGDS, 0x00);                 
  /* PTHDS: ??=0,PTHDS6=0,PTHDS5=0,PTHDS4=0,PTHDS3=0,PTHDS2=0,PTHDS1=0,PTHDS0=0 */
  setReg8(PTHDS, 0x00);                 
  /* PTJDS: PTJDS7=0,PTJDS6=0,PTJDS5=0,PTJDS4=0,PTJDS3=0,PTJDS2=0,PTJDS1=0,PTJDS0=0 */
  setReg8(PTJDS, 0x00);                 
  /* ### Shared modules init code ... */
  /* ### Init_SCI "SCI3" init code ... */
  SCI3_Init();
  /* INTC_WCR: ENB=1,??=0,??=0,??=0,??=0,MASK=0 */
  setReg8(INTC_WCR, 0x80);              
  /* Set initial interrupt priority 0 */
  asm {
    move.w SR,D0;
    andi.l #0xF8FF,D0;
    move.w D0,SR;
  }
}

/* Initialization of the CPU registers in FLASH */

/* NVPROT: FPS6=1,FPS5=1,FPS4=1,FPS3=1,FPS2=1,FPS1=1,FPS0=1,FPOPEN=1 */
unsigned char NVPROT_INIT @0x0000040D = 0xFF;

/* NVOPT: KEYEN1=0,KEYEN0=1,??=1,??=1,??=1,??=1,SEC1=1,SEC0=1 */
unsigned char NVOPT_INIT @0x0000040F = 0x7F;
/* END Cpu. */

/*
** ###################################################################
**
**     This file was created by Processor Expert 3.07 [04.34]
**     for the Freescale ColdFireV1 series of microcontrollers.
**
** ###################################################################
*/
