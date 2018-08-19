## svn
svn copy http://10.76.1.125:8080/svn/muffler-sim-web/trunk http://10.76.1.125:8080/svn/muffler-sim-web/branches/dev4gy -m 'make branches gy'

## code

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

## prj env anth
sudo chgrp -R www-data .
sudo chmod -R g+w .
export QuadrangulateRootPath="/home/gy/svnproject/Quadrangulate"


## edit
/etc/apache2/envvars
```bash
export MufflerSimRootPath="/home/gy/svnproject/muffler-sim-web/branches/dev4gy/muffler_sim/"
export QuadrangulateRootPath="/home/gy/svnproject/Quadrangulate"
export QuadrangulateServerPath="10.76.1.105:8001"
```

cd /etc/apache2/sites-available/siteall.conf
```conf
<VirtualHost *:8000>
    WSGIDaemonProcess project1 processes=1 threads=15 python-path=/home/gy/svnproject/muffler-sim-web/branches/dev4gy/muffler_sim/muffler_sim
    
    WSGIDaemonProcess project2 processes=1 threads=15 python-path=/home/gy/svnproject/Quadrangulate/Quadrangulate

    <Location "/muffler">
        WSGIProcessGroup project1
    </Location>
    Alias /muffler/static  /home/gy/svnproject/muffler-sim-web/branches/dev4gy/muffler_sim/static/    
    <Directory /home/gy/svnproject/muffler-sim-web/branches/dev4gy/muffler_sim/static>
        Require all granted
    </Directory>

    WSGIScriptAlias /muffler  /home/gy/svnproject/muffler-sim-web/branches/dev4gy/muffler_sim/muffler_sim/wsgi.py process-group=project1
    WSGIPassAuthorization on

    <Directory /home/gy/svnproject/muffler-sim-web/branches/dev4gy/muffler_sim/muffler_sim>
        <Files wsgi.py>
            Require all granted
        </Files>
        AllowOverride none
        Require all granted
    </Directory>



 
    <Location "/Quad">
        WSGIProcessGroup project2
    </Location>

    Alias /Quad/static  /home/gy/svnproject/Quadrangulate/static
    <Directory /home/gy/svnproject/Quadrangulate/static>
        Require all granted
    </Directory>
    Alias /Quad/media/ /home/gy/svnproject/Quadrangulate/media/
    <Directory /home/gy/svnproject/Quadrangulate/media>
        Require all granted
    </Directory>

    WSGIScriptAlias /Quad  /home/gy/svnproject/Quadrangulate/Quadrangulate/wsgi.py process-group=project2

    WSGIPassAuthorization on

    <Directory /home/gy/svnproject/Quadrangulate/Quadrangulate>
             <Files wsgi.py>
                   Require all granted
             </Files>
            AllowOverride none
            Require all granted
    </Directory>
</VirtualHost>
```
sudo a2ensite siteall


index.html:
```html
{% load staticfiles %}
  <link rel="stylesheet" href="{%static '/css/bootstrap.css'%}">
  <link rel="stylesheet" href="{%static '/css/highcharts.css'%}">
  <link rel="stylesheet" href="{%static '/css/bootstrap-slider.css'%}">
  <script src="{%static '/js/jquery-3.3.1.min.js'%}"></script>  

        //引用 '/Quad/media/'
        INPUT_DRAW_OBJECT.model = '/' + {{MEDIA_URL}} + '/' + INPUT_DRAW_OBJECT.model_name + '/input/' + INPUT_DRAW_OBJECT.model_name + '.obj';

```
calate.js

setting.py
```python

TEMPLATES = [
    {
        'BACKEND': 'django.template.backends.django.DjangoTemplates',
        'DIRS': [],
        'APP_DIRS': True,
        'OPTIONS': {
            'context_processors': [
                'django.template.context_processors.debug',
                'django.template.context_processors.request',
                'django.contrib.auth.context_processors.auth',
                'django.contrib.messages.context_processors.messages',
                # use '/' + {{MEDIA_URL}} + '/' in template
                'django.template.context_processors.media',
            ],
        },
    },
]

STATIC_URL = '/Quad/static/'
STATICFILES_DIRS = (
    os.path.join(BASE_DIR, 'static/'),
)

MEDIA_URL = '/Quad/media/'
MEDIA_ROOT = os.path.join(BASE_DIR, 'media/')

ALLOWED_HOSTS = ['*']
```


sudo service apache2 reload
sudo service apache2 restart





