.PHONY: all clean


compiler = gcc
depend = fft/fft.c wave/wave.c
ouput = wavfilter

wavfilter:
	$(compiler) main.c $(depend) -o $(ouput) -lm
	
all: wavfilter

clean:
	rm -rf *.o *.exe *~ wavetrans wavfilter wav/noisy1.dat wav/*-clean.wav wav/*-equalized.wav