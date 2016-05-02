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
#define ENABLE_POSTFILTER 0

#include "celt.h"
#include "arch.h"
#include "celt_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "FslTypes.h"
#include "Fat.h"

//#include <ansi_parms.h>

//#include "console_io.h"
//#include "sys/uart_console_config.h"

#define MAX_PACKET 1275


//#define _EWL_OS_DISK_FILE_SUPPORT 1
//#define _MSL_WIDE_CHAR 1
//#define  _EWL_WFILEIO_AVAILABLE 1
//#define MSL_FILE_CONSOLE_ROUTINES 1


const long TAM_BUFFER=1000;

  short fin01[TAM_BUFFER];

  short fin02[TAM_BUFFER];

long pwrite_buffer=0;

long pread_buffer=0;


int sel_buffer_write=0;

int sel_buffer_read=0;


 char datoH=0;
 unsigned char datoL=0;

  __interrupt void isrVadc1(void)
{
  // Write your interrupt code here ... 

  if (sel_buffer_write==0 )
      fin01[pwrite_buffer]=(ADCR<<4)-0x7FFF;             //capture dato del conversor 
  else
      fin02[pwrite_buffer]=(ADCR<<4)-0x7FFF;             //capture dato del conversor 
  pwrite_buffer++;
  if (pwrite_buffer>=TAM_BUFFER){
          sel_buffer_write=(sel_buffer_write==1)?0:1;
          pwrite_buffer=0;
        }
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

//void isrVadc1(void);

void MCU_init(void); /* Device initialization function declaration */


void SCI_send_string(char * mensaje, int tam);

void SCI_send_header(unsigned char *mensaje,int tam);

void SCI_send_ustring(unsigned char * mensaje, int tam);

void SCI_send_uchar( char dato);

short readFrame(  short * in, short frame_size, int print);




int main(void) {
  
  MCU_init(); /* call Device Initialization */
  //SOPT_COPE =0;        /*apago el watchdog*/
  /* include your code here */
  
   int err;
   //char *inFile, *outFile;
 //  FILE *fin, *fout;
 CELTHeader header;
   CELTMode *mode=NULL;
   CELTEncoder *enc;
  // CELTDecoder *dec;
   char len;
   celt_int32 frame_size;
   int channels;
   int bytes_per_packet;
   unsigned char data[MAX_PACKET];
   unsigned char packet[60];
   int rate;
   int complexity;
   int i;
   short print=0;
#if !(defined (FIXED_POINT) && defined(STATIC_MODES))
   
   double rmsd = 0;
#endif

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
   frame_size = atoi(argv[3]);
   */
   
   
   rate=31250;
   frame_size=64;
   channels=1;
   
   ADCSC1_ADCH=8;//selecciono el canal del pin 6 del  puerto A

   
   mode = celt_mode_create(rate, frame_size, NULL);
   celt_mode_info(mode, CELT_GET_LOOKAHEAD, &skip);
   
   if (mode == NULL)
   {
      //fprintf(stderr, "failed to create a mode\n");
      return 1;
   }
   
  // bytes_per_packet = atoi(argv[4]);
   bytes_per_packet = 32;
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
   
   
  /* dec = celt_decoder_create(mode, channels, &err);
   if (err != 0)
   {
   //   fprintf(stderr, "Failed to create the decoder: %s\n", celt_strerror(err));
      return 1;
   }
   /*
   if (argc>7)
   {
      complexity=atoi(argv[5]);
      celt_encoder_ctl(enc,CELT_SET_COMPLEXITY(complexity));
   }
    */ 
    header.bytes_per_packet=bytes_per_packet;
    
    
    celt_header_to_packet(&header,packet,60);
    SCI_send_header(packet,60);
   in_in = (celt_int16*)malloc(frame_size*channels*sizeof(celt_int16));
   out = (celt_int16*)malloc(frame_size*channels*sizeof(celt_int16));
   if (in_in == NULL)
   {
      //fprintf(stderr, "failed to create a mode\n");
      return 1;
   }
   
    unsigned char nbByte=8;
  
  unsigned char u8Error=111;
  
   int ret=-1;
   
  ret=SD_Init(); 
  FAT_Read_Master_Block();
  
  
  u8Error=FAT_FileOpen("PRUEBA.oga",DELETE);
  FAT_FileClose();
  
  u8Error=FAT_FileOpen("PRUEBA.oga",CREATE);
  
  FAT_FileWrite("rarara22",nbByte);
   
   for(int i= 0;i<25;i++)
   {
      //err = fread(in, sizeof(short), frame_size*channels, fin);
        readFrame(in_in,frame_size*channels,print);
        
        
      /*if (feof(fin))
         break;
         */
      len = celt_encode(enc, in_in, NULL, data, bytes_per_packet);//frame_size
      if (len <= 0)
          break;
        // fprintf (stderr, "celt_encode() failed: %s\n", celt_strerror(len));

      // This is for simulating bit errors 
#if 0
      int errors = 0;
      int eid = 0;
      // This simulates random bit error 
      for (i=0;i<len*8;i++)
      {
         if (rand()%atoi(argv[8])==0)
         {
            if (i<64)
            {
               errors++;
               eid = i;
            }
            data[i/8] ^= 1<<(7-(i%8));
         }
      }
      if (errors == 1)
         data[eid/8] ^= 1<<(7-(eid%8));
      else if (errors%2 == 1)
         data[rand()%8] ^= 1<<rand()%8;
#endif

#if 0 // Set to zero to use the encoder's output instead 
      // This is to simulate packet loss 
      if (argc==9 && rand()%1000<atoi(argv[argc-3]))
      //if (errors && (errors%2==0))
         //err = celt_decode(dec, NULL, len, out, frame_size);
      else
         //err = celt_decode(dec, data, len, out, frame_size);
      //if (err != 0)
        // fprintf(stderr, "celt_decode() failed: %s\n", celt_strerror(err));
#else
      for (i=0;i<frame_size*channels;i++)
         out[i] = in_in[i];
#endif
#if !(defined (FIXED_POINT) && defined(STATIC_MODES))
      for (i=0;i<frame_size*channels;i++)
      {
         rmsd += (in_in[i]-out[i])*1.0*(in_in[i]-out[i]);
         //out[i] -= in[i];
      }
#endif
      count++;
   //   fwrite(out+skip, sizeof(short), (frame_size-skip)*channels, fout);
   //SCI_send_uchar(len); 
   FAT_FileWrite(data,len);  
   //SCI_send_ustring(data,len);
      skip = 0;
   }
   //PRINT_MIPS(stderr);
   
   FAT_FileClose();
   celt_encoder_destroy(enc);
   /*
   celt_decoder_destroy(dec);
   fclose(fin);
   fclose(fout);
   */
   celt_mode_destroy(mode);
   free(in_in);
   free(out);
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


void SCI_send_string(char *mensaje,int tam){
 //int tam =strlen(mensaje);
 int i;
 for ( i=0;i<tam;i++){
    SCI1C2_TE=1; //habilito la transmicion
    SCI1D= mensaje[i];     //escribo el dato 
    while (SCI1S1_TDRE==0){//espero que se envie el dato
    }    
        
 }
}

void SCI_send_header(unsigned char *mensaje,int tam){
 //int tam =strlen(mensaje);
 int i;
 for ( i=0;i<tam;i++){
    SCI1C2_TE=1; //habilito la transmicion
    SCI1D= mensaje[i];     //escribo el dato 
    while (SCI1S1_TDRE==0){//espero que se envie el dato
    }    
        
 }
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
      SCI1D=(unsigned char) dato>> 8*(3-i);
      while (SCI1S1_TDRE==0){//espero que se envie el dato
      }    
    }
        
 }
}

short readFrame(short *in, short frame_size, int print){
int j, c;
c=pread_buffer;
for ( j =0;(j<frame_size);j++){
  

      if (sel_buffer_read==0)
          in[j]= fin01[j+c];
      else
          in[j]= fin02[j+c];
          
        
      if (c+j>=TAM_BUFFER){
          sel_buffer_read= (sel_buffer_read==0);//cambio de buffer 0 o 1
          c=-j;
          //j=0;
      }
      
      if (print){
        char datoH=( char)(in[j]>>8);
        char datoL= ( unsigned char)(in[j]); 
        SCI_send_uchar(datoH);
        SCI_send_uchar(datoL);
      }
}
     
pread_buffer=j+c;

}