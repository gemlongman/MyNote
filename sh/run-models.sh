#!/bin/bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/gy/usr/Linux/64/gcc7/lib
MODEL_FOLD=./models
# /home/gy/share/workfile/shapnet/ShapeNetSemv0
# ./models
BIN=./scan_prediction
ICP_DIR=./shn_icp
SPHERE=./sphere-coarse.binary.obj
ReportFile=./report.txt
LOG_FILE=log.txt
OBJ=$1

function sum(){
    input=($*)
    summary=0.0
    # echo "${input[*]}"
    for i in ${input[*]} ; do
        # echo "i==${i}"
        summary=$(echo "$summary+$i"|bc)
    done
    echo $summary
    # return summary
}


if [ ! -z "$1" ]; then 
    rm -rf $OBJ
    mkdir $OBJ
    LOG_FILE=$OBJ/log.txt
    date 2>&1 | tee  $LOG_FILE
    printf '***** start  to run model: %s\n\n' $OBJ       2>&1 | tee -a $LOG_FILE
    $BIN icp_dir=$ICP_DIR stddev=0 angle_thd=0.4 normal_method=3 weight_method=3 predict_method=9 noise=0 obj=$MODEL_FOLD/$OBJ.norm.binary.obj sphere=$SPHERE itr=50 prefix=$OBJ/$OBJ.  2>&1 | tee -a $LOG_FILE
    printf '***** finish to run model: %s\n\n' $OBJ       2>&1 | tee -a $LOF_FILE
    date 2>&1 | tee  -a $LOG_FILE
    exit
fi


# rm -rf ./Output
echo "" > $ReportFile

for d in $MODEL_FOLD/*.obj ; do
    OBJfile=$d
    # echo "OBJfile:$OBJfile"
    OBJ=$(echo ${OBJfile##*/}) 
    OBJ=$(echo ${OBJ%%.*})
    # echo "file:$OBJ"

    nMethod=3
    wMethod=3
    pMethod=9
    Noise=0.1
    OBJ_dir=Output/$OBJ$nMethod$wMethod$pMethod$Noise
    LOG_FILE=$OBJ_dir/log.txt

    mkdir -p $OBJ_dir

    StartTime=$(date +%s.%N)
    # date 2>&1 | tee  $LOG_FILE
    # printf '***** start  to run model: %s\n\n' $OBJ       2>&1 | tee -a $LOG_FILE
    # #prefix : belong output file
    # $BIN icp_dir=$ICP_DIR stddev=0 normal_method=$nMethod weight_method=3 predict_method=$pMethod noise=$Noise obj=$OBJfile sphere=$SPHERE itr=50 prefix=$OBJ_dir/$OBJ.  2>&1 | tee -a $LOG_FILE
    # printf '***** finish to run model: %s\n\n' $OBJ       2>&1 | tee -a $LOF_FILE
    # date 2>&1 | tee  -a $LOG_FILE
    Endtime=$(date +%s.%N)

    start_s=$(echo $StartTime | cut -d '.' -f 1)  
    start_ns=$(echo $StartTime | cut -d '.' -f 2)  
    end_s=$(echo $Endtime | cut -d '.' -f 1)  
    end_ns=$(echo $Endtime | cut -d '.' -f 2)  
    time=$(( ( 10#$end_s - 10#$start_s ) * 1000 + ( 10#$end_ns / 1000000 - 10#$start_ns / 1000000 ) ))  


    iterations=` grep "Terminate at iteration:" $LOG_FILE`
    iterations=${iterations##*"Terminate at iteration: "}

    coverateRate=` grep "coverate rate" $LOG_FILE |  grep -Eo '[01].?[0-9]*'`
    ARRcoverateRate=($coverateRate)
    # iterations == exper ${#ARRcoverateRate[@]} - 1
    lastcoverateRate=${ARRcoverateRate[${iterations}]}


    predictionCost=` grep "prediction cost:" $LOG_FILE  |  grep -Eo '[0-9]+\.[0-9]*' `
    predictionCost=$(sum "$predictionCost")
    

    printf "$OBJ $nMethod $wMethod $pMethod $Noise \t iterations $iterations \t coverateRate ${lastcoverateRate} \t predictionCost $predictionCost\t $time ms \n" >> $ReportFile
done




