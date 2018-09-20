## compile on linux
```c
g++ -o g711_decoder -I. wav_header.cc wav_file.cc g711.c g711_interface.c g711_decoder.cc
```


## Run the executable
```c
./g711_decoder 160 u input.g711u output.wav
```
