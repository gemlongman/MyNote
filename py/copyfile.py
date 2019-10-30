"""
随机选图片并复制到另一个文件夹中
copy data
from face,sketch to trainA,trainB
before copy will remove target
suchas
fromDirs=['datasets/caf/face' ,'datasets/caf/sketch1' ,'datasets/caf/sketch2' ]
toDirs=['datasets/s2f/trainA' ,'datasets/s2f/trainB' ,'datasets/caf/facetest3' ]

"""

import  os, random, shutil


fromDirs = ['/home/gem/gygit/test/caf_cycle/sketch2/trainA' , '/home/gem/gygit/test/caf_cycle/sketch2/trainB']

toDirs = ['/home/gem/gygit/test/caf_cycle/sketch2/testA' , '/home/gem/gygit/test/caf_cycle/sketch2/testB']

filesNum = 50

isDeleteOld = 0

#maybe useful in case copy 20% file for val or test
def copyFile(fromDir,toDir,num,isDeleteOld=0):
        pathDir = os.listdir(fromDir)    #取图片的原始路径
        
        if num < 0:
            return;
        if num < 1:
            fileNums = len(pathDir)
            rate=num   #自定义抽取图片的比例，比方说100张抽10张，那就是0.1
            pickNums=int(fileNums*rate) #按照rate比例从文件夹中取一定数量图片
        else:
            pickNums = num 
        sample = random.sample(pathDir, pickNums)  #随机选取picknumber数量的样本图片
        #print (sample)
       
        deleteTargetOld=0      
        if not os.path.isdir(toDir):
            os.makedirs(toDir)
        elif deleteTargetOld:
            shutil.rmtree(toDir)
            os.makedirs(toDir)
        
        print('copyFile from %s to %s within %d' % (fromDir,toDir,pickNums))
        for name in sample:
                shutil.copyfile(os.path.join(fromDir, name), os.path.join(toDir, name) )
                #shutil.move(os.path.join(fromDir, name), os.path.join(toDir, name) )
        return

def searchfile(onedir,name):
    pathDir = os.listdir(onedir)
    #remove suffix
    (name, extension) = os.path.splitext(name)
    #print('name %s - extension %s'%(name,extension))
    for afile in pathDir:
        if name in afile:
            #print("find!!!!name %s - file %s - onedir%s"%(name,afile,onedir))
            return os.path.join(onedir, afile)
    return -1

def multiDirSameFilesCopy(fromDirS,toDirS,num,deleteOld=1):
    #maybe it should support coping between one and multi 
    if len(fromDirS)>len(toDirS):
        print('it should be same dirs of source and target')
        return

    #make sure mini file number    
    minFiles=0
    minDirPath=''
    #pathDirS=[]useless
    for onedir in fromDirS:
        pathDir = os.listdir(onedir)
        #pathDirS.append(pathDir)
        fileNums = len(pathDir)
        if 0==minFiles or fileNums <minFiles:
            minFiles = fileNums
            minDirPath = pathDir
    if num > minFiles:
        num = minFiles

    sample = random.sample(minDirPath, num)  #随机选取picknumber数量的样本图片
   
    for i in range(0, len(fromDirS)):
        fromDir = fromDirS[i]
        toDir = toDirS[i]
        if not os.path.isdir(toDir):
            os.makedirs(toDir)
        elif deleteOld:
            shutil.rmtree(toDir)
            os.makedirs(toDir)
        print('copyFile from %s to %s within %d' % (fromDir,toDir,num))
        for name in sample:
            try:
                shutil.copyfile(os.path.join(fromDir, name), os.path.join(toDir, name) )
            except  FileNotFoundError as e:
                #notice!!!
                #it is could be png in face but jpg in others,so it need transfer them to all jpg
                #find same file
                fromfile=searchfile(fromDir,name)
                if -1 == fromfile:
                    print("error",e)
                else:
                    shutil.copyfile(fromfile, os.path.join(toDir, name) )
    return

if __name__ == '__main__':
	#fromDir = "datasets/caf/face"    #源图片文件夹路径
	#toDir = 'datasets/caf/facetest'    #移动到新的文件夹路径
	#copyFile(fromDir,toDir,10)
    multiDirSameFilesCopy(fromDirs,toDirs,filesNum,isDeleteOld)  
