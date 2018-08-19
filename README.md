# MyNote
own note

## svn
**creat br**
svn copy http://10.76.1.125:8080/svn/muffler-sim-web/trunk http://10.76.1.125:8080/svn/muffler-sim-web/branches/dev4gy -m 'make branches gy'

**merge to trunk**
```
cd ~/svnproject/muffler-sim-web/trunk
svn merge http://10.76.1.125:8080/svn/muffler-sim-web/branches/dev4gy

```

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