#/bin/bash
function pause(){
   read -p "$*"
}

# #compile:
# gcc wavetrans.c fft.c wave.c -o wavetrans -lm
make clean
make wavfilter

./wavfilter -t Transmissionloss.txt -i wav/simple440add1000.wav -o wav/simple400add1000-tqualized.wav

exit 0
#---------------------------------------------------------------------------------------------------

#test simple440merge1000:
echo "
#Fi       Ai      Qi
31        1.0     1.0
62        1.0     1.0
125       1.0     1.0
250       1.0     1.0
500       1.0     1.0
1000      1.0     1.0
2000      0.01     1.0
4000      1.0     1.0
8000      1.0     1.0
150000    1.0     1.0
" > equalizerall.txt
./wavetrans -e equalizerall.txt -i wav/simple440merge1000.wav -o wav/simple440merge1000-2000-equalized.wav


echo "
#Fi       Ai      Qi
31        1.0     1.0
62        1.0     1.0
125       1.0     1.0
250       1.0     1.0
500       1.0     1.0
1000      0.01     1.0
2000      1.0     1.0
4000      1.0     1.0
8000      1.0     1.0
150000    1.0     1.0
" > equalizerall.txt
./wavetrans -e equalizerall.txt -i wav/simple440merge1000.wav -o wav/simple440merge1000-1000-equalized.wav


echo "
#Fi       Ai      Qi
31        1.0     1.0
62        1.0     1.0
125       1.0     1.0
250       1.0     1.0
500       0.01     1.0
1000      1.0     1.0
2000      1.0     1.0
4000      1.0     1.0
8000      1.0     1.0
150000    1.0     1.0
" > equalizerall.txt
./wavetrans -e equalizerall.txt -i wav/simple440merge1000.wav -o wav/simple440merge1000-500-equalized.wav



#test simple440add1000:
echo "
#Fi       Ai      Qi
31        1.0     1.0
62        1.0     1.0
125       1.0     1.0
250       1.0     1.0
500       1.0     1.0
1000      1.0     1.0
2000      0.01     1.0
4000      1.0     1.0
8000      1.0     1.0
150000    1.0     1.0
" > equalizerall.txt
./wavetrans -e equalizerall.txt -i wav/simple440add1000.wav -o wav/simple440add1000-2000-equalized.wav


echo "
#Fi       Ai      Qi
31        1.0     1.0
62        1.0     1.0
125       1.0     1.0
250       1.0     1.0
500       1.0     1.0
1000      0.01     1.0
2000      1.0     1.0
4000      1.0     1.0
8000      1.0     1.0
150000    1.0     1.0
" > equalizerall.txt
./wavetrans -e equalizerall.txt -i wav/simple440add1000.wav -o wav/simple440add1000-1000-equalized.wav


echo "
#Fi       Ai      Qi
31        1.0     1.0
62        1.0     1.0
125       1.0     1.0
250       1.0     1.0
500       0.01     1.0
1000      1.0     1.0
2000      1.0     1.0
4000      1.0     1.0
8000      1.0     1.0
150000    1.0     1.0
" > equalizerall.txt
./wavetrans -e equalizerall.txt -i wav/simple440add1000.wav -o wav/simple440add1000-500-equalized.wav


exit 0
# --------------------------------------------------------------------------------------------------------------------------
echo "
#Fi       Ai      Qi
440    0.01     1
5000    1     1
" > equalizer4test.txt
./wavetrans -e equalizer4test.txt -i wav/simple.wav -o wav/simple-440-0.01-1-equalized.wav

echo "
#Fi       Ai      Qi
440    1    1
5000    0.01     1
" > equalizer4test.txt
./wavetrans -e equalizer4test.txt -i wav/simple.wav -o wav/simple-5000-0.01-equalized.wav



echo "
#Fi       Ai      Qi
500    0.5     1
" > equalizer4test.txt
./wavetrans -e equalizer4test.txt -i wav/simple.wav -o wav/simple-500-0.5-1-equalized.wav

echo "
#Fi       Ai      Qi
500    1.0     1.0
" > equalizer4test.txt
./wavetrans -e equalizer4test.txt -i wav/simple.wav -o wav/simple-500-1.0-1-equalized.wav

echo "
#Fi       Ai      Qi
500    0.01     1
" > equalizer4test.txt
./wavetrans -e equalizer4test.txt -i wav/simple.wav -o wav/simple-500-0.01-1-equalized.wav

echo "
#Fi       Ai      Qi
500    1.0     1.0
" > equalizer4test.txt
./wavetrans -e equalizer4test.txt -i wav/simple.wav -o wav/simple-500-1.0-1-equalized.wav

echo "
#Fi       Ai      Qi
500    2.0     1
" > equalizer4test.txt
./wavetrans -e equalizer4test.txt -i wav/simple.wav -o wav/simple-500-2.0-1-equalized.wav

#pause
exit 0
