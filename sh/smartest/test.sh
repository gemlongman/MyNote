#!/bin/bash
# SMART TEST SCRIPT
# by WXT
# 20161009

echo "# Message: Installing smartmontools ..."
sudo apt-get install -y smartmontools > /dev/null && \
echo "# Message: Installed smartmontools."
DEV=`ls /dev/sd?` && \

function checkmessage(){
  checkfile=$1
  errors=0
  warning=0
  infos=0
  # warning=`grep -c "Pre-fail" $checkfile`
  # infos=`grep -c "Old_age" $checkfile`
  # echo "# Warnings: $warning"
  # echo "# infos: $infos"

  isData=false
  # cat $checkfile | while read LINE   :while循环内改变无法传递到循环外
  while read LINE  
  do
    if [ $isData = "true"  ]
      then
      if [ -n "$LINE" ]
        then
        ATTRIBUTE=($LINE)
       
        if [ "${ATTRIBUTE[6]}" = "Pre-fail" ] 
          then
          warning=$(( $warning + 1 ))
        elif [ "${ATTRIBUTE[6]}" = "Old_age" ] 
          then
          infos=$(( $infos + 1 ))
        else
          infos=$(( $infos + 1 ))
        fi
      else
        break
      fi

    elif [ "$LINE" = "ID# ATTRIBUTE_NAME          FLAG     VALUE WORST THRESH TYPE      UPDATED  WHEN_FAILED RAW_VALUE" ]
      then 
      isData=true
      #echo "start"
    fi
  done < $checkfile

  #grep "Error Count" $checkfile
  errors=`grep "Error Count" $checkfile | tr -cd "[0-9]"`
  echo "# Errors: $errors"
  echo "# Warnings: $warning"
  echo "# Infos: $infos"
  #grep "SMART overall-health self-assessment test result:" $checkfile '
  result=` grep "SMART overall-health self-assessment test result:" $checkfile |  grep -Eo '(FAILED)|(PASSED)' `
  echo "# result(PASSED/FAILED) : $result "
}


function runtest(){
  for SD in $DEV
  do
    echo
    echo "# Message: Detecting $SD ..."
    sudo smartctl -s on $SD > /dev/null && \
    NUMOLD=`sudo smartctl -l selftest $SD | grep -c 'Completed without error'` && \
    echo "# Message: Testing $SD (about 2 minutes) ..."
    sudo smartctl -t short $SD > /dev/null && \
    while :
    do
      sleep 1m
      ST=`sudo smartctl -l selftest $SD `  && \
      NUMNEW=`echo "$ST" | grep -c 'Completed without error'` 
      echo "$ST" | tail -n 1
      #echo "-$NUMNEW -gt $NUMOLD "
      if [ "$NUMOLD" = "" ] || [ "$NUMNEW" = "" ] || [ $NUMNEW -gt $NUMOLD ]; then break; fi
    done
    if [ ! -d "./smartest" ]; then mkdir ./smartest; fi
    sudo smartctl -a $SD > ./smartest/${SD:5} && \
    sudo smartctl -l error $SD >> ./smartest/${SD:5} && \
    echo "# Message: $SD tested."
    echo "# Message: Wrote file ./smartest/${SD:5} ."
    
    checkmessage ./smartest/${SD:5}

  done
}

runtest