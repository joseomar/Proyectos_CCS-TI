/* Copyright (c) 2007-2008 CSIRO
   Copyright (c) 2007-2009 Xiph.Org Foundation
   Written by Jean-Marc Valin */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//#ifdef HAVE_CONFIG_H
#include "config.h"
//#endif




#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */


#ifdef __cplusplus
 extern "C"
#endif


//#define  _EWL_CONSOLE_SUPPORT 1
//#define ENABLE_POSTFILTER 0

#include "celt.h"
#include "arch.h"
#include "celt_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//#include <ansi_parms.h>

//#include "console_io.h"
//#include "sys/uart_console_config.h"

// #include <stdarg.h>

#define MAX_PACKET 512


//#define _EWL_OS_DISK_FILE_SUPPORT 1
//#define _MSL_WIDE_CHAR 1
//#define  _EWL_WFILEIO_AVAILABLE 1
//#define MSL_FILE_CONSOLE_ROUTINES 1

#define TAM_FRAME 256
#define TAM_BUFFER 1

short alter_adc =0;
  
 celt_int16 short_in =0;
  celt_int16 buffer01[TAM_BUFFER] [TAM_FRAME]; //arreglo bidimencional  [paquetes , datos]
  celt_int16 buffer02[TAM_BUFFER] [TAM_FRAME];



short p_w=0; //indice del paquete a escribir
unsigned short i_w; //indice del dato a escribir 
                   //dentro del paquete p_w

short p_r=0; //indice del paquete a leer
unsigned short i_r; //indice del dato a leer
                   //dentro del paquete p_r

short cant_p=0; //cantidad de paquetes escritos en el buffer

char s_r=0; //indice del buffer de lectura

char s_w=0; //indice del buffer de escritura

int cant_conv=0; //cantidad de conversiones mientras se esta comprimiendo




char mensaje[]="HOLA MUNDO";
 char datoH=0;
 unsigned char datoL=0;
 
 int primer_byte=1;
 



 
 void SCI_send_string(char * mensaje);

void SCI_send_header(unsigned char *mensaje,int tam);

void SCI_send_ustring(unsigned char * mensaje, int tam);

void SCI_send_uchar( char dato);

short * readFrame();

void inicializar_buffers(void);

int switch_buffers(char read_buffer);

  void isrVadc1(void);


 
 __interrupt void isrVsci1rx(void)
{
// Write your interrupt code here ... 
  
  /*
  SCI1S1; //reconoce la interrupción
  
  
  
  
    
   if (s_w==0 ){
        //in[j]=(short)((fin01[(2*j+1)+c] <<8 ) | ( fin01[(2*j)+c] &0xFF)  );
        if (primer_byte)
            short_in=( celt_int16)(SCI1D<<8);//leo el caracter que entro
            
        else{
             short_in|=( celt_int16)(SCI1D &0xFF);
            buffer01[p_w][i_w]=short_in;
            i_w++;
        }
   } else{
        if (primer_byte)
            short_in=( celt_int16)(SCI1D<<8);//leo el caracter que entro
            
        else {
            short_in|=( celt_int16)(SCI1D &0xFF);
            buffer02[p_w][i_w]=short_in;
            i_w++;
        }
   }  
    
    primer_byte=(primer_byte==0);
  
    if (i_w>=TAM_FRAME){
      i_w=0;
      p_w++;
    }
    if (p_w>=TAM_BUFFER){
      
     //SCI1C2_SBK=1;
     //SCI1C2_RE=0;  //deshabilito la recepcion
     //SCI1C2_RIE=0; //deshabilito interrupcion por Rx 
     PTCD_PTCD3=1;//  deshabilito al transmisor
     //SCI_send_uchar(19);//mando XOFF
    }

    */
}
/* end of isrVsci1rx */


