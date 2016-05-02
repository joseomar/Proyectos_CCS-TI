/*
 * main.c
 */

/*Celt*/
#include "config.h"

#include "celt.h"
#include "arch.h"
#include "celt_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*fatfs*/
#include "diskio.h"
#include "ff.h"
#include "ffconf.h"
#include "integer.h"
#include <stdio.h>


/*stellaris*/
/*
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
*/
#include "inc/hw_types.h"
/*
#include "driverlib/ssi.h"
#include "driverlib/gpio.h"
*/
#include "driverlib/sysctl.h"

#define MAX_PACKET 1275



FATFS Fatfs;		/* File system object */
FIL Fil;			/* File object */
UINT bw;





int main(void)
{
	FRESULT rc;				/* Result code */

		SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
				SYSCTL_XTAL_16MHZ);


	int err;
	//char *inFile, *outFile;
	//  FILE *fin, *fout;
	CELTHeader header;
	CELTMode *mode = NULL;
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
	short print = 0;
#if !(defined (FIXED_POINT) && defined(STATIC_MODES))

	double rmsd = 0;
#endif

	int count = 0;
	celt_int32 skip;
	celt_int16 *in_in, *out;

	rate = 31250;
	frame_size = 64;
	channels = 1;

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

	celt_header_init(&header, mode, channels);
	header.nb_channels = channels;

	enc = celt_encoder_create(mode, channels, &err);
	if (err != 0)
	{
		//fprintf(stderr, "Failed to create the encoder: %s\n", celt_strerror(err));
		return 1;
	}

	header.bytes_per_packet = bytes_per_packet;

	celt_header_to_packet(&header, packet, 60);
	//envio cabecera celt tmaño 60 bytes
	//SCI_send_header(packet,60);

	in_in = (celt_int16*) malloc(frame_size * channels * sizeof(celt_int16));
	out = (celt_int16*) malloc(frame_size * channels * sizeof(celt_int16));
	if (in_in == NULL)
	{
		//fprintf(stderr, "failed to create a mode\n");
		return 1;
	}

	unsigned char nbByte = 8;

	unsigned char u8Error = 111;

	int ret = -1;


	rc = f_mount(0, &Fatfs);
	rc = f_open(&Fil, "HELLO.TXT", FA_WRITE | FA_CREATE_ALWAYS);	//abre o crea un archivo


	for (i=0; i < 25; i++)
	{
		//err = fread(in, sizeof(short), frame_size*channels, fin);

		rc = f_write(&Fil, "Hello world2222!\r\n", 18, &bw);
		//readFrame(in_in, frame_size * channels, print);

		/*if (feof(fin))
		 break;
		 */
		len = celt_encode(enc, in_in, NULL, data, bytes_per_packet); //frame_size
		if (len <= 0)
			break;

		for (i = 0; i < frame_size * channels; i++)
			out[i] = in_in[i];
#if !(defined (FIXED_POINT) && defined(STATIC_MODES))
		for (i = 0; i < frame_size * channels; i++)
		{
			rmsd += (in_in[i] - out[i]) * 1.0 * (in_in[i] - out[i]);
			//out[i] -= in[i];
		}
#endif
		count++;

		//   fwrite(out+skip, sizeof(short), (frame_size-skip)*channels, fout);
		skip = 0;
	}
	celt_encoder_destroy(enc);
	celt_mode_destroy(mode);
	free(in_in);
	free(out);

	for (;;)
	{
	}

}


