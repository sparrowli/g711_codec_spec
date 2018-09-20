#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "g711_interface.h"
#include "wav_file.h"

/* Runtime statistics */
#include <time.h>
#define CLOCKS_PER_SEC_G711 1000

/* function for reading audio data from PCM file */
bool readframe(int16_t* data, FILE* inp, size_t length) {
  size_t rlen = fread(data, sizeof(int16_t), length, inp);
  if (rlen >= length)
    return false;
  memset(data + rlen, 0, (length - rlen) * sizeof(int16_t));
  return true;
}

bool ReadEncodedFrame(uint8_t* data, FILE* inp, size_t length) {
  size_t rlen = fread(data, sizeof(uint8_t), length, inp);
  if (rlen >= length)
    return false;
  memset(data + rlen, 0, (length - rlen) * sizeof(uint8_t));
  return true;
}

int main(int argc, char* argv[]) {
  char inname[256], outname[256], bitname[256];
  // FILE* inp;
  FILE* outp;
  FILE* bitp = NULL;
	using namespace mixerengine;
	// WavWriter* wav_ = NULL;
	WavReader* wav_reader_ = NULL;
  int framecnt;
  bool endfile;

  size_t framelength = 80;

  /* Runtime statistics */
  double starttime;
  double runtime;
  double length_file;

	size_t read_len = 0;
  size_t stream_len = 0;
  uint8_t encoded[480];
  int16_t shortdata[480];
  int16_t decoded[480];
  uint8_t streamdata[1000];
  int16_t speechType[1];
  char law[2];
  char versionNumber[40];

  if ((argc != 5) && (argc != 6)) {
    printf("\n\nWrong number of arguments or flag values.\n\n");

    printf("\n");
    printf("\nG.711 test application\n\n");
    printf("Usage:\n\n");
    printf("./g711_encoder framelength law infile outfile \n\n");
    printf("framelength: Framelength in samples.\n");
    printf("law        : Coding law, A och u.\n");
    printf("infile     : Normal speech input file\n");
    printf("outfile    : Speech output file\n\n");
    printf("outbits    : Output bitstream file [optional]\n\n");
    exit(0);

  }

  /* Get version and print */
  WebRtcG711_Version(versionNumber, 40);

  printf("-----------------------------------\n");
  printf("G.711 version: %s\n\n", versionNumber);

  /* Get frame length */
  int framelength_int = atoi(argv[1]);
  if (framelength_int < 0) {
      printf("  G.711: Invalid framelength %d.\n", framelength_int);
      exit(1);
  }
  framelength = static_cast<size_t>(framelength_int);

  /* Get compression law */
  strcpy(law, argv[2]);

  /* Get Input and Output files */
  sscanf(argv[3], "%s", inname);
  sscanf(argv[4], "%s", outname);

	wav_reader_ = new WavReader(inname);
	//wav_ = new WavWriter(outname, 8000, 1);

  if (argc == 6) {
    sscanf(argv[5], "%s", bitname);
    if ((bitp = fopen(bitname, "wb")) == NULL) {
      printf("  G.711: Cannot read file %s.\n", bitname);
      exit(1);
    }
  }

	/*
  if ((inp = fopen(inname, "rb")) == NULL) {
    printf("  G.711: Cannot read file %s.\n", inname);
    exit(1);
  }
	*/
  if ((outp = fopen(outname, "wb")) == NULL) {
    printf("  G.711: Cannot write file %s.\n", outname);
    exit(1);
  }
  printf("\nInput:  %s\nOutput: %s\n", inname, outname);
  if (argc == 6) {
    printf("\nBitfile:  %s\n", bitname);
  }

  starttime = clock() / (double) CLOCKS_PER_SEC_G711; /* Runtime statistics */

  /* Initialize encoder and decoder */
  framecnt = 0;
  endfile = false;
  read_len = framelength;
  printf("write error read_len = %d|frame_length=%d\n", read_len, framelength);
  while (!endfile) {
    framecnt++;
    /* Read speech block */
    // endfile = readframe(shortdata, inp, framelength);
    read_len = wav_reader_->ReadSamples(framelength, shortdata);

		if (read_len < framelength)
			endfile = true;
    if (!strcmp(law, "u")) {
      WebRtcG711_EncodeU(shortdata, read_len, encoded);
    } else {
      printf("Wrong law mode\n");
      exit(1);
    }
    /* Write coded speech to file */
    if (fwrite(encoded, sizeof(uint8_t), read_len, outp) != read_len) {
      printf("write error read_len = %d|frame_length=%d\n", read_len, framelength);
      return -1;
    }

		//wav_->WriteSamples(decoded, framelength);

	}
  printf("error count = %d\n", framecnt);
  runtime = (double)(clock() / (double) CLOCKS_PER_SEC_G711 - starttime);
  length_file = ((double) framecnt * (double) framelength / 8000);
  printf("\n\nLength of speech file: %.1f s\n", length_file);
  printf("Time to run G.711:      %.2f s (%.2f %% of realtime)\n\n",
         runtime,
         (100 * runtime / length_file));
  printf("---------------------END----------------------\n");

	// delete wav_;
	// wav_ = NULL;

  // fclose(inp);
  fclose(outp);

	return 0;
}