int switch_buffers(char read_buffer){
  char cambio= (read_buffer==0);
   
      #ifndef FCS_MODE

      if(p_w<TAM_BUFFER)       //#FIXME!! ¿como saber que ya termine de escribir en el otro buffer?
          return (0);    // estoy escribiendo en el otro buffer
      else

      #else
             inicializar_buffers();    
                        
      #endif      
            
          s_r=cambio;       //cambio el buffer de lectura
          p_r=0;            //pongo en cero sus punteros
          i_r=0;
          
          s_w=read_buffer;  //cambio el buffer de escritura
          p_w=0;            //pongo en cero sus punteros
          i_w=0;
          
          /*
          //SCI1C2_SBK=0;
          SCI1C2_RE=1;  //habilito la recepcion
          SCI1C2_RIE=1; //habilite interrupcion por Rx 
          PTCD_PTCD3=0;//  habilito al transmisor
          //SCI_send_uchar(17);//mando XON
          */
          return (1);
}


void inicializar_buffers(void ){
   /* pwrite_buffer=0;
    sel_buffer_write=0;
    */
    //SCI2C2_SBK=0;
    //SCI2C2_RIE=1; //habilite interrupcion por Rx 
    /*
    SCI1C2_TE=1; //habilito la transmicion
    SCI1D=0x17; //transmite XON para abrir transmision 
    while (SCI1S1_TDRE==0){//espero que se envie el dato
    } 
    */
    
    
    //SCI2C2_RE=1;  //habilito la recepcion
    //SCI_send_uchar(17);//mando XON
   // PTCD_PTCD3=0;//  habilito al transmisor
    p_w=0;
    i_w=0;
    #ifndef FCS_MODE    
        while (p_w <TAM_BUFFER)  {  } ////#FIXME!! espero a que el buffer 1 se llene
    #else
    for(int k=0;k<=TAM_FRAME;k++)
      for (int l=0;l<=TAM_BUFFER;l++)
            buffer01[l][k]=(rand() % 32767) - 16384;
    #endif
    s_w=1;
    s_r=0;
    
    p_w=0;
    p_r=0;
    i_w=0;
    i_r=0;
    
     //PTCD_PTCD3=0;//  habilito al transmisor
     

    //SCI1C2_SBK=0;
}




 // __interrupt void isrVadc1(void)
  void isrVadc1(void)
{
  // Write your interrupt code here ... 

 // alter_adc=(alter_adc==1)?0:1;//salteo una conversion 
   
  //if (alter_adc==1){//salteo una conversion
    
    if (s_w==0 )
        buffer01[p_w][i_w]=(ADCR<<4)-0x7FFF;             //capture dato del conversor 
    else
        buffer02[p_w][i_w]=(ADCR<<4)-0x7FFF;             //capture dato del conversor 
    i_w++;
    
    if (i_w>=TAM_FRAME){
        i_w=0;
        p_w++;
      }
  //}  //alter_adc
      
    /*
   
  
  if (pwrite_buffer>=TAM_BUFFER){
          sel_buffer_write=(sel_buffer_write==1)?0:1;
          pwrite_buffer=0;
        }
       */
}


static  celt_uint32
_le_32 (celt_uint32 i)
{
   celt_uint32 ret=i;
#if !defined(__LITTLE_ENDIAN__) && ( defined(WORDS_BIGENDIAN) || defined(__BIG_ENDIAN__) )
   ret =  (i>>24);
   ret += (i>>8) & 0x0000ff00;
   ret += (i<<8) & 0x00ff0000;
   ret += (i<<24);
#endif
   return ret;
}
 
 
 
 
 #include <stdarg.h>
 void print_c( char * fmt, ... )
{
  // create and start the va_list
  va_list listPointer ;
   va_start( listPointer, fmt ) ;

  // vsprintf is an example of
  // a function that works with
  // an "already started" va_list
   char buf[ 150 ] ;
  vsprintf( buf, fmt, listPointer ) ;
  SCI_send_string(buf);
  
}





static long Big_counter;
#define TPM3_PER_SEC 624994.039501416

void tic (void){
Big_counter = 0;
TPM3CNT = 0; 

}


__interrupt void isrVtpm3ovf(void)
{
//TPM3SC;
//lee registro de estado y control
TPM3SC_TOF=0;
//aclare bandera de sobreflujo

  Big_counter++;

}

