
## svn
**project**
http://10.76.1.125:8080/svn/QuadWebApp
http://10.76.1.125:8080/svn/muffler-sim-web
svn checkout http://10.76.1.125:8080/svn/geo_sim_sdk

**creat br**
svn copy http://10.76.1.125:8080/svn/muffler-sim-web/trunk http://10.76.1.125:8080/svn/muffler-sim-web/branches/dev4gy -m 'make branches gy'

**merge to trunk**
```
cd ~/svnproject/muffler-sim-web/trunk
svn merge http://10.76.1.125:8080/svn/muffler-sim-web/branches/dev4gy

```


### proxy

