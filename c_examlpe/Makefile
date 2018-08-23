
.PHONY: all clean

Include := -I "E:\Program Files\juce-5.3.2-windows\JUCE\modules" -I "E:\MinGW\include" -I "E:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib" -I "E:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\atlmfc\lib" -I "C:\Program Files (x86)\Windows Kits\10\lib\10.0.16299.0\ucrt\x86" -I "C:\Program Files (x86)\Windows Kits\10\lib\10.0.16299.0\um\x86" -I "C:\Program Files (x86)\Windows Kits\NETFXSDK\4.6\Lib\um\x86"

main:
	gcc main.c -o runme $(Include)


all:waveform fftexample wavetrans

clean:
	rm -rf *.o *~ *.exe 