void toc (void){
//Big_counter = 0;
//TPM3CNT = 0; 
double secs = 0;
char mensaje[80];
secs = (double) (Big_counter*0xFF+TPM3CNT) / (double)TPM3_PER_SEC;
    print_c("Elapsed seconds = %f \n", secs);
   // SCI_send_string(mensaje);

}

char KBI=0;

__interrupt void isrVirq(void)
{
  /* Write your interrupt code here ... */
        IRQSC_IRQACK=1;
          KBI=1;
}
/* end of isrVirq */

  


__interrupt void isrVftm1ovf(void)
{
  /* Write your interrupt code here ... */
  
  FTM1SC_TOF=0;      //limpio bandera de sobreflujo
  isrVadc1();        //llamo a leer el nuevo valor en ADC  
  ADCSC1_ADCH=8;   //inicio una nueva conversion
  cant_conv++;

}
/* end of isrVftm1ovf */


void MCU_init(void); /* Device initialization function declaration */




int main(void) {
  
  MCU_init(); /* call Device Initialization */
  SOPT_COPE =0;        /*apago el watchdog*/
  /* include your code here */
  
   int err;
   //char *inFile, *outFile;
 //  FILE *fin, *fout;
   CELTHeader header;
   far CELTMode *mode=NULL;
   far CELTEncoder *enc;
   //CELTDecoder *dec;
   int len;
   celt_int32 frame_size;
   int channels;
   int bytes_per_packet;
   unsigned char data[MAX_PACKET];
   unsigned char packet[60];
   int rate;
   int complexity;
   int bitrate;
   int i;
   short print=0;
#if !(defined (FIXED_POINT) && defined(STATIC_MODES))
   
   double rmsd = 0;
#endif

   char msj_error [30];
   int count = 0;
   celt_int32 skip;
   celt_int16 *in_in, *out;
/*
   if (argc != 9 && argc != 8 && argc != 7)
   {
      fprintf (stderr, "Usage: testcelt <rate> <channels> <frame size> "
               " <bytes per packet> [<complexity> [packet loss rate]] "
               "<input> <output>\n");
      return 1;
   }
   
   rate = atoi(argv[1]);
   channels = atoi(argv[2]);
   TAM_FRAME = atoi(argv[3]);
   */
   rate=7500;
   frame_size=TAM_FRAME;
   channels=1;
   
  
    //ADCSC1_ADCH=8;//selecciono el canal del pin 6 del  puerto A
     
      
   mode = celt_mode_create(rate, frame_size, NULL);
   celt_mode_info(mode, CELT_GET_LOOKAHEAD, &skip);
   
   if (mode == NULL)
   {
      //fprintf(stderr, "failed to create a mode\n");
      return 1;
   }
   
  // bytes_per_packet = atoi(argv[4]);
   //bytes_per_packet = 32;
   bitrate=10;
   bytes_per_packet = (bitrate*1000*frame_size/rate+4)/8;
   if (bytes_per_packet < 0 || bytes_per_packet > MAX_PACKET)
   {
      //fprintf (stderr, "bytes per packet must be between 0 and %d\n",
        //                MAX_PACKET);
      return 1;
   }
   /*
   inFile = argv[argc-2];
   fin = fopen("prueba", "rb");
   if (!fin)
   {
  //    fprintf (stderr, "Could not open input file %s\n", argv[argc-2]);
      return 1;
   }
   //outFile = argv[argc-1];
   fout = fopen(outFile, "wb+");
   if (!fout)
   {
     // fprintf (stderr, "Could not open output file %s\n", argv[argc-1]);
      return 1;
   }
     */
   
   celt_header_init(&header, mode, channels);
   header.nb_channels = channels;

   enc = celt_encoder_create(mode, channels, &err);
   if (err != 0)
   {
      //fprintf(stderr, "Failed to create the encoder: %s\n", celt_strerror(err));
      return 1;
   }
   
   /*
   dec = celt_decoder_create(mode, channels, &err);
   if (err != 0)
   {
   //   fprintf(stderr, "Failed to create the decoder: %s\n", celt_strerror(err));
      return 1;
   }
   
   if (argc>7)
   {
      complexity=atoi(argv[5]);
      celt_encoder_ctl(enc,CELT_SET_COMPLEXITY(complexity));
   }
    */ 
    header.bytes_per_packet=bytes_per_packet;
    
   // print_c("bytes_per_packet= %d\n", bytes_per_packet);
    
   //in_in = (celt_int16*)malloc(frame_size*channels*sizeof(celt_int16));
   out = (celt_int16*)malloc(frame_size*channels*sizeof(celt_int16));
   /*
   if (in_in == NULL)
   {
      //fprintf(stderr, "failed to create a mode\n");
      return 1;
   }
     */
     
    
   //ADCSC1_ADCH=8;//selecciono el canal del pin 6 del  puerto A
   
   celt_header_to_packet(&header,packet,60);
     
   
    SCI_send_header(packet,60) ;
   while (KBI==0){
  }
  //PTAD_PTAD4=1;LED1
  //PTAD_PTAD5=1; LED2
  
  //ADCSC1_ADCH=8;//selecciono el canal del pin 6 del  puerto A
  cant_conv=0;
  FTM1SC_TOIE=1;// inicio el timer1
   inicializar_buffers();

  celt_encoder_ctl(enc,CELT_RESET_STATE);
   //for (int k =0;k<50000;k++)
   for(;;)
   {  
      //err = fread(in, sizeof(short), frame_size*channels, fin);
      
      
       
       in_in=readFrame();
        
        
      /*if (feof(fin))
         break;
    
         */
         
       //celt_encoder_print(enc);
      
     // in_in=fin01[0];
      //tic();     
      
        
      len = celt_encode(enc, in_in, NULL, data, bytes_per_packet);//frame_size
      //toc();
      //if (cant_conv>=TAM_FRAME)
       //print_c("cant_conv %d / tam frame %d = %f \n",cant_conv,TAM_FRAME,(float)cant_conv/(float)TAM_FRAME);
      //print_c("%d",cant_conv);
       cant_conv=0; 
       /*
      if (len <= 0){         
         SCI_send_string ("celt_encode() failed: %d\n");
          break;
      }
      
      
      //celt_encoder_print(enc);
      //exit(0);
    //PTCD_PTCD3=PTCD_PTCD3==1?0:1;
     
    //err = celt_decode(dec, data, len, out);
    if (err < 0){
          
         SCI_send_string("celt_decode() failed: %d\n");
         //SCI_send_header(err,2);
         
          break;         
      }
      
       
    
    //for (i=0;i<frame_size;i++)
      //    out[i]=in_in[i];
            
     // count++;
   //   fwrite(out+skip, sizeof(short), (frame_size-skip)*channels, fout);
   //SCI_send_uchar(len);   
   //SCI_send_header(in_in,frame_size*2);
  //#if !defined(__MEM_ANALISYS__)
  */
      SCI_send_header(data,len);
      /*
  //#endif
      //skip = 0;
      */
   }
   //PRINT_MIPS(stderr);
   
   celt_encoder_destroy(enc);
   
   //celt_decoder_destroy(dec);
   /*
   fclose(fin);
   fclose(fout);
   */
   celt_mode_destroy(mode);
   free(in_in);
   //free(out);
#ifdef RESYNTH
   if (rmsd > 0)
   {
      rmsd = sqrt(rmsd/(1.0*frame_size*channels*count));
    //  fprintf (stderr, "Error: encoder doesn't match decoder\n");
     // fprintf (stderr, "RMS mismatch is %f\n", rmsd);
      return 1;
   } else {
     // fprintf (stderr, "Encoder matches decoder!!\n");
   }
#endif
   //return 0;




  for(;;) {
    /* __RESET_WATCHDOG(); by default, COP is disabled with device init. When enabling, also reset the watchdog. */
  } /* loop forever */
  /* please make sure that you never leave main */
}



