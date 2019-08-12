bash shell计算时间差
2017年10月25日 16:50:17 hai0808 阅读数：892更多
个人分类： bash
function getTiming() {  
    start=$1
    end=$2
    start_s=$(echo $start | cut -d '.' -f 1)  
    start_ns=$(echo $start | cut -d '.' -f 2)  
    end_s=$(echo $end | cut -d '.' -f 1)  
    end_ns=$(echo $end | cut -d '.' -f 2)  
    time=$(( ( 10#$end_s - 10#$start_s ) * 1000 + ( 10#$end_ns / 1000000 - 10#$start_ns / 1000000 ) ))  
    echo "$time ms"
}

start=$(date +%s.%N)
end=$(date +%s.%N)
runtime=$(getTiming $start $end)
echo "runtime: "$runtime 


### 1. 按子串分割截取
 
${varible#*string}               从左往右，删除最短的一个以string结尾的子串，即截取第一个string子串之后的字符串
${varible##*string}             从左往右，删除最长的一个以string结尾的子串，即截取最后一个string子串之后的字符串
${varible%string*}              从右往左，删除最短的一个以string开头的子串，即截取最后一个string子串之前的字符串
${varible%%string*}          从右往左，删除最长的一个以string开头的子串，即截取第一个string子串之前的字符串

---------------------

本文来自 finewings 的CSDN 博客 ，全文地址请点击：https://blog.csdn.net/finewings/article/details/5718133?utm_source=copy 

#一行加后缀：把文件面 改成“string”，字符串，用于程序输入
for i in $(ls);do echo \"$i\",;done > out.txt