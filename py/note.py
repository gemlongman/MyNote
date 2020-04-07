# 后台运行
python -m visdom.server -p 8097 &>/dev/null &
后台执行但是关闭窗口ssh链接会断开

setsid python 。。。。。 > ~/gygit/test/output.txt 2>&1
后台学习

python 多进程

处理文件，进程效果更好，占了8个核，线程貌似还在用一个核~

https://blog.csdn.net/zhuzuwei/article/details/80907079

```

def long_time_task(name):
  print('Run task %s (%s)...' % (name, os.getpid()))
  start = time.time()
  time.sleep(random.random() * 3)
  end = time.time()
  print('Task %s runs %0.2f seconds.' % (name, (end - start)))
 
if __name__=='__main__':
  p = Pool(4)  # 创建4个进程
  for i in range(5):
    p.apply_async(long_time_task, args=(i,))
  print('Waiting for all subprocesses done...')
  p.close()
  p.join()
  print('All subprocesses done.')
```
多线程

https://blog.csdn.net/houyanhua1/article/details/78229034