void SCI_send_uchar( char dato){
 SCI1C2_TE=1; //habilito la transmicion
 SCI1D=dato;  //escribo el dato 
 while (SCI1S1_TDRE==0){//espero que se envie el dato
    } 
 
}


void SCI_send_string(char *mensaje){
 unsigned long tam =strlen(mensaje);
 int i;
 for ( i=0;i<tam;i++){
    SCI2C2_TE=1; //habilito la transmicion
    SCI2D= mensaje[i];     //escribo el dato 
    while (SCI2S1_TDRE==0){//espero que se envie el dato
    }    
    //SCI2C2_TE=0;
        
 }
}

void SCI_send_header(unsigned char *mensaje,int tam){
 //int tam =strlen(mensaje);
 unsigned char dato=0;
 int i;
 for ( i=0;i<tam;i++){
    
    SCI2C2_TE=1;//SCI1C2_TE=1; //habilito la transmicion
    dato=mensaje[i];
    //if ((dato == 17)|(dato==19))
    //  dato&=0xFE; //borro el ultimo bit el menos significativo 
                    //para que no sea ni XON ni XOFF
    SCI2D=dato ;  //SCI1D=dato ;     //escribo el dato 
    while (SCI2S1_TDRE==0){//while (SCI1S1_TDRE==0){//espero que se envie el dato    
    }    
        
 }
 PTAD_PTAD5=PTAD_PTAD5?0:1;
//SCI2C2_TE=0;
}


