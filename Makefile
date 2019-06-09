all:
	g++ -o g711_decoder -I. wav_header.cc wav_file.cc g711.c g711_interface.c g711_decoder.cc

enc:
	g++ -o g711_encoder -I. wav_header.cc wav_file.cc g711.c g711_interface.c g711_encoder.cc

clean:
	rm -rf *.o g711.wav 2.pcm
