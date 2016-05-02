/* SCI definitions */
#define SCIBRH (*((volatile unsigned char*)(0x00C8)))
#define SCIBRL (*((volatile unsigned char*)(0x00C9)))
#define SCICR1 (*((volatile unsigned char*)(0x00CA)))
#define SCICR2 (*((volatile unsigned char*)(0x00CB)))
#define SCISR1 (*((volatile unsigned char*)(0x00CC)))
#define SCISR2 (*((volatile unsigned char*)(0x00CD)))
#define SCIDRH (*((volatile unsigned char*)(0x00CE)))
#define SCIDRL (*((volatile unsigned char*)(0x00CF)))
/* ERROR code and STATUS definitions */
#define ERROR_OK 1
#define ERROR_ERROR 0
#define START_CYCLE 1
#define WAIT_CYCLE 0
/*Global variables*/
unsigned char SCIIniTx;
unsigned char SCIString[12]={'F','R','E','E','S','C','A','L','E',0xa,0xd,'\0'};
unsigned char *SCIStringp;
unsigned char Stringcase;
#pragma CODE_SEG __NEAR_SEG NON_BANKED
/*
 * SCIIsr: Interrupt Service routine for the SCI module
 * Clear TDRE and RDRF flags
 * In transmission sends a character string until NULL character
 * In reception changes from uppercase to lowercase as required
 *
 * Parameters: None
 *
 * Return : None
 */
interrupt void SCIIsr(void) {
	if (SCISR1 & 0x80){ /*If transmission flag is set*/
		SCISR1;
		if(*(SCIStringp++) != '\0'){
			if(*SCIStringp > 0xD){ /*Avoid to change CR and LF characters*/
				SCIDRL=*SCIStringp + Stringcase;
			}
			else{
				SCIDRL=*SCIStringp;
			}
		}
		else{
			SCIIniTx=START_CYCLE; /*Start new transmission cycle*/
			SCICR2 &= 0x7F; /*Disable TDRE interrupt*/
		}
	}
	if(SCISR1 & 0x20){ /*If reception flag is set*/
		SCISR1;
		if(SCIDRL == 'U' || SCIDRL == 'u'){
			Stringcase = 0x00; /*Uppercase the character string*/
		}

		else if(SCIDRL == 'L' || SCIDRL == 'l'){
			Stringcase = 0x20; /*Lowercase the character string*/
		}
	}
	return;
}
#pragma CODE_SEG DEFAULT
/*
 * SCIConfig: Configures SCI port at 19200 bps, 8 data bits, no parity,
 * enable transmission, reception and RDRF interrupt
 *
 * Parameters: None
 *
 * Return : Error code
 */
unsigned char SCIConfig(void){
	SCIBRL = 0x0D; /*Configure baud rate at 19200 bps with*/
	SCIBRH = 0x00; /*an SCI clock modulo of 4MHz*/
	SCICR1 = 0x00; /*8 data bits, no parity*/
	SCICR2 = 0x2C; /*Enable Tx, Rx, and RDRF interrupt*/
	if (SCISR1 & 0x80){ /*Poll TDRE flag*/
		return ERROR_OK; /*TDRE set, return OK*/
	}
	else{
		return ERROR_ERROR; /*TDRE clear, return ERROR*/
	}
}
/*
 * SCITx: Write data byte to SCIDRL register to transmission and
 * enable TDRE interrrupt.
 *
 * Parameters: SCIByte
 *
 * Return : None
 */
void SCITx(unsigned char SCIByte){
	SCIDRL = SCIByte; /*Write data byte to SCIDRL register*/
	SCICR2 |= 0x80; /*Enable TDRE interrupt*/
}
unsigned char main(void){
	if (SCIConfig()) /*Configure SCI port*/
		;
	else{
		return ERROR_ERROR;
	}
	EnableInterrupts;

	SCIIniTx = START_CYCLE; /*Initialize transmission cycle flag*/
	for (;;){
		if(SCIIniTx == START_CYCLE){
			SCIIniTx = WAIT_CYCLE;
			SCIStringp=SCIString; /*Set pointer to character string*/
			SCITx(*SCIStringp + Stringcase);/*Send first byte of string*/
		}
	}
	return ERROR_OK;
}