void SCI_send_ustring(unsigned char *mensaje,int tam){
 //int tam =strlen(mensaje);
 int i,j;
 unsigned char msj[4];
 celt_uint32 dato;
 
 
 
 for (j=0;j<tam;j+=4){
      
      msj[0]=mensaje[j];
      msj[1]=mensaje[j+1];
      msj[2]=mensaje[j+2];
      msj[3]=mensaje[j+3];
      dato= (celt_uint32) (msj[0])<<24+(celt_uint32) (msj[1])<<16+\
      (celt_uint32) (msj[2])<<8+(celt_uint32) (msj[3]);
      
      dato= _le_32 (dato);
      /*
      msj[0]=(unsigned char) dato>>24;
      msj[1]=(unsigned char) (dato>>16);
      msj[2]=(unsigned char) (dato>>8);
      msj[3]=(unsigned char) (dato);
       */
     
    for ( i=0;i<4;i++){
      SCI1C2_TE=1; //habilito la transmicion
      SCI1D=(unsigned char) msj[i];
      while (SCI1S1_TDRE==0){//espero que se envie el dato
      }    
    }
        
 }
}

celt_int16 * readFrame(){


if(p_r>=TAM_BUFFER)
 while (!switch_buffers(s_r)){}//espero para evitar leer y 
                                        //escribir en el mismo buffer 

if (s_r==0)
  return( buffer01[p_r++]) ;
else
  return(buffer02[p_r++]) ;

//p_r++;
}



/*
for ( j =0;(j<TAM_FRAME);j++){
  

      if (sel_buffer_read==0)
          //in[j]= (short)((short)fin01[(2*j)+c]*0xFF+(unsigned)fin01[(2*j+1)+c]);
          in[j]=(short)((fin01[(2*j+1)+c] <<8 ) | ( fin01[(2*j)+c] &0xFF)  );
      else
          in[j]=(short)((fin02[(2*j+1)+c] <<8 ) | ( fin02[(2*j)+c] &0xFF)  );
          
        
      if (c+j>=TAM_BUFFER){
          break;
          /*
          sel_buffer_read= (sel_buffer_read==0);//cambio de buffer 0 o 1
          while (sel_buffer_read<>sel_buffer_write){//para evitar leer y escribir en el mismo buffer
          }
          c=-j;
          //j=0;
          */
          
          
          /*
      }
      
      if (print){
        char datoH=( char)(in[j]>>8);
        char datoL= ( unsigned char)(in[j]); 
        SCI_send_uchar(datoH);
        SCI_send_uchar(datoL);
      }
      
}
     
pread_buffer=j+c;
*/


