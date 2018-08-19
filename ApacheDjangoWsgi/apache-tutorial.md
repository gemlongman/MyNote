# Suppose django project path is /path/to/project/
# and the tree structure is
# /path/to/project/
# -- project/
# ----- wsgi.py
# -- static/
# -- env/ (created by `virtualenv` command)
# -- libs/
# ----- lib1/
# -- app1/
# -- app2/

# install apache2
sudo apt-get install apache2

# install wsgi for python 3
sudo apt-get install libapache2-mod-wsgi-py3
# (the author of [2] says that we should activate virtual env
# and use `pip install mod_wsgi` to get the right version,
# but i failed)

# check apache version
apachectl -v

# add site config
sudo emacs /etc/apache2/sites-available/sitename.conf
'''
<VirtualHost *:8000>
    ServerName my.server.name
    ServerAdmin myemail@address
  
    Alias /static/ /path/to/project/static/
    
    <Directory /path/to/project/static>
        Require all granted
    </Directory>
  
    WSGIScriptAlias / /path/to/project/project/wsgi.py
  
    <Directory /path/to/project/project>
    <Files wsgi.py>
        Require all granted
    </Files>
    </Directory>
</VirtualHost>
'''
# ([2] says for virtual env created by `virtualenv` command,
# we should activate it in wsgi.py;
# while for virtual env created by `pyvenv` command,
# we should set `WSGIDaemonProcess` and `WSGIProcessGroup` in sitename.conf,
# i don't know)

# add port listener
sudo emacs /etc/apache2/ports.conf
'''
Listen 8000
'''

# modify wsgi.py
'''
# activate virtualenv
python_home = '/path/to/project/env'
activate_this = python_home + '/bin/activate_this.py'
with open(activate_this) as af:
  exec(af.read(), {'__file__':activate_this})

# add project dir & libs dir to path
PROJECT_DIR = dirname(dirname(abspath(__file__)))
import sys
sys.path.insert(0, PROJECT_DIR)
sys.path.insert(0, os.path.join(PROJECT_DIR, 'libs/lib1'))
'''

# change user privilege
# (for these directories that can be modified by user)
sudo chgrp -R www-data /path/to/project/.../some/dir/that/can/be/modified/by/user
sudo chmod -R g+w /path/to/project/.../some/dir/that/can/be/modified/by/user

# activate site
sudo a2ensite sitename

# reload apache service
sudo service apache2 reload

# if there are errors, check log file
tail -n 100 /var/log/apache2/error.log

# Reference:
# [1] https://code.ziqiangxuetang.com/django/django-deploy.html
# [2] https://modwsgi.readthedocs.io/en/develop/user-guides/virtual-environments.html
