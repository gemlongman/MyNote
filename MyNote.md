# MyNote
own note

## svn

svn checkout http://10.76.1.125:8080/svn/szylib/trunk/szylib

**creat br**
svn copy http://10.76.1.125:8080/svn/muffler-sim-web/trunk http://10.76.1.125:8080/svn/muffler-sim-web/branches/dev4gy -m 'make branches gy'

svn copy http://10.76.1.125:8080/svn/scan_prediction/trunk http://10.76.1.125:8080/svn/scan_prediction/branches/gydev -m 'make branches gy'

**merge to trunk**
```
cd ~/svnproject/muffler-sim-web/trunk
svn merge http://10.76.1.125:8080/svn/muffler-sim-web/branches/dev4gy

```

## env

sudo apt-get update
sudo apt-get upgrade
sudo apt-get install libf2c2-dev libeigen3-dev libalglib-dev liblbfgs-dev petsc-dev libboost-all-dev
sudo apt-get install cmake g++ libffi-dev  libmount-dev libpcre++-dev
sudo apt-get install libxml2-dev libpcl-dev libvtk6-dev libglfw3-dev libglew-dev libglib2.0-dev
sudo apt-get install build-essential libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev libproj-dev
vtk
	创建build文件夹进入build文件夹进行配置，进行配置ccmake .. 
	5.运行步骤4的代码会跳出一下配置：
	如果没有编译过的会提示没有缓存，可以按c进行配置，之后就会出现这个界面，用上下建选择条目，BUILD_SHARE_LIBS 选择ON ，VTK_Group_Qt选择 On就可以了，其它的可以不做更改，再次按c进行配置确认，按g保存配置。
	make
	sudo make install

geo_sim_sdk/trunk script $ sudo ./install.sh 
hjlib/build $ sudo make install
sudo ./szy-lib-install.sh 

package ： glut 

eigen-eigen
spectra-0.6.2 copy include usr/include/spectra


## pythoncode

if "QuadrangulateRootPath" in os.environ and os.getenv('QuadrangulateRootPath') != '' :
    python_home = os.getenv('QuadrangulateRootPath') + '/env'
else :
    python_home = '/home/zcy/.virtualenvs/newpy'

## python

source env/bin/activate 
python3 manage.py runserver 0.0.0.0:8001

## apache

47.254.42.173:8000

sudo a2ensite sitequad
sudo a2ensite sitemuffler
sudo service apache2 reload
sudo service apache2 restart
tail -n 100 /var/log/apache2/error.log

mutil prj:
https://blog.csdn.net/LCQ1005428477/article/details/81103463


## html load mkdown
https://github.com/markedjs/marked

## prj env anth
sudo chgrp -R www-data .
sudo chmod -R g+w .
export QuadrangulateRootPath="/home/gy/svnproject/Quadrangulate"

## gcc math.h
gcc wavetrans.c fft.c wave.c -o wavetrans -lm

https://blog.csdn.net/aiwoziji13/article/details/7330333


## octave
```m
d=textread("log.log", "%f");
d=reshape(d, 3, size(d, 1)/3);
plot(d(1,:), d(3,:));

https://octave.org/doc/v4.0.0/Audio-Data-Processing.html
%Octave-Programming 
%Denny Jack Muttathil
% Apply STFT on a small Wav-File
% My audiofile is exactly 10,8525 seconds long.
% If you want to check the length of the wavfile, use the following code 
% in the command window:
% [y,fs]=wavread('Name of the wavfile');
% Total Time=lenght(y)/fs 
% Now the actual code: 
%------------------------------------------------------------------------
[y,fs]=wavread('simple440.wav');
% y defines the length of my vector
% fs defines the number of samples per second in the vector y
% wavread, just as the names say, reads my wavfile
% 'Test' is the name of my wavfile 
t=linspace(0,length(y)/fs,length(y));
% Generating a time vector with linspace
% 0 is my start time
% length(y)/fs is my end/stop time
% length(y) is the number of samples in y 
plot(t,y);
% Shows my signal in the time domain

% Now a little bit explanation
% Since my wavfile is about 11 seconds long, i want to take out
% small parts of my wav file, which are 50ms long. On these small
% segments, i want to apply the hanning function. These function uses 
% a window, which performs a stft on each segments. The window is half  
%the size of my segment.

fftlen = 4096; 

segl = 0.05; 
% Length of my segment on which my hanning function is applied
w=segl/2;
% Length of my window size
si=1; %Start index
ei=10.8526; %End index

for m= 1:0.025:(10.8526/w)-1
% Using a for loop to see, what exactly happens
y_a = y(si:ei); 
y_a= y_a.*hann(segl);
Ya=fft(y_a, fftlen);

si=si+w; % Updates my start index 
ei=ei+w; % Updates my end index 

f=0:1:fftlen-1;
f1=fs/(fftlen-1);

figure; 
plot(f1, 20*log10(abs(Ya)));

```


##js
```js
function sleep(milliseconds) {
  var start = new Date().getTime();
  for (var i = 0; i < 1e7; i++) {
    if ((new Date().getTime() - start) > milliseconds){
      break;
    }
  }
}

async function main(){
	awite sleep(3000);
}

///
function foo(){
$.post(show_result_url, JSON.stringify(post_data), show_result_callback);
}

foo();
intervalVariable = setInterval(foo, 5000);
//
clearInterval(intervalVariable);
///
```

jQuery BlockUI 实现锁屏
http://bookshadow.com/weblog/2014/09/26/jquery-blockui-js-introduction/

##linux u disk system

sudo dd if=windows_10_ultimate_x64_2018.iso of=/dev/sdb


### latex 
latexmk -xelatex book_DGP.tex

### 全文件替换
```
grep -rl "Krizhevsky2012imagenet" *|xargs -i sed -i 's/Krizhevsky2012imagenet/krizhevsky2012imagenet/g' "{}"

```

{
  "server":"172.96.206.101",
  "server_port":12306,
  "local_port":1080,
  "password":"gaohongyu",
  "timeout":600,
  "method":"aes-256-cfb"
}

sudo ssserver -c /etc/shadowsocks.json -d start

sudo ssserver -d stop

sudo vi /etc/rc.local
/usr/bin/python /usr/local/bin/ssserver -c /etc/shadowsocks.json -d start


```

shutdown -h +60